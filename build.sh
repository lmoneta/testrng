echo "building test programs"
root -b -q Build.C

export LD_LIBRARY_PATH=${PWD}/testU01/lib:$LD_LIBRARY_PATH

