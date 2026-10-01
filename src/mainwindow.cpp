#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Analyzer.h"

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


    //页面2将图片和参数面板放入布局中
    QHBoxLayout* page2layout=new QHBoxLayout(picturePage);
    //左侧布局
    picture=new QLabel(picturePage);
    picture->setScaledContents(true);
    picture1=new QLabel(picturePage);
    picture1->setScaledContents(true);
    QVBoxLayout* leftlayout=new QVBoxLayout(picturePage);
    leftlayout->addWidget(picture);
    leftlayout->addWidget(picture1);

    //右侧布局
    QVBoxLayout* rightpanel=new QVBoxLayout(picturePage);
    //阈值行
    QHBoxLayout* thresholdline=new QHBoxLayout(picturePage);
    QLabel* thresholdname=new QLabel(picturePage);thresholdname->setText("Threshold");
    m_thresholdSlider=new QSlider(picturePage);m_thresholdSlider->setOrientation(Qt::Horizontal);m_thresholdSlider->setRange(0,255);m_thresholdSlider->setValue(127);
    QLabel* thresholdValue=new QLabel(picturePage);
    thresholdline->addWidget(thresholdname);
    thresholdline->addWidget(m_thresholdSlider);
    thresholdline->addWidget(thresholdValue);
    rightpanel->addLayout(thresholdline);
    //形态学
    QHBoxLayout* morphline=new QHBoxLayout(picturePage);
    QLabel* morphname=new QLabel(picturePage);morphname->setText("morph");
    QRadioButton* rb_none  = new QRadioButton("none");
    QRadioButton* rb_open  = new QRadioButton("open");
    QRadioButton* rb_close = new QRadioButton("close");
    m_morphGroup = new QButtonGroup(this);   // ← 新面孔,作用:组内互斥
    m_morphGroup->addButton(rb_none,  0);   // 第二个参数是 id
    m_morphGroup->addButton(rb_open,  1);
    m_morphGroup->addButton(rb_close, 2);
    morphline->addWidget(morphname);
    morphline->addWidget(rb_none);
    morphline->addWidget(rb_open);
    morphline->addWidget(rb_close);
    rightpanel->addLayout(morphline);
    //核大小
    QHBoxLayout* kernelSizeline=new QHBoxLayout(picturePage);
    QLabel* kernelSizename=new QLabel(picturePage);kernelSizename->setText("kernelSize");
    m_kernelSize=new QSpinBox(picturePage);m_kernelSize->setRange(1,15);m_kernelSize->setValue(3);
    kernelSizeline->addWidget(kernelSizename);
    kernelSizeline->addWidget(m_kernelSize);
    rightpanel->addLayout(kernelSizeline);
    //最小面积
    QHBoxLayout* minArealine=new QHBoxLayout(picturePage);
    QLabel* minAreaname=new QLabel(picturePage);minAreaname->setText("minArea");
    m_minArea=new QSpinBox(picturePage);m_minArea->setRange(1,1000);m_minArea->setValue(10);
    minArealine->addWidget(minAreaname);
    minArealine->addWidget(m_minArea);
    rightpanel->addLayout(minArealine);
    //分析按钮
    m_analyzeButton=new QPushButton(picturePage);m_analyzeButton->setText("Analyze");
    rb_none->setChecked(true);
    connect(m_analyzeButton,&QPushButton::clicked,this,&MainWindow::onAnalyzeClick);
    rightpanel->addWidget(m_analyzeButton);
    //导出按钮
    QPushButton* ExportButton=new QPushButton(picturePage);ExportButton->setText("Export CSV");
    connect(ExportButton,&QPushButton::clicked,this,&MainWindow::onExportCsv);
    rightpanel->addWidget(ExportButton);
    //放入布局
    page2layout->addLayout(leftlayout);
    page2layout->addLayout(rightpanel);

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



    thread=new QThread(this);
    worker=new Worker;
    worker->moveToThread(thread);
    qRegisterMetaType<statistics>("statistics");

    thread->start();
    connect(this,&MainWindow::startAnalyze,worker,&Worker::doAnalyze);   //叫ui来干活
    connect(worker,&Worker::analyzeDone,this,&MainWindow::onAnalyzeDone);//ui收结果
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);  // 线程自我清理
    connect(thread,&QThread::finished,worker,&QThread::deleteLater);
}

MainWindow::~MainWindow()
{
    thread->quit();
    thread->wait();
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
    //"标定"
    bool ok;
    double ratio=QInputDialog::getDouble(this,"标定","每像素多少微米？",1.0,0.001,1000,3,&ok);
    if(!ok) return;

    m_currentImg=img;
    m_ratio=ratio;
    m_lastParams=collectParams();
    emit startAnalyze(m_currentImg,m_ratio,collectParams());
}

AnalyzeParams MainWindow::collectParams() const
{
    AnalyzeParams p;
    p.threshold  = m_thresholdSlider->value();   // 滑条当前值 0-255
    int t        = m_morphGroup->checkedId();    // 0/1/2
    p.morphType  = (t < 0) ? 0 : t;              // 兜底:没选中=不处理
    p.kernelSize = m_kernelSize->value();
    p.minArea    = m_minArea->value();
    return p;
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

void MainWindow::onAnalyzeDone(const statistics &ss)
{
    m_lastResult=ss;
    QMessageBox::information(this, "颗粒统计",
                             QString("颗粒数:%1\n平均面积:%2\n最大:%3 最小:%4\n平均直径：%5\n最大：%6 最小%7\n平均圆度：%8\n真实直径为：%9"
                                     "—— 粒径分布 ——\nD10:%10  D50:%11  D90:%12\n跨度span:%13")
                                .arg(ss.count).arg(ss.avgA).arg(ss.maxA).arg(ss.minA)
                                .arg(ss.avgD).arg(ss.maxD).arg(ss.minD).arg(ss.avgR).arg(ss.realD)
                                .arg(ss.d10).arg(ss.d50).arg(ss.d90).arg(ss.span));

    cv::Mat result=m_currentImg.clone();
    cv::drawContours(result,ss.contours,-1,cv::Scalar(0,255,0),2);

    cv::Mat rgbResult;
    cv::cvtColor(result,rgbResult,cv::COLOR_BGR2RGB);
    QImage qres(rgbResult.data,rgbResult.cols,rgbResult.rows,rgbResult.step,QImage::Format_RGB888);


    picture1->setPixmap(QPixmap::fromImage(qres.copy()));

    m_busy=false;
    m_analyzeButton->setDisabled(false);
    m_analyzeButton->setText("Analyze");
}

void MainWindow::onAnalyzeClick()
{
    if(m_currentImg.empty()) return;
    if(m_busy) return;

    m_busy=true;
    m_analyzeButton->setDisabled(true);
    m_analyzeButton->setText("分析中...");
    AnalyzeParams p = collectParams();   // 只取一次
    m_lastParams = p;                    // 存快照
    emit startAnalyze(m_currentImg, m_ratio, p);   // 发同一份
}

void MainWindow::onExportCsv()
{
    if(m_lastResult.curve.empty()){
        QMessageBox::warning(this,"提示","请先分析一张图片");
        return;
    }

    QString path = QFileDialog::getSaveFileName(this,"导出检测报告","","CSV 文件(*.csv)");
    if(path.isEmpty()) return;

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) return;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "\xEF\xBB\xBF";     // BOM:让 Excel 认 UTF-8

    // ① 参数行(用 m_lastParams)
    out << QString("# 参数: threshold=%1, morphType=%2, kernelSize=%3, minArea=%4, ratio=%5\n")
               .arg(m_lastParams.threshold).arg(m_lastParams.morphType)
               .arg(m_lastParams.kernelSize).arg(m_lastParams.minArea)
               .arg(m_ratio, 0, 'f', 3);

    // ② 统计行(用 m_lastResult)
    out << QString("# 统计: count=%1, D10=%2, D50=%3, D90=%4, span=%5\n")
               .arg(m_lastResult.count)
               .arg(m_lastResult.d10, 0, 'f', 2).arg(m_lastResult.d50, 0, 'f', 2)
               .arg(m_lastResult.d90, 0, 'f', 2).arg(m_lastResult.span, 0, 'f', 3);

    // ③ 21 点表
    out << "percentile,size_um\n";
    for(int i = 0; i < (int)m_lastResult.curve.size(); i++){
        out << (i * 5) << "," << m_lastResult.curve[i] << "\n"; // 第3行:写一行"百分位,粒径"
    }

    file.close();
    QMessageBox::information(this,"完成","已导出 CSV");
}




void Worker::doAnalyze(const cv::Mat &img, double ratio,const AnalyzeParams& params)
{
    Analyzer ana;
    emit analyzeDone(ana.analyze(img,ratio,params));
}
