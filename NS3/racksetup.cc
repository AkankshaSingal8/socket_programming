/*
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
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("DataCenterExample");

int main(int argc, char *argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Create 3 nodes: 1 TOR switch and 2 hosts (representing a rack)
    NodeContainer hosts;
    hosts.Create(2); // 2 hosts

    NodeContainer torSwitch;
    torSwitch.Create(1); // 1 TOR switch

    // Log information about the topology
    NS_LOG_INFO("Creating a topology with 1 ToR switch and 2 hosts");
    NS_LOG_INFO("Host 1 connected to ToR switch.");
    NS_LOG_INFO("Host 2 connected to ToR switch.");

    // Create point-to-point links between TOR switch and each host
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Gbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("1ms"));

    NetDeviceContainer devicesHostToTor1, devicesHostToTor2;

    // Install links between the first host and TOR switch
    devicesHostToTor1 = pointToPoint.Install(NodeContainer(hosts.Get(0), torSwitch.Get(0)));
    NS_LOG_INFO("Link established: Host 1 <--> ToR Switch");

    // Install links between the second host and TOR switch
    devicesHostToTor2 = pointToPoint.Install(NodeContainer(hosts.Get(1), torSwitch.Get(0)));
    NS_LOG_INFO("Link established: Host 2 <--> ToR Switch");

    // Install Internet stack on all nodes
    InternetStackHelper stack;
    stack.Install(hosts);
    stack.Install(torSwitch); 

    // Assign IP addresses
    Ipv4AddressHelper address1, address2;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfacesHostToTor1 = address1.Assign(devicesHostToTor1);
    NS_LOG_INFO("Assigned IP to Host 1: " << interfacesHostToTor1.GetAddress(0));

    address2.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfacesHostToTor2 = address2.Assign(devicesHostToTor2);
    NS_LOG_INFO("Assigned IP to Host 2: " << interfacesHostToTor2.GetAddress(0));

    // Set up UDP Echo Server on Host 1
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(hosts.Get(0));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));
    NS_LOG_INFO("UDP Echo Server set up on Host 1");

    // Set up UDP Echo Client on Host 2
    UdpEchoClientHelper echoClient(interfacesHostToTor1.GetAddress(0), 9); // Send to Host 1
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(hosts.Get(1)); // Client on Host 2
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
    NS_LOG_INFO("UDP Echo Client set up on Host 2, sending to Host 1");

    // Run the simulation
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
