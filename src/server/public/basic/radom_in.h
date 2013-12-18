#ifndef __MIG_FM_PUBLIC_BASIC_RADOM_IN__
#define __MIG_FM_PUBLIC_BASIC_RADOM_IN__
#include "basic/basictypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define RATE_SIZE 100
#define TABLE_SIZE (RATE_SIZE*RATE_SIZE)

namespace base{

class SysRadom{
public:
public:
	SysRadom();
	virtual ~SysRadom();

	static SysRadom *GetInstance();
	static void FreeInstance();

private:
	static SysRadom   *instance_;
public:
	bool InitRandom ();

	int64 GetRandomID ();

	bool DeinitRandom ();
private:
	FILE *m_urandomfp;
};

class MigRadomInV2
{
public:
	MigRadomInV2(int num = 0);
	~MigRadomInV2(void);
public:
	void SetRateTable(int num = 0);
	void Reset();
	void GetPrize(int* rands,int num);
private:
	int*       rate_table_;
	int        num_;
	int        index_;
};
class MigRadomIn
{
public:
	MigRadomIn(void);
	~MigRadomIn(void);
public:

    void SetRateTable();
    int GetPrize();

private:
   unsigned short mRateTable[TABLE_SIZE];	
   int gN;
   int gM;
   int iIndex;

};
}
#endif

