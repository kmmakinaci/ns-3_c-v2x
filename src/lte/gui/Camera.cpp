/*
 * Camera.cpp
 *
 *  Created on: Sep 28, 2015
 *      Author: marian
 */

#include "Camera.h"

namespace ns3 {

Camera::Camera() {
	lpressed = false;
	rpressed = false;
	oldx = anglex = oldy = angley = tarx = tary = tarz = camx = camy = camz = 0;
	alfa = 3*M_PI/2.0;
	beta = 3*M_PI/2.0;
	rad = 20;
}

Camera::~Camera() {
	// TODO Auto-generated destructor stub
}

void
Camera::setLPress(bool l,int x, int y){
	if(l){
		oldx = x;
		oldy = y;
		lpressed=true;
	}
	else{
		oldx = 0;
		oldy = 0;
		lpressed=false;
	}
}

void
Camera::setRpress(bool r,int x, int y){
	if(!lpressed){
		if(r){
			oldx = x;
			oldy = y;
			rpressed=true;
		}
		else{
			oldx = 0;
			oldy = 0;
			rpressed=false;
		}
	}
}

void
Camera::getMotion(int x,int y){
	if(lpressed){
		double radiscale=(rad/1000)*2;
		tarx+=radiscale * (-1*(oldx-x) * sin(beta) + (oldy-y) * cos(beta));
		tary+= radiscale * ((oldx-x) * cos(beta) + (oldy-y) * sin(beta));
		oldx=x;
		oldy=y;
	}
	else if(rpressed){
		beta+=(oldx-x)*M_PI*0.005;
		alfa+=(oldy-y)*M_PI*0.005;
		oldx=x;
		oldy=y;
	}
}

void Camera::zoomOut(){
	rad+=ZOOM_SCALE;
}

void Camera::zoomIn(){
	rad-=ZOOM_SCALE;
}


void Camera::updateCamera(){
	camx = tarx + rad * sin(alfa) * cos(beta);
	camy = tary + rad * sin(alfa) * sin(beta);
	camz = tarz + rad * cos(alfa);
}

void Camera::get(){
	updateCamera();
	gluLookAt(camx,camy,camz,tarx,tary,tarz,0,0,1);
}


void Camera::setTarget(float x,float y,float z){
	tarx= x;
	tary = y;
	tarz=z;
}

} /* namespace ns3 */
