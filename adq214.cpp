#include "ADQ214.h"
#include <QMessageBox>

const unsigned int adq_num = 1;

ADQ214::ADQ214(QObject *parent) : QObject(parent)
{
    adq_cu = CreateADQControlUnit();
    success = true;

    int apirev = ADQAPI_GetRevision();     // 获取API版本
    qDebug() << IS_VALID_DLL_REVISION(apirev);
    connectADQDevice();

    num_of_devices = 0;
    num_of_failed = 0;
    num_of_ADQ214 = 0;

    //    setupadq.num_samples_collect = nFFT*2;  // 设置采样点数
    setupadq.stream_ch = ADQ214_STREAM_ENABLED_BOTH;
    setupadq.stream_ch &= 0x7;
    setupadq.num_buffers = 64;
    setupadq.size_buffers = 1024;
    setupadq.clock_source = 0;       //0 = Internal clock
    setupadq.pll_divider = 2;        //在Internal clock=0时，设置，f_clk = 800MHz/divider
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
    num_of_devices = ADQControlUnit_FindDevices(adq_cu);			//找到所有与电脑连接的ADQ，并创建一个指针列表，返回找到设备的总数
    num_of_failed = ADQControlUnit_GetFailedDeviceCount(adq_cu);
    num_of_ADQ214 = ADQControlUnit_NofADQ214(adq_cu);				//返回找到ADQ214设备的数量
    if((num_of_failed > 0)||(num_of_devices == 0))
    {
        qDebug()<<"采集卡未连接";
        isADQ214Connected = false;
        //        QMessageBox::critical(this, QString::("采集卡未连接！！"), QString::fromStdString("采集卡未连接"));
    }
    else if (num_of_ADQ214 != 0)
    {
        qDebug()<<"采集卡已连接";
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

bool ADQ214::Config_ADQ214()                   // 配置采集卡
{
    success = false;
    if (!isADQ214Connected)
    {
        QMessageBox::critical(NULL, QString::fromStdString("采集卡未连接！！"), QString::fromStdString("采集卡未连接"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    else
    {
        success = ADQ214_SetDataFormat(adq_cu, adq_num,ADQ214_DATA_FORMAT_UNPACKED_16BIT);          // 设置数据格式，不知有没有用
        // 设置TransferBuffer大小及数量
        int num_buffers = 256;
        int size_buffers = 1024;
        success = success && ADQ214_SetTransferBuffers(adq_cu, adq_num, num_buffers, size_buffers);
        success = success && ADQ214_SetClockSource(adq_cu, adq_num, setupadq.clock_source);          // 设置时钟源
        success = success && ADQ214_SetPllFreqDivider(adq_cu, adq_num, setupadq.pll_divider);        // 设置PLL分频数

        // 配置ADQ214的内部寄存器

        // quint16 CMD = 0;
        // success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,CMD);                      //命令
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x31,0,mainSettings.triggerLevel);  //触发电平
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x32,0,mainSettings.nPulsesAcc);      //累加脉冲数
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x33,0,mainSettings.nPointsPerBin);  //距离门点数，需要为偶数
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x34,0,mainSettings.nRangeBin+3);      //距离门数
        // success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x35,0,write_data5);              //目标频带下限，保留
        // success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x36,0,write_data6);              //目标频带上限，保留
        uint Total_Points_Num = mainSettings.nPointsPerBin * (mainSettings.nRangeBin+3);            // 内部信号，处理总点数
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x37,0,Total_Points_Num);            //单脉冲处理总点数
    }
    return success;
}

bool ADQ214::CaptureData2Buffer()         // 采集数据到缓存
{
    success = ADQ214_DisarmTrigger(adq_cu, adq_num);
    success = success && ADQ214_SetStreamStatus(adq_cu, adq_num,setupadq.stream_ch);
    success = success && ADQ214_ArmTrigger(adq_cu, adq_num);

    unsigned int samples_to_collect;
    samples_to_collect = (mainSettings.nRangeBin + 2)*nFFT_half*4;
    unsigned int total_sample_points = samples_to_collect;

    int nloops = 0;

    ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0xFFFE);   // bit[0]置0
    ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0x0001);   // bit[0]置1

    while (samples_to_collect > 0)
    {
        nloops ++;
        //        qDebug() << "Loops:" << nloops;
        if (setupadq.trig_mode == 1)        //If trigger mode is sofware
        {
            ADQ214_SWTrig(adq_cu, adq_num);
        }

        //            ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,write_data0&0xFF7F);   // bit[7]置0
        //            ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,write_data0|0x0080);   // bit[7]置1
        ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0xFFFE);   // bit[0]置0
        ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0x0001);   // bit[0]置1

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

void ADQ214::WriteSpecData2disk()         // 将数据转换成功率谱，写入到文件
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

void ADQ214::ConvertData2Spec()           // 将数据转换成功率谱
{
    qDebug() << "Start Convert!!";
    int nPSD = (mainSettings.nRangeBin+2)*nFFT_half;  // PSD功率谱长度

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

void ADQ214::Init_Buffers()           // 初始化功率谱数据存储缓冲区
{
    int nPSD = (mainSettings.nRangeBin+2) * nFFT_half;  // PSD功率谱长度

    if (psd_res != nullptr)
        delete psd_res;
    psd_res = new PSD_DATA[nPSD];       //待优化，不用每次new

    if (psd_array != nullptr)           //待优化，不用每次new
        delete psd_array;
    psd_array = new double[nPSD];
    qDebug() << "nPSD = " << nPSD;
}
