#ifndef _ROBOT_PRIZE_RATE_H__
#define _ROBOT_PRIZE_RATE_H__
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


typedef struct luck_prize{
	int prize;
	int rate;
	int min;
	int max;

	luck_prize(){
		prize = 0;
		rate = 0;
		min = 0;
		max = 0;
	}
}*pluck_prize;

#define TABLE_SIZE 10000

class PrizeRate{
public:
	PrizeRate();
	~PrizeRate();
public:
	void SetRateTable();
	int GetPrize(pluck_prize prize_info,int count);
private:
   int  rate_table_[TABLE_SIZE];
   int gN;//控制gM变化，
   int gM;//抽奖时的区域
};
#endif
