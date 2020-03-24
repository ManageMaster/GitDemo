#set project name manually
project_name="GitDemo"

#create Auto head file
commit_id=`git rev-parse HEAD`

if [ -z $commit_id ];then
	echo "Error!!!  do not link to any git repository"
	exit
fi

if [ ! -f "include/Auto_Head.h"];then
	vim include/Auto_Head.h
fi

echo "#pragma once\n" > include/Auto_Head.h
echo "/*WARNING:this head file was created automatically by shell,please do not modify anything in this file!!!*/\n" >> include/Auto_Head.h
echo "#define COMMIT_ID \""$commit_id"\" \n" >> include/Auto_Head.h
echo "#define PROJECT_NAME \""$project_name"\" \n" >> include/Auto_Head.h

make_binary(){
if [ ! -d "build/" ];then
	mkdir build/
fi

cd build
cmake .. #running Cmake
make #makefile
mv CPPStart $project_name
cd ..
}

clean_binary(){
if [ -d "build/" ];then
	rm -rf build/
fi
}
case $1 in
    'clean')  
		clean_binary
    ;;
    'reset')  
		clean_binary
		make_binary
    ;;
    'build')  
		make_binary
    ;;
	'run')
		make_binary
		./build/$project_name
	;;
	'all')
		clean_binary
		make_binary
		./build/$project_name
	;;
	'test')
		project_name="CPPStart"
		#use default project name in test mode
		clean_binary
		make_binary
	;;
    *)  
		echo 'start building binary'
		make_binary
    ;;
esac

