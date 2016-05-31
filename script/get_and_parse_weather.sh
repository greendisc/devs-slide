#!/bin/bash

#config
outfile=weather.txt
initdata="2013-01-01 05:55:00"

# main program execution
echo -n "" > $outfile;
utcdata=`date +%s -d "$initdata"`

# Original url was:
# http://www.wunderground.com/history/airport/KCEF/2013/1/1/DailyHistory.html
#?req_city=Holyoke&req_state=MA&req_statename=Massachusetts&&theprefset=SHOWMETAR&theprefvalue=0&format=1

WEBHEAD="http://www.wunderground.com/history/airport/KCEF/"
WEBQUERY="DailyHistory.html?req_city=Holyoke&req_state=MA&req_statename=Massachusetts&&theprefset=SHOWMETAR&theprefvalue=0&format=1" 

echo "This script collects historical weather information from Wunderground";
#day=1
#month=1
for month in `seq 1 12` ; do
    for day in `seq 1 31`; do
        if [ $month -eq 2 ] && [ $day -gt 28 ] ; then
            break;
        fi;
        if [ $day -gt 30 ]; then
            if [ $month -eq 2 ] || [ $month -eq 4 ] || [ $month -eq 6 ] || [ $month -eq 9 ] || [ $month -eq 11 ] ; then
                break;
            fi;
        fi;
        echo "Parsing: $day/$month/2013";
        echo "URL: $WEBHEAD/2013/$month/$day/$WEBQUERY"
        curl -X GET "$WEBHEAD/2013/$month/$day/$WEBQUERY" > /tmp/auxfile.txt;
        cat /tmp/auxfile.txt | tail -n +2 | awk -F, '{print $2","$14}' | sed -e 's#<br />##g' | grep -v 'Temperature' > /tmp/auxfile2.txt ;
        cat /tmp/auxfile2.txt | awk -v var=$utcdata -F, '{ command="date +%s -d\"" $2 "\""; command | getline $2; close(command); print ($2-var)","(($1-32)*5/9)}' >> $outfile;
    done;
done;


