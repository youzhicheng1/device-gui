#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <algorithm>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //加载 QSS 样式(从资源)
    QFile qss(":/style.qss");
    if (qss.open(QFile::ReadOnly)) {
        this->setStyleSheet(qss.readAll());
        qss.close();
    }

    //菜单栏
    mnBar=menuBar();
    QMenu* device=new QMenu("device");
    QMenu* edit=new QMenu("edit");
    //设备菜单栏
    mnBar->addMenu(device);
    QAction *add=new QAction("add");
    QAction *quit=new QAction("quit");
    device->addAction(add);
    device->addSeparator();//添加分割线
    device->addAction(quit);
    //编辑菜单栏
    mnBar->addMenu(edit);
    QAction *del=new QAction("del");
    QAction *clear=new QAction("clear");
    edit->addAction(del);
    edit->addSeparator();
    edit->addAction(clear);
    //文件菜单栏
    QMenu* file=new QMenu("file");
    mnBar->addMenu(file);
    QAction* save=new QAction("save");
    QAction* open=new QAction("open");
    file->addAction(save);
    file->addSeparator();
    file->addAction(open);
    //信号连接
    connect(add,&QAction::triggered,this,&MainWindow::on_AddButton_clicked);
    connect(quit,&QAction::triggered,this,&QWidget::close);
    connect(del,&QAction::triggered,this,&MainWindow::on_DelButton_clicked);
    connect(clear,&QAction::triggered,this,&MainWindow::on_ClearButton_clicked);
    connect(save,&QAction::triggered,this,&MainWindow::SaveDevice);
    connect(open,&QAction::triggered,this,&MainWindow::OpenDevice);

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
    QWidget* picturePage=new QWidget(this);
    //将散落的按钮放入布局便于添加进页面0
    QVBoxLayout* layout=new QVBoxLayout(devicePage);
    layout->addWidget(ui->horizontalLayoutWidget);
    layout->addWidget(ui->listWidget);
    //页面1显示设备管理并添加布局居中
    QLabel* info=new QLabel("设备管理 v5 ",infoPage);
    info->setAlignment(Qt::AlignCenter);
    QVBoxLayout* lay2=new QVBoxLayout(infoPage);
    lay2->addWidget(info);
    //页面2将图片放入布局中
    picture=new QLabel(picturePage);
    picture->setScaledContents(true);
    picture1=new QLabel(picturePage);
    picture1->setScaledContents(true);
    QVBoxLayout* lay3=new QVBoxLayout(picturePage);
    lay3->addWidget(picture);
    lay3->addWidget(picture1);

    //加入界面
    stackPages->addWidget(devicePage);
    stackPages->addWidget(infoPage);
    stackPages->addWidget(picturePage);
    setCentralWidget(stackPages);
    //工具栏添加按钮用于切换界面
    QAction *devicepage=new QAction("device");
    QAction *about=new QAction("about");
    QAction *pic=new QAction("picture");
    toolBar->addAction(devicepage);
    toolBar->addAction(about);
    toolBar->addAction(pic);
    connect(pic, &QAction::triggered, this, &MainWindow::OpenPicture);

    connect(devicepage,&QAction::triggered,this,[this]{stackPages->setCurrentIndex(0);});
    connect(about,&QAction::triggered,this,[this]{stackPages->setCurrentIndex(1);});
    connect(pic,&QAction::triggered,this,[this]{stackPages->setCurrentIndex(2);});

    //添加右键菜单
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget,&QWidget::customContextMenuRequested,this,[this](const QPoint& pos){
        QMenu menu(this);
        menu.addAction("添加设备",this,&MainWindow::on_AddButton_clicked);
        menu.addAction("删除设备",this,&MainWindow::on_DelButton_clicked);
        menu.addAction("清空设备",this,&MainWindow::on_ClearButton_clicked);

        menu.exec(ui->listWidget->mapToGlobal(pos));
    });



    QThread* thread=new QThread(this);
    Worker* worker=new Worker;
    worker->moveToThread(thread);
    connect(thread,&QThread::started,worker,&Worker::dowork);
    connect(worker,&Worker::finished,thread,&QThread::quit);
    connect(worker,&Worker::progress,this,[](int p){
        qDebug()<<"当前进度："<<p;
    });

    thread->start();
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);  // 线程自我清理
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

void MainWindow::convertPicture(cv::Mat img)
{
    //灰度图
    cv::Mat gray;
    cv::cvtColor(img,gray,cv::COLOR_BGR2GRAY);
    //二值图
    cv::Mat binary;
    cv::threshold(gray,binary,127,255,cv::THRESH_BINARY);

    QImage qbin(binary.data,binary.cols,binary.rows,binary.step,QImage::Format_Grayscale8);
    //寻找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
    //筛选颗粒
    std::vector<double> areas;
    std::vector<double> diameters;
    std::vector<double> circularities;
    for(const auto &c:contours){
        double area=cv::contourArea(c);
        if(area<10) continue;
        areas.push_back(area);
        //直径
        double d=2*std::sqrt(area/CV_PI);
        //圆度
        double p=cv::arcLength(c,true);
        double circ=4*CV_PI*area/(p*p);

        diameters.push_back(d);
        circularities.push_back(circ);
    }
    //"标定"
    bool ok;
    double ratio=QInputDialog::getDouble(this,"标定","每像素多少微米？",1.0,0.001,1000,3,&ok);
    if(!ok) return;
    double pixperMicron=ratio;
    //统计
    ////面积
    double sum=0,maxA=0,minA=1e9;
    for(double a:areas){
        sum+=a;
        maxA=std::max(maxA,a);
        minA=std::min(minA,a);
    }
    double avg=areas.empty()?0:(sum/areas.size());

    ////直径
    double sumD=0,maxD=0,minD=1e9;
    for(double d:diameters){
        sumD+=d;
        maxD=std::max(maxD,d);
        minD=std::min(minD,d);
    }
    double avgD=diameters.empty()?0:(sumD/diameters.size());
    double realD=avgD*pixperMicron;

    ////圆度
    double sumC=0;
    for(double C:circularities){
        sumC+=C;
    }
    double avgC=circularities.empty()?0:(sumC/circularities.size());

    // 显示
    QMessageBox::information(this, "颗粒统计",
                             QString("颗粒数:%1\n平均面积:%2\n最大:%3 最小:%4\n平均直径：%5\n最大：%6 最小%7\n平均圆度：%8\n真实直径为：%9")
                                 .arg(areas.size()).arg(avg).arg(maxA).arg(minA).arg(avgD).arg(maxD).arg(minD).arg(avgC).arg(realD));

    cv::Mat result=img.clone();
    cv::drawContours(result,contours,-1,cv::Scalar(0,255,0),2);

    cv::Mat rgbResult;
    cv::cvtColor(result,rgbResult,cv::COLOR_BGR2RGB);
    QImage qres(rgbResult.data,rgbResult.cols,rgbResult.rows,rgbResult.step,QImage::Format_RGB888);


    picture1->setPixmap(QPixmap::fromImage(qres.copy()));
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

void MainWindow::SaveDevice()
{
    QString path=QFileDialog::getSaveFileName(this,"保存设备列表","D:\\Linux课件2025\\Linux课件2025\\Qt练习\\Device_gui_v2","文本文档(*.txt)");
    if(path.isEmpty()) return;

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) return;

    QTextStream out(&file);
    for(int i=0;i<ui->listWidget->count();i++){
        out<<ui->listWidget->item(i)->text()<<"\n";
    }

    file.close();
}

void MainWindow::OpenDevice()
{
    QString path=QFileDialog::getOpenFileName(this,"保存设备列表","D:\\Linux课件2025\\Linux课件2025\\Qt练习\\Device_gui_v2","文本文档(*.txt)");
    if(path.isEmpty()) return;

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) return;

    QString all=file.readAll();
    QStringList lines=all.split("\n");
    for(auto eachline:lines){
        if(eachline.isEmpty()){
            break;
        }
        ui->listWidget->addItem(eachline);
    }

    file.close();
    updateStatus();
}

void MainWindow::OpenPicture()
{
    QString path=QFileDialog::getOpenFileName(this,"打开图片","D:/","图片 (*.png *.jpg *.bmp)");
    if(path.isEmpty()) QMessageBox::warning(this,"错误","图片不存在");

    // QPixmap pix(path);
    // if(pix.isNull()) QMessageBox::warning(this,"错误","图片打开失败");

    cv::Mat img = cv::imread(path.toLocal8Bit().toStdString());
    if(img.empty()) QMessageBox::warning(this,"错误","图片打开失败");
    convertPicture(img);

    cv::Mat rgb;
    cv::cvtColor(img,rgb,cv::COLOR_BGR2RGB);
    QImage qimg(rgb.data,rgb.cols,rgb.rows,rgb.step,QImage::Format_RGB888);
    QPixmap pix=QPixmap::fromImage(qimg.copy());

    picture->setPixmap(pix);
}

