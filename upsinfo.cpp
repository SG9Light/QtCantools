#include "upsinfo.h"


upsInfo::upsInfo(QObject *parent) : QObject(parent)
{
    mTimer1->start(1000);
    connect(mTimer1, &QTimer::timeout, this, &upsInfo::UpsModPoll);
}

/**********************************************************************
UpsModPoll-----广播查询在线模块
Parameters:
Return Value:
**********************************************************************/
void upsInfo::UpsModPoll()
{
    CAN_OBJ vciMsg;
    CAN_APDU_T apdu;

    apdu.pData = NULL;
    apdu.u16Length = 0;
    apdu.ucDestinationId = 0x3f;
    apdu.ucFrag = 0;
    apdu.ucMsgClass = 4;
    apdu.ucRsRq = 0;
    apdu.ucServiceCode = 1;
    apdu.ucSourceId = 1;

    APDU_To_VCI(&vciMsg, &apdu);
    emit e_UpsPoll(vciMsg);

}
