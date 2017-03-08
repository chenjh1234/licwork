#ifndef SLIC_DATA_H
#define SLIC_DATA_H


#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QMutex>

#include "LInfoMng.h"
#include "LVenderMng.h"
#include "LPackageMng.h"
#include "LVenderDef.h"
#include "LPackageDef.h"
#include "LLicFile.h"
#include "LLicEncrypt.h"

#include "SAppMng.h"
#include "SPackMng.h"
// all the things: needs threadsafe:
class SLicData
{
public:
    SLicData();
    ~SLicData();
    //LInfoMng _packMng;
// package manage:
    // add
    int addPackage(SPackInfo *info);
    // remove
    int removePackage(QString uuid);// move the pakage to packRemoved; lock in: from uncloadFile
    int removePackage(QString packid,SPackInfo * inf);// move the pakage to packRemoved; base removed

 
    bool isPackidInPack(QString packid);// if packid=vender+ppackge+version exist in DB
    bool isUuid(QString packid,QString uuid);  // if uuid exist in the same packid in DB
    bool isVenderSign(QString vendername,QString venderSign);// if the vendersign is the same with the venderSign that in the same vender in DB
    bool isPackageMsgFind(QString packid, QString msg ,QString value); // found if name = v exists in SPackInfo --->mapPack
    QString encodePackageName(QString vender,QString package,QString version);
    QMap <QString ,SPackInfo *> findPackageInfo(QString msg, QString value);
// app manage:
    int addApp(SAppInfo &app);   
    
    bool isPackidInApp(QString packid); 
// common
    //bool isInMap(QString packid);
    int registerApp(QString packid,QString ip,QString pid,QString user, int number);


// data storage:
    QMap<QString,SPackMng *> mapPack;// packages in used , maped by name = venername + packagename +version;
    SAppMng appMng; // app table
    SPackMng packRemoved; // removed package table
//
    QMap<QString,SAppMng *> mapApp;

    QMutex _lockPack; //  for add ;remove 
    QMutex _lockApp;

};
#endif
