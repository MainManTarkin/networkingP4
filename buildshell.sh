#!/bin/sh
OUTPUTNAME="a.out"                           #g++ output name
PASSEDARGS=""           #arguments to pass onto ran program (should be placed in "")
VALGRINDLUNCH=0
RUNAFTER=0
RETURNVALUE=0
GOTARGS=0

#-o selects output name
#-r runs progam after compiling it
#-v means begin runnign in valgrind (only works if run now arg is called)

while getopts "o:a:vr" options; do         # Loop: Get the next option;
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
    a)
      PASSEDARGS=${OPTARG}
      ;;
    *)                                    # If unknown (any other) option:
      GOTARGS=1 #dont run anything after getopt is ran
      exit_abnormal                       # Exit abnormally.
      ;;
  esac
done

#ugly bash if else loop
if [ $GOTARGS -eq 0 ]; then

  g++ testMain.cpp networking.cpp -o $OUTPUTNAME -Wall -Werror --pedantic -std=c++11 -g 

  RETURNVALUE=$?

  if [ $RETURNVALUE -eq 0 ]; then

      if [ $RUNAFTER -eq 1 ]; then

          if [ $VALGRINDLUNCH -eq 1 ]; then
              valgrind ./$OUTPUTNAME $PASSEDARGS
          else
              ./$OUTPUTNAME $PASSEDARGS
          fi

        RETURNVALUE=$?

        printf 'Program return code: %d\n' $RETURNVALUE

      fi
  else

      printf 'Error with g++: %d\n' $RETURNVALUE

  fi

fi