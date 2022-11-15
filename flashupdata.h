#ifndef FLASHUPDATA_H
#define FLASHUPDATA_H

#include <QObject>
#include "types.h"
#include "canmessagedeal.h"

//-----------------------------------------------------------------------------
//状态信息宏定义
//握手,下发
#define HAND_COMM_QUERY 0x62
//握手上传
#define HAND_OK_RESPOND 0xaa

//解密成功
#define CHIP_DECODE_SUCCESS 0x00
#define CHIP_DECODE_FAIL	   10

//API版本匹配问题
#define API_VESION_OK	0x6c
#define API_VERSION_FAIL 14




//扇区擦除相关
//扇区选择
#define ERASE_SECTOR_ALL 6
#define ERASE_SECTOR_A 1
#define ERASE_SECTOR_B 2
#define ERASE_SECTOR_C 3
#define ERASE_SECTOR_D 4
#define ERASE_SECTOR_BCD 5

//扇区擦除返回码定义
//擦除成功
#define ERASE_SUCCESFULL 0
//未指定扇区
#define ERASE_NO_SPECIFIED_SECTOR 20
//清0失败
#define ERASE_PRECONDITION_FAIL 21
//扇区擦除失败
#define ERASE_FAIL 22
//紧致失败
#define ERASE_COMPACT_FAIL 23
//预紧致失败
#define ERASE_PRECOMPACT_FAIL 24

//PROGRAM
//编程允许
#define PROGRAM_ENABLE 0x6e
//因扇区未擦除不允许编程
#define PROGRAM_DIABLE 0x70

//VERIFY
//校验成功
#define VERIFY_OK	0
//校验失败
#define VERIFY_FAIL 10

//数据传输
//BLOCK传输头正确
#define BLOCK_HEAD_OK	0
//BLOCK头正确接收并文件传输结束
#define FILE_TRANS_END 5
//BLOCK SIZE >1024
#define BLOCK_SIZE_OVERLOW 10
//BLOCK ADD OVERLOW
#define BLOCK_ADD_OVERLOW 20
//BLOCK head and add are overlow
#define BLOCK_SEZE_ADD_OVERLOW	30

//one block check sum
#define CHECK_SUM_SUCCESFUL	0x82
#define CHECK_SUM_FAIL			0x83


//dsp 反馈接收到数据
#define BLOCK_RECV_DATA_OK 0

//Macro definition
//sevice code denfine
//握手
#define HANDS_COMM_SRVCODE 0x01
//芯片解密
#define CHIP_DECODE_SRVCODE 0x02
//API版本信息
#define API_VERSION_SRVCODE 0x03
//擦除扇区
#define ERASE_SECTOR_SRVCODE 0x04
//编程开始指令
#define PROGRAM_SRVCODE 0x05
//传输BLOCK文件头信息
#define BLOCK_HEAD_SRVCODE 0x06
//相应BLOCK传输有效数据
#define BLOCK_DATA_SRVCODE 0x07
//传输相应BLOCK校验和--累加和
#define BLOCK_CHECKSUM_SRVCODE 0x08
//传输对应BLOCK的program状态
#define BLOCK_PROMG_STATUS_SRVCODE 0x09
//FLASH 校验
#define VERIFY_SRVCODE 0x0A
//FLASH 调试
#define DEBUG_SRVCODE 0x3D

//升级进度标志相关宏定义
//进度复位状态
#define	PROGRESS_IN_RESET		0xff
//升级开始
#define	PROGRESS_IN_START		0
//握手OK
#define PROGRESS_IN_HAND_OK		1
//解密OK
#define	PROGRESS_IN_DECODE_OK	2
//API版本OK
#define	PROGRESS_IN_APIVERSION_OK	3
//擦除扇区OK
#define	PROGRESS_IN_ERASE_OK		4
//编程允许OK
#define	PROGRESS_IN_PROG_ENA_OK	5
//BLOCK头信息应答OK
#define	PROGRESS_IN_HEADRESPOND_OK	6
//BLOCK校验和应答OK
#define	PROGRESS_IN_BLOCKCHECK_OK	7
//FLASH 编程OK
#define	PROGRESS_IN_FLASH_PROG_OK	8
//FLASH 编程校验OK

//目标升级使能与否
#define TARGET_UPDATE_ENABLE 1
#define TARGET_UPDATE_DISABLE 0

//升级过程中本次任务处理完成与否
#define TASK_HANDLED 1
#define TASK_HANDLE_NON 0


//握手状态
enum CAN_MSG_HANGLE_ERR_ENUM
{
    CAN_MSG_HANDLE_OK = 0,
    CAN_MSG_HANDLE_INVALID_MAC_ID,
    CAN_MSG_HANDLE_INVALID_BATT_GRP,
    CAN_MSG_HANDLE_INVALID_OBJ,
    CAN_MSG_HANDLE_INVALID_MSG_CLASS,
    CAN_MSG_HANDLE_INVALID_SRVC_COD,

    CAN_MSG_HANDLE_EOL
};

enum
{
    //
    FLASH_ERROR_NONE=0,
    //握手
    HANDS_COMM_ERROR,
    //芯片解密
    CHIP_DECODE_ERROR,
    //API版本信息
    API_VERSION_ERROR,
    //擦除扇区
    ERASE_SECTOR_ERROR,
    //编程
    PROGRAM_ERROR,
    //block head
    BLOCK_HEAD_ERROR,
    //block data
    BLOCK_DATA_ERROR,
    //BLOCK累加和
    BLOCK_CHECKSUM_ERROR,
    //BLOCK的编程状态
    BLOCK_PROMG_STATUS_ERROR,
    //verify
    VERIFY_ERROR,
};


class FlashUpData : public QObject
{
    Q_OBJECT
public:
    explicit FlashUpData(QObject *parent = nullptr);

public:

    int HandCommXmitFcb(void);
    int HandCommRecvFcb(void);

    int ChipDecodeXmitFcb(void);
    int ChipDecodeRecvFcb(void);

    int ApiVersionXmitFcb(void);
    int ApiVersionRecvFcb(void);

    int EraseSectorXmitFcb(void);
    int EraseSectorRecvFcb(void);

    int ProgramXmitFcb(void);
    int ProgramRecvFcb(void);

    int BlockHeadXmitFcb(void);
    int BlockHeadRecvFcb(void);

    int BlockDataXmitFcb(void);
    int BlockDataRecvFcb(void);

    int BlockChecksumXmitFcb(void);
    int BlockChecksumRecvFcb(void);

    int BlockProgStatusXmitFcb(void);
    int BlockProgStatusRecvFcb(void);

    int VerifyXmitFcb(void);
    int VerifyRecvFcb(void);

    UCHAR FlashupdateTaskHandle(UCHAR ucRecvAddr);
    void FlashupdateTaskReset(void);

    void AppMsgRecv(CAN_APDU_T *pCanRecvMsg);
    void AppMsgXmit(CAN_XMIT_QUEUE_MSG_T * pCanXmitMsg);

    BYTE ReadAByteAsciiHex(BYTE *buf, DWORD& pos);
    void ReadASectionAsciiHex(BYTE *buf, DWORD& pos, WORD& nLength, UINT& nAddress, BYTE *sectionbuff);
    void parseFlashAddress(WORD & wSectionLen, UINT & uSectionAddress, BYTE* ucSectionBuff,
                           UINT16 & u16FlagValue, bool bFlashKernel,
                           bool &bSectSendCplt, BYTE * &pSaveData, UINT16 &SaveLen);
    void FlashUpdateRoutine(void);

public:
    CAN_XMIT_QUEUE_MSG_T m_XmitQueueMsg;
    UCHAR m_ucXmitMsgBuf[8];

    CAN_APDU_T m_XmitMsg;
    CAN_APDU_T m_RecvMsg;

signals:

private:
    _HOST_MODULE_ITC_T *m_pHostModuleItc;

    //待升级目标变量
    //=0,表示升级旁路，=1~32升级模块
    _FLASHUPDATE_TARGET_T m_tFlashupdateTarged[INV_MODULE_MAX_CNT];

    uint16_t m_u16SendBlocks;
    bool m_bReadEnd;
    bool m_bSectSendCplt;	//默认值为一次性传完
    bool m_bSendFlag;	//未传输标志位

    UINT m_uSectionAddress; //对应SECTION地址
    WORD m_wSectionLen; //对应SECTION的长度

    BYTE m_ucSectionBuff[1024*300];
    BYTE *m_pSaveData;
    uint16_t m_SaveLen; //保存长度

    uint16_t m_u16FlagValue; //标志位值
    unsigned long m_ulPos;//待传输的数据在BLOCK中的地址

    //文件传输扇区统计量
    uint16_t m_u16BlockNumOfSection;    //每一个SECTION 分成N个BLOCK
    uint16_t m_u16CurrentBlockNum;  //N个BLOCK中当前BLOCK号
    uint16_t m_u16RemainWordOfSection;  //当前SECTION中不足1024字(一个BLOCK),剩余字数
    uint16_t m_u16FrameNumOfRemainBlock;    //剩余BLOCK(不足1024字)需用的CAN帧数
    uint16_t m_u16LastFrameWordNumOfRemainBlock;    //剩余BLOCK(不足1024字)所有帧中最后一帧的字的个数

    uint16_t m_u16ReadHexFileEnd;     //读取HEX文件结束标志,其值=0x6789 表示结束

    UINT m_uBlockAddress; //对应BLOCK地址

    int m_ModErrorCnt[INV_MODULE_MAX_CNT];  //模块错误计数器
    int m_ModErrorType; //模块错误类型

    uint16_t m_u16CurrentFrameNum;  //待传输的帧号
    uint16_t m_u16ProgramPorcess; //升级过程进度标志,防止重入导致状态机错乱

    uint16_t m_u32RespondModuleFlag;  //根据后台指定的升级模块看是否有握手应答信号

    WORD m_wBlockLen;   //对应BLOCK的长度
    UINT16 m_u16ResendCnt;  //BLOCK重发次数


};

#endif // FLASHUPDATA_H
