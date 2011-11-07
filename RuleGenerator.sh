#! /bin/bash
#
# Author: John Gamboa
#
# Generates all Makefile ".o" rules needed to compile the MClone project. Since
# nvcc doesn't understand the -M family flags, I was forced to create a Makefile
# that had all the rules and dependencies for each .o file. Since I though doing
# this by hand would be a huge sin, I've decided to create these two scripts
# (the other one is the ruleGen.lua), that create all the ".o" rules and append
# it to the file passed as argument.
#
# This .sh script just calls the ruleGen.lua script many times for each mclone
# source directory. The ruleGen.lua script is the most important one, that uses
# the -M family flags from the g++ compiler to know what are de dependencies of
# each .o file.
#
# Unfortunately, it seems using this script from the
# doc/Hacking/MakeRuleGenerator directory doesn't work properly, so I recommend
# you to svn cp it to the mclone root directory before running it.

CPPFILES=./src/*.cpp 
CPPFILES=$(echo $CPPFILES ./src/*/*.cpp)
CPPFILES=$(echo $CPPFILES ./src/*/*.cc)

OBJFILES=""
#echo OBJFILES: $OBJFILES
#echo CPPFILES: $CPPFILES

for f in $CPPFILES
do
	f=`basename $f`

	if [ `echo $OBJFILES $f | sed -e 's/.*\.cpp/cpp/'` == "cpp" ]
	then
		f=$(echo 'obj/'$f)
		OBJFILES=$(echo $OBJFILES $f | sed -e 's/\(.*\)\.cpp/\1\.o/')
	elif [ `echo $OBJFILES $f | sed -e 's/.*\.cc/cc/'` == "cc" ]
	then
		f=$(echo 'obj/'$f)
		OBJFILES=$(echo $OBJFILES $f | sed -e 's/\(.*\)\.cc/\1\.o/')
	fi
done

echo CPPFILES: $CPPFILES
echo ""
echo OBJFILES: $OBJFILES

echo "OBJS := $OBJFILES" >> ./Makefile
echo "" >> ./Makefile

objFolder='| obj\n'

for f in $CPPFILES
do
	echo "Processing $f file..."
	printf "obj/" >> ./Makefile
	printf "`g++ $f -MM -I./src/ -I/usr/local/include/ImageMagick/` $objFolder" >> ./Makefile
	echo '	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@' >> ./Makefile
	echo "" >> ./Makefile
done

