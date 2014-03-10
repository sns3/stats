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

#ifndef UNIT_CONVERSION_COLLECTOR_H
#define UNIT_CONVERSION_COLLECTOR_H

#include <ns3/data-collection-object.h>
#include <ns3/traced-value.h>
#include <ns3/traced-callback.h>
#include <ns3/nstime.h>


namespace ns3 {

/**
 * \brief
 */
class UnitConversionCollector : public DataCollectionObject
{
public:
  /**
   * \enum ConversionType_t
   * \brief
   */
  typedef enum
  {
    TRANSPARENT = 0,
    FROM_BYTES_TO_BIT,
    FROM_BYTES_TO_KBIT,
    FROM_BYTES_TO_MBIT,
    FROM_SECONDS_TO_MS,
    FROM_LINEAR_TO_DB,
    FROM_LINEAR_TO_DBM
  } ConversionType_t;

  /**
   * \param conversionType
   * \return
   */
  static std::string GetConversionTypeName (ConversionType_t conversionType);

  ///
  UnitConversionCollector ();

  // inherited from ObjectBase base class
  static TypeId GetTypeId ();

  // ATTRIBUTE SETTERS AND GETTERS ////////////////////////////////////////////

  /**
   * \param conversionType
   */
  void SetConversionType (ConversionType_t conversionType);

  /**
   * \return
   */
  ConversionType_t GetConversionType () const;

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
  void TraceSinkInteger8 (int8_t oldData, int8_t newData);

  /**
   * \brief
   * \param oldData
   * \param newData
   */
  void TraceSinkInteger16 (int16_t oldData, int16_t newData);

  /**
   * \brief
   * \param oldData
   * \param newData
   */
  void TraceSinkInteger32 (int32_t oldData, int32_t newData);

  /**
   * \brief
   * \param oldData
   * \param newData
   */
  void TraceSinkInteger64 (int64_t oldData, int64_t newData);

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

  /**
   * \param original
   * \return
   */
  virtual double Convert (double original) const;

private:
  /// The conversion type used.
  ConversionType_t m_conversionType;

  ///
  Time::Unit m_timeUnit;

  /**
   * \brief Indicate that the next sample would be the first sample received.
   *
   * The first sample of data received from the probe usually contains
   * uninitialized old data values. This fact makes Valgrind unhappy. Because
   * of this, the trace sinks "force initialize" the first sample of this old
   * data value to zero.
   */
  bool m_isFirstSample;

  ///< Output trace source.
  TracedCallback<double, double> m_output;

  ///< OutputValue trace source.
  TracedCallback<double> m_outputValue;

  ///< OutputTimeValue trace source.
  TracedCallback<double, double> m_outputTimeValue;

}; // end of class UnitConversionCollector


} // end of namespace ns3


#endif /* UNIT_CONVERSION_COLLECTOR_H */
