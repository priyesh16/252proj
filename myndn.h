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

using namespace ns3;

class NodeInfo {
public:
	Ptr<Node> node;
	std::list<Ptr<Node> > oneHopList; //List of one hop nbrs.
	std::list<NodeInfo *> oneHopNodeInfoList; // List of Nodeinfos of one hop nbrs.
	std::string nodeName;    // like hostname
	std::string prefixName;
	Ptr<Node> nextHopNode; //Next node to route to (This is to be deleted and directly added to fib)
	// like ip address
// (*oneHopInfoList).oneHopList is the list of twoHopNbrs going through that oneHopNbr
// note the twoHopNbr could be the source node also..so always check for that
} ;

#endif /* SCRATCH_MYNDN_H_ */
