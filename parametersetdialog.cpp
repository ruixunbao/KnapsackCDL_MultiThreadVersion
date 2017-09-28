#include "parametersetdialog.h"
#include "ui_parametersetdialog.h"
#include "mainwindow.h"
#include <QRegExp>
#include <QRegExpValidator>

ParameterSetDialog::ParameterSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterSetDialog)
{
    ui->setupUi(this);
    paraSettingFile = new SettingFile();
    //laser Parameters
    QRegExp laserPulseEnergyRegExp("^([1-9][0-9])([.]{1}[0-9]{1,3}){0,1}|"
                                   "([1][0][0])([.]{1}[0]{1,3}){0,1}$");     //���� 10-100
    QRegExp laserLocalPowerRegExp("^([1-3])([.]{1}[0-9][0-9][0-9]){0,1}|"
                                  "[4]([.]{1}[0][0][0]){0,1}$");             //���� 1-4
    QRegExp laserRPFRegExp("^([1-9][0-9]*)([.]{1}[0-9]{1,3}){0,1}$");           //Ƶ�� ��0
    QRegExp laserPulseWidthRegExp("^([1-9][0-9]*)([.]{1}[0-9]{1,3}){0,1}$");        //������ ��0
    QRegExp laserWaveLengthRegExp("^([1-9][0-9]*)([.]{1}[0-9]{1,3}){0,1}$");        //���� ��0
    QRegExp laserAOMFreqRegExp("^([1-9][0-9]*)([.]{1}[0-9]{1,3}){0,1}$");         //AOM ��0
    //Scan Parameters
    QRegExp elevationAngleRegExp("^([0-8]?[0-9])([.]{1}[0-9]{0,3}){0,1}|"
                                 "([9][0])([.]{1}[0]{1,3}){0,1}$");         //������ 0-90
    QRegExp azAngleStartRegExp("^([1-9]?[0-9])([.]{1}[0-9]{1,3}){0,1}|"
                               "([1][0,1][0-9])([.]{1}[0-9]{1,3}){0,1}|"
                               "([1][2][0])([.]{1}[0]{1,3}){0,1}$");       //��ʼ�� 0-120
    QRegExp azAngleStepRegExp("^([1-9]?[0-9])([.]{1}[0-9]{1,3}){0,1}|"
                              "([1][0,1][0-9])([.]{1}[0-9]{1,3}){0,1}|"
                              "([1][2][0])([.]{1}[0]{1,3}){0,1}$");        //������ 0-120
    QRegExp angleNumRegExp("^[1-9]?[0-9]*$");               //������ ��0 int
    QRegExp circleNumRegExp("^([1-9][0-9]*)([.]{1}[0-9]{1,3}){0,1}$");       //Բ���� ��0
    QRegExp motorSpeedRegExp("^([3-8][0-9])([.]{1}[0-9]{1,3}){0,1}|"
                             "90([.]{1}[0]{1,3}){0,1}$");                    //�������ٶ� 30-90
    QRegExp intervalTimeRegExp("^([1-9][0-9]*)([.]{1}[0-9]{1,3}){0,1}$");                                         //��ʱ̽���� ��0
    QRegExp groupTimeRegExp("^([1-9][0-9]*)([.]{1}[0-9]{1,3}){0,1}$");                                            //��ʱ̽�ⵥ��ʱ�� ��0
    //Sample Parameters
    QRegExp triggerLevelRegExp("^([0]|[1-7]?[0-9]?[0-9]?[0-9]|"
                               "[8][0-1][0-9][0-9]|[8][0-1][9][0-2])$");       //������ƽ 0-8192 int
    QRegExp nPointsPreTriggerRegExp("^([0]|[1-9]?[0-9]?[0-9]|[1][0][0-1][0-9]|"
                                    "[1][0][2][0-4])$");                 //Ԥ�������� 0-1024 int
    //Real Time Process
    QRegExp nPulsesAccRegExp("^(20000|[1-9][0-9][0-9][0-9]|[1][0-9][0-9][0-9][0-9])$");    //�ۼ������� 1000-20000 int
    QRegExp nRangeBinRegExp("^([1][0-9]|20)$");       //�������� 10-20 int
    QRegExp nPointsPerBinRegExp("^([1-4][0-9][0,2,4,6,8]|500)$");        //�������ڵ��� 100-500 int
    QRegExp nPointsMirrorWidthRegExp("^([0]|[1-9]?[0-9]?[0-9]|[1][0][0-1][0-9]|[1][0][2][0-4])$");//�����ȵ��� 0-1024 int
    //Vector Velocity Inversion
    QRegExp nPointsObjFreqRegExp("^(([5-9])([.]{1}[0-9]{1,3}){0,1}|([1-2][0-9])([.]{1}[0-9]{1,3}){0,1}|"
                                "([3][0])([.]{1}[0]{1,3}){0,1})$");                //������ٷ�Χ 5-30
    QRegExp nDirsVectorCalRegExp("^[3-9]|[1][0-9]|[2][0-4]$");          //ʸ����������ľ���������� 3-24
    //File Store
    QRegExp nDirsPerFileRegExp("^([1-9][0-9][0-9]|[1][0-9][0-9][0-9]|2000)$");    //���ļ��ڵķ����� 100-2000 int

    //laser
    laserPulseEnergyRegExpV = new QRegExpValidator(laserPulseEnergyRegExp,this);
    laserLocalPowerRegExpV =  new QRegExpValidator(laserLocalPowerRegExp,this);
    laserRPFRegExpV = new QRegExpValidator(laserRPFRegExp,this);
    laserPulseWidthRegExpV = new QRegExpValidator(laserPulseWidthRegExp,this);
    laserWaveLengthRegExpV = new QRegExpValidator(laserWaveLengthRegExp,this);
    laserAOMFreqRegExpV = new QRegExpValidator(laserAOMFreqRegExp,this);
    //Scan Parameters
    elevationAngleRegExpV = new QRegExpValidator(elevationAngleRegExp,this);
    azAngleStartRegExpV = new QRegExpValidator(azAngleStartRegExp,this);
    azAngleStepRegExpV = new QRegExpValidator(azAngleStepRegExp,this);
    angleNumRegExpV = new QRegExpValidator(angleNumRegExp,this);
    circleNumRegExpV = new QRegExpValidator(circleNumRegExp,this);
    motorSpeedRegExpV = new QRegExpValidator(motorSpeedRegExp,this);
    intervalTimeRegExpV = new QRegExpValidator(intervalTimeRegExp,this);
    groupTimeRegExpV = new QRegExpValidator(groupTimeRegExp,this);
    //Sample Parameters
    triggerLevelRegExpV = new QRegExpValidator(triggerLevelRegExp,this);
    nPointsPreTriggerRegExpV = new QRegExpValidator(nPointsPreTriggerRegExp,this);
    //Real Time Process
    nPulsesAccRegExpV = new QRegExpValidator(nPulsesAccRegExp,this);
    nRangeBinRegExpV = new QRegExpValidator(nRangeBinRegExp,this);
    nPointsPerBinRegExpV = new QRegExpValidator(nPointsPerBinRegExp,this);
    nPointsMirrorWidthRegExpV = new QRegExpValidator(nPointsMirrorWidthRegExp,this);
    //Vector Velocity Inversion
    nPointsObjFreqRegExpV = new QRegExpValidator(nPointsObjFreqRegExp,this);
    nDirsVectorCalRegExpV = new QRegExpValidator(nDirsVectorCalRegExp,this);
    //File Store
    nDirsPerFileRegExpV = new QRegExpValidator(nDirsPerFileRegExp,this);

    ui->laserPulseEnergyLineEdit->setValidator(laserPulseEnergyRegExpV);
    ui->laserLocalPowerLineEdit->setValidator(laserLocalPowerRegExpV);
    ui->laserRPFLineEdit->setValidator(laserRPFRegExpV);
    ui->laserPulseWidthLineEdit->setValidator(laserPulseWidthRegExpV);
    ui->laserWaveLengthLineEdit->setValidator(laserWaveLengthRegExpV);
    ui->laserAOMFreqLineEdit->setValidator(laserAOMFreqRegExpV);

    ui->elevationAngleLineEdit->setValidator(elevationAngleRegExpV);
    ui->azAngleStartLineEdit->setValidator(azAngleStartRegExpV);
    ui->azAngleStepLineEdit->setValidator(azAngleStepRegExpV);
    ui->angleNumLineEdit->setValidator(angleNumRegExpV);
    ui->circleNumLineEdit->setValidator(circleNumRegExpV);
    ui->motorSpeedLineEdit->setValidator(motorSpeedRegExpV);
    ui->intervalTimeLineEdit->setValidator(intervalTimeRegExpV);
    ui->groupTimeLineEdit->setValidator(groupTimeRegExpV);

    ui->triggerLevelLineEdit->setValidator(triggerLevelRegExpV);
    ui->nPointsPretriggerLineEdit->setValidator(nPointsPreTriggerRegExpV);

    ui->nPulsesAccLineEdit->setValidator(nPulsesAccRegExpV);
    ui->nRangeBinLineEdit->setValidator(nRangeBinRegExpV);
    ui->nPointsPerBinLineEdit->setValidator(nPointsPerBinRegExpV);
    ui->nPointsMirrorWidthLineEdit->setValidator(nPointsMirrorWidthRegExpV);
    ui->nPointsObjFreqLineEdit->setValidator(nPointsObjFreqRegExpV);
    ui->nDirsVectorCalLineEdit->setValidator(nDirsVectorCalRegExpV);
    ui->nDirsPerFileLineEdit->setValidator(nDirsPerFileRegExpV);
}

ParameterSetDialog::~ParameterSetDialog()
{
    delete paraSettingFile;
    delete ui;
    //laser Parameters
    delete laserPulseEnergyRegExpV;
    delete laserLocalPowerRegExpV;
    delete laserRPFRegExpV;
    delete laserPulseWidthRegExpV;
    delete laserWaveLengthRegExpV;
    delete laserAOMFreqRegExpV;
    //Scan Parameters
    delete elevationAngleRegExpV;
    delete azAngleStartRegExpV;
    delete azAngleStepRegExpV;
    delete angleNumRegExpV;
    delete circleNumRegExpV;
    delete motorSpeedRegExpV;
    delete intervalTimeRegExpV;
    delete groupTimeRegExpV;
    //Sample Parameters
    delete triggerLevelRegExpV;
    delete nPointsPreTriggerRegExpV;
    //Real Time Process
    delete nPulsesAccRegExpV;
    delete nRangeBinRegExpV;
    delete nPointsPerBinRegExpV;
    delete nPointsMirrorWidthRegExpV;
    //Vector Velocity Inversion
    delete nPointsObjFreqRegExpV;
    delete nDirsVectorCalRegExpV;
    //File Store
    delete nDirsPerFileRegExpV;
}

SOFTWARESETTINGS ParameterSetDialog::getParaSettings()
{
    paraSettings.laserPulseEnergy = ui->laserPulseEnergyLineEdit->text().toDouble();
    paraSettings.laserLocalPower = ui->laserLocalPowerLineEdit->text().toDouble();
    paraSettings.laserAOMFreq = ui->laserAOMFreqLineEdit->text().toDouble();
    paraSettings.laserPulseWidth = ui->laserPulseWidthLineEdit->text().toDouble();
    paraSettings.laserRPF = ui->laserRPFLineEdit->text().toDouble();
    paraSettings.laserWaveLength = ui->laserWaveLengthLineEdit->text().toDouble();

    paraSettings.detectMode = ui->detectModeComboBox->currentIndex();
    paraSettings.elevationAngle = ui->elevationAngleLineEdit->text().toDouble();
    paraSettings.azAngleStart = ui->azAngleStartLineEdit->text().toDouble();
    paraSettings.azAngleStep = ui->azAngleStepLineEdit->text().toDouble();
    paraSettings.angleNum = ui->angleNumLineEdit->text().toUInt();
    paraSettings.circleNum = ui->circleNumLineEdit->text().toDouble();
    paraSettings.isAngleChecked = ui->anglekeyRadioButton->isChecked();
    paraSettings.isCircleChecked = ui->circlekeyRadioButton->isChecked();
    paraSettings.motorSpeed = ui->motorSpeedLineEdit->text().toDouble();
    paraSettings.intervalTime = ui->intervalTimeLineEdit->text().toDouble();
    paraSettings.groupTime = ui->groupTimeLineEdit->text().toDouble();

    paraSettings.sampleFreq = ui->sampleFreqComboBox->currentText().toDouble();
    paraSettings.triggerLevel = ui->triggerLevelLineEdit->text().toUInt();
    paraSettings.nPointsPreTrigger = ui->nPointsPretriggerLineEdit->text().toUInt();
    paraSettings.nPulsesAcc = ui->nPulsesAccLineEdit->text().toUInt();
    paraSettings.nRangeBin = ui->nRangeBinLineEdit->text().toUInt();
    paraSettings.nPointsPerBin = ui->nPointsPerBinLineEdit->text().toUInt();
    paraSettings.nPointsMirrorWidth = ui->nPointsMirrorWidthLineEdit->text().toUInt();
    paraSettings.overlapRatio = ui->overlapRatioComboBox->currentText().toDouble();

    paraSettings.nPointsObjFreq = getObjFreqPoints();
    paraSettings.nDirsVectorCal = ui->nDirsVectorCalLineEdit->text().toUInt();

    paraSettings.dataFilePath = ui->dataFilePathLineEdit->text();
    paraSettings.autoCreateDateDir = ui->autoCreateDateDirCheckBox->isChecked();
    paraSettings.nDirsPerFile = ui->nDirsPerFileLineEdit->text().toUInt();

    return paraSettings;
}

void ParameterSetDialog::setParaSettings(SOFTWARESETTINGS settings)
{
    paraSettings = settings;
    refreshDisp();
}

void ParameterSetDialog::refreshDisp()
{
    // laser parameter refresh display on textchanged
    ui->laserPulseEnergyLineEdit->setText(QString::number(paraSettings.laserPulseEnergy));
    ui->laserLocalPowerLineEdit->setText(QString::number(paraSettings.laserLocalPower));
    ui->laserAOMFreqLineEdit->setText(QString::number(paraSettings.laserAOMFreq));
    ui->laserPulseWidthLineEdit->setText(QString::number(paraSettings.laserPulseWidth));
    ui->laserRPFLineEdit->setText(QString::number(paraSettings.laserRPF));
    ui->laserWaveLengthLineEdit->setText(QString::number(paraSettings.laserWaveLength));

    // scan parameter refresh display on text changed, classified by detect mode
    ui->detectModeComboBox->setCurrentIndex(paraSettings.detectMode);
    if (paraSettings.detectMode == 0) {
        // single group detection
        ui->anglekeyRadioButton->setEnabled(true);
        ui->circlekeyRadioButton->setEnabled(true);
        ui->angleNumLineEdit->setEnabled(paraSettings.isAngleChecked);
        ui->circleNumLineEdit->setEnabled(paraSettings.isCircleChecked);
        ui->intervalTimeLineEdit->setEnabled(false);
        ui->groupTimeLineEdit->setEnabled(false);

        ui->angleNumLineEdit->setText(QString::number(paraSettings.angleNum));
        ui->circleNumLineEdit->setText(QString::number(paraSettings.circleNum));
        ui->anglekeyRadioButton->setChecked(paraSettings.isAngleChecked);
        ui->circlekeyRadioButton->setChecked(paraSettings.isCircleChecked);
    }
    else if (paraSettings.detectMode == 1) {
        // continous detection
        ui->anglekeyRadioButton->setEnabled(false);
        ui->angleNumLineEdit->setEnabled(false);
        ui->circlekeyRadioButton->setEnabled(false);
        ui->circleNumLineEdit->setEnabled(false);
        ui->intervalTimeLineEdit->setEnabled(false);
        ui->groupTimeLineEdit->setEnabled(false);

        ui->angleNumLineEdit->setText(QString::fromLocal8Bit("������"));
        ui->circleNumLineEdit->setText(QString::fromLocal8Bit("������"));
        ui->anglekeyRadioButton->setChecked(false);
        ui->circlekeyRadioButton->setChecked(false);
    }
    else if (paraSettings.detectMode == 2) {
        // scheduled detection
        ui->anglekeyRadioButton->setEnabled(false);
        ui->angleNumLineEdit->setEnabled(false);
        ui->circlekeyRadioButton->setEnabled(false);
        ui->circleNumLineEdit->setEnabled(false);
        ui->intervalTimeLineEdit->setEnabled(true);
        ui->groupTimeLineEdit->setEnabled(true);

        ui->angleNumLineEdit->setText(QString::fromLocal8Bit("������"));
        ui->circleNumLineEdit->setText(QString::fromLocal8Bit("������"));
        ui->anglekeyRadioButton->setChecked(false);
        ui->circlekeyRadioButton->setChecked(false);
    }

    ui->elevationAngleLineEdit->setText(QString::number(paraSettings.elevationAngle));
    ui->azAngleStartLineEdit->setText(QString::number(paraSettings.azAngleStart));
    ui->azAngleStepLineEdit->setText(QString::number(paraSettings.azAngleStep));

    ui->motorSpeedLineEdit->setText(QString::number(paraSettings.motorSpeed));
    ui->intervalTimeLineEdit->setText(QString::number(paraSettings.intervalTime));
    ui->groupTimeLineEdit->setText(QString::number(paraSettings.groupTime));

    ui->sampleFreqComboBox->setCurrentText(QString::number(paraSettings.sampleFreq));
    ui->triggerLevelLineEdit->setText(QString::number(paraSettings.triggerLevel));
    QString voltageLevelStr = "=" + QString::number(getVoltageTriggerLevel(), 'f', 3) + "V";
    ui->triggerLevelInVoltageDispLabel->setText(voltageLevelStr);
    ui->nPointsPretriggerLineEdit->setText(QString::number(paraSettings.nPointsPreTrigger));
    QString preTriggerWidthTimeStr = "=" + QString::number(getPreTriggerTimeLength(), 'f', 3) + "ns";
    ui->preTriggerInTimeDispLabel->setText(preTriggerWidthTimeStr);
    ui->nPointsMirrorWidthLineEdit->setText(QString::number(paraSettings.nPointsMirrorWidth));
    double minDetectRange = getMinDetectRange();
    QString minDetectRangeDispStr = "=" + QString::number(minDetectRange, 'f', 2) + "m" + "/" +
            QString::number(minDetectRange*qSin(qDegreesToRadians(paraSettings.elevationAngle)), 'f', 2) + "m";
    ui->minDetectRangeDispLabel->setText(minDetectRangeDispStr);
    ui->nPointsPerBinLineEdit->setText(QString::number(paraSettings.nPointsPerBin));
    double rangeResol = getRangeResolution();
    QString rangeResolDispStr = "=" + QString::number(rangeResol, 'f', 2) + "m" + "/" +
            QString::number(rangeResol*qSin(qDegreesToRadians(paraSettings.elevationAngle)), 'f', 2) + "m";
    ui->rangeResolDispLabel->setText(rangeResolDispStr);
    ui->overlapRatioComboBox->setCurrentText(QString::number(paraSettings.overlapRatio));
    ui->nRangeBinLineEdit->setText(QString::number(paraSettings.nRangeBin));
    double maxDetctRange = getMaxDetectRange();
    QString maxDetctRangeDispStr = "=" + QString::number(maxDetctRange, 'f', 2) + "m" + "/" +
            QString::number(maxDetctRange*qSin(qDegreesToRadians(paraSettings.elevationAngle)), 'f', 2) + "m";
    ui->maxDetectRangeDispLabel->setText(maxDetctRangeDispStr);
    ui->nPulsesAccLineEdit->setText(QString::number(paraSettings.nPulsesAcc));

    double objVelocity = getObjVelocity();
    ui->nPointsObjFreqLineEdit->setText(QString::number(objVelocity));
    QString objFreqPointsDispStr = "m/s=" + QString::number(paraSettings.nPointsObjFreq) +
            QString::fromLocal8Bit("��");
    ui->objFreqPointsDispLabel->setText(objFreqPointsDispStr);
    ui->nDirsVectorCalLineEdit->setText(QString::number(paraSettings.nDirsVectorCal));

    QString dataStorePath;
    if (paraSettings.autoCreateDateDir) {
        QString currentDate = QDate::currentDate().toString("yyyyMMdd");
        dataStorePath = paraSettings.dataFilePath+"/"+currentDate;
    }
    else {
        dataStorePath = paraSettings.dataFilePath;
    }
    ui->dataFilePathLineEdit->setText(dataStorePath);
    QDir currentDir(dataStorePath);
    if (!currentDir.exists()) {
        ui->dataFilePathLineEdit->setStyleSheet("color:red");
    }
    else{
        ui->dataFilePathLineEdit->setStyleSheet("color:black");
    }
    ui->dataFilePathLineEdit->setFont(QFont("Microsoft Yahei"));
    ui->autoCreateDateDirCheckBox->setChecked(paraSettings.autoCreateDateDir);
    ui->nDirsPerFileLineEdit->setText(QString::number(paraSettings.nDirsPerFile));

    //�Ҳ�״̬����Ϣ
    QFileInfo userIniFileInfo(paraSettingFile->getUserIniFilePath());
    ui->settingFilePathDispLineEdit->setText(userIniFileInfo.fileName());
    if (!userIniFileInfo.exists()) {
        ui->settingFilePathDispLineEdit->setStyleSheet("color:red");
    }
    else {
        ui->settingFilePathDispLineEdit->setStyleSheet("color:black");
    }
    ui->settingFilePathDispLineEdit->setFont(QFont("Microsoft Yahei"));
    ui->settingFilePathDispLineEdit->setToolTip(paraSettingFile->getUserIniFilePath());
}

uint ParameterSetDialog::getObjFreqPoints()
{
    double losVelocityRange = ui->nPointsObjFreqLineEdit->text().toDouble();
    double objFreqPoints = losVelocityRange/(paraSettings.sampleFreq/nFFT*paraSettings.laserWaveLength/2);
    if (objFreqPoints - qFloor(objFreqPoints) < 0.5) {
        objFreqPoints = qFloor(objFreqPoints);
    }
    else {
        objFreqPoints = qCeil(objFreqPoints);
    }
    return objFreqPoints;
}

double ParameterSetDialog::getObjVelocity()
{
    double objVelocity = paraSettings.sampleFreq/nFFT*paraSettings.laserWaveLength/2*paraSettings.nPointsObjFreq;
    return objVelocity;
}

double ParameterSetDialog::getVoltageTriggerLevel()
{
    double voltageLevel = 1.1*paraSettings.triggerLevel/qPow(2,13);
    return voltageLevel;
}

double ParameterSetDialog::getPreTriggerTimeLength()
{
    double widthInTime = 1/paraSettings.sampleFreq*1000*paraSettings.nPointsPreTrigger;
    return widthInTime;
}

double ParameterSetDialog::getRangeResolution()
{
    double overlapRatio = paraSettings.overlapRatio;
    int pointsPerBin = paraSettings.nPointsPerBin;
    double sampleFreq = paraSettings.sampleFreq;
    double resol = lightSpeed/sampleFreq/1000000/2;
    double rangeResol = resol*(pointsPerBin*(1-overlapRatio));
    return rangeResol;
}

double ParameterSetDialog::getMinDetectRange()
{
    int pointsPerBin = paraSettings.nPointsPerBin;
    int mirrorWidthPoints = paraSettings.nPointsMirrorWidth;
    double sampleFreq = paraSettings.sampleFreq;
    double resol = lightSpeed/sampleFreq/1000000/2;
    double minDetectRange = resol*(mirrorWidthPoints+pointsPerBin/2);
    return minDetectRange;
}

double ParameterSetDialog::getMaxDetectRange()
{
    double overlapRatio = paraSettings.overlapRatio;
    int pointsPerBin = paraSettings.nPointsPerBin;
    int mirrorWidthPoints = paraSettings.nPointsMirrorWidth;
    int nRangeBin = paraSettings.nRangeBin;
    double sampleFreq = paraSettings.sampleFreq;
    double resol = lightSpeed/sampleFreq/1000000/2;
    double maxDetectRange = resol*(mirrorWidthPoints+pointsPerBin+
                                   pointsPerBin*(nRangeBin-1)*(1-overlapRatio)-pointsPerBin/2);
    return maxDetectRange;
}

void ParameterSetDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << event->localPos();
    qDebug() << ui->dataFilePathLineEdit->x();
    qDebug() << ui->dataFilePathLineEdit->y();
    qDebug() << event->FocusIn;
}

void ParameterSetDialog::on_laserLocalPowerLineEdit_textChanged(const QString &arg1)
{
    paraSettings.laserLocalPower = arg1.toDouble();
}

void ParameterSetDialog::on_laserPulseEnergyLineEdit_textChanged(const QString &arg1)
{
    paraSettings.laserPulseEnergy = arg1.toDouble();
//    DispSettings disp(paraSettings);
}

void ParameterSetDialog::on_laserRPFLineEdit_textChanged(const QString &arg1)
{
    paraSettings.laserRPF = arg1.toDouble();
    refreshDisp();
}

void ParameterSetDialog::on_laserPulseWidthLineEdit_textChanged(const QString &arg1)
{
    paraSettings.laserPulseWidth = arg1.toDouble();
}

void ParameterSetDialog::on_laserWaveLengthLineEdit_textChanged(const QString &arg1)
{
    paraSettings.laserWaveLength = arg1.toDouble();
    refreshDisp();
}

void ParameterSetDialog::on_laserAOMFreqLineEdit_textChanged(const QString &arg1)
{
    paraSettings.laserAOMFreq = arg1.toDouble();
}

void ParameterSetDialog::on_sampleFreqComboBox_activated(const QString &arg1)
{
    paraSettings.sampleFreq = arg1.toDouble();
    refreshDisp();
//    DispSettings disp(paraSettings);
}

void ParameterSetDialog::on_triggerLevelLineEdit_textChanged(const QString &arg1)
{
    paraSettings.triggerLevel = arg1.toUInt();
//    DispSettings disp(paraSettings);
    refreshDisp();
}

void ParameterSetDialog::on_nPointsPretriggerLineEdit_textChanged(const QString &arg1)
{
    paraSettings.nPointsPreTrigger = arg1.toUInt();
    refreshDisp();
}

void ParameterSetDialog::on_nPointsMirrorWidthLineEdit_textChanged(const QString &arg1)
{
    paraSettings.nPointsMirrorWidth = arg1.toUInt();
    refreshDisp();
}

void ParameterSetDialog::on_nPointsPerBinLineEdit_textChanged(const QString &arg1)
{
    paraSettings.nPointsPerBin = arg1.toUInt();
    refreshDisp();
}

void ParameterSetDialog::on_overlapRatioComboBox_activated(const QString &arg1)
{
    paraSettings.overlapRatio = arg1.toDouble();
    refreshDisp();
}

void ParameterSetDialog::on_nRangeBinLineEdit_textChanged(const QString &arg1)
{
    paraSettings.nRangeBin = arg1.toUInt();
    refreshDisp();
}

void ParameterSetDialog::on_nPulsesAccLineEdit_textChanged(const QString &arg1)
{
    paraSettings.nPulsesAcc = arg1.toUInt();
    refreshDisp();
}

void ParameterSetDialog::on_nPointsObjFreqLineEdit_textChanged(const QString &arg1)
{
    qDebug() << "Changed Obj Freq";
    paraSettings.nPointsObjFreq = getObjFreqPoints();
    QString objFreqPointsDispStr = "m/s=" + QString::number(paraSettings.nPointsObjFreq) +
            QString::fromLocal8Bit("��");
    ui->objFreqPointsDispLabel->setText(objFreqPointsDispStr);
}

void ParameterSetDialog::on_nPointsObjFreqLineEdit_editingFinished()
{
    refreshDisp();
}

void ParameterSetDialog::on_nDirsVectorCalLineEdit_textChanged(const QString &arg1)
{
    paraSettings.nDirsVectorCal = arg1.toUInt();
    refreshDisp();
}

void ParameterSetDialog::on_detectModeComboBox_activated(int index)
{
    paraSettings.detectMode = index;
    refreshDisp();
}

void ParameterSetDialog::on_anglekeyRadioButton_clicked(bool checked)
{
    paraSettings.isAngleChecked = checked;
    paraSettings.isCircleChecked = !checked;
    refreshDisp();
}

void ParameterSetDialog::on_circlekeyRadioButton_clicked(bool checked)
{
    paraSettings.isCircleChecked = checked;
    paraSettings.isAngleChecked = !checked;
    refreshDisp();
}

void ParameterSetDialog::on_azAngleStartLineEdit_textChanged(const QString &arg1)
{
    paraSettings.azAngleStart = arg1.toDouble();
}

void ParameterSetDialog::on_azAngleStepLineEdit_textChanged(const QString &arg1)
{
    paraSettings.azAngleStep = arg1.toDouble();
    if (paraSettings.detectMode == 0) {
        if (paraSettings.isAngleChecked) {
            paraSettings.circleNum = paraSettings.angleNum/(360.0/paraSettings.azAngleStep);
        }
        else if(paraSettings.isCircleChecked){
            paraSettings.angleNum = paraSettings.circleNum*(360.0/paraSettings.azAngleStep);
        }
    }
    refreshDisp();
}

void ParameterSetDialog::on_motorSpeedLineEdit_textChanged(const QString &arg1)
{
    paraSettings.motorSpeed = arg1.toDouble();
    refreshDisp();
}

void ParameterSetDialog::on_elevationAngleLineEdit_textChanged(const QString &arg1)
{
    paraSettings.elevationAngle = arg1.toDouble();
}

void ParameterSetDialog::on_intervalTimeLineEdit_textChanged(const QString &arg1)
{
    if (paraSettings.detectMode == 2) {
        paraSettings.intervalTime = arg1.toDouble();
    }
    refreshDisp();
}

void ParameterSetDialog::on_groupTimeLineEdit_textChanged(const QString &arg1)
{
    if (paraSettings.detectMode == 2) {
        paraSettings.groupTime = arg1.toDouble();
    }
    refreshDisp();
}

void ParameterSetDialog::on_nDirsPerFileLineEdit_textChanged(const QString &arg1)
{
    paraSettings.nDirsPerFile = arg1.toDouble();
}

void ParameterSetDialog::on_autoCreateDateDirCheckBox_clicked(bool checked)
{
    paraSettings.autoCreateDateDir = checked;
    refreshDisp();
}

void ParameterSetDialog::on_pathModifyPushButton_clicked()
{
    qDebug() << paraSettings.dataFilePath;
    QString newPath = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("�������ݱ���·��"),
                                                        paraSettings.dataFilePath,
                                                        QFileDialog::ShowDirsOnly |
                                                        QFileDialog::DontResolveSymlinks);
    if (!newPath.isEmpty()) {
        paraSettings.dataFilePath = newPath;
    }
    refreshDisp();
}

void ParameterSetDialog::on_angleNumLineEdit_textChanged(const QString &arg1)
{
    if (paraSettings.detectMode == 0) {
        if (paraSettings.isAngleChecked) {
            paraSettings.angleNum = arg1.toUInt();
            paraSettings.circleNum = paraSettings.angleNum/(360.0/paraSettings.azAngleStep);
            refreshDisp();
//            ui->circleNumLineEdit->setText(QString::number(paraSettings.circleNum));
        }
    }
}

void ParameterSetDialog::on_circleNumLineEdit_textChanged(const QString &arg1)
{
    if (paraSettings.detectMode == 0) {
        if (paraSettings.isCircleChecked) {
            paraSettings.circleNum = arg1.toDouble();
            paraSettings.angleNum = paraSettings.circleNum*360.0/paraSettings.azAngleStep;
            refreshDisp();
//            ui->angleNumLineEdit->setText(QString::number(paraSettings.angleNum));
        }
    }
}

void ParameterSetDialog::on_saveSettingsPushButton_clicked()
{
    QString defaultSettingFilePath = paraSettingFile->getDefaultIniFilePath();
    QString userSettingFilePath = paraSettingFile->getUserIniFilePath();
    QDir userSettingFile(userSettingFilePath);
    if (defaultSettingFilePath == userSettingFilePath &&
            !userSettingFile.exists()) {
        userSettingFilePath = QFileDialog::getSaveFileName(this,
                                                           QString::fromLocal8Bit("���Ϊ�����ļ�"),
                                                           paraSettings.dataFilePath, tr("*.ini"));
//        qDebug() << userIniFilePath;
        if (!userSettingFilePath.isEmpty()) {
            if (!userSettingFilePath.endsWith(".ini", Qt::CaseInsensitive)) {
                userSettingFilePath.append(".ini");
            }
            paraSettingFile->setUserIniFilePath(userSettingFilePath);
            paraSettingFile->writeSettings(paraSettings);
        }
    }
    else {
        paraSettingFile->writeSettings(paraSettings);
    }
    refreshDisp();
}

void ParameterSetDialog::on_saveasSettingsPushButton_clicked()
{
    QString userIniFileName = QFileDialog::getSaveFileName(this,
                                                           QString::fromLocal8Bit("���Ϊ�����ļ�"),
                                                           paraSettings.dataFilePath, tr("*.ini"));
    if (!userIniFileName.isEmpty()) {
        if (!userIniFileName.endsWith(".ini", Qt::CaseInsensitive)) {
            userIniFileName.append(".ini");
        }
        paraSettingFile->setUserIniFilePath(userIniFileName);
        paraSettingFile->writeSettings(paraSettings);
    }
    refreshDisp();
}

void ParameterSetDialog::on_loadSettingsPushButton_clicked()
{
    QString userIniFileName = QFileDialog::getOpenFileName(this,
                                                           QString::fromLocal8Bit("���������ļ�"),
                                                           paraSettings.dataFilePath, tr("*.ini"));
    if (!userIniFileName.isEmpty()) {
        qDebug() << userIniFileName;
        paraSettingFile->setUserIniFilePath(userIniFileName);
        paraSettings = paraSettingFile->readSettings();
    }
    refreshDisp();
}
