#!/bin/sh

#source /data/jmayer/software/root-6.00.02/bin/thisroot.sh

if [ -n "$1" ]
then
    DIR=$1
    root -l -b -q 'efficiency.cxx("'${DIR}'")'
else
    # DIR=$(ls -d out/*/ | tail -n 1)
    echo "Please input the path of root data folder"
fi

# 2020-03-27T22-40-34