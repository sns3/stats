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

#ifndef SCALAR_COLLECTOR_H
#define SCALAR_COLLECTOR_H

#include <ns3/data-collection-object.h>
#include <ns3/traced-callback.h>
#include <ns3/nstime.h>


namespace ns3 {

/**
 * \brief
 */
class ScalarCollector : public DataCollectionObject
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

  /**
   * \param inputDataType
   * \return
   */
  static std::string GetInputDataTypeName (InputDataType_t inputDataType);

  /**
   * \enum OutputType_t
   * \brief
   */
  typedef enum
  {
    OUTPUT_TYPE_SUM = 0,
    OUTPUT_TYPE_AVERAGE_PER_SAMPLE,
    OUTPUT_TYPE_AVERAGE_PER_SECOND
  } OutputType_t;

  /**
   * \param outputType
   * \return
   */
  static std::string GetOutputTypeName (OutputType_t outputType);

  ///
  ScalarCollector ();

  // inherited from ObjectBase base class
  static TypeId GetTypeId ();

  // ATTRIBUTE SETTERS AND GETTERS ////////////////////////////////////////////

  /**
   * \param inputDataType
   */
  void SetInputDataType (InputDataType_t inputDataType);

  /**
   * \return
   */
  InputDataType_t GetInputDataType () const;

  /**
   * \param outputType
   */
  void SetOutputType (OutputType_t outputType);

  /**
   * \return
   */
  OutputType_t GetOutputType () const;

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
  double    m_sumDouble;
  uint64_t  m_sumUinteger;
  uint32_t  m_numOfSamples;
  Time      m_firstSample;
  Time      m_lastSample;
  bool      m_hasReceivedSample;

  InputDataType_t  m_inputDataType;
  OutputType_t     m_outputType;

  TracedCallback<double> m_output;

}; // end of class ScalarCollector


} // end of namespace ns3


#endif /* SCALAR_COLLECTOR_H */
