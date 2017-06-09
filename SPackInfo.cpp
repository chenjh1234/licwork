#include "SPackInfo.h"

SPackInfo::SPackInfo():LInfo()
{
    init();
}
SPackInfo::~SPackInfo()
{

}

void SPackInfo::init()
{
    limit = 0;
    err = "";
    start = 0;
    ret= 1;
    stat = 0;
}
/// get real stat;;
int SPackInfo::setStat()
{
    int iflag ;
    iflag = getStat();
    if (iflag == STAT_OFF || iflag == STAT_UNLOAD || iflag == STAT_EXP) 
    {
        stat = iflag;
        limit = 0;
    }
    else if (iflag == STAT_OK) 
    {
        stat = iflag;
        limit = get(PLIMIT).toInt();;
    }
    return 1;
}

/// get real stat;;
int SPackInfo::setUnload()
{
    int iflag ;
    iflag = getStat();
    if (iflag == STAT_OK) 
        stat = STAT_UNLOAD;
    return setStat();
}
/// get real stat;;
int SPackInfo::getStat()
{
 
    LFileDate fd;
    QString str,str1,start,end;
    int iflag;
    str = fd.curDT();
    start = get(PSTARTDATE).toString();
    end = get(PENDDATE).toString();
    if (str < start ) 
        iflag = STAT_OFF;
    else if (str >= start && str <= end) 
    {
        if (iflag != STAT_UNLOAD)  
            iflag = STAT_OK;   
    }
    else 
        iflag = STAT_EXP;
    return iflag;
}
/// @return 
/// 0: unchanged; n, limit changed 
int SPackInfo::checkStat()
{
     
    int i, iflag;
    iflag = getStat();
 // if changed
    if ( iflag == stat ) return 0;
    if (stat == STAT_OFF && iflag == STAT_OK) return limit;
    if (stat == STAT_OK && iflag == STAT_EXP ) return -1*limit;
    if (stat == STAT_OK && iflag == STAT_UNLOAD) return -1*limit;
    return 0;
}
bool SPackInfo::isBorrowIn()
{
    QString str,bmid;
    int len = 5;
    str = get(PBORROW).toString();
    bmid = get(BMID).toString();
    if (str.length() > len && bmid.length() > len) return true;
     return false;
 
}
bool SPackInfo::isBorrowOut()
{
    QString str,bmid;
    int len = 5;
    bmid = get(BMID).toString();
    //qDebug() << "isBorrowOut =" << bmid << bmid.length() << limit;
    if (bmid.length() > len && limit < 0 ) return true;
    return false;

}
int SPackInfo::encode(QDataStream& ds)
{
    LInfo::encode(ds);
    ds << packid;
    ds << err;
    ds << ret;
    ds << limit;
    ds << start;
    ds << stat;
    return 1;
    #if 0
    QString packid,err;
    int ret;
    int limit;
    int start;
    #endif
}
int SPackInfo::decode(QDataStream& ds)
{

    LInfo::decode(ds);
    ds >> packid;
    ds >> err;
    ds >> ret;
    ds >> limit;
    ds >> start;
    ds >> stat;
    return 1;

}
