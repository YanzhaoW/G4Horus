#!/bin/sh

#source /data/jmayer/software/root-6.00.02/bin/thisroot.sh

if [ -n "$1" ]
then
    DIR=$1
    rm ./Plots/*.png
    rm ./Tables/*.csv
    pngname="$1*.png"
    rm $pngname
    root -l -b -q 'efficiency.cxx("'${DIR}'")'
    Rscript DataPlot.R
else
    # DIR=$(ls -d out/*/ | tail -n 1)
    echo "Please input the path of root data folder"
fi

cp ./Plots/*.png $1
cp ./Tables/*.csv $1
cp ./Clo*.png $1

# 2020-03-27T22-40-34