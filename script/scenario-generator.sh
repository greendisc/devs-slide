#!/bin/bash

NUMRACK=20
NUMSERVER=20
NUMIRC=10

#Solana
FANSPEEDS="1800 2400 3000 3600"
#Decathlete
#FANSPEEDS="6000 7000 8000 9000 10000"

INLET="18 22 26 30"

# If OPSERVERFAN is set to true, dc-sim overrides fan speed and 
# computes server optimum FS for given wkload
OPTSERVERFAN=false 

#Solana
#Calculix 256 cpu=89 mem=127
#Gamess 256 cpu=76 mem=28
#Mcf 256 cpu=18 mem=110

#Calculix 128 cpu=67 mem=77
#Gamess 128 cpu=52 mem=30
#Mcf 128 cpu=23 mem=102

#LoadGen100 cpu=96 mem=145
#LoadGen75 #cpu=70 #mem=110
#Loadgen50 #cpu=50 #mem=70
#LoadGen25 #cpu=32 #mem=35
#Idle cpu=0 mem=0

#Decathlete
#Calculix6 cpu=80 mem=16
#Calculix1 cpu=40 mem=10
#Mcf6 cpu=20 mem=60
#Mcf1 cpu=20 mem=20 
#Idle cpu=0 mem=0

#Synthetic
cpu=0
mem=0

if [ "$#" -ne "2" ]; then
    echo "Usage: $0 <dc input file> <matlab output file>";
fi;

infile=$1
outfile=./tmp.json
simout=$2
echo "'Tinlet','FS','ITpower','FSpower','IRCpower','Pump','Chiller','TotalPower'" > $simout ;

echo "Data Center input file is: $infile.";
for int in $INLET ; do
    for fs in $FANSPEEDS ; do
        
        echo "Generating temporary file for Tinlet=$int and FanSpeed=$fs";
        echo "{" > $outfile;
        
        cat $infile >> $outfile;
        echo "," >> $outfile;

        # Room parameters
        #-------------------------------------
        echo "\"RoomParams\" :{ " >> $outfile;
        echo "    \"IRCAirTemp\" : [" >> $outfile ;
        for i in `seq 1 $(($NUMIRC-1))` ; do
            echo "        [\"rc$i\",$int]," >> $outfile ; 
        done;
        echo "        [\"rc${NUMIRC}\",$int]" >> $outfile ; 
        echo "     ]," >> $outfile;
        
        echo "    \"ServerFanSpeed\" : [" >> $outfile ;
        this=0
        for i in `seq 1 $NUMRACK` ; do
            for j in `seq 1 $NUMSERVER` ; do
                this=$(($this + 1))
                if [ $i -eq $NUMRACK ] && [ $j -eq $NUMSERVER ] ; then
                    echo "          [\"rack$i\", \"s$this\", $fs]" >> $outfile ;
                else
                    echo "          [\"rack$i\", \"s$this\", $fs]," >> $outfile ;
                fi;
            done;
        done;
        echo "     ]" >> $outfile;
        echo "}," >> $outfile;

        # Workload parameters
        #------------------------------------
        echo "\"Workload\" :{ " >> $outfile;
        echo "    \"ServerPower\" : [" >> $outfile ;
        this=0
        for i in `seq 1 $NUMRACK` ; do
            for j in `seq 1 $NUMSERVER` ; do
                this=$(($this + 1))
                if [ $i -eq $NUMRACK ] && [ $j -eq $NUMSERVER ] ; then
                    echo "          [\"rack$i\", \"s$this\", $cpu, $mem]" >> $outfile ;
                else
                    echo "          [\"rack$i\", \"s$this\", $cpu, $mem]," >> $outfile ;
                fi;
            done;
        done;
        echo "     ]" >> $outfile;
        echo "}" >> $outfile;
        echo "}" >> $outfile;
        
        # Calling simulator
        #--------------------
        line=`LD_LIBRARY_PATH=/usr/local/lib/ ./dc-simulator.out $outfile | grep -v 'IT'`;
        echo "$int,$fs,$line" >> $simout;

    done;
done ;

