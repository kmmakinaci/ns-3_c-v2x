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

#ifndef BEACONING_PROTOCOL_HEADER_H
#define BEACONING_PROTOCOL_HEADER_H

#include <ns3/header.h>
namespace ns3 {


class BeaconingProtocolHeader : public Header
{
public:

	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);
	virtual void Print (std::ostream &os) const;


	void SetId (uint16_t id);
	void SetTimeStamp(uint64_t ts);
	void SetLatitude (uint32_t latitude);
	void SetLongitude(uint32_t longitude);
	void SetSpeed(uint32_t speed);
	void SetHeading(uint16_t heading);
	void SetPosIndex(uint16_t posindex);


	uint16_t GetId(void) const;
	uint64_t GetTimeStamp(void) const;
	uint32_t GetLatitude(void) const;
	uint32_t GetLongitude(void) const;
	uint32_t GetSpeed(void) const;
	uint16_t GetHeading(void) const;
	uint16_t GetPosIndex(void) const;

private:
	uint16_t m_id;
	uint64_t m_ts;
	uint32_t m_latitude;
	uint32_t m_longitude;
	uint32_t m_speed;
	uint16_t m_heading;
	uint16_t m_posindex;

};

}

#endif
