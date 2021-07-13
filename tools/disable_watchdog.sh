#!/bin/bash

#Disables the GPU's Hang Check. Requires sudo Access.
echo N > /sys/module/i915/parameters/enable_hangcheck
