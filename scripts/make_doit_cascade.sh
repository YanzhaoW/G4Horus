#! /bin/bash

#111Sn:
#energies=( 761.97 954.05 1152.98 1610.47 )

#226Ra:
# energies=( 186.21 242.00 295.22 351.93 609.32 665.45 
# 768.36 806.18 934.06 1120.29 1155.21 1238.12 1280.98 
# 1377.67 1401.52 1407.99 1509.21 1661.27 1729.60 1764.49 
# 1847.43 2118.51 2204.06 2447.70 )

#152Eu:
energies=( 121.78 244.70 344.28 411.12 443.96 778.90 867.38 964.06 
1085.84 1089.74 1112.08 1212.95 1299.14 1408.01)

eventN=1000000
printN=100000
file=Decay_Scheme_152Eu


filename="doit_${file}.mac"
touch $filename

echo "/run/printProgress $printN" > $filename

for e in "${energies[@]}"; 
do
    e_r=$( LC_ALL=C /usr/bin/printf "%.*f\n" 0 $e )
    echo "/analysis/setFileName ${e_r}_on" >> $filename
    echo "/cascade/mode 1" >> $filename
    echo "/cascade/energy $e keV" >> $filename
    echo "/cascade/file $file" >> $filename
    echo "/run/beamOn $eventN" >> $filename
    echo "/analysis/setFileName ${e_r}_off" >> $filename
    echo "/cascade/mode 0" >> $filename
    echo "/cascade/energy $e keV" >> $filename
    echo "/cascade/file $file" >> $filename
    echo "/run/beamOn $eventN" >> $filename
done