#ifndef DIALOGCANBOX_H
#define DIALOGCANBOX_H

#include <QDialog>
#include "main.h"
#include "lib/ECanVci.h"

extern long  mRecCnt;   //设备接收消息计数器

namespace Ui {
class DialogCanBox;
}

class DialogCanBox : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCanBox(QWidget *parent = nullptr);
    ~DialogCanBox();

    void OpenCanBox(void);
    void CloseCanBox(void);
    void RxThread(void);
    void TransmitMsg(void);

private:
    Ui::DialogCanBox *ui;

    QThread *taskCan = new QThread;
};

#endif // DIALOGCANBOX_H
