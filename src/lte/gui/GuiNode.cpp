/*
 * GuiNode.cpp
 *
 *  Created on: Sep 28, 2015
 *      Author: marian
 */

#include "GuiNode.h"


namespace ns3 {
GLfloat PositionLight[]={0.0f,1000.0f,100.0f,1.0f};
GLfloat SpecularLight[] = {0.8f,0.8f,0.8f,1.0f};
GLfloat DiffuseLight[] = {0.6f,0.6f,0.6f,1.0f};
GLfloat AmbientLight_buildings[] = {0.4f,0.4f,0.4f,1.0f};
GLfloat AmbientLight_car[] = {0.1f,0.1f,0.1f,1.0f};

GuiNode::GuiNode(int idn,double xt,double yt,double hdg,int shape) {
	x = xt;
	y = yt;
	id = idn;
	posindex=-1;
	this->hdg = hdg;

	m_shape = shape;
}

GuiNode::GuiNode(int idn,double xt,double yt,double hdg,int shape,int psx) {
	x = xt;
	y = yt;
	id = idn;
	posindex = psx;
	this->hdg = hdg;

	m_shape = shape;
}

GuiNode::~GuiNode() {

}

void GuiNode::setPosition(double xt,double yt){
	x = xt;
	y = yt;
}

void GuiNode::draw(){
	draw(R,G,B);
}
void GuiNode::draw(float r, float g, float b){
	glPushMatrix();
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glColor3f(r,g,b);
	glLightfv(GL_LIGHT0, GL_POSITION,PositionLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR,SpecularLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT,AmbientLight_car);
	glTranslatef(x,y,0);
	glRotatef(hdg,0.0f,0.0f,1.0f);
	glScalef(0.5f,0.5f,0.5f);
	glCallList(m_shape);
	glPopMatrix();
	char string[12];
	if(posindex!=-1)
		sprintf(string,"%d(%d)",id,posindex);
	else
		sprintf(string,"%d",id);
	glRasterPos3f(x,y,2.0f);
	for (int j = 0; string[j]!='\0'; j++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[j]);
	}
}

void GuiNode::draw(float r, float g, float b,std::string tag){
	glPushMatrix();
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glColor3f(r,g,b);
	glLightfv(GL_LIGHT0, GL_POSITION,PositionLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR,SpecularLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT,AmbientLight_car);
	glTranslatef(x,y,0);
	glRotatef(hdg,0.0f,0.0f,1.0f);
	glScalef(0.5f,0.5f,0.5f);
	glCallList(m_shape);
	glPopMatrix();
	char string[28];
	sprintf(string,"%s",tag.c_str());
	glRasterPos3f(x,y,25.0f);
	for (int j = 0; string[j]!='\0'; j++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[j]);
	}
}

}
