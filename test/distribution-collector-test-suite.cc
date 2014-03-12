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

  std::string m_input;
  std::list<double> m_expectedSample;
  std::list<uint32_t> m_expectedCount;
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
    m_input (input)
{
  NS_LOG_FUNCTION (this << name
                        << DistributionCollector::GetOutputTypeName (type)
                        << minValue << maxValue << binLength
                        << input << expectedOutput);

  double sample = 0.0;
  uint32_t count = 0;
  std::istringstream iss (expectedOutput);

  while (iss.good ())
    {
      iss >> sample;
      m_expectedSample.push_back (sample);
      NS_ASSERT (iss.good ());
      iss >> count;
      m_expectedCount.push_back (count);
    }

  NS_ASSERT (m_expectedSample.size () == m_expectedCount.size ());

  m_collector = CreateObject<DistributionCollector> ();
  m_collector->SetMinValue (minValue);
  m_collector->SetMaxValue (maxValue);
  m_collector->SetBinLength (binLength);
  m_collector->SetOutputType (type);
}


void
DistributionCollectorTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << GetName ());

  const bool ret = m_collector->TraceConnectWithoutContext (
    "Output", MakeCallback (&DistributionCollectorTestCase::CollectorCallback,
                            this));
  NS_ASSERT (ret);

  Simulator::Schedule (MilliSeconds (1),
                       &DistributionCollectorTestCase::FeedInput,
                       this);
  Simulator::Stop (MilliSeconds (2));
  Simulator::Run ();

  //NS_TEST_ASSERT_MSG_EQ (m_expectedSample.size (), 0,
  //                       "Too few actual samples produced");
  Simulator::Destroy ();
}


void
DistributionCollectorTestCase::DoTeardown ()
{
  NS_LOG_FUNCTION (this << GetName ());
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
                         "Too many actual samples produced");

  if (m_expectedSample.size () > 0)
    {
      NS_TEST_ASSERT_MSG_EQ (m_expectedSample.front (), sample,
                             "Sample values do not match");
      NS_TEST_ASSERT_MSG_EQ (m_expectedCount.front (), count,
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

  AddTestCase (new DistributionCollectorTestCase ("histogram-1",
                                                  DistributionCollector::OUTPUT_TYPE_HISTOGRAM,
                                                  0.0, 100.0, 10.0,
                                                  "-10 10 30 50 70 90 110",
                                                  "5 1 15 1 25 0 35 1 45 0 55 1 65 0 75 1 85 0 95 2"),
               TestCase::QUICK);

  AddTestCase (new DistributionCollectorTestCase ("histogram-2",
                                                  DistributionCollector::OUTPUT_TYPE_HISTOGRAM,
                                                  -100.0, 0.0, 20.0,
                                                  "-30 -10 10 30 50 70 90",
                                                  "-90 0 -70 0 -50 0 -30 1 -10 6"),
               TestCase::QUICK);

  AddTestCase (new DistributionCollectorTestCase ("histogram-3",
                                                  DistributionCollector::OUTPUT_TYPE_HISTOGRAM,
                                                  0.0, 9.0, 2.0,
                                                  "10 9 8 7 6 5 4 3 2 1 0",
                                                  "1 2 3 2 5 2 7 2 9 3"),
               TestCase::QUICK);

} // end of `DistributionCollectorTestSuite ()`


static DistributionCollectorTestSuite g_distributionCollectorTestSuiteInstance;


} // end of namespace ns3
