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
