#!/bin/sh
while true
do 
       process=`ps aux | grep spider | grep -v grep`;
 
                  if [ "$process" == "" ]; then
  
                       echo "no process";
                      ./spider
                       sleep 10;
                  else
                      echo "process exsits";
                   fi
 
 done
