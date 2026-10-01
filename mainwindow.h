#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QToolBar>
#include <QTimer>
#include <QDateTime>
#include <QKeyEvent>
#include <QStackedWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <QThread>
#include "Analyzer.h"

class Worker;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void updateStatus();
    QString timeupdate();
    void keyPressEvent(QKeyEvent* event) override;
    void convertPicture(cv::Mat img);

private slots:
    void on_AddButton_clicked();

    void on_DelButton_clicked();

    void on_ClearButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void SaveDevice();
    void OpenDevice();
    void OpenPicture();
    void onAnalyzeDone(const statistics& ss);
private:
    Ui::MainWindow *ui;
    int device_count=0;
    QStatusBar* stsBar=nullptr;
    QMenuBar* mnBar=nullptr;
    QToolBar* toolBar=nullptr;
    QStackedWidget *stackPages=nullptr;
    QLabel* picture=nullptr;
    QLabel* picture1=nullptr;
    Analyzer* aa=nullptr;
    Worker* worker=nullptr;
    QThread* thread=nullptr;
    cv::Mat m_currentImg;//当前图
signals:
    void startAnalyze(const cv::Mat& img,double ratio);
};

class Worker:public QObject{
    Q_OBJECT
public slots:
    void doAnalyze(const cv::Mat& img, double ratio);
signals:
    void analyzeDone(const statistics& ss);
};

#endif // MAINWINDOW_H
