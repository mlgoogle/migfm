#include "prize_rate.h"
#include "base/logic_comm.h"
#include <sstream>
#include <fstream>

PrizeRate::PrizeRate(void)
{


    SetRateTable();
    gN=0;
    gM=0;
}

PrizeRate::~PrizeRate(void)
{

}

void PrizeRate::SetRateTable()
{
    srand((int)time(0));
    memset(rate_table_,0,sizeof(rate_table_));
	int refcount_ = 0;
	std::ostringstream	anystream;
    for(; refcount_< TABLE_SIZE;__sync_fetch_and_add(&refcount_,1))
    {
        int liIndex  =  (int) ((float)TABLE_SIZE * (rand() /(RAND_MAX + 1.0)));
        while(rate_table_[liIndex] != 0)
        {
            liIndex ++;
            liIndex = liIndex % TABLE_SIZE;

        }
        rate_table_[liIndex] = refcount_;
    }
}
/*****************************************
*参数： apStruPrizeInfo 传入中奖率配置的数组
*       count           中奖率数组的长度
*返回： 中奖倍数
*备注： mRateTable为长度10000的数组，内容为0~9999的随机序数字。
*       将mRateTable平分为100个区域，当前抽奖区由gM表示。
*       在gM区域抽奖若干次后在换为下一个区域，以保证抽奖能在mRateTable数组上比较均匀的分布。
*******************************************/
int PrizeRate::GetPrize(pluck_prize prize_info,int count)
{
    int liRet = 0;


    int  lnCount =count;

    int liIndex  =  (int) ((float)(TABLE_SIZE/100) * (rand() /(RAND_MAX + 1.0)));
    ///
    if(gN <10)
    {
        gN++;
    }
    else
    {

        gN=0;
        gM++;
        gM = gM % 100;

    }

    int num = rate_table_[liIndex+gM*100];

    LOG_DEBUG2("num %d",num);
    for(int i = 0 ; i < lnCount;i++)
    {
        if(num >= prize_info[i].min && num <=  prize_info[i].max)
        {
            liRet = prize_info[i].prize;
            break;
        }
    }

     return liRet;

}
