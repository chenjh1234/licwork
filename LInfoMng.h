#ifndef INFOMNG_H
#define INFOMNG_H
#include <QDebug>
#include <QMap>
#include <QList>
#include <QVariant>
//#include "comfun.h"
#include <iostream>
using namespace std;

#ifndef Q2CH
    #define Q2CH toUtf8().data()
#endif

#define KEY_NOT_EXIST " Err: Key is not exist: "
/// store a map of name = value paires,if LInfo as a param ()it must be &,return as well
/// LInfo & getSome(LInfo &aa);
class LInfo
{
public:
    LInfo();
    ~LInfo();
    QString nameid;

    int set(QString s,QVariant v);
    QVariant  get(QString);
    QVariant  get(const char * ch);
    QStringList nameList();
    int remove(QString);
    bool isEQ(QString n,QVariant v);
    int size(){ return m_map.size();};
    bool isKey(QString str);
    QString getText();
    QString getText(char **ch);
    // param must be &, when &, the address is the same as caller ,and donot release it when end of this function
    //                  when not &, the address is differ from  caller ,and  release it when end of this function
    //                 infact not &, it will copy the param ,but it donot know how to copy ,if no operator =;
    LInfo & operator= (LInfo &info);
    LInfo & operator+= (LInfo &info );
    LInfo & operator+ (LInfo &info );
    //LInfo  operator= (LInfo info);
    void clear();
    QMap<QString ,QVariant> map(){return m_map;};
 
 
//protected:
    QMap<QString ,QVariant> m_map;
     
};
/**
 *  store LInfo pointer ine QLlist;
 *  becareful: removeOptin(b)
 *  b= true:
 *      default,  delete LInfo * pointer when rmInfo(): \n
 *      when we add  LInfo * from new : info = new
 *      LInfo();add(info). otherwise: Abort (core dumped)
 *  
 *  
 *  b= false:    donnot delete LInfo * pointer when rmInfo() :\n
 *  when we use LInfo * locally: LInfo info;add(&info).
 *  otherwise: delete the LInfo * pointer mannually,when
 *  rmInfo()

 *   if LInfoMng as a param ()it must be &,return as well 
 *  LInfoMng & getSome(LInfoMng &aa);
 */

class LInfoMng
{
public:
    LInfoMng();
    ~LInfoMng();
    bool removeOption(){return _removeOption;};
    void setRemoveOption(bool b);
    int rmInfo(char *name);
    int rmInfo(LInfo *info);
    int rmInfo(QString name);
    int rmInfo(int idx);//is dangrous if the info is local; not from new; we setRemoveOption = false; not delete LInof * wehn rmInfo;
    int rmAll();
    int clear();
    int size();
    int find(char *name);
    int find(QString name);
    int add(LInfo *info);// the info cannot be local, must be info = new LInfo(); otherwise it will be core down;
    int find(QString n,QVariant v,QString n1=QString(""),QVariant  v1=0,QString n2=QString(""),QVariant v2=0);// &&
    LInfo * get(int i);
    LInfo * get(char *name);
    LInfo * get(QString name);
    QString getText(char ** cc);

    int  lenOf(char ** cc);
    bool isKeysExist(char ** ch);
    bool isKeyValid(char * key,char ** array);

    LInfoMng &operator= (LInfoMng &mng);
    //bool checkKey(char *k,char **keys);
protected:
    QList<LInfo *> m_list;
    bool _removeOption;
     
};
#endif
