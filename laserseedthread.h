#ifndef LASERSEEDTHREAD_H
#define LASERSEEDTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QByteArray>

class LaserSeedThread : public QThread
{
    Q_OBJECT
public:
    LaserSeedThread(QObject *parent = 0);
    ~LaserSeedThread();

    void transaction(const QString &portName, const QByteArray &request);
    void run();

signals:
    void seed_PortNotOpen();
    void responseSeed(const QString &s);
    void timeoutSeed();


private:
    QString portName;
    QByteArray request;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
};
#endif // LASERSEEDTHREAD_H
