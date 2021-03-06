/*
 * Copyright (c) 2014 Juniper Networks, Inc. All rights reserved.
 */

#ifndef __SFLOW_PARSER_H__
#define __SFLOW_PARSER_H__

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <sandesh/sandesh_trace.h>

#include "base/util.h"
#include "sflow.h"

struct SFlowFlowSampleData {
    SFlowFlowSample flow_sample;
    boost::ptr_map<uint32_t, SFlowFlowRecord> flow_records;

    explicit SFlowFlowSampleData() : 
        flow_sample(), flow_records() {
    }
    ~SFlowFlowSampleData() {
    }
    friend inline std::ostream& operator<<(std::ostream& out, 
                    const SFlowFlowSampleData& fs_data);
};

std::ostream& operator<<(std::ostream& out, 
            const SFlowFlowSampleData& fs_data) {
    out << fs_data.flow_sample;
    boost::ptr_map<uint32_t, SFlowFlowRecord>::const_iterator it =
        fs_data.flow_records.begin();
    for (; it != fs_data.flow_records.end(); ++it) {
        SFlowFlowHeader* sflow_header = (SFlowFlowHeader*)it->second;
        out << *sflow_header;
    }
    return out;
}

struct SFlowData {
    SFlowHeader sflow_header;
    boost::ptr_vector<SFlowFlowSampleData> flow_samples;

    explicit SFlowData() : sflow_header(), flow_samples() {
    }
    ~SFlowData() {
    }
    friend inline std::ostream& operator<<(std::ostream& out, 
                                           const SFlowData& sflow_data);
};

std::ostream& operator<<(std::ostream& out, const SFlowData& sflow_data) {
    out << std::endl;
    out << sflow_data.sflow_header;
    boost::ptr_vector<SFlowFlowSampleData>::const_iterator it = 
        sflow_data.flow_samples.begin();
    out << "Num of Flow Samples: " << sflow_data.flow_samples.size() 
        << std::endl;
    for (; it != sflow_data.flow_samples.end(); ++it) {
        out << *it << std::endl;
    }
    return out;
}

class SFlowParser {
public:
    explicit SFlowParser(const uint8_t* buf, size_t len,
                         SandeshTraceBufferPtr trace_buf);
    ~SFlowParser();
    int Parse(SFlowData* const sflow_data);
private:
    int ReadSFlowHeader(SFlowHeader& sflow_header);
    int ReadSFlowFlowSample(SFlowFlowSampleData& flow_sample_data, 
                            bool expanded);
    int ReadSFlowFlowHeader(SFlowFlowHeader& flow_header);
    // move decoding functions to a different class
    int DecodeEthernetHeader(const uint8_t* ethh,
                             SFlowFlowEthernetData& eth_data);
    int DecodeIpv4Header(const uint8_t* ipv4h, SFlowFlowIpData& ip_data);
    int DecodeLayer4Header(const uint8_t* l4h, SFlowFlowIpData& ip_data);
    int SkipBytes(size_t len);
    int ReadData32(uint32_t& data32);
    int ReadBytes(uint8_t *bytes, size_t len);
    int ReadIpaddress(SFlowIpaddress& ipaddr);
    
    const uint8_t* const raw_datagram_;
    uint16_t length_;
    const uint8_t* const end_ptr_;
    const uint32_t* decode_ptr_;
    SandeshTraceBufferPtr trace_buf_;
    
    DISALLOW_COPY_AND_ASSIGN(SFlowParser);
};

#endif // __SFLOW_PARSER_H__

