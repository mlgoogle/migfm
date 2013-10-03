#include "radom_in.h"
#include "log/mig_log.h"
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

namespace base{

SysRadom* SysRadom::instance_ = NULL;
SysRadom* SysRadom::GetInstance(){
	if (instance_==NULL){
		instance_ = new SysRadom();
	}

	return instance_;
}

void SysRadom::FreeInstance(){
	delete instance_;
}

SysRadom::SysRadom(){
	InitRandom();
}

SysRadom::~SysRadom(){
	DeinitRandom();
}

bool SysRadom::InitRandom(){
	m_urandomfp = fopen ("/dev/urandom", "rb");
	if (m_urandomfp == NULL) {
		assert (0);
		return false;
	}
	setvbuf (m_urandomfp, NULL, _IONBF, 0);
}

int64 SysRadom::GetRandomID(){
	int64 rd = 0;
	do {
		errno = 0;
		fread (&rd, sizeof (rd), 1, m_urandomfp);
	} while (errno == EINTR);
	return rd;
}

bool SysRadom::DeinitRandom(){
    fclose (m_urandomfp);
	return true;
}

MigRadomIn::MigRadomIn(void)
{
    SetRateTable();
    gN=0;
    gM=0;
}

MigRadomIn::~MigRadomIn(void)
{
	
}

void MigRadomIn::SetRateTable()
{
    srand((int)time(0));
    memset(mRateTable,0,sizeof(mRateTable));
	int refcount_ = 0;
    for(; refcount_< TABLE_SIZE;__sync_fetch_and_add(&refcount_,1))
    {
        int liIndex  =  (int) ((float)TABLE_SIZE * (rand() /(RAND_MAX + 1.0)));
        while(mRateTable[liIndex] != 0)
        {
            liIndex ++;
            liIndex = liIndex % TABLE_SIZE;

        }
        mRateTable[liIndex] = refcount_;
    }
}

int MigRadomIn::GetPrize()
{
    int liRet = 0;

    int index  =  (int) ((float)(TABLE_SIZE/RATE_SIZE) * (rand() /(RAND_MAX + 1.0)));
	index+=iIndex%RATE_SIZE;
    ///
back:
    if(gN <RATE_SIZE)
    {
        //gN++;
		__sync_fetch_and_add(&gN,1);
    }
    else
    {

        gN=0;
        __sync_fetch_and_add(&gM,1);
        gM = gM % RATE_SIZE;
		goto back;
    }

    int num = mRateTable[index+gM*RATE_SIZE];
	__sync_fetch_and_add(&iIndex,1);
    return num;

}

}
