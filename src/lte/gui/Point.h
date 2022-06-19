/*
 * Point.h
 *
 *  Created on: Sep 28, 2015
 *      Author: marian
 */

#ifndef POINT_H_
#define POINT_H_

#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Utils.h"

#define R 1.0f
#define G 0.0f
#define B 0.0f

namespace ns3 {

class Point : public GuiObject {
public:
	Point(int id_n,double xt,double yt);
	virtual ~Point();

	void draw();
	void draw(float r,float g,float r);

private:
	double x;
	double y;

	int id;

	int shape;
};

} /* namespace ns3 */
#endif /* POINT_H_ */
