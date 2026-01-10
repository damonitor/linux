#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0

import os
import subprocess
import time

import _damon_sysfs

ksft_skip = 4

def get_total_mem_bytes():
    if not os.path.isfile('/proc/meminfo'):
        return None, 'meminfo file not found'
    with open('/proc/meminfo', 'r') as f:
        for line in f:
            fields = line.split()
            if fields[0] != 'MemTotal:':
                continue
            return int(fields[1]) * 1024, None
    return None, 'MemTotal line not found'

def is_test_passed(wss_collected, estimated_wss):
    wss_collected.sort()
    percentile = 75
    nr_collections = len(wss_collected)
    sample = wss_collected[int(nr_collections * percentile / 100)]
    error_rate = abs(sample - estimated_wss) / estimated_wss
    print('%d-th percentile (%d) error %f' %
            (percentile, sample, error_rate))

    acceptable_error_rate = 0.2
    if error_rate < acceptable_error_rate:
        return True

    print('the error rate is not acceptable (> %f)' %
            acceptable_error_rate)
    for percentile in [0, 25, 50, 65, 75, 85, 95, 100]:
        idx = min(int(nr_collections * percentile / 100), nr_collections - 1)
        print('%3d-th percentile: %20d (error %f)' %
              (percentile, wss_collected[idx],
               abs(wss_collected[idx] - estimated_wss) / estimated_wss))
    return False

def main():
    total_mem_bytes, err = get_total_mem_bytes()
    if err is not None:
        print('getting total mem size fail (%s)' % err)
        exit(ksft_skip)

    # repeatedly access 10% of total memory
    sz_region = total_mem_bytes / 10
    proc = subprocess.Popen([
        './access_memory', '1', '%d' % sz_region, '1000', 'repeat'])
    kdamonds = _damon_sysfs.Kdamonds([_damon_sysfs.Kdamond(
            contexts=[_damon_sysfs.DamonCtx(
                ops='vaddr',
                targets=[_damon_sysfs.DamonTarget(pid=proc.pid)],
                monitoring_attrs=_damon_sysfs.DamonAttrs(
                    # auto-tune intervals with the default suggestion
                    intervals_goal=_damon_sysfs.IntervalsGoal(
                        access_bp=400, aggrs=3, min_sample_us=5000,
                        max_sample_us=10000000)),
                schemes=[_damon_sysfs.Damos(
                    access_pattern=_damon_sysfs.DamosAccessPattern(
                        # >= 25% access rate, >= 200ms age
                        nr_accesses=[5, 20], age=[2, 2**64 - 1]))] # schemes
                )] # contexts
            )]) # kdamonds

    err = kdamonds.start()
    if err != None:
        print('kdamond start failed: %s' % err)
        exit(1)

    test_passed = False
    wss_collected = []
    nr_failures = 0
    while proc.poll() is None and nr_failures < 10:
        time.sleep(0.1)
        err = kdamonds.kdamonds[0].update_schemes_tried_bytes()
        if err != None:
            print('tried bytes update failed: %s' % err)
            exit(1)

        wss_collected.append(
                kdamonds.kdamonds[0].contexts[0].schemes[0].tried_bytes)

        nr_collections = len(wss_collected)
        if nr_collections == 40:
            test_passed = is_test_passed(wss_collected, sz_region)
            if test_passed:
                break
            nr_failures += 1
            wss_collected = []
    proc.terminate()

    if test_passed is True:
        return
    exit(1)

if __name__ == '__main__':
    main()
