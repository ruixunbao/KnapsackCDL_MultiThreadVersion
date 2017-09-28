#ifndef COMPASSTHREAD_H
#define COMPASSTHREAD_H
#include<QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QtSerialPort/QSerialPort>

class CompassThread : public QThread
{
    Q_OBJECT
public:
    explicit CompassThread(QObject *parent = nullptr);
    ~CompassThread();
    void transaction(const QString &portName, const QByteArray &request);
    void run() Q_DECL_OVERRIDE;
signals:
    void response(const QByteArray &s);
    void error(const QString &s);
    void timeout(const QString &s);
private:
    QString portName;
    QByteArray request;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
};

#endif // COMPASSTHREAD_H
