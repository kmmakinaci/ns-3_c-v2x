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


#include <ns3/udp-socket-factory.h>
#include <ns3/udp-socket.h>
#include <ns3/address.h>
#include <ns3/inet-socket-address.h>
#include <ns3/packet-socket-address.h>
#include <ns3/socket.h>
#include <ns3/address-utils.h>
#include "lte-ue-net-device.h"
#include "lte-ue-phy.h"
#include "beaconing-protocol.h"
#include "sumo-mobility-model.h"
#include "location-table.h"
#include "ns3/network-module.h"
#include <iostream>
#include <fstream>

namespace ns3
{
const uint8_t BeaconingProtocol::TRAFFIC_CLASS = 0;
const float BeaconingProtocol::BEACON_INTERVAL = 100;

NS_LOG_COMPONENT_DEFINE ("BeaconingProtocol");

NS_OBJECT_ENSURE_REGISTERED (BeaconingProtocol);

std::vector<Ptr<Node>> nodeList;

uint32_t numVeh = 370; //TODO: MERT num of vehicle dependency

// Global variables
std::string simtime = "log_sim_v2x.csv";
std::string simrxtx = "log_rxtx_v2x.csv";
uint32_t ctr_totRx = 0; 	// Counter for total received packets
uint32_t ctr_totTx = 0; 	// Counter for total transmitted packets
uint32_t ctr_totRealTx = 0; 	// Counter for total transmitted packets

uint32_t ctr_totRx5 = 0; 	// Counter for total received packets
uint32_t ctr_totTx5 = 0; 	// Counter for total transmitted packets
uint32_t ctr_totRxC5 = 0; 	// Counter for total received packets

uint32_t ctr_totRx22 = 0; 	// Counter for total received packets
uint32_t ctr_totTx22 = 0; 	// Counter for total transmitted packets
uint32_t ctr_totRxC22 = 0; 	// Counter for total received packets

uint32_t ctr_totRx49 = 0; 	// Counter for total received packets
uint32_t ctr_totTx49 = 0; 	// Counter for total transmitted packets
uint32_t ctr_totRxC49 = 0; 	// Counter for total received packets

uint32_t ctr_totRxC5inDist = 0;
uint32_t ctr_totRxC22inDist = 0;
uint32_t ctr_totRxC49inDist = 0;

uint32_t ctr_totRx5inDist = 0;
uint32_t ctr_totRx22inDist = 0;
uint32_t ctr_totRx49inDist = 0;

uint32_t ctr_totTx5inDist = 0;
uint32_t ctr_totTx22inDist = 0;
uint32_t ctr_totTx49inDist = 0;

double PRR = 0;
int printCnt =0;
int distLevel = 3000;


Ptr<OutputStreamWrapper> log_simtime;
Ptr<OutputStreamWrapper> log_rxtx;


std::vector<uint16_t> commList, commList2, commList3;
uint16_t tId, tId2, tId3;

//std::vector<std::vector <uint16_t>> commList;//create  a communication list which contains each communicated node for every individual node


void PrintStatus (void)
{
    if (ctr_totRx > ctr_totRealTx)
    {
        ctr_totRx = ctr_totRealTx;
    }

    if (ctr_totRxC5inDist > ctr_totTx5inDist)
    {
    	ctr_totRxC5inDist = ctr_totTx5inDist;
    }

    if (ctr_totRxC22inDist > ctr_totTx22inDist)
    {
    	ctr_totRxC22inDist = ctr_totTx22inDist;
    }

    if (ctr_totRxC49inDist > ctr_totTx49inDist)
    {
    	ctr_totRxC49inDist = ctr_totTx49inDist;
    }

	printCnt++;
	if( (printCnt%numVeh) == 0){
		if( ctr_totRx != 0 && ctr_totRealTx !=0)
			PRR = ctr_totRx / ctr_totRealTx ;


		sort(commList.begin(), commList.end());
		decltype(commList)::iterator result = unique(commList.begin(), commList.end());
		commList.erase(result, commList.end());

		sort(commList2.begin(), commList2.end());
		decltype(commList2)::iterator result2 = unique(commList2.begin(), commList2.end());
		commList2.erase(result2, commList2.end());

		sort(commList3.begin(), commList3.end());
		decltype(commList3)::iterator result3 = unique(commList3.begin(), commList3.end());
		commList3.erase(result3, commList3.end());

		*log_simtime->GetStream() <<Simulator::Now ().GetSeconds () << ";" << ctr_totRx << ";" << ctr_totTx << ";" << PRR << ";"<< ctr_totRx5 << ";" << ctr_totTx5 << ";" << ctr_totRxC5 << ";"<< ctr_totRx22 << ";" << ctr_totTx22<< ";" << ctr_totRxC22 << ";"<< ctr_totRx49 << ";" << ctr_totTx49 << ";" << ctr_totRxC49 << ";" << commList.size() <<";"<< commList2.size() <<";"<< commList3.size() << ";"<< ctr_totRx5inDist <<";"<< ctr_totRx22inDist << ";" << ctr_totRx49inDist << ";"<< ctr_totRxC5inDist <<";"<< ctr_totRxC22inDist << ";" << ctr_totRxC49inDist  << ";"<< ctr_totRx5inDist <<";"<< ctr_totRx22inDist << ";" << ctr_totRx49inDist << ";"<< ctr_totRealTx<<std::endl;


		printCnt = 0;

		ctr_totRx = 0; 	// Counter for total received packets
		ctr_totTx = 0; 	// Counter for total transmitted packets
		ctr_totRealTx = 0; 	// Counter for total transmitted packets

		ctr_totRx5 = 0; 	// Counter for total received packets
		ctr_totTx5 = 0; 	// Counter for total transmitted packets
		ctr_totRxC5 = 0; 	// Counter for total received packets

		ctr_totRx22 = 0; 	// Counter for total received packets
		ctr_totTx22 = 0; 	// Counter for total transmitted packets
		ctr_totRxC22 = 0; 	// Counter for total received packets

		ctr_totRx49 = 0; 	// Counter for total received packets
		ctr_totTx49 = 0; 	// Counter for total transmitted packets
		ctr_totRxC49 = 0; 	// Counter for total received packets

		ctr_totRxC5inDist = 0;
		ctr_totRxC22inDist = 0;
		ctr_totRxC49inDist = 0;

		ctr_totRx5inDist = 0;
		ctr_totRx22inDist = 0;
		ctr_totRx49inDist = 0;

		ctr_totTx5inDist = 0;
		ctr_totTx22inDist = 0;
		ctr_totTx49inDist = 0;

	}

    Simulator::Schedule(MilliSeconds(1000), &PrintStatus);
    //<<(double)(((double) (ctr_totRx5/ctr_totRxC5)) / ctr_totTx5)
    //*log_simtime->GetStream() << "G" << ";" <<Simulator::Now ().GetSeconds () << ";" << node_cnt<< ";"<< ctr_totRx << ";" << ctr_totTx << ";" << (double) (ctr_totRx/node_cnt) / ctr_totTx << ";" <<std::endl;

    //ctr_totRx = 0; 	// Counter for total received packets
    //ctr_totTx = 0; 	// Counter for total transmitted packets
    //ctr_totRx5 = 0; 	// Counter for total received packets
    //ctr_totRxC5 = 0; 	// Counter for total received packets
    //ctr_totTx5 = 0; 	// Counter for total transmitted packets
}

double getDistance(BeaconingProtocolHeader h, Ptr<Node> m_node){
	double curx = ((double)h.GetLatitude()/100.0)-10000.0;
	double cury = ((double)h.GetLongitude()/100.0)-10000.0;
	double curheading = h.GetHeading();

			double x = ((double)m_node->GetObject<SumoMobilityModel>()->GetLatitude()/100.0)-10000.0;
			double y = ((double)m_node->GetObject<SumoMobilityModel>()->GetLongitude()/100.0)-10000.0;
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

	return dist;
}

TypeId 
BeaconingProtocol::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::BeaconingProtocol")
    						.SetParent<Object> ()
							.AddConstructor<BeaconingProtocol> ()
							;
	return tid;
}

BeaconingProtocol::BeaconingProtocol ()
{
	msec = CreateObject<UniformRandomVariable> ();
	msec->SetAttribute ("Min", DoubleValue (0));
	msec->SetAttribute ("Max", DoubleValue (99000));

    AsciiTraceHelper ascii;
    log_simtime = ascii.CreateFileStream(simtime);
    log_rxtx = ascii.CreateFileStream(simrxtx);

    *log_simtime->GetStream() << "Simtime;TotalRx;TotalTx;PRR;Node5Rx;Node5Tx;TxFromNode5;Node22Rx;Node22Tx;TxFromNode22;Node49Rx;Node49Tx;TxFromNode49;ActiveCommNodes5;ActiveCommNodes22;ActiveCommNodes49;RxInDist5;RxInDist22;RxInDist49;RxCInDist5;RxCInDist22;RxCInDist49;TxInDist5;TxInDist22;TxInDist49;totalRealTx" << std::endl;

    Simulator::Schedule(MilliSeconds(100), &PrintStatus);
}

BeaconingProtocol::~BeaconingProtocol ()
{
	Simulator::Cancel (m_sendBEvent);
	Simulator::Cancel (m_sendB);
}

void BeaconingProtocol::start(){
	Connect();
	Simulator::ScheduleNow (&BeaconingProtocol::ScheduleBeaconSending, this);
}


void 
BeaconingProtocol::SetNode (Ptr<Node> node/*, NodeContainer ues*/)
{
	m_node = node;

	nodeList.push_back(node);

}

void
BeaconingProtocol::CancelSchedules()
{
	Simulator::Cancel (m_sendBEvent);
	Simulator::Cancel (m_sendB);
}

void
BeaconingProtocol::NotifyNewAggregate ()
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
BeaconingProtocol::ScheduleBeaconSending ()
{
	double timeb;

	timeb = msec->GetValue ();
	m_sendB = Simulator::Schedule(MicroSeconds (timeb),
			&BeaconingProtocol::SendBeacon, this);
	m_sendBEvent = Simulator::Schedule (MilliSeconds (BeaconingProtocol::BEACON_INTERVAL),
			&BeaconingProtocol::ScheduleBeaconSending, this);
}

void BeaconingProtocol::ShowAvailable(){

}

void BeaconingProtocol::Connect(){

	Address remoteAddress = InetSocketAddress (Ipv4Address("225.0.0.0") , 8000);
	Address localAddress = InetSocketAddress (Ipv4Address::GetAny (), 8000);

	if (!m_socket)
	{
		m_socket = Socket::CreateSocket (m_node, UdpSocketFactory::GetTypeId ());
		if (InetSocketAddress::IsMatchingType (remoteAddress) ||
				PacketSocketAddress::IsMatchingType (remoteAddress))
		{
			if (m_socket->Bind () == -1)
			{
				NS_FATAL_ERROR ("Failed to bind socket");
			}
		}
		m_socket->Connect (remoteAddress);
		m_socket->SetAllowBroadcast (true);
		m_socket->ShutdownRecv ();
		m_connected=true;
	}

	if (!m_socket_sink)
	{
		m_socket_sink = Socket::CreateSocket (m_node, UdpSocketFactory::GetTypeId ());
		if (m_socket_sink->Bind (localAddress) == -1)
		{
			NS_FATAL_ERROR ("Failed to bind socket");
		}
		m_socket_sink->Listen ();
		m_socket_sink->ShutdownSend ();
		if (addressUtils::IsMulticast (localAddress))
		{
			Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket_sink);
			if (udpSocket)
			{
				// equivalent to setsockopt (MCAST_JOIN_GROUP)
				udpSocket->MulticastJoinGroup (0, localAddress);
			}
			else
			{
				NS_FATAL_ERROR ("Error: joining multicast on a non-UDP socket");
			}
		}
	}

	m_socket_sink->SetRecvCallback (MakeCallback (&BeaconingProtocol::ReceiveBeacon, this));
	m_socket_sink->SetAcceptCallback (
			MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
			MakeCallback (&BeaconingProtocol::HandleAccept, this));
}


void
BeaconingProtocol::ReceiveBeacon (Ptr<Socket> socket)
{
	Ptr<Packet> packet;
	Address from;
	Address localAddress;
	double dist = 0;
	while ((packet = socket->RecvFrom (from)))
	{
		if (packet->GetSize () == 0)
		{
			std::cout<<"communicatioın dropped because of packet size < 0 \n\r"<<std::endl;
			break;
		}
		socket->GetSockName (localAddress);
		BeaconingProtocolHeader h;
		packet->RemoveHeader(h);
		m_node->GetObject<LocationTable>()->AddPosEntry(h);

		//if( Simulator::Now ().GetSeconds()>30){
			if(h.GetId()==1)
			{
				dist = getDistance(h, m_node);
				if(dist <= distLevel)
					ctr_totRxC5inDist ++;

				*log_rxtx->GetStream() << Simulator::Now ().GetMilliSeconds()<< ";  node" <<m_node->GetId()<<";received beacon from "<<h.GetId()<<";TimeStamp: "<<h.GetTimeStamp() << ";distance " << dist <<";" <<(Simulator::Now ().GetNanoSeconds()- h.GetTimeStamp())/1000000<<std::endl;
				ctr_totRxC5++;

				//add comm dev list
				//find individual packet on that list if concurrent item remove it
				//return number of item in list
			}
			if(m_node->GetId()==1)
			{
				dist = getDistance(h, m_node);
				if(dist <= distLevel)
					ctr_totRx5inDist ++;

				*log_rxtx->GetStream() << Simulator::Now ().GetNanoSeconds () << ";  node" <<m_node->GetId()<<";received beacon from "<<h.GetId()<<";TimeStamp: "<<h.GetTimeStamp() << ";distance" << dist <<" ;" << (Simulator::Now ().GetNanoSeconds()- h.GetTimeStamp())/1000000<<std::endl;
				ctr_totRx5++;

				tId = h.GetId();
				commList.push_back(tId);
			}

			if(h.GetId()==2)
			{
				dist = getDistance(h, m_node);
				if(dist <= distLevel)
					ctr_totRxC22inDist ++;

				*log_rxtx->GetStream() << Simulator::Now ().GetMilliSeconds()<< ";  node" <<m_node->GetId()<<";received beacon from "<<h.GetId()<<";TimeStamp: "<<h.GetTimeStamp() << ";distance " << dist <<" ;" <<(Simulator::Now ().GetNanoSeconds()- h.GetTimeStamp())/1000000<<std::endl;
				ctr_totRxC22++;

				//add comm dev list
				//find individual packet on that list if concurrent item remove it
				//return number of item in list
			}
			if(m_node->GetId()==2)
			{
				dist = getDistance(h, m_node);
				if(dist <= distLevel)
					ctr_totRx22inDist ++;

				*log_rxtx->GetStream() << Simulator::Now ().GetNanoSeconds () << ";  node" <<m_node->GetId()<<";received beacon from "<<h.GetId()<<";TimeStamp: "<<h.GetTimeStamp() << ";distance " << dist <<" ;" << (Simulator::Now ().GetNanoSeconds()- h.GetTimeStamp())/1000000<<std::endl;
				ctr_totRx22++;

				tId2 = h.GetId();
				commList2.push_back(tId2);
			}

			if(h.GetId()==4)
			{
				dist = getDistance(h, m_node);
				if(dist <= distLevel)
					ctr_totRxC49inDist ++;

				*log_rxtx->GetStream() << Simulator::Now ().GetMilliSeconds()<< ";  node" <<m_node->GetId()<<";received beacon from "<<h.GetId()<<";TimeStamp: "<<h.GetTimeStamp() << ";distance" << dist <<" ;" <<(Simulator::Now ().GetNanoSeconds()- h.GetTimeStamp())/1000000<<std::endl;
				ctr_totRxC49++;

				//add comm dev list
				//find individual packet on that list if concurrent item remove it
				//return number of item in list
			}
			if(m_node->GetId()==4)
			{
				dist = getDistance(h, m_node);
				if(dist <= distLevel)
					ctr_totRx49inDist ++;

				*log_rxtx->GetStream() << Simulator::Now ().GetNanoSeconds () << ";  node" <<m_node->GetId()<<";received beacon from "<<h.GetId()<<";TimeStamp: "<<h.GetTimeStamp() << ";distance" << dist <<" ;" << (Simulator::Now ().GetNanoSeconds()- h.GetTimeStamp())/1000000<<std::endl;
				ctr_totRx49++;

				tId3 = h.GetId();
				commList3.push_back(tId3);
			}


			ctr_totRx++;
		//}
	}
}

void BeaconingProtocol::HandleAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_FUNCTION (this << s << from);
  s->SetRecvCallback (MakeCallback (&BeaconingProtocol::ReceiveBeacon, this));
  m_socketList.push_back (s);
}



void
BeaconingProtocol::SendBeacon ()
{

	//std::vector<ptime> m = m_node->GetObject<LocationTable>()->GetFreshNb();
	Ptr<Packet> packet = Create<Packet> (0);

	BeaconingProtocolHeader h;
	h.SetId(m_node->GetId());
	h.SetTimeStamp(Simulator::Now().GetNanoSeconds());
	h.SetLatitude(m_node->GetObject<SumoMobilityModel>()->GetLatitude());
	h.SetLongitude(m_node->GetObject<SumoMobilityModel>()->GetLongitude());
	h.SetSpeed(m_node->GetObject<SumoMobilityModel>()->GetSpeed());
	h.SetHeading(m_node->GetObject<SumoMobilityModel>()->GetHeading());
	h.SetPosIndex(m_node->GetObject<LocationTable>()->m_cur_posindex);
	//std::cout<<"bEACON SENDED!"<<std::endl;

	//if( Simulator::Now ().GetSeconds()>30)
	//{
		if(h.GetId()==5)
		{
			*log_rxtx->GetStream() << Simulator::Now ().GetSeconds () << ";"<<"BEACON SENDED! with ID: "<<";"<< m_node->GetId() << std::endl;
			ctr_totTx5++;
		}

		if(h.GetId()==22)
		{
			*log_rxtx->GetStream() << Simulator::Now ().GetSeconds () << ";"<<"BEACON SENDED! with ID: "<<";"<< m_node->GetId() << std::endl;
			ctr_totTx22++;
		}

		if(h.GetId()==49)
		{
			*log_rxtx->GetStream() << Simulator::Now ().GetSeconds () << ";"<<"BEACON SENDED! with ID: "<<";"<< m_node->GetId() << std::endl;
			ctr_totTx49++;
		}

		ctr_totTx++;
	//}


		//TODO:mert
		//uint32_t simTime = Simulator::Now().GetMilliSeconds();
		//Ptr<MobilityModel> posMobility = m_node->GetObject<MobilityModel>();
		//Vector posTx = posMobility->GetPosition();

//
//		// check for each UE distance to transmitter
//		for (uint32_t i = 0 ; i < 370 ; i++)
//		{
//			if(nodeList.at(i)->GetId() != m_node->GetId()){
//				Ptr<MobilityModel> mob = nodeList.at(i)->GetObject<MobilityModel>();
//				Vector posRx = mob->GetPosition();
//
//				double distance = sqrt(pow((posTx.x - posRx.x),2.0)+pow((posTx.y - posRx.y),2.0));
//				if  (distance <= distLevel)
//				{
//					ctr_totRealTx++;
//
//					if(m_node->GetId() == 5)
//						ctr_totTx5inDist ++;
//					if(m_node->GetId() == 22)
//						ctr_totTx22inDist ++;
//					if(m_node->GetId() == 49)
//						ctr_totTx49inDist ++;
//				}
//			}
//		}

	packet->AddHeader(h);

	if (m_connected != 0)
	{
		m_socket->Send (packet);
	}


}

} //namespace
