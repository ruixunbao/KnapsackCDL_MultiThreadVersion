#include "compassthread.h"
#include <QtSerialPort/QSerialPort>

#include <QTime>

#include <QDebug>

QT_USE_NAMESPACE

CompassThread::CompassThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
}

CompassThread::~CompassThread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void CompassThread::transaction(const QString &portName, const QByteArray &request)
{
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->request = request;
    if (!isRunning())
    {
        start();
    }
    else
    {
        cond.wakeOne();
    }
}

void CompassThread::run()
{
    bool currentPortNameChanged = false;
    mutex.lock();
    QString currentPortName;
    if (currentPortName != portName)
    {
        currentPortName = portName;
        currentPortNameChanged = true;
    }
    int currentWaitTimeout = 1000;
    waitTimeout=3000;
    QByteArray currentRequest = request;
    mutex.unlock();
    QSerialPort serial;
    while (!quit)
    {
        if (currentPortNameChanged)
        {
            serial.close();
            serial.setPortName(currentPortName);
            if (!serial.open(QIODevice::ReadWrite))
            {
                emit error(tr("Can't open %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }
        }
        QByteArray requestData = currentRequest;
        serial.write(requestData);

        if (serial.waitForBytesWritten(waitTimeout))                        //���ֽ�д��ʱ�Ϳ�����ʱ����ֹ������
        {
            if (serial.waitForReadyRead(currentWaitTimeout))                //���źſɶ�ʱ�Ϳ�����ʱ����ֹ������
            {
                QByteArray responseData = serial.readAll();                  //�ȶ�ȡһ��
                while (serial.waitForReadyRead(30))                          // �����20s�ڻ��пɶ����ݾͼ�����ȡ
                {
                    responseData += serial.readAll();                        //����ȡ�ź�ƴ��Ϊ�����Ľ����ź�
                }
                emit this->response(responseData);
            }
            else
            {
                emit timeout(tr("Wait read response timeout %1")
                             .arg(QTime::currentTime().toString()));
            }

        }
        else
        {
            emit timeout(tr("Wait write request timeout %1")
                         .arg(QTime::currentTime().toString()));
        }

        mutex.lock();
        cond.wait(&mutex);
        if (currentPortName != portName)
        {
            currentPortName = portName;
            currentPortNameChanged = true;
        }
        else
        {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = waitTimeout;
        currentRequest = request;
        mutex.unlock();
    }
}
