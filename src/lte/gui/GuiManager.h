/*
 * GuiManager.h
 *
 *  Created on: Sep 28, 2015
 *      Author: marian
 */

#ifndef GUIMANAGER_H_
#define GUIMANAGER_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>
#include <vector>
#include <semaphore.h>
#include <ns3/core-module.h>
#include "Topology.h"
#include "Camera.h"
#include "GuiNode.h"
#define HEIGHT 1024
#define WIDTH  1280

typedef struct specialColor{
	int node;
	int r;
	int g;
	int b;
	std::string tag;
}sColor;

namespace ns3 {

class GuiManager {
public:
	GuiManager();
	virtual ~GuiManager();
	void init();
	void Idle(void);
	void MouseButton(int button, int state, int x, int y);
	void MouseMotion(int x, int y);
	void Keyboard(unsigned char key, int x, int y);

	void setNode(int id, double x, double y,double hdg);
	void setNodePi(int id, double x, double y,double hdg,int posindex);
	void setNodeStatic(int id, double x, double y,double hdg);
	void clearNodes();

	void drawSceene();
	void drawSceeneX();
	void reshape(GLint width, GLint height);
	void displaysub(void);
	int init_SDL();
	void start();

	void addSpecialColor(int node,std::string tag,int r,int g,int b);
	void clearSpecialColor();
	void subView();
	void SceeneView();
	void drawCarShape();

	bool GetPause();
	Topology *getTopology();

	bool keeprunning;
	bool resized;
	Camera cam;
	Topology *topo;

	int screen_width;
	int screen_height;

	std::vector<GuiNode> nodevec;
private:
	long time;

	int car_shape;
	int point_shape;

	SDL_mutex *lock;

	bool pause;

	std::vector<GuiNode> nodevec_static;
	std::map<int,sColor> sc_list;
};

}
#endif /* GUIMANAGER_H_ */
