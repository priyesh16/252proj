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
	std::list<Ptr<Node> > oneHopNbr;
	std::list<NodeInfo *> oneHopNbrInfo;
	std::string nodeName;    // like hostname
	std::string prefixName;  // like ip address
// (*oneHopNbrInfo).oneHopNbr is the list of twoHopNbrs going through that oneHopNbr
} ;

#endif /* SCRATCH_MYNDN_H_ */
