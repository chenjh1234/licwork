#ifndef SLIC_DATA_H
#define SLIC_DATA_H


#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QMutex>
#include <QSettings>

#include "LInfoMng.h"
#include "LVenderMng.h"
#include "LPackageMng.h"
#include "LVenderDef.h"
#include "LPackageDef.h"
#include "LLicFile.h"
#include "LLicEncrypt.h"
#include "SLog.h"
#include "DBstat.h"

#include "SAppMng.h"
#include "SPackMng.h"

#define ORG_NAME "GE"
#define SAPP_NAME "LIC_SERVER"
#define SAPP_VERSION "1.0"

#define DB_PTR "dbptr"
#define DB_REGISTER "dbname"

#define APP_LOG "alog.log"
#define ERR_LOG "elog.log"
#define PACKAGE_LOG "plog.log"
#define DB_FILE "fileDB.db"
#define DB_FILE_PTR "fileDB.db.ptr"
// all the things: needs threadsafe:
class SLicData
{
public:
    SLicData();
    ~SLicData();
    //LInfoMng _packMng;
// package manage:---------------------------------------------
    // add
    int addPackage(SPackInfo *info);
    // remove
    int removePackage(QString uuid);// move the pakage to packRemoved; lock in: from uncloadFile,call 
    int removePackage(QString packid,SPackInfo * inf);// move the pakage to packRemoved; base removed

    // test used
    int packSize(); /// number of packages registerd;
    QStringList packNames();
    SPackMng * packMng(QString packid);

    int removedSize();
    bool isUuid(QString packid,QString uuid);  // if uuid exist in the same packid in DB
    bool isVenderSign(QString vendername,QString venderSign);// if the vendersign is the same with the venderSign that in the same vender in DB
    QString encodePackageId(QString vender,QString package,QString version);

private:
    bool isPackidInPack(QString packid);// if packid=vender+ppackge+version exist in DB
    bool isPackageMsgFind(QString packid, QString msg ,QString value); // found if name = v exists in SPackInfo --->mapPack
    QString encodeAppId(QString ip,QString pid);
    QMap <QString ,SPackInfo *> findPackageInfo(QString msg, QString value);
public:
    // borrow:
    int borrow(SPackInfo *pack);
    int borrowReturn(SPackInfo *pack);
// app manage:--------------------------------------------
    int addApp(SAppInfo &app);  // >=0 OK
    int rmApp(SAppInfo &app);  

    int registerApp( SAppInfo & info);
    int releaseApp( SAppInfo & info);
    //
    int appSize();/// all apps registerd in the DB
    int appPackSize();
    QStringList appPacks();/// packages used of all registed apps.
    SAppMng * appMng(QString packid); //

// HH-------------------------------------------------------
    int appHB(SAppInfo &app);
    int ckeckHB();
// DB------------------------------------------------
    int saveDB();
    int loadDB();

    int saveDBMsg();
    int loadDBMsg();

    int registerDB();
    bool isDBRegisted();

    int clear();
    QString getDBFile();
    QString getDBFileIndex();
// util
    QString hex(QString);
    QString unHex(QString);
private:
// log :
    void plog(SPackInfo *info,QString str ="");
    void plog(QString s);

    void elog(QString s);
    void elog(SAppInfo *info,QString str ="");
    void alog(SAppInfo *info);

// statis
    DBsecond stApp,stAppErr;
    bool isPackidInApp(QString packid); 
// log object
    SLog _elog,_plog,_alog;
// mutex:
    QMutex _lockPack; //  for add ;remove 
    QMutex _lockApp;
    QMutex _lockLog;
    LFileDate _dt;

// data storage:
    QMap<QString,SPackMng *> mapPack;// packages in used , maped by name = venername + packagename +version;
    SPackMng packRemoved; // removed package table
    SPackMng borrowIn,borrowOut;
//
    QMap<QString,SAppMng *> mapApp;


};
#endif
