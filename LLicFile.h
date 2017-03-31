#ifndef LLIC_FILE_H
#define LLIC_FILE_H


#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>

#include "LInfoMng.h"
#include "LVenderMng.h"
#include "LPackageMng.h"
#include "LVenderDef.h"
#include "LPackageDef.h"
#include "LEncrypt.h"


#define ENDL "\n"
#define REM_CHAR "#"   //for comment
#define SPE_CHAR "="   // seprate key and values

#define VENDERNAME  "vendername"
#define PACKAGENAME  "packagename"

#define NO_VENDERNAME " No venderName appeared met packagename"
#define INVALID_KEY " Err: is a invalid key :"
#define VENDER_REM "#  vender info: venderName mush be the firstline\n";
#define PACKAGE_REM "#  package info: packageName mush be the firstline\n";


class LLicFile
{
public:
    LLicFile();
    ~LLicFile();
    int readFile(char *file);
    int readFile(QString str);
    //int read(char *file);
    int writeFile(char *file);
    int writeFile(QString str);
    int writeStream(QTextStream *outs);
    int readStream(QTextStream *in);
    LInfo *  package(int i);
    LInfo *  vender(int i);
    LInfo *  vender();
    LVenderMng& venderMng(){return _vender;};
    LPackageMng& packageMng(){return _package;};
    int size();
    void clear();
     
    // is the key valid:
   // bool isVenderKeyValid(char *key);
    //bool isPackageKeyValid(char *key);
    // is the vender &package key exist:
    bool isKeysExist();
    //bool isKeysExist( char ** ch, LInfoMng *im);
    //bool checkKey(char *k,char **keys);
    //bool isKeyValid(char *k,char **keys);

   // int  lenOf(char ** cc);
 
private:
    LVenderMng  _vender;
    LPackageMng _package;
};
#endif
#if 0

# licfile examples:
# 
#  vender info: venderName mush be the firstline

venderName = geoeast
clientName = bgp 
serverName =   12345
serverID =   12345
uuid = 699a6047-90e7-4f1d-b3d0-f377587362d6
borrow = on  #on|off
venderKey = 123

#  package info: packageName mush be the firstline version the 2nd

packageName = pc1
version  = 1.0
# type: task|user|node
type = task
limit = 10
startDate = 20170101
endDate = 20180101
key = 123

#  package info: packageName mush be the firstline version the 2nd

packageName = pc2
version  = 1.0
# type: task|user|node
type = task
limit = 10
startDate = 20170101
endDate = 20180101
key = 123

#endif
