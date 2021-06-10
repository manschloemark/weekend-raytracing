# This is pretty sloppy b/c in order to specify the NUM_PROCS you need to also specify the output file... 
# It would be better if I actually looked at the arguments and identified them.
if [[ -n $1 ]]
then filename=$1
	if [[ -n $2 ]]
	then numprocs="-DNUM_THREADS=$2"
	else numprocs=""
	fi
else
	filename=$(git branch --show-current)
	numprocs=""
fi

pushd ../build
g++ ../src/main.cpp -o $filename -fopenmp $numprocs
popd
