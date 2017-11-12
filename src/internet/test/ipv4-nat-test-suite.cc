/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2012 University of Washington
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
 */

#include "ns3/test.h"
#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/ipv4-nat.h"
#include "ns3/ipv4-address.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/simple-channel.h"
#include "ns3/simple-net-device.h"
#include "ns3/inet-socket-address.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/core-module.h"

using namespace ns3;

class Ipv4NatAddRemoveRulesStatic : public TestCase
{
public:
  Ipv4NatAddRemoveRulesStatic ();
  virtual ~Ipv4NatAddRemoveRulesStatic ();

private:
  virtual void DoRun (void);
  
};

Ipv4NatAddRemoveRulesStatic::Ipv4NatAddRemoveRulesStatic ()
  : TestCase ("Add and remove NAT rules from object")
{
}

Ipv4NatAddRemoveRulesStatic::~Ipv4NatAddRemoveRulesStatic ()
{
}

class Ipv4NatAddRemoveRulesDynamic : public TestCase
{
public:
  Ipv4NatAddRemoveRulesDynamic ();
  virtual ~Ipv4NatAddRemoveRulesDynamic ();

private:
  virtual void DoRun (void);
  
};

Ipv4NatAddRemoveRulesDynamic::Ipv4NatAddRemoveRulesDynamic ()
  : TestCase ("Add and remove NAT rules from object")
{
}

Ipv4NatAddRemoveRulesDynamic::~Ipv4NatAddRemoveRulesDynamic ()
{
}

class Ipv4NatStaticHeaderPostChanging : public TestCase
{
public:
  Ipv4NatStaticHeaderPostChanging ();
  virtual ~Ipv4NatStaticHeaderPostChanging ();

private:
  virtual void DoRun (void);
  
};

Ipv4NatStaticHeaderPostChanging::Ipv4NatStaticHeaderPostChanging ()
  : TestCase (" Post Routing of the packet ")
{
}

Ipv4NatStaticHeaderPostChanging::~Ipv4NatStaticHeaderPostChanging ()
{
}

class Ipv4NatStaticHeaderPreChanging : public TestCase
{
public:
  Ipv4NatStaticHeaderPreChanging ();
  virtual ~Ipv4NatStaticHeaderPreChanging ();

private:
  virtual void DoRun (void);
  
};

Ipv4NatStaticHeaderPreChanging::Ipv4NatStaticHeaderPreChanging ()
  : TestCase (" Post Routing of the packet ")
{
}

Ipv4NatStaticHeaderPreChanging::~Ipv4NatStaticHeaderPreChanging ()
{
}

class Ipv4NatStaticHeaderChecksum : public TestCase
{
public:
  Ipv4NatStaticHeaderChecksum ();
  virtual ~Ipv4NatStaticHeaderChecksum ();

private:
  virtual void DoRun (void);
  
};

Ipv4NatStaticHeaderChecksum::Ipv4NatStaticHeaderChecksum ()
  : TestCase (" Post Routing of the packet ")
{
}

Ipv4NatStaticHeaderChecksum::~Ipv4NatStaticHeaderChecksum ()
{
}


void
Ipv4NatAddRemoveRulesStatic::DoRun (void) // Static NAT
{
  // Create single NAT test node with outer interface of 203.0.113.1 and 
  // inner interface of 192.168.0.1
  Ptr<Node> testNode = CreateObject<Node> ();
  InternetStackHelper stack;
  stack.Install (testNode);

  Ptr<SimpleNetDevice> rxDev1, rxDev2;
  { // first, outside interface
    rxDev1 = CreateObject<SimpleNetDevice> ();
    rxDev1->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev1);
    Ptr<Ipv4> ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev1);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("203.0.113.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }
  { // second, inside interface
    rxDev2 = CreateObject<SimpleNetDevice> ();
    rxDev2->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev2);
    Ptr<Ipv4> ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev2);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("192.168.0.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }

  Ptr<Ipv4Nat> nat = CreateObject<Ipv4Nat> ();
  nat->SetOutside (1);
  nat->SetInside (2);
  testNode->AggregateObject (nat);

  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 0, "list is not initialized empty");
  Ipv4StaticNatRule rule1 (Ipv4Address ("192.168.0.3"), Ipv4Address ("203.0.113.103"));
  Ipv4StaticNatRule rule2 (Ipv4Address ("192.168.0.4"), Ipv4Address ("203.0.113.104"));
  nat->AddStaticRule (rule1);
  nat->AddStaticRule (rule2);
  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 2, "adding to list failed");

  nat->RemoveStaticRule (1);  // should remove the 192.168.0.3 rule, as 192.168.0.4 pushed it down the stack
  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 1, "removing from list failed");
  Ipv4StaticNatRule returnRule = nat->GetStaticRule (0);
  NS_TEST_ASSERT_MSG_EQ (returnRule.GetLocalIp (), Ipv4Address ("192.168.0.4"), "fetching from list failed");


}

void
Ipv4NatAddRemoveRulesDynamic::DoRun (void) // Dynamic
{
  // Create single NAT test node with outer interface of 203.0.113.1 and 
  // inner interface of 192.168.0.1
  Ptr<Node> testNode = CreateObject<Node> ();
  InternetStackHelper stack;
  stack.Install (testNode);

  Ptr<SimpleNetDevice> rxDev1, rxDev2;
  { // first, outside interface
    rxDev1 = CreateObject<SimpleNetDevice> ();
    rxDev1->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev1);
    Ptr<Ipv4> ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev1);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("203.0.113.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }
  { // second, inside interface
    rxDev2 = CreateObject<SimpleNetDevice> ();
    rxDev2->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev2);
    Ptr<Ipv4> ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev2);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("192.168.0.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }

  Ptr<Ipv4Nat> nat = CreateObject<Ipv4Nat> ();
  nat->SetOutside (1);
  nat->SetInside (2);
  testNode->AggregateObject (nat);

  NS_TEST_ASSERT_MSG_EQ (nat->GetNDynamicRules (), 0, "list is not initialized empty");
  //Adding the address to be translated to and port pools.
  nat->AddAddressPool ("202.101.43.0", "0.0.0.4","0.0.0.5", "255.255.255.0"); 
  nat->AddPortPool (49153, 49154);
  // Add a rule here to map outbound connections from n0, port 49153, UDP
  Ipv4DynamicNatRule rule1 (Ipv4Address ("192.168.1.0"), Ipv4Mask ("255.255.255.0"));
  nat->AddDynamicRule (rule1);
  NS_TEST_ASSERT_MSG_EQ (nat->GetNDynamicRules (), 1, "adding to list failed");

  nat->RemoveDynamicRule (0); 
  NS_TEST_ASSERT_MSG_EQ (nat->GetNDynamicRules (), 0, "removing from list failed");
  
}

void
Ipv4NatStaticHeaderPostChanging::DoRun (void) // Post-Routing
{
  // Create single NAT test node with outer interface of 203.0.113.1 and 
  // inner interface of 192.168.0.1
  Ptr<Node> testNode = CreateObject<Node> ();
  InternetStackHelper stack;
  stack.Install (testNode);
  Ptr<Ipv4> ipv4;

  Ptr<SimpleNetDevice> rxDev1, rxDev2;
  { // first, outside interface
    rxDev1 = CreateObject<SimpleNetDevice> ();
    rxDev1->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev1);
    ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev1);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("203.0.113.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }
  { // second, inside interface
    rxDev2 = CreateObject<SimpleNetDevice> ();
    rxDev2->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev2);
    ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev2);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("192.168.0.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }

  Ptr<Ipv4Nat> nat = CreateObject<Ipv4Nat> ();
  nat->SetOutside (1);
  nat->SetInside (2);
  testNode->AggregateObject (nat);

  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 0, "list is not initialized empty");
  Ipv4StaticNatRule rule1 (Ipv4Address ("192.168.0.3"), Ipv4Address ("203.0.113.103"));
  nat->AddStaticRule (rule1);
  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 1, "adding to list failed");
  /**
   * \Packet is created
   */
  Ptr<Packet> p = Create<Packet> (123); 
  Ipv4Header ipHeader;
  ipHeader.SetSource (Ipv4Address ("192.168.0.3"));
  ipHeader.SetProtocol (0);
  ipHeader.SetPayloadSize (p->GetSize ());
  ipHeader.SetTtl (255);
  p->AddHeader (ipHeader);
  Ptr<Ipv4Netfilter> ipv4nf = ipv4->GetNetfilter ();
  ContinueCallback ccb = MakeCallback (&Ipv4Netfilter::NetfilterConntrackConfirm, ipv4nf);
  ipv4nf->ProcessHook (PF_INET, NF_INET_POST_ROUTING, p, rxDev2, rxDev1, ccb);
  p->RemoveHeader(ipHeader);
  Ipv4Address address;
  address = ipHeader.GetSource ();
  NS_TEST_EXPECT_MSG_EQ (address, "203.0.113.103", "Mapping Failed");
}

void
Ipv4NatStaticHeaderPreChanging::DoRun (void) // Pre-Routing
{
  // Create single NAT test node with outer interface of 203.0.113.1 and 
  // inner interface of 192.168.0.1
  Ptr<Node> testNode = CreateObject<Node> ();
  InternetStackHelper stack;
  stack.Install (testNode);
  Ptr<Ipv4> ipv4;

  Ptr<SimpleNetDevice> rxDev1, rxDev2;
  { // first, outside interface
    rxDev1 = CreateObject<SimpleNetDevice> ();
    rxDev1->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev1);
    ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev1);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("203.0.113.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }
  { // second, inside interface
    rxDev2 = CreateObject<SimpleNetDevice> ();
    rxDev2->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev2);
    ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev2);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("192.168.0.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }

  Ptr<Ipv4Nat> nat = CreateObject<Ipv4Nat> ();
  nat->SetOutside (1);
  nat->SetInside (2);
  testNode->AggregateObject (nat);

  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 0, "list is not initialized empty");
  Ipv4StaticNatRule rule1 (Ipv4Address ("192.168.0.4"),Ipv4Address ("203.0.113.104"));
  nat->AddStaticRule (rule1);
  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 1, "adding to list failed");
  /**
   * \Packet is created
   */
  Ptr<Packet> p = Create<Packet> (123); 
  Ipv4Header ipHeader;
  ipHeader.SetDestination (Ipv4Address ("203.0.113.104"));
  ipHeader.SetProtocol (0);
  ipHeader.SetPayloadSize (p->GetSize ());
  ipHeader.SetTtl (255);
  p->AddHeader (ipHeader);
  Ptr<Ipv4Netfilter> ipv4nf = ipv4->GetNetfilter ();
  ContinueCallback ccb = MakeCallback (&Ipv4Netfilter::NetfilterConntrackConfirm, ipv4nf);
  ipv4nf->ProcessHook (PF_INET, NF_INET_PRE_ROUTING, p, rxDev1, rxDev2, ccb);
  p->RemoveHeader(ipHeader);
  Ipv4Address address;
  address = ipHeader.GetDestination ();
  NS_TEST_EXPECT_MSG_EQ (address, Ipv4Address ("192.168.0.4"), "Mapping Failed");
}

void
Ipv4NatStaticHeaderChecksum::DoRun (void) // Checksum in Post Routing
{
  GlobalValue::Bind("ChecksumEnabled",BooleanValue(true));
  // Create single NAT test node with outer interface of 203.0.113.1 and 
  // inner interface of 192.168.0.1
  Ptr<Node> testNode = CreateObject<Node> ();
  InternetStackHelper stack;
  stack.Install (testNode);
  Ptr<Ipv4> ipv4;

  Ptr<SimpleNetDevice> rxDev1, rxDev2;
  { // first, outside interface
    rxDev1 = CreateObject<SimpleNetDevice> ();
    rxDev1->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev1);
    ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev1);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("203.0.113.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }
  { // second, inside interface
    rxDev2 = CreateObject<SimpleNetDevice> ();
    rxDev2->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev2);
    ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev2);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("192.168.0.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }

  Ptr<Ipv4Nat> nat = CreateObject<Ipv4Nat> ();
  nat->SetOutside (1);
  nat->SetInside (2);
  testNode->AggregateObject (nat);

  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 0, "list is not initialized empty");
  Ipv4StaticNatRule rule1 (Ipv4Address ("192.168.0.3"), Ipv4Address ("203.0.113.103"));
  nat->AddStaticRule (rule1);
  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 1, "adding to list failed");
  /**
   * \Packet is created
   */
  Ptr<Packet> p = Create<Packet> (123); 
  Ipv4Header ipHeader;
  ipHeader.SetSource (Ipv4Address ("192.168.0.3"));
  ipHeader.SetDestination (Ipv4Address ("203.0.113.105"));
  ipHeader.SetProtocol (0);
  ipHeader.SetPayloadSize (p->GetSize ());
  ipHeader.SetTtl (255);
  ipHeader.EnableChecksum();
  p->AddHeader (ipHeader);
  //Ptr<Ipv4Netfilter> ipv4nf = CreateObject<Ipv4Netfilter> ();
  //Ptr<Node> node = nat->GetObject<Node> ();
  //Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
  Ptr<Ipv4Netfilter> ipv4nf = ipv4->GetNetfilter ();
  ContinueCallback ccb = MakeCallback (&Ipv4Netfilter::NetfilterConntrackConfirm, ipv4nf);
  ipv4nf->ProcessHook (PF_INET, NF_INET_POST_ROUTING, p, rxDev2, rxDev1, ccb);
  //nat->DoNatPostRouting (NF_INET_POST_ROUTING, p, rxDev2, rxDev1, MakeNullCallback<uint32_t,Ptr<Packet>>()););
  p->RemoveHeader(ipHeader);
  
  Ipv4Address address;
  address = ipHeader.GetSource ();
  NS_TEST_EXPECT_MSG_EQ (address, Ipv4Address ("203.0.113.103"), "Mapping Failed");

  NS_TEST_ASSERT_MSG_EQ (ipHeader.IsChecksumOk(), true, "Checksum Not Okay");

}

/**
 * \Class for Static NAT rules
 */
class Ipv4NatStatic : public TestCase
{
  public:
    Ipv4NatStatic ();
    virtual ~Ipv4NatStatic ();

  private:
    virtual void DoRun (void);
};

Ipv4NatStatic::Ipv4NatStatic ()
  : TestCase ("Test that NAT works with static rules")
{
}

Ipv4NatStatic::~Ipv4NatStatic ()
{
}

void
Ipv4NatStatic::DoRun (void)
{
}

/**
 * \Class for Dynamic NAT rules
 */
class Ipv4NatDynamic : public TestCase
{
  public:
    Ipv4NatDynamic ();
    virtual ~Ipv4NatDynamic ();

  private:
    virtual void DoRun (void);
};

Ipv4NatDynamic::Ipv4NatDynamic ()
  : TestCase ("Test that NAT works with dynamic rules")
{
}

Ipv4NatDynamic::~Ipv4NatDynamic ()
{
}

void
Ipv4NatDynamic::DoRun (void)
{
}

/**
 * \Class for Test Suite
 */
class Ipv4NatTestSuite : public TestSuite
{
public:
  Ipv4NatTestSuite ();
};

Ipv4NatTestSuite::Ipv4NatTestSuite ()
  : TestSuite ("ipv4-nat", UNIT)
{
  AddTestCase (new Ipv4NatAddRemoveRulesStatic);
  AddTestCase (new Ipv4NatStatic);
  AddTestCase (new Ipv4NatAddRemoveRulesDynamic);
  AddTestCase (new Ipv4NatDynamic);
  AddTestCase (new Ipv4NatStaticHeaderPostChanging);
  AddTestCase (new Ipv4NatStaticHeaderPreChanging);
  AddTestCase (new Ipv4NatStaticHeaderChecksum);
}

static Ipv4NatTestSuite ipv4NatTestSuite;

