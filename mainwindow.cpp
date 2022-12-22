#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    qRegisterMetaType<CAN_OBJ>("CAN_OBJ");//注册结构

    ui->mCanFlag_label->setText("USBCAN-II Pro : 关闭");
    ui->mCanFlag_label->setStyleSheet("color:red;");

    connect(objCanBox, &CanBox::e_Disp, ui->mDisp_textBrowser, &QTextBrowser::append);
    connect(this, &MainWindow::e_filePath, objCanFlash, &CanFlashupdate::GetUpdataFile);
    connect(ui->mStartUp_pushButton, &QPushButton::clicked, objCanFlash, &CanFlashupdate::FlashupdateThread);
    connect(objCanFlash, &CanFlashupdate::e_MsgToSerial, this, [=](CAN_OBJ pVci){
        objCanBox->TransmitMsg(pVci);
    });

    connect(objCanBox, &CanBox::e_BoxRx, this, [=](CAN_OBJ pVci){
        objCanFlash->Msg_RxSerial(pVci);
    });

    //整体进度显示
    ui->mOverall_progressBar->setValue(0);
    connect(objCanFlash, &CanFlashupdate::e_UpProgressSet,ui->mOverall_progressBar, &QProgressBar::setMaximum);
    connect(objCanFlash, &CanFlashupdate::e_UpProgressDisp,ui->mOverall_progressBar, &QProgressBar::setValue);
    //BLOCK进度显示
    ui->mBlocknum_progressBar->setValue(0);
    connect(objCanFlash, &CanFlashupdate::e_BlockProgressSet,ui->mBlocknum_progressBar,&QProgressBar::setMaximum);
    connect(objCanFlash, &CanFlashupdate::e_BlockProgressDisp,ui->mBlocknum_progressBar,&QProgressBar::setValue);
}

/************************************************************************************
函数名称:	MainWindow
功能描述: 析构函数
参数：
***********************************************************************************/
MainWindow::~MainWindow()
{
    objCanBox->deleteLater();
    delete ui;
}

/************************************************************************************
函数名称:	on_mOpen_pushButton_clicked
功能描述: 打开CAN槽函数
参数：
***********************************************************************************/
void MainWindow::on_mOpen_pushButton_clicked()
{
    int dwr = objCanBox->OpenCanBox();
    switch (dwr) {
    case 1:
        QMessageBox::information(this,nullptr,"设备打开成功",QMessageBox::Ok);
        break;

    case 2:
        QMessageBox::critical(this,nullptr,"驱动打开失败",QMessageBox::Ok);
         break;

    case 3:
        QMessageBox::critical(this,nullptr,"CAN初始化失败",QMessageBox::Ok);
         break;

    case 4:
         QMessageBox::critical(this,nullptr,"通道1启动失败",QMessageBox::Ok);
        break;
    default:

        break;
    }
    if(dwr)
    {
        ui->mCanFlag_label->setText("USBCAN-II Pro : 打开");
        ui->mCanFlag_label->setStyleSheet("color:green;");
    }

}

/************************************************************************************
函数名称:	on_mClose_pushButton_clicked
功能描述: 关闭can槽函数
参数：
***********************************************************************************/
void MainWindow::on_mClose_pushButton_clicked()
{
    objCanBox->CloseCanBox();
    ui->mCanFlag_label->setText("USBCAN-II Pro : 关闭");
    ui->mCanFlag_label->setStyleSheet("color:red;");
}

/************************************************************************************
函数名称:	on_mSend_pushButton_clicked
功能描述: 发送槽函数
参数：
***********************************************************************************/
void MainWindow::on_mSend_pushButton_clicked()
{
    CAN_OBJ mVci;

    mVci.RemoteFlag = 0;
    mVci.ExternFlag = 0;
    mVci.ID = 1;
    mVci.DataLen = 2;
    mVci.Data[0] = 0x55;
    mVci.Data[1] = 0xff;


    objCanBox->TransmitMsg(mVci);
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

    emit e_filePath(fileName);
    ui->mDataPath_lineEdit->setText(fileName);

}
