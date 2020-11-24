/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 INRIA, 2008 Timo Bingmann
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
 * Author: Bastien Tauran <bastien.tauran@viveris.fr>
 */
#ifndef STATS_GNUPLOT_H
#define STATS_GNUPLOT_H

#include <string>
#include <vector>
#include <utility>

#include "ns3/gnuplot.h"

namespace ns3 {

/**
 * TODO
 * \ingroup StatsGnuplot
 *
 * \brief a simple class to generate StatsGnuplot-ready plotting commands
 *        from a set of datasets.
 *
 * This class really represents a single graph on which multiple datasets
 * can be plotted.
 */
class StatsGnuplot : public Gnuplot
{
public:
  /**
   * TODO
   * \param outputFilename the name of the file where the rendering of the
   *        graph will be generated if you feed the command stream output by
   *        Gnuplot::GenerateOutput to the gnuplot program.
   * \param title title line of the plot page 
   */
  StatsGnuplot (const std::string& outputFilename="", const std::string& title = "");

  /**
   * TODO
   * \param osControl the output stream on which the relevant StatsGnuplot
   * control commands should be generated. Including output file and
   * terminal headers.
   * \param osData the output stream on which the relevant StatsGnuplot
   * data values should be generated.
   * \param dataFileName the name for the data file that will be
   * written.
   *
   * \brief Writes StatsGnuplot commands and data values to two
   * different outputs streams.
   */
  void GetDataStream (std::string dataFileName, std::ofstream *ofs);
};

} // namespace ns3

#endif /* STATS_GNUPLOT_H */
