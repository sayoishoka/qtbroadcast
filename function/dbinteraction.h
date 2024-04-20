#ifndef __DBINTERACTION_H__
#define __DBINTERACTION_H__

#include "daotype.h"

#include <QSqlQuery>
#include <QThread>

/** 设备 **/
QVector<Device> SelectGpDevs(uint groupNo, uint devNo = 0, QString devName = "");  // 获取分组下设备列表
QVector<Device> SelectDevs();                                                      // 获取所有设备
bool InsertDev(uint devNo, QString devName, uchar volume);                         // 插入设备
bool setDevVolume(uint devNo, uchar volume);                                       // 设置设备音量
uint getDevNo(QString devName);
uint getDevGroupNo(int devNo);
/** 分组 **/
QVector<Group> GetAllGroup();                           // 获取所有分组信息
bool InsertGroup(QString groupName, uint groupNo);      // 添加分组
bool DeleteGroup(uint groupNo);                         // 删除分组
bool UpDateGroupName(uint groupNo, QString groupName);  // 修改分组名称
bool InsertDevToGroup(uint devNo, uint groupNo);        // 添加分组中的设备
bool DeleteDevToGroup(uint devNo);                      // 删除分组中的设备
uint getGroupNo(QString groupName);

/** 节目 **/
bool AddProgram(int pmNo, QString &pmName, int pmType, QString &playType, QTime &duration);         // 添加节目
bool UpdateProgram(int oldPmNo, int newPmNo, int pmtye, QString &pmName, QString &playType, QTime &duration);  // 更新节目
bool DeleteProgram(int pmNo);                                                                       // 删除节目
Program SelectProgram(int pmNo);                                                                    // 查询节目信息
bool AddFileToProgram(int pmNo, int fileNo, int priority);                                          // 给节目分配播放文件
bool RemovePlayFile(int pmNo, int fileNo);                                                          // 移除节目中要播放的文件
QVector<FileInfo> SelectPmFile(int pmNo);                                                           // 查询节目下分配的文件列表
bool AddProgramDev(int pmNo, int devNo);                                                            // 给节目分配设备
bool RemoveProgramDev(int pmNo, int devNo);                                                         // 移除节目下的设备
QVector<Device> SelectPlayDev(int pmNo);                                                            // 查询节目下分配的设备列表
bool AddTimingInfo(int pmNo, QDate &startDate, QDate &endDate, QTime &startTime, int weekCnt);      // 当节目为定时时，添加定时信息
bool UpdateTimingInfo(int pmNo, QDate &startDate, QDate &endDate, QTime &startTime, int weekCnt);   // 修改定时信息
bool SelectTimeingInfo(int pmNo);
std::tuple<QDate, QDate, QTime, int> SelectTimingInfo(int pmNo);                                    // 查询定时节目的定时信息
QVector<Program> SelectPrograms(QString pmName);                                                    // 根据节目名进行模糊查询
QVector<std::tuple<int, int, QTime>> SelectCurDatePm();                                             // 查询当天需要播放的节目
QVector<uint> SelectPmDevs(int pmNo);                                                               // 查询节目下设备


/** 文件 **/
bool AddFileRecord(QString &fileName,
                   QString &fileType,
                   QString &filePath);                   // 添加文件
bool SetAuditStatus(int fileNo, bool auditResult);       // 设置文件审核状态
bool DeleteFileRecord(int fileNo);                       // 删除文件记录
QString SelectFilePath(int fileNo);                      // 查找文件路径
QVector<FileInfo> SelectAllFileInfo(QString &fileType);  // 查找所有文件信息 by fileType
QVector<FileInfo>
SelectFileList(int checkStatus, QString fuzzyName);     // 根据审核状态和模糊名字查询

/** 权限管理 **/
std::tuple<bool, int>
SelectUser(QString userName, QString userWd);                                 // 查询用户信息
QVector<std::tuple<QString, int>> SelectPms(int roleNo);                      // 查询角色父权限
QVector<std::tuple<QString, int>> SelectPms(int roleNo, int faFuncNo);        // 查询角色子权限

QVector<std::tuple<QString, int>> SelectFunc();                               // 查询一级功能
QVector<std::tuple<QString, int>> SelectFunc(int faNo);                       // 查询二级功能

bool AddPmsToRole(int roleNo, int funcNo);                                    // 给角色添加权限功能
bool RemovePmsToRole(int roleNo, int funcNo);                                 // 给角色移除权限功能
bool SelectHasPms(int roleNo, int funcNo);                                    // 查询角色是否有权限

QVector<std::tuple<QString, int>> SelectAllRole();                            // 查询所有角色信息
QVector<std::tuple<QString, int>> selectRoleByName(const QString &roleName);  // 根据名字查询角色

bool AddRole(QString roleName, int roleNo);                                   // 新增角色
bool RemoveRole(int roleNo);                                                  // 移除角色

// username : 模糊查询; roleNo : 为0,不约束此字段;
QVector<User> SelectUserList(QString userName, int roleNo);  // 条件查询用户
bool AddUser(QString userName, int role);                    // 添加用户
bool ModifyUserRole(QString userName, int newRoleNo);        // 修改用户角色
bool SetUserStatus(QString userName, bool userStatus);       // 设置用户状态
bool ResetUsetPw(QString userName);                          // 重置用户密码
#endif
