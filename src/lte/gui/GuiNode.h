/*
 * guiNode.h
 *
 *  Created on: Sep 28, 2015
 *      Author: marian
 */

#ifndef GUINODE_H_
#define GUINODE_H_

#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include<string>
#include "GuiObject.h"
#define R 0.0f
#define G 0.0f
#define B 1.0f


namespace ns3 {

class GuiNode : public GuiObject {
public:
	GuiNode(int id_n,double xt,double yt,double hdg,int shape);
	GuiNode(int id_n,double xt,double yt,double hdg,int shape,int psx);
	virtual ~GuiNode();

	void setPosition(double xt,double yt);

	void draw();
	void draw(float r, float g,float b);
	void draw(float r, float g,float b,std::string);

	int id;
	int posindex;
	double x;
	double y;
	double hdg;

private:



	int m_shape;
};

}
#endif /* guiNode_H_ */
