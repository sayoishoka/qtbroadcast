#include "monitor.h"
#include "QtMultimedia/qaudioformat.h"
#include "QtMultimedia/qaudiooutput.h"
#include "connect/dispatcher.h"
#include "lib/NAudioServerLib.h"
#include "qjsonobject.h"

MonitorModule *MonitorModule::getInstance()
{
    static MonitorModule monitorModule;
    return &monitorModule;
}

MonitorModule::MonitorModule(QObject *parent)
    : QObject{parent}
{
    InitMapper();
    na_set_monitor_callback(MonitorCallback, DecodeCallback);
    initializeAudioPlayback();
}



MonitorModule::~MonitorModule() {
    cleanupAudioPlayback();
}

void MonitorModule::cleanupAudioPlayback() {
    if (audioOutput) {
        audioOutput->stop();
        audioOutput->deleteLater();
        audioOutput = nullptr;
    }
    if (audioBuffer) {
        audioBuffer->close();
        audioBuffer->deleteLater();
        audioBuffer = nullptr;
    }
}



// 定义接收监听音频数据帧的回调函数
 void __stdcall  MonitorCallback(unsigned int devno, unsigned char* data, unsigned int len) {
    // 这里可以处理接收到的原始音频数据帧
    // devno 是设备的编号，data 是接收到的数据，len 是数据的长度
    // 例如，可以将数据发送到处理线程或保存到文件
     qDebug()<<"接收监听音频数据帧";
}


void MonitorModule::initializeAudioPlayback() {
    // 设置音频格式
    audioFormat.setSampleRate(44100); // 例如，采样率为44100Hz
    audioFormat.setChannelCount(2);   // 立体声
    audioFormat.setSampleSize(16);    // 16位
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);

    // 创建音频输出对象
    audioOutput = new QAudioOutput(audioFormat, this);
    audioOutput->setBufferSize(1024 * 1024); // 设置合适的缓冲区大小

    // 创建音频缓冲区
    audioBuffer = new QBuffer(this);
    audioBuffer->open(QIODevice::WriteOnly);

    // 启动音频输出
    if (audioOutput->state() == QAudio::ActiveState) {
        audioOutput->start(audioBuffer);
    }
}

void MonitorModule::playAudio(const unsigned char *data, unsigned int len) {
    if (audioOutput && audioBuffer && data && len > 0) {
        // 将PCM数据写入音频缓冲区
        audioBuffer->write(reinterpret_cast<const char*>(data), len);

        // 确保音频输出正在运行
        if (audioOutput->state() == QAudio::StoppedState) {
            audioOutput->start(audioBuffer);
        }
    }
}

// 假设这是您的回调函数
void __stdcall DecodeCallback(unsigned int devno, unsigned char* data, unsigned int len) {
    MonitorModule *monitorModule = MonitorModule::getInstance();
    if (monitorModule) {
        monitorModule->playAudio(data, len);
    }
}


void MonitorModule::InitMapper()
{


    Dispatcher *dispatcher = Dispatcher::getDispatcher();

    // 注册设置自动触发参数的函数
    dispatcher->Register("setAutoTrigPara", std::bind(&MonitorModule::setAutoTrigPara, this, std::placeholders::_1));

    // 注册获取自动触发参数的函数
    dispatcher->Register("getAutoPara", std::bind(&MonitorModule::getAutoPara, this, std::placeholders::_1));

    // 注册设置监听参数的函数
    dispatcher->Register("setMonitorPara", std::bind(&MonitorModule::setMonitorPara, this, std::placeholders::_1));

    // 注册获取监听参数的函数
    dispatcher->Register("getMonitorPara", std::bind(&MonitorModule::getMonitorPara, this, std::placeholders::_1));

    // 注册启动设备监听的函数
    dispatcher->Register("startDevMonitor", std::bind(&MonitorModule::startDevMonitor, this, std::placeholders::_1));

    // 注册停止设备监听的函数
    dispatcher->Register("stopDevMonitor", std::bind(&MonitorModule::stopDevMonitor, this, std::placeholders::_1));

    // 注册启动监听解码的函数
    dispatcher->Register("startMonitorDecode", std::bind(&MonitorModule::startMonitorDecode, this, std::placeholders::_1));

    // 注册停止监听解码的函数
    dispatcher->Register("stopMonitorDecode", std::bind(&MonitorModule::stopMonitorDecode, this, std::placeholders::_1));
}

/** 函数名称 :设置自动触发参数
  * 函数功能 :设置设备自动触发采集参数，只对采集设备有效
  * 输入参数 :devno:设置参数的设备编号
  *          para:自动触发参数{
  *             unsigned char trig_enable;  //自动触发开关
                unsigned char priority;     //自动采集时音源的优先级
                unsigned short trig_val;    //触发值
  *          }
  * 返 回 值 :status 记录参数设置状态
  * 函数作者 :张昊
  * 完成日期 :2024-3-25
  * 修改记录 :
  */
QJsonObject MonitorModule::setAutoTrigPara(QJsonObject &data){
    int   devNo      =  data.value ("devNo").toInt ();
    unsigned char  trigEnable =  (unsigned char)data.value("trigEnable").toInt();
    unsigned char  priority   =  (unsigned char)data.value("priority").toInt();
    unsigned short trig_val    =  (unsigned short)data.value ("trig_va").toInt ();

    _auto_trig_para trig_para ;
    trig_para.trig_enable = trigEnable;
    trig_para.priority    = priority  ;
    trig_para.trig_val    = trig_val  ;

    m_autotrigpara.insert({devNo,trig_para});
    bool status = !na_set_auto_trig_para (devNo,&trig_para);
    QJsonObject obj;
    obj.insert("response", "reSetAutoTrigPara");
    obj.insert("status", status);
    return obj;
}

/** 函数名称 :获取自动触发参数
  * 函数功能 :获取给设备设置的自动触发参数
  * 输入参数 :devno:设置参数的设备编号
  * 返 回 值 :paraList:{
  *             int devNo;                  //设备编号
  *             unsigned char trig_enable;  //自动触发开关
                unsigned char priority;     //自动采集时音源的优先级
                unsigned short trig_val;    //触发值
  *          }
  * 函数作者 :张昊
  * 完成日期 :2024-3-25
  * 修改记录 :
  */
QJsonObject MonitorModule::getAutoPara(QJsonObject &data){
    int devNo = data.value("devNo").toInt();
    QJsonObject response;

    for(auto &it : m_autotrigpara){
        if(it.first == devNo){
            _auto_trig_para para = it.second;
            QJsonObject paraList;
            paraList.insert("trigEnable", QJsonValue::fromVariant(para.trig_enable));
            paraList.insert("devNo", QJsonValue::fromVariant(it.first));
            paraList.insert("priority", QJsonValue::fromVariant(para.priority));
            paraList.insert("trigVal", QJsonValue::fromVariant(para.trig_val));

            response.insert("response", "reGetAutoPara");
            response.insert("paraList", paraList);
            break;
        }
    }
    qDebug()<<response;
    return response;
}

/**
 * 函数名称 : setMonitorPara
 * 函数功能 : 设置设备的监听参数
 * 输入参数 : devNo       : 设置参数的设备编号
 *            autoMonotor : 上线自动监听状态
 *            saveData    : 音频数据保存状态
 *            saveDir     : 监听文件保存目录
 *            fileName    : 保存文件名
 *            fileSize    : 保存单个文件的大小
 *
 * 返 回 值 : status      : 设置参数的状态
 * 函数作者 : 张昊
 * 完成日期 : 2024-3-25
 * 修改记录 :
 */
QJsonObject MonitorModule::setMonitorPara(QJsonObject &data){
    int           devNo       = data.value("devNo").toInt();
    unsigned char autoMonotor = data.value("autoMonotor").toInt();
    unsigned char saveData    = data.value("saveData").toInt();
    QString       saveDir     = data.value("dirList").toString();
    QString       fileName    = data.value("fileNameList").toString();
    int           fileSize    = data.value("fileSize").toInt();

    _mon_para para ;
    para.auto_mon             = autoMonotor;
    para.save_audio_data      = saveData;
    para.file_size            = fileSize;
    strncpy(para.save_dir,  saveDir .toStdString().c_str(), sizeof(para.save_dir));
    strncpy(para.file_name, fileName.toStdString().c_str(), sizeof(para.file_name));


    m_monitorpara.insert({devNo, para});

    bool status = !na_set_monitor_para(devNo, &para);

    QJsonObject response;
    response.insert("response", "reSetMonitorPara");
    response.insert("status", status);


    return response;
}

/**
 * 函数名称 : setMonitorPara
 * 函数功能 : 设置设备的监听参数
 * 输入参数 :  devNo       : 设置参数的设备编号
 *            autoMonotor : 上线自动监听状态
 *            saveData    : 音频数据保存状态
 *            saveDir     : 监听文件保存目录
 *            fileName    : 保存文件名
 *            fileSize    : 保存单个文件的大小
 *
 * 返 回 值 : status      : 设置参数的状态
 * 函数作者 : 张昊
 * 完成日期 : 2024-3-25
 * 修改记录 :
 */
QJsonObject MonitorModule::getMonitorPara(QJsonObject &data){
    int devNo = data.value("devNo").toInt();
    QJsonObject response;

    auto h = m_monitorpara[devNo];
    qDebug()<<h.file_size;

    auto it = m_monitorpara.find(devNo);
    if(it != m_monitorpara.end()){
        _mon_para para = it->second;
        QJsonObject paraList;
        paraList.insert("autoMonotor", QJsonValue::fromVariant(para.auto_mon));
        paraList.insert("devNo", QJsonValue::fromVariant(it->first));
        paraList.insert("fileName", QJsonValue::fromVariant(QString::fromStdString(para.file_name)));
        paraList.insert("fileSize", QJsonValue::fromVariant(para.file_size));
        paraList.insert("saveData", QJsonValue::fromVariant(para.save_audio_data));

        response.insert("response", "reGetMonitorPara");
        response.insert("paraList", paraList);
    }

    return response;
}


/** 函数名称 :startDevMonitor
  * 函数功能 :启动设备监听
  * 输入参数 :devNo       : 启动监听的设备编号
  * 返 回 值 :status      : 启动监听的状态
  * 函数作者 :张昊
  * 完成日期 :2024-4-1
  * 修改记录 :
  */


QJsonObject MonitorModule::startDevMonitor(QJsonObject &data) {
    int devNo = data.value("devNo").toInt();

    // 调用SDK接口启动监听
    bool status = na_start_dev_monitor(devNo);

    QJsonObject response;
    response.insert("response", "reStartDevMonitor");
    response.insert("status", status);

    return response;
}

/**
 * 函数名称 : stopDevMonitor
 * 函数功能 : 停止设备监听
 * 输入参数 : devNo       : 停止监听的设备编号
 * 返回值 : status      : 停止监听的状态
 * 函数作者 : 张昊
 * 完成日期 : 2024-4-1
 * 修改记录 :
 */

QJsonObject MonitorModule::stopDevMonitor(QJsonObject &data) {
    int devNo = data.value("devNo").toInt();

    // 调用SDK接口停止监听
    bool status = na_stop_dev_monitor(devNo);

    QJsonObject response;
    response.insert("response", "reStopDevMonitor");
    response.insert("status", status);

    return response;
}


/**
 * 函数名称 : startMonitorDecode
 * 函数功能 : 启动监听解码
 * 输入参数 : devNo       : 启动监听解码的设备编号
 * 返回值 : status      : 启动监听解码的状态
 * 函数作者 : 张昊
 * 完成日期 : 2024-4-1
 * 修改记录 :
 */

QJsonObject MonitorModule::startMonitorDecode(QJsonObject &data) {
    int devNo = data.value("devNo").toInt();

    // 调用SDK接口启动监听解码
    bool status = na_start_monitor_decode(devNo);

    QJsonObject response;
    response.insert("response", "reStartMonitorDecode");
    response.insert("status", status);

    return response;
}


/**
 * 函数名称 : stopMonitorDecode
 * 函数功能 : 停止监听解码
 * 输入参数 : devNo       : 停止监听解码的设备编号
 * 返回值 : status      : 停止监听解码的状态
 * 函数作者 : 张昊
 * 完成日期 : 2024-4-1
 * 修改记录 :
 */

QJsonObject MonitorModule::stopMonitorDecode(QJsonObject &data) {
    int devNo = data.value("devNo").toInt();

    // 调用SDK接口停止监听解码
    bool status = na_stop_monitor_decode(devNo);

    QJsonObject response;
    response.insert("response", "reStopMonitorDecode");
    response.insert("status", status);

    return response;
}
