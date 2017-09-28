#include "ADQ214.h"
#include <QMessageBox>

const unsigned int adq_num = 1;

ADQ214::ADQ214(QObject *parent) : QObject(parent)
{
    adq_cu = CreateADQControlUnit();
    success = true;

    int apirev = ADQAPI_GetRevision();     // ��ȡAPI�汾
    qDebug() << IS_VALID_DLL_REVISION(apirev);
    connectADQDevice();

    num_of_devices = 0;
    num_of_failed = 0;
    num_of_ADQ214 = 0;

    //    setupadq.num_samples_collect = nFFT*2;  // ���ò�������
    setupadq.stream_ch = ADQ214_STREAM_ENABLED_BOTH;
    setupadq.stream_ch &= 0x7;
    setupadq.num_buffers = 64;
    setupadq.size_buffers = 1024;
    setupadq.clock_source = 0;       //0 = Internal clock
    setupadq.pll_divider = 2;        //��Internal clock=0ʱ�����ã�f_clk = 800MHz/divider
    psd_res = nullptr;
    psd_array = nullptr;
}

void ADQ214::Transfer_Settings(const SOFTWARESETTINGS &settings)
{
    mainSettings = settings;
}

PSD_DATA* ADQ214::get_PSD_Union()
{
    return psd_res;
}

double* ADQ214::get_PSD_double()
{
    return psd_array;
}



void ADQ214::connectADQDevice()
{
    int num_of_devices,num_of_failed,num_of_ADQ214;
    num_of_devices = ADQControlUnit_FindDevices(adq_cu);			//�ҵ�������������ӵ�ADQ��������һ��ָ���б������ҵ��豸������
    num_of_failed = ADQControlUnit_GetFailedDeviceCount(adq_cu);
    num_of_ADQ214 = ADQControlUnit_NofADQ214(adq_cu);				//�����ҵ�ADQ214�豸������
    if((num_of_failed > 0)||(num_of_devices == 0))
    {
        qDebug()<<"�ɼ���δ����";
        isADQ214Connected = false;
        //        QMessageBox::critical(this, QString::("�ɼ���δ���ӣ���"), QString::fromStdString("�ɼ���δ����"));
    }
    else if (num_of_ADQ214 != 0)
    {
        qDebug()<<"�ɼ���������";
        isADQ214Connected = true;
    }
}

void ADQ214::Start_Capture()
{
    if(!Config_ADQ214())
        return;

    setupadq.data_stream_target = new quint16[(mainSettings.nRangeBin + 2) * nFFT_half * 4];
    memset(setupadq.data_stream_target, 0, (mainSettings.nRangeBin + 2) * nFFT_half * 8);

    if(!CaptureData2Buffer())
    {
        qDebug() << ("Collect failed!");
        delete setupadq.data_stream_target;
        return;
    }
    qDebug() << ("Collect finished!");
    qDebug() << "Convert start";

    ConvertData2Spec();
    WriteSpecData2disk();
    delete setupadq.data_stream_target;

    if(success == 0)
    {
        qDebug() << "Error!";
        DeleteADQControlUnit(adq_cu);
    }
    qDebug() << "Write finished";
}

bool ADQ214::Config_ADQ214()                   // ���òɼ���
{
    success = false;
    if (!isADQ214Connected)
    {
        QMessageBox::critical(NULL, QString::fromStdString("�ɼ���δ���ӣ���"), QString::fromStdString("�ɼ���δ����"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    else
    {
        success = ADQ214_SetDataFormat(adq_cu, adq_num,ADQ214_DATA_FORMAT_UNPACKED_16BIT);          // �������ݸ�ʽ����֪��û����
        // ����TransferBuffer��С������
        int num_buffers = 256;
        int size_buffers = 1024;
        success = success && ADQ214_SetTransferBuffers(adq_cu, adq_num, num_buffers, size_buffers);
        success = success && ADQ214_SetClockSource(adq_cu, adq_num, setupadq.clock_source);          // ����ʱ��Դ
        success = success && ADQ214_SetPllFreqDivider(adq_cu, adq_num, setupadq.pll_divider);        // ����PLL��Ƶ��

        // ����ADQ214���ڲ��Ĵ���

        // quint16 CMD = 0;
        // success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,CMD);                      //����
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x31,0,mainSettings.triggerLevel);  //������ƽ
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x32,0,mainSettings.nPulsesAcc);      //�ۼ�������
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x33,0,mainSettings.nPointsPerBin);  //�����ŵ�������ҪΪż��
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x34,0,mainSettings.nRangeBin+3);      //��������
        // success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x35,0,write_data5);              //Ŀ��Ƶ�����ޣ�����
        // success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x36,0,write_data6);              //Ŀ��Ƶ�����ޣ�����
        uint Total_Points_Num = mainSettings.nPointsPerBin * (mainSettings.nRangeBin+3);            // �ڲ��źţ������ܵ���
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x37,0,Total_Points_Num);            //�����崦���ܵ���
    }
    return success;
}

bool ADQ214::CaptureData2Buffer()         // �ɼ����ݵ�����
{
    success = ADQ214_DisarmTrigger(adq_cu, adq_num);
    success = success && ADQ214_SetStreamStatus(adq_cu, adq_num,setupadq.stream_ch);
    success = success && ADQ214_ArmTrigger(adq_cu, adq_num);

    unsigned int samples_to_collect;
    samples_to_collect = (mainSettings.nRangeBin + 2)*nFFT_half*4;
    unsigned int total_sample_points = samples_to_collect;

    int nloops = 0;

    ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0xFFFE);   // bit[0]��0
    ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0x0001);   // bit[0]��1

    while (samples_to_collect > 0)
    {
        nloops ++;
        //        qDebug() << "Loops:" << nloops;
        if (setupadq.trig_mode == 1)        //If trigger mode is sofware
        {
            ADQ214_SWTrig(adq_cu, adq_num);
        }

        //            ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,write_data0&0xFF7F);   // bit[7]��0
        //            ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,write_data0|0x0080);   // bit[7]��1
        ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0xFFFE);   // bit[0]��0
        ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0x0001);   // bit[0]��1

        do
        {
            setupadq.collect_result = ADQ214_GetTransferBufferStatus(adq_cu, adq_num, &setupadq.buffers_filled);
            //            qDebug() << ("Filled: ") << setupadq.buffers_filled;
        } while ((setupadq.buffers_filled == 0) && (setupadq.collect_result));

        setupadq.collect_result = ADQ214_CollectDataNextPage(adq_cu, adq_num);
        //        qDebug() << "setupadq.collect_result = " << setupadq.collect_result;

        int samples_in_buffer = qMin(ADQ214_GetSamplesPerPage(adq_cu, adq_num), samples_to_collect);
        //        qDebug() << "samples_in_buffer = " << samples_in_buffer;

        if (ADQ214_GetStreamOverflow(adq_cu, adq_num))
        {
            //            qDebug() << ("Warning: Streaming Overflow!");
            setupadq.collect_result = 0;
        }

        if (setupadq.collect_result)
        {
            memcpy((void*)&setupadq.data_stream_target[total_sample_points - samples_to_collect],
                    ADQ214_GetPtrStream(adq_cu, adq_num), samples_in_buffer* sizeof(quint16));
            samples_to_collect -= samples_in_buffer;
            qDebug() << "samples_to_collect = "<<samples_to_collect;
        }
        else
        {
            qDebug() << ("Collect next data page failed!");
            samples_to_collect = 0;
        }
    }

    success = success && ADQ_DisarmTrigger(adq_cu, adq_num);

    success = success && ADQ214_SetStreamStatus(adq_cu, adq_num,0);
    return success;
}

void ADQ214::WriteSpecData2disk()         // ������ת���ɹ����ף�д�뵽�ļ�
{
    // Write to data to file after streaming to RAM, because ASCII output is too slow for realtime.
    //    qDebug() << "Writing streamed Spectrum data in RAM to disk" ;
    QFile Specfile("data_Spec.txt");
    if(Specfile.open(QFile::WriteOnly))
    {
        //        qDebug() << "File opens";
        QTextStream out(&Specfile);
        //        qDebug() << "mainSettings.nRangeBin+2 = " << mainSettings.nRangeBin+2;
        for (int k=0; (k<(mainSettings.nRangeBin+2)*nFFT_half); k++)
        {
            //            qDebug() << "k = " << k;
            out <<psd_res[k].data64 << endl;
        }
        Specfile.close();
    }

    //    qDebug() << "Write finished";
}

void ADQ214::ConvertData2Spec()           // ������ת���ɹ�����
{
    qDebug() << "Start Convert!!";
    int nPSD = (mainSettings.nRangeBin+2)*nFFT_half;  // PSD�����׳���

    int i = 0, k = 0, l = 0;
    for (l=0;l<(mainSettings.nRangeBin + 2);l++) {
        qDebug() << "l = " << l;
        for (k=0,i=0; (k<nFFT_half); k++,k++)
        {
            psd_res[nFFT_half*l + nFFT_half-1 - k].pos[3] = setupadq.data_stream_target[nFFT*2*l + i];
            psd_res[nFFT_half*l + nFFT_half-1 - k].pos[2] = setupadq.data_stream_target[nFFT*2*l + i+1];
            psd_res[nFFT_half*l + nFFT_half-1 - k].pos[1] = setupadq.data_stream_target[nFFT*2*l + i+4];
            psd_res[nFFT_half*l + nFFT_half-1 - k].pos[0] = setupadq.data_stream_target[nFFT*2*l + i+5];
            psd_res[nFFT_half*l + nFFT_half-1 - k-1].pos[3] = setupadq.data_stream_target[nFFT*2*l + i+2];
            psd_res[nFFT_half*l + nFFT_half-1 - k-1].pos[2] = setupadq.data_stream_target[nFFT*2*l + i+3];
            psd_res[nFFT_half*l + nFFT_half-1 - k-1].pos[1] = setupadq.data_stream_target[nFFT*2*l + i+6];
            psd_res[nFFT_half*l + nFFT_half-1 - k-1].pos[0] = setupadq.data_stream_target[nFFT*2*l + i+7];

            i = i + 8;
        }
    }

    for (k=0; k<nPSD; k++)
    {
        psd_array[k] = double(psd_res[k].data64);
    }
    qDebug() << "Covert finished!!!";
}

void ADQ214::Init_Buffers()           // ��ʼ�����������ݴ洢������
{
    int nPSD = (mainSettings.nRangeBin+2) * nFFT_half;  // PSD�����׳���

    if (psd_res != nullptr)
        delete psd_res;
    psd_res = new PSD_DATA[nPSD];       //���Ż�������ÿ��new

    if (psd_array != nullptr)           //���Ż�������ÿ��new
        delete psd_array;
    psd_array = new double[nPSD];
    qDebug() << "nPSD = " << nPSD;
}
