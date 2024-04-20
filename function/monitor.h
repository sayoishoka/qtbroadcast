#ifndef MONITOR_H
#define MONITOR_H

#include "lib/NAudioServerLib.h"
#include <QObject>
#include <QAudio>
#include <QAudioFormat>
#include <QBuffer>
#include <QIODevice>
#include<QAudioOutput>

class MonitorModule : public QObject
{
    Q_OBJECT
public:
    static MonitorModule *getInstance();

    ~MonitorModule();

    void initializeAudioPlayback();
    void playAudio(const unsigned char *data, unsigned int len);
    void cleanupAudioPlayback();
private:
    explicit MonitorModule(QObject *parent = nullptr);

    QAudioFormat audioFormat; // 音频格式
    QAudioDeviceInfo info;     // 音频设备信息
    QAudioOutput *audioOutput = nullptr; // 音频输出对象
    QBuffer *audioBuffer = nullptr; // 音频缓冲区

    std::map<unsigned char, _auto_trig_para> m_autotrigpara;
    std::map<unsigned char, _mon_para> m_monitorpara;

    void InitMapper();

    QJsonObject setAutoTrigPara(QJsonObject &data);    //设置自动触发参数
    QJsonObject getAutoPara(QJsonObject &data);        //获取自动触发参数

    QJsonObject setMonitorPara(QJsonObject &data);     //设置监听参数
    QJsonObject getMonitorPara(QJsonObject &data);     //获取设备的监听参数


    QJsonObject startDevMonitor(QJsonObject &data);    //设备启动监听
    QJsonObject stopDevMonitor(QJsonObject &data);     //设备停止监听
    QJsonObject startMonitorDecode(QJsonObject &data); //启动监听解码
    QJsonObject stopMonitorDecode(QJsonObject &data);  //停止监听解码

signals:

};
extern void __stdcall MonitorCallback(unsigned int devno, unsigned char* data, unsigned int len);
extern void __stdcall DecodeCallback(unsigned int devno, unsigned char* data, unsigned int len);
#endif // MONITOR_H
