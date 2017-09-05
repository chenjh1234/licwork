#ifndef SAPP_INFO_H
#define SAPP_INFO_H
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>

#include "LInfoMng.h"
#include "LFileDate.h"

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
// for return:
#define APP_VENDERSIGN "vendersign"
#define APP_PBORROW "pborrow"


//#define APP_RET "ret"
//
//#define APP_TYPE "ptype"


#define APP_HB  30 //S
#define CHECK_HB 30*6 //180S = 3M

#if 0
#define APP_RTYPE_TASK 0
#define APP_RTYPE_USER 1
#define APP_RTYPE_NODE 2
#endif
#define APP_KEYS {(char*)APP_VENDER,(char*)APP_PACKAGE,(char*)APP_VERSION,(char*)APP_NAME,\
    (char *)APP_HOST,(char *)APP_IP,(char *)APP_USER,\
    (char *)APP_PID,(char *)APP_NUMBER,(char *)APP_STAT,\
    NULL} 

class SAppInfo : public LInfo
{
public:
    SAppInfo();
    ~SAppInfo();
    int appHB();
    bool checkHB(long intvs);
    int encode(QDataStream& ds);
    int decode(QDataStream& ds);
    bool isExpired(long intvs);
    QString packid;// vender+package+version
    QString appid;// ip+pid;
    QString user;// ip+pid;
    int number;
    long hbstat;
    long start,end;
    QString rtype;//APP_TYPE:PTYPE_NODE:TYPE_TASK:PTYPE_USER
    QString err;
    int ret;

  
private:
    void init();
    LFileDate fd;
  

};

#endif
