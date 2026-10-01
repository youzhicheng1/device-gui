#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    stsBar=statusBar();
    stsBar->showMessage(QString("当前设备数：%1").arg(ui->listWidget->count()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::updateStatus()
{
    stsBar->showMessage(QString("当前设备数：%1").arg(ui->listWidget->count()));
}


void MainWindow::on_AddButton_clicked()
{
    QString str=QInputDialog::getText(this,"新建设备","设备名");
    QList<QListWidgetItem*> isfind=ui->listWidget->findItems(str,Qt::MatchExactly);
    if(isfind.empty()){
        ui->listWidget->addItem(str);
    }else QMessageBox::warning(this,"警告","设备已存在");

    //stsBar->showMessage(QString("当前设备数：%1").arg(++device_count));
    //stsBar->showMessage(QString("当前设备数：%1").arg(ui->listWidget->count()));
    updateStatus();
}




void MainWindow::on_DelButton_clicked()
{
    delete ui->listWidget->takeItem(ui->listWidget->currentRow());
    //stsBar->showMessage(QString("当前设备数：%1").arg(--device_count));
    //stsBar->showMessage(QString("当前设备数：%1").arg(ui->listWidget->count()));
    updateStatus();
}




void MainWindow::on_ClearButton_clicked()
{
    ui->listWidget->clear();
    //stsBar->showMessage(QString("当前设备数：%1").arg(0));
    //stsBar->showMessage(QString("当前设备数：%1").arg(ui->listWidget->count()));
    updateStatus();
}


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString str=QInputDialog::getText(this,"修改设备","设备名",QLineEdit::Normal,item->text());
    QList<QListWidgetItem*> isfind=ui->listWidget->findItems(str,Qt::MatchExactly);
    if(isfind.empty()){
        item->setText(str);
    }else QMessageBox::warning(this,"警告","设备已存在");

    //stsBar->showMessage(QString("当前设备数：%1").arg(ui->listWidget->count()));
    updateStatus();
}

