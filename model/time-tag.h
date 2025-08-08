/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Magister Solutions Ltd.
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
 * Author: Frans Laakso <frans.laakso@magister.fi>
 */

#ifndef TIME_TAG_H
#define TIME_TAG_H

#include "ns3/nstime.h"
#include "ns3/tag.h"

#include <ostream>

namespace ns3
{

/*
 * There are 3 classes defined here: PhyTimeTag, MacTimeTag,
 * and DevTimeTag. Except of the name difference, they share
 * exactly the same definitions.
 */

/**
 * @ingroup magister-stats
 * @brief Time tag used to identify the time when packet is enqueued at PHY
 *        level.
 */
class PhyTimeTag : public Tag
{
  public:
    /**
     * @brief Get the type ID
     * @return the object TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * @brief Get the type ID of instance
     * @return the object TypeId
     */
    virtual TypeId GetInstanceTypeId(void) const;

    /**
     * Default constructor.
     */
    PhyTimeTag();

    /**
     * Constructor with initialization parameters.
     * @param senderTimestamp
     */
    PhyTimeTag(Time senderTimestamp);

    /**
     * Serializes information to buffer from this instance of PhyTimeTag
     * @param i Buffer in which the information is serialized
     */
    virtual void Serialize(TagBuffer i) const;

    /**
     * Deserializes information from buffer to this instance of PhyTimeTag
     * @param i Buffer from which the information is deserialized
     */
    virtual void Deserialize(TagBuffer i);

    /**
     * Get serialized size of PhyTimeTag
     * @return Serialized size in bytes
     */
    virtual uint32_t GetSerializedSize() const;

    /**
     * Print time stamp of this instance of PhyTimeTag
     * @param &os Output stream to which tag timestamp is printed.
     */
    virtual void Print(std::ostream& os) const;

    /**
     * Get sender time stamp of this tag.
     * @return Sender timestamp
     */
    Time GetSenderTimestamp(void) const;

    /**
     * Set sender time stamp of this tag.
     * @param senderTimestamp Sender time stamp
     */
    void SetSenderTimestamp(Time senderTimestamp);

  private:
    Time m_senderTimestamp;
};

/**
 * @ingroup magister-stats
 * @brief Time tag used to identify the time when packet is enqueued at MAC
 *        level.
 */
class MacTimeTag : public Tag
{
  public:
    /**
     * @brief Get the type ID
     * @return the object TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * @brief Get the type ID of instance
     * @return the object TypeId
     */
    virtual TypeId GetInstanceTypeId(void) const;

    /**
     * Default constructor.
     */
    MacTimeTag();

    /**
     * Constructor with initialization parameters.
     * @param senderTimestamp
     */
    MacTimeTag(Time senderTimestamp);

    /**
     * Serializes information to buffer from this instance of MacTimeTag
     * @param i Buffer in which the information is serialized
     */
    virtual void Serialize(TagBuffer i) const;

    /**
     * Deserializes information from buffer to this instance of MacTimeTag
     * @param i Buffer from which the information is deserialized
     */
    virtual void Deserialize(TagBuffer i);

    /**
     * Get serialized size of MacTimeTag
     * @return Serialized size in bytes
     */
    virtual uint32_t GetSerializedSize() const;

    /**
     * Print time stamp of this instance of MacTimeTag
     * @param &os Output stream to which tag timestamp is printed.
     */
    virtual void Print(std::ostream& os) const;

    /**
     * Get sender time stamp of this tag.
     * @return Sender timestamp
     */
    Time GetSenderTimestamp(void) const;

    /**
     * Set sender time stamp of this tag.
     * @param senderTimestamp Sender time stamp
     */
    void SetSenderTimestamp(Time senderTimestamp);

  private:
    Time m_senderTimestamp;
};

/**
 * @ingroup magister-stats
 * @brief Time tag used to identify the time when packet is enqueued at device
 *        level.
 */
class DevTimeTag : public Tag
{
  public:
    /**
     * @brief Get the type ID
     * @return the object TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * @brief Get the type ID of instance
     * @return the object TypeId
     */
    virtual TypeId GetInstanceTypeId(void) const;

    /**
     * Default constructor.
     */
    DevTimeTag();

    /**
     * Constructor with initialization parameters.
     * @param senderTimestamp
     */
    DevTimeTag(Time senderTimestamp);

    /**
     * Serializes information to buffer from this instance of DevTimeTag
     * @param i Buffer in which the information is serialized
     */
    virtual void Serialize(TagBuffer i) const;

    /**
     * Deserializes information from buffer to this instance of DevTimeTag
     * @param i Buffer from which the information is deserialized
     */
    virtual void Deserialize(TagBuffer i);

    /**
     * Get serialized size of DevTimeTag
     * @return Serialized size in bytes
     */
    virtual uint32_t GetSerializedSize() const;

    /**
     * Print time stamp of this instance of DevTimeTag
     * @param &os Output stream to which tag timestamp is printed.
     */
    virtual void Print(std::ostream& os) const;

    /**
     * Get sender time stamp of this tag.
     * @return Sender timestamp
     */
    Time GetSenderTimestamp(void) const;

    /**
     * Set sender time stamp of this tag.
     * @param senderTimestamp Sender time stamp
     */
    void SetSenderTimestamp(Time senderTimestamp);

  private:
    Time m_senderTimestamp;
};

} // namespace ns3

#endif /* TIME_TAG_H */
