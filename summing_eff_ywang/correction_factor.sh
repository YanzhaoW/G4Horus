#!/bin/sh

if [ -n "$1" ]
then
    DIR=$1
    root -l -b -q 'correction_factor.cxx("'${DIR}'")'
    Rscript analysis.R
    Rscript combine.R
    rm ./Rplots.pdf
    cp ./factors.csv $1
    cp ./Summing_Sim_Ex_Comp* $1
else
    # DIR=$(ls -d out/*/ | tail -n 1)
    echo "Please input the path of root data folder"
fi