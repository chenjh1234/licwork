#ifndef SLIC_DATA_H
#define SLIC_DATA_H


#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QMutex>
#include <QSettings>
#include <QDir>

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

#define PASSWD_PROOF "0123456"
#define DELIMIT_PROOF "%%"

#define LIC_ROOT_PATH "LIC_ROOT" //  licdatadir 
#define LIC_DB_PATH "db" //  $LIC_ROOT/db
#define LIC_LOG_PATH "log" //  $LIC_ROOT/log
#define LIC_FILE_PATH "file" //  $LIC_ROOT/file

#define DB_PTR "dbptr"
#define DB_REGISTER "dbname"

#define ALOG "ALOG"
#define ELOG "PLOG"
#define PLOG "ELOG"

//#define APP_LOG "alog.log"
//#define ERR_LOG "elog.log"
//#define PACKAGE_LOG "plog.log"

#define DB_FILE "fileDB.db"
#define DB_FILE_PTR "fileDB.db.ptr"

#define DB_PACKFILE "packDB.db"
#define DB_PACKFILE_PTR "packDB.db.ptr"

#define DBDATE_MIS 10
// all the things: needs threadsafe:
class SLicData
{
public:
    SLicData();
    ~SLicData();
    //LInfoMng _packMng;
// package manage:---------------------------------------------
// interface: add remove unload: add a saveDBPackage()
    // add
    int addPackage(SPackInfo *info);//return size of info in mng
    // remove
    int unloadPackage(QString uuid,QString filen);// move the pakage to packRemoved; lock in: from uncloadFile,call 
    int removePackage(QString uuid);// move the pakage to packRemoved; lock in: from uncloadFile,call 
    int removePackage(QString packid,SPackInfo * inf);// move the pakage to packRemoved; base removed // return size of info in mng;
    int unloadPackage(QString packid,SPackInfo * inf);// set the info limit = 0;  

    int createProof(QString prooffile,SPackInfo * inf);// inf:create unloaded package proof file,usally for borrow In package ,unalod and return to the borrow Out server
    int verifyProof(QString filen,SPackInfo * &info);// inf:found the Package borrowOut,for borrow return;

    // test used
    int packSize(); /// number of packages registerd;
    QStringList packNames();
    SPackMng * packMng(QString packid);

    int removedSize();
    QStringList removedPackages();
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
    int borrowReturn(QString filen);
// app manage:--------------------------------------------
    int addApp(SAppInfo &app);  // >=0 OK return size of info in mng;
    int rmApp(SAppInfo &app);  

    int registerApp( SAppInfo & info);
    int releaseApp( SAppInfo & info);
    //
    int appSize();/// all apps registerd in the DB
    int appPackSize();
    QStringList appPacks();/// packages used of all registed apps.
    SAppMng * appMng(QString packid); //

// HB,check cycle:------------------------------------------------------
    int appHB(SAppInfo &app);

    int checkHB(long intvs);//return number of expired
    int checkHB(QString pack,long intvs);
    int checkPackExp();    // return :number of expired
    int checkPackExp(QString packID);// return number of pack expired;

    int checkAppExp(long intvS);//return number of app expired;
    int checkAppExp(QString packID,long intvs);
// DB------------------------------------------------
    int saveDB();//return bytes of filePtr
    int loadDB();
     
    // apck app:
    int saveDBPack(QDataStream &ds);
    int saveDBApp(QDataStream &ds);
    int loadDBPack(QDataStream &ds);
    int loadDBApp(QDataStream &ds);


    int saveDBMsg();
    int loadDBMsg();
    int saveDBMsg(QString fileDB,QString filePtr);
    int loadDBMsg(QString fileDB,QString filePtr);

    int registerDB();
    bool isDBRegisted();

    int clear();
    QString getDBFile();
    QString getDBFileIndex();
   // forPackageOnly:
    int saveDBPackage();
    int loadDBPackage();
    QString getDBPackFile();
    QString getDBPackFileIndex();

// util
    QString hex(QString);
    QString unHex(QString);
// pathname:
    QString curLogName(QString ty);// usrrent elog filename ,elog20170101.log ,month changed the log changed
    int changeLogNames();// check if the log filename(month) changed,if changed newit:
    // dir:
    QString rootDir();
    QString dbDir();
    QString fileDir();
    QString logDir();
    //path:
    QString dbName();
    QString dbPtrName();
    QString dbPackageName();
    QString dbPackagePtrName();
    bool mkDir(QString dir);// make sure the dir exist;

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
