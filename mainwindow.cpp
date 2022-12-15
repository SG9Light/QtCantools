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
    ui->mCanFlag_label->setText("USBCAN-II Pro : 关闭");
    ui->mCanFlag_label->setStyleSheet("color:red;");
}

/************************************************************************************
函数名称:	on_mOpen_pushButton_clicked
功能描述: 打开CAN槽函数
参数：
***********************************************************************************/
void MainWindow::on_mOpen_pushButton_clicked()
{
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
