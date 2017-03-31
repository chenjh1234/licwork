#ifndef SPACK_MNG_H
#define SPACK_MNG_H


#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QMutex>
#include "LInfoMng.h"
#include "SPackInfo.h"
#include "SAppMng.h"
/// repack the package;
#if 0
struct packBlock
{
    SPackInfo  *info;
    int start;
    int len;
};
#endif
class PResource
{
public:
    PResource() ;
    int limit;
    int used;
    QString type;
//
    int size();
    int add(SPackInfo *inf);
    int remove(SPackInfo * inf);
    int changed(SPackInfo * inf,int change);
    SPackInfo * find(SPackInfo * inf);
    int appRequest(SAppInfo &info,SAppMng *mng);
    int appRelease(SAppInfo &info,SAppMng *mng);
    int encode(QDataStream &ds);
    int decode(QDataStream &ds);
    void clear();

    SPackInfo * getInfo(int used);// which info we register app;
    QList<SPackInfo* > pList;
};

class SPackMng : public LInfoMng
{
public:
    SPackMng();
    ~SPackMng();
 
    int appRequest(SAppInfo &info,SAppMng * mng);// pointer: maybe NULL
    int appRelease(SAppInfo &info,SAppMng * mng);
    //
    int addInfo(SPackInfo *inf);
    int removeInfo(SPackInfo *inf);
    int clear();
    // 
    int encode(QDataStream &ds);
    int decode(QDataStream &ds);
    //-------
    int taskSize();
    int nodeSize();
    int userSize();

    int taskLimit();
    int nodeLimit();
    int userLimit();

    int taskUsed();
    int nodeUsed();
    int userUsed();

    PResource * getPResource(QString ty);
private:
    PResource rnode,rtask,ruser;
    void init();
    QMutex _lock;
   
  

};

#endif

