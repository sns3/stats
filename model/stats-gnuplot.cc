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

#include "stats-gnuplot.h"
#include "ns3/assert.h"
#include <fstream>
#include <ostream>
#include <stdexcept>

namespace ns3 {

StatsGnuplot::StatsGnuplot (const std::string& outputFilename, const std::string& title)
  : Gnuplot (outputFilename, title)
{
}

// Set dataFileName argument
void
StatsGnuplot::GetDataStream (std::string dataFileName, std::ofstream *ofs)
{
  ofs->open (dataFileName, std::ios::out | std::ios::app);
  if (!(*ofs) || !(ofs->is_open ()))
    {
      NS_FATAL_ERROR ("Error creating file " << dataFileName << " for output");
    }
}

} // namespace ns3
