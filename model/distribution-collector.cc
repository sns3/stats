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

#include "distribution-collector.h"
#include <ns3/log.h>
#include <ns3/simulator.h>
#include <ns3/enum.h>
#include <ns3/double.h>
#include <limits>
#include <cmath>

NS_LOG_COMPONENT_DEFINE ("DistributionCollector");


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DistributionCollector);


std::string //static
DistributionCollector::GetOutputTypeName (DistributionCollector::OutputType_t outputType)
{
  switch (outputType)
    {
    case DistributionCollector::OUTPUT_TYPE_HISTOGRAM:
      return "OUTPUT_TYPE_HISTOGRAM";
    case DistributionCollector::OUTPUT_TYPE_PROBABILITY:
      return "OUTPUT_TYPE_PROBABILITY";
    case DistributionCollector::OUTPUT_TYPE_CUMULATIVE:
      return "OUTPUT_TYPE_CUMULATIVE";
    default:
      return "";
    }
}


DistributionCollector::DistributionCollector ()
  : m_outputType (DistributionCollector::OUTPUT_TYPE_HISTOGRAM),
    m_minValue (0.0),
    m_maxValue (0.0),
    m_binLength (0.0),
    m_isInitialized (false),
    m_bins (0)
{
  NS_LOG_FUNCTION (this << GetName ());

  // Delay the initialization, giving the user the chance to set the attributes.
  Simulator::ScheduleNow (&DistributionCollector::InitializeBins, this);
}


TypeId // static
DistributionCollector::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::DistributionCollector")
    .SetParent<DataCollectionObject> ()
    .AddConstructor<DistributionCollector> ()
    .AddAttribute ("MinValue",
                   "The smallest sample value accepted by this collector. "
                   "Input samples less than this value will be filed as the "
                   "first bin.",
                   DoubleValue (std::numeric_limits<double>::max ()),
                   MakeDoubleAccessor (&DistributionCollector::SetMinValue,
                                       &DistributionCollector::GetMinValue),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MaxValue",
                   "The largest sample value accepted by this collector. "
                   "Input samples less than this value will be filed as the "
                   "last bin.",
                   DoubleValue (-std::numeric_limits<double>::max ()),
                   MakeDoubleAccessor (&DistributionCollector::SetMaxValue,
                                       &DistributionCollector::GetMaxValue),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("BinLength",
                   "The length of each bin category, which has also a counter "
                   "to keep track of the number of times samples have occurred "
                   "within the bin's range.",
                   DoubleValue (-1.0),
                   MakeDoubleAccessor (&DistributionCollector::SetBinLength,
                                       &DistributionCollector::GetBinLength),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("OutputType",
                   "Determines the mechanism of processing the incoming samples.",
                   EnumValue (DistributionCollector::OUTPUT_TYPE_HISTOGRAM),
                   MakeEnumAccessor (&DistributionCollector::SetOutputType,
                                     &DistributionCollector::GetOutputType),
                   MakeEnumChecker (DistributionCollector::OUTPUT_TYPE_HISTOGRAM,   "HISTOGRAM",
                                    DistributionCollector::OUTPUT_TYPE_PROBABILITY, "PROBABILITY",
                                    DistributionCollector::OUTPUT_TYPE_CUMULATIVE,  "CUMULATIVE"))
    .AddTraceSource ("Output",
                     "A bin identifier and the value corresponding to that bin. "
                     "Emitted upon the instance's destruction.",
                     MakeTraceSourceAccessor (&DistributionCollector::m_output))
    .AddTraceSource ("OutputCount",
                     "The number of received samples. "
                     "Emitted upon the instance's destruction.",
                     MakeTraceSourceAccessor (&DistributionCollector::m_outputCount))
    .AddTraceSource ("OutputSum",
                     "The sum of the received samples. "
                     "Emitted upon the instance's destruction.",
                     MakeTraceSourceAccessor (&DistributionCollector::m_outputSum))
    .AddTraceSource ("OutputMin",
                     "The minimum value from the received samples. "
                     "Emitted upon the instance's destruction.",
                     MakeTraceSourceAccessor (&DistributionCollector::m_outputMin))
    .AddTraceSource ("OutputMax",
                     "The maximum value from the received samples. "
                     "Emitted upon the instance's destruction.",
                     MakeTraceSourceAccessor (&DistributionCollector::m_outputMax))
    .AddTraceSource ("OutputMean",
                     "The mean of the received samples. "
                     "Emitted upon the instance's destruction.",
                     MakeTraceSourceAccessor (&DistributionCollector::m_outputMean))
    .AddTraceSource ("OutputStddev",
                     "The standard deviation of the received samples. "
                     "Emitted upon the instance's destruction.",
                     MakeTraceSourceAccessor (&DistributionCollector::m_outputStddev))
    .AddTraceSource ("OutputVariance",
                     "The variance of the received samples. "
                     "Emitted upon the instance's destruction.",
                     MakeTraceSourceAccessor (&DistributionCollector::m_outputVariance))
    .AddTraceSource ("OutputSqrSum",
                     "The sum of squares of the received samples. "
                     "Emitted upon the instance's destruction.",
                     MakeTraceSourceAccessor (&DistributionCollector::m_outputSqrSum))
  ;
  return tid;
}


void
DistributionCollector::InitializeBins ()
{
  NS_LOG_FUNCTION (this << GetName ());

  if (m_minValue >= m_maxValue)
    {
      NS_FATAL_ERROR ("MinValue (" << m_minValue << ") must be less than MaxValue (" << m_maxValue << ").");
    }
  else if (m_binLength <= 0)
    {
      NS_FATAL_ERROR ("BinLength (" << m_binLength << ") must be greater than zero.");
    }
  else
    {
      const double range = m_maxValue - m_minValue;
      NS_ASSERT (range > 0);

      if (range < m_binLength)
        {
          NS_LOG_WARN (this << "Only one bin is created; this statistics would look funny.");
        }

      m_bins = new Bins (m_minValue, m_maxValue, m_binLength);

      /*
       * Update this class' attributes in case there is some changes, e.g.,
       * when the MaxValue is extended because the last bin is too short.
       */
      m_minValue = m_bins->GetMinValue ();
      m_maxValue = m_bins->GetMaxValue ();
      m_binLength = m_bins->GetBinLength ();

      m_isInitialized = true;
    }
}


void
DistributionCollector::DoDispose ()
{
  NS_LOG_FUNCTION (this << GetName ());

  if (IsEnabled ())
    {
      // Compute output for `Output` trace source.

      switch (m_outputType)
        {
        case DistributionCollector::OUTPUT_TYPE_HISTOGRAM:
          {
            for (uint32_t i = 0; i < m_bins->GetNumOfBins (); i++)
              {
                m_output (m_bins->GetCenterOfBin (i),
                          static_cast<double> (m_bins->GetCountOfBin (i)));
              }
            break;
          }

        case DistributionCollector::OUTPUT_TYPE_PROBABILITY:
          {
            const uint32_t n = m_calculator.getCount ();
            double p = 0.0;
            for (uint32_t i = 0; i < m_bins->GetNumOfBins (); i++)
              {
                p = static_cast<double> (m_bins->GetCountOfBin (i)) / n;
                m_output (m_bins->GetCenterOfBin (i), p);
              }
            break;
          }

        case DistributionCollector::OUTPUT_TYPE_CUMULATIVE:
          {
            const uint32_t n = m_calculator.getCount ();
            double c = 0.0;
            double p = 0.0;
            for (uint32_t i = 0; i < m_bins->GetNumOfBins (); i++)
              {
                p = static_cast<double> (m_bins->GetCountOfBin (i)) / n;
                c += p;
                m_output (m_bins->GetCenterOfBin (i), c);
              }
            break;
          }

        default:
          break;

        } // end of `switch (m_outputType)`

      // Other trace sources are taken from the online calculator.

      m_outputCount (m_calculator.getCount ());
      m_outputSum (m_calculator.getSum ());
      m_outputMin (m_calculator.getMin ());
      m_outputMax (m_calculator.getMax ());
      m_outputMean (m_calculator.getMean ());
      m_outputStddev (m_calculator.getStddev ());
      m_outputVariance (m_calculator.getVariance ());
      m_outputSqrSum (m_calculator.getSqrSum ());

    } // end of `if (IsEnabled ())`

  if (m_isInitialized)
    {
      delete m_bins;
      m_bins = 0;
    }

} // end of `void DoDispose ()`


// ATTRIBUTE SETTERS AND GETTERS //////////////////////////////////////////////

void
DistributionCollector::SetMinValue (double minValue)
{
  NS_LOG_FUNCTION (this << GetName () << minValue);
  m_minValue = minValue;
}


double
DistributionCollector::GetMinValue () const
{
  return m_minValue;
}


void
DistributionCollector::SetMaxValue (double maxValue)
{
  NS_LOG_FUNCTION (this << GetName () << maxValue);
  m_maxValue = maxValue;
}


double
DistributionCollector::GetMaxValue () const
{
  return m_maxValue;
}


void
DistributionCollector::SetBinLength (double binLength)
{
  NS_LOG_FUNCTION (this << GetName () << binLength);
  m_binLength = binLength;
}


double
DistributionCollector::GetBinLength () const
{
  return m_binLength;
}


void
DistributionCollector::SetOutputType (OutputType_t outputType)
{
  NS_LOG_FUNCTION (this << GetName () << GetOutputTypeName (outputType));
  m_outputType = outputType;
}


DistributionCollector::OutputType_t
DistributionCollector::GetOutputType () const
{
  return m_outputType;
}


// TRACE SINKS ////////////////////////////////////////////////////////////////

void
DistributionCollector::TraceSinkDouble (double oldData, double newData)
{
  NS_LOG_FUNCTION (this << GetName () << newData);

  if (!m_isInitialized)
    {
      NS_FATAL_ERROR ("This collector instance has not been initialized yet.");
    }

  if (IsEnabled ())
    {
      m_bins->NewSample (newData);
      m_calculator.Update (newData);
    }
}


void
DistributionCollector::TraceSinkInteger8 (int8_t oldData, int8_t newData)
{
  TraceSinkDouble (0, static_cast<double> (newData));
  // Note: old data is discarded.
}


void
DistributionCollector::TraceSinkInteger16 (int16_t oldData, int16_t newData)
{
  TraceSinkDouble (0, static_cast<double> (newData));
  // Note: old data is discarded.
}


void
DistributionCollector::TraceSinkInteger32 (int32_t oldData, int32_t newData)
{
  TraceSinkDouble (0, static_cast<double> (newData));
  // Note: old data is discarded.
}


void
DistributionCollector::TraceSinkInteger64 (int64_t oldData, int64_t newData)
{
  TraceSinkDouble (0, static_cast<double> (newData));
  // Note: old data is discarded.
}


void
DistributionCollector::TraceSinkUinteger8 (uint8_t oldData, uint8_t newData)
{
  TraceSinkDouble (0, static_cast<double> (newData));
  // Note: old data is discarded.
}


void
DistributionCollector::TraceSinkUinteger16 (uint16_t oldData, uint16_t newData)
{
  TraceSinkDouble (0, static_cast<double> (newData));
  // Note: old data is discarded.
}


void
DistributionCollector::TraceSinkUinteger32 (uint32_t oldData, uint32_t newData)
{
  TraceSinkDouble (0, static_cast<double> (newData));
  // Note: old data is discarded.
}


void
DistributionCollector::TraceSinkUinteger64 (uint64_t oldData, uint64_t newData)
{
  TraceSinkDouble (0, static_cast<double> (newData));
  // Note: old data is discarded.
}


// BINS CLASS METHOD DEFINITION ///////////////////////////////////////////////

DistributionCollector::Bins::Bins (double minValue,
                                   double maxValue,
                                   double binLength)
  : m_minValue (minValue),
    m_maxValue (maxValue),
    m_binLength (binLength)
{
  NS_ASSERT (m_minValue < m_maxValue);
  NS_ASSERT (m_binLength > 0.0);

  const double initialRange = m_maxValue - m_minValue;
  //NS_ASSERT (initialRange > 0.0);
  NS_LOG_DEBUG (this << " InitialRange=" << initialRange);

  m_numOfBins = static_cast<uint32_t> (std::floor (initialRange / m_binLength));
  NS_LOG_DEBUG (this << " NumOfBins=" << m_numOfBins);

  double newRange = static_cast<double> (m_numOfBins) * m_binLength;
  if (newRange < initialRange)
    {
      // Extend by 1 more bin.
      newRange += m_binLength;
      m_numOfBins++;
    }
  NS_ASSERT (newRange >= initialRange);
  NS_LOG_DEBUG (this << " NewRange=" << newRange);

  // Extend the maximum value.
  m_maxValue = m_minValue + newRange;
  //NS_ASSERT (m_maxValue > m_minValue);
  NS_LOG_DEBUG (this << " new MaxValue=" << m_maxValue);

  m_bins.resize (m_numOfBins, 0);
}


double
DistributionCollector::Bins::GetMinValue () const
{
  return m_minValue;
}


double
DistributionCollector::Bins::GetMaxValue () const
{
  return m_maxValue;
}


double
DistributionCollector::Bins::GetBinLength () const
{
  return m_binLength;
}


uint32_t
DistributionCollector::Bins::GetNumOfBins () const
{
  return m_numOfBins;
}

void
DistributionCollector::Bins::NewSample (double newSample)
{
  const uint32_t binIndex = DetermineBin (newSample);
  NS_ASSERT_MSG (binIndex < m_numOfBins, "Out of bound bin index " << binIndex);
  m_bins[binIndex] += 1;
}


uint32_t
DistributionCollector::Bins::GetCountOfBin (uint32_t binIndex) const
{
  NS_ASSERT (binIndex < m_numOfBins);
  return m_bins[binIndex];
}


double
DistributionCollector::Bins::GetCenterOfBin (uint32_t binIndex) const
{
  NS_ASSERT (binIndex < m_numOfBins);
  const double binStart = m_minValue + (static_cast<double> (binIndex) * m_binLength);
  NS_ASSERT (binStart < m_maxValue);
  const double binCenter = binStart + (m_binLength / 2.0);
  NS_ASSERT (binCenter < m_maxValue);
  return binCenter;
}


uint32_t
DistributionCollector::Bins::DetermineBin (double sample) const
{
  if (sample < m_minValue)
    {
      return 0;  // Sample less than the minimum value goes to the first bin.
    }
  else if (sample < m_maxValue)
    {
      const double binIndex = std::floor ((sample - m_minValue) / m_binLength);
      return static_cast<uint32_t> (binIndex);
    }
  else
    {
      //NS_ASSERT (sample >= m_maxValue);
      // Sample equal or greater than the maximum value goes to the last bin.
      return m_numOfBins - 1;
    }
}


} // end of namespace ns3
