/*
 * Topology.cpp
 *
 *  Created on: Sep 28, 2015
 *      Author: marian
 */

#include "Topology.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#define MAXLON 29.049140
#define MINLON 29.041731
#define MAXLAT 40.991075
#define MINLAT 40.985763
#define NO 4098605.9//4099245.8//4098605.9 //4099245.8 ////
#define WO 2904240.6//2904239.9//2906225.3//2905662.8////
namespace ns3 {

Topology::Topology() {
	exists = false;
}

Topology::~Topology() {
}

void
Topology::setTopology(std::vector<double> x,std::vector<double> y, std::vector<int> c) {
	for(unsigned int i=0;i<x.size();i++){
		std::cout<<x[i]<<" "<<y[i]<<std::endl;
		coordsx.push_back(x[i]);
		coordsy.push_back(y[i]);
	}
	for(unsigned int i=0;i<c.size();i++){
		counts.push_back(c[i]);
	}
	exists = true;
}

bool Topology::isExists(){
	return exists;
}

void
Topology::prepare(){
	float delta_theta = 0.01;
	c_list=glGenLists(1);
	glNewList(c_list,GL_COMPILE);
	glPointSize(5.0f);
	glColor3f(0.55f,0.55f,0.55f);
	int s=0;
	for(unsigned int i=0;i<counts.size();i++){
		for(int j=0;j<counts.at(i)-1;j++){
			double x1=coordsx.at(s),y1=coordsy.at(s),x2=coordsx.at(s+1),y2=coordsy.at(s+1);
			double alfa=atan((x2-x1)/(y2-y1)),down=alfa+M_PI/2,up=alfa-M_PI/2;
			glBegin(GL_QUADS);
			float x,y;
			x=x1+1.50*sin(down);
			y=y1+1.50*cos(down);
			glVertex3f(x,y,0.0f);
			x=x1+1.50*sin(up);
			y=y1+1.50*cos(up);
			glVertex3f(x,y,0.0f);
			x=x2+1.50*sin(up);
			y=y2+1.50*cos(up);
			glVertex3f(x,y,0.0f);
			x=x2+1.50*sin(down);
			y=y2+1.50*cos(down);
			glVertex3f(x,y,0.0f);
			s++;
			glEnd();
			glBegin(GL_POLYGON);
			for( float angle = 0; angle < 2*M_PI; angle += delta_theta )
				glVertex3f(  1.5*cos(angle)+x2, 1.5*sin(angle)+y2, 0 );
			glEnd();
			glEnd();
		}
		s++;
	}
	glEnd();
	glEndList();
	b_list=glGenLists(1);
	glNewList(b_list,GL_COMPILE);
	for(unsigned int i=0;i<buildings.size();i++){
		drawBox(buildings.at(i));
	}
	glEndList();
}

void Topology::addBuildingsFromTxtFile(std::string txt){
	std::ifstream f(txt.c_str());
	std::vector<double> xgui,ygui;
	double x,y;
	int pos;
	while(!f.eof()){
		std::string s;
		std::getline(f,s);
		pos = s.find(",");
		x=atof(s.substr(0,pos).c_str());
		if(std::isnan(x)){
			addBuilding(xgui,ygui,20);
			xgui.clear();
			ygui.clear();
			continue;
		}
		s.erase(0,pos+1);
		y=atof(s.c_str());
		xgui.push_back(x);
		ygui.push_back(y);
	}
}

void Topology::addBuildingsFromShapeFile(std::string dbf,std::string shp){
	DBFHandle d;
	SHPHandle h;
	bool shape;
	int nShapeType,pentities;
	SHPObject *psObject;
	d=DBFOpen(dbf.c_str(),"rb");
	h=SHPOpen(shp.c_str(),"rb");
	SHPGetInfo( h, &pentities, &nShapeType, NULL, NULL );
	std::vector<double> xs,ys;
	for(int i=0;i<pentities;i++){
		shape=true;
		psObject=SHPReadObject(h,i);
		double *x=psObject->padfX,*y=psObject->padfY;
		int floor=DBFReadIntegerAttribute(d,i,6),v=psObject->nVertices;
		double h=floor*3.0;
		if(psObject->nSHPType==0)
			continue;
		for(int j=0;j<v;j++){
			xs.push_back((x[j%v]*100000)-WO);
			ys.push_back((y[j%v]*100000)-NO);
			if(x[j%v]>MAXLON || x[j%v] < MINLON || y[j%v] > MAXLAT || y[j%v] < MINLAT){
				shape=false;
				break;
			}
		}
		if(shape){
			addBuilding(xs,ys,h);
		}
		xs.clear();
		ys.clear();
	}
}

void Topology::addBuilding(std::vector<double> xs,std::vector<double> ys,int h){
	building b;
	b.xs=xs;
	b.ys=ys;
	b.h=h;
	buildings.push_back(b);
}

void Topology::drawNormal(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3){
	double px,py,pz,qx,qy,qz,nx,ny,nz;
	px=x2-x1;
	py=y2-y1;
	pz=z2-z1;
	qx=x3-x1;
	qy=y3-y1;
	qz=z3-z1;
	nx=(py*qz)-(pz*qy);
	ny=(pz*qx)-(px*qz);
	nz=(px*qy)-(py*qx);
	double length=sqrt(nx*nx+ny*ny+nz*nz);
	glNormal3f(nx/length,ny/length,nz/length);
}

void Topology::drawBox(building b){
	glBegin(GL_QUADS);
	glColor3f(0.8f, 0.85f, 0.78f);
	for(unsigned int i=0;i<b.xs.size();i++){
		glVertex3f(b.xs.at(i),b.ys.at(i),0);
		drawNormal(b.xs.at(i),b.ys.at(i),0,b.xs.at(i),b.ys.at(i),b.h,b.xs.at((i+1)%(b.xs.size())),b.ys.at((i+1)%(b.xs.size())),0);
		glVertex3f(b.xs.at(i),b.ys.at(i),b.h);
		drawNormal(b.xs.at(i),b.ys.at(i),b.h,b.xs.at(i),b.ys.at(i),0,b.xs.at((i+1)%(b.xs.size())),b.ys.at((i+1)%(b.xs.size())),b.h);
		glVertex3f(b.xs.at((i+1)%(b.xs.size())),b.ys.at((i+1)%(b.xs.size())),b.h);
		drawNormal(b.xs.at((i+1)%(b.xs.size())),b.ys.at((i+1)%(b.xs.size())),b.h,b.xs.at(i),b.ys.at(i),b.h,b.xs.at((i+1)%(b.xs.size())),b.ys.at((i+1)%(b.xs.size())),0);
		glVertex3f(b.xs.at((i+1)%(b.xs.size())),b.ys.at((i+1)%(b.xs.size())),0);
		drawNormal(b.xs.at((i+1)%(b.xs.size())),b.ys.at((i+1)%(b.xs.size())),0,b.xs.at((i+1)%(b.xs.size())),b.ys.at((i+1)%(b.xs.size())),b.h,b.xs.at(i),b.ys.at(i),0);
	}
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.8f,0.5f,0.5f);
	for(unsigned int i=0;i<b.xs.size();i++){
		glVertex3f(b.xs.at(i),b.ys.at(i),b.h);
		drawNormal(b.xs.at(i),b.ys.at(i),b.h,b.xs.at((i+1)%(b.xs.size())),b.ys.at((i+1)%(b.xs.size())),b.h,b.xs.at((i-1+b.xs.size())%(b.xs.size())),b.ys.at((i-1+b.xs.size())%(b.xs.size())),b.h);
	}
	glEnd();
}

void Topology::draw(){
	glCallList(c_list);
	glEnable(GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT0, GL_POSITION,PositionLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR,SpecularLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT,AmbientLight_buildings);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glCallList(b_list);
	glDisable(GL_LIGHTING);
}

}
