#!/bin/bash

CPUPOWER=( 89 76 18 0 0 0 0 0 )
MEMPOWER=( 127 28 110 0 0 0 0 0 )
FANSPEED=( 2400 2400 2400 2400 2400 2400 2400 2400)

NUMRACK=20
NUMSERVER=20

this=0
idx=0

echo "Fanspeeds";
for i in `seq 1 $NUMRACK` ; do
    for j in `seq 1 $NUMSERVER` ; do
        this=$(($this + 1))
        fs=${FANSPEED[$idx]}
        if [ $idx -eq "7" ] ; then
            idx=0
        else
            idx=$(($idx + 1))
        fi;

        if [ $i -eq $NUMRACK ] && [ $j -eq $NUMSERVER ] ; then
            echo "          [\"rack$i\", \"s$this\", $fs]" 
        else
            echo "          [\"rack$i\", \"s$this\", $fs],"
        fi;
    done;
done;

this=0
idx=0
echo "Server Power";
for i in `seq 1 $NUMRACK` ; do
    for j in `seq 1 $NUMSERVER` ; do
        this=$(($this + 1))
        cpu=${CPUPOWER[$idx]}
        mem=${MEMPOWER[$idx]}
        if [ $idx -eq "7" ] ; then
            idx=0
        else
            idx=$(($idx + 1))
        fi;

        if [ $i -eq $NUMRACK ] && [ $j -eq $NUMSERVER ] ; then
            echo "          [\"rack$i\", \"s$this\", $cpu, $mem]";
        else
            echo "          [\"rack$i\", \"s$this\", $cpu, $mem]," ;
        fi;
    done;
done;


