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

#ifndef INTERVAL_RATE_COLLECTOR_H
#define INTERVAL_RATE_COLLECTOR_H

#include <ns3/data-collection-object.h>
#include <ns3/traced-callback.h>
#include <ns3/event-id.h>
#include <ns3/nstime.h>


namespace ns3 {


/**
 * \brief
 */
class IntervalRateCollector : public DataCollectionObject
{
public:
  /**
   * \enum InputDataType_t
   * \brief
   */
  typedef enum
  {
    INPUT_DATA_TYPE_DOUBLE = 0,
    INPUT_DATA_TYPE_UINTEGER
  } InputDataType_t;

  ///
  IntervalRateCollector ();

  // inherited from ObjectBase base class
  static TypeId GetTypeId ();

  // ATTRIBUTE SETTERS AND GETTERS ////////////////////////////////////////////

  /**
   * \param intervalLength
   * \warning Updating interval length after the simulation has started may
   *          produce undefined behaviour.
   */
  void SetIntervalLength (Time intervalLength);

  /**
   * \return
   */
  Time GetIntervalLength () const;

  /**
   * \param inputDataType
   */
  void SetInputDataType (InputDataType_t inputDataType);

  /**
   * \return
   */
  InputDataType_t GetInputDataType () const;

  /**
   * \param unit
   */
  void SetTimeUnit (Time::Unit unit);

  /**
   * \return
   */
  Time::Unit GetTimeUnit () const;

  // TRACE SINKS //////////////////////////////////////////////////////////////

  /**
   * \brief
   * \param oldData
   * \param newData
   */
  void TraceSinkDouble (double oldData, double newData);

  /**
   * \brief
   * \param oldData
   * \param newData
   */
  void TraceSinkUinteger8 (uint8_t oldData, uint8_t newData);

  /**
   * \brief
   * \param oldData
   * \param newData
   */
  void TraceSinkUinteger16 (uint16_t oldData, uint16_t newData);

  /**
   * \brief
   * \param oldData
   * \param newData
   */
  void TraceSinkUinteger32 (uint32_t oldData, uint32_t newData);

  /**
   * \brief
   * \param oldData
   * \param newData
   */
  void TraceSinkUinteger64 (uint64_t oldData, uint64_t newData);

protected:
  // Inherited from Object base class
  virtual void DoDispose ();

private:
  ///
  void FirstInterval ();

  ///
  void NewInterval ();

  double           m_intervalSumDouble;
  double           m_overallSumDouble;
  uint64_t         m_intervalSumUinteger;
  uint64_t         m_overallSumUinteger;
  Time             m_intervalLength;
  InputDataType_t  m_inputDataType;
  Time::Unit       m_timeUnit;
  EventId          m_nextReset;

  TracedCallback<double> m_outputOverall;
  TracedCallback<double, double> m_outputWithTime;
  TracedCallback<double> m_outputWithoutTime;

}; // end of class IntervalRateCollector


} // end of namespace ns3


#endif /* INTERVAL_RATE_COLLECTOR_H */
