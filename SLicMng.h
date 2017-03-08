#ifndef SLIC_MNG_H
#define SLIC_MNG_H


#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QMap>


#include "SLicData.h"
#define OK 0
#define LOADFILE_FILE_ERR -1
#define LOADFILE_VENDERKEY_ERR -2
#define LOADFILE_PACKAGEKEY_ERR -3
#define LOADFILE_MID_ERR -4
#define LOADFILE_VENDERSIGN_ERR -5
#define LOADFILE_UUID_ERR -6
#define LOADFILE_LOAD_ERR -7

#define LOADFILE_ERR { \
    (char *)"Load file: openfile ok",\
    (char *)"Load file: openfile  error!!",\
    (char *)"Load file: verify venderKey error!!",\
    (char *)"Load file: verify packageKey error!!",\
    (char *)"Load file: MID error!!",\
    (char *)"Load file: venderSign error!!",\
    (char *)"Load file: uuid error,the licFile already loaded!!",\
    (char *)"Load file: load error,add to database error!!",\
    NULL}

class SLicMng
{
public:
    SLicMng();
    ~SLicMng();
   
    //
    int loadFile(QString filename,QString serverPri);
    int checkLicFile(QString filename,QString serverPri);
    //
    //int unloadPackage(QString vender,QString package,QString version,QString serverPri);
    int unloadFile(QString filename,QString serverPri);
    //
    int loginApp(SAppInfo &msg);
    int logoutApp(SAppInfo &msg);
    //
    int heartBeat(QString vender,QString package,QString version,QString host,QString pid);
    //
    int borrow(QString vender,QString package,QString version,int number,QString mid);
    //SLicData * getData(){return _data;};
    //void setData(SLicData* dt){_data = dt;};
// check:
    bool isUuid(QString packid,QString uuid);  
    bool isVenderSign(QString vendername,QString venderSign);
    // encode:
    

    static SLicData * data;
//private:
    void init();
    QMap<int,QString>  mapLoadFile;// for load file error;

};


#endif
