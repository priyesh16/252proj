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

#include "myndn.h"


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




Ptr<Node> from;
Ptr<Node> to;

std::vector<std::string> names;
ns3::AnnotatedTopologyReader topologyReader("", 1);
std::vector<NodeInfo> nbrTable(NODE_CNT);
//NodeContainer nodeContainer;

ndn::ndnSIM::trie_with_policy< ndn::Name,
					ndn::ndnSIM::smart_pointer_payload_traits<ndn::detail::RegisteredPrefixEntry>,
					ndn::ndnSIM::counting_policy_traits> prefixTrie;

std::vector<Ptr<ndn::Name> > prefix;



void
fill_names() {
	std::vector<std::string>::const_iterator namesIter;
	int i = 0;

	for (i = 0; i < NODE_CNT; i++) {
		//prefixNames[i](prefixNamesArr[i]);
		prefix.push_back(Create<ndn::Name> (prefixNamesArr[i])); // another way to create name
		names.push_back(prefixNamesArr[i]);
		prefixTrie.insert(*(prefix[i]), Create < ndn::detail::RegisteredPrefixEntry > (prefix[i]));
	}
	// Test
	super::iterator item = prefixTrie.find_exact(*(prefix[4]));
	//cout << "Prefix found" << *((item->payload ())->GetPrefix()) << endl;
	ndn::Name n1("/0/2/1/1/3");
	item = prefixTrie.longest_prefix_match(n1);
	//cout << "Longest Prefix found" << *((item->payload ())->GetPrefix()) << endl;
}

void
fill_twoHopNbrInfo() {
	std::list<NodeInfo * > oneHopNodeInfoList;
	std::list<NodeInfo *>::const_iterator oneHopInfoListIter;
	std::list<Ptr<Node> > oneHopList;
	std::list<Ptr<Node> >::const_iterator oneHopListIter;
	Ptr<Node> sourceNode;

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
	std::vector<Ptr<Node> >::iterator nodeIter;
	std::vector<std::string>::const_iterator namesIter;
	NodeContainer nodeContainer = NodeContainer::GetGlobal();
	std::list<TopologyReader::Link> links;
	links = topologyReader.GetLinks();
	std::list<TopologyReader::Link>::iterator linkiter;
	std::string fromName = "";
	std::string toName = "";
	int pos;

	for(linkiter = links.begin() ; linkiter != links.end() ; linkiter++ ) {
		from = (*linkiter).GetFromNode();
		fromName = (*linkiter).GetFromNodeName();
		to = (*linkiter).GetToNode();
		toName = (*linkiter).GetToNodeName();
		//std::cout << "Pri : " << fromName << " -> " << toName << " : " << to->GetId() << "\n";
		pos = from->GetId();
		nbrTable[pos].node = from;
		nbrTable[pos].nodeName = fromName;
		nbrTable[pos].prefixStr = names[pos];
		nbrTable[pos].prefixName = prefix[pos];
		nbrTable[pos].oneHopList.push_back(to);
		//std::cout << "Pri : " << toName << " -> " << fromName << " : " << from->GetId() << "\n";
		pos = to->GetId();
		nbrTable[pos].node = to;
		nbrTable[pos].nodeName = toName;
		nbrTable[pos].prefixStr = names[pos];
		nbrTable[pos].prefixName = prefix[pos];
		nbrTable[pos].oneHopList.push_back(from);
	}
	//std::cout << std::endl;
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
	std::string prefixStr;
	std::string sourceName;
	std::string oneHopNbrName;
	std::string twoHopNbrName;
	int i;


	for(i = 0; i != NODE_CNT; i++ ) {
		sourceName = nbrTable[i].nodeName;
		prefixStr = nbrTable[i].prefixStr;
		oneHopNodeInfoList = nbrTable[i].oneHopNodeInfoList;
		for(oneHopInfoListIter = oneHopNodeInfoList.begin() ; oneHopInfoListIter != oneHopNodeInfoList.end() ; oneHopInfoListIter++ ) {
			oneHopNbr = (*oneHopInfoListIter)->node;
			oneHopNbrName = (*oneHopInfoListIter)->nodeName;
			twoHopList = (*oneHopInfoListIter)->oneHopNodeInfoList;
			for (twoHopListIter = twoHopList.begin(); twoHopListIter != twoHopList.end(); twoHopListIter++) {
				twoHopNbrName = (*twoHopListIter)->nodeName;
				//std::cout << "Pri " << sourceName << " " << prefixStr << " -> " <<oneHopNbrName << " -> " << twoHopNbrName <<"\n";
			}
		}
	}
}


/*
void
CalculateRoutes () {
	std::vector<Ptr<Node> >::iterator nodeIter;
	std::vector<std::string>::const_iterator namesIter;
	NodeContainer nodeContainer = NodeContainer::GetGlobal();
	std::list<TopologyReader::Link> links;
	links = topologyReader.GetLinks();
	std::list<TopologyReader::Link>::iterator linkiter;
	std::string fromName = "";
	std::string toName = "";
	int pos;
	Ptr<ndn::L3Protocol> ndnFromProt;
	Ptr<ndn::Face> fromFace;
	Ptr<ndn::L3Protocol> ndnToProt;
	Ptr<ndn::Face> toFace;

	for(linkiter = links.begin() ; linkiter != links.end() ; linkiter++ ) {
		ndnFromProt = (*linkiter).GetFromNode ()->GetObject<ndn::L3Protocol> ();
		if (ndnFromProt != 0)
	        fromFace = ndnFromProt->GetFaceByNetDevice ((*linkiter).GetFromNetDevice ());
		ndnToProt = (*linkiter).GetToNode ()->GetObject<ndn::L3Protocol> ();
		if (ndnToProt != 0)
	        toFace = ndnToProt->GetFaceByNetDevice ((*linkiter).GetToNetDevice ());



	for (NodeList::Iterator node = NodeList::Begin (); node != NodeList::End (); node++) {
		Ptr<GlobalRouter> source = (*node)->GetObject<GlobalRouter> ();
		if (source == 0)
			continue;

			Ptr<ndn::Fib>  fib  = source->GetObject<ndn::Fib> ();
			NS_LOG_DEBUG (" prefix " << prefix << " reachable via face " << *i->second.get<0> ()
            << " with distance " << i->second.get<1> ()
            << " with delay " << i->second.get<2> ());
			Ptr<ndn::fib::Entry> entry = fib->Add (prefix, i->second.get<0> (), i->second.get<1> ());
			entry->SetRealDelayToProducer (i->second.get<0> (), Seconds (i->second.get<2> ()));
			Ptr<Limits> faceLimits = i->second.get<0> ()->GetObject<Limits> ();
			Ptr<Limits> fibLimits = entry->GetObject<Limits> ();
			if (fibLimits != 0)
				fibLimits->SetLimits (faceLimits->GetMaxRate (), 2 * i->second.get<2> () );
		}
	}
}
*/
void
print_name(ndn::Name &namePrefix) {
	ndn::Name::const_iterator i;
	for (i = namePrefix.begin(); i != namePrefix.end(); i++) {
		cout << "/" << *i;
	}
	cout<< "\n";
}


void
fill_nbrTableTrie() {
	std::list<NodeInfo * > oneHopNodeInfoList;
	std::list<NodeInfo * > oneHopNodeInfoList1;
	std::list<NodeInfo *>::const_iterator oneHopInfoListIter;
	std::list<NodeInfo *>::const_iterator oneHopInfoListIter1;
	std::list<Ptr<Node> > oneHopList;
	std::list<NodeInfo *>::reverse_iterator reviter;
	std::list<Ptr<Node> >::const_iterator oneHopListIter;
	std::list<NodeInfo *> twoHopList;
	std::list<NodeInfo *> twoHopList1;
	std::list<NodeInfo *>::const_iterator twoHopListIter;
	std::list<NodeInfo *>::const_iterator twoHopListIter1;
	Ptr<Node> oneHopNbr;
	Ptr<Node> twoHopNbr;
	std::string prefixStr;
	std::string sourceName;
	std::string oneHopNbrName;
	std::string oneHopNbrPrefix;
	std::string twoHopNbrStr;
	std::string twoHopNbrPreStr;
	ndn::Name foundPrefStr;
	Ptr<ndn::Name> twoHopNbrName;
	std::vector<std::string>::const_iterator namesIter;
	int i = 0;
	std::vector<Ptr<ndn::Name> > tmpPrefix;
	super::iterator item;
	std::string destPrefix = nbrTable[DEST].prefixStr;
	Ptr<ndn::Name> destPrefixName = nbrTable[DEST].prefixName;
	Ptr<Node> nextHop;
	int found = 0;
	Ptr<ndn::Name> srcPrefixName;
	cout<< "Prefix " << destPrefix << endl;


	for(i = 0; i != NODE_CNT; i++ ) {
		found = 0;
		sourceName = nbrTable[i].nodeName;
		srcPrefixName = nbrTable[i].prefixName;
		prefixStr = nbrTable[i].prefixStr;
		cout << "Sourcename " << sourceName << "\n";

		oneHopNodeInfoList = nbrTable[i].oneHopNodeInfoList;
		// If source is the dest then break
		if(prefixStr == destPrefix) {
			cout << "Current node " << sourceName << " is the dest \n";
			found = 1;
			nextHop = 0;
			nbrTable[i].nextHopNode = nbrTable[i].node;
			continue;
		}

		ndn::ndnSIM::trie_with_policy< ndn::Name,
											ndn::ndnSIM::smart_pointer_payload_traits<ndn::detail::RegisteredPrefixEntry>,
											ndn::ndnSIM::counting_policy_traits > tmpTrie;

		for(oneHopInfoListIter = oneHopNodeInfoList.begin() ; oneHopInfoListIter != oneHopNodeInfoList.end() ; oneHopInfoListIter++ ) {
			oneHopNbr = (*oneHopInfoListIter)->node;
			oneHopNbrName = (*oneHopInfoListIter)->nodeName;
			oneHopNbrPrefix = (*oneHopInfoListIter)->prefixStr;
			cout << "\t" <<"1HopNbr " << oneHopNbrName << "\n";
			twoHopList = (*oneHopInfoListIter)->oneHopNodeInfoList;
			// If one hop nbr is the dest then break
			if(oneHopNbrPrefix == destPrefix) {
				cout << "\tNext hop " << oneHopNbrName << " is the dest \n";
				found = 1;
				nextHop = oneHopNbr;
				nbrTable[i].nextHopNode = oneHopNbr;
				break;
			}
			for (twoHopListIter = twoHopList.begin(); twoHopListIter != twoHopList.end(); twoHopListIter++) {
				twoHopNbrName = (*twoHopListIter)->prefixName;
				twoHopNbrPreStr = (*twoHopListIter)->prefixStr;
				twoHopNbrStr = (*twoHopListIter)->nodeName;
				// If two hop nbr is the source then continue
				if(prefixStr == twoHopNbrPreStr) {
					cout << "\t\tThe two hop nbr " << twoHopNbrStr << " and the source are the same \n";
					continue;
				}
				cout << "\t\t" << "2HopNbr " << twoHopNbrStr << " : " << twoHopNbrPreStr << "\n";
				tmpTrie.insert((*twoHopNbrName), Create < ndn::detail::RegisteredPrefixEntry > (twoHopNbrName));
				nbrTable[i].nbrTrie = &tmpTrie;
			}
		}
		if (found != 1) {
			item = (tmpTrie).longest_prefix_match(*(destPrefixName));
			// If no prefix match with destination then parent node is the nbr
			if (item == 0) {
				cout << "The original prefix" << srcPrefixName <<"\t" << i;
				//print_name(*srcPrefixName);
				item = (tmpTrie).longest_prefix_match(*srcPrefixName);
			}
			// if item is still 0 then ideally assert
			//if (item != 0) {
				foundPrefStr = *((item->payload ())->GetPrefix());
				cout << "Longest Prefix found" << foundPrefStr << endl;
				oneHopNodeInfoList1 = nbrTable[i].oneHopNodeInfoList;
				for(oneHopInfoListIter1 = oneHopNodeInfoList1.begin() ; oneHopInfoListIter1 != oneHopNodeInfoList1.end() ; oneHopInfoListIter1++ ) {
					twoHopList1 = (*oneHopInfoListIter1)->oneHopNodeInfoList;
					for (twoHopListIter1 = twoHopList1.begin(); twoHopListIter1 != twoHopList1.end(); twoHopListIter1++) {
						if ((*twoHopListIter1)->prefixName->compare(foundPrefStr) == 0) {
								cout << "Nbr is " << (*oneHopInfoListIter1)->nodeName << endl;
								nbrTable[i].nextHopNode = (*oneHopInfoListIter1)->node;
								found = 1;
								break;
						}
					}
					if (found == 1)
						break;
				}
			//}
		}
		cout << "\n-------------------------------------------------\n";
	}
}

void
calculate_next_hops(void) {
	std::list<NodeInfo * > oneHopNodeInfoList;
	std::list<NodeInfo *>::const_iterator oneHopInfoListIter;
	std::list<Ptr<Node> > oneHopList;
	std::list<Ptr<Node> >::const_iterator oneHopListIter;
	std::list<NodeInfo *> twoHopList;
	std::list<NodeInfo *>::const_iterator twoHopListIter;
	Ptr<Node> oneHopNbr;
	Ptr<Node> twoHopNbr;
	std::string prefixStr;
	std::string sourceName;
	std::string oneHopNbrName;
	std::string twoHopNbrName;
	Ptr<ndn::Name> dstPrefixName;
	std::string dstPrefixStr;
	int i;
	super::iterator item;
	//ndn::ndnSIM::trie_with_policy< ndn::Name,
	//									ndn::ndnSIM::smart_pointer_payload_traits<ndn::detail::RegisteredPrefixEntry>,
	//									ndn::ndnSIM::counting_policy_traits > nbrTrie;


	for(i = 0; i < NODE_CNT; i++ ) {
		sourceName = nbrTable[i].nodeName;
		dstPrefixName = nbrTable[3].prefixName;
		dstPrefixStr = nbrTable[PROD].prefixStr;
		prefixStr = nbrTable[i].prefixStr;

		//item = (*(nbrTable[6].nbrTrie)).longest_prefix_match(*dstPrefixName);
		//cout << "Longest Prefix found" << *((item->payload ())->GetPrefix()) << endl;
		oneHopNodeInfoList = nbrTable[i].oneHopNodeInfoList;
		for(oneHopInfoListIter = oneHopNodeInfoList.begin() ; oneHopInfoListIter != oneHopNodeInfoList.end() ; oneHopInfoListIter++ ) {
			oneHopNbr = (*oneHopInfoListIter)->node;
			oneHopNbrName = (*oneHopInfoListIter)->nodeName;
			twoHopList = (*oneHopInfoListIter)->oneHopNodeInfoList;
			for (twoHopListIter = twoHopList.begin(); twoHopListIter != twoHopList.end(); twoHopListIter++) {
				twoHopNbrName = (*twoHopListIter)->nodeName;
				//std::cout << "Pri " << sourceName << " " << prefixStr << " -> " <<oneHopNbrName << " -> " << twoHopNbrName <<"\n";
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
  //print_nbr_table();
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

  fill_nbrTableTrie();
  //calculate_next_hops();

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigins (prefixstr, producer);

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes ();
  ndn::L3Protocol::FaceList m_faces;
  /*
  int id;
  std::cout << "face size" << m_faces.size() << "\n";
  ndn::Fib::

  for (int j = 0; j < 1; j++) {
	 id = faceList[j]->GetNode()->GetId();
	 std::cout << "id" << id << "\n";
     //std::cout << "Pri : FaceList" << nbrTable[id].nodeName << "\n";
  }
*/
  Simulator::Stop (Seconds (2.0));
  ndn::AppDelayTracer::InstallAll("outfile.txt");
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
