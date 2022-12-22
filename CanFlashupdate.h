#ifndef CANFLASHUPDATA_H
#define CANFLASHUPDATA_H

#include "canflashconst.h"

class CanFlashupdate :public QObject
{
    Q_OBJECT

public:
    CanFlashupdate(QObject *parent = nullptr);
    virtual ~CanFlashupdate();

    void CanFlashInit(void);

    int HandCommXmitFcb(void);
    int HandCommRecvFcb(void);

    int Msg_Recv(CAN_APDU_T *pCanRecvMsg);
    int Msg_Xmit(CAN_XMIT_QUEUE_MSG_T * pCanXmitMsg);

    void FlashUpdateRoutine(void);

    CAN_XMIT_QUEUE_MSG_T m_XmitQueueMsg;
    uint8_t m_ucXmitMsgBuf[8];

    CAN_APDU_T m_XmitMsg;
    CAN_APDU_T m_RecvMsg;

    void WaitHandleTimerPost(void);

    int NewPeriodicWaitMsgGen(uint nIDEvent);

    void ResendOneBlock(void);

    void SetRespondModuleFlag(uint8_t ucRecvAddr);

    int ChipDecodeXmitFcb(void);
    int ChipDecodeRecvFcb(void);

    uint8_t FlashupdateTaskHandle(uint8_t ucRecvAddr);

    int ApiVersionXmitFcb(void);
    int ApiVersionRecvFcb(void);

    void FlashupdateTaskReset(void);

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

    uint32_t CheckSum(uint16_t u16Length, uint32_t nAddress, uint8_t *buf);

    static void ReadASectionAsciiHex(uint8_t *buf, uint64_t& pos, uint16_t& nLength, uint& nAddress, uint8_t *sectionbuff);
    static uint8_t ReadAByteAsciiHex(uint8_t *buf, uint64_t& pos);
    static void parseFlashAddress(uint16_t & wSectionLen, uint & uSectionAddress, uint8_t* ucSectionBuff,
                                  uint16_t & u16FlagValue, bool bFlashKernel,
                                  bool &bSectSendCplt, uint8_t * &pSaveData, uint16_t &SaveLen);

    void FlashupdateThread(void);
    void GetUpdataFile(QString path);
    int32_t Msg_RxSerial(CAN_OBJ pCanObj);
    int32_t Msg_TxSerial(CAN_APDU_T *pMsg);
    int GetFlashTotalStep(void);

signals:
    void e_MsgToSerial(CAN_OBJ pCanObj);
    void e_UpProgressSet(int n);
    void e_UpProgressDisp(int n);

    void e_BlockProgressSet(int n);
    void e_BlockProgressDisp(int n);

private:
    QThread *objCanFlashTask = new QThread;
    //升级退出标志
    uint8_t m_bRsExit;
    //0:未启动升级; 1:启动升级; 2:正在升级; 3:升级完成 4:升级失败;
    uint8_t FlashUpdateThreadConv;
    //Flash Update 缓冲区
    uint8_t cFlashUpdateBuf[FLASH_UPDATE_FILE_BUFF_SIZE];
    uint8_t m_ucMsgClass;

    //根据后台指定的升级模块看是否有握手应答信号
    uint32_t	m_u32RespondModuleFlag;

    //升级过程进度标志,防止重入导致状态机错乱
    uint16_t	m_u16ProgramPorcess;

    //读取HEX文件结束标志
    //其值=0x6789 表示结束
    uint16_t 	m_u16ReadHexFileEnd;

    _HOST_MODULE_ITC_T *m_pHostModuleItc;

    //当前传输Block数
    uint16_t m_u16SendBlocks;

    //标志位值
    uint16_t m_u16FlagValue;
    //是否传输了标示位
    uint16_t m_bSendFlag;

    //读到文件结束标志
    bool m_bReadEnd;

    //本次SECTION未传完标志
    bool m_bSectSendCplt;
    uint8_t *m_pSaveData;
    uint16_t m_SaveLen;

    //各个定时器超时次数
    uint16_t m_u16TimerExpiredCnt[9];

    //CheckSumm error FLAG
    uint16_t m_u16CheckSumError;

    //BLOCK重发次数
    uint16_t m_u16ResendCnt;

    //待传输的帧号
    uint16_t m_u16CurrentFrameNum;

    //待升级目标变量
    //=0,表示升级旁路，=1~32升级模块
    _FLASHUPDATE_TARGET_T m_tFlashupdateTarged[INV_MODULE_MAX_CNT];

    //BLOCK文件校验和
    uint16_t m_u16BlockChecksum;

    //文件传输相关
    //每一个SECTION 分成N个BLOCK
    uint16_t	m_u16BlockNumOfSection;
    //N个BLOCK中当前BLOCK号
    uint16_t	m_u16CurrentBlockNum;
    //当前SECTION中不足1024字(一个BLOCK),剩余字数
    uint16_t 	m_u16RemainWordOfSection;
    //剩余BLOCK(不足1024字)需用的CAN帧数
    uint16_t	m_u16FrameNumOfRemainBlock;
    //剩余BLOCK(不足1024字)所有帧中最后一帧的字的个数
    uint16_t	m_u16LastFrameWordNumOfRemainBlock;
    //对应SECTION的长度
    uint16_t m_wSectionLen;
    //对应SECTION地址
    uint m_uSectionAddress;

    //总的帧数-1
    uint16_t m_u16FrameNumOneBlock;
    //最后一帧的字的个数
    uint16_t m_u16LastFrameWordNumOneBlock;
    //待传输的BLOCK号
    uint16_t m_u16BlockNum;

    //本次BLOCK的校验和
    uint32_t m_u32CheckSumOneBlock;

    //待传输的数据在BLOCK中的地址
    uint64_t m_ulPos;

    uint8_t m_ucSectionBuff[1024*300];
    //对应BLOCK地址
    uint m_uBlockAddress;

    //对应BLOCK的长度
    uint16_t m_wBlockLen;

    int m_ModErrorCnt[INV_MODULE_MAX_CNT];
    int m_ModErrorType;

    void SetFlashFailTip(int i);

};

extern void VCI_To_APDU(CAN_APDU_T *pApdu, CAN_OBJ *pVci);
extern void APDU_To_VCI( CAN_OBJ *pVci, CAN_APDU_T *pApdu);
extern _HOST_MODULE_ITC_T gHostModuleItc;

#endif // CANFLASHUPDATA_H
