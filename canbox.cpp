#include "canbox.h"

CanBox::CanBox(QObject *parent) : QObject(parent)
{
    this->moveToThread(taskCan);
    taskCan->start();

    mCloseCan = true;
    mRecCnt = 0;

    connect(this,&CanBox::e_Close, this, &CanBox::RxThread);
}

CanBox::~CanBox()
{
    taskCan->quit();
    taskCan->wait();
    taskCan->deleteLater();
}

/************************************************************************************
函数名称:	OpenCanBox
功能描述: 打开CANBOX
参数：
***********************************************************************************/
int CanBox::OpenCanBox()
{
    if(!OpenDevice(USBCAN2,DeviceInd0, CANInd0))
    {
        return 2;
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
        CloseDevice(USBCAN2,DeviceInd0);
        return 3;
    }

    if(!StartCAN(USBCAN2,DeviceInd0,CANInd0))
    {
        CloseDevice(USBCAN2,DeviceInd0);
        return 4;
    }

    mCloseCan = false;
    emit e_Close();
    return 1;

}

/************************************************************************************
函数名称:	CloseCanBox
功能描述: 关闭CANBOX
参数：
***********************************************************************************/
void CanBox::CloseCanBox()
{
    CloseDevice(USBCAN2,DeviceInd0);
    mCloseCan = true;
}

/************************************************************************************
函数名称:	RxThread
功能描述: CANBOX接收主线程
参数：
***********************************************************************************/
void CanBox::RxThread()
{
    CAN_OBJ pReceive[200];
    ERR_INFO err;
    unsigned long res = 10;

    while (1)
    {
        if(mCloseCan)
        {
            return;
        }

        res = Receive(USBCAN2,DeviceInd0,CANInd0,pReceive,50,10);
        for(unsigned long i = 0; i < res;i++)
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
            emit e_Disp(receive_str);
        }

    }
}

/************************************************************************************
函数名称:	TransmitMsg
功能描述: CANBOX数据发送函数
参数：
***********************************************************************************/
void CanBox::TransmitMsg()
{
    QString transmit_str;
    CAN_OBJ vciMsg;
    vciMsg.ID = 1;
    vciMsg.DataLen = 2;
    vciMsg.Data[0] = 0;
    vciMsg.Data[1] = 1;
    vciMsg.Data[2] = 2;
    vciMsg.Data[3] = 3;

    if(mCloseCan)return;
    Transmit(USBCAN2,DeviceInd0,CANInd0,&vciMsg,1);
    transmit_str = "发送完成:";
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
    emit e_Disp(transmit_str);

}

