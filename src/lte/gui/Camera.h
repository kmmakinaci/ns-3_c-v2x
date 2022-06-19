/*
 * Camera.h
 *
 *  Created on: Sep 28, 2015
 *      Author: marian
 */

#ifndef CAMERA_H_
#define CAMERA_H_
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string>
#include <sstream>
#define ZOOM_SCALE 10

namespace ns3 {

class Camera {
public:
	Camera();
	virtual ~Camera();

	void setTarget(float x,float y,float z);
	void setLPress(bool l,int x,int y);
	void setRpress(bool r,int x,int y);
	void getMotion(int x,int y);
	void updateCamera();
	void get();

	void zoomIn();
	void zoomOut();

private:
	int oldx;
	int oldy;
	bool lpressed;
	bool rpressed;

	float anglex;
	float angley;
	float alfa;
	float beta;
	float tarx;
	float tary;
	float tarz;
	float rad;
	float camx;
	float camy;
	float camz;
};

} /* namespace ns3 */
#endif /* CAMERA_H_ */
