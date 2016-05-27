/*
 * myndn.h
 *
 *  Created on: May 19, 2016
 *      Author: pri
 */

#ifndef SCRATCH_MYNDN_H_
#define SCRATCH_MYNDN_H_

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"
#include "../src/ndnSIM/utils/trie/trie.h"
#include "../src/ndnSIM/utils/trie/trie-with-policy.h"
#include "../src/ndnSIM/utils/trie/counting-policy.h"
#include "../src/ndnSIM/ndn.cxx/detail/pending-interests-container.h"
#include "../src/ndnSIM/ndn.cxx/detail/registered-prefix-container.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"
#include "../src/ndnSIM/helper/ndn-global-routing-helper.h"
#include "../src/ndnSIM/model/ndn-net-device-face.h"
#include "../src/ndnSIM/model/ndn-global-router.h"
using namespace std;
using namespace ns3;
using namespace ndn;

typedef ndn::ndnSIM::trie_with_policy< ndn::Name,
					ndn::ndnSIM::smart_pointer_payload_traits<ndn::detail::RegisteredPrefixEntry>,
					ndn::ndnSIM::counting_policy_traits > twoNbrTrie;


typedef ndn::ndnSIM::trie_with_policy< Name,
                                    ndnSIM::smart_pointer_payload_traits<ndn::detail::RegisteredPrefixEntry>,
                                    ndnSIM::counting_policy_traits > super;

std::string interestPrefixstr = "/prefix";

class NodeInfo {
public:
	Ptr<Node> node;
	std::list<Ptr<Node> > oneHopList; //List of one hop nbrs.
	std::list<NodeInfo *> oneHopNodeInfoList; // List of Nodeinfos of one hop nbrs.
	std::string nodeName;    // like hostname
	std::string prefixStr;
	Ptr<ndn::Name> prefixName;
	twoNbrTrie *nbrTrie;
	Ptr<Node> nextHopNode; //Next node to route to (This is to be deleted and directly added to fib)
	// like ip address
// (*oneHopInfoList).oneHopList is the list of twoHopNbrs going through that oneHopNbr
// note the twoHopNbr could be the source node also..so always check for that
} ;

std::string prefixNamesArr[] = {
		"/0/2/1",		//a
		"/0/1/1/1", 	//b
		"/0/2",			//c
		"/0/3/1",		//d
		"/0/2/1/1",		//e
		"/0/1/1",		//f
		"/0/1/1/1/1",	//g
		"/0/1/1/1/2",	//h
		"/0/1",			//i
		"/0",			//j
		"/0/3",			//k
		"/0/3/1/1",		//m
		"/0/2/1/1/2",	//n
		"/0/2/1/1/1",	//o
		"/0/3/1/2",		//p
		"/0/3/1/1/1",	//q
		"/0/2/1/1/2/1",	//r
		"/0/3/1/2/1" // s
};

#define NODE_CNT 18
#define CONS 7 //node h
#define PROD 15 // node q
#define DEST PROD


void
add_path(unsigned firstNode,unsigned SecndNode, int metric, const string str);


#endif /* SCRATCH_MYNDN_H_ */
