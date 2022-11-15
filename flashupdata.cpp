#include "flashupdata.h"
#include <QDebug>

//BLOCK 重发次数
#define BLOCK_RESEND_CNT	3
//一个BLOCK按1024个字来算,DSP侧开辟的缓冲区最大为1024字
#define BLOCK_SIZE	1024
//一个CAN帧最多传输有效字数为3(6字节)
#define FRAME_VALID_DATA_SIZE		3
//一个完整的BLOCK(1024字)需341帧(3个有效字)+1帧(1个有效字)=1024字
#define FRAME_ALL_NUM_ONE_BLOCK	342
//一个完整的BLOCK所有帧中最后一帧的字的个数
#define LAST_FRAME_WORD_NUM_ONE_BLOCK 1
//一个完整的BLOCK所有帧中非最后一帧的字的个数
#define NON_LAST_FRAME_WORD_NUM_ONE_BLOCK 3

//读取HEX文件结束标志定义
#define READ_HEX_END_FLAG_VALID		0x6789
#define READ_HEX_END_FLAG_RESET 		0

//握手
#define HANDS_COMM_XMIT &FlashUpData::HandCommXmitFcb
#define HANDS_COMM_RECV &FlashUpData::HandCommRecvFcb

//芯片解密---只收
#define CHIP_DECODE_XMIT &FlashUpData::ChipDecodeXmitFcb
#define CHIP_DECODE_RECV &FlashUpData::ChipDecodeRecvFcb

//API版本信息
#define API_VERSION_XMIT &FlashUpData::ApiVersionXmitFcb
#define API_VERSION_RECV &FlashUpData::ApiVersionRecvFcb

//擦除扇区
#define ERASE_SECTOR_XMIT &FlashUpData::EraseSectorXmitFcb
#define ERASE_SECTOR_RECV &FlashUpData::EraseSectorRecvFcb

//编程开始
#define PROGRAM_XMIT &FlashUpData::ProgramXmitFcb
#define PROGRAM_RECV &FlashUpData::ProgramRecvFcb

//传输BLOCK文件头信息
#define BLOCK_HEAD_XMIT &FlashUpData::BlockHeadXmitFcb
#define BLOCK_HEAD_RECV &FlashUpData::BlockHeadRecvFcb

//传输BLOCK文件有效数据
#define BLOCK_DATA_XMIT &FlashUpData::BlockDataXmitFcb
#define BLOCK_DATA_RECV &FlashUpData::BlockDataRecvFcb

//传输对应BLOCK文件的累加校验和
#define BLOCK_CHECKSUM_XMIT &FlashUpData::BlockChecksumXmitFcb
#define BLOCK_CHECKSUM_RECV &FlashUpData::BlockChecksumRecvFcb

//传输对应BLOCK的编程状态
#define BLOCK_PROGRAN_STATUS_XMIT &FlashUpData::BlockProgStatusXmitFcb
#define BLOCK_PROGRAN_STATUS_RECV &FlashUpData::BlockProgStatusRecvFcb

//FLASH校验
#define VERIFY_XMIT &FlashUpData::VerifyXmitFcb
#define VERIFY_RECV &FlashUpData::VerifyRecvFcb

//发送、接收处理函数定义
typedef int (FlashUpData::*_MSG_XMIT_FCB)(void);
typedef int (FlashUpData::*_MSG_RECV_FCB)(void);

typedef struct
{
    unsigned char ucServiceCode;
    _MSG_RECV_FCB pMsgXmitFcb;
    _MSG_RECV_FCB pMsgRecvFcb;
}_CAN_FLASHUPDATE_MSG_T;

_CAN_FLASHUPDATE_MSG_T CanFlashupdateMsgObjs[]=
{
    //握手
    {HANDS_COMM_SRVCODE, HANDS_COMM_XMIT, HANDS_COMM_RECV},
    //芯片解密
    {CHIP_DECODE_SRVCODE, CHIP_DECODE_XMIT, CHIP_DECODE_RECV},
    //API版本信息
    {API_VERSION_SRVCODE, API_VERSION_XMIT, API_VERSION_RECV},
    //擦除扇区
    {ERASE_SECTOR_SRVCODE, ERASE_SECTOR_XMIT, ERASE_SECTOR_RECV},
    //编程
    {PROGRAM_SRVCODE, PROGRAM_XMIT, PROGRAM_RECV},
    //block head
    {BLOCK_HEAD_SRVCODE, BLOCK_HEAD_XMIT, BLOCK_HEAD_RECV},
    //block data
    {BLOCK_DATA_SRVCODE, BLOCK_DATA_XMIT, BLOCK_DATA_RECV},
    //BLOCK累加和
    {BLOCK_CHECKSUM_SRVCODE, BLOCK_CHECKSUM_XMIT, BLOCK_CHECKSUM_RECV},
    //BLOCK的编程状态
    {BLOCK_PROMG_STATUS_SRVCODE, BLOCK_PROGRAN_STATUS_XMIT, BLOCK_PROGRAN_STATUS_RECV},
    //verify
    {VERIFY_SRVCODE, VERIFY_XMIT, VERIFY_RECV},

};

#define CAN_FLASHUPDATE_MSG_OBJ_CNT (sizeof(CanFlashupdateMsgObjs) / sizeof(_CAN_FLASHUPDATE_MSG_T))


/************************************************************************************
函数名称:	FlashUpData
功能描述: 构造函数
参数：
***********************************************************************************/
FlashUpData::FlashUpData(QObject *parent) : QObject(parent)
{

}

/************************************************************************************
函数名称:	HandCommXmitFcb
功能描述: 握手信号
参数：
***********************************************************************************/
int FlashUpData::HandCommXmitFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	HandCommXmitFcb
功能描述: 握手信号
参数：
***********************************************************************************/
int FlashUpData::HandCommRecvFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	ChipDecodeXmitFcb
功能描述: 信号解密
参数：
***********************************************************************************/
int FlashUpData::ChipDecodeXmitFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	ChipDecodeRecvFcb
功能描述: 信号解密
参数：
***********************************************************************************/
int FlashUpData::ChipDecodeRecvFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	ApiVersionXmitFcb
功能描述: API版本信息
参数：
***********************************************************************************/
int FlashUpData::ApiVersionXmitFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	ApiVersionRecvFcb
功能描述: API版本信息
参数：
***********************************************************************************/
int FlashUpData::ApiVersionRecvFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	EraseSectorXmitFcb
功能描述: 擦除扇区
参数：
***********************************************************************************/
int FlashUpData::EraseSectorXmitFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	EraseSectorRecvFcb
功能描述: 擦除扇区
参数：
***********************************************************************************/
int FlashUpData::EraseSectorRecvFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	ProgramXmitFcb
功能描述: 编程
参数：
***********************************************************************************/
int FlashUpData::ProgramXmitFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	ProgramRecvFcb
功能描述: 编程
参数：
***********************************************************************************/
int FlashUpData::ProgramRecvFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	BlockHeadXmitFcb
功能描述: 传输BLOCK文件头信息
参数：
***********************************************************************************/
int FlashUpData::BlockHeadXmitFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	BlockHeadRecvFcb
功能描述: 传输BLOCK文件头信息
参数：
***********************************************************************************/
int FlashUpData::BlockHeadRecvFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	BlockDataXmitFcb
功能描述: 传输BLOCK文件有效数据
参数：
***********************************************************************************/
int FlashUpData::BlockDataXmitFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	BlockDataRecvFcb
功能描述: 传输BLOCK文件有效数据
参数：
***********************************************************************************/
int FlashUpData::BlockDataRecvFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	BlockChecksumRecvFcb
功能描述: 传输对应BLOCK文件的累加校验和
参数：
***********************************************************************************/
int FlashUpData::BlockChecksumXmitFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	BlockChecksumRecvFcb
功能描述: 传输对应BLOCK文件的累加校验和
参数：
***********************************************************************************/
int FlashUpData::BlockChecksumRecvFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	BlockProgStatusXmitFcb
功能描述: 传输对应BLOCK的编程状态
参数：
***********************************************************************************/
int FlashUpData::BlockProgStatusXmitFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	BlockProgStatusRecvFcb
功能描述: 传输对应BLOCK的编程状态
参数：
***********************************************************************************/
int FlashUpData::BlockProgStatusRecvFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	VerifyXmitFcb
功能描述: flash校验
参数：
***********************************************************************************/
int FlashUpData::VerifyXmitFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	VerifyRecvFcb
功能描述: flash校验
参数：
***********************************************************************************/
int FlashUpData::VerifyRecvFcb()
{
    return  0;

}

/************************************************************************************
函数名称:	AppMsgXmit
功能描述: 应用层发送函数
参数：
***********************************************************************************/
void FlashUpData::AppMsgXmit(CAN_XMIT_QUEUE_MSG_T * pCanXmitMsg)
{
    unsigned long i;

    m_XmitQueueMsg = *pCanXmitMsg;

    //查找对应的处理函数，并执行
    for (i = 0; i < CAN_FLASHUPDATE_MSG_OBJ_CNT; i++)
    {
        if (m_XmitQueueMsg.ucServiceCode == CanFlashupdateMsgObjs[i].ucServiceCode)
        {
            if (NULL != CanFlashupdateMsgObjs[i].pMsgXmitFcb)
            {
                (this->*(CanFlashupdateMsgObjs[i].pMsgXmitFcb))();
                break;
            }
        }
    }

}

/************************************************************************************
函数名称:	AppMsgRecv
功能描述: 应用层接收函数
参数：
***********************************************************************************/
void FlashUpData::AppMsgRecv(CAN_APDU_T *pCanRecvMsg)
{
    unsigned long i;

    //查找对应的处理函数，并执行
    for (i = 0; i < CAN_FLASHUPDATE_MSG_OBJ_CNT; i++)
    {
        if (pCanRecvMsg->ucServiceCode == CanFlashupdateMsgObjs[i].ucServiceCode)
        {
            if (NULL != CanFlashupdateMsgObjs[i].pMsgRecvFcb)
            {
                (this->*(CanFlashupdateMsgObjs[i].pMsgRecvFcb))();
                break;
            }
        }
    }

}

/************************************************************************************
函数名称:	ReadAByteAsciiHex
功能描述: 编码转换
参数：
***********************************************************************************/
BYTE FlashUpData::ReadAByteAsciiHex(BYTE *buf, DWORD& pos)
{
    BYTE nData;
    int ucInput1;
    int ucInput2;

    while (buf[pos] == 0x0D || buf[pos] == 0x0A || buf[pos] == ' ')
    {
        pos++;
    }

    ucInput1 = *(buf+pos);
    ucInput2 = *(buf+pos+1);
    if (ucInput1>='a')
    {
        ucInput1 = ucInput1-'a'+10;
    }
    else if (ucInput1 > 0x39)
    {
        ucInput1 = ucInput1 - 0x37;					/*'A' - 'F'*/
    }
    else
    {
        ucInput1 = ucInput1 - 0x30;					/*'0' - '9'*/
    }

    if (ucInput2>='a')
    {
        ucInput2 = ucInput2-'a'+10;
    }
    else if (ucInput2 > 0x39)
    {
        ucInput2 = ucInput2 - 0x37;					/*'A' - 'F'*/
    }
    else
    {
        ucInput2 = ucInput2 - 0x30;					/*'0' - '9'*/
    }

    nData = (ucInput1 << 4) + ucInput2;

    pos +=2;

    while (buf[pos] == 0x0D || buf[pos] == 0x0A || buf[pos] == ' ')
    {
        pos++;
    }

    return nData;
}

/************************************************************************************
函数名称:	ReadASectionAsciiHex
功能描述: 读取升级文件数据并分配扇区
参数：
***********************************************************************************/
void FlashUpData::ReadASectionAsciiHex(BYTE *buf, DWORD &pos, WORD &nLength, UINT &nAddress, BYTE *sectionbuff)
{

    nLength = 0;
    nAddress = 0;

    //越过文件起始标志
    if (buf[pos]==0x02)
    {
        pos++;
    }

    while (buf[pos] == 0x0D || buf[pos] == 0x0A || buf[pos] == ' ')
    {
        pos++;
    }

    //文件结束
    if (buf[pos] == 0x03)
    {
        qDebug()<<"ReadASectionAsciiHex:read end!\n"<<endl;
        return;
    }

    qDebug()<<"Read Address!\r\n"<<endl;

    nAddress = 0;

    //读取到地址
    if (buf[pos]==0x24 && buf[pos+1]==0x41)
    {
        pos += 2;
        BYTE *pAdd = (BYTE*)&nAddress;
        qDebug()<< "Adrress: "<<buf[pos] << buf[pos+1] << buf[pos+2]<<endl;
        while (buf[pos]!=',')
        {
            nAddress = nAddress<<8;
            pAdd[0] = ReadAByteAsciiHex(buf, pos);
        }
        pos++;
        qDebug()<<"leave  Read Address!\r\n"<<endl;
    }

    int nByteLen = 0;
    //读到下一个地址或文件结束时停止
    while (buf[pos]!=0x24 && buf[pos]!=0x03)
    {
        sectionbuff[nByteLen+1] = ReadAByteAsciiHex(buf, pos);	//高字节在前
        sectionbuff[nByteLen] = ReadAByteAsciiHex(buf, pos);
        //printf("%02X:%d ", sectionbuff[nByteLen], nByteLen);
        nByteLen += 2;
    }

    if (nByteLen%2)
    {
        qDebug()<<"nByteLen is ODD!\n"<<endl;
    }

    nLength = nByteLen/2;
    qDebug()<<"Read A Section!\r\n"<<endl;
}

/************************************************************************************
函数名称:	parseFlashAddress
功能描述: FLASH地址解析
参数：
***********************************************************************************/
void FlashUpData::parseFlashAddress(WORD & wSectionLen, UINT & uSectionAddress, BYTE* ucSectionBuff,
                                                 UINT16 & u16FlagValue, bool bFlashKernel,
                                                 bool &bSectSendCplt, BYTE * &pSaveData, UINT16 &SaveLen)
{
    //不更新kernel
    if (!bFlashKernel)
    {
        if (uSectionAddress>=0x3f4000)
        {
            wSectionLen = 0;
        }
        else if (uSectionAddress==0x3f3fff)	//恰好是标志位
        {
            u16FlagValue = *((UINT16*)ucSectionBuff);
            wSectionLen = 0;
        }
        else if (uSectionAddress+wSectionLen>0X3f4000)
        {
            u16FlagValue = *((UINT16*)(ucSectionBuff+2*(0x3f3fff-uSectionAddress)));
            wSectionLen = 0x3f3fff-uSectionAddress; //超过标志位的扔掉
        }
    }
    else
    {
        if (uSectionAddress>=0x3f4000)
        {}
        else if (uSectionAddress==0x3f3fff)	//恰好是标志位
        {
            u16FlagValue = *((UINT16*)ucSectionBuff);

            if (wSectionLen==1)
                wSectionLen = 0;
            else
            {
                uSectionAddress = 0x3f4000;
                wSectionLen--;
                memmove(ucSectionBuff, ucSectionBuff+2, wSectionLen*2);
            }
        }
        else if (uSectionAddress+wSectionLen<0x3f3fff)	//小于标志位
        {}
        else if (uSectionAddress+wSectionLen==0x3f3fff)
        {
            u16FlagValue = *((UINT16*)(ucSectionBuff+2*(0x3f3fff-uSectionAddress)));
            wSectionLen--;
        }
        else	//大于标志位
        {
            u16FlagValue = *((UINT16*)(ucSectionBuff+2*(0x3f3fff-uSectionAddress)));

            //置本次SECTION未处理完标志
            bSectSendCplt = FALSE;

            //未传完的数据大小
            SaveLen = uSectionAddress+wSectionLen-0x3f4000;
            pSaveData = new BYTE[SaveLen*2];
            memcpy(pSaveData, ucSectionBuff+2*(0x3f4000-uSectionAddress), SaveLen*2);

            wSectionLen = 0x3f3fff-uSectionAddress; //将长度截止到标志位前
        }
    }
}

/************************************************************************************
函数名称:	FlashUpdateRoutine
功能描述: FLASH在线升级主程序
参数：
***********************************************************************************/
void FlashUpData::FlashUpdateRoutine(void)
{
    CAN_XMIT_QUEUE_MSG_T TempMsg;
    UCHAR ucStateFlag = 0;

    switch(10)
    {
    //开始升级
    case STATUS_FLASH_START:
        m_pHostModuleItc->u16FlashupdateStatus = STATUS_SELCET_NODE;

        //置当前传输的Block数为0
        m_u16SendBlocks = 0;
        m_bReadEnd = FALSE;
        m_bSectSendCplt = TRUE;	//默认值为一次性传完
        m_bSendFlag = FALSE;	//未传输标志位
        qDebug()<<"开始升级";
        break;

        //对指定节点开始升级,首先发握手命令
    case STATUS_SELCET_NODE:
        //发握手命令
        TempMsg.u16DestinationId = 0x3f;
        TempMsg.ucServiceCode = HANDS_COMM_SRVCODE;
        TempMsg.ucMsgClass = 0;
        TempMsg.ucFrag = NONFRAG_MSG;
        TempMsg.ucRsRq = RS_MSG;
        TempMsg.ucSourceId = MAC_ID_MON;
        AppMsgXmit(&TempMsg);

        break;
        //等待握手应答信号
    case STATUS_WAITING_HANDS_RESPOND:
        //DO NOTHING
        break;
        //等待芯片解密信号
    case STATUS_WAITING_CHIP_DECODE:
        //do nothing
        break;
        //等待API版本确认信息
    case STATUS_WAITING_API_VERSION:
        //do nothing
        break;
        //API VERSION OK
    case STATUS_API_OK:
        //下发擦除命令
        TempMsg.u16DestinationId = 0x3f;
        TempMsg.ucServiceCode = ERASE_SECTOR_SRVCODE;
        TempMsg.ucMsgClass = 0;
        TempMsg.ucFrag = NONFRAG_MSG;
        TempMsg.ucRsRq = RS_MSG;
        TempMsg.ucSourceId = MAC_ID_MON;
        AppMsgXmit(&TempMsg);

        break;
        //等待擦除命令应答
    case STATUS_FLASH_ERASE_GOING:

        break;
        //擦除成功
    case STATUS_FLASH_ERASED:
        //下发编程请求命令
        TempMsg.u16DestinationId = 0x3f;
        TempMsg.ucServiceCode = PROGRAM_SRVCODE;
        TempMsg.ucMsgClass = 0;
        TempMsg.ucFrag = NONFRAG_MSG;
        TempMsg.ucRsRq = RS_MSG;
        TempMsg.ucSourceId = MAC_ID_MON;
        AppMsgXmit(&TempMsg);
        break;
        //等待编程允许状态
    case STATUS_PROGRAM_PERMIT_WAITING:
        //do nothing
        break;
        //编程允许
    case STATUS_PROGRAM_ENABLE:
    {
        //首先读一个Section相关信息，从总的缓冲区中指定位置开始读出一个BLOCK的信息，首先读一个Section相关信息
L1:     //上次有数据未处理完，则优先处理
        if (!m_bSectSendCplt)
        {
            m_bSectSendCplt = true;
            m_uSectionAddress = 0x3f4000;
            m_wSectionLen = m_SaveLen;
            memcpy(m_ucSectionBuff, m_pSaveData, m_wSectionLen*2);
            delete[] m_pSaveData;
        }
        else if (!m_bReadEnd)
        {
            ReadASectionAsciiHex(cFlashUpdateBuf, m_ulPos, m_wSectionLen, m_uSectionAddress, m_ucSectionBuff);
            if (m_wSectionLen > 0)
            {
//                bool bFlashKernel = (m_pHostModuleItc->u16FlashUpdateKernelFlag == 0x95);

//                parseFlashAddress(m_wSectionLen,m_uSectionAddress,m_ucSectionBuff,m_u16FlagValue,
//                                  bFlashKernel,m_bSectSendCplt,m_pSaveData,m_SaveLen);
                if (m_wSectionLen==0)
                    goto L1;
            }
            else
            {
                //读取到文件结束标志
                m_bReadEnd = TRUE;
                goto L1;
            }
        }
        else
        {
            if (!m_bSendFlag)	//传输标志位
            {
                m_bSendFlag = TRUE;
                m_wSectionLen = 1;
                *((UINT16*)m_ucSectionBuff) = m_u16FlagValue;
                m_uSectionAddress = 0x3f3fff;
            }
            else	//传输结束帧
            {
                m_wSectionLen = 0;
                m_uSectionAddress = 0;
            }

        }

 //       FLASH_APP_TRACE("after parseFlashAddress:m_ulPos=%x, m_wSectionLen=0x%x,m_uSectionAddress=0x%x\n", m_ulPos,m_wSectionLen,m_uSectionAddress);


        if (m_wSectionLen==1&&m_uSectionAddress==0x3f3fff)
        {
 //           FLASH_APP_LOG(__FILE__, __LINE__, "Send Flag Block!\n");
        }


        //计算本次SECTION共划分多少个BLOCK
        m_u16BlockNumOfSection = m_wSectionLen/BLOCK_SIZE;

        qDebug() << "本次扇区为："<< m_u16BlockNumOfSection <<endl;

        //section 中剩余的字数
        if (m_wSectionLen>(m_u16BlockNumOfSection*BLOCK_SIZE))
        {
            m_u16RemainWordOfSection = m_wSectionLen-m_u16BlockNumOfSection*BLOCK_SIZE;
            //剩余BLOCK的帧数
            if (m_u16RemainWordOfSection%3)
            {
                m_u16FrameNumOfRemainBlock = m_u16RemainWordOfSection/3 +1;
                //剩余BLOCK中最后一帧字的个数
                m_u16LastFrameWordNumOfRemainBlock = m_u16RemainWordOfSection - (m_u16RemainWordOfSection/3)*3;
            }

            else
            {
                m_u16FrameNumOfRemainBlock = m_u16RemainWordOfSection/3;
                //剩余BLOCK中最后一帧字的个数
                m_u16LastFrameWordNumOfRemainBlock = NON_LAST_FRAME_WORD_NUM_ONE_BLOCK;
            }


            //本次SECTION总共的BLOCK数
            m_u16BlockNumOfSection = m_u16BlockNumOfSection+1;
        }

        //结束,长度为0
        else if (0 == m_wSectionLen)
        {
            m_u16RemainWordOfSection = 0;
            m_u16FrameNumOfRemainBlock = 0;

            //置读取HEX文件结束标志
            m_u16ReadHexFileEnd = READ_HEX_END_FLAG_VALID;

        }

        //整除且长度不为0,则最后一个BLOCK也是1024
        else
        {
            m_u16RemainWordOfSection = BLOCK_SIZE;
            m_u16FrameNumOfRemainBlock = FRAME_ALL_NUM_ONE_BLOCK;
            m_u16LastFrameWordNumOfRemainBlock = LAST_FRAME_WORD_NUM_ONE_BLOCK;
        }

        //首次的BLOCK地址就是SECTION地址
        m_uBlockAddress = m_uSectionAddress;


        //每读一次SECTON,当前BLOCK都应该初始化成0
        //其范围为[0,m_u16BlockNumOfSection),其中最大值m_u16BlockNumOfSection为一个少于1024字的BLOCK
        m_u16CurrentBlockNum = 0;

        //重设模块错误次数
        memset(m_ModErrorCnt, 0, sizeof(m_ModErrorCnt));
        m_ModErrorType = FLASH_ERROR_NONE;

        m_u16CurrentFrameNum = 0;

        //下发BLOCK头
        TempMsg.u16DestinationId = 0x3f;
        TempMsg.ucServiceCode = BLOCK_HEAD_SRVCODE;
        TempMsg.ucMsgClass = 0;
        TempMsg.ucFrag = NONFRAG_MSG;
        TempMsg.ucRsRq = RS_MSG;
        TempMsg.ucSourceId = MAC_ID_MON;

//        FLASH_APP_TRACE("Read section: u16DestinationId=%x,ucServiceCode=%x,ucMsgClass=%x,ucSourceId=%x\n",
//                        TempMsg.u16DestinationId,TempMsg.ucServiceCode,TempMsg.ucMsgClass,TempMsg.ucSourceId);
//        FLASH_APP_TRACE("Read section: Read over and start a task for sending head\n");

        AppMsgXmit(&TempMsg);
    }
        break;

        //头BLOCK传输中
    case STATUS_BLOCK_HEAD_WAITING:
        //do nothing
        break;
        //下一个BLOCK头
    case STATUS_NEXT_BLOCK_HEAD:
        //下发BLOCK头
        TempMsg.u16DestinationId = 0x3f;
        TempMsg.ucServiceCode = BLOCK_HEAD_SRVCODE;
        TempMsg.ucMsgClass = 0;
        TempMsg.ucFrag = NONFRAG_MSG;
        TempMsg.ucRsRq = RS_MSG;
        TempMsg.ucSourceId = MAC_ID_MON;
        AppMsgXmit(&TempMsg);
        break;

        //头传输结束
    case STATUS_BLOCK_HEAD_OK:
        //BLOCK数据传输过程中
    case STATUS_BLOCK_DATATRANS_WAITING:
        TempMsg.u16DestinationId = 0x3f;
        TempMsg.ucServiceCode = BLOCK_DATA_SRVCODE;
        TempMsg.ucMsgClass = 0;
        TempMsg.ucFrag = NONFRAG_MSG;
        TempMsg.ucRsRq = RS_MSG;
        TempMsg.ucSourceId = MAC_ID_MON;
        AppMsgXmit(&TempMsg);
        break;

        //BLOCK数据传输结束
    case STATUS_BLOCK_DATATRANS_END:
        TempMsg.u16DestinationId = 0x3f;
        TempMsg.ucServiceCode = BLOCK_CHECKSUM_SRVCODE;
        TempMsg.ucMsgClass = 0;
        TempMsg.ucFrag = NONFRAG_MSG;
        TempMsg.ucRsRq = RS_MSG;
        TempMsg.ucSourceId = MAC_ID_MON;
        AppMsgXmit(&TempMsg);
        break;
        //block校验中
    case STATUS_BLOCK_CHECKSUM_WAITING:
        //do nothing
        break;

        //BLOCK校验OK
    case STATUS_BLOCK_CHECKSUM_OK:
        //获取对应的编程状态命令
        TempMsg.u16DestinationId = 0x3f;
        TempMsg.ucServiceCode = BLOCK_PROMG_STATUS_SRVCODE;
        TempMsg.ucMsgClass = 0;
        TempMsg.ucFrag = NONFRAG_MSG;
        TempMsg.ucRsRq = RS_MSG;
        TempMsg.ucSourceId = MAC_ID_MON;
        AppMsgXmit(&TempMsg);

        break;
        //编程状态
    case STATUS_BLOCK_PROGRAM_WAITING:
        //DO NOTHING
        break;

        //编程完毕
    case STATUS_BLOCK_PROGRAM_COMPLETE:
        break;

        //文件传输结束
    case STATUS_FILE_TRANS_END:
        break;

        //校验中
    case STATUS_FLASH_VERIFYING:
        //do nothing
        break;

        //校验完毕
    case STATUS_FLASH_UPDATE_OVER:
    {
        //状态机复位
        m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_INVALID;

        //升级进度标志复位
        m_u16ProgramPorcess = PROGRESS_IN_RESET;
    }
        break;

    default:

        //升级进度标志复位
        m_u16ProgramPorcess = PROGRESS_IN_RESET;
        ucStateFlag = 1;
        break;

    }

    if ((1 == ucStateFlag) && (m_u32RespondModuleFlag != 0))
    {
        m_u32RespondModuleFlag = 0;
    }
}
