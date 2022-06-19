/*
 * sumo-connection.cpp
 *
 *  Created on: Aug 11, 2015
 *      Author: marian
 */

#include <ns3/beaconing-protocol.h>
#include "sumo-connection.h"
#include <ns3/location-table.h>
#include <unistd.h>

namespace ns3 {


SumoConnection::SumoConnection(std::string sumo_prog,NodeContainer c,NetDeviceContainer devices,unsigned int size,float step_size,GuiManager *man) {
	system(sumo_prog.c_str());
	usleep(4000000);
	m_nodeContainer = c;
	m_devices = devices;
	m_size = size;
	m_stepsize = step_size;
	m_client.connect("localhost",8888);
	current_index = 0;
	cursec=-1;
	if(man!=0){
		m_man=man;
		std::vector<std::string> ids = m_client.lane.getIDList();

		int count=0;
		for(int i=0;i<m_client.lane.getIDCount();i++){
			libsumo::TraCIPositionVector shape =  m_client.lane.getShape(ids[i]);
			count=0;
			for(unsigned int j=0;j<shape.size();j++){
				xs.push_back(shape[j].x);
				ys.push_back(shape[j].y);
				count++;
			}
			counts.push_back(count);
		}
		std::cout<<m_client.lane.getIDCount()<<" "<<xs.size()<<" "<<ys.size()<<" "<<counts.size()<<std::endl;
		m_man->getTopology()->setTopology(xs,ys,counts);
		//		m_man=man;
		//		m_client->getPolygons();
		//		m_man->getTopology()->setTopology(m_client->coordx,m_client->coordy,m_client->counts);
	}
	else
		m_man=0;
	Simulator::Schedule(Seconds(0),&SumoConnection::disablePhysical,this);
	Simulator::Schedule (MilliSeconds (m_stepsize*1000), &SumoConnection::iterate, this);
}


SumoConnection::~SumoConnection() {
	system("killall sumo");
}


void SumoConnection::disablePhysical(){
	//std::cout<< " device size in simulation: "<<m_size<<std::endl;
	for(unsigned int i=0;i<m_size;i++){
		DynamicCast<LteUePhy>(DynamicCast<LteUeNetDevice>(m_devices.Get(i))->GetPhy())->SetDisabled(true);
	}
}

void SumoConnection::makeAllNodesPassive(){
	NodeMap::iterator i=activeNodeList.begin();
	while(i!=activeNodeList.end()){
		i->second.active=false;
		i++;
	}
}

void SumoConnection::syncronizeNodes(){
	std::vector<unsigned int> rest;
	std::vector<std::string> ids = m_client.vehicle.getIDList();
	if(m_man!=0){
		m_man->clearNodes();
	}
	for(int i=0;i<m_client.vehicle.getIDCount();i++){
		NodeMap::iterator it = activeNodeList.find(ids.at(i));
		if(it!=activeNodeList.end()){
			libsumo::TraCIPosition pos  = m_client.vehicle.getPosition(ids.at(i));
			it->second.node->GetObject<SumoMobilityModel>()->SetPosition(Vector(pos.x,pos.y,0.00));
			//it->second.node->GetObject<SumoMobilityModel>()->SetPositionAndSpeed(Vector(m_client->ps.at(i).x,
			//						m_client->ps.at(i).y,0.00),m_client->speeds.at(i),m_client->angles.at(i),m_client->roadids.at(i),m_client->laneids.at(i),getAcc(m_client->laneids.at(i)));
			it->second.active=true;
			if(m_man!=0){
				//m_man->setNode(it->second.node->GetId(),pos.x,pos.y,m_client.vehicle.getAngle(ids.at(i)));
				m_man->setNodePi(it->second.node->GetId(),pos.x,pos.y,m_client.vehicle.getAngle(ids.at(i)),it->second.node->GetObject<LocationTable>()->m_cur_posindex);

			}
		}
		else{
			rest.push_back(i);
		}
	}
	for(unsigned int i=0;i<rest.size();i++){
		libsumo::TraCIPosition pos  = m_client.vehicle.getPosition(ids.at(rest.at(i)));
		SumoNode newNode;
		newNode.node = m_nodeContainer.Get(current_index);
		current_index++;
		newNode.active=true;
		newNode.id = ids.at(rest.at(i));
		newNode.node->GetObject<SumoMobilityModel>()->SetPositionAndSpeed(Vector(pos.x,
				pos.y,0.00),m_client.vehicle.getSpeed(ids.at(rest.at(i))),m_client.vehicle.getAngle(ids.at(rest.at(i))),"","",0);
		newNode.node->GetObject<SumoMobilityModel>()->SetErrorValues(0,0);
		newNode.node->GetObject<BeaconingProtocol>()->start();
		if(m_man!=0){
		//	m_man->setNode(newNode.node->GetId(),pos.x,pos.y,m_client.vehicle.getAngle(ids.at(rest.at(i))));
			m_man->setNodePi(newNode.node->GetId(),pos.x,pos.y,m_client.vehicle.getAngle(ids.at(rest.at(i))),newNode.node->GetObject<LocationTable>()->m_cur_posindex);

		}
		DynamicCast<LteUePhy>(DynamicCast<LteUeNetDevice>(m_devices.Get(newNode.node->GetId()-1))->GetPhy())->SetDisabled(false);
		activeNodeList.insert(std::pair<std::string,SumoNode>(ids.at(rest.at(i)),newNode));
	}
}

void SumoConnection::clearDeadNodes(){
	NodeMap::iterator i=activeNodeList.begin();
	while(i!=activeNodeList.end()){
		if(!(i->second.active)){
			NodeMap::iterator tmp = i;
			DynamicCast<LteUePhy>(DynamicCast<LteUeNetDevice>(m_devices.Get(tmp->second.node->GetId()-1))->GetPhy())->SetDisabled(true);
			i++;
			activeNodeList.erase(tmp);
		}
		else{
			i++;
		}
	}
}

void SumoConnection::changeErrorValues(int nodeid,int error){
	m_nodeContainer.Get(nodeid)->GetObject<SumoMobilityModel>()->SetErrorValues(error,error);
}


void SumoConnection::iterate(){
	m_client.simulationStep();
	makeAllNodesPassive();
	syncronizeNodes();
	clearDeadNodes();
	Simulator::Schedule (Seconds (m_stepsize), &SumoConnection::iterate, this);
}

} /* namespace ns3 */
