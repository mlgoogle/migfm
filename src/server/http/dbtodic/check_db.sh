#!/bin/sh
while true
do 
       process=`ps aux | grep dbtodic.out | grep -v grep`;
 
                  if [ "$process" == "" ]; then
  
                       echo "no process";
                      ./dbtodic.out
                       sleep 10;
                  else
                      echo "process exsits";
                   fi
 
 done
