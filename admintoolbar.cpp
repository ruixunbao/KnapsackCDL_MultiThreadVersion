#include "admintoolbar.h"

AdminToolBar::AdminToolBar()
{
    setIconSize(QSize(48, 48));

    setAction = this->addAction(QString::fromLocal8Bit("�ɼ���"));
    setAction->setIcon(QIcon(":/images/Gear.png"));

    setMovable(false);
}
