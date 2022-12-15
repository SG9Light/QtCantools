#ifndef CANMESSAGEDEAL_H
#define CANMESSAGEDEAL_H

#include <QObject>
#include <QThread>

#include "lib/ECanVci.h"
#include "types.h"


class CanMessageDeal : public QThread
{
    Q_OBJECT
public:
    explicit CanMessageDeal();

    void ReceiveMsg();

    void MsgRecv(CAN_APDU_T *pCanRecvMsg);
    void MsgXmit(IN CAN_APDU_T *pMsg);

protected:
    void run() override;

signals:
    void my_signal(QString str);//信号函数

private:
    CAN_APDU_T m_XmitMsg;
    CAN_APDU_T m_RecvMsg;

};

extern void VCI_To_APDU(CAN_APDU_T *pApdu, CAN_OBJ *pVci);
extern void APDU_To_VCI( CAN_OBJ *pVci, CAN_APDU_T *pApdu);

#endif // CANMESSAGEDEAL_H
