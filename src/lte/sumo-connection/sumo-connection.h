/*
 * sumo-connection.h
 *
 *  Created on: Aug 11, 2015
 *      Author: marian
 */

#ifndef SUMO_CONNECTION_H_
#define SUMO_CONNECTION_H_
#include <ns3/node-container.h>
#include <ns3/core-module.h>
#include <ns3/net-device-container.h>
#include <ns3/lte-ue-phy.h>
#include <ns3/lte-ue-net-device.h>
#include <ns3/sumo-mobility-model.h>
#include <ns3/GuiManager.h>
#include "TraCIAPI.h"
#include <stdlib.h>
#include <map>

namespace ns3 {

typedef struct SumoNode{
	Ptr<Node> node;
	std::string id;
	bool active;
}SumoNode;

class SumoConnection {
public:
	SumoConnection(std::string sumo_prog,NodeContainer c,NetDeviceContainer devices,unsigned int size,float step_size,GuiManager *man);
	virtual ~SumoConnection();
	void iterate();

	void changeErrorValues(int nodeid,int error);

private:
	void makeAllNodesPassive();
	void syncronizeNodes();
	void clearDeadNodes();
	void disablePhysical();


private:
	unsigned int m_size;
	float m_stepsize;
	unsigned int current_index;

	NodeContainer m_nodeContainer;
	NetDeviceContainer m_devices;
	typedef std::map<std::string,SumoNode> NodeMap;
	NodeMap activeNodeList;

	int cursec;
	TraCIAPI m_client;
	GuiManager *m_man;

	std::vector<double> xs;
	std::vector<double> ys;
	std::vector<int> counts;
};

} /* namespace asd */
#endif /* SUMO_CONNECTION_H_ */
