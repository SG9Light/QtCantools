#include "CanFlashupdate.h"

//发送、接收处理函数定义
typedef int (CanFlashupdate::*_MSG_XMIT_FCB)(void);
typedef int (CanFlashupdate::*_MSG_RECV_FCB)(void);

typedef struct
{
	uint8_t ucServiceCode;
	_MSG_RECV_FCB pMsgXmitFcb;
	_MSG_RECV_FCB pMsgRecvFcb;	
}_CAN_FLASHUPDATE_MSG_T;

//握手
#define HANDS_COMM_XMIT &CanFlashupdate::HandCommXmitFcb
#define HANDS_COMM_RECV &CanFlashupdate::HandCommRecvFcb
//芯片解密---只收
#define CHIP_DECODE_XMIT &CanFlashupdate::ChipDecodeXmitFcb
#define CHIP_DECODE_RECV &CanFlashupdate::ChipDecodeRecvFcb
//API版本信息
#define API_VERSION_XMIT &CanFlashupdate::ApiVersionXmitFcb
#define API_VERSION_RECV &CanFlashupdate::ApiVersionRecvFcb
//擦除扇区
#define ERASE_SECTOR_XMIT &CanFlashupdate::EraseSectorXmitFcb
#define ERASE_SECTOR_RECV &CanFlashupdate::EraseSectorRecvFcb
//编程开始
#define PROGRAM_XMIT &CanFlashupdate::ProgramXmitFcb
#define PROGRAM_RECV &CanFlashupdate::ProgramRecvFcb
//传输BLOCK文件头信息
#define BLOCK_HEAD_XMIT &CanFlashupdate::BlockHeadXmitFcb
#define BLOCK_HEAD_RECV &CanFlashupdate::BlockHeadRecvFcb
//传输BLOCK文件有效数据
#define BLOCK_DATA_XMIT &CanFlashupdate::BlockDataXmitFcb
#define BLOCK_DATA_RECV &CanFlashupdate::BlockDataRecvFcb
//传输对应BLOCK文件的累加校验和
#define BLOCK_CHECKSUM_XMIT &CanFlashupdate::BlockChecksumXmitFcb
#define BLOCK_CHECKSUM_RECV &CanFlashupdate::BlockChecksumRecvFcb
//传输对应BLOCK的编程状态
#define BLOCK_PROGRAN_STATUS_XMIT &CanFlashupdate::BlockProgStatusXmitFcb
#define BLOCK_PROGRAN_STATUS_RECV &CanFlashupdate::BlockProgStatusRecvFcb
//FLASH校验
#define VERIFY_XMIT &CanFlashupdate::VerifyXmitFcb
#define VERIFY_RECV &CanFlashupdate::VerifyRecvFcb

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

//每隔10S获取一次
//注意顺序不要改,允许在后面添加
PERIODIC_FLASH_UPDATE_CMD_T PeriodicSendEraseCmdObjs[] =
{
	//等待握手应答信号
	{HANDS_COMM_SRVCODE, HANDS_COMM_TIMER, 6000},
	//等待芯片解密应答信号
	{CHIP_DECODE_SRVCODE, CHIP_DECODE_TIMER, 4000},
	//等待API版本信息应答
	{ API_VERSION_SRVCODE, API_VERSION_TIMER, 2000},
	//等待擦除信号10S
	{ ERASE_SECTOR_SRVCODE, ERASE_SECTOR_TIMER, 10000},
	//等待编程允许信号
	{ PROGRAM_SRVCODE, PROGRAM_TIMER, 2000},
	//等待头信息应答信号
	{ BLOCK_HEAD_SRVCODE, BLOCK_HEAD_TIMER, 4000},
	//等待BLOCK校验和应答信息
	{ BLOCK_CHECKSUM_SRVCODE, BLOCK_CHECKSUM_TIMER, 2000},
	//等待FLASH编程状态信息
	{ BLOCK_PROMG_STATUS_SRVCODE, BLOCK_PROMG_STATUS_TIMER, 2000},
	//等待FLASH编程校验应答信息
	{ VERIFY_SRVCODE, VERIFY_TIMER, 2000}
};

_HOST_MODULE_ITC_T gHostModuleItc;

/**********************************************************************
CanFlashupdate-----构造函数
Parameters:
Return Value:
**********************************************************************/
CanFlashupdate::CanFlashupdate(QObject *parent) : QObject(parent)
{
    this->moveToThread(objCanFlashTask);
    objCanFlashTask->start();

    //状态初始化
    CanFlashInit();
}

/**********************************************************************
~CanFlashupdate-----析构函数
Parameters:
Return Value:
**********************************************************************/
CanFlashupdate::~CanFlashupdate()
{
    objCanFlashTask->quit();
    objCanFlashTask->wait();
    objCanFlashTask->deleteLater();

}

/**********************************************************************
CanFlashInit-----canflash升级数据、标志初始化
Parameters:
Return Value:
**********************************************************************/
void CanFlashupdate::CanFlashInit()
{
    m_bRsExit = false;

    m_ucMsgClass = 0;
    m_pHostModuleItc = &gHostModuleItc;

    m_XmitMsg.ucMsgClass = m_ucMsgClass;

    m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_INVALID;
    m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_START;

    //默认升级内核
    m_pHostModuleItc->u16FlashUpdateKernelFlag = 0x95;
    for (int i=0; i<TIMER_CNT_LEN ; i++)
    {
        m_u16TimerExpiredCnt[i] = RESEND_WAITING_RESET_CNT;
    }
    memset(m_ModErrorCnt, 0, sizeof(m_ModErrorCnt));
    m_ModErrorType = FLASH_ERROR_NONE;
}

/**********************************************************************
HandCommXmitFcb-----握手信号下发               
Parameters:
Return Value: 
**********************************************************************/
int CanFlashupdate::HandCommXmitFcb(VOID)
{
    qDebug("flash update: running HandCommXmitFcb!\n");

	uint8_t ucErrCode = CAN_MSG_HANDLE_OK;

	m_XmitMsg.pData = m_ucXmitMsgBuf;

	//升级与协议层交互的信息
	m_XmitMsg.ucRsRq = RS_MSG;
	//传送2字节
	m_XmitMsg.u16Length = 2;

	//将指针m_XmitMsg.pData也指向将要发送的数据
	*((uint16_t *)(m_XmitMsg.pData + 0)) = HAND_COMM_QUERY;

	//初始化数据
	m_ulPos = 0;
	m_uBlockAddress = 0;
	m_uSectionAddress = 0;
	m_wBlockLen = 0;

 	m_u16ResendCnt = BLOCK_RESEND_CNT;

	//超时计数器处理
	if (RESEND_WAITING_RESET_CNT == m_u16TimerExpiredCnt[0])
	{
		m_u16TimerExpiredCnt[0] = RESEND_WAITING_START_CNT;
	}

	//等待握手响应
	m_pHostModuleItc->u16FlashupdateStatus = STATUS_WAITING_HANDS_RESPOND;

	return ucErrCode;
}

/**********************************************************************
HandCommRecvFcb-----握手信号接收                   
Parameters:		=0xaa表示握手成功			
Return Value: 
**********************************************************************/
int CanFlashupdate::HandCommRecvFcb(VOID)
{
	uint16_t u16RetrunStatus;
	CAN_XMIT_QUEUE_MSG_T TempMsg;

	u16RetrunStatus = *(uint16_t *)(m_RecvMsg.pData);

    qDebug("Run 'HandCommRecvFcb' nAddr=0x%02X\n", m_RecvMsg.ucSourceId);

	//如果进度不在等待握手应答则放弃此次接收到的握手信号
	if (PROGRESS_IN_START != m_u16ProgramPorcess)
	{
		return 1;
	}

	//握手成功
	if (HAND_OK_RESPOND == u16RetrunStatus)
	{
        qDebug("Receive HANDS_COMM OK, nAddr=0x%02X\n", m_RecvMsg.ucSourceId);
		
		//置相应模块回应标志
		SetRespondModuleFlag(m_RecvMsg.ucSourceId);
		
		//通过握手超时来获取可升级模块
        if (1 /*== FlashupdateTaskHandle(m_RecvMsg.ucSourceId)*/)
		{
            qDebug("Receive All Hand Comm!\n");
			
			//升级进度标志,防止收到芯片解密回馈信息之前，进度还未更新到位
	 		m_u16ProgramPorcess = PROGRESS_IN_HAND_OK;

			//等待解密信息反馈,启动等待延时TBD
			m_pHostModuleItc->u16FlashupdateStatus = STATUS_WAITING_CHIP_DECODE;			
			m_u16TimerExpiredCnt[0] = RESEND_WAITING_RESET_CNT;

			//下发获取芯片解密信息
			TempMsg.u16DestinationId = 0x3f;
			TempMsg.ucServiceCode = CHIP_DECODE_SRVCODE;
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
			Msg_Xmit(&TempMsg);


			
		}

		//wait othger node repond
		else
		{
			;//do nothing
		}
		
	}

	//握手失败,置反馈状态字给后台TBD
	else
	{
        qDebug("HandCommRecvFcb:Data=%04X, nAddr=0x%02X\n", u16RetrunStatus, m_RecvMsg.ucSourceId);
	}
	return u16RetrunStatus;
}

/**********************************************************************
Msg_Xmit-----消息发送处理        
Parameters:  pCanXmitMsg:应用层正在发送消息内容
Return Value: 
**********************************************************************/
int CanFlashupdate::Msg_Xmit(IN CAN_XMIT_QUEUE_MSG_T * pCanXmitMsg)
{
    int  ret = 0;
    uint64_t i;

	//更新当前发送数据
	m_XmitQueueMsg = *pCanXmitMsg;

	//根据RS/RE标志，升级与协议层交互的信息
	m_XmitMsg.ucServiceCode = m_XmitQueueMsg.ucServiceCode;
	m_XmitMsg.ucSourceId = MAC_ID_MON;
	m_XmitMsg.ucDestinationId = (m_XmitQueueMsg.u16DestinationId & 0x00ff);
	m_XmitMsg.u16Length = 0;
    m_XmitMsg.ucFrag = m_XmitQueueMsg.ucFrag;

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

	//Can not find the same service code, then get a error
    if (CAN_FLASHUPDATE_MSG_OBJ_CNT == i)
	{
		ret = CAN_MSG_HANDLE_INVALID_SRVC_COD;
	}
	else
	{
		//Xmit to protocol layer
        Msg_TxSerial(&m_XmitMsg);
	}

	return ret;
	
}


/**********************************************************************
Msg_Recv-----消息接收处理             
Parameters:pCanRecvMsg:由协议层上传的消息内容
Return Value: 
**********************************************************************/
int CanFlashupdate::Msg_Recv(CAN_APDU_T *pCanRecvMsg)
{
    uint64_t i;

	//更新当前发送数据
	m_RecvMsg = *pCanRecvMsg;

	//查找对应的处理函数，并执行
	for (i = 0; i < CAN_FLASHUPDATE_MSG_OBJ_CNT; i++)
	{
		if (m_RecvMsg.ucServiceCode == CanFlashupdateMsgObjs[i].ucServiceCode)
		{
			if (NULL != CanFlashupdateMsgObjs[i].pMsgRecvFcb)
			{
				(this->*(CanFlashupdateMsgObjs[i].pMsgRecvFcb))();
				break;
			}
		}
	}

	//Can not find the same service code, then get a error
	if (CAN_FLASHUPDATE_MSG_OBJ_CNT == i)
	{

	}
	
	return 0;

}

/**********************************************************************
VCI_To_APDU-----底层转应用层
Parameters:pCanRecvMsg:由协议层上传的消息内容
Return Value:
**********************************************************************/
void VCI_To_APDU(CAN_APDU_T *pApdu, CAN_OBJ *pVci)
{
    pApdu->ucMsgClass = ((pVci->ID&0x03c0)>>6);
	pApdu->ucServiceCode = (pVci->Data[1]&0x7f);
	pApdu->ucRsRq = (pVci->Data[1]>>7);
	pApdu->ucFrag = (pVci->Data[0]>>7);
    pApdu->ucSourceId = (pVci->ID&0x3f);
	pApdu->ucDestinationId = (pVci->Data[0]&0x3f);
	pApdu->u16Length = pVci->DataLen-2;	
    pApdu->pData = (uint8_t *)&(pVci->Data[2]);
}

/**********************************************************************
VCI_To_APDU-----应用层转底层
Parameters:pCanRecvMsg:由协议层上传的消息内容
Return Value:
**********************************************************************/
void APDU_To_VCI( CAN_OBJ *pVci, CAN_APDU_T *pApdu)
{
    pVci->ID = (((uint16_t)pApdu->ucMsgClass)<<6)+pApdu->ucSourceId;
//	pVci->TimeStamp;//保留不用 
//	pVci->TimeFlag;//保留不用 
//	pVci->SendType;//保留不用
	
	pVci->RemoteFlag = 0;//是否是远程帧
	pVci->ExternFlag = 0;//是否是扩展帧
	pVci->DataLen = pApdu->u16Length+2;
	pVci->Data[0] = (pApdu->ucFrag<<7)|(pApdu->ucDestinationId);
	pVci->Data[1] = (pApdu->ucRsRq<<7)|(pApdu->ucServiceCode);
	memcpy(pVci->Data+2, pApdu->pData, pApdu->u16Length);
//	pVci->Reserved[3];
}

/**********************************************************************
FlashUpdateRoutine-----FLAHS UPDATE主程序              
Parameters:              	
Return Value: 
**********************************************************************/
void CanFlashupdate::FlashUpdateRoutine(void)
{
	CAN_XMIT_QUEUE_MSG_T TempMsg;

	uint8_t ucStateFlag;

	ucStateFlag = 0;

	//置FLASH UDATE 状态机启动
	switch(m_pHostModuleItc->u16FlashupdateStatus)
	{
		//开始升级
		//获取升级对象
		case STATUS_FLASH_START:

            qDebug("m_u16ProgramPorcess = PROGRESS_IN_START\n");

			 //升级进度标志
	 		m_u16ProgramPorcess = PROGRESS_IN_START;
			 
			//复位读取HEX文件结束标志
			m_u16ReadHexFileEnd = READ_HEX_END_FLAG_RESET;
			
			m_pHostModuleItc->u16FlashupdateStatus = STATUS_SELCET_NODE;
			
			//置当前传输的Block数为0
			m_u16SendBlocks = 0;
			m_bReadEnd = FALSE;
			m_bSectSendCplt = TRUE;	//默认值为一次性传完
			m_bSendFlag = FALSE;	//未传输标志位
			break;

		//对指定节点开始升级
		//首先发握手命令
		case STATUS_SELCET_NODE:	
			//发握手命令
			TempMsg.u16DestinationId = 0x3f;
			TempMsg.ucServiceCode = HANDS_COMM_SRVCODE;
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
            qDebug("Send HANDS_COMM.DestinationId=%02X", TempMsg.u16DestinationId);
			Msg_Xmit(&TempMsg);			
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
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
			Msg_Xmit(&TempMsg);
			
			break;
		//等待擦除命令应答
		case STATUS_FLASH_ERASE_GOING:

			break;
		//擦除成功
		case STATUS_FLASH_ERASED:
			//下发编程请求命令
			TempMsg.u16DestinationId = 0x3f;
			TempMsg.ucServiceCode = PROGRAM_SRVCODE;
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
			Msg_Xmit(&TempMsg);
			break;
		//等待编程允许状态
		case STATUS_PROGRAM_PERMIT_WAITING:
			//do nothing
			break;
		//编程允许
		case STATUS_PROGRAM_ENABLE:
			{
				static int jj = 1;
			//--------------------------------------
			//首先读一个Section相关信息
			//从总的缓冲区中指定位置开始读出一个BLOCK的信息
L1:			//首先要计算BLOCK的大小

			//上次有数据未处理完，则优先处理
			if (!m_bSectSendCplt)
			{
				m_bSectSendCplt = TRUE;
				m_uSectionAddress = 0x3f4000;
				m_wSectionLen = m_SaveLen;
				memcpy(m_ucSectionBuff, m_pSaveData, m_wSectionLen*2);
				delete[]m_pSaveData;
			}
			else if (!m_bReadEnd)
			{
				ReadASectionAsciiHex(cFlashUpdateBuf, m_ulPos, m_wSectionLen, m_uSectionAddress, m_ucSectionBuff);
				jj++;

                qDebug("%d: Read section:m_ulPos=%llx, m_wSectionLen=0x%x,m_uSectionAddress=0x%x",
                       jj-1, m_ulPos,m_wSectionLen,m_uSectionAddress);

                qDebug("Flash update: Read a section from target file--over");

				if (m_wSectionLen>0)
				{
					bool bFlashKernel = (m_pHostModuleItc->u16FlashUpdateKernelFlag==0x95);
					parseFlashAddress(m_wSectionLen, m_uSectionAddress, m_ucSectionBuff, 
						m_u16FlagValue, bFlashKernel,
						m_bSectSendCplt, m_pSaveData, m_SaveLen);
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
					*((uint16_t*)m_ucSectionBuff) = m_u16FlagValue;
					m_uSectionAddress = 0x3f3fff;
				}
				else	//传输结束帧
				{
					m_wSectionLen = 0;
					m_uSectionAddress = 0;
				}

			}
            qDebug("after parseFlashAddress:m_ulPos=%llx, m_wSectionLen=0x%x,m_uSectionAddress=0x%x",
                   m_ulPos,m_wSectionLen,m_uSectionAddress);

			//计算本次SECTION共划分多少个BLOCK
			m_u16BlockNumOfSection = m_wSectionLen/BLOCK_SIZE;

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

			//--------------------------------------
			
			//下发BLOCK头
			TempMsg.u16DestinationId = 0x3f;
			TempMsg.ucServiceCode = BLOCK_HEAD_SRVCODE;
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;

            qDebug("Read section: u16DestinationId=%x,ucServiceCode=%x,ucMsgClass=%x,ucSourceId=%x\n",
				TempMsg.u16DestinationId,TempMsg.ucServiceCode,TempMsg.ucMsgClass,TempMsg.ucSourceId);
            qDebug("Read section: Read over and start a task for sending head\n");

			Msg_Xmit(&TempMsg);
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
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
			Msg_Xmit(&TempMsg);
			break;

		//头传输结束
		case STATUS_BLOCK_HEAD_OK:
		//BLOCK数据传输过程中
		case STATUS_BLOCK_DATATRANS_WAITING:
			TempMsg.u16DestinationId = 0x3f;
			TempMsg.ucServiceCode = BLOCK_DATA_SRVCODE;
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
			Msg_Xmit(&TempMsg);
			break;

		//BLOCK数据传输结束
		case STATUS_BLOCK_DATATRANS_END:
			TempMsg.u16DestinationId = 0x3f;
			TempMsg.ucServiceCode = BLOCK_CHECKSUM_SRVCODE;
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
			Msg_Xmit(&TempMsg);
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
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
			Msg_Xmit(&TempMsg);

			break;
		//编程状态
		case STATUS_BLOCK_PROGRAM_WAITING:
			//DO NOTHING
			break;

		//编程完毕
		case STATUS_BLOCK_PROGRAM_COMPLETE:
            qDebug("PROGRAM_COMPLETE!");
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
			//状态机复位
			m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_INVALID;
			//升级进度标志复位
	 		m_u16ProgramPorcess = PROGRESS_IN_RESET;
            qDebug("flash update over!");
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

/**********************************************************************
NewPeriodicWaitMsgGen-----定时等待到则将擦除命令入队             
Parameters:		
Return Value: 
**********************************************************************/
int CanFlashupdate::NewPeriodicWaitMsgGen(uint nIDEvent)
{
	int i,j;
	CAN_XMIT_QUEUE_MSG_T TempMsg;


	//超时退出升级,并且报失败
	//如果启动了则累加,没启动也不重发
	i = nIDEvent-FLASH_TIMER_BASE;

	m_u16TimerExpiredCnt[i]++;
	
	if (m_u16TimerExpiredCnt[i] > RESEND_WAITING_MAX_CNT)
	{
		m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_OVER;
		
		//升级进度标志复位
		m_u16ProgramPorcess = PROGRESS_IN_RESET;
		
		//返回给后台失败信息
		for ( j=0; j<INV_MODULE_MAX_CNT; j++)
		{
			m_pHostModuleItc->u16UpdateStatus[j] = STATUS_HOST_UPDATE_FAIL;
		}
		
		for ( j=0; j<TIMER_CNT_LEN;j++)
		{
			m_u16TimerExpiredCnt[j] = RESEND_WAITING_RESET_CNT;
		}

		//升级失败提示
        FlashUpdateThreadConv = 4;	//升级失败标志
	}	
	else
	{
		if (0 == i)
		{
			//取得指定升级模块中有响应的模块
			UINT32 uHasRespondMod = m_pHostModuleItc->m_FlashModIdx & m_pHostModuleItc->m_FlashRsModIdx;
			
			//取得后台指定升级但握手无反应的模块，认为不在线
			UINT32 u32NoRespondModule = (m_pHostModuleItc->m_FlashModIdx ^ uHasRespondMod);
			
			//不响应反馈给后台表示指定的模块不在线
			for (j=0; j <INV_MODULE_MAX_CNT; j++)
			{
				if (((u32NoRespondModule>>j)&1) == 1)
				{
					m_pHostModuleItc->u16UpdateStatus[j] = STATUS_UPDATE_NODE_OFF_LINE;
				}
				
			}
			
			//取得可以升级的模块
			m_pHostModuleItc->m_FlashModIdx = m_pHostModuleItc->m_FlashRsModIdx;	
			
            qDebug("Can Flash Mod=0x%08X", m_pHostModuleItc->m_FlashModIdx);
			
			//后台指定的模块存在允许升级，则将状态机转到下一个状态
			if (m_pHostModuleItc->m_FlashModIdx)
			{
				//关握手定时器
//				m_pProtocolMgr->KillTimer(nIDEvent);
				
				m_u16TimerExpiredCnt[0] = RESEND_WAITING_RESET_CNT;	
                qDebug("stopped HandleTimer!");
				
				//返回给后台正在升级标志
				for (int k=0; k<INV_MODULE_MAX_CNT; k++)
				{
					if (m_pHostModuleItc->m_FlashModIdx&(1<<k))
					{
						m_pHostModuleItc->u16UpdateStatus[k] = STATUS_HOST_UPDAT_ING;
					}
				}
				//重置下一个进度的响应标志
				m_pHostModuleItc->m_FlashRsModIdx = 0;
				
				//下发获取芯片解密信息
				TempMsg.u16DestinationId = 0x3f;
				TempMsg.ucServiceCode = CHIP_DECODE_SRVCODE;
				TempMsg.ucMsgClass = m_ucMsgClass;
				TempMsg.ucFrag = NONFRAG_MSG;
				TempMsg.ucRsRq = RS_MSG;
				TempMsg.ucSourceId = MAC_ID_MON;
				Msg_Xmit(&TempMsg);
				
				//升级进度标志
				m_u16ProgramPorcess = PROGRESS_IN_HAND_OK;
			}
			else 
			{
				TempMsg.u16DestinationId = 0x3f;
				TempMsg.ucServiceCode = PeriodicSendEraseCmdObjs[i].ucServiceCode;
				TempMsg.ucMsgClass = m_ucMsgClass;
				TempMsg.ucFrag = NONFRAG_MSG;
				TempMsg.ucRsRq = RS_MSG;
				TempMsg.ucSourceId = MAC_ID_MON;
				Msg_Xmit(&TempMsg);	
			}
			
		}
		else if (i==6)
		{
			//出现校验错误，重发
			if (m_u16CheckSumError==1)
			{
				//升级进度标志,重发允许接收BLOCK头应答
				m_u16ProgramPorcess = PROGRESS_IN_PROG_ENA_OK;
				//重置响应标志
				m_pHostModuleItc->m_FlashRsModIdx = 0;
				ResendOneBlock();
			}
			else //未收到所有模块的校正和
			{
				TempMsg.u16DestinationId = 0x3f;
				TempMsg.ucServiceCode = PeriodicSendEraseCmdObjs[i].ucServiceCode;
				TempMsg.ucMsgClass = m_ucMsgClass;
				TempMsg.ucFrag = NONFRAG_MSG;
				TempMsg.ucRsRq = RS_MSG;
				TempMsg.ucSourceId = MAC_ID_MON;
				Msg_Xmit(&TempMsg);	
				
				//重置响应标志
				m_pHostModuleItc->m_FlashRsModIdx = 0;
			}
		}
		else
		{
			TempMsg.u16DestinationId = 0x3f;
			TempMsg.ucServiceCode = PeriodicSendEraseCmdObjs[i].ucServiceCode;
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
			Msg_Xmit(&TempMsg);	
			
			//重置响应标志
			m_pHostModuleItc->m_FlashRsModIdx = 0;
		}
		
	}
	
	
	return 0;
}

/**********************************************************************
ResendOneBlock-----获取要升级的节点             
Parameters:		
Return Value: 
**********************************************************************/
VOID CanFlashupdate::ResendOneBlock(VOID)
{
	uint16_t j;
	
	if (m_u16ResendCnt)
	{
		m_u16CurrentFrameNum = 0;
		//状态机回到传输下一个BLOCK头
		m_pHostModuleItc->u16FlashupdateStatus = STATUS_NEXT_BLOCK_HEAD;
		m_u16ResendCnt--;
		
	}

	//重发超3次报错并退出升级TBD
	else
	{
		//状态机回到传输下一个BLOCK头
		m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_OVER;
		
		//升级进度标志复位
	 	m_u16ProgramPorcess = PROGRESS_IN_RESET;

		//返回给后台失败信息
		for ( j=0; j<INV_MODULE_MAX_CNT;j++)
		{
			m_pHostModuleItc->u16UpdateStatus[j] = STATUS_HOST_UPDATE_FAIL;
		}

		for ( j=0; j<TIMER_CNT_LEN;j++)
		{
			m_u16TimerExpiredCnt[j] = RESEND_WAITING_RESET_CNT;
		}
	}
}

/**********************************************************************
SetRespondModuleFlag-----FLASH UPDATE 相应模块有握手应答，说明可升级            
Parameters:	
Return Value:
**********************************************************************/
VOID CanFlashupdate::SetRespondModuleFlag(uint8_t ucRecvAddr)
{
	if (m_pHostModuleItc->sFlashUpdateFlag==FLASHUPDATE_OBJECT_REC)
	{
		if (ucRecvAddr>=0x02 && ucRecvAddr<=0x04)
		{
			m_pHostModuleItc->m_FlashRsModIdx |= (1<<(ucRecvAddr-0x02));
		}	
	}
	else if (m_pHostModuleItc->sFlashUpdateFlag==FLASHUPDATE_OBJECT_INV)
	{
		if (ucRecvAddr>=0x05 && ucRecvAddr<=0x1E)
		{
			m_pHostModuleItc->m_FlashRsModIdx |= (1<<(ucRecvAddr-0x05));
		}
	}
	else
	{
		//do sth
	}

}

/**********************************************************************
ChipDecodeXmitFcb-----解密命令下发--保留               
Parameters:	
Return Value: 
**********************************************************************/
int CanFlashupdate::ChipDecodeXmitFcb(VOID)
{
	
	uint8_t ucErrCode = CAN_MSG_HANDLE_OK;

	m_XmitMsg.pData = m_ucXmitMsgBuf;

	//升级与协议层交互的信息
	m_XmitMsg.ucRsRq = RS_MSG;
	//传送2字节
	m_XmitMsg.u16Length = 0;

	//等待解密信息反馈,启动等待延时TBD
	//先赋状态值，再发送，防止已经收到回应后再赋值
	m_pHostModuleItc->u16FlashupdateStatus = STATUS_WAITING_CHIP_DECODE;

	//超时计数器处理
	if (RESEND_WAITING_RESET_CNT == m_u16TimerExpiredCnt[1])
	{
		m_u16TimerExpiredCnt[1] = RESEND_WAITING_START_CNT;
	}

	return ucErrCode;
}

/**********************************************************************
ChipDecodeRecvFcb-----解密信号接收           
Parameters:		=0xaa表示握手成功		
Return Value: 
**********************************************************************/
int CanFlashupdate::ChipDecodeRecvFcb(VOID)
{
	uint16_t u16RetrunStatus;
	CAN_XMIT_QUEUE_MSG_T TempMsg;

	u16RetrunStatus = *(uint16_t *)(m_RecvMsg.pData);;

	//如果进度不在等待解密应答则放弃此次接收到的应答信号
	if (PROGRESS_IN_HAND_OK != m_u16ProgramPorcess)
	{
		return 1;
	}
	
	//芯片解密OK
	if (CHIP_DECODE_SUCCESS == u16RetrunStatus)
	{
		if (0 == FlashupdateTaskHandle(m_RecvMsg.ucSourceId))
		{	
			//升级进度标志
	 		m_u16ProgramPorcess = PROGRESS_IN_DECODE_OK;

			//下发获取API版本信息
			TempMsg.u16DestinationId = 0x3f;
			TempMsg.ucServiceCode = API_VERSION_SRVCODE;
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;

			Msg_Xmit(&TempMsg);

			//reset task
			FlashupdateTaskReset();

			//reset expired cnt
			m_u16TimerExpiredCnt[1] = RESEND_WAITING_RESET_CNT;


			
		}

		//wait othger node repond
		else
		{
			;//do nothing
		}
	}

	//擦除失败,置反馈状态字给后台TBD
	else
	{
		;//
	}
    return 0;
}

/**********************************************************************
FlashupdateTaskHandle-----FLASH UPDATE 某项任务是否都已处理完毕                 
Parameters:		
Return Value: =0表示处理完成,=1还有节点待处理
**********************************************************************/
uint8_t CanFlashupdate::FlashupdateTaskHandle(uint8_t ucRecvAddr)
{
    uint8_t ucRet;
	ucRet = 1;

	if (m_pHostModuleItc->sFlashUpdateFlag==FLASHUPDATE_OBJECT_REC)
	{
		if (ucRecvAddr>=0x02 && ucRecvAddr<=0x04)
		{
			m_pHostModuleItc->m_FlashRsModIdx |= (1<<(ucRecvAddr-0x02));
		}	
	}
	else if (m_pHostModuleItc->sFlashUpdateFlag==FLASHUPDATE_OBJECT_INV)
	{
		if (ucRecvAddr>=0x05 && ucRecvAddr<=0x1E)
		{
			m_pHostModuleItc->m_FlashRsModIdx |= (1<<(ucRecvAddr-0x05));
		}
	}

	if (m_pHostModuleItc->m_FlashRsModIdx==m_pHostModuleItc->m_FlashModIdx)
	{
		m_pHostModuleItc->m_FlashRsModIdx = 0;
		ucRet = 0;
	}

	return ucRet;
		
}

/**********************************************************************
ApiVersionXmitFcb-----API版本核对命令下发--保留          
Parameters:		
Return Value: 
**********************************************************************/
int CanFlashupdate::ApiVersionXmitFcb(VOID)
{
	
	uint8_t ucErrCode = CAN_MSG_HANDLE_OK;

	m_XmitMsg.pData = m_ucXmitMsgBuf;

	//升级与协议层交互的信息
	m_XmitMsg.ucRsRq = RS_MSG;
	//传送2字节
	m_XmitMsg.u16Length = 0;

	//等待API版本核对信息反馈,启动等待延时TBD
	m_pHostModuleItc->u16FlashupdateStatus = STATUS_WAITING_API_VERSION;

	//超时计数器处理
	if (RESEND_WAITING_RESET_CNT == m_u16TimerExpiredCnt[2])
	{
		m_u16TimerExpiredCnt[2] = RESEND_WAITING_START_CNT;
	}
	return ucErrCode;
}

/**********************************************************************
ApiVersionRecvFcb-----API版本核对信号接收
Parameters: =0x6C表示版本匹配,允许擦除 =14 版本不匹配
Return Value: 
**********************************************************************/
int CanFlashupdate::ApiVersionRecvFcb(VOID)
{
	uint16_t u16RetrunStatus;

	u16RetrunStatus = *(uint16_t *)(m_RecvMsg.pData);;

	//如果进度不在等待API版本应答则放弃此次接收到的应答信号
	if (PROGRESS_IN_DECODE_OK != m_u16ProgramPorcess)
	{
		return 1;
	}
	
	//API版本核对成功
	if (API_VESION_OK == u16RetrunStatus)
	{
		if (0 == FlashupdateTaskHandle(m_RecvMsg.ucSourceId))
		{
			//升级进度标志
	 		m_u16ProgramPorcess = PROGRESS_IN_APIVERSION_OK;

			m_pHostModuleItc->u16FlashupdateStatus = STATUS_API_OK;

			//reset task
			FlashupdateTaskReset();

			//reset expired cnt
			m_u16TimerExpiredCnt[2] = RESEND_WAITING_RESET_CNT;			
		}

		//wait othger node repond
		else
		{
			;//do nothing
		}
	}

	//擦除失败,置反馈状态字给后台TBD
	else
	{
		;//
	}
	
    return 0;
}

/**********************************************************************
FlashupdateTaskReset-----FLASH UPDATE 某项任务启动                  
Parameters:		
Return Value: 
**********************************************************************/
VOID CanFlashupdate::FlashupdateTaskReset(VOID)
{
	uint8_t i;

	for (i=0; i<INV_MODULE_MAX_CNT; i++)
	{
		if (TARGET_UPDATE_ENABLE == m_tFlashupdateTarged[i].ucTargetEnable)
		{
			 m_tFlashupdateTarged[i].ucTaskHandled = TASK_HANDLE_NON;
		}

	}		
}

/**********************************************************************
EraseSectorXmitFcb-----扇区擦除命令下发                
Parameters:			
Return Value: 
**********************************************************************/
int CanFlashupdate::EraseSectorXmitFcb(VOID)
{
	uint8_t ucErrCode = CAN_MSG_HANDLE_OK;
	
	m_XmitMsg.pData = m_ucXmitMsgBuf;

	//升级与协议层交互的信息
	m_XmitMsg.ucRsRq = RS_MSG;
	//传送2字节擦除指定扇区
	m_XmitMsg.u16Length = 2;

	//将指针m_XmitMsg.pData也指向将要发送的数据
	if (0x95 == m_pHostModuleItc->u16FlashUpdateKernelFlag)
	{
		*((uint16_t *)(m_XmitMsg.pData + 0)) = ERASE_SECTOR_ALL;
	}

	else
	{
		*((uint16_t *)(m_XmitMsg.pData + 0)) = ERASE_SECTOR_BCD;
	}

	m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_ERASE_GOING;
	
	//超时计数器处理
	if (RESEND_WAITING_RESET_CNT == m_u16TimerExpiredCnt[3])
	{
		m_u16TimerExpiredCnt[3] = RESEND_WAITING_START_CNT;
	}
	
	return ucErrCode;
}

/**********************************************************************
EraseSectorRecvFcb-----扇区擦除命令反馈               
Parameters:			
Return Value:  
**********************************************************************/
int CanFlashupdate::EraseSectorRecvFcb(VOID)
{
	uint16_t u16RetrunStatus;

	u16RetrunStatus = *(uint16_t *)(m_RecvMsg.pData);;

	//如果进度不在等待API版本应答则放弃此次接收到的应答信号
	if (PROGRESS_IN_APIVERSION_OK != m_u16ProgramPorcess)
	{
		return 1;
	}
	
	//擦除成功,准备发编程命令
	if (ERASE_SUCCESFULL == u16RetrunStatus)
	{
		if (0 == FlashupdateTaskHandle(m_RecvMsg.ucSourceId))
		{
			//升级进度标志
	 		m_u16ProgramPorcess = PROGRESS_IN_ERASE_OK;

			m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_ERASED;

			//reset task
			FlashupdateTaskReset();

			//reset expired cnt
			m_u16TimerExpiredCnt[3] = RESEND_WAITING_RESET_CNT;			
		}

		//wait othger node repond
		else
		{
			;//do nothing
		}
	}

	//擦除失败TBD
	else
	{
		;//
	}
	
    return 0;
}

/**********************************************************************
ProgramXmitFcb-----编程命令下发               
Parameters:		
Return Value: 
**********************************************************************/
int CanFlashupdate::ProgramXmitFcb(VOID)
{
	uint8_t ucErrCode = CAN_MSG_HANDLE_OK;
	
	m_XmitMsg.pData = m_ucXmitMsgBuf;

	//升级与协议层交互的信息
	m_XmitMsg.ucRsRq = RS_MSG;
	
	m_XmitMsg.u16Length = 0;

	m_pHostModuleItc->u16FlashupdateStatus = STATUS_PROGRAM_PERMIT_WAITING;

	//超时计数器处理
	if (RESEND_WAITING_RESET_CNT == m_u16TimerExpiredCnt[4])
	{
		m_u16TimerExpiredCnt[4] = RESEND_WAITING_START_CNT;
	}

	return ucErrCode;
}

/**********************************************************************
ProgramRecvFcb-----编程命令反馈                
Parameters:		
Return Value: 
**********************************************************************/
int CanFlashupdate::ProgramRecvFcb(VOID)
{
	uint16_t u16RetrunStatus;
	uint16_t j;

	u16RetrunStatus = *(uint16_t *)(m_RecvMsg.pData);

    qDebug("Flash update: Recv program permit1,nAddr=%02X", m_RecvMsg.ucSourceId);

	//如果进度不合法则放弃此次接收到的应答信号
	if (PROGRESS_IN_ERASE_OK != m_u16ProgramPorcess)
	{
		return 1;
	}
	
	//允许编程,准备发数据头信息
	if (PROGRAM_ENABLE == u16RetrunStatus)
	{
		if (0 == FlashupdateTaskHandle(m_RecvMsg.ucSourceId))
		{
			//升级进度标志
	 		m_u16ProgramPorcess = PROGRESS_IN_PROG_ENA_OK;
			
			m_pHostModuleItc->u16FlashupdateStatus = STATUS_PROGRAM_ENABLE;

			m_u16ResendCnt = BLOCK_RESEND_CNT;

			//for test
            qDebug("Flash update: Recv program permit succesful,\n");
			
			//reset task
			FlashupdateTaskReset();

			//reset expired cnt
			m_u16TimerExpiredCnt[4] = RESEND_WAITING_RESET_CNT;
		}

		//wait othger node repond
		else
		{
			;//do nothing
		}
	}

	//擦除失败TBD
	else
	{
		//--20100408:直接退出--
		m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_OVER;
		//升级进度标志复位
	 	m_u16ProgramPorcess = PROGRESS_IN_RESET;

		//返回给后台失败信息
		for (j=0; j<INV_MODULE_MAX_CNT; j++)
		{
			m_pHostModuleItc->u16UpdateStatus[j] = STATUS_HOST_UPDATE_FAIL;
		}

		for ( j=0; j<TIMER_CNT_LEN; j++)
		{
			m_u16TimerExpiredCnt[j] = RESEND_WAITING_RESET_CNT;
		}
	}
	
    return 0;
}

/**********************************************************************
BlockHeadXmitFcb-----下传BLOCK数据                 
Parameters:		
Return Value:  
**********************************************************************/
int CanFlashupdate::BlockHeadXmitFcb(VOID)
{
	uint8_t ucErrCode = CAN_MSG_HANDLE_OK;
	uint16_t u16BlockLen;
	UINT32 u32BlockAddr;

    qDebug("Flash update: Send head--m_u16CurrentBlockNum=%d,m_u16BlockNumOfSection=%d",
           m_u16CurrentBlockNum,m_u16BlockNumOfSection);

	//取当前待传的BLOCK的长度和地址
	if (m_u16CurrentBlockNum >= (m_u16BlockNumOfSection-1))
	{
		u16BlockLen = m_u16RemainWordOfSection;
		//总的帧数
		m_u16FrameNumOneBlock = m_u16FrameNumOfRemainBlock;
		//最后一帧字的个数
		m_u16LastFrameWordNumOneBlock = m_u16LastFrameWordNumOfRemainBlock;

        qDebug("m_u16LastFrameWordNumOneBlock=%d", m_u16LastFrameWordNumOneBlock);

		m_u32CheckSumOneBlock  
			= CheckSum(u16BlockLen, m_uBlockAddress, (m_ucSectionBuff+m_u16CurrentBlockNum*BLOCK_SIZE*2));
	}

	else
	{
		//1024字
		u16BlockLen = BLOCK_SIZE;
		//一个完整的BLOCK需要分的帧数
		m_u16FrameNumOneBlock = FRAME_ALL_NUM_ONE_BLOCK;
		//最后一帧字的个数
		m_u16LastFrameWordNumOneBlock = LAST_FRAME_WORD_NUM_ONE_BLOCK;

		//本次BLOCK的校验和
		m_u32CheckSumOneBlock 
			= CheckSum(u16BlockLen, m_uBlockAddress, (m_ucSectionBuff+m_u16CurrentBlockNum*BLOCK_SIZE*2));
	}

	//当前帧号
	m_u16CurrentFrameNum = 0;
	//BLOCK地址
	u32BlockAddr = m_uBlockAddress;
	m_XmitMsg.pData = m_ucXmitMsgBuf;
	//升级与协议层交互的信息
	m_XmitMsg.ucRsRq = RS_MSG;
	//传送6字节信息头
	m_XmitMsg.u16Length = 6;

	//将指针m_XmitMsg.pData也指向将要发送的数据
	//数据来源取自后台下传的DATA TBD
	*((uint16_t *)(m_XmitMsg.pData + 0)) = u16BlockLen;
	*((uint16_t *)(m_XmitMsg.pData + 2)) = (uint16_t)(u32BlockAddr&0x0ffff);
	*((uint16_t *)(m_XmitMsg.pData + 4)) = (uint16_t)((u32BlockAddr>>16)&0x0ffff);

	m_pHostModuleItc->u16FlashupdateStatus = STATUS_BLOCK_HEAD_WAITING;

	//for test
    qDebug("Flash update: Send one block head\n");
    qDebug("Flash update: Send head--m_u16FrameNumOneBlock=%d\n",m_u16FrameNumOneBlock);
    emit e_BlockProgressSet(m_u16FrameNumOneBlock-1);
	
	//超时计数器处理
	if (RESEND_WAITING_RESET_CNT == m_u16TimerExpiredCnt[5])
	{
		m_u16TimerExpiredCnt[5] = RESEND_WAITING_START_CNT;
	}

	return ucErrCode;
}

/**********************************************************************
BlockHeadRecvFcb-----下传BLOCK数据应答                  
Parameters:		
Return Value: 
**********************************************************************/
int CanFlashupdate::BlockHeadRecvFcb(VOID)
{
	uint16_t u16RetrunStatus;
	uint16_t j;

	u16RetrunStatus = *(uint16_t *)(m_RecvMsg.pData);

	//如果进度不合法则放弃此次接收到的应答信号
	if (PROGRESS_IN_PROG_ENA_OK != m_u16ProgramPorcess)
	{
        qDebug("m_u16TimerExpiredCnt[5]=%d\n", m_u16TimerExpiredCnt[5]);
		return 1;
	}

    qDebug("Flash update: Recv one block head respond %d, nAddr=0x%02X", u16RetrunStatus, m_RecvMsg.ucSourceId);
	
	//允许传有效数据,准备发数据信息
	if (BLOCK_HEAD_OK == u16RetrunStatus)
	{
		if (0 == FlashupdateTaskHandle(m_RecvMsg.ucSourceId))
		{
			//升级进度标志
	 		m_u16ProgramPorcess = PROGRESS_IN_HEADRESPOND_OK;
			
			m_pHostModuleItc->u16FlashupdateStatus = STATUS_BLOCK_HEAD_OK;
			m_u16ResendCnt = BLOCK_RESEND_CNT;

			//reset task
			FlashupdateTaskReset();

			//reset expired cnt
			m_u16TimerExpiredCnt[5] = RESEND_WAITING_RESET_CNT;

            qDebug("Flash update: Recv ALL block head respond=0");
		}
	}

	//文件传输结束
	else if (FILE_TRANS_END == u16RetrunStatus)
	{
		//如果DSP反馈升级成功信息,同时需与监控自身标志核对,
		//如果监控标志表示文件的确传完才认为结束并报成功
		//监控本身认为HEX文件还没传完,则报失败
		if (READ_HEX_END_FLAG_VALID != m_u16ReadHexFileEnd)
		{
			m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_OVER;
			//升级进度标志复位
	 		m_u16ProgramPorcess = PROGRESS_IN_RESET;

			//返回给后台失败信息
			for ( j=0; j<INV_MODULE_MAX_CNT;j++)
			{
				m_pHostModuleItc->u16UpdateStatus[j] = STATUS_HOST_UPDATE_FAIL;
			}

			for ( j=0; j<TIMER_CNT_LEN;j++)
			{
				m_u16TimerExpiredCnt[j] = RESEND_WAITING_RESET_CNT;
			}

			//直接退出
			return 1;
		}

		//升级完成了一个模块
		//反馈给后台状态信息
		//旁路
		if (0x0f == m_RecvMsg.ucSourceId )
		{			
			m_pHostModuleItc->u16UpdateStatus[0] = STATUS_HOST_UPDATE_SUCCESFULL;			
		}

		//双电源
		else if ((0x02 <= m_RecvMsg.ucSourceId ) && (0x04 >= m_RecvMsg.ucSourceId ))
		{
			m_pHostModuleItc->u16UpdateStatus[m_RecvMsg.ucSourceId-0x02] = STATUS_HOST_UPDATE_SUCCESFULL;
		}
		//逆变
		else if ((0x05 <= m_RecvMsg.ucSourceId ) && (0x1E >= m_RecvMsg.ucSourceId ))
		{
			m_pHostModuleItc->u16UpdateStatus[m_RecvMsg.ucSourceId-0x05] = STATUS_HOST_UPDATE_SUCCESFULL;
		}

		//do nothing
		else
		{
			
		}		
		
		//是否所有的模块都升级完毕
		if (0 == FlashupdateTaskHandle(m_RecvMsg.ucSourceId))
		{
			//更新当前升级进度
			m_u16SendBlocks++;
			m_pHostModuleItc->m_curFlashProgress = m_u16SendBlocks*100/m_pHostModuleItc->m_TotalFlashSteps;

			m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_OVER;
			
			//升级进度标志复位
	 		m_u16ProgramPorcess = PROGRESS_IN_RESET;

			//reset task
			FlashupdateTaskReset();

			m_u16ResendCnt = BLOCK_RESEND_CNT;

			//reset expired cnt
			m_u16TimerExpiredCnt[5] = RESEND_WAITING_RESET_CNT;
			
			m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_OVER;
            FlashUpdateThreadConv = 3;	//升级成功
		}

        qDebug("Flash update: Recv one block head respond=5, nAddress=0x%02X", m_RecvMsg.ucSourceId);
	}
	
	//需重发TBD
	else
	{
		ResendOneBlock();
	}
    return 0;
}

/**********************************************************************
CheckSum-----校验和计算
Parameters:
Return Value:
**********************************************************************/
UINT32 CanFlashupdate::CheckSum(uint16_t u16Length, UINT32 nAddress, uint8_t *buf)
{
	UINT32 R=0;
	uint16_t wData;
	
	R += u16Length;
	
	//	R += nAddress >> 16;
	//	R += nAddress &  0xFFFF;
	R += nAddress;
	
	for(int i=0; i<u16Length*2; i+=2 )
	{
		wData = ((uint16_t)buf[i+1] << 8) + buf[i];
		R += wData;
	}
	
	return R;
}

/**********************************************************************
BlockDataXmitFcb-----下传BLOCK数据             
Parameters:	
Return Value: 
**********************************************************************/
int CanFlashupdate::BlockDataXmitFcb(VOID)
{
	uint8_t ucErrCode = CAN_MSG_HANDLE_OK;

	//for test
    qDebug("Flash update: Send one block data1=%d, u16CurrentFrameNum=%d",
           m_u16CurrentBlockNum, m_u16CurrentFrameNum);
    emit e_BlockProgressDisp(m_u16CurrentFrameNum);

	//取当前需要传输的数据
	uint16_t *pu16Ptr = (uint16_t*)(m_ucSectionBuff+m_u16CurrentBlockNum*BLOCK_SIZE*2);
	pu16Ptr = pu16Ptr+m_u16CurrentFrameNum*3;
	
	m_XmitMsg.pData = m_ucXmitMsgBuf;

	//升级与协议层交互的信息
	m_XmitMsg.ucRsRq = RS_MSG;
	
	//当前BLOCK的最后一帧
	if (m_u16CurrentFrameNum >= (m_u16FrameNumOneBlock-1) )
	{
		//字节数
		m_XmitMsg.u16Length = m_u16LastFrameWordNumOneBlock*2;
		
		for (uint16_t i=0; i<m_u16LastFrameWordNumOneBlock; i++)
		{
			*((uint16_t *)(m_XmitMsg.pData + i*2)) = *(pu16Ptr+i);
		}

		//等待传下一个BLOCK
		m_pHostModuleItc->u16FlashupdateStatus = STATUS_BLOCK_DATATRANS_END;
	}

	else
	{
		//字节数
		m_XmitMsg.u16Length = NON_LAST_FRAME_WORD_NUM_ONE_BLOCK*2;
		
		*((uint16_t *)(m_XmitMsg.pData + 0)) = *pu16Ptr;
		*((uint16_t *)(m_XmitMsg.pData + 2)) = *(pu16Ptr+1);
		*((uint16_t *)(m_XmitMsg.pData + 4)) = *(pu16Ptr+2);
		m_pHostModuleItc->u16FlashupdateStatus = STATUS_BLOCK_DATATRANS_WAITING;
		m_u16CurrentFrameNum++;
	}
	
	return ucErrCode;
}

/**********************************************************************
BlockDataRecvFcb-----下传BLOCK数据应答              
Parameters:	
Return Value: 
**********************************************************************/
int CanFlashupdate::BlockDataRecvFcb(VOID)
{
	uint16_t u16RetrunStatus;
	CAN_XMIT_QUEUE_MSG_T TempMsg;

	u16RetrunStatus = *(uint16_t *)(m_RecvMsg.pData);

    qDebug("Flash update: Recv one block data respond1, nAddr=0x%02X", m_RecvMsg.ucSourceId);
	if (m_RecvMsg.ucSourceId==0x01)		//屏蔽监控发来的消息,防止监控在线时蹦溃
		return 1;

	//允许传有效数据,准备发数据信息
	if (BLOCK_RECV_DATA_OK == u16RetrunStatus)
	{
		if (0 == FlashupdateTaskHandle(m_RecvMsg.ucSourceId))
		{
			TempMsg.u16DestinationId = 0x3f;
			TempMsg.ucServiceCode = BLOCK_DATA_SRVCODE;
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
			Msg_Xmit(&TempMsg);
		}

		//wait othger node repond
		else
		{
			;//do nothing
		}
			
	}

	//擦除失败TBD
	else
	{
		ResendOneBlock();
	}
	return 0;
}

/**********************************************************************
BlockChecksumXmitFcb-----下传BLOCK校验和数据              
Parameters:
Return Value: 
**********************************************************************/
int CanFlashupdate::BlockChecksumXmitFcb(VOID)
{
	uint8_t ucErrCode = CAN_MSG_HANDLE_OK;

    qDebug("Flash update: send one block checksum=%x",m_u32CheckSumOneBlock);
	
	m_XmitMsg.pData = m_ucXmitMsgBuf;

	//升级与协议层交互的信息
	m_XmitMsg.ucRsRq = RS_MSG;
	//传送6字节信息
	m_XmitMsg.u16Length = 4;

	//将指针m_XmitMsg.pData也指向将要发送的数据
	//数据来源取自后台下传的DATA TBD
	//定义一个指针
	*((uint16_t *)(m_XmitMsg.pData + 0)) = (uint16_t)(m_u32CheckSumOneBlock&0x0ffff);
	*((uint16_t *)(m_XmitMsg.pData + 2)) = (uint16_t)((m_u32CheckSumOneBlock>>16)&0x0ffff);
	
	m_pHostModuleItc->u16FlashupdateStatus = STATUS_BLOCK_CHECKSUM_WAITING;

	//清校正和错误标志
	m_u16CheckSumError = 0; 

	//超时计数器处理
	if (RESEND_WAITING_RESET_CNT == m_u16TimerExpiredCnt[6])
	{
		m_u16TimerExpiredCnt[6] = RESEND_WAITING_START_CNT;
	}

	return ucErrCode;
}

/**********************************************************************
BlockChecksumRecvFcb-----下传BLOCK数据校验和应答                 
Parameters:		
Return Value: 
**********************************************************************/
int CanFlashupdate::BlockChecksumRecvFcb(VOID)
{
	uint16_t u16RetrunStatus;

	//for test
    qDebug("Flash update: Recv one block checksum respond1");

    u16RetrunStatus = *(uint16_t *)(m_RecvMsg.pData);

	//如果进度不合法则放弃此次接收到的应答信号
	if (PROGRESS_IN_HEADRESPOND_OK != m_u16ProgramPorcess)
	{
        qDebug("BlockChecksumRecvFcb:PROGRESS_IN_HEADRESPOND_NOTOK!");
        qDebug("m_u16TimerExpiredCnt[6]=%d", m_u16TimerExpiredCnt[6]);
		return 1;
	}

	//状态机不在等待接收校验和状态则放弃
	if (STATUS_BLOCK_CHECKSUM_WAITING!=m_pHostModuleItc->u16FlashupdateStatus)
	{
        qDebug("BlockChecksumRecvFcb:u16FlashupdateStatus=%d", m_pHostModuleItc->u16FlashupdateStatus);
        qDebug("m_u16TimerExpiredCnt[6]=%d", m_u16TimerExpiredCnt[6]);
		return 1;
	}

    qDebug("respond=0x%02X, nAddr=0x%02X", u16RetrunStatus, m_RecvMsg.ucSourceId);
	
	//允许传有效数据,准备发数据信息
	if (CHECK_SUM_SUCCESFUL == u16RetrunStatus)
	{
		if (0 == FlashupdateTaskHandle(m_RecvMsg.ucSourceId))
		{
			//升级进度标志
	 		m_u16ProgramPorcess = PROGRESS_IN_BLOCKCHECK_OK;
			
			m_pHostModuleItc->u16FlashupdateStatus = STATUS_BLOCK_CHECKSUM_OK;

			m_u16ResendCnt = BLOCK_RESEND_CNT;
			
			//reset task
			FlashupdateTaskReset();

			//reset expired cnt
			m_u16TimerExpiredCnt[6] = RESEND_WAITING_RESET_CNT;

            qDebug("Flash update: Recv one block ALL checksum respond=0x82");
		}

		//wait othger node repond
		else
		{
			;//do nothing
		}
	}

	//校验失败,需重发TBD
	else
	{
		//置校正和错误标志,不关闭定时器，等待超时
		m_u16CheckSumError = 1;
		
		uint8_t ucRecvAddr = m_RecvMsg.ucSourceId;
		//置模块错误标志及次数
		m_ModErrorType = BLOCK_CHECKSUM_ERROR;
		if (m_pHostModuleItc->sFlashUpdateFlag==FLASHUPDATE_OBJECT_REC)
		{
			if (ucRecvAddr>=0x02 && ucRecvAddr<=0x04)
			{
				m_ModErrorCnt[ucRecvAddr-0x02]++;
			}	
		}
		else if (m_pHostModuleItc->sFlashUpdateFlag==FLASHUPDATE_OBJECT_INV)
		{
			if (ucRecvAddr>=0x05 && ucRecvAddr<=0x1E)
			{
				m_ModErrorCnt[ucRecvAddr-0x05]++;
			}
		}
	}
    return 0;
}

/**********************************************************************
BlockProgStatusXmitFcb-----获取FLASH编程状态信息                
Parameters:		
Return Value: 
**********************************************************************/
int CanFlashupdate::BlockProgStatusXmitFcb(VOID)
{
	uint8_t ucErrCode = CAN_MSG_HANDLE_OK;
	
	m_XmitMsg.pData = m_ucXmitMsgBuf;

	//升级与协议层交互的信息
	m_XmitMsg.ucRsRq = RS_MSG;
	//传送6字节信息
	m_XmitMsg.u16Length = 0;
		
	m_pHostModuleItc->u16FlashupdateStatus = STATUS_BLOCK_PROGRAM_WAITING;

	//超时计数器处理
	if (RESEND_WAITING_RESET_CNT == m_u16TimerExpiredCnt[7])
	{
		m_u16TimerExpiredCnt[7] = RESEND_WAITING_START_CNT;
	}

    qDebug("Send Block Program status");

	return ucErrCode;
}

/**********************************************************************
BlockProgStatusRecvFcb-----DSP上传编程状态               
Parameters:			
Return Value: 
**********************************************************************/
int CanFlashupdate::BlockProgStatusRecvFcb(VOID)
{
	uint16_t u16RetrunStatus;
	CAN_XMIT_QUEUE_MSG_T TempMsg;

	uint16_t j;

	u16RetrunStatus = *(uint16_t *)(m_RecvMsg.pData);

    qDebug("Flash update: Recv program status respond=%X,nAddr=0x%02X", u16RetrunStatus, m_RecvMsg.ucSourceId);

	//如果进度不合法则放弃此次接收到的应答信号
	if (PROGRESS_IN_BLOCKCHECK_OK != m_u16ProgramPorcess)
	{
		return 1;
	}
	
	//允许传有效数据,准备发数据信息
	if (BLOCK_RECV_DATA_OK == u16RetrunStatus)
	{

		if (0 == FlashupdateTaskHandle(m_RecvMsg.ucSourceId))
        {
            //升级进度标志
	 		m_u16ProgramPorcess = PROGRESS_IN_FLASH_PROG_OK;
			
			//DSP反馈本次BLOCK编程成功
			//准备接收FLASH接收校验状态
			m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_VERIFYING;


			//下发获取FLASH校验信息
			TempMsg.u16DestinationId = 0x3f;
			TempMsg.ucServiceCode = VERIFY_SRVCODE;
			TempMsg.ucMsgClass = m_ucMsgClass;
			TempMsg.ucFrag = NONFRAG_MSG;
			TempMsg.ucRsRq = RS_MSG;
			TempMsg.ucSourceId = MAC_ID_MON;
			Msg_Xmit(&TempMsg);
			
			//reset task
			FlashupdateTaskReset();

			//reset expired cnt
			m_u16TimerExpiredCnt[7] = RESEND_WAITING_RESET_CNT;
			
			m_u16ResendCnt = BLOCK_RESEND_CNT;
		}

		//wait othger node repond
		else
		{
			;//do nothing
		}
		
	}

	//本次BLOCK编程失败,需重发TBD
	else
	{
		//--20100408:直接退出--
		m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_OVER;

		//升级进度标志复位
	 	m_u16ProgramPorcess = PROGRESS_IN_RESET;
		
		//返回给后台失败信息
		for (j=0; j<INV_MODULE_MAX_CNT; j++)
		{
			m_pHostModuleItc->u16UpdateStatus[j] = STATUS_HOST_UPDATE_FAIL;
		}

		for ( j=0; j<TIMER_CNT_LEN; j++)
		{
			m_u16TimerExpiredCnt[j] = RESEND_WAITING_RESET_CNT;
		}
	}
    return 0;
}

/**********************************************************************
VerifyXmitFcb-----校验命令下发                
Parameters:		
Return Value: 
**********************************************************************/
int CanFlashupdate::VerifyXmitFcb(VOID)
{
	uint8_t ucErrCode = CAN_MSG_HANDLE_OK;
	
	m_XmitMsg.pData = m_ucXmitMsgBuf;

	//升级与协议层交互的信息
	m_XmitMsg.ucRsRq = RS_MSG;
	
	m_XmitMsg.u16Length = 0;
	m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_VERIFYING;

	//超时计数器处理
	if (RESEND_WAITING_RESET_CNT == m_u16TimerExpiredCnt[8])
	{
		m_u16TimerExpiredCnt[8] = RESEND_WAITING_START_CNT;
	}

	return ucErrCode;
}

/**********************************************************************
VerifyRecvFcb-----校验命令反馈                  
Parameters:		
Return Value: 
**********************************************************************/
int CanFlashupdate::VerifyRecvFcb(VOID)
{
	uint16_t u16RetrunStatus;
	uint16_t j;
	
    printf("Flash update: verify recv1,m_u16CurrentBlockNum=%d,m_u16BlockNumOfSection=%d",
           m_u16CurrentBlockNum,m_u16BlockNumOfSection);

	u16RetrunStatus = *(uint16_t *)(m_RecvMsg.pData);;

	//如果进度不合法则放弃此次接收到的应答信号
	if (PROGRESS_IN_FLASH_PROG_OK != m_u16ProgramPorcess)
	{
        qDebug("pROGRESS_IN_FLASH_PROG_NOTOK,m_u16ProgramPorcess=0x%X", m_u16ProgramPorcess);
		return 1;
	}
	
	//校验成功,升级成功,反馈给后台TBD
	if (VERIFY_OK == u16RetrunStatus)
	{
        qDebug("vERIFY_OK!\n");
		if (0 == FlashupdateTaskHandle(m_RecvMsg.ucSourceId))
		{
			//更新当前升级进度
			m_u16SendBlocks++;
            emit e_UpProgressDisp(m_u16SendBlocks);

			//升级进度标志
	 		m_u16ProgramPorcess = PROGRESS_IN_PROG_ENA_OK;

			//本次SECTION所有BLOCK编程完成
			if (m_u16CurrentBlockNum >= (m_u16BlockNumOfSection-1))
			{
				//准备去读下一个section
				m_pHostModuleItc->u16FlashupdateStatus = STATUS_PROGRAM_ENABLE;		
				//for test
				printf("Flash update: verify recv11\n");
			}

			//还有BLOCK需传输,
			else
			{
				//当前BLOCK号加1
				m_u16CurrentBlockNum++;
				//重设模块错误次数
				memset(m_ModErrorCnt, 0, sizeof(m_ModErrorCnt));
				m_ModErrorType = FLASH_ERROR_NONE;
				//BLOCK地址加1024
				m_uBlockAddress = m_uBlockAddress+1024;
				//状态机回到传输下一个BLOCK头
				m_pHostModuleItc->u16FlashupdateStatus = STATUS_NEXT_BLOCK_HEAD;
				//for test
				printf("Flash update: verify recv22\n");
			}
			//reset task
			FlashupdateTaskReset();

			//reset expired cnt
			m_u16TimerExpiredCnt[8] = RESEND_WAITING_RESET_CNT;

			m_u16ResendCnt = BLOCK_RESEND_CNT;
		}

		//wait othger node repond
		else
		{
			;//do nothing
		}
		
	}

	//校验失败,需要重新擦除FLASH
	else
	{
		//--20100408:直接退出--
		m_pHostModuleItc->u16FlashupdateStatus = STATUS_FLASH_UPDATE_OVER;
		//升级进度标志复位
	 	m_u16ProgramPorcess = PROGRESS_IN_RESET;
		
		//返回给后台失败信息
		for (j=0; j<INV_MODULE_MAX_CNT; j++)
		{
			m_pHostModuleItc->u16UpdateStatus[j] = STATUS_HOST_UPDATE_FAIL;
		}

		for ( j=0; j<TIMER_CNT_LEN; j++)
		{
			m_u16TimerExpiredCnt[j] = RESEND_WAITING_RESET_CNT;
		}
	}
	
    return 0;
}

/**********************************************************************
ReadASectionAsciiHex-----读一个扇区数据
Parameters:
Return Value:
**********************************************************************/
void CanFlashupdate::ReadASectionAsciiHex(uint8_t *buf, uint64_t& pos, uint16_t& nLength, uint& nAddress, uint8_t *sectionbuff)
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
		printf("ReadASectionAsciiHex:read end!\n");
		return;
	}
	
	printf("Read Address!\r\n");
	
	nAddress = 0;
	
	//读取到地址
	if (buf[pos]==0x24 && buf[pos+1]==0x41)
	{
		printf("enter  Read Address!\r\n");
		pos += 2;	
		uint8_t *pAdd = (uint8_t*)&nAddress;
		printf("Adrress:%02X, %02X, %02X\r\n", buf[pos], buf[pos+1], buf[pos+2]);
		while (buf[pos]!=',')
		{
			nAddress = nAddress<<8;
			pAdd[0] = ReadAByteAsciiHex(buf, pos);
			printf("%02X ", pAdd[0]);
		}
		pos++;
		printf("leave  Read Address!\r\n");
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
		printf("nByteLen is ODD!\n");
	}
	
	nLength = nByteLen/2;
	
	printf("Read A Section!\r\n");
	
}

/**********************************************************************
ReadAByteAsciiHex-----ByteAscii---Hex
Parameters:
Return Value:
**********************************************************************/
uint8_t CanFlashupdate::ReadAByteAsciiHex(uint8_t *buf, uint64_t& pos)
{
	uint8_t nData;
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

/**********************************************************************
parseFlashAddress-----获得标志位值，并将数据从标志位分开
Parameters:
Return Value:
**********************************************************************/
void CanFlashupdate::parseFlashAddress(uint16_t & wSectionLen, uint & uSectionAddress, uint8_t* ucSectionBuff,
												 uint16_t & u16FlagValue, bool bFlashKernel, 
												 bool &bSectSendCplt, uint8_t * &pSaveData, uint16_t &SaveLen)
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
			u16FlagValue = *((uint16_t*)ucSectionBuff);
			wSectionLen = 0;
		}
		else if (uSectionAddress+wSectionLen>0X3f4000)
		{
			u16FlagValue = *((uint16_t*)(ucSectionBuff+2*(0x3f3fff-uSectionAddress)));
			wSectionLen = 0x3f3fff-uSectionAddress; //超过标志位的扔掉
		}
	}
	else
	{
		if (uSectionAddress>=0x3f4000)
		{}
		else if (uSectionAddress==0x3f3fff)	//恰好是标志位
		{
			u16FlagValue = *((uint16_t*)ucSectionBuff);
			
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
			u16FlagValue = *((uint16_t*)(ucSectionBuff+2*(0x3f3fff-uSectionAddress)));
			wSectionLen--;
		}
		else	//大于标志位
		{
			u16FlagValue = *((uint16_t*)(ucSectionBuff+2*(0x3f3fff-uSectionAddress)));
			
			//置本次SECTION未处理完标志
			bSectSendCplt = FALSE;
			
			//未传完的数据大小
			SaveLen = uSectionAddress+wSectionLen-0x3f4000;
			pSaveData = new uint8_t[SaveLen*2];
			memcpy(pSaveData, ucSectionBuff+2*(0x3f4000-uSectionAddress), SaveLen*2);
			
			wSectionLen = 0x3f3fff-uSectionAddress; //将长度截止到标志位前
		}
	}
}

/**********************************************************************
FlashupdateThread-----升级线程
Parameters: pCanRecvMsg:由协议层上传的消息内容
Return Value:
**********************************************************************/
void CanFlashupdate::FlashupdateThread()
{
    while(1)
    {
        if (m_bRsExit)
        {
            qDebug() << "升级结束，线程退出";
            return;
        }
        FlashUpdateRoutine();

        if (((FlashUpdateThreadConv==3)||(FlashUpdateThreadConv==4))
                && gHostModuleItc.u16FlashupdateStatus == STATUS_FLASH_UPDATE_INVALID)
            break;
        QThread::msleep(50);
    }

    FlashUpdateThreadConv = 0;
}

/**********************************************************************
ReadUpdataFile-----文件路径接收到该线程处理
Parameters: pCanRecvMsg:由协议层上传的消息内容
Return Value:
**********************************************************************/
void CanFlashupdate::GetUpdataFile(QString path)
{
    if(path == NULL)
    {
        qDebug() << "文件路径为空：";
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QByteArray arr = file.readAll();
    memcpy(cFlashUpdateBuf, arr.data(), arr.size());

    m_pHostModuleItc->m_TotalFlashSteps = GetFlashTotalStep();
    emit e_UpProgressSet(m_pHostModuleItc->m_TotalFlashSteps);

    arr.clear();
    file.close();
}

/**********************************************************************
VCI_To_APDU-----消息接收底层转运用层
Parameters: pCanRecvMsg:由协议层上传的消息内容
Return Value:
**********************************************************************/
int32_t CanFlashupdate::Msg_RxSerial(CAN_OBJ pCanObj)
{
    CAN_APDU_T apdu;
    //数据拆分解析
    VCI_To_APDU(&apdu, &pCanObj);
    Msg_Recv(&apdu);

    return 0;
}

/**********************************************************************
Msg_TxSerial-----消息转发至canbox
Parameters: pCanRecvMsg:由协议层上传的消息内容
Return Value:
**********************************************************************/
int32_t CanFlashupdate::Msg_TxSerial(CAN_APDU_T *pMsg)
{
    CAN_OBJ vciMsg;
    APDU_To_VCI(&vciMsg, pMsg);
    emit e_MsgToSerial(vciMsg);

    return 0;
}

/**********************************************************************
GetFlashTotalStep-----获取升级进度
Parameters:
Return Value:
**********************************************************************/
int CanFlashupdate::GetFlashTotalStep()
{
    uint64_t pos = 0;
    uint16_t nLength = 0;
    uint nAddress;
    uint8_t ByteArary[500*1024];
    uint8_t *ucSection = ByteArary;
    bool bFlashKernel = (m_pHostModuleItc->u16FlashUpdateKernelFlag==0x95);
    uint16_t flagValue;
    bool bSectSendCplt = TRUE;
    uint8_t * pSaveData;
    uint16_t SaveLen;
    bool bReadEnd = FALSE;
    bool bSendFlag = FALSE;

    int index = 1;	//Section 序号
    int nTotalStep = 0;

    //第一次直接进入
    while (nLength || (!bSendFlag) )
    {
L1:
    if (!bSectSendCplt)
    {
        bSectSendCplt = TRUE;
        nAddress = 0x3f4000;
        nLength = SaveLen;
        memcpy(ucSection, pSaveData, nLength*2);
        delete[] pSaveData;
    }
    else if (!bReadEnd)
    {
        CanFlashupdate::ReadASectionAsciiHex(cFlashUpdateBuf, pos, nLength , nAddress,  ucSection);

        if (nLength>0)
        {
            CanFlashupdate::parseFlashAddress(nLength, nAddress, ucSection,
                flagValue, bFlashKernel,
                bSectSendCplt, pSaveData, SaveLen);
            if (nLength==0)
                goto L1;
        }
        else
        {
            bReadEnd = TRUE;
        }
    }
    else
    {
        if (!bSendFlag)	//传输标志位
        {
            bSendFlag = TRUE;
            nLength = 1;
            nAddress = 0x3f3fff;
        }
        else	//传输结束帧
        {
            nLength = 0;
            nAddress = 0;
        }
    }

    qDebug("%d	nLen=%02X nAddress=%04X", index, nLength, nAddress);
    qDebug("Data: %02X %02X %02X %02X %02X %02X",
           ucSection[0], ucSection[1], ucSection[2], ucSection[3], ucSection[4], ucSection[5]);
    qDebug("EndData:  %02X %02X %02X %02X %02X %02X", ucSection[nLength*2-1],
            ucSection[nLength*2-2], ucSection[nLength*2-3], ucSection[nLength*2-4],
            ucSection[nLength*2-5], ucSection[nLength*2-6]);

    nTotalStep += nLength/1024;
    if (nLength%1024)
        nTotalStep++;

    index++;
    }

    //文件结束帧算一步
    nTotalStep++;

    qDebug("flagValue=0x%04X", flagValue);
    qDebug("Index=%d", index);
    qDebug("nTotalStep=%d", nTotalStep);

    return nTotalStep;
}




