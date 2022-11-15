#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdio.h"

/************************************************************************************
函数名称:	MainWindow
功能描述: 构造函数
参数：
***********************************************************************************/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->mCanFlag_label->setText("USBCAN-II Pro : 关闭");
    ui->mCanFlag_label->setStyleSheet("color:red;");
    connect(Candeal, &CanMessageDeal::my_signal, this,[=](QString str){
        ui->mDisp_textBrowser->append(str);
    });
}

/************************************************************************************
函数名称:	MainWindow
功能描述: 析构函数
参数：
***********************************************************************************/
MainWindow::~MainWindow()
{
    StopCanThread();

    delete Candeal;
    delete ui;
}

/************************************************************************************
函数名称:	StopCanThread
功能描述: 关闭CAN盒
参数：
***********************************************************************************/
void MainWindow::StopCanThread()
{
    Candeal->CloseCanBox();
    Candeal->wait();
    Candeal->quit();
    ui->mCanFlag_label->setText("USBCAN-II Pro : 关闭");
    ui->mCanFlag_label->setStyleSheet("color:red;");
}

void MainWindow::ReadASectionAsciiHex1(BYTE *buf, DWORD &pos, WORD &nLength, UINT &nAddress, BYTE *sectionbuff)
{

    nLength = 0;
    nAddress = 0;

    //越过文件起始标志
    if (buf[pos]==0x02)
    {
        pos++;
    }

    while (buf[pos] == 0x0D || buf[pos] == 0x0A || buf[pos] == ' ')
    {
        pos++;
    }

    //文件结束
    if (buf[pos] == 0x03)
    {
        qDebug() << "ReadASectionAsciiHex:read end!\n";
        return;
    }

    printf("Read Address!\r\n");

    nAddress = 0;

    //读取到地址
    if (buf[pos]==0x24 && buf[pos+1]==0x41)
    {
        qDebug() << "enter  Read Address!\r\n";
        pos += 2;
        BYTE *pAdd = (BYTE*)&nAddress;
        printf("Adrress:%02X, %02X, %02X\r\n", buf[pos], buf[pos+1], buf[pos+2]);
        while (buf[pos]!=',')
        {
            nAddress = nAddress<<8;
            pAdd[0] = ReadAByteAsciiHex1(buf, pos);
            printf("%02X ", pAdd[0]);
        }
        pos++;
        qDebug() << "leave  Read Address!\r\n";
    }

    int nByteLen = 0;
    //读到下一个地址或文件结束时停止
    while (buf[pos]!=0x24 && buf[pos]!=0x03)
    {
        sectionbuff[nByteLen+1] = ReadAByteAsciiHex1(buf, pos);	//高字节在前
        sectionbuff[nByteLen] = ReadAByteAsciiHex1(buf, pos);
        nByteLen += 2;
    }

    if (nByteLen%2)
    {
        qDebug() << "nByteLen is ODD!\n";
    }

    nLength = nByteLen/2;

    qDebug() << "Read A Section!\r\n";
}

BYTE MainWindow::ReadAByteAsciiHex1(BYTE *buf, DWORD &pos)
{
    BYTE nData;
    int ucInput1;
    int ucInput2;

    while (buf[pos] == 0x0D || buf[pos] == 0x0A || buf[pos] == ' ')
    {
        pos++;
    }

    ucInput1 = *(buf+pos);
    ucInput2 = *(buf+pos+1);
    if (ucInput1>='a')
    {
        ucInput1 = ucInput1-'a'+10;
    }
    else if (ucInput1 > 0x39)
    {
        ucInput1 = ucInput1 - 0x37;					/*'A' - 'F'*/
    }
    else
    {
        ucInput1 = ucInput1 - 0x30;					/*'0' - '9'*/
    }

    if (ucInput2>='a')
    {
        ucInput2 = ucInput2-'a'+10;
    }
    else if (ucInput2 > 0x39)
    {
        ucInput2 = ucInput2 - 0x37;					/*'A' - 'F'*/
    }
    else
    {
        ucInput2 = ucInput2 - 0x30;					/*'0' - '9'*/
    }

    nData = (ucInput1 << 4) + ucInput2;

    pos +=2;

    while (buf[pos] == 0x0D || buf[pos] == 0x0A || buf[pos] == ' ')
    {
        pos++;
    }

    return nData;
}

/************************************************************************************
函数名称:	on_mOpen_pushButton_clicked
功能描述: 打开CAN槽函数
参数：
***********************************************************************************/
void MainWindow::on_mOpen_pushButton_clicked()
{
    if(IsOpenFlag == CanOpenOk)
    {
        QMessageBox::warning(this,nullptr,"CAN已打开",QMessageBox::Ok);
        return;
    }

    Candeal->OpenCanBox();

    switch (IsOpenFlag)
    {
    case CanBoxClose:
        QMessageBox::critical(this,nullptr,"CAN未打开",QMessageBox::Ok);
        break;
    case CanDeviceFail :
        QMessageBox::critical(this,nullptr,"驱动打开失败",QMessageBox::Ok);
        break;
    case CanInitFail   :
        QMessageBox::critical(this,nullptr,"CAN初始化失败",QMessageBox::Ok);
        break;
    case CanStar1Fail  :
        QMessageBox::critical(this,nullptr,"通道1启动失败",QMessageBox::Ok);
        break;
    case CanStar2Fail  :
        QMessageBox::critical(this,nullptr,"通道2启动失败",QMessageBox::Ok);
        break;
    default:
        break;

    }
    if(IsOpenFlag != CanOpenOk)
    {
        return;
    }

    ui->mCanFlag_label->setText("USBCAN-II Pro : 打开");
    ui->mCanFlag_label->setStyleSheet("color:green;");
    Candeal->start();
}

/************************************************************************************
函数名称:	on_mClose_pushButton_clicked
功能描述: 关闭can槽函数
参数：
***********************************************************************************/
void MainWindow::on_mClose_pushButton_clicked()
{
    StopCanThread();
}

/************************************************************************************
函数名称:	on_mSend_pushButton_clicked
功能描述: 发送槽函数
参数：
***********************************************************************************/
void MainWindow::on_mSend_pushButton_clicked()
{
    Candeal->m_XmitMsg.ucDestinationId = 0x3f;
    Candeal->m_XmitMsg.ucServiceCode = 0x21;
    Candeal->m_XmitMsg.ucMsgClass = 2;
    Candeal->m_XmitMsg.ucFrag = 0;
    Candeal->m_XmitMsg.ucRsRq = 1;
    Candeal->m_XmitMsg.ucSourceId = 1;

    Candeal->MsgXmit(&Candeal->m_XmitMsg);
}

/************************************************************************************
函数名称:	on_mStartUp_pushButton_clicked
功能描述: 开始升级flash槽函数
参数：
***********************************************************************************/
void MainWindow::on_mStartUp_pushButton_clicked()
{
    flash->HandCommXmitFcb();
    flash->FlashUpdateRoutine();
}

/************************************************************************************
函数名称:	on_mOpenFile_pushButton_clicked
功能描述: 选择升级文件
参数：
***********************************************************************************/
void MainWindow::on_mOpenFile_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"打开","","*.ipm5k");
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,nullptr,"文件路径为空",QMessageBox::Ok);
        return;
    }
    ui->mDataPath_lineEdit->setText(fileName);

    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray arr = file.readAll();
        qDebug()<< arr.size();
        memcpy(cFlashUpdateBuf,arr,arr.size());

        file.close();
    }

}
