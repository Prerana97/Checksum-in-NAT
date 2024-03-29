/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 University of Texas at Dallas
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
 * Author: Qasim Javed <qasim@utdallas.edu>
 */
#ifndef IPV4_CONNTRACK_L3_PROTOCOL
#define IPV4_CONNTRACK_L3_PROTOCOL

#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "netfilter-conntrack-l3-protocol.h"
#include "ipv4-netfilter-hook.h"

namespace ns3 {

class Packet;
class NetDevice;

class Ipv4ConntrackL3Protocol : public NetfilterConntrackL3Protocol
{
public:
  Ipv4ConntrackL3Protocol ();
  uint16_t RegisterPreRoutingHook ();
  uint16_t RegisterInHook ();
  uint16_t RegisterOutHook ();
  uint16_t RegisterPostRoutingHook ();
  uint32_t Ipv4Confirm (Hooks_t hookNumber, Ptr<Packet> packet, Ptr<NetDevice> in, Ptr<NetDevice> out, ContinueCallback& ccb);

private:
  NetfilterHookCallback Ipv4ConntrackIn;
  NetfilterHookCallback Ipv4ConntrackLocal;
  NetfilterHookCallback Ipv4ConntrackConfirm;
  /**
   * \param hookNumber The hook number
   * \param p Packet that is handed over to the callback chain for this hook
   * \param in NetDevice which received the packet
   * \param out The outgoing NetDevice
   * \param ccb If not NULL, this callback will be invoked once the hook
   * callback chain has finished processing
   */
  uint32_t Ipv4ConntrackPreRoutingHook (Hooks_t hookNumber, Ptr<Packet> p, Ptr<NetDevice> in, Ptr<NetDevice> out, ContinueCallback& ccb);
  /**
   * \param hookNumber The hook number
   * \param p Packet that is handed over to the callback chain for this hook
   * \param in NetDevice which received the packet
   * \param out The outgoing NetDevice
   * \param ccb If not NULL, this callback will be invoked once the hook
   * callback chain has finished processing
   */
  uint32_t Ipv4ConntrackInHook (Hooks_t hookNumber, Ptr<Packet> p, Ptr<NetDevice> in, Ptr<NetDevice> out, ContinueCallback& ccb);
  /**
   * \param hookNumber The hook number
   * \param p Packet that is handed over to the callback chain for this hook
   * \param in NetDevice which received the packet
   * \param out The outgoing NetDevice
   * \param ccb If not NULL, this callback will be invoked once the hook
   * callback chain has finished processing
   */
  uint32_t Ipv4ConntrackOutHook (Hooks_t hookNumber, Ptr<Packet> p, Ptr<NetDevice> in, Ptr<NetDevice> out, ContinueCallback& ccb);
  /**
   * \param hookNumber The hook number
   * \param p Packet that is handed over to the callback chain for this hook
   * \param in NetDevice which received the packet
   * \param out The outgoing NetDevice
   * \param ccb If not NULL, this callback will be invoked once the hook
   * callback chain has finished processing
   */
  uint32_t Ipv4ConntrackPostRoutingHook (Hooks_t hookNumber, Ptr<Packet> p, Ptr<NetDevice> in, Ptr<NetDevice> out, ContinueCallback& ccb);
  /**
   * \param packet Packet that should be converted to a tuple
   * \param tuple The created tuple is stored here
   * \returns true if success, false otherwise
   *
   * Protocol specific method to convert a packet into a tuple
   * for connection tracking purposes.
   */
  bool hasPacketToTuple (Ptr<Packet> packet, NetfilterConntrackTuple& tuple);
  /**
   * \param inverse The resulting inverse of the provided tuple
   * \param orig The original tuple that should be inverted
   * \returns true if success, false otherwise
   *
   * Protocol specific method to invert the passed tuple
   */
  bool hasInvertTuple (NetfilterConntrackTuple& inverse, NetfilterConntrackTuple& orig);

};

}

#endif /* IPV4_CONNTRACK_L3_PROTOCOL */
