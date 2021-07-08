# Script that makes it easier for me to build my program
# Just run build.sh FILENAME to compile the program with the appropriate flags
# Alternatively you can omit the filename and the output file will be named
# <current_git_branch>.exe
# Also it uses pushd and popd to build the exe in weekend-raytracing/build/
if [[ -n $1 ]]
then filename=$1
else
	filename=$(git branch --show-current).exe
fi

pushd ../build
g++ -pthread ../src/main.cpp -o $filename
popd
