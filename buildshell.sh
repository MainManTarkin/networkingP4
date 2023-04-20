#!/bin/sh
OUTPUTNAME="a.out"                           #g++ output name
VALGRINDLUNCH=0
RUNAFTER=0
RETURNVALUE=0

#-o selects output name
#-r runs progam after compiling it
#-v means begin runnign in valgrind (only works if run now arg is called)

while getopts "o:vr" options; do         # Loop: Get the next option;
                                          # use silent error checking;
                                          # options n and t take arguments.
  case "${options}" in                    # 
    r)                                    # If the option is v,
      RUNAFTER=1
      ;;
    v)                                    # If the option is v,
      VALGRINDLUNCH=1
      ;;
    o)
        OUTPUTNAME=${OPTARG}
      ;;
    *)                                    # If unknown (any other) option:
      exit_abnormal                       # Exit abnormally.
      ;;
  esac
done

g++ testMain.cpp networking.cpp -o $OUTPUTNAME -Wall -Werror --pedantic -std=c++11 -g 

RETURNVALUE=$?

if [ $RETURNVALUE -eq 0 ]; then

    if [ $RUNAFTER -eq 1 ]; then

        if [ $VALGRINDLUNCH -eq 1 ]; then
            valgrind ./$OUTPUTNAME 
        else
            ./$OUTPUTNAME
        fi

    fi
else

    printf 'Error with g++: %d\n' $RETURNVALUE

fi