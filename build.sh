echo "building test programs"
root -b -q Build.C

export LD_LIBRARY_PATH=${PWD}/testU01/lib:$LD_LIBRARY_PATH

echo "Ready to run the test !"
echo "type for example ./testu01_bigcrush -small"
