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

#ifndef LOCATION_TABLE_H
#define LOCATION_TABLE_H

#include <vector>
#include <algorithm>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/node.h>
#include <ns3/log.h>
#include <ns3/event-id.h>
#include <ns3/random-variable-stream.h>
#include "beaconing-protocol-header.h"

namespace ns3 {

typedef struct ptime{
	uint32_t gnAddr;
	uint32_t Ts;
}ptime;

class LocationTable : public Object
{

public:

	static const float LOCATION_ENTRY_LIFETIME;

	static TypeId GetTypeId (void);
	LocationTable ();
	~LocationTable ();
	void DoDispose (void);

	struct LocTableEntry
	{
		uint64_t gnAddr;
		uint64_t Ts;
		uint32_t Lat;
		uint32_t Long;
		uint32_t Speed;
		uint16_t Heading;
		uint16_t PosIndex;
		uint64_t arrival_ts;
		double dist;

		bool operator < (const LocTableEntry& str) const
		{
			return (dist < str.dist);
		}
		bool operator > (const LocTableEntry& str) const
		{
			return (dist > str.dist);
		}
	};

	typedef std::vector<struct LocTableEntry> Table;
	Table m_Table;

	/**
	 * Add an entry to the location table when receiving a new beacon
	 */
	void AddPosEntry (BeaconingProtocolHeader h);
	uint16_t CalculateAndGetPosindex();
	// void AddPosEntry ();
	double CartesianDistance (double lat2, double lon2, double lat1, double lon1);
	std::vector<ptime> GetFreshNb();
	float GetLinkQuality ();
	/**
	 * Add an entry to the location table with the own data of the node
	 */
	void AddOwnEntry();

	/**
	 * Clean the location table
	 */
	void CleanTable();

	Table GetTable();
	int GetNbNeighs();
	int getEntry (unsigned int gnAddr);
	void SetNode (Ptr<Node> node);
	void NotifyNewAggregate ();
	void Print();
	Ptr<Node> m_node;
	uint16_t m_cur_posindex;
	uint16_t max_pos_index;

private:

	EventId m_posEvent;
	EventId m_updateEvent;

	BeaconingProtocolHeader m_cur_entry;
	Ptr<UniformRandomVariable> rednr;

	//for calcposindex //*(10*0.1*vec[i].dist); //FROM THE PAPER PIt=argmaxPIU∑u=1Φ(PI,PIu+u)·(μ+η·dt,u)
	double m_mu;
	double m_n;

	bool first;

	void ScheduleCleanTable();
	void ScheduleUpdatePos();
};

}; //namespace ns3

#endif /* LOCATION_TABLE_H */
