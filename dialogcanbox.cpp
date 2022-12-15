#include "dialogcanbox.h"
#include "ui_dialogcanbox.h"

//全局变量定义
long  mRecCnt = 0;

DialogCanBox::DialogCanBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCanBox)
{
    ui->setupUi(this);

    this->moveToThread(taskCan);
    taskCan->start();
}

DialogCanBox::~DialogCanBox()
{
    delete ui;
}

/************************************************************************************
函数名称:	OpenCanBox
功能描述: 打开CANBOX
参数：
***********************************************************************************/
void DialogCanBox::OpenCanBox()
{
    if(!OpenDevice(USBCAN2,DeviceInd0, CANInd0))
    {
        QMessageBox::critical(this,nullptr,"驱动打开失败",QMessageBox::Ok);
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
        QMessageBox::critical(this,nullptr,"CAN初始化失败",QMessageBox::Ok);
        CloseDevice(USBCAN2,DeviceInd0);
        return;
    }

    if(!StartCAN(USBCAN2,DeviceInd0,CANInd0))
    {
        QMessageBox::critical(this,nullptr,"通道1启动失败",QMessageBox::Ok);
        CloseDevice(USBCAN2,DeviceInd0);
        return;
    }

}

/************************************************************************************
函数名称:	CloseCanBox
功能描述: 关闭CANBOX
参数：
***********************************************************************************/
void DialogCanBox::CloseCanBox()
{
    taskCan->quit();
    taskCan->wait();
    CloseDevice(USBCAN2,DeviceInd0);
}

/************************************************************************************
函数名称:	RxThread
功能描述: CANBOX接收主线程
参数：
***********************************************************************************/
void DialogCanBox::RxThread()
{
    CAN_OBJ pReceive[200];
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
    }
    Sleep(1);
}

/************************************************************************************
函数名称:	TransmitMsg
功能描述: CANBOX数据发送函数
参数：
***********************************************************************************/
void DialogCanBox::TransmitMsg()
{
    QString transmit_str;
    CAN_OBJ vciMsg;

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

}




