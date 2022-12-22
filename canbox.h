#ifndef CANBOX_H
#define CANBOX_H

#include "main.h"
#include "lib/ECanVci.h"

class MainWindow;

class CanBox : public QObject
{
    Q_OBJECT
public:
    explicit CanBox(QObject *parent = nullptr);
    ~CanBox();

    int OpenCanBox(void);
    void CloseCanBox(void);
    void RxThread(void);
    void TransmitMsg(CAN_OBJ pVci);

signals:
    void e_Open(void);
    void e_Disp(QString str);
    void e_BoxRx(CAN_OBJ pVci);

private:
    QThread *taskCan = new QThread;

    long  mRecCnt;   //设备接收消息计数器
    bool mCloseCan;  //关闭CAN则退出接收线程

    int nDeviceType;    //设备类型
    int nDeviceInd;     //
    int nCANInd;        //CAN通道ID
};

#endif // CANBOX_H
