#ifndef DEVICESCONTROLTHREAD_H
#define DEVICESCONTROLTHREAD_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>
#include <global_defines.h>

class DevicesControlThread : public QThread
{
    Q_OBJECT
public:
    DevicesControlThread(QObject *parent = 0);
    ~DevicesControlThread();

    void startWork(SOFTWARESETTINGS settings);

protected:
    void run() Q_DECL_OVERRIDE;

signals:
    void errorInfo(QString *errorCode);
};

#endif // DEVICESCONTROLTHREAD_H
