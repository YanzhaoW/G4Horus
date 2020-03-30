#! /bin/bash

particle="gamma" 
eventN=1000000
printN=10000

DecayFile="Ra_226.txt"
OutFile="../scripts/Ra_226.mac"

touch $OutFile
echo "/run/printProgress $printN" > $OutFile
echo "/gun/particle gamma" >> $OutFile
energies=( $( cat $DecayFile | awk '{print $1}' ) )


for E in "${energies[@]}"
do
    E_int=$( LC_ALL=en_US.UTF-8 printf "%.0f\n"  $E )
    echo "/analysis/setFileName $E_int" >> $OutFile
    echo "/gun/energy $E keV" >> $OutFile
    echo "/run/beamOn $eventN" >> $OutFile
done



