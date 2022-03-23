iotlab-experiment submit -n "ttn-arroseur1" -d 120 -l 1,archi=st-lrwan1:sx1276+site=saclay

iotlab-experiment wait --timeout 30 --cancel-on-timeout

iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes

make IOTLAB_NODES=$1 iotlab-flash

