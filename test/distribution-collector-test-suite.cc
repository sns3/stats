/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Magister Solutions
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
 * Author: Budiarto Herman <budiarto.herman@magister.fi>
 *
 */

/**
 * \file distribution-collector-test-suite.cc
 * \ingroup stats
 * \brief Test cases for DistributionCollector.
 *
 * Usage example:
 * \code
 *    $ ./test.py --suite=distribution-collector
 * \endcode
 */

#include <ns3/distribution-collector.h>
#include <ns3/core-module.h>
#include <sstream>
#include <list>


NS_LOG_COMPONENT_DEFINE ("DistributionCollectorTest");

namespace ns3 {


/**
 * \ingroup stats
 * \brief
 */
class DistributionCollectorTestCase : public TestCase
{
public:
  /**
   * \brief Construct a new test case.
   * \param name the test case name, which will be printed on the report
   * \param type
   * \param minValue
   * \param maxValue
   * \param binLength
   * \param input
   * \param expectedOutput
   */
  DistributionCollectorTestCase (std::string name,
                                 DistributionCollector::OutputType_t type,
                                 double minValue,
                                 double maxValue,
                                 double binLength,
                                 std::string input,
                                 std::string expectedOutput);

private:
  virtual void DoRun ();
  virtual void DoTeardown ();
  void FeedInput () const;
  void CollectorCallback (double sample, double count);

  DistributionCollector::OutputType_t m_type;
  double m_minValue;
  double m_maxValue;
  double m_binLength;
  std::string m_input;
  std::string m_expectedOutput;
  std::list<double> m_expectedSample;
  std::list<double> m_expectedCount;
  Ptr<DistributionCollector> m_collector;

}; // end of `class DistributionCollectorTestCase`


DistributionCollectorTestCase::DistributionCollectorTestCase (std::string name,
                                                              DistributionCollector::OutputType_t type,
                                                              double minValue,
                                                              double maxValue,
                                                              double binLength,
                                                              std::string input,
                                                              std::string expectedOutput)
  : TestCase (name),
    m_type (type),
    m_minValue (minValue),
    m_maxValue (maxValue),
    m_binLength (binLength),
    m_input (input),
    m_expectedOutput (expectedOutput)
{
  NS_LOG_FUNCTION (this << name
                        << DistributionCollector::GetOutputTypeName (type)
                        << minValue << maxValue << binLength
                        << input << expectedOutput);
}


void
DistributionCollectorTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << GetName ());

  // Convert expectedOutput string to list.
  double sample = 0.0;
  double count = 0;
  std::istringstream iss (m_expectedOutput);
  while (iss.good ())
    {
      iss >> sample;
      m_expectedSample.push_back (sample);
      NS_ASSERT (iss.good ());
      iss >> count;
      m_expectedCount.push_back (count);
    }
  NS_ASSERT (m_expectedSample.size () == m_expectedCount.size ());

  // Create the collector to test.
  m_collector = CreateObject<DistributionCollector> ();
  m_collector->SetMinValue (m_minValue);
  m_collector->SetMaxValue (m_maxValue);
  m_collector->SetBinLength (m_binLength);
  m_collector->SetOutputType (m_type);

  // Connect the collector's output to a callback of this class.
  const bool ret = m_collector->TraceConnectWithoutContext (
    "Output", MakeCallback (&DistributionCollectorTestCase::CollectorCallback,
                            this));
  NS_ASSERT (ret);

  // Push inputs into the collector after 1 ms of simulation time.
  Simulator::Schedule (MilliSeconds (1),
                       &DistributionCollectorTestCase::FeedInput,
                       this);

  Simulator::Stop (MilliSeconds (2));
  Simulator::Run ();
  Simulator::Destroy ();
}


void
DistributionCollectorTestCase::DoTeardown ()
{
  NS_LOG_FUNCTION (this << GetName ());
  /*
   * We destroy the collector here, earlier than it's supposed to be, in order
   * to flush the output of the collector out.
   */
  m_collector->Dispose ();
  m_collector = 0;
}


void
DistributionCollectorTestCase::FeedInput () const
{
  NS_LOG_FUNCTION (this << GetName ());

  double sample = 0.0;
  std::istringstream iss (m_input);
  while (iss.good ())
    {
      iss >> sample;
      Simulator::ScheduleNow (&DistributionCollector::TraceSinkDouble,
                              m_collector, sample, sample);
    }
}


void
DistributionCollectorTestCase::CollectorCallback (double sample, double count)
{
  NS_LOG_FUNCTION (this << GetName () << sample << count);

  NS_ASSERT (m_expectedSample.size () == m_expectedCount.size ());
  NS_TEST_ASSERT_MSG_GT (m_expectedSample.size (), 0,
                         "Received more samples than expected");

  if (m_expectedSample.size () > 0)
    {
      NS_TEST_ASSERT_MSG_EQ_TOL (m_expectedSample.front (), sample, 0.001,
                                 "Sample values do not match");
      NS_TEST_ASSERT_MSG_EQ_TOL (m_expectedCount.front (), count, 0.0001,
                                 "Count values for sample " << m_expectedSample.front ()
                                                            << " do not match");
      m_expectedSample.pop_front ();
      m_expectedCount.pop_front ();
    }
}


/**
 * \brief Test suite `distribution-collector`, verifying the
 *        DistributionCollector class.
 */
class DistributionCollectorTestSuite : public TestSuite
{
public:
  DistributionCollectorTestSuite ();
};


DistributionCollectorTestSuite::DistributionCollectorTestSuite ()
  : TestSuite ("distribution-collector", UNIT)
{
  //LogComponentEnable ("DistributionCollectorTest", LOG_LEVEL_ALL);
  //LogComponentEnable ("DistributionCollectorTest", LOG_PREFIX_ALL);
  //LogComponentEnable ("DistributionCollector", LOG_LEVEL_ALL);
  //LogComponentEnable ("DistributionCollector", LOG_PREFIX_ALL);

  AddTestCase (new DistributionCollectorTestCase ("d-1-histogram",
                                                  DistributionCollector::OUTPUT_TYPE_HISTOGRAM,
                                                  0.0, 100.0, 10.0,
                                                  "-10 10 30 50 70 90 110",
                                                  "5 1 15 1 25 0 35 1 45 0 55 1 65 0 75 1 85 0 95 2"),
               TestCase::QUICK);

  AddTestCase (new DistributionCollectorTestCase ("d-2-histogram",
                                                  DistributionCollector::OUTPUT_TYPE_HISTOGRAM,
                                                  -100.0, 0.0, 20.0,
                                                  "-30 -10 10 30 50 70 90",
                                                  "-90 0 -70 0 -50 0 -30 1 -10 6"),
               TestCase::QUICK);

  AddTestCase (new DistributionCollectorTestCase ("d-3-histogram",
                                                  DistributionCollector::OUTPUT_TYPE_HISTOGRAM,
                                                  0.0, 9.0, 2.0,
                                                  "10 9 8 6 5 4 3 2 1 0",
                                                  "1 2 3 2 5 2 7 1 9 3"),
               TestCase::QUICK);

  AddTestCase (new DistributionCollectorTestCase ("d-3-probability",
                                                  DistributionCollector::OUTPUT_TYPE_PROBABILITY,
                                                  0.0, 9.0, 2.0,
                                                  "10 9 8 6 5 4 3 2 1 0",
                                                  "1 0.2 3 0.2 5 0.2 7 0.1 9 0.3"),
               TestCase::QUICK);

  AddTestCase (new DistributionCollectorTestCase ("d-3-cumulative",
                                                  DistributionCollector::OUTPUT_TYPE_CUMULATIVE,
                                                  0.0, 9.0, 2.0,
                                                  "10 9 8 6 5 4 3 2 1 0",
                                                  "1 0.2 3 0.4 5 0.6 7 0.7 9 1"),
               TestCase::QUICK);

  AddTestCase (new DistributionCollectorTestCase ("d-4-histogram",
                                                  DistributionCollector::OUTPUT_TYPE_HISTOGRAM,
                                                  -100.0, 0.0, 10.0,
                                                  "-33 -32 -31 -77 -76 -75 -74 -73 -72 -71",
                                                  "-95 0 -85 0 -75 7 -65 0 -55 0 -45 0 -35 3 -25 0 -15 0 -5 0"),
               TestCase::QUICK);

  AddTestCase (new DistributionCollectorTestCase ("d-4-probability",
                                                  DistributionCollector::OUTPUT_TYPE_PROBABILITY,
                                                  -100.0, 0.0, 10.0,
                                                  "-33 -32 -31 -77 -76 -75 -74 -73 -72 -71",
                                                  "-95 0 -85 0 -75 0.7 -65 0 -55 0 -45 0 -35 0.3 -25 0 -15 0 -5 0"),
               TestCase::QUICK);

  AddTestCase (new DistributionCollectorTestCase ("d-4-cumulative",
                                                  DistributionCollector::OUTPUT_TYPE_CUMULATIVE,
                                                  -100.0, 0.0, 10.0,
                                                  "-33 -32 -31 -77 -76 -75 -74 -73 -72 -71",
                                                  "-95 0 -85 0 -75 0.7 -65 0.7 -55 0.7 -45 0.7 -35 1 -25 1 -15 1 -5 1"),
               TestCase::QUICK);

} // end of `DistributionCollectorTestSuite ()`


static DistributionCollectorTestSuite g_distributionCollectorTestSuiteInstance;


} // end of namespace ns3
