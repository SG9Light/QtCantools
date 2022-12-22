#ifndef UPSINFO_H
#define UPSINFO_H

#include "main.h"
#include "CanFlashupdate.h"

class upsInfo : public QObject
{
    Q_OBJECT
public:
    explicit upsInfo(QObject *parent = nullptr);

    void UpsModPoll(void);

signals:
    void e_UpsPoll(CAN_OBJ pVci);

private:
    QTimer *mTimer1 = new QTimer;

};

#endif // UPSINFO_H
