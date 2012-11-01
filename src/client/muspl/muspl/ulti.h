#pragma once

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)						\
{											\
	if ((x))								\
	{										\
		x->Release();						\
		x = 0;								\
	}										\
}
#endif
