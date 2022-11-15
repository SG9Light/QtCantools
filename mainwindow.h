#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

#include "canmessagedeal.h"
#include "flashupdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void StopCanThread();

private slots:
    void on_mOpen_pushButton_clicked();

    void on_mClose_pushButton_clicked();

    void on_mSend_pushButton_clicked();

    void on_mStartUp_pushButton_clicked();

    void on_mOpenFile_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    CanMessageDeal *Candeal = new CanMessageDeal;
    FlashUpData *flash = new FlashUpData;

};
#endif // MAINWINDOW_H
