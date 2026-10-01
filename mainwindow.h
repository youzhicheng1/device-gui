#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QToolBar>
#include <QTimer>
#include <QDateTime>
#include <QKeyEvent>

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

private slots:
    void on_AddButton_clicked();

    void on_DelButton_clicked();

    void on_ClearButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    int device_count=0;
    QStatusBar* stsBar=nullptr;
    QMenuBar* mnBar=nullptr;
    QToolBar* toolBar=nullptr;
};
#endif // MAINWINDOW_H
