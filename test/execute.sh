# Get only the filename without extension
SRCFILE=$1
NAMEMINUXEXT=`expr $SRCFILE : '\([^\.]\+\)'` #info expr -> String expressions -> `STRING : REGEX` 
EXECUTABLE=$NAMEMINUXEXT.exe
gcc $SRCFILE -o $EXECUTABLE
case $? in
	0)
		echo "EXECUTABLE: $EXECUTABLE"
		./$EXECUTABLE
		;;
	*)
		echo "GCC ERROR: $?"
		;;
esac
