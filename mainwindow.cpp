#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //菜单栏
    mnBar=menuBar();
    QMenu* doc=new QMenu("doc");
    QMenu* edit=new QMenu("edit");
    //文件菜单栏
    mnBar->addMenu(doc);
    QAction *add=new QAction("add");
    QAction *quit=new QAction("quit");
    doc->addAction(add);
    doc->addSeparator();//添加分割线
    doc->addAction(quit);
    //编辑菜单栏
    mnBar->addMenu(edit);
    QAction *del=new QAction("del");
    QAction *clear=new QAction("clear");
    edit->addAction(del);
    doc->addSeparator();
    edit->addAction(clear);
    //信号连接
    connect(add,&QAction::triggered,this,&MainWindow::on_AddButton_clicked);
    connect(quit,&QAction::triggered,this,&QWidget::close);
    connect(del,&QAction::triggered,this,&MainWindow::on_DelButton_clicked);
    connect(clear,&QAction::triggered,this,&MainWindow::on_ClearButton_clicked);

    //工具栏
    toolBar=new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea,toolBar);
    toolBar->setFloatable(1);
    toolBar->setMovable(1);
    toolBar->addAction(add);
    toolBar->addAction(del);
    toolBar->addAction(clear);

    //当前时间
    QTimer* timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::updateStatus);
    timer->start(1000);

    //状态栏
    stsBar=statusBar();

    //多页面
    stackPages=new QStackedWidget(this);
    QWidget* devicePage=new QWidget(this);
    QWidget* infoPage=new QWidget(this);
    //将散落的按钮放入布局便于添加进页面0
    QVBoxLayout* layout=new QVBoxLayout(devicePage);
    layout->addWidget(ui->horizontalLayoutWidget);
    layout->addWidget(ui->listWidget);
    //页面2显示设备管理并添加布局居中
    QLabel* info=new QLabel("设备管理 v5 ",infoPage);
    info->setAlignment(Qt::AlignCenter);
    QVBoxLayout* lay2=new QVBoxLayout(infoPage);
    lay2->addWidget(info);
    //加入界面
    stackPages->addWidget(devicePage);
    stackPages->addWidget(infoPage);
    setCentralWidget(stackPages);
    //工具栏添加按钮用于切换界面
    QAction *device=new QAction("device");
    QAction *about=new QAction("about");
    toolBar->addAction(device);
    toolBar->addAction(about);
    connect(device,&QAction::triggered,this,[this]{stackPages->setCurrentIndex(0);});
    connect(about,&QAction::triggered,this,[this]{stackPages->setCurrentIndex(1);});
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::updateStatus()
{
    stsBar->showMessage(QString("当前设备数：%1 | 当前时间：%2").arg(ui->listWidget->count()).arg(timeupdate()));
}

QString MainWindow::timeupdate()
{
    QString str=QDateTime::currentDateTime().toString("hh:mm:ss");
    return str;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Delete){
        delete ui->listWidget->takeItem(ui->listWidget->currentRow());
        updateStatus();
    }
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

