#include "SPackMng.h"

SPackMng::SPackMng():LInfoMng()
{
    init();
}
SPackMng::~SPackMng()
{

}

void SPackMng::init()
{

}
int SPackMng::request(QString ip,QString pid, QString user,int number)
{
    #if 0
    _lock.lock();
    SPakcInfo *info;
    int i,sz,left,ret;
    int n,bn;
    n = 0;// number of
    bn = 0;

    sz = mng->size();
// if individual enough
    for (i = 0; i <sz;i++) 
    {
        info = mng->get(i);
        left = info->limit - info->used;
        if (left >= number) 
        {
            ret = number;
            break;
        }
    }

    _lock.unlock();
    #endif

}
