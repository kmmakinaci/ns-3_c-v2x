/*
 * GuiManager.cpp
 *
 *  Created on: Sep 28, 2015
 *      Author: marian
 */

#include "GuiManager.h"
#include "Polygons.h"
#include <unistd.h>

namespace ns3 {
GuiManager *current;

static int handle_keypress(SDL_Event event){
	switch(event.key.keysym.sym){
	case SDLK_e:
		if(current->nodevec.size()>0){
			current->cam.setTarget(current->nodevec[0].x,current->nodevec[0].y,0.0f);
		}
		break;
	case SDLK_LSHIFT:
		break;
	default:
		break;
	}
	return 0;
}



static int handle_event(void *unused){
	SDL_Event event;
	while(1){
		SDL_WaitEvent(&event);
		switch (event.type) {
		case SDL_KEYDOWN:
			handle_keypress(event);
			break;
		case SDL_KEYUP:
			//handle_keyrelease(event);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if(event.button.button==SDL_BUTTON_LEFT){
				current->cam.setLPress(true,event.motion.x,event.motion.y);
			}
			if(event.button.button==SDL_BUTTON_RIGHT){
				current->cam.setRpress(true,event.motion.x,event.motion.y);
			}
			if(event.button.button==SDL_BUTTON_WHEELDOWN){
				current->cam.zoomIn();
			}
			if(event.button.button==SDL_BUTTON_WHEELUP){
				current->cam.zoomOut();
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if(event.button.button==SDL_BUTTON_LEFT){
				current->cam.setLPress(false,event.motion.x,event.motion.y);
			}
			if(event.button.button==SDL_BUTTON_RIGHT){
				current->cam.setRpress(false,event.motion.x,event.motion.y);
			}
			break;
		case SDL_MOUSEMOTION:
			current->cam.getMotion(event.motion.x,event.motion.y);
			break;
		case SDL_VIDEORESIZE:
			current->screen_width = event.resize.w;
			current->screen_height = event.resize.h;
			current->resized=true;
			break;
		case SDL_QUIT:
			exit(0);
		}
	}
	return 0;
}



GuiManager::GuiManager() {
	time=0;
	screen_width = WIDTH;
	screen_height = HEIGHT;
	current = this;
	lock =NULL;
	keeprunning=true;
	resized=false;
	topo = new Topology();
}

GuiManager::~GuiManager() {
	// TODO Auto-generated destructor stub
}

bool GuiManager::GetPause(){
	return pause;
}

void GuiManager::drawCarShape(){
	car_shape=glGenLists(1);
	glNewList(car_shape,GL_COMPILE);
	glBegin(GL_TRIANGLES);

	for (unsigned int i = 0; i < sizeof(car_data)/sizeof(double); i += 6)
	{
		glVertex3f(car_data[i], car_data[i + 1], car_data[i + 2]);
		glNormal3f(car_data[i + 3], car_data[i + 4], car_data[i + 5]);
	}
	glColor3f(0.55f,0.55f,0.55f);
	for (unsigned int i = 0; i < sizeof(car_win_data)/sizeof(double); i += 6)
	{
		glVertex3f(car_win_data[i], car_win_data[i + 1], car_win_data[i + 2]);
		glNormal3f(car_win_data[i + 3], car_win_data[i + 4], car_win_data[i + 5]);
	}
	glEnd();
	glEndList();
}

void GuiManager::setNode(int id, double x, double y,double hdg)
{
	if (SDL_LockMutex(lock) == 0){
		nodevec.push_back(GuiNode(id,x,y,hdg,car_shape));
		SDL_UnlockMutex(lock);
	}
}
void GuiManager::setNodePi(int id, double x, double y,double hdg,int posindex)
{
	if (SDL_LockMutex(lock) == 0){
		nodevec.push_back(GuiNode(id,x,y,hdg,car_shape,posindex));
		SDL_UnlockMutex(lock);
	}
}


void GuiManager::setNodeStatic(int id, double x, double y,double hdg)
{
	if (SDL_LockMutex(lock) == 0){
		nodevec_static.push_back(GuiNode(id,x,y,hdg,car_shape));
		SDL_UnlockMutex(lock);
	}
}

void GuiManager::clearNodes()
{
	if (SDL_LockMutex(lock) == 0){
		nodevec.clear();
		SDL_UnlockMutex(lock);
	}
}
void GuiManager::subView(){
	float aspect = (float)screen_width / (float)screen_height;
	glViewport(screen_width*0.9,0,screen_width*0.1, screen_height);
	glScissor(screen_width*0.9,0,screen_width*0.1, screen_height);
	glEnable(GL_SCISSOR_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120.0, aspect, 0.1, 100000.0);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0, 0, 0,1);
	glEnable(GL_DEPTH_TEST);
}

void GuiManager::SceeneView(){
	float aspect = (float)screen_width / (float)screen_height;
	glViewport(0,0,screen_width*0.9, screen_height);
	glScissor(0,0,screen_width*0.9, screen_height);
	glEnable(GL_SCISSOR_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, aspect, 0.1, 100000.0);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.7f, 0.9f, 0.7f,0.0f);
	glEnable(GL_DEPTH_TEST);
}

void GuiManager::displaysub(void){
	subView();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_PROJECTION ) ;
	glPushMatrix() ;
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW ) ;
	glPushMatrix() ;
	glLoadIdentity() ;
	glDisable( GL_DEPTH_TEST ) ;
	glRasterPos3f( 0,0,.09 ) ; // center of screen. (-1,0) is center left.
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	std::stringstream s;
	s<<Simulator::Now().GetMilliSeconds();
	std::string str = s.str();
	for (unsigned int j = 0; j<str.size(); j++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[j]);
	}
	glEnable( GL_DEPTH_TEST ) ;
	glPopMatrix();
}


void GuiManager::drawSceeneX(){
	SceeneView();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	cam.get();
	topo->draw();
	glutSolidCube(1.0);
	glEnable(GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	for(unsigned int i=0;i<nodevec.size();i++){
		std::map<int,sColor>::iterator it = sc_list.find(nodevec.at(i).id);
		if (it != sc_list.end()){
			nodevec.at(i).draw(it->second.r,it->second.g,it->second.b,it->second.tag);
		}
		else{
			nodevec.at(i).draw();
		}
	}
	for(unsigned int i=0;i<nodevec_static.size();i++){
		nodevec_static.at(i).draw();
	}
	glDisable(GL_LIGHTING);
}


void GuiManager::drawSceene(){
	if(resized){
		SDL_SetVideoMode( screen_width, screen_height, 32, SDL_OPENGL | SDL_RESIZABLE );
		resized=0;
	}
	if (SDL_LockMutex(lock) == 0){
		displaysub();
		drawSceeneX();
		SDL_UnlockMutex(lock);
	}
	else {
		fprintf(stderr, "Couldn't lock mutex\n");
	}

	glDisable(GL_SCISSOR_TEST);
	SDL_GL_SwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int GuiManager::init_SDL(){
	if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
	{
		return 0;
	}
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	// TODO: Add error check to this screen surface init
	SDL_SetVideoMode( screen_width, screen_height, 32, SDL_OPENGL | SDL_RESIZABLE );
	return 1;
}

static int guistart(void *unused){
	int s=0;
	char **d=NULL;
	SDL_Thread *thread;
	glutInit(&s,d);
	current->init_SDL();
	current->drawCarShape();
	current->topo->prepare();
	current->cam.setTarget(current->topo->coordsx[0],current->topo->coordsx[1],0);
	thread = SDL_CreateThread(handle_event, NULL);
	while(current->keeprunning){
		current->drawSceene();
		usleep(70000);
	}
	SDL_WaitThread(thread,NULL);
	return 0;
}

void GuiManager::start(){
	lock = SDL_CreateMutex();
	SDL_CreateThread(guistart, NULL);
}


Topology *GuiManager::getTopology(){
	return topo;
}
void GuiManager::addSpecialColor(int node,std::string tag,int r,int g,int b){
	sColor newcolor;
	newcolor.node = node;
	newcolor.tag = tag;
	newcolor.r = r;
	newcolor.g = g;
	newcolor.b = b;
	if(SDL_LockMutex(lock)==0){
		sc_list.insert(std::pair<int,sColor>(node,newcolor));
		SDL_UnlockMutex(lock);
	}
}

void GuiManager::clearSpecialColor(){
	if(SDL_LockMutex(lock)==0){
		sc_list.clear();
		SDL_UnlockMutex(lock);
	}
}
}


