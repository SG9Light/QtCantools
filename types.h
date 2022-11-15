#ifndef TYPES_H
#define TYPES_H

#include "lib/ECanVci.h"

//Flash Update 缓冲区，大小为512k
#define FLASH_UPDATE_FILE_BUFF_SIZE			0x80000
#define INV_MODULE_MAX_CNT 32

#define FRAG_MSG 1
#define NONFRAG_MSG 0

#define RS_MSG 1
#define RQ_MSG 0

// CAN ID allocated
enum _CAN_MAC_ID_ENUM
{
    MAC_ID_RSVDX00 = 0,
    MAC_ID_MON = 0x01,
    MAC_ID_MOD1_REC,
    MAC_ID_MOD2_REC,
    MAC_ID_MOD3_REC,
    MAC_ID_CHG,
    MAC_ID_STS,
    MAC_ID_RSVDX07,
    MAC_ID_RSVDX08,
    MAC_ID_RSVDX09,
    MAC_ID_RSVDX0A,
    MAC_ID_RSVDX0B,
    MAC_ID_RSVDX0C,
    MAC_ID_RSVDX0D,
    MAC_ID_RSVDX0E,
    MAC_ID_RSVDX0F,
    MAC_ID_MOD1_INV = 0x10,
    MAC_ID_MOD2_INV = 0x11,
    MAC_ID_MOD3_INV = 0x12,
    MAC_ID_MOD4_INV = 0x13,
    MAC_ID_MOD5_INV = 0x14,
    MAC_ID_MOD6_INV = 0x15,
    MAC_ID_MOD7_INV = 0x16,
    MAC_ID_MOD8_INV = 0x17,
    MAC_ID_MOD9_INV = 0x18,
    MAC_ID_MOD10_INV = 0x19,
    MAC_ID_MOD11_INV = 0x1A,
    MAC_ID_MOD12_INV = 0x1B,
    MAC_ID_MOD13_INV = 0x1C,
    MAC_ID_MOD14_INV = 0x1D,
    MAC_ID_MOD15_INV = 0x1E,
    MAC_ID_MOD16_INV = 0x1F,
    MAC_ID_MOD17_INV = 0x20,
    MAC_ID_MOD18_INV = 0x21,
    MAC_ID_MOD19_INV = 0x22,
    MAC_ID_MOD20_INV = 0x23,
    MAC_ID_MOD21_INV = 0x24,
    MAC_ID_MOD22_INV = 0x25,
    MAC_ID_MOD23_INV = 0x26,
    MAC_ID_MOD24_INV = 0x27,
    MAC_ID_MOD25_INV = 0x28,
    MAC_ID_MOD26_INV = 0x29,
    MAC_ID_MOD27_INV = 0x2A,
    MAC_ID_MOD28_INV = 0x2B,
    MAC_ID_MOD29_INV = 0x2C,
    MAC_ID_MOD30_INV = 0x2D,
    MAC_ID_MOD31_INV = 0x2E,
    MAC_ID_MOD32_INV = 0x2F,
    MAC_ID_EOL
};

//全局变量声明
extern int  IsOpenFlag; //设备启动标志
extern long  mRecCnt;   //设备接收消息计数器

//Flash Update 缓冲区
extern BYTE cFlashUpdateBuf[FLASH_UPDATE_FILE_BUFF_SIZE];

enum
{
    CanBoxClose,
    CanDeviceFail,
    CanInitFail,
    CanStar1Fail,
    CanStar2Fail,
    CanOpenOk
};

//应用层接口数据格式
typedef struct
{
    UCHAR ucMsgClass;
    UCHAR ucServiceCode;
    UCHAR ucRsRq;
    UCHAR ucFrag;
    UCHAR ucSourceId;
    UCHAR ucDestinationId;
    UINT16 u16Length;			//有效数据长度
    UCHAR *pData;
}CAN_APDU_T;

typedef struct
{
    UCHAR ucMsgClass;
    UCHAR ucServiceCode;
    UCHAR ucRsRq;
    UCHAR ucFrag;
    UCHAR ucSourceId;
    UINT16 u16DestinationId;//byte1: Battery group index; byte0:CAN MAC ID
}CAN_XMIT_QUEUE_MSG_T;

//故障交互数据类型
typedef enum
{
    FAULT_DATA_GET, //后台通信至
    FAULT_DATA_PROCING,
    FAULT_DATA_READY //模块通信完成后至
}_FAULT_DATA_FLAG;

typedef enum
{
    //初始无效状态
    STATUS_FLASH_UPDATE_INVALID=0,
    //开始选择节点升级
    STATUS_FLASH_START,
    //对指定节点开始升级过程
    STATUS_SELCET_NODE,

    //等待握手信号状态
    STATUS_WAITING_HANDS_RESPOND ,
    //等待芯片解密应答信号
    STATUS_WAITING_CHIP_DECODE ,
    //等待API版本确认信息
    STATUS_WAITING_API_VERSION ,
    //API ok
    STATUS_API_OK,

    //擦除中状态
    STATUS_FLASH_ERASE_GOING,
    //擦除结束
    STATUS_FLASH_ERASED,
    //等待编程允许状态
    STATUS_PROGRAM_PERMIT_WAITING,
    //编程允许
    STATUS_PROGRAM_ENABLE,
    //传输BLOCK头
    STATUS_BLOCK_HEAD_WAITING,
    //传输BLOCK头结束
    STATUS_BLOCK_HEAD_OK,

    //传输下一个BLOCK
    STATUS_NEXT_BLOCK_HEAD,

    //等待允许传输BLOCK数据
    STATUS_BLOCK_DATATRANS_WAITING,
    //BLOCK数据传输结束
    STATUS_BLOCK_DATATRANS_END,
    //等待BLOCK校验和应答
    STATUS_BLOCK_CHECKSUM_WAITING,
    //BLOCK校验OK
    STATUS_BLOCK_CHECKSUM_OK,
    //编程状态等待
    STATUS_BLOCK_PROGRAM_WAITING,
    //BLOCK编程完成
    STATUS_BLOCK_PROGRAM_COMPLETE,
    //文件传输完成
    STATUS_FILE_TRANS_END,
    //FLASH 校验中
    STATUS_FLASH_VERIFYING,
    //FLASH 校验完毕
    STATUS_FLASH_VERIFY,
    //FLASH UPDATE 完成
    STATUS_FLASH_UPDATE_OVER,

    //FLASH调试状态
    STATUS_FLASH_DEBUG=0x3d

}_FLASHUPDATE_STATUS;

typedef struct
{
    //后台通信发给模块通信的通信任务的队列对象指针
    //	TemplateQueue<_HOST_MODULE_QUEUE_T> *pTaskQueueFromHostToModule;

    //后台通信dataflag
    UINT16	usHostCommDataFlag;

    //需升级的模块号
    UINT16 u16ModIdx;

    //需升级的模块位
    int m_FlashModIdx;	//bit0~bit31,bit位为1时代表需要升级

    //响应升级命令的模块
    int m_FlashRsModIdx;

    //需升级的模块号数组
    //	vector <UINT16> m_vFlashModID;

    //故障数据更新完成标志
    _FAULT_DATA_FLAG usFaultDataFlag;

    //.....
    UINT16 uint1sFlashUpdateFlag;

    //需要升级kernel的标志
    UINT16 u16FlashUpdateKernelFlag;

    //Flash update 状态机
    _FLASHUPDATE_STATUS u16FlashupdateStatus;

    UINT32 u16UpdateStatus[INV_MODULE_MAX_CNT];

    UINT16 sFlashUpdateFlag;

    //在线升级当前进度
    UINT16 m_curFlashProgress;

    //在线升级总步骤
    UINT16 m_TotalFlashSteps;

    //在线升级命令超时Timer
    //	TimerObserver *m_pFlashCmdTimer;

    //配置信息查询标志
    UINT16 m_ReadConfigFlag;	//0 等待查询; 1 查询成功; 2 查询失败

    //版本信息读取标志
    UINT16 m_ReadVerFlag;	//0 等待查询; 1 查询成功; 2 查询失败

}_HOST_MODULE_ITC_T;

#endif // TYPES_H
