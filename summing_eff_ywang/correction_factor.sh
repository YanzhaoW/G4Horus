#!/bin/sh

if [ -n "$1" ]
then
    DIR=$1
    root -l -b -q 'correction_factor.cxx("'${DIR}'")'
    cp ./factors.csv $1
else
    # DIR=$(ls -d out/*/ | tail -n 1)
    echo "Please input the path of root data folder"
fi