#include "SAppMng.h"

SAppMng::SAppMng():LInfoMng()
{
    init();
}
SAppMng::~SAppMng()
{

}

void SAppMng::init()
{

}
int SAppMng::encode(QDataStream &ds)
{
    int i;
    SAppInfo *info;

    ds << size();
    for(i = 0; i < size();i++)
    {
        info = (SAppInfo *)get(i);
        if (info !=NULL) 
        {
            info->encode(ds);
        }
        else
        {
            return -1;
        }
    }
    return size();
 
}
int SAppMng::decode(QDataStream &ds)
{
    int i,sz;
    SAppInfo *info;

    ds >>sz;
    for(i = 0; i <sz ;i++)
    {
        info = new SAppInfo();
        if (info !=NULL) 
        {
            info->decode(ds);
            if (info == NULL) return -1;
            if (add(info) <= 0) return -1; 
        }
        else return -1;
    }
    return sz;
}
