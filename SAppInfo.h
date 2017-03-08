#ifndef SAPP_INFO_H
#define SAPP_INFO_H
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>

#include "LInfoMng.h"

#define APP_VENDER "vender"
#define APP_PACKAGE "package"
#define APP_VERSION "version"
#define APP_NAME "appname"
#define APP_HOST "hostname"
#define APP_IP "ip"
#define APP_USER "user"
#define APP_PID "pid"
#define APP_NUMBER "number"
#define APP_STAT "stat"



#define APP_KEYS {(char*)APP_VENDER,(char*)APP_PACKAGE,(char*)APP_VERSION,(char*)APP_NAME,\
    (char *)APP_HOST,(char *)APP_IP,(char *)APP_USER,\
    (char *)APP_PID,(char *)APP_NUMBER,(char *)APP_STAT,\
    NULL} 

class SAppInfo : public LInfo
{
public:
    SAppInfo();
    ~SAppInfo();
    QString packid;// vender+package+version
    QString appid;// ip+pid;
    int number;
    int hbstat;
    int startMS,endMS;
  
private:
    void init();
  

};

#endif
