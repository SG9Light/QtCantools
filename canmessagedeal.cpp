#include "canmessagedeal.h"
#include <QDebug>

/************************************************************************************
函数名称:	CanMessageDeal
功能描述: 构造函数
参数：
***********************************************************************************/
CanMessageDeal::CanMessageDeal()
{

}

/************************************************************************************
函数名称:	CloseCanBox
功能描述: 设备关闭函数
参数：
***********************************************************************************/
void CanMessageDeal::CloseCanBox()
{
    if(IsOpenFlag == CanOpenOk)
    {
        IsOpenFlag = CanBoxClose;
        CloseDevice(USBCAN2,DeviceInd0);
    }
}

/************************************************************************************
函数名称:	OpenCanBox
功能描述: 设备打开函数
参数：
***********************************************************************************/
void CanMessageDeal::OpenCanBox()
{
    if(!OpenDevice(USBCAN2,DeviceInd0, CANInd0))
    {
        IsOpenFlag = CanDeviceFail;
        return;
    }

    INIT_CONFIG canconfig[1];
    canconfig->Mode = 0;    //正常模式
    canconfig->Filter = 0;  //滤波不使能
    canconfig->AccCode = 0;
    canconfig->AccMask = 0xFFFFFFFF;  //掩码
    canconfig->Timing0 = 0x03;
    canconfig->Timing1 = 0x1c;  //125k
    if(!InitCAN(USBCAN2, DeviceInd0, CANInd0, canconfig))
    {
        IsOpenFlag = CanInitFail;
        CloseDevice(USBCAN2,DeviceInd0);
        return;
    }

    if(!StartCAN(USBCAN2,DeviceInd0,CANInd0))
    {
        IsOpenFlag = CanStar1Fail;
        CloseDevice(USBCAN2,DeviceInd0);
        return;
    }

    IsOpenFlag = CanOpenOk;

}

/************************************************************************************
函数名称:	ReceiveMsg
功能描述: CAN消息接收函数
参数：
***********************************************************************************/
void CanMessageDeal::ReceiveMsg()
{
    CAN_OBJ pReceive[100];
    ERR_INFO err;
    unsigned long res = 10;

    res = Receive(USBCAN2,DeviceInd0,CANInd0,pReceive,50,10);
    for(unsigned long i = 0;i < res;i++)
    {
        if(res==4294967295)
        {
            if(ReadErrInfo(USBCAN2,DeviceInd0,CANInd0,&err)!=STATUS_ERR)
            {
                qDebug()<<"读取数据失败!"<<"错误码为："<<QString::number(err.ErrCode,16);
            }
        }
        QString receive_str = "接收:";
        mRecCnt++;
        receive_str.append(QString::number(mRecCnt,10));
        receive_str.append("  帧ID:");
        receive_str.append(QString::number(pReceive[i].ID,16));
        receive_str.append("    数据：");
        for (int a=0;a<pReceive[i].DataLen;a++)
        {
            receive_str.append(QString::number(pReceive[i].Data[a],16));
            receive_str.append(" ");
        }
        emit my_signal(receive_str);//子线程处理完毕//触发自定义的信号

        CAN_APDU_T apdu;
        VCI_To_APDU(&apdu, &(pReceive[i]));

//        if (apdu.ucMsgClass==0
//            && apdu.ucSourceId!=0	//修正监控在线时容易蹦溃的问题
//            )
//        {
//            dlg->m_CanFlashObj.Msg_Recv(&apdu);
//        }
//        else if (apdu.ucMsgClass==2)
//        {
//            dlg->m_CanCtrlObj.Msg_Recv(&apdu);
//        }

//        if (apdu.ucMsgClass!=0)
//        {
//            gUpsSysInfo.Msg_Recv(&apdu);
//        }
    }
    Sleep(1);
}

/************************************************************************************
函数名称:	TransmitMsg
功能描述: CAN消息发送函数
参数：
***********************************************************************************/
void CanMessageDeal::MsgXmit(CAN_APDU_T *pMsg)
{
    QString transmit_str;

    CAN_OBJ vciMsg;
    APDU_To_VCI(&vciMsg, pMsg);
    DWORD flag = Transmit(USBCAN2,DeviceInd0,CANInd0,&vciMsg,1);

    if(!flag)
    {
        transmit_str = "发送失败:";
    }
    else
    {
        transmit_str = "发送完成:";
    }
    mRecCnt++;
    transmit_str.append(QString::number(mRecCnt,10));
    transmit_str.append("  帧ID:");
    transmit_str.append(QString::number(vciMsg.ID,16));
    transmit_str.append("    数据：");
    for (int i=0; i<vciMsg.DataLen; i++)
    {
        transmit_str.append(QString::number(vciMsg.Data[i],16));
        transmit_str.append(" ");
    }
    emit my_signal(transmit_str);//消息显示
}

/************************************************************************************
函数名称:	run
功能描述: 重写虚函数
参数：
***********************************************************************************/
void CanMessageDeal::run()
{
    qDebug()<<"线程启动";
    while(IsOpenFlag == CanOpenOk)
    {
        ReceiveMsg();
    }

}

/************************************************************************************
函数名称:	VCI_To_APDU
功能描述: CAN底层转应用层协议
参数：
***********************************************************************************/
void VCI_To_APDU(CAN_APDU_T *pApdu, CAN_OBJ *pVci)
{
    UINT16 canId = 0;
//    BYTE *pByte = (BYTE*)&canId;

    pVci->ID = 1;
    //    pByte[0] = (pVci->ID[1]>>5)|(pVci->ID[0]<<3);
    //    pByte[1] = pVci->ID[0]>>5;

    pApdu->ucMsgClass = ((canId&0x03c0)>>6);
    pApdu->ucServiceCode = (pVci->Data[1]&0x7f);
    pApdu->ucRsRq = (pVci->Data[1]>>7);
    pApdu->ucFrag = (pVci->Data[0]>>7);
    pApdu->ucSourceId = (canId&0x3f);
    pApdu->ucDestinationId = (pVci->Data[0]&0x3f);
    pApdu->u16Length = pVci->DataLen-2;
    pApdu->pData = (UCHAR*)&(pVci->Data[2]);
}

/************************************************************************************
函数名称:	APDU_To_VCI
功能描述: 应用层协议转CAN底层
参数：
***********************************************************************************/
void APDU_To_VCI( CAN_OBJ *pVci, CAN_APDU_T *pApdu)
{
    pVci->ID = (((UINT16)pApdu->ucMsgClass)<<6)+pApdu->ucSourceId;

    pVci->RemoteFlag = 0;//是否是远程帧
    pVci->ExternFlag = 0;//是否是扩展帧
    pVci->DataLen = pApdu->u16Length+2;
    pVci->Data[0] = (pApdu->ucFrag<<7)|(pApdu->ucDestinationId);
    pVci->Data[1] = (pApdu->ucRsRq<<7)|(pApdu->ucServiceCode);
    memcpy(pVci->Data+2, pApdu->pData, pApdu->u16Length);
}

