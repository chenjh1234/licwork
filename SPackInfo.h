#ifndef SPACK_INFO_H
#define SPACK_INFO_H


#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include "LPackageDef.h"
#include "LInfoMng.h"
#include "LFileDate.h"
#define VENDERSIGN "vendersign"
// for borrow out:
#define BMID "bmid" //for borrow out : server ID of the borrow in server;for borrowin :server ID of borrowO out
  
// values define:
//PBORROW:  
#define BORROW_YES "yes"
#define BORROW_NO "no"  

#define PBORROW_SIZE 5

#define PTYPE_TASK "task"
#define PTYPE_NODE "node"
#define PTYPE_USER "user"

#define STAT_OFF 0 // before startdate,limit = 0;
#define STAT_OK 1  // ok limit= LIMIT
#define STAT_EXP -1// exp limit = 0;
#define STAT_UNLOAD -2// unloaded limit = 0;




class SPackInfo : public LInfo
{
public:
    SPackInfo();
    ~SPackInfo();
    //int request(QString type, int number);
    int encode(QDataStream& ds);
    int decode(QDataStream& ds);
    
    bool isBorrowIn();
    bool isBorrowOut();

    int checkStat();// check if stat changed 0:unchanged ,?chengd circle use
    int getStat();// get the real stat
    int setStat();// set limit if exped ,off  ;change limit acording to real stat:
    int setUnload();// set unload( if stat ok)
    QString statStr();

    QString packid,err;
    int ret;// for 
    int limit;// limit of the license
    int start;// start number in resource
    int stat;// OFF|OK|EXP|UNLOAD

  
private:
    void init();
  

};
#endif
