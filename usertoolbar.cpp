#include "usertoolbar.h"

UserToolBar::UserToolBar()
{
    startAction = this->addAction(QString::fromLocal8Bit("��ʼ"));
    quitAction = this->addAction(QString::fromLocal8Bit("�˳�"));

    setIconSize(QSize(48, 48));
    startAction->setIcon(QIcon(":/images/Start.png"));
    quitAction->setIcon(QIcon(":/images/Standby.png"));
    setMovable(false);
    startIcon = true;       // ��ǰͼ�꣬start
    connect(startAction,&QAction::triggered,this, &UserToolBar::changeStartIcon);
}

void UserToolBar::changeStartIcon()
{
    if(startIcon)
    {
        startAction->setIcon(QIcon(":/images/Stop.png"));
        startIcon = false;
    }
    else
    {
        startAction->setIcon(QIcon(":/images/Start.png"));
        startIcon = true;
    }
}

void UserToolBar::set_to_stopped()      //�趨Ϊֹͣ״̬����ͼ��Ϊ���Կ�ʼ
{
    startAction->setIcon(QIcon(":/images/Start.png"));
    startIcon = true;
}
