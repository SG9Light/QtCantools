#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "main.h"
#include "canbox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_mOpen_pushButton_clicked();

    void on_mClose_pushButton_clicked();

    void on_mSend_pushButton_clicked();

    void on_mOpenFile_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    CanBox *objCanBox = new CanBox;

};
#endif // MAINWINDOW_H
