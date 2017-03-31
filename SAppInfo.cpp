#include "SAppInfo.h"

SAppInfo::SAppInfo():LInfo()
{
    init();
}
SAppInfo::~SAppInfo()
{

}

void SAppInfo::init()
{
    err = "";
    number = 0;
    hbstat = 0;
    start = 0;
    end = 0;
    ret = 1;

}


int SAppInfo::appHB()
{
    hbstat = fd.sEP();
    return hbstat;
}
bool SAppInfo::checkHB()
{
    if(hbstat ==0)
    {
        hbstat = fd.sEP();
    }

    if (fd.sEP() - hbstat > CHECK_HB ) return true;
    else return false;
}
int SAppInfo::encode(QDataStream& ds)
{
    LInfo::encode(ds);
    ds << packid;
    ds << appid;
    ds << user;
    ds << number;
    ds << qint64(hbstat);
    ds << qint64(start)<<qint64(end);
    ds << rtype;
    ds << err;
    ds << ret;
    return 1;


    #if 0
        QString packid;// vender+package+version
    QString appid;// ip+pid;
    QString user;// ip+pid;
    int number;
    long hbstat;
    long start,end;
    QString rtype;//APP_TYPE:PTYPE_NODE:TYPE_TASK:PTYPE_USER
    QString err;
    int ret;
    #endif
}
int SAppInfo::decode(QDataStream& ds)
{
    LInfo::decode(ds);
    qint64 hb,s,e;
    ds >> packid;
    ds >> appid;
    ds >> user;
    ds >> number;
    ds >> hb;
    ds >> s >> e;
    ds >> rtype;
    ds >> err;
    ds >> ret;

    hbstat = hb;
    start  = s;
    end = e;

    hbstat = 0;
    return 1;

}
