## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_module('magister-stats', ['core', 'network', 'stats'])
    obj.source = [
        'model/application-delay-probe.cc',
        'model/bytes-probe.cc',
        'model/collector-map.cc',
        'model/interval-rate-collector.cc',
        'model/distribution-collector.cc',
        'model/scalar-collector.cc',
        'model/unit-conversion-collector.cc',
        'model/multi-file-aggregator.cc',
        'model/magister-gnuplot-aggregator.cc',
        ]

    module_test = bld.create_ns3_module_test_library('magister-stats')
    module_test.source = [
        'test/distribution-collector-test-suite.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'magister-stats'
    headers.source = [
        'model/magister-stats.h',
        'model/application-delay-probe.h',
        'model/bytes-probe.h',
        'model/collector-map.h',
        'model/interval-rate-collector.h',
        'model/distribution-collector.h',
        'model/scalar-collector.h',
        'model/unit-conversion-collector.h',
        'model/multi-file-aggregator.h',
        'model/magister-gnuplot-aggregator.h',
        ]
