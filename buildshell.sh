#!/bin/sh
OUTPUTNAME="a.out"                           #g++ output name
VALGRINDLUNCH=0


while getopts "o:v" options; do         # Loop: Get the next option;
                                          # use silent error checking;
                                          # options n and t take arguments.
  case "${options}" in                    # 
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

if [ $VALGRINDLUNCH -eq 1 ]; then
    valgrind ./$OUTPUTNAME 
else
    ./$OUTPUTNAME
fi