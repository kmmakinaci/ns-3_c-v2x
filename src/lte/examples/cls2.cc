/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */


#include "ns3/lte-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store.h"
#include "ns3/sumo-connection.h"
#include "ns3/sumo-mobility-model.h"
#include "ns3/GuiManager.h"
#include <cfloat>
#include <sstream>
#define UNUSED(x) ((x)=(x))
using namespace ns3;

// This trace will log packet transmissions and receptions from the application
// layer.  The parameter 'localAddrs' is passed to this trace in case the
// address passed by the trace is not set (i.e., is '0.0.0.0' or '::').  The
// trace writes to a file stream provided by the first argument; by default,
// this trace file is 'UePacketTrace.tr'


void CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId)
{
	ObjectFactory factory;
	factory.SetTypeId (typeId);
	Ptr<Object> protocol = factory.Create <Object> ();
	node->AggregateObject (protocol);
}
void InstallBeaconing(NodeContainer c){
	for(uint32_t i=0;i<c.GetN();i++) {
		Ptr<Node> node=c.Get(i);
		CreateAndAggregateObjectFromTypeId (node, "ns3::LocationTable");
		CreateAndAggregateObjectFromTypeId (node, "ns3::BeaconingProtocol");
		DynamicCast<LteUeNetDevice>(node->GetDevice(0))->GetMac()->SetLocationTable(node->GetObject<LocationTable>());
	}
}
void
UePacketTrace (Ptr<OutputStreamWrapper> stream, const Address &localAddrs, std::string context, Ptr<const Packet> p, const Address &srcAddrs, const Address &dstAddrs)
{
	std::ostringstream oss;
	*stream->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << context << "\t" << p->GetSize () << "\t";
	if(context[0]!='t')
		std::cout << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << context << "\t" << p->GetSize () << "\t";

	if (InetSocketAddress::IsMatchingType (srcAddrs))
	{
		oss << InetSocketAddress::ConvertFrom (srcAddrs).GetIpv4 ();
		if (!oss.str ().compare ("0.0.0.0")) //srcAddrs not set
		{
			*stream->GetStream () << Ipv4Address::ConvertFrom (localAddrs) << ":" << InetSocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << InetSocketAddress::ConvertFrom (dstAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
			if(context[0]!='t')
				std::cout << Ipv4Address::ConvertFrom (localAddrs) << ":" << InetSocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << InetSocketAddress::ConvertFrom (dstAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;

		}
		else
		{
			oss.str ("");
			oss << InetSocketAddress::ConvertFrom (dstAddrs).GetIpv4 ();
			if (!oss.str ().compare ("0.0.0.0")) //dstAddrs not set
			{
				*stream->GetStream () << InetSocketAddress::ConvertFrom (srcAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" <<  Ipv4Address::ConvertFrom (localAddrs) << ":" << InetSocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
				if(context[0]!='t')
					std::cout << InetSocketAddress::ConvertFrom (srcAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" <<  Ipv4Address::ConvertFrom (localAddrs) << ":" << InetSocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;

			}
			else
			{
				*stream->GetStream () << InetSocketAddress::ConvertFrom (srcAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << InetSocketAddress::ConvertFrom (dstAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
				if(context[0]!='t')
					std::cout << InetSocketAddress::ConvertFrom (srcAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << InetSocketAddress::ConvertFrom (dstAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;

			}
		}
	}
	else if (Inet6SocketAddress::IsMatchingType (srcAddrs))
	{
		oss << Inet6SocketAddress::ConvertFrom (srcAddrs).GetIpv6 ();
		if (!oss.str ().compare ("::")) //srcAddrs not set
		{
			*stream->GetStream () << Ipv6Address::ConvertFrom (localAddrs) << ":" << Inet6SocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
		}
		else
		{
			oss.str ("");
			oss << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 ();
			if (!oss.str ().compare ("::")) //dstAddrs not set
			{
				*stream->GetStream () << Inet6SocketAddress::ConvertFrom (srcAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << Ipv6Address::ConvertFrom (localAddrs) << ":" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
			}
			else
			{
				*stream->GetStream () << Inet6SocketAddress::ConvertFrom (srcAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
			}
		}
	}
	else
	{
		*stream->GetStream () << "Unknown address type!" << std::endl;
	}
}

/*
 * The topology is the following:
 *
 *          UE1..........(20 m)..........UE2
 *   (0.0, 0.0, 1.5)            (20.0, 0.0, 1.5)
 *
 * Please refer to the Sidelink section of the LTE user documentation for more details.
 *
 */

NS_LOG_COMPONENT_DEFINE ("LteSlOutOfCovrg");

int main (int argc, char *argv[])
{
	uint16_t simTime = 100;                 // Simulation time in seconds
	uint32_t numVeh = 4;                  // Number of vehicles
	double ueTxPower = 23.0;                // Transmission power in dBm
	double probResourceKeep = 0.0;          // Probability to select the previous resource again [0.0-0.8]
	uint32_t mcs = 20;                      // Modulation and Coding Scheme
	bool harqEnabled = false;               // Retransmission enabled
	bool adjacencyPscchPssch = true;        // Subchannelization scheme
	bool partialSensing = false;            // Partial sensing enabled (actual only partialSensing is false supported)
	uint16_t sizeSubchannel = 10;           // Number of RBs per subchannel
	uint16_t numSubchannel = 3;             // Number of subchannels per subframe
	uint16_t startRbSubchannel = 0;         // Index of first RB corresponding to subchannelization
	uint16_t pRsvp = 100;				    // Resource reservation interval
	uint16_t t1 = 4;                        // T1 value of selection window
	uint16_t t2 = 100;                      // T2 value of selection window
	uint16_t slBandwidth;                   // Sidelink bandwidth

	CommandLine cmd;
	cmd.AddValue ("time", "Simulation Time", simTime);
	cmd.AddValue ("numVeh", "Number of Vehicles", numVeh);
	cmd.AddValue ("adjacencyPscchPssch", "Scheme for subchannelization", adjacencyPscchPssch);
	cmd.AddValue ("sizeSubchannel", "Number of RBs per Subchannel", sizeSubchannel);
	cmd.AddValue ("numSubchannel", "Number of Subchannels", numSubchannel);
	cmd.AddValue ("startRbSubchannel", "Index of first subchannel index", startRbSubchannel);
	cmd.AddValue ("T1", "T1 Value of Selection Window", t1);
	cmd.AddValue ("T2", "T2 Value of Selection Window", t2);
	//cmd.AddValue ("harqEnabled", "HARQ Retransmission Enabled", harqEnabled);
	//cmd.AddValue ("partialSensingEnabled", "Partial Sensing Enabled", partialSensing);

	cmd.AddValue ("mcs", "Modulation and Coding Scheme", mcs);
	cmd.AddValue ("pRsvp", "Resource Reservation Interval", pRsvp);
	cmd.AddValue ("probResourceKeep", "Probability for selecting previous resource again", probResourceKeep);
	cmd.Parse (argc, argv);

	//Configure the UE for UE_SELECTED scenario
	Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (ueTxPower));
	Config::SetDefault ("ns3::LteUePhy::RsrpUeMeasThreshold", DoubleValue (-10.0));
	// Enable V2X communication on PHY layer
	Config::SetDefault ("ns3::LteUePhy::EnableV2x", BooleanValue (true));

	// Set power
	Config::SetDefault ("ns3::LteUePowerControl::Pcmax", DoubleValue (ueTxPower));
	Config::SetDefault ("ns3::LteUePowerControl::PsschTxPower", DoubleValue (ueTxPower));
	Config::SetDefault ("ns3::LteUePowerControl::PscchTxPower", DoubleValue (ueTxPower));

	if (adjacencyPscchPssch)
	{
		slBandwidth = sizeSubchannel * numSubchannel;
	}
	else
	{
		slBandwidth = (sizeSubchannel+2) * numSubchannel;
	}

	// Configure for UE selected
	Config::SetDefault ("ns3::LteUeMac::UlBandwidth", UintegerValue(slBandwidth));
	Config::SetDefault ("ns3::LteUeMac::EnableV2xHarq", BooleanValue(harqEnabled));
	Config::SetDefault ("ns3::LteUeMac::EnableAdjacencyPscchPssch", BooleanValue(adjacencyPscchPssch));
	Config::SetDefault ("ns3::LteUeMac::EnablePartialSensing", BooleanValue(partialSensing));
	Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue(mcs));
	Config::SetDefault ("ns3::LteUeMac::SlSubchannelSize", UintegerValue (sizeSubchannel));
	Config::SetDefault ("ns3::LteUeMac::SlSubchannelNum", UintegerValue (numSubchannel));
	Config::SetDefault ("ns3::LteUeMac::SlStartRbSubchannel", UintegerValue (startRbSubchannel));
	Config::SetDefault ("ns3::LteUeMac::SlPrsvp", UintegerValue(pRsvp));
	Config::SetDefault ("ns3::LteUeMac::SlProbResourceKeep", DoubleValue(probResourceKeep));
	Config::SetDefault ("ns3::LteUeMac::SelectionWindowT1", UintegerValue(t1));
	Config::SetDefault ("ns3::LteUeMac::SelectionWindowT2", UintegerValue(t2));
	Config::SetDefault ("ns3::LteUeMac::EnableGeoScheduling", BooleanValue(true));
	Config::SetDefault ("ns3::LteUeMac::GeoSchPpsValue", UintegerValue(10));

	ConfigStore inputConfig;
	inputConfig.ConfigureDefaults();

	//Sidelink bearers activation time
	Time slBearersActivationTime = Seconds (2.0);

	//Create the helpers
	Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
	Ptr<Node> pgw = epcHelper->GetPgwNode();

	// LTE Helper
	Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
	lteHelper->SetEpcHelper(epcHelper);
	lteHelper->DisableNewEnbPhy();

	  Ptr<LteV2xHelper> lteV2xHelper = CreateObject<LteV2xHelper> ();
	    lteV2xHelper->SetLteHelper (lteHelper);
	//Enable Sidelink
	lteHelper->SetEnbAntennaModelType ("ns3::NistParabolic3dAntennaModel");
    //
    lteHelper->SetAttribute ("UseSameUlDlPropagationCondition", BooleanValue(true));
    Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", StringValue ("54990"));
    //Config::SetDefault ("ns3::CniUrbanmicrocellPropagationLossModel::Frequency", DoubleValue(5800e6));
    lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Cost231PropagationLossModel"));
    //lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::NakagamiPropagationLossModel"));
    //lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::FriisPropagationLossModel"));

	lteHelper->Initialize();
	NodeContainer ueAllNodes;


	// Create nodes
	ueAllNodes.Create (numVeh);
	std::cout<<ueAllNodes.Get(0)->GetId()<<std::endl;
	// Install constant random positions
	MobilityHelper mobVeh;
	mobVeh.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	for(unsigned int i=0;i<numVeh;i++){
		positionAlloc->Add(Vector (10000, 0.0, 0.0));
	}
	mobility.SetPositionAllocator (positionAlloc);
	mobility.SetMobilityModel ("ns3::SumoMobilityModel");
	mobility.Install (ueAllNodes);

	NS_LOG_INFO ("Installing UE's network devices...");
	lteHelper->SetAttribute("UseSidelink", BooleanValue (true));
	NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueAllNodes);

	// Install the IP stack on the UEs
	NS_LOG_INFO ("Installing IP stack...");
	InternetStackHelper internet;
	internet.Install (ueAllNodes);

	// Assign IP adress to UEs
	NS_LOG_INFO ("Allocating IP addresses and setting up network route...");
	Ipv4InterfaceContainer ueIpIface;
	ueIpIface = epcHelper->AssignUeIpv4Address (ueDevs);
	Ipv4StaticRoutingHelper Ipv4RoutingHelper;

	for(uint32_t u = 0; u < ueAllNodes.GetN(); ++u)
	{
		Ptr<Node> ueNode = ueAllNodes.Get(u);
		// Set the default gateway for the UE
		Ptr<Ipv4StaticRouting> ueStaticRouting = Ipv4RoutingHelper.GetStaticRouting(ueNode->GetObject<Ipv4>());
		ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress(), 1);
	}


	uint32_t groupL2Address = 255;
	Ipv4Address groupAddress4 ("225.0.0.0");     //use multicast address as destination
	Ipv6Address groupAddress6 ("ff0e::1");     //use multicast address as destination
	Address remoteAddress;
	Address localAddress;
	Ptr<LteSlTft> tft;


	remoteAddress = InetSocketAddress (groupAddress4, 8000);
	localAddress = InetSocketAddress (Ipv4Address::GetAny (), 8000);
	tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, groupAddress4, groupL2Address);
	lteV2xHelper->ActivateSidelinkBearer (Seconds(0.1),ueDevs , tft);


	Ptr<LteUeRrcSl> ueSidelinkConfiguration = CreateObject<LteUeRrcSl>();
	ueSidelinkConfiguration->SetSlEnabled(true);
	ueSidelinkConfiguration->SetV2xEnabled(true);

	LteRrcSap::SlV2xPreconfiguration preconfiguration;
	preconfiguration.v2xPreconfigFreqList.freq[0].v2xCommPreconfigGeneral.carrierFreq = 54890;
	preconfiguration.v2xPreconfigFreqList.freq[0].v2xCommPreconfigGeneral.slBandwidth = slBandwidth;

	preconfiguration.v2xPreconfigFreqList.freq[0].v2xCommTxPoolList.nbPools = 1;
	preconfiguration.v2xPreconfigFreqList.freq[0].v2xCommRxPoolList.nbPools = 1;

	SlV2xPreconfigPoolFactory pFactory;
	pFactory.SetHaveUeSelectedResourceConfig (true);
	pFactory.SetSlSubframe (std::bitset<20> (0xFFFFF));
	pFactory.SetAdjacencyPscchPssch (adjacencyPscchPssch);
	pFactory.SetSizeSubchannel (sizeSubchannel);
	pFactory.SetNumSubchannel (numSubchannel);
	pFactory.SetStartRbSubchannel (startRbSubchannel);
	pFactory.SetStartRbPscchPool (0);
	pFactory.SetDataTxP0 (-4);
	pFactory.SetDataTxAlpha (0.9);

	preconfiguration.v2xPreconfigFreqList.freq[0].v2xCommTxPoolList.pools[0] = pFactory.CreatePool ();
	preconfiguration.v2xPreconfigFreqList.freq[0].v2xCommRxPoolList.pools[0] = pFactory.CreatePool ();
	ueSidelinkConfiguration->SetSlV2xPreconfiguration (preconfiguration);
	lteHelper->InstallSidelinkV2xConfiguration (ueDevs, ueSidelinkConfiguration);

	InstallBeaconing(ueAllNodes);
	//Set Sidelink bearers
	///*** End of application configuration ***///
	//Trace file table header

	SumoConnection *sumoCon=NULL;
	GuiManager *m=new GuiManager();
	UNUSED(sumoCon);
	sumoCon = new SumoConnection("sumo-gui -c /home/mert/repos/vanet/highway_s3/hghway.sumo.cfg&",ueAllNodes,ueDevs,numVeh,0.01f,m);
	//sumoCon = new SumoConnection("sumo-gui -c /home/mert/repos/vanet/manhattan/manh.sumo.cfg&",ueAllNodes,ueDevs,numVeh,0.01f,m);
	//sumoCon = new SumoConnection("sumo-gui -c /home/mert/repos/vanet/ornek_50l/grid.sumo.cfg&",ueAllNodes,ueDevs,numVeh,0.01f,m);

	m->start();
	NS_LOG_INFO ("Enabling Sidelink traces...");

    NS_LOG_INFO ("Enabling LTE traces...");
    //lteHelper->EnableTraces();
	//lteHelper->EnableSlPscchMacTraces ();j
	//lteHelper->EnableSlPsschMacTraces ();

	lteHelper->EnableSlRxPhyTraces ();
	lteHelper->EnableSlPscchRxPhyTraces ();

	lteHelper->EnablePhyTraces ();
	lteHelper->EnableMacTraces ();
	lteHelper->EnableRlcTraces ();
	lteHelper->EnablePdcpTraces ();


	NS_LOG_INFO ("Starting simulation...");

    Simulator::Stop(MilliSeconds(simTime*1000+40));

	Simulator::Run ();
	Simulator::Destroy ();
	return 0;

}
