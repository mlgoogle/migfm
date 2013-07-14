#include "robot_engine.h"
#include "log/basic_log.h"
#include <assert.h>
namespace robot{

RobotEngine* RobotEngine::engine_ = NULL;

RobotEngine::RobotEngine(){
}

RobotEngine::~RobotEngine(){

}

RobotEngine* RobotEngine::GetInstance(){
	if (engine_==NULL){
		engine_ = new RobotEngine();
	}

	return engine_;
}

void RobotEngine::FreeInstance(){
	if (engine_){
		delete engine_;
		engine_ =NULL;
	}
}

}