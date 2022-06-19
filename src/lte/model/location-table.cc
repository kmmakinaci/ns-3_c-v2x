/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

#include <ns3/simulator.h>
#include <ns3/mobility-model.h>

#include "sumo-mobility-model.h"
#include "beaconing-protocol.h"
#include "location-table.h"

#include "ns3/network-module.h"
#include <iostream>
#include <fstream>


#define MAX_NEIGHBOOR_DIST 300000

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LocationTable");

NS_OBJECT_ENSURE_REGISTERED (LocationTable);
const float LocationTable::LOCATION_ENTRY_LIFETIME = 1000;

std::string simIdx = "log_idx_v2x.csv";
Ptr<OutputStreamWrapper> log_simIdx;

TypeId 
LocationTable::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::LocationTable")
 	.SetParent<Object> ()
	.AddConstructor<LocationTable> ();

	return tid;
}

LocationTable::LocationTable ()
{
	NS_LOG_FUNCTION (this);
	Simulator::ScheduleNow (&LocationTable::ScheduleUpdatePos, this); // schedule update own position
	Simulator::ScheduleNow (&LocationTable::ScheduleCleanTable, this); //schedule clean table
	m_cur_posindex= 0;
	//FIXME this one can be setted with pps*subchannelsize
	max_pos_index = 300;
	m_mu=10;
	m_n= 0.1;
	rednr = CreateObject<UniformRandomVariable> ();
	first=true;

    AsciiTraceHelper ascii;
    log_simIdx = ascii.CreateFileStream(simIdx);
}

LocationTable::~LocationTable ()
{
	NS_LOG_FUNCTION (this);
	Simulator::Cancel (m_updateEvent);
	Simulator::Cancel (m_posEvent);
}

void
LocationTable::DoDispose (void)
{
	NS_LOG_FUNCTION (this);
	m_node = 0;
	Object::DoDispose ();
}

void 
LocationTable::SetNode (Ptr<Node> node)
{
	NS_LOG_FUNCTION (this);
	m_node = node;
}
void LocationTable::Print(){
	std::cout<<"*** Node "<<m_node->GetId()<<" Location Table ***"<<std::endl;
	for(Table::iterator it=m_Table.begin();it<m_Table.end();it++){
		std::cout<<(*it).gnAddr<<" "<<(*it).Lat<<" "<<(*it).Long<<std::endl;
	}
}

void
LocationTable::NotifyNewAggregate ()
{
	if (m_node == 0)
	{
		Ptr<Node>node = this->GetObject<Node>();
		// verify that it's a valid node and that
		// the node has not been set before
		if (node != 0)
		{
			this->SetNode (node);
		}
	}
	Object::NotifyNewAggregate ();
}

void 
LocationTable::ScheduleUpdatePos ()
{
	m_posEvent = Simulator::ScheduleNow (&LocationTable::AddOwnEntry, this);
}

void
LocationTable::ScheduleCleanTable ()
{
	m_updateEvent = Simulator::ScheduleNow (&LocationTable::CleanTable, this);
}

uint16_t LocationTable::CalculateAndGetPosindex(){
	if(m_Table.size()==0 || m_Table.size()==1){
//		if(first){
//			m_cur_posindex = rednr->GetInteger (0, 80);
//			first=false;
//		}
		return m_cur_posindex;
	}
	else{
		//FIXME add the loose time update the locations
		first=false;
		double curx = ((double)m_cur_entry.GetLatitude()/100.0)-10000.0;
		double cury = ((double)m_cur_entry.GetLongitude()/100.0)-10000.0;
		double curheading = m_cur_entry.GetHeading();
		std::vector<LocTableEntry> vec;
		for (Table::iterator i = m_Table.begin (); i != m_Table.end (); i++){
			if(i->gnAddr != m_cur_entry.GetId()){
				double x = ((double)i->Lat/100.0)-10000.0;
				double y = ((double)i->Long/100.0)-10000.0;
				double relative_bearing = atan2(cury-y,curx-x)*57.295779513+360;
				double dist = sqrt((curx-x)*(curx-x) + (cury-y)*(cury-y));
				if(relative_bearing>360){
					relative_bearing-=360;
				}
				//Sort for vehicle
				double b_diff = curheading-relative_bearing;

				if(b_diff<0){
					b_diff+=360;
				}
				if(curheading>180){
					if(b_diff>180){
						dist=-1*dist;
						//std::cout<<"BEHIND: dist->"<<dist<<" "<<x<<" "<< y<<" "<<relative_bearing<<" "<<curheading<<" "<<relative_bearing<<std::endl;
					}
				}
				else{
					if(b_diff<180){
						dist=-1*dist;
						//std::cout<<"BEHIND: dist->"<<dist<<" "<<x<<" "<< y<<" "<<relative_bearing<<" "<<curheading<<" "<<relative_bearing<<std::endl;
					}
				}
				i->dist = dist;
				if(dist>0){
					vec.push_back(*i);
				}
			}
		}
		if(vec.size()>0){
			std::sort(vec.begin(),vec.end());
			int max_arg = 0;
			int pi = -1;
			for(unsigned int tmp_pos_ind = 0;tmp_pos_ind <max_pos_index;tmp_pos_ind++){
				int sum = 0;
				for(unsigned int i=0;i<vec.size();i++){
					if((vec[i].PosIndex + i + 1 )==tmp_pos_ind){
						sum+=1*(m_mu+m_n*vec[i].dist);////FROM THE PAPER PIt=argmaxPIU∑u=1Φ(PI,PIu+u)·(μ+η·dt,u)
					}
				}
				if(sum>=max_arg){
					max_arg = sum;
					pi=tmp_pos_ind;
				}
			}
			//m_cur_posindex = pi;

			m_cur_posindex = (pi+10)%max_pos_index;

			//(default mod idx)
			//pi = m_cur_entry.GetId() % 300;
			//m_cur_posindex = pi;

			//(random mod idx)
			//int v1 = rand() % 300;         // v1 in the range 0 to 300
			//m_cur_posindex = v1;
		}
		else{
			m_cur_posindex=0;
		}
		*log_simIdx->GetStream() << Simulator::Now ().GetSeconds () << ";" << m_cur_entry.GetId()<<";" <<" MY POS INDEX IS: "<<";" <<m_cur_posindex<<";";
		//std::cout<< m_cur_entry.GetId()<<" MY POS INDEX IS: "<<m_cur_posindex<<" ";
		for(unsigned int i=0;i<vec.size();i++){
			*log_simIdx->GetStream() <<vec[i].gnAddr<<";"<<vec[i].PosIndex<<";"<<vec[i].dist<<";";
			//std::cout<<vec[i].gnAddr<<","<<vec[i].PosIndex<<","<<vec[i].dist<<" ";
		}
		//std::cout<<std::endl;
		*log_simIdx->GetStream() << std::endl;
	}

	return m_cur_posindex;
}


int
LocationTable::getEntry (unsigned int gnAddr)
{
	for (uint32_t i = 0; i < m_Table.size(); i++){
		if (m_Table[i].gnAddr == gnAddr){
			return i;
		}
	}
	return -1;
}


void
LocationTable::AddOwnEntry ()
{
	if (m_node != 0)
	{
		BeaconingProtocolHeader h;
		h.SetId(m_node->GetId());
		h.SetTimeStamp(Simulator::Now().GetNanoSeconds());
		h.SetLatitude(m_node->GetObject<SumoMobilityModel>()->GetLatitude());
		h.SetLongitude(m_node->GetObject<SumoMobilityModel>()->GetLongitude());
		h.SetSpeed(m_node->GetObject<SumoMobilityModel>()->GetSpeed());
		h.SetHeading(m_node->GetObject<SumoMobilityModel>()->GetHeading());
		h.SetPosIndex(m_cur_posindex);
		m_cur_entry  = h;
		AddPosEntry(h);
	}
	NS_LOG_DEBUG  ("LOCATION_TABLE on node "<<m_node->GetId() <<" add own entry called");

	//FOR DEBUGGIN A NODE
	//	if(m_node->GetId()==3){
	//		std::cout<<"-------"<<Simulator::Now().GetNanoSeconds()<<"--------"<<std::endl;
	//		for (Table::iterator i = m_Table.begin (); i != m_Table.end (); i++)
	//		{
	//			std::cout<<i->gnAddr<<" "<<i->Lat<<" "<<i->Long<<" "<<i->Ts<<std::endl;
	//		}
	//		std::cout<<"--------"<<std::endl;
	//	}
	//m_Table.clear();
	//ntable ->AddPosEntry (vector);
	m_posEvent = Simulator::Schedule (MilliSeconds (BeaconingProtocol::BEACON_INTERVAL), &LocationTable::AddOwnEntry, this);
}
double LocationTable::CartesianDistance (double lat2, double lon2, double lat1, double lon1)
{
	double distance;
	distance = sqrt(((lat1-lat2)*(lat1-lat2))+((lon1-lon2)*(lon1-lon2)));
	return distance;
}

void
LocationTable::AddPosEntry (BeaconingProtocolHeader h)
{bool exist = false;
struct LocTableEntry entry;
entry.gnAddr = h.GetId();
entry.Ts = h.GetTimeStamp();
entry.Lat = h.GetLatitude();
entry.Long = h.GetLongitude();
entry.Speed = h.GetSpeed();
entry.Heading = h.GetHeading();
entry.PosIndex = h.GetPosIndex();
entry.arrival_ts = Simulator::Now().GetNanoSeconds();

if (m_Table.size() != 0)
{
	for (Table::iterator i = m_Table.begin (); i != m_Table.end (); i++)
	{

		if (i->gnAddr == entry.gnAddr)
		{
			NS_LOG_DEBUG  ("LOCATION_TABLE: Node "<< entry.gnAddr << " is in the table");
			exist = true;
			if (i->Ts == entry.Ts){
				NS_LOG_DEBUG  ("LOCATION_TABLE: Node "<< entry.gnAddr << " is in the table and I last stored it in this TS. I do not store it again ");
				return;
			}
			else
			{
				m_Table.erase (i);
				NS_LOG_DEBUG  ("LOCATION_TABLE: Node "<< entry.gnAddr << " was a neigh so I store it as such");
				m_Table.push_back (entry);
				return;
			}
		}
	}
	if (exist == false){
		if(entry.gnAddr == m_node->GetId()){
			NS_LOG_DEBUG  ("LOCATION_TABLE: Node "<< entry.gnAddr << " was not in the table. It is the local node. I store it as a neigh");
		}
		else{
			NS_LOG_DEBUG  ("LOCATION_TABLE: Node "<< entry.gnAddr << " was not in the table, I store it as not a neigh");
		}
		m_Table.push_back (entry);
	}
}
else{
	if(entry.gnAddr == m_node->GetId()){
		NS_LOG_DEBUG  ("LOCATION_TABLE: Node "<< entry.gnAddr << " was not in the table. the table was empty. It is the local node. I store it as a neigh");
	}
	else{
		NS_LOG_DEBUG  ("LOCATION_TABLE: Node "<< entry.gnAddr << " was not in the table (table was empty), I store it as not a neigh");
	}
	m_Table.push_back (entry);
}

}

void
LocationTable::CleanTable ()
{
	Time time = Simulator::Now ();
	double interval;
	if (m_Table.size() != 0)
	{unsigned int c=0;
	for (Table::iterator i = m_Table.begin ();
			i != m_Table.end () && c<m_Table.size(); )
	{
		interval = ((double)(time.GetNanoSeconds()) - (double)(i->Ts))/1000000.0;
		if (interval >= LOCATION_ENTRY_LIFETIME)
		{
			i=m_Table.erase (i);
		}
		else{
			i++;
		}
		c++;
	}
	}

	m_updateEvent = Simulator::Schedule (Seconds (0.1), &LocationTable::CleanTable, this);
}

float LocationTable::GetLinkQuality (){
	return 0.0;
}


std::vector<ptime>
LocationTable::GetFreshNb ()
{
	std::vector<ptime> res;
	if (m_Table.size() != 0)
	{
		for (Table::iterator i = m_Table.begin ();
				i != m_Table.end (); i++)
		{
			if(m_node->GetId ()!=i->gnAddr){
				//&& (i->PosAcc == 2)
				if (((Simulator::Now().GetNanoSeconds()-i->arrival_ts) < 100000000) )
				{
					ptime t;
					t.gnAddr = i->gnAddr;
					t.Ts = Simulator::Now().GetNanoSeconds()-i->arrival_ts;
					res.push_back(t);
				}
			}
		}
	}
	return res;
}


int
LocationTable::GetNbNeighs ()
{
	return m_Table.size();
}

LocationTable::Table 
LocationTable::GetTable ()
{
	return m_Table;
}

} //namespace ns3
