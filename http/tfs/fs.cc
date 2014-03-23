#include "fs.h"
#include "tfs.h"
#include <stdio.h>
#include <stdlib.h>

namespace filestorage{

MFSEngine* MFSEngine::Create(int32 type){
    MFSEngine* engine_ = NULL;
	switch (type){
		case TFS_TYPE:
			engine_ = new TFSEnginImpl();
			break;
	}
	return engine_;
}

}