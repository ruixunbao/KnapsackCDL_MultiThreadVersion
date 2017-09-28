#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "compass.h"
#include "motor.h"
#include "motorthread.h"
#include "adq214.h"
#include "global_defines.h"
#include "settingfile.h"
#include <parametersetdialog.h>.h>

#include <QDebug>
#include "display/wind_display.h"
#include <QLabel>
#include <QTimer>
#include <QWidget>
#include "mainwindow.h"
#include <QToolBar>
#include <QAction>
#include <QMouseEvent>
#include <QKeyEvent>
#include <usertoolbar.h>
#include <admintoolbar.h>
#include <laserpulse.h>
#include <laserseed.h>
#include <dispsettings.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int H_low;
    int H_high;

    double H_speed[Max_nLayers];
    double H_direction[Max_nLayers];
    double V_speed[Max_nLayers];
    double Height_values[Max_nLayers];

signals:
    void size_changed();                    // 用于调整实时风速显示的widget高度

private slots:
    // GUI界面相关
    void toolBarControlTimerOutFcn();
    void UpdateHeightsValue();

    void actionStartTriggered();
    void actionQuitTriggered();
    void actionSetTriggered();

private:
    Ui::MainWindow *ui;
    // GUI界面
    UserToolBar *userToolBar;
    AdminToolBar *adminToolBar;
    bool isUserToolBarShowed;
    bool isAdminToolBarShowed;
    void showToolBar(bool isUserToolBarShowed, bool isAdminToolBarShowed);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    QTimer *toolBarControlTimer;
    QTimer *doubleAltKeyPressedClassifyTimer;

    //参数配置
    SOFTWARESETTINGS mysetting;
    SettingFile *mysettingfile;

    // 数据显示
    wind_display *speedDispWidget;
    void resizeEvent(QResizeEvent * event);
};

#endif // MAINWINDOW_H
