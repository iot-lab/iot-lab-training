iotlab-experiment submit -n "ttn-arroseur1" -d 120 -l 1,archi=st-lrwan1:sx1276+site=saclay

iotlab-experiment wait --timeout 30 --cancel-on-timeout

iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes

echo "prouuut"
echo $HOHO
echo "rooote"

make IOTLAB_NODES=$HOHO iotlab-flash

