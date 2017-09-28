#include "compass.h"
#include <QDebug>

Compass::Compass(QObject *parent) : QObject(parent)
{
    connect(&compassThread, &CompassThread::response, this, &Compass::showResponse);
    connect(&compassThread, &CompassThread::error, this, &Compass::processError);
    connect(&compassThread, &CompassThread::timeout, this, &Compass::processError);
}

void Compass::read()
{
    //Ҫ���͵�����
    QString str = "6804000408";
    stringToHex(str,sendData);
    compassThread.transaction("COM8",sendData);
}

void Compass::stringToHex(QString str, QByteArray &sendData)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    sendData.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr = str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = charToHex(hstr);
        lowhexdata = charToHex(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        sendData[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    sendData.resize(hexdatalen);
}

char Compass::charToHex(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

double Compass::stringToAngle(const QString &c)
{
    double answer;
    QString sign = c.mid(0,1);                    //��ȡ����λ������λ��С��λ
    QString round1 = c.mid(1,1);
    QString round2 = c.mid(2,2);
    QString decimal = c.mid(4,2);
    answer = 100*round1.toDouble()+round2.toDouble()+0.01*decimal.toDouble(); //��������Ϊʵ�ʽǶ�
    if(sign.toInt() == 1)
    {
        answer = -1*answer;               //����λΪ1ʱȡ��
    }
    return answer;
}

void Compass::showResponse(const QByteArray &s)
{
    double angle;
    QString temp = s.toHex();             //�����ź�ת16����

    QString a,b,c;
    a = temp.mid(8,6);                     //��ȡ�����ź� pitch roll head
    b = temp.mid(14,6);
    c = temp.mid(20,6);

    angle = stringToAngle(c);
    emit this->compassAngleReady(angle);
}

void Compass::processError(const QString &s)
{
    qDebug()<<"compass error:"<<s;
}


