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

#ifndef DISTRIBUTION_COLLECTOR_H
#define DISTRIBUTION_COLLECTOR_H

#include <ns3/data-collection-object.h>
#include <ns3/traced-callback.h>
#include <ns3/basic-data-calculators.h>
#include <vector>

namespace ns3 {


/**
 * \ingroup aggregator
 * \brief Collector which computes the value distribution of the input samples.
 *
 * ### Input ###
 * This class provides 9 trace sinks for receiving inputs. Each trace sink
 * is a function with a signature similar to the following:
 * \code
 *   void TraceSinkP (P oldData, P newData);
 * \endcode
 * where `P` is one of the 9 supported data types. This type of signature
 * follows the trace source signature types commonly exported by probes.
 * Although different data types are accepted, they are all internally
 * processed using `double` data type.
 *
 * ### Processing ###
 * This class begins by setting up a set of _bins_. Each bin covers an equal
 * length of input value range which does not overlap with the range of other
 * bins. Each received input sample is categorized into exactly one of these
 * bins. In this case, that bin's counter is increased by one. At the end of
 * the simulation, the bins would represent the distribution information of all
 * the received samples.
 *
 * The setup of the bins can be configured through the `MinValue`, `MaxValue`,
 * and `BinLength` attributes. The corresponding methods SetMinValue(),
 * SetMaxValue(), and SetBinLength() can also be used for the same purpose.
 * If the last bin ends up shorter than the rest, then its range is simply
 * extended beyond the `MaxValue` to match the length of the other bins.
 *
 * \warning There is no valid default value for these attributes, so they must
 *          be set before the simulation begins, or else the class will raise
 *          an error.
 *
 * Input values which do not fit into the range of all the bins are handled as
 * follows.
 * - Input values which are less than `MinValue` are categorized into the first
 *   bin.
 * - Input values which are equal or greater than `MaxValue` are categorized
 *   into the last bin.
 *
 * \see DistributionCollector::Bins
 *
 * \todo To allow alternative configuration using `NumberOfBins` attribute
 *       instead of `BinLength` alternative.
 *
 * ### Output ###
 * At the end of the instance's life (e.g., when the simulation ends), the
 * `Output` trace source is fired, typically several times in a row, to export
 * the output. Each time the trace source is fired, it contains the bin
 * identifier (i.e., the center value of the bin) and the corresponding value
 * of that bin. The bin value is determined by the selected output type, which
 * can be modified by calling the SetOutputType() method or setting the
 * `OutputType` attribute. The burst of output is guaranteed to be in order
 * from the first bin (the lowest identifier) until the last bin.
 *
 * In addition, the class also computes several statistical information and
 * export them as output trace sources.
 * - `OutputCount`
 * - `OutputSum`
 * - `OutputMin`
 * - `OutputMax`
 * - `OutputMean`
 * - `OutputStddev`
 * - `OutputVariance`
 * - `OutputSqrSum`
 *
 * Finally, when the OUTPUT_TYPE_CUMULATIVE is selected as the output type, the
 * class also includes percentile information in the following trace sources.
 * - `Output5thPercentile`
 * - `Output25thPercentile`
 * - `Output50thPercentile`
 * - `Output75thPercentile`
 * - `Output95thPercentile`
 * Note that linear interpolation is used to calculate these percentile
 * information, and thus might have some errors.
 *
 * All the additional statistical and percentile trace sources mentioned above
 * are also emitted in string format through the `OutputString` trace source.
 * The resulting string also includes the parameters used to collect the samples
 * (e.g., the `MinValue`, `MaxValue`, and `BinLength` attributes). Example
 * `OutputString` output:
 * \code
 * % min_value: 0
 * % max_value: 1
 * % bin_length: 0.02
 * % num_of_bins: 50
 * % output_type: 'OUTPUT_TYPE_CUMULATIVE'
 * % count: 9
 * % sum: 4.40882
 * % min: 0.258985
 * % max: 1.29714
 * % mean: 0.489869
 * % stddev: 0.457671
 * % variance: 0.209463
 * % sqr_sum: 3.83545
 * % percentile_5: 0.2315
 * % percentile_25: 0.2375
 * % percentile_50: 0.245
 * % percentile_75: 0.265
 * % percentile_95: 0.9855
 * \endcode
 */
class DistributionCollector : public DataCollectionObject
{
public:
  /**
   * \enum OutputType_t
   * \brief Type of output supported by this class.
   */
  typedef enum
  {
    /**
     * Number of samples from each bin is presented as it is (i.e., absolute
     * value).
     */
    OUTPUT_TYPE_HISTOGRAM = 0,
    /**
     * Number of samples from each bin is presented as a value relative to
     * the total number of samples (i.e., ranging between 0.0 and 1.0). Thus,
     * producing a probability distribution function (PDF).
     */
    OUTPUT_TYPE_PROBABILITY,
    /**
     * The values associated with each bin is the sum of number of samples from
     * that bin and all the preceding bins, presented as a value relative to
     * the total number of samples (i.e., ranging between 0.0 and 1.0). Thus,
     * producing a cumulative distribution function (CDF).
     */
    OUTPUT_TYPE_CUMULATIVE
  } OutputType_t;

  /**
   * \param outputType an arbitrary output type.
   * \return representation of the output type in string.
   */
  static std::string GetOutputTypeName (OutputType_t outputType);

  /// Creates a new collector instance.
  DistributionCollector ();

  // inherited from ObjectBase base class
  static TypeId GetTypeId ();

  // ATTRIBUTE SETTERS AND GETTERS ////////////////////////////////////////////

  /**
   * \param minValue the smallest value accepted by this collector.
   */
  void SetMinValue (double minValue);

  /**
   * \return the smallest value accepted by this collector.
   */
  double GetMinValue () const;

  /**
   * \param maxValue the largest value accepted by this collector.
   */
  void SetMaxValue (double maxValue);

  /**
   * \return the largest value accepted by this collector.
   */
  double GetMaxValue () const;

  /**
   * \param binLength a positive number indicating the length of each bin
   *                  category.
   */
  void SetBinLength (double binLength);

  /**
   * \return the length of each bin category.
   */
  double GetBinLength () const;

  /**
   * \param outputType the processing mechanism used by this instance.
   */
  void SetOutputType (OutputType_t outputType);

  /**
   * \return the processing mechanism used by this instance.
   */
  OutputType_t GetOutputType () const;

  // TRACE SINKS //////////////////////////////////////////////////////////////

  /**
   * \brief Trace sink for receiving data from `double` valued trace sources.
   * \param newData the new value.
   *
   * This method serves as a trace sink to `double` valued trace sources.
   */
  void TraceSinkDouble1 (double newData);

  /**
   * \brief Trace sink for receiving data from `double` valued trace sources.
   * \param oldData the original value.
   * \param newData the new value.
   *
   * This method serves as a trace sink to `double` valued trace sources.
   */
  void TraceSinkDouble (double oldData, double newData);

  /**
   * \brief Trace sink for receiving data from `int8_t` valued trace sources.
   * \param oldData the original value.
   * \param newData the new value.
   *
   * This method serves as a trace sink to `int8_t` valued trace sources.
   * The data will be converted to double and then simply passed to the
   * TraceSinkDouble() method.
   */
  void TraceSinkInteger8 (int8_t oldData, int8_t newData);

  /**
   * \brief Trace sink for receiving data from `int16_t` valued trace sources.
   * \param oldData the original value.
   * \param newData the new value.
   *
   * This method serves as a trace sink to `int16_t` valued trace sources.
   * The data will be converted to double and then simply passed to the
   * TraceSinkDouble() method.
   */
  void TraceSinkInteger16 (int16_t oldData, int16_t newData);

  /**
   * \brief Trace sink for receiving data from `int32_t` valued trace sources.
   * \param oldData the original value.
   * \param newData the new value.
   *
   * This method serves as a trace sink to `int32_t` valued trace sources.
   * The data will be converted to double and then simply passed to the
   * TraceSinkDouble() method.
   */
  void TraceSinkInteger32 (int32_t oldData, int32_t newData);

  /**
   * \brief Trace sink for receiving data from `int64_t` valued trace sources.
   * \param oldData the original value.
   * \param newData the new value.
   *
   * This method serves as a trace sink to `int64_t` valued trace sources.
   * The data will be converted to double and then simply passed to the
   * TraceSinkDouble() method.
   */
  void TraceSinkInteger64 (int64_t oldData, int64_t newData);

  /**
   * \brief Trace sink for receiving data from `uint8_t` valued trace sources.
   * \param oldData the original value.
   * \param newData the new value.
   *
   * This method serves as a trace sink to `uint8_t` valued trace sources.
   * The data will be converted to double and then simply passed to the
   * TraceSinkDouble() method.
   */
  void TraceSinkUinteger8 (uint8_t oldData, uint8_t newData);

  /**
   * \brief Trace sink for receiving data from `uint16_t` valued trace sources.
   * \param oldData the original value.
   * \param newData the new value.
   *
   * This method serves as a trace sink to `uint16_t` valued trace sources.
   * The data will be converted to double and then simply passed to the
   * TraceSinkDouble() method.
   */
  void TraceSinkUinteger16 (uint16_t oldData, uint16_t newData);

  /**
   * \brief Trace sink for receiving data from `uint32_t` valued trace sources.
   * \param oldData the original value.
   * \param newData the new value.
   *
   * This method serves as a trace sink to `uint32_t` valued trace sources.
   * The data will be converted to double and then simply passed to the
   * TraceSinkDouble() method.
   */
  void TraceSinkUinteger32 (uint32_t oldData, uint32_t newData);

  /**
   * \brief Trace sink for receiving data from `uint64_t` valued trace sources.
   * \param oldData the original value.
   * \param newData the new value.
   *
   * This method serves as a trace sink to `uint64_t` valued trace sources.
   * The data will be converted to double and then simply passed to the
   * TraceSinkDouble() method.
   */
  void TraceSinkUinteger64 (uint64_t oldData, uint64_t newData);

protected:
  // Inherited from Object base class
  virtual void DoDispose ();

private:
  /**
   * \internal
   * Automatically invoked at the beginning of simulation. Responsible for
   * creating the required set of bins based on the given `MinValue`,
   * `MaxValue`, and `BinLength` parameters.
   */
  void InitializeBins ();

  /**
   * \internal
   * \code
   *   Y
   *   ^        + (x2, y2)
   *   |       /
   *   |      + (x1, y1)
   *   |     /
   *   |    /
   *   |   + (x0, y0)
   *   |
   *   +-----------------> X
   * \endcode
   *
   * We assume \f$x_0\f$ and \f$x_2\f$ are the center of two adjacent bins.
   * Thus, it follows that \f$x_2 - x_0\f$ equals to bin length \f$b\f$. The
   * formula for computing \f$x_1\f$ is therefore:
   *
   *    \f[
   *    x_1 = x_0 + (\frac{y_1 - y_0}{y_2 - y_0} \times b)
   *    \f]
   */
  double GetInterpolatedX1 (double x0, double y0, double y1, double y2) const;

  OutputType_t  m_outputType;      ///< `OutputType` attribute.
  double        m_minValue;        ///< `MinValue` attribute.
  double        m_maxValue;        ///< `MaxValue` attribute.
  double        m_binLength;       ///< `BinLength` attribute.
  bool          m_isInitialized;   ///< True after InitializeBins().

  TracedCallback<double, double> m_output;        ///< `Output` trace source.
  TracedCallback<std::string> m_outputString;     ///< `OutputString` trace source.

  TracedCallback<double> m_output5thPercentile;   ///< `Output5thPercentile` trace source.
  TracedCallback<double> m_output25thPercentile;  ///< `Output25thPercentile` trace source.
  TracedCallback<double> m_output50thPercentile;  ///< `Output50thPercentile` trace source.
  TracedCallback<double> m_output75thPercentile;  ///< `Output75thPercentile` trace source.
  TracedCallback<double> m_output95thPercentile;  ///< `Output95thPercentile` trace source.

  TracedCallback<uint32_t> m_outputCount;   ///< `OutputCount` trace source.
  TracedCallback<double> m_outputSum;       ///< `OutputSum` trace source.
  TracedCallback<double> m_outputMin;       ///< `OutputMin` trace source.
  TracedCallback<double> m_outputMax;       ///< `OutputMax` trace source.
  TracedCallback<double> m_outputMean;      ///< `OutputMean` trace source.
  TracedCallback<double> m_outputStddev;    ///< `OutputStddev` trace source.
  TracedCallback<double> m_outputVariance;  ///< `OutputVariance` trace source.
  TracedCallback<double> m_outputSqrSum;    ///< `OutputSqrSum` trace source.

  /// Tools for online computing of most of the statistical information.
  MinMaxAvgTotalCalculator<double> m_calculator;

  /**
   * \brief A set of bins utilized by DistributionCollector.
   *
   * Example bins illustration for `min` = 0.0, `max` = 5.0, `binLength` = 1.0:
   * \code
   *             0.0     1.0     2.0     3.0     4.0     5.0
   *              +-------+-------+-------+-------+-------+
   *              |       |       |       |       |       |
   *              +-------+-------+-------+-------+-------+
   * Bin index:       0       1       2       3       4
   * Bin center:     0.5     1.5     2.5     3.5     4.5
   * \endcode
   *
   * Example bins illustration for `min` = 0.0, `max` = 7.0, `binLength` = 2.0,
   * where `max` gets extended to 8.0 to ensure all bins are of equal length:
   * \code
   *             0.0           2.0           4.0           6.0           8.0
   *              +-------------+-------------+-------------+-------------+
   *              |             |             |             |             |
   *              +-------------+-------------+-------------+-------------+
   * Bin index:          0             1             2             3
   * Bin center:        1.0           3.0           5.0           7.0
   * \endcode
   */
  class Bins
  {
  public:
    /**
     * \brief Create a set of empty bins.
     * \param minValue the lower bound of the first bin.
     * \param maxValue the upper bound of the last bin.
     * \param binLength a positive number indicating the length of each bin.
     *
     * May extend the upper bound of the last bin to enforce same length to all
     * bins.
     */
    Bins (double minValue, double maxValue, double binLength);
    /// \return the lower bound of the first bin.
    double GetMinValue () const;
    /// \return the upper bound of the last bin.
    double GetMaxValue () const;
    /// \return the length of each bin.
    double GetBinLength () const;
    /// \return the number of bins maintained in this instance.
    uint32_t GetNumOfBins () const;
    /// Increase the counter of the bin associated with the given sample by 1.
    void NewSample (double newSample);
    /// \return the current value of the counter of a certain bin.
    uint32_t GetCountOfBin (uint32_t binIndex) const;
    /// \return the sum of lower bound and upper bound divided by two.
    double GetCenterOfBin (uint32_t binIndex) const;
    /**
     * \param sample a new sample.
     * \return the bin index where the given sample should belong to.
     */
    uint32_t DetermineBin (double sample) const;
  private:
    double                 m_minValue;   ///< The lower bound of the first bin.
    double                 m_maxValue;   ///< The upper bound of the last bin.
    double                 m_binLength;  ///< The length of each bin.
    uint32_t               m_numOfBins;  ///< The number of bins.
    std::vector<uint32_t>  m_bins;       ///< Internal bins representation.

  }; // end of class Bins

  /// The bin categories.
  Bins *m_bins;

}; // end of class DistributionCollector


} // end of namespace ns3


#endif /* DISTRIBUTION_COLLECTOR_H */
