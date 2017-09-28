#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QWidget>
#include <QLabel>
#include <QFont>
#include <QString>
#include <QGridLayout>
#include "display/wind_display.h"
#include "display/rt_display.h"
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //读取配置文件
    mysettingfile = new SettingFile();
    mysetting = mysettingfile->readSettings();
    qDebug() << mysetting.angleNum;

    //工具栏配置
    userToolBar = new UserToolBar();
    addToolBar(Qt::TopToolBarArea, userToolBar);
    connect(userToolBar->startAction, &QAction::triggered, this, &MainWindow::actionStartTriggered);
    connect(userToolBar->quitAction, &QAction::triggered, this, &MainWindow::actionQuitTriggered);
    adminToolBar = new AdminToolBar();
    addToolBar(Qt::TopToolBarArea, adminToolBar);
    connect(adminToolBar->setAction, &QAction::triggered, this, &MainWindow::actionSetTriggered);
    isUserToolBarShowed = false;
    isAdminToolBarShowed = false;
    showToolBar(isUserToolBarShowed, isAdminToolBarShowed);

    toolBarControlTimer = new QTimer(this);
    toolBarControlTimer->setSingleShot(true);
    toolBarControlTimer->setInterval(1000);
    connect(toolBarControlTimer, &QTimer::timeout, this, &MainWindow::toolBarControlTimerOutFcn);
    doubleAltKeyPressedClassifyTimer = new QTimer(this);
    doubleAltKeyPressedClassifyTimer->setSingleShot(true);

    //显示部分
    speedDispWidget = new wind_display(this);
    speedDispWidget->set_nLayers(mysetting.nRangeBin);
    UpdateHeightsValue();

    ui->gridLayout->addWidget(speedDispWidget);

    connect(this, &MainWindow::size_changed,speedDispWidget, &wind_display::setSubSize);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete speedDispWidget;
    delete userToolBar;
    delete adminToolBar;
    toolBarControlTimer->stop();
    toolBarControlTimer->deleteLater();
    doubleAltKeyPressedClassifyTimer->stop();
    doubleAltKeyPressedClassifyTimer->deleteLater();
    delete mysettingfile;
}

void MainWindow::toolBarControlTimerOutFcn()
{
    isUserToolBarShowed = false;
    showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
}

void MainWindow::showToolBar(bool isUserToolBarShowed, bool isAdminToolBarShowed)
{
    userToolBar->setVisible(isUserToolBarShowed);
    adminToolBar->setVisible(isAdminToolBarShowed);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!isAdminToolBarShowed) {
        if (event->button() == Qt::RightButton){
            if (isUserToolBarShowed) {
                isUserToolBarShowed = false;
                showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
                toolBarControlTimer->stop();
            }
            else {
                isUserToolBarShowed = true;
                showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
                toolBarControlTimer->stop();
                toolBarControlTimer->start(3000);
            }
        }
        else if(event->button() == Qt::LeftButton) {
            isUserToolBarShowed = false;
            showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
            toolBarControlTimer->stop();
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt) {
        if (doubleAltKeyPressedClassifyTimer->isActive()) {
            if (isAdminToolBarShowed) {
                isAdminToolBarShowed = false;
                isUserToolBarShowed = false;
                showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
            }
            else {
                toolBarControlTimer->stop();
                isAdminToolBarShowed = true;
                isUserToolBarShowed = true;
                showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
            }
        }
        else {
            doubleAltKeyPressedClassifyTimer->start(300);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);
    emit size_changed();
}

void MainWindow::UpdateHeightsValue()           //
{
    double range_resolution = 300/mysetting.sampleFreq/2*mysetting.nPointsPerBin;
    for(int i=0;i<mysetting.nRangeBin;i++) {
        Height_values[i] = (i+1.5)*range_resolution;
    }
    speedDispWidget->setHeights(Height_values);
}

void MainWindow::actionStartTriggered()
{

}

void MainWindow::actionQuitTriggered()
{

}

void MainWindow::actionSetTriggered()
{
//    DispSettings disp(mysetting);
    ParameterSetDialog *parameterSetDialog = new ParameterSetDialog(this);
    parameterSetDialog->setParaSettings(mysetting);
    if (parameterSetDialog->exec() == QDialog::Accepted) {
        mysetting =  parameterSetDialog->getParaSettings();
        speedDispWidget->set_nLayers(mysetting.nRangeBin);
        UpdateHeightsValue();
    }
    delete parameterSetDialog;
}
