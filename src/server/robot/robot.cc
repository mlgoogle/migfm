#include "robot.h"
#include <stdio.h>
#include <stdlib.h>
#include "client/linux/handler/exception_handler.h"
#include "robot_engine.h"

static bool DumpCallBack(const char* dump_path,const char* minidump_id,
						 void* contect,bool succeeded){
    printf("Dump path:%s/%s.dump\n",dump_path,minidump_id);
    return succeeded;
}

__attribute__((visibility("default")))
int InitRobot(void){
    google_breakpad::ExceptionHandler eh(".",NULL,DumpCallBack,NULL,true);
	robot::RobotEngine::GetInstance();

}

__attribute__((visibility("default")))
int CloseRobot(void){
	robot::RobotEngine::FreeInstance();
}

