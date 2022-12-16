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

    connect(objCanBox, &CanBox::e_Disp, ui->mDisp_textBrowser, &QTextBrowser::append);
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
    objCanBox->TransmitMsg();
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

        file.close();
    }

}
