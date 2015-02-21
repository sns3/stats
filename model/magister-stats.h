/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Magister Solutions
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

#ifndef MAGISTER_STATS_H
#define MAGISTER_STATS_H

namespace ns3 {

/**
 * \brief Common signature used by callback to collector's trace source.
 * \param value the value generated by the collector.
 */
typedef void (* CollectorOutputCallback) (double value);

/**
 * \brief Common signature used by callback to collector's trace source.
 * \param time the current simulation time, using unit determined by the
 *             `TimeUnit` attribute of the collector class, or in seconds if
 *             the collector class doesn't have such attribute.
 * \param value the value generated by the collector.
 */
typedef void (* CollectorTimedOutputCallback) (double time, double value);

/**
 * \brief Common signature used by callback to collector's trace source.
 * \param x the current simulation time, using unit determined by the
 *             `TimeUnit` attribute of the collector class, or in seconds if
 *             the collector class doesn't have such attribute.
 * \param y the value generated by the collector.
 */
typedef void (* Collector2dOutputCallback) (double time, double value);

/**
 * \brief Common signature used by callback to collector's trace source.
 * \param info various setup and statistical information used by the collector
 *             to produce its output.
 */
typedef void (* CollectorInformationCallback) (std::string info);

} // end of namespace ns3


#endif /* MAGISTER_STATS_H */
