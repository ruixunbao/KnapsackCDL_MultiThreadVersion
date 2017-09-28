#include "laserPulse.h"
#include <Qdebug.h>

LaserPulse::LaserPulse(QObject *parent) : QObject(parent)
{
    connect(&Laserpulsethread,SIGNAL(responsePulse(QString)),this,SLOT(receive_response(QString)));
    connect(&Laserpulsethread,SIGNAL(Pulse_PortNotOpen()),this,SLOT(portError()));
    connect(&Laserpulsethread,SIGNAL(timeoutPulse()),this,SLOT(timeout()));

    laserPort="COM4";
    powerSet=true;
    fire=false;
    close=false;
}

void LaserPulse::beginPulseLaser()
{
    StringToHex("AA 55 C1 01 01 01 00",senddata);
    Laserpulsethread.transaction(laserPort,senddata);
    fire=true;
}

void LaserPulse::setPulsePower(const int &s)
{
    QString key =QString("%1").arg(s,4,16,QLatin1Char('0')).toUpper();
    bool ok;
    int aa=key.left(2).toInt(&ok,16)+key.right(2).toInt(&ok,16);
    QString key2 =QString("%1").arg(aa,4,16,QLatin1Char('0')).toUpper();
    QString power="AA 55 C3 02 "+key.right(2)+" "+key.left(2)+" "+key2.right(2)+" "+key2.left(2);
    StringToHex(power,senddata);
    Laserpulsethread.transaction(laserPort,senddata);
    powerSet=false;
}

void LaserPulse::closePulseLaser()
{
    StringToHex("AA 55 C1 01 00 00 00",senddata);
    Laserpulsethread.transaction(laserPort,senddata);
    close=true;
}

void LaserPulse::receive_response(const QString &temp)
{
    if(!powerSet)
    {
        QString powerAnswer=temp.mid(8,2);
        if(powerAnswer!="00")
        {
            errorCode="���弤�����������ô���";
            emit this->laserPulseError(errorCode);
        }
        else
        {
            powerSet=true;
            emit this->laserWorkRight();
            qDebug()<<"���弤�����������óɹ�";
        }
    }
    else
    {
        if(fire||close)
        {
            if(fire)
            {
                if(temp=="55aac101000000")
                {
                    setPulsePower(1000); //������
                    qDebug()<<"���弤�����򿪳ɹ�";
                }
                else
                {
                    errorCode="���弤�������쳣";
                    emit this->laserPulseError(errorCode);
                }
                fire=false;
            }
            else
            {
                if(temp=="55aac101000000")
                {
                    emit this->pulseCloseReady();
                    qDebug()<<"���弤�����ر�����";
                }
                else
                {
                    errorCode="���弤�����ر��쳣";
                    emit this->laserPulseError(errorCode);
                }
                close=false;
            }

        }
        else
        {
            QString checkAnswer=temp.mid(8,2);
            if(checkAnswer=="00")
            {
                errorCode="���弤�������쳣";
                emit this->laserPulseError(errorCode);
            }
            else
            {
                QString checkAnswer=temp.mid(10,2);
                if(checkAnswer!="00")
                {
                    bool ok;
                    int s=checkAnswer.right(1).toInt(&ok,16);
                    QString key =QString("%1").arg(s,4,2,QLatin1Char('0'));
                    if(key.left(1)=="1")
                    {errorCode.append("�����¶��쳣;");}
                    if(key.right(1)=="1")
                    {errorCode.append("ģ���¶��쳣;");}
                    if(key.mid(1,1)=="1")
                    {errorCode.append("���빦���쳣;");}
                    if(key.mid(2,1)=="1")
                    {errorCode.append("�������弤�����¶��쳣;");}
                    emit this->laserPulseError(errorCode);
                }
                else
                {
                    qDebug()<<"���弤������������";
                }

            }
        }
    }
}

void LaserPulse::checkLaser()
{
    StringToHex("AA 55 D3 00 00 00",senddata);
    Laserpulsethread.transaction(laserPort,senddata);
}

char LaserPulse::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

void LaserPulse::StringToHex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

void LaserPulse::portError()
{
    errorCode="���弤�������ڴ��쳣";
    emit this->laserPulseError(errorCode);
}

void LaserPulse::timeout()
{
    errorCode="���弤�����������ݶ�ȡ�쳣";
    emit this->laserPulseError(errorCode);
}
