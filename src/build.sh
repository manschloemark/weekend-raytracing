# This is pretty sloppy b/c in order to specify the NUM_PROCS you need to also specify the output file... 
# It would be better if I actually looked at the arguments and identified them.
if [[ -n $1 ]]
then filename=$1
else
	filename=$(git branch --show-current).exe
fi

pushd ../build
g++ -pthread ../src/main.cpp -o $filename
popd
