#!/bin/bash

iotlab-experiment submit -n "Saclay" -d 10 -l $1,site=saclay+archi=st-lrwan1:sx1276

iotlab-experiment wait --timeout 30 --cancel-on-timeout

iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes

make BOARD=b-l072z-lrwan1 IOTLAB_NODE=auto -C ../Sunline flash

make BOARD=b-l072z-lrwan1 IOTLAB_NODE=auto -C ../Sunline term

