#!/bin/bash

DCSIM_PORT=4321
DCSIM_HOST=cronos.lsi.die

SLURMSIM_PORT=1234
SLURMSIM_HOST=slurm-vm2.lsi.die

POWERLOG=outfile.txt
WKLOAD=wkload.txt
WEATHER=weather.txt
#DCSTATUS=dc_status.txt

echo "" > $$POWERLOG;

if [ "$#" -ne "1" ]; then
    echo "Usage: $0 <dc config file>" ;
fi; 
dcConfig=$1

echo "Launching full simulation environment";
echo " -- Launching DCsim";
./sim-launcher.out --config=$dcConfig --wkload=$DCSIM_PORT --weather=$WEATHER --port=$DCSIM_PORT > $POWERLOG ;
sleep 2;

echo " -- Launching SlurmSim";

