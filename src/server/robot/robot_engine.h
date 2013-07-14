#ifndef _ROBOT_ROBOT_ENGINE_H__
#define _ROBOT_ROBOT_ENGINE_H__
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <list>
namespace robot{
class RobotEngine{
public:
	RobotEngine(void);
	~RobotEngine(void);
public:
	static RobotEngine* GetInstance(void);
	static void FreeInstance(void);
public:
	static RobotEngine*  engine_;
};

}

#endif