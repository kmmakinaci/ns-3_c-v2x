

#ifndef SUMO_MOBILITY_MODEL_H
#define SUMO_MOBILITY_MODEL_H

#include <vector>
#include <ns3/random-variable-stream.h>
#include <ns3/ptr.h>
#include <ns3/nstime.h>
#include <ns3/mobility-model.h>
namespace ns3 {
class SumoMobilityModel : public MobilityModel
{
public:
  static TypeId GetTypeId (void);
  SumoMobilityModel ();
  void SetPositionAndSpeed (const float &latitude, const float &longitude, const float &speed, const float & heading, const std::string &edgeid, const std::string &laneid,int acc);
  void SetPositionAndSpeed (const Vector &position, const float &speed, const float &heading, const std::string &edgeid, const std::string &laneid,int acc); // Vector position in (x,y,z) coordinates
  void SetPosition (const float &latitude, const float &longitude);
  void SetPosition (const Vector &position);
  void SetGeodesicPosition (const std::vector<float> &position);
  void SetErrorValues(float errorratevertical,float errorratehorizontal);
  float GetHorizontalErrorValue();
  float GetVerticalErrorValue();
  std::vector<uint32_t> GetGeodesicPosition (void) const;
  std::vector<float> GetGeodesicPosition0 (void) const;
  void UpdateGeodedesicPosition (void);
  uint32_t GetLatitude (void) const;
  uint32_t GetLongitude (void) const;
  uint32_t GetSpeed (void) const;
  uint16_t GetHeading (void) const;
  double GetLatitudeO (void) const;
  double GetLongitudeO (void) const;
  double GetSpeedO (void) const;
  double GetHeadingO (void) const;
  int GetAcc(void) const;
  std::string GetEdgeId (void) const;
  std::string GetLaneId (void) const;

  //gps extension
  void SetGpsEnabled(bool gpsenabled);
  bool GetGpsEnabled();
  void SetOdometryEnabled(bool odometryenabled);
  bool GetOdometryEnabled();

private:
  virtual void DoSetPosition (const float &latitude, const float &longitude);
  virtual void DoSetPosition (const Vector &position);
  virtual Vector DoGetPosition (void) const;
  virtual Vector DoGetVelocity (void) const;

  mutable Time m_lastUpdate;
  mutable Vector m_position;

  float errorratevertical;
  float errorratehorizontal;
  int m_acc;

  bool m_gpsenabled;
  bool m_odometryenabled;
  uint32_t m_latitude;
  uint32_t m_longitude;
  uint16_t m_speed;
  uint16_t m_heading;

  double m_latitudeO;
  double m_longitudeO;
  double m_speedO;
  double m_headingO;
  Ptr<NormalRandomVariable> rand;
  Ptr<UniformRandomVariable> unirand;

  std::string m_edgeId;
  std::string m_laneId;
};

} // namespace ns3

#endif
 
