#! /bin/bash

particle="gamma" 
maxE=1000
minE=1000
step=0
eventN=1000000
printN=10000

rm doit_from*.mac
filename="doit_from${minE}keV_upto${maxE}keV_step${step}keV.mac"
touch $filename

echo "/run/printProgress $printN" > $filename
echo "/gun/particle gamma" >> $filename

if [ $step -eq 0 ]
then
    echo "/analysis/setFileName $minE" >> $filename
    echo "/gun/energy $minE keV" >> $filename
    echo "/run/beamOn $eventN" >> $filename
else
    for (( i=0;i<(maxE-minE)/step+1;i++ ))
    do
        (( E=i*step+minE ))
        echo "/analysis/setFileName $E" >> $filename
        echo "/gun/energy $E keV" >> $filename
        echo "/run/beamOn $eventN" >> $filename
    done
fi



