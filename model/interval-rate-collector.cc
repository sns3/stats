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

#include "interval-rate-collector.h"
#include <ns3/log.h>
#include <ns3/simulator.h>
#include <ns3/enum.h>

NS_LOG_COMPONENT_DEFINE ("IntervalRateCollector");


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (IntervalRateCollector);


IntervalRateCollector::IntervalRateCollector ()
  : m_intervalSumDouble (0.0),
    m_overallSumDouble (0.0),
    m_intervalSumUinteger (0),
    m_overallSumUinteger (0),
    m_currentIntervalStart (Simulator::Now ()),
    m_intervalLength (Seconds (1.0)),
    m_timeUnit (Time::S),
    m_nextReset ()
{
  NS_LOG_FUNCTION (this << GetName ());

  // delayed start to ensure attributes are completely initialized
  Simulator::ScheduleNow (&IntervalRateCollector::FirstInterval, this);
}


TypeId // static
IntervalRateCollector::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::IntervalRateCollector")
    .SetParent<DataCollectionObject> ()
    .AddConstructor<IntervalRateCollector> ()
    .AddAttribute ("IntervalLength",
                   "Control the frequency of producing output. For example, "
                   "an interval length of one second (the default) causes "
                   "this collector instance to emit new output via the "
                   "`OutputWithTime` and `OutputWithoutTime` trace sources "
                   "after every one second. Accumulated values are reset to "
                   "zero after every output invocation, hence the values "
                   "emitted by these trace sources can be regarded as rate or "
                   "throughput. Note that an interval length of zero prevents "
                   "these trace sources from emitting any output.",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&IntervalRateCollector::SetIntervalLength,
                                     &IntervalRateCollector::GetIntervalLength),
                   MakeTimeChecker ())
    .AddAttribute ("TimeUnit",
                   "",
                   EnumValue (Time::S),
                   MakeEnumAccessor (&IntervalRateCollector::SetTimeUnit,
                                     &IntervalRateCollector::GetTimeUnit),
                   MakeEnumChecker (Time::Y,    "Y",   // year, 365 days
                                    Time::D,    "D",   // day, 24 hours
                                    Time::H,    "H",   // hour, 60 minutes
                                    Time::MIN,  "MIN", // minute, 60 seconds
                                    Time::S,    "S",   // second
                                    Time::MS,   "MS",  // millisecond
                                    Time::US,   "US",  // microsecond
                                    Time::NS,   "NS",  // nanosecond
                                    Time::PS,   "PS",  // picosecond
                                    Time::FS,   "FS",  // femtosecond
                                    Time::LAST, "LAST"))
    .AddTraceSource ("OutputOverall",
                     "The accumulated sum, "
                     "fired when the collector instance is destroyed.",
                     MakeTraceSourceAccessor (&IntervalRateCollector::m_outputOverall))
    .AddTraceSource ("OutputWithTime",
                     "The interval's ending time and the accumulated sum during the interval",
                     MakeTraceSourceAccessor (&IntervalRateCollector::m_outputWithTime))
    .AddTraceSource ("OutputWithoutTime",
                     "The accumulated sum during the last interval",
                     MakeTraceSourceAccessor (&IntervalRateCollector::m_outputWithoutTime))
  ;
  return tid;
}


void
IntervalRateCollector::DoDispose ()
{
  NS_LOG_FUNCTION (this << GetName ());

  if (IsEnabled ())
    {
      const double combinedSum = m_overallSumDouble + static_cast<double> (m_overallSumUinteger);
      m_outputOverall (combinedSum);
    }

  NewInterval (); // also invoke interval-based output at the end of simulation
}


void
IntervalRateCollector::SetIntervalLength (Time intervalLength)
{
  NS_LOG_FUNCTION (this << GetName () << intervalLength.GetSeconds ());
  m_intervalLength = intervalLength;
}


Time
IntervalRateCollector::GetIntervalLength () const
{
  return m_intervalLength;
}


void
IntervalRateCollector::SetTimeUnit (Time::Unit unit)
{
  NS_LOG_FUNCTION (this << GetName () << unit);
  m_timeUnit = unit;
}


Time::Unit
IntervalRateCollector::GetTimeUnit () const
{
  return m_timeUnit;
}


void
IntervalRateCollector::FirstInterval ()
{
  NS_LOG_FUNCTION (this << GetName ());

  m_currentIntervalStart = Simulator::Now ();

  if (m_intervalLength > MilliSeconds (0))
    {
      // Schedule the next interval
      m_nextReset = Simulator::Schedule (m_intervalLength,
                                         &IntervalRateCollector::NewInterval,
                                         this);
    }
}


void
IntervalRateCollector::NewInterval ()
{
  NS_LOG_FUNCTION (this << GetName ());

  if (IsEnabled ())
    {
      /*
       * Unleash the accumulated values as output via the trace sources. Here,
       * the time information is computed as the addition of the start time of
       * the interval and the interval length. This is a workaround from the
       * fact that Simulator::Now () returns 0 during object destruction
       * (i.e., DoDispose), therefore falsely indicating the last interval of
       * the simulation as the 0-th interval.
       */
      const double time = (m_currentIntervalStart + m_intervalLength).ToDouble (m_timeUnit);
      const double combinedSum = m_intervalSumDouble + static_cast<double> (m_intervalSumUinteger);
      m_outputWithTime (time, combinedSum);
      m_outputWithoutTime (combinedSum);
    }

  // Reset the accumulated values.
  m_intervalSumDouble = 0.0;
  m_intervalSumUinteger = 0;
  m_currentIntervalStart = Simulator::Now ();

  if (m_intervalLength > MilliSeconds (0))
    {
      // Schedule the next interval
      m_nextReset = Simulator::Schedule (m_intervalLength,
                                         &IntervalRateCollector::NewInterval,
                                         this);
    }
}


void
IntervalRateCollector::TraceSinkDouble (double oldData, double newData)
{
  NS_LOG_FUNCTION (this << GetName () << oldData << newData);

  if (IsEnabled ())
    {
      m_intervalSumDouble += newData;
      m_overallSumDouble += newData;
    }
}


void
IntervalRateCollector::TraceSinkUinteger8 (uint8_t oldData, uint8_t newData)
{
  TraceSinkUinteger64 (static_cast<uint64_t> (oldData),
                       static_cast<uint64_t> (newData));
}


void
IntervalRateCollector::TraceSinkUinteger16 (uint16_t oldData, uint16_t newData)
{
  TraceSinkUinteger64 (static_cast<uint64_t> (oldData),
                       static_cast<uint64_t> (newData));
}


void
IntervalRateCollector::TraceSinkUinteger32 (uint32_t oldData, uint32_t newData)
{
  TraceSinkUinteger64 (static_cast<uint64_t> (oldData),
                       static_cast<uint64_t> (newData));
}


void
IntervalRateCollector::TraceSinkUinteger64 (uint64_t oldData, uint64_t newData)
{
  NS_LOG_FUNCTION (this << GetName () << oldData << newData);

  if (IsEnabled ())
    {
      m_intervalSumUinteger += newData;
      m_overallSumUinteger += newData;
    }
}


} // end of namespace ns3
