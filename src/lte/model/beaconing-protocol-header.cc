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


#include "beaconing-protocol-header.h"

namespace ns3 {

TypeId
BeaconingProtocolHeader::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::BeaconingProtocolHeader")
    		.SetParent<Header> ()
    		.AddConstructor<BeaconingProtocolHeader> ()
    		;
	return tid;
}

TypeId
BeaconingProtocolHeader::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

uint32_t
BeaconingProtocolHeader::GetSerializedSize (void) const
{
	return 26;
}

void
BeaconingProtocolHeader::Serialize (Buffer::Iterator start) const
{

	Buffer::Iterator i = start;
	i.WriteHtonU16(m_id);
	i.WriteHtonU64(m_ts);
	i.WriteHtonU32(m_latitude);
	i.WriteHtonU32(m_longitude);
	i.WriteHtonU32(m_speed);
	i.WriteHtonU16(m_heading);
	i.WriteHtonU16(m_posindex);
}

uint32_t
BeaconingProtocolHeader::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;
	m_id = i.ReadNtohU16();
	m_ts = i.ReadNtohU64();
	m_latitude = i.ReadNtohU32 ();
	m_longitude = i.ReadNtohU32 ();
	m_speed = i.ReadNtohU32 ();
	m_heading= i.ReadNtohU16();
	m_posindex= i.ReadNtohU16();
	return GetSerializedSize();
}

void
BeaconingProtocolHeader::Print (std::ostream &os) const
{
	os << "****Beacon Header**** "
			<< "Latitude: " << m_latitude << " "
			<< "Longitude: " << m_latitude <<	"Speed: "<<m_speed<<" Heading: "<<m_heading<<" PosIndex: "<<m_posindex;
	;
}



void BeaconingProtocolHeader::SetLatitude (uint32_t latitude){
	m_latitude=latitude;
}

void BeaconingProtocolHeader::SetLongitude(uint32_t longitude){
	m_longitude=longitude;
}
void BeaconingProtocolHeader::SetSpeed(uint32_t speed){
	m_speed=speed;
}

void BeaconingProtocolHeader::SetHeading(uint16_t heading){
	m_heading=heading;
}

void BeaconingProtocolHeader::SetPosIndex(uint16_t posindex){
	m_posindex=posindex;
}

void  BeaconingProtocolHeader::SetId (uint16_t id){
	m_id = id;
}

void BeaconingProtocolHeader::SetTimeStamp(uint64_t ts){
	m_ts=ts;
}


uint16_t BeaconingProtocolHeader::GetId(void) const{
	return m_id;
}

uint64_t BeaconingProtocolHeader::GetTimeStamp (void) const
{
	return m_ts;
}
uint32_t BeaconingProtocolHeader::GetLatitude (void) const
{
	return m_latitude;
}

uint32_t BeaconingProtocolHeader::GetLongitude (void) const
{
	return m_longitude;
}

uint32_t BeaconingProtocolHeader::GetSpeed (void) const
{
	return m_speed;
}

uint16_t BeaconingProtocolHeader::GetHeading(void) const
{
	return m_heading;
}

uint16_t BeaconingProtocolHeader::GetPosIndex(void) const
{
	return m_posindex;
}
}; // namespace ns3
