#include "types.h"

//全局变量定义
int  IsOpenFlag = 0;
long  mRecCnt = 0;

//Flash Update 缓冲区
BYTE cFlashUpdateBuf[FLASH_UPDATE_FILE_BUFF_SIZE];
