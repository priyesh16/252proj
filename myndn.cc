/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 University of California, Los Angeles
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
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */
// ndn-simple.cc
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"
#include "myndn.h"

using namespace ns3;

/**
 * This scenario simulates a very simple network topology:
 *
 *
 *      +----------+     1Mbps      +--------+     1Mbps      +----------+
 *      | consumer | <------------> | router | <------------> | producer |
 *      +----------+         10ms   +--------+          10ms  +----------+
 *
 *
 * Consumer requests data from producer with frequency 10 interests per second
 * (interests contain constantly increasing sequence number).
 *
 * For every received interest, producer replies with a data packet, containing
 * 1024 bytes of virtual payload.
 *
 * To run scenario and see what is happening, use the following command:
 *
 *     NS_LOG=ndn.Consumer:ndn.Producer ./waf --run=ndn-simple
 */


#define NODE_CNT 19
#define CONS 7 //node h
#define PROD 16 // node q


Ptr<Node> from;
Ptr<Node> to;

std::vector<std::string> names;
ns3::AnnotatedTopologyReader topologyReader("", 1);
std::vector<NodeInfo> nbrTable(NODE_CNT);
//NodeContainer nodeContainer;

void
fill_names() {
	std::vector<std::string>::const_iterator namesIter;
	std::vector<Ptr<ndn::Name> > prefix;

	names.push_back("/0/2/1"); // a
	names.push_back("/0/1/1/1"); // b
	names.push_back("/0/2"); // c
	names.push_back("/0/3/1"); // d
	names.push_back("/0/2/1/1"); // e
	names.push_back("/0/1/1"); // f
	names.push_back("/0/1/1/1/1"); // g
	names.push_back("/0/1/1/1/2"); // h
	names.push_back("/0/1"); // i
	names.push_back("/0"); // j
	names.push_back("/0/3"); // k
	names.push_back("/0/20/1"); // l
	names.push_back("/0/3/1/1"); // m
	names.push_back("/0/2/1/1/2"); // n
	names.push_back("/0/2/1/1/1"); // o
	names.push_back("/0/3/1/2"); // p
	names.push_back("/0/3/1/1/1"); // q
	names.push_back("/0/2/1/1/2/1"); // r
	names.push_back("/0/3/1/2/1"); // s
	for(namesIter = names.begin(); namesIter != names.end(); namesIter++){
		prefix.push_back(Create<ndn::Name> (*namesIter)); // another way to create name
	}
}
void
fill_twoHopNbrInfo() {
	std::list<NodeInfo * > oneHopNodeInfoList;
	std::list<NodeInfo *>::const_iterator oneHopInfoListIter;
	std::list<Ptr<Node> > oneHopList;
	std::list<Ptr<Node> >::const_iterator oneHopListIter;
	int j;
	int i;

	for(i = 0; i != NODE_CNT; i++ ) {
		  oneHopList = nbrTable[i].oneHopList;
		  for(oneHopListIter = oneHopList.begin() ; oneHopListIter != oneHopList.end() ; oneHopListIter++ ) {
			  for (j = 0; j != NODE_CNT; j++) {
				  if (nbrTable[j].node == *oneHopListIter) {
					  nbrTable[i].oneHopNodeInfoList.push_back(&nbrTable[j]);
				  }
			  }
		  }
	}
}

void
fill_nbr_table() {
	int i = 0;
	std::vector<Ptr<Node> >::iterator nodeIter;
	std::vector<std::string>::const_iterator namesIter;
	NodeContainer nodeContainer = NodeContainer::GetGlobal();
	std::list<TopologyReader::Link> links;
	links = topologyReader.GetLinks();
	std::list<TopologyReader::Link>::iterator linkiter;
	std::string nodeName = "";


	namesIter = names.begin();
	for(nodeIter = nodeContainer.begin(); nodeIter != nodeContainer.end(); nodeIter++ ) {
		  nbrTable[i].node = *nodeIter;
		  for(linkiter = links.begin() ; linkiter != links.end() ; linkiter++ ) {
				  TopologyReader::Link linkint = *linkiter;
				  from = linkint.GetFromNode();
				  if (from == *nodeIter) {
					  nodeName = linkint.GetFromNodeName();
					  to = linkint.GetToNode();
					  std::cout << "Pri : " << linkint.GetFromNodeName() << " -> " << linkint.GetToNodeName() << "\n";
					  nbrTable[i].oneHopList.push_back(to);
				  }
		  }
		  nbrTable[i].nodeName = nodeName;
		  nbrTable[i].prefixName = *namesIter;
		  namesIter++;
		  i++;
	}
	fill_twoHopNbrInfo();
}

void
print_nbr_table() {
	std::list<NodeInfo * > oneHopNodeInfoList;
	std::list<NodeInfo *>::const_iterator oneHopInfoListIter;
	std::list<Ptr<Node> > oneHopList;
	std::list<Ptr<Node> >::const_iterator oneHopListIter;
	std::list<NodeInfo *> twoHopList;
	std::list<NodeInfo *>::const_iterator twoHopListIter;
	Ptr<Node> oneHopNbr;
	Ptr<Node> twoHopNbr;
	std::string sourceName;
	std::string oneHopNbrName;
	std::string twoHopNbrName;
	int i;

	for(i = 0; i != NODE_CNT; i++ ) {
		sourceName = nbrTable[i].nodeName;
		oneHopNodeInfoList = nbrTable[i].oneHopNodeInfoList;
		for(oneHopInfoListIter = oneHopNodeInfoList.begin() ; oneHopInfoListIter != oneHopNodeInfoList.end() ; oneHopInfoListIter++ ) {
			oneHopNbr = (*oneHopInfoListIter)->node;
			oneHopNbrName = (*oneHopInfoListIter)->nodeName;
			twoHopList = (*oneHopInfoListIter)->oneHopNodeInfoList;
			for (twoHopListIter = twoHopList.begin(); twoHopListIter != twoHopList.end(); twoHopListIter++) {
				twoHopNbrName = (*twoHopListIter)->nodeName;
				std::cout << "Pri " << sourceName << " -> " <<oneHopNbrName << " -> " << twoHopNbrName <<"\n";
			}
		}
	}
}


int
main (int argc, char *argv[])
{
  std::string prefixstr = "/prefix";


  // Setting default parameters for PointToPoint links and channels
  Config::SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("1Mbps"));
  Config::SetDefault ("ns3::PointToPointChannel::Delay", StringValue ("10ms"));
  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", StringValue ("10"));

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse (argc, argv);

  fill_names();

  topologyReader.SetFileName("scratch/paper_topo.txt");
  topologyReader.Read();

  fill_nbr_table();
  /*

    //nodes.Create (names.size());

*/


  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::LabelRoute");
  ndnHelper.InstallAll ();
  
  // Installing global routing interface on all nodes

  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll ();
  
  NodeContainer nodeContainer = NodeContainer::GetGlobal();

  // Getting containers for the consumer/producer
  Ptr<Node> producer = nodeContainer.Get (PROD);
  for (int j = 0; j < 11; j++) {
   std::cout << "Pri : ID" << nodeContainer.Get (j)->GetId() << " Addresses of node j :" << nodeContainer.Get (j)->GetDevice(0)->GetAddress() <<  "\n";
  }
  print_nbr_table();
  NodeContainer consumerNodes;
  consumerNodes.Add (nodeContainer.Get (CONS));

  ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
  consumerHelper.SetPrefix (prefixstr);
  consumerHelper.SetAttribute ("Frequency", StringValue ("1")); // 10 interests a second
  consumerHelper.Install (consumerNodes);

  ndn::AppHelper producerHelper ("ns3::ndn::Producer");
  producerHelper.SetPrefix (prefixstr);
  producerHelper.SetAttribute ("PayloadSize", StringValue("1024"));
  producerHelper.Install (producer);

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigins (prefixstr, producer);

  // Calculate and install FIBs
 ndn::GlobalRoutingHelper::CalculateRoutes ();

  Simulator::Stop (Seconds (2.0));
  ndn::AppDelayTracer::InstallAll("outfile.txt");
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
