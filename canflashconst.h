#ifndef CANFLASHCONST_H
#define CANFLASHCONST_H

#include <windows.h>
#include "lib/ECanVci.h"
#include "main.h"

//Flash Update 缓冲区，大小为512k
#define FLASH_UPDATE_FILE_BUFF_SIZE			0x80000

//读取HEX文件结束标志定义
#define READ_HEX_END_FLAG_VALID		0x6789
#define READ_HEX_END_FLAG_RESET 		0

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

//模块最大数量
#define INV_MODULE_MAX_CNT 32
#define REC_MODULE_MAX_CNT 3

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

//-------------------------------------
//升级进度标志相关宏定义
//进度复位状态
#define	PROGRESS_IN_RESET		0xff
//升级开始
#define	PROGRESS_IN_START		0
//握手OK
#define 	PROGRESS_IN_HAND_OK		1
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
#define	PROGRESS_IN_FLASH_VERIFY_OK	9

//------------------------------------
#define FRAG_MSG 1
#define NONFRAG_MSG 0

#define RS_MSG 1
#define RQ_MSG 0

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

//编程
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

//flash update
#define MSG_FLASH_UPDATE	0x02

#define SRV_FLASH_UPDATE_REC 0x20
#define SRV_FLASH_UPDATE_INV 0x21
#define SRV_FLASH_UPDATE_BYP 0x22
#define SRV_FLASH_UPDATE_CHG 0x23

//flash update object
#define OBJ_FLASH_UPDATE_REC 1
#define OBJ_FLASH_UPDATE_INV 2
#define OBJ_FLASH_UPDATE_BYP 3
#define OBJ_FLASH_UPDATE_MON 4
#define OBJ_FLASH_UPDATE_CHG 8
#define OBJ_FLASH_UPDATE_END 0

//需升级kernel标志
#define FLASH_UPDATE_KERNEL 0x95
//
#define ADD_BROADCAST 0x3f
//初始状态
#define	STATUS_HOST_UPDATE_INI  0
//尚未开始升级
#define	STATUS_HOST_HAVENOT_START 1
//正在升级中
#define	STATUS_HOST_UPDAT_ING 2
//升级成功
#define	STATUS_HOST_UPDATE_SUCCESFULL 3
//升级失败
#define	STATUS_HOST_UPDATE_FAIL 4
//指定升级节点不在线
#define	STATUS_UPDATE_NODE_OFF_LINE 5

//升级大对象---
//REC
#define FLASHUPDATE_OBJECT_REC 1
//INV
#define FLASHUPDATE_OBJECT_INV 2
//BYP
#define FLASHUPDATE_OBJECT_BYP 3
//CHG
#define FLASHUPDATE_OBJECT_CHG 8

//-----20100118:增加处理共同升级整流、逆变的处理变量类型定义
//目标升级使能与否
#define TARGET_UPDATE_ENABLE 1
#define TARGET_UPDATE_DISABLE 0

//升级过程中本次任务处理完成与否
#define TASK_HANDLED 1
#define TASK_HANDLE_NON 0

//各个定时器重发次数定义
#define RESEND_WAITING_MAX_CNT      15
#define RESEND_WAITING_START_CNT    1
#define RESEND_WAITING_RESET_CNT	0
#define TIMER_CNT_LEN 9

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

//应用层接口数据格式
typedef struct
{
    uint8_t ucMsgClass;
    uint8_t ucServiceCode;
    uint8_t ucRsRq;
    uint8_t ucFrag;
    uint8_t ucSourceId;
    uint8_t ucDestinationId;
    uint16_t u16Length;			//有效数据长度
    uint8_t *pData;
}CAN_APDU_T;

typedef struct
{
    uint8_t ucMsgClass;
    uint8_t ucServiceCode;
    uint8_t ucRsRq;
    uint8_t ucFrag;
    uint8_t ucSourceId;
    uint16_t u16DestinationId;//byte1: Battery group index; byte0:CAN MAC ID
}CAN_XMIT_QUEUE_MSG_T;

//协议层状态
enum CAN_PROTOCOL_ST
{
    CAN_PROTOCOL_OK = 0,
    CAN_PROTOCOL_NO_MSG,
    CAN_PROTOCOL_NO_TIMER,
    CAN_PROTOCOL_PORT_ERR,
    CAN_PROTOCOL_INVALID_RQ_FCB,
    CAN_PROTOCOL_INVALID_FRAG_XMIT_FCB,
    CAN_PROTOCOL_INVALID_FRAG_RECV_FCB,

    CAN_PROTOCOL_EOL
};

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
    //后台通信dataflag
    uint16_t usHostCommDataFlag;
    //需升级的模块号
    uint16_t u16ModIdx;
    //需升级的模块位
    int m_FlashModIdx;	//bit0~bit31,bit位为1时代表需要升级
    //响应升级命令的模块
    int m_FlashRsModIdx;
    //故障数据更新完成标志
    _FAULT_DATA_FLAG usFaultDataFlag;
    //.....
    uint16_t uint1sFlashUpdateFlag;
    //需要升级kernel的标志
    uint16_t u16FlashUpdateKernelFlag;
    //Flash update 状态机
    _FLASHUPDATE_STATUS u16FlashupdateStatus;
    //在线升级当前状态
    uint32_t u16UpdateStatus[INV_MODULE_MAX_CNT];
    //在线升级当前标志
    uint16_t sFlashUpdateFlag;
    //在线升级当前进度
    uint16_t m_curFlashProgress;
    //在线升级总步骤
    uint16_t m_TotalFlashSteps;
    //配置信息查询标志
    uint16_t m_ReadConfigFlag;	//0 等待查询; 1 查询成功; 2 查询失败
    //版本信息读取标志
    uint16_t m_ReadVerFlag;	//0 等待查询; 1 查询成功; 2 查询失败

}_HOST_MODULE_ITC_T;

typedef struct
{
    uint8_t ucMsgClass;
    uint8_t ucSrvcCode;
    uint8_t ucDestinationID; //3// 广播:3f ; 模块号:1~10;
}_HOST_MODULE_QUEUE_T;

typedef struct
{
    uint8_t ucServiceCode;
    uint nIDEvent;
    uint uElapse;
}PERIODIC_FLASH_UPDATE_CMD_T;

typedef struct
{
    //升级的目标地址
    uint8_t ucTargeAddr;
    //此目标地址升级使能=1使能
    uint8_t ucTargetEnable;
    //升级过程中本次任务处理完毕=1处理完毕
    uint8_t ucTaskHandled;
}_FLASHUPDATE_TARGET_T;

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

enum
{
    FLASH_TIMER_BASE = 2,
    //等待握手应答信号
    HANDS_COMM_TIMER = 2,
    //等待芯片解密应答信号
    CHIP_DECODE_TIMER,
    //等待API版本信息应答
    API_VERSION_TIMER,
    //等待擦除信号10S
    ERASE_SECTOR_TIMER,
    //等待编程允许信号
    PROGRAM_TIMER,
    //等待头信息应答信号
    BLOCK_HEAD_TIMER,
    //等待BLOCK校验和应答信息
    BLOCK_CHECKSUM_TIMER,
    //等待FLASH编程状态信息
    BLOCK_PROMG_STATUS_TIMER,
    //等待FLASH编程校验应答信息
    VERIFY_TIMER,
    //
    FLASH_MAX_TIMER,
};



#endif // CANFLASHCONST_H


