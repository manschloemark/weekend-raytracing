if [[ -n $1 ]]
then filename=$1
else filename=$(git branch --show-current)
fi

pushd ../build
g++ ../src/main.cpp -o $filename -fopenmp
popd
