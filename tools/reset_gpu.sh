#!/bin/bash

cd /sys/kernel/debug/dri/0
echo 0 > i915_error_state
echo 1 > i915_wedged
cd -
