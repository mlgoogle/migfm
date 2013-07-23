#ifndef __MIG_FM_PUBLIC_BASIC_RADOM_IN__
#define __MIG_FM_PUBLIC_BASIC_RADOM_IN__
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define RATE_SIZE 100
#define TABLE_SIZE (RATE_SIZE*RATE_SIZE)

namespace base{

class SysRadom{
public:
	static bool InitRandom ();

	static int GetRandomID ();

	static bool DeinitRandom ();
private:
	static FILE *m_urandomfp;
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

