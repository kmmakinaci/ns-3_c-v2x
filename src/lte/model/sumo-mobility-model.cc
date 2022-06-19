/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, Uwicore Laboratory UMH, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>, Ramon Bauza <rbauza@umh.es>
 */

#include<stdio.h>
#include <ns3/node-list.h>
#include <ns3/node.h>
#include <ns3/double.h>
#include "sumo-mobility-model.h"

#define SUMO_NEGATIF_OFFSET 10000

using namespace std;
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SumoMobilityModel);

TypeId
SumoMobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SumoMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<SumoMobilityModel> ()
  ;
  return tid;
}

SumoMobilityModel::SumoMobilityModel ()
{
	  m_latitudeO = 0;
	  m_longitudeO = 0;
	  m_speedO = 0;
	  m_headingO = 0;
	  m_latitude = (uint32_t) (m_latitudeO * 1000000 * 10);
	  m_longitude = (uint32_t) (m_longitudeO * 1000000 * 10);
	  m_speed = (uint16_t) (m_speedO * 100);
	  m_heading = (uint16_t) (m_headingO * 10);
	  m_edgeId = "";
	  m_laneId = "";
	  errorratevertical=0;
	  errorratehorizontal=0;
	  m_gpsenabled=true;
	  m_odometryenabled=false;
	  rand =  CreateObject<NormalRandomVariable> ();
	  rand->SetAttribute ("Mean", DoubleValue (0));
	  rand->SetAttribute ("Variance", DoubleValue (10));
	  unirand = CreateObject<UniformRandomVariable>();
}

void SumoMobilityModel::SetErrorValues(float errorratevertical,float errorratehorizontal){
	  this->errorratevertical=errorratevertical;
	  this->errorratehorizontal=errorratehorizontal;
}
float SumoMobilityModel::GetHorizontalErrorValue(){
	return errorratehorizontal;
}
float SumoMobilityModel::GetVerticalErrorValue(){
	return errorratevertical;
}

void SumoMobilityModel::SetGpsEnabled(bool gpsenabled){
	m_gpsenabled=gpsenabled;
}
bool SumoMobilityModel::GetGpsEnabled(){
	return m_gpsenabled;
}
void SumoMobilityModel::SetOdometryEnabled(bool odometryenabled){
	m_odometryenabled=odometryenabled;
}
bool SumoMobilityModel::GetOdometryEnabled(){
	return m_odometryenabled;
}

void SumoMobilityModel::SetPositionAndSpeed (const Vector &position, const float &speed, const float &heading, const std::string &edgeid, const std::string &laneid,int acc)
{
   m_position = position;
   if(m_gpsenabled){
	   m_speedO = speed;
	   m_headingO = heading;
	   m_edgeId = edgeid;
	   m_laneId = laneid;
	   //FIXED CIRCLE
	   double error = errorratehorizontal*rand->GetValue();
	   double angle = unirand->GetValue(0,M_PI/4);
	   m_latitudeO = /*2904231.6*/SUMO_NEGATIF_OFFSET+m_position.y + error*sin(angle);
	   m_longitudeO = /*4098607.9*/SUMO_NEGATIF_OFFSET+m_position.x + error*cos(angle);
	   m_speedO = speed;
	   m_headingO = heading;
	   m_latitude = (uint32_t) (m_latitudeO*100);
	   m_longitude = (uint32_t) (m_longitudeO*100);
	   m_speed = (uint16_t) (m_speedO*100);
	   m_heading = (uint16_t) (m_headingO);
   	   m_acc=acc;}
   else{
	   m_edgeId = "";
	   m_laneId = "";
	   m_latitudeO = 0;
	   m_longitudeO =0;
	   m_speedO = 0;
	   m_headingO = 0;
	   m_latitude = 0;
	   m_longitude = 0;
	   if(m_odometryenabled){
		   m_speed = speed;
		   m_speedO = speed;
	   }
	   else{
		   m_speed = 0;
		   m_speedO = 0;
	   }
	   m_headingO = 0;
	   m_heading = 0;
	   acc=0;
   }
	NotifyCourseChange ();
}

void
SumoMobilityModel::SetPosition (const float &latitude, const float &longitude)
{
  DoSetPosition (latitude, longitude);
}

void
SumoMobilityModel::SetPosition (const Vector &position)
{
  DoSetPosition (position);
}

uint32_t 
SumoMobilityModel::GetLongitude (void) const
{
  return m_longitude;
}

uint32_t 
SumoMobilityModel::GetLatitude (void) const
{
  return m_latitude;
}

uint32_t 
SumoMobilityModel::GetSpeed (void) const
{
  return m_speed;
}

int SumoMobilityModel::GetAcc(void) const{
	return m_acc;
}
uint16_t
SumoMobilityModel::GetHeading (void) const
{
  return m_heading;
}

double
SumoMobilityModel::GetLongitudeO (void) const
{
  return m_longitudeO;
}

double
SumoMobilityModel::GetLatitudeO (void) const
{
  return m_latitudeO;
}

double
SumoMobilityModel::GetSpeedO (void) const
{
  return m_speedO;
}

double
SumoMobilityModel::GetHeadingO (void) const
{
  return m_headingO;
}

void 
SumoMobilityModel::DoSetPosition (const float &latitude, const float &longitude)
{
	  m_latitudeO = latitude;
	  m_longitudeO = longitude;
	  m_latitude = (uint32_t) (m_latitudeO * 100);
	  m_longitude = (uint32_t) (m_longitudeO * 100);
	//  UpdateGeodecentricPosition ();
}

inline Vector
SumoMobilityModel::DoGetPosition (void) const
{
  return m_position;
}

void 
SumoMobilityModel::DoSetPosition (const Vector &position)
{
	  m_position = position;
	  UpdateGeodedesicPosition ();
}

inline Vector
SumoMobilityModel::DoGetVelocity (void) const
{
  return Vector(m_speed,0,0);
}

std::vector<uint32_t> 
SumoMobilityModel::GetGeodesicPosition (void) const
{

	 std::vector<uint32_t> geodesicPos;
	 geodesicPos.push_back(m_latitude);
	 geodesicPos.push_back(m_longitude);
	 return geodesicPos;
}

std::vector<float> 
SumoMobilityModel::GetGeodesicPosition0 (void) const
{
	  std::vector<float> geodesicPos;
	  geodesicPos.push_back(m_latitudeO);
	  geodesicPos.push_back(m_longitudeO);
	  return geodesicPos;
}

std::string
SumoMobilityModel::GetEdgeId (void) const
{
  return m_edgeId;
}

std::string 
SumoMobilityModel::GetLaneId (void) const
{
  return m_laneId;
}

/*void
SumoMobilityModel::UpdateGeodecentricPosition (void)
{
  std::vector <double> geocentricPos = GeoToLocConvert (m_latitudeO, m_longitudeO, 0.0);
  m_position.x = geocentricPos[0];
  m_position.y = geocentricPos[1];
  m_position.z = geocentricPos[2];
}
 */
void SumoMobilityModel::UpdateGeodedesicPosition (void)
{
  m_latitudeO = SUMO_NEGATIF_OFFSET+m_position.y;
  m_longitudeO = SUMO_NEGATIF_OFFSET+m_position.x;
  m_latitude = (uint32_t) (m_latitudeO * 100);
  m_longitude = (uint32_t) (m_longitudeO * 100);}


} // namespace ns3
 
