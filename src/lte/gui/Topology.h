/*
 * Topology.h
 *
 *  Created on: Sep 28, 2015
 *      Author: marian
 */

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include <pthread.h>
#include <cmath>
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <shapefil.h>
#include <string>
namespace ns3 {

class Topology {
public:
	Topology();
	virtual ~Topology();

	typedef struct building{
		std::vector<double> xs;
		std::vector<double> ys;
		int h;
	}building;

	void setTopology(std::vector<double> x,std::vector<double> y, std::vector<int> c);
	void addBuildingsFromShapeFile(std::string dbf,std::string shp);
	void addBuildingsFromTxtFile(std::string txt);
	bool isExists();

	void prepare();
	void addBuilding(std::vector<double> xs,std::vector<double> ys,int h);
	void drawNormal(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3);
	void drawBox(building b);
	void draw();
	std::vector<double> coordsx;
	std::vector<double> coordsy;
	std::vector<building> buildings;

	std::vector<int> counts;
private:
	int c_list;
	int b_list;

	bool exists;

};

} /* namespace asd */
#endif /* TOPOLOGY_H_ */
