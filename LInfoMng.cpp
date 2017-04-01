#include "LInfoMng.h"

LInfo::LInfo()
{

}
LInfo::~LInfo()
{
    clear();
}
/// operator =
LInfo & LInfo::operator= (LInfo &info)
{
    //LInfo *ret;
    int i;
    QStringList slist;
    QVariant v;
    slist = info.nameList();
    //ret = new LInfo();

    //qDebug() << "=,size=" << info.size() << size();
    //qDebug() << "nlist=" << slist;
    clear();

    for(i = 0; i < info.size();i++)
    {
        v = info.get(slist[i]);
        set(slist[i],v);
    }
    nameid = info.nameid;
    //qDebug() << "ret = " << ret.nameList();

    return *this;
}
LInfo & LInfo::operator+(LInfo &info )
{
    *this += info;
    return *this;
}
LInfo & LInfo::operator+= (LInfo &info )
{
     //LInfo *ret;
    int i;
    QStringList slist;
    QVariant v;
    slist = info.nameList();
    //ret = new LInfo();

   // qDebug() << "size=" << info.size();
    //qDebug() << "nlist=" << slist;

    for(i = 0; i < info.size();i++)
    {
        v = info.get(slist[i]);
        set(slist[i],v);
    }
    nameid = info.nameid;
    //qDebug() << "ret = " << ret.nameList();

    return *this;
} /// remove all keys 
void LInfo::clear()
{
    m_map.clear();
}
///  if the name = value exist
bool LInfo::isEQ(QString n,QVariant v)
{
    QVariant v1;
    v1 = get(n);
    if (v1 == v) return true;
    else 
        return false;
}
/// get the list of names
QStringList LInfo::nameList()
{
    QStringList slist;
    slist = m_map.keys();
    return slist;
}
/// remove by name
int LInfo::remove(QString name)
{
    return m_map.remove(name);
}

/// set name = value
int LInfo::set(QString name,QVariant v)
{
    //qDebug() << name << v ;//<< m_map;
    m_map[name] = v;
    //qDebug() << name << v;
    return 0;
}
/// get the value of name
QVariant  LInfo::get(const char * ch)
{
    QString str;
    str = ch;
    return get(str);

}
/// get the value of name
QVariant  LInfo::get(QString n)
{
    QVariant v;
    if (m_map.contains(n))  
        return m_map[n];
    else
        return  v;
}
/// if the name(key) exist;
bool LInfo::isKey(QString str)
{
    return m_map.contains(str);
}
QString LInfo::getText()
{
        //LInfo *ret;
    int i;
    QStringList slist;
    QVariant v;
    slist = nameList();
    QString s,s1;
    s = "";
    qDebug() << "getText sz = " << size();
    for(i = 0; i < size();i++)
    {
        s1 = slist[i] + "=" + get(slist[i]).toString() + "\n";
        s = s +s1;
    }
    return s;
}
/// get name = value string lines, according given array names:array
QString LInfo::getText(char ** array)
{
    QString ret ,str,str1;
    char **ch = array;
    int i;     
    ret = "";
    i=0;
    str1 = "";
    while (ch[i] != NULL) 
    {
        str = ch[i];
        if (isKey(str)) 
        {
            str1 = str + " = "  + m_map[str].toString() + "\n";
            ret = ret + str1;
        }
        
        i++;
    }
    return ret;
    
}
void LInfo::encode(QDataStream &ds)
{
    int i;
    QStringList slist;
    slist = nameList();

    ds << size();
    ds << nameid;
    for(i = 0; i < size();i++)
    {
        ds << slist[i] << get(slist[i]).toString();
    }
 
}
void LInfo::decode(QDataStream &ds)
{
    int i,sz;
    QStringList slist;
    QString s1,s2;
    slist = nameList();
    ds >> sz;
    ds >> nameid;
    for(i = 0; i < sz ;i++)
    {
        ds >>s1>>s2;
        set(s1,s2);
    }
}
// infomng:==========================================================
LInfoMng::LInfoMng()
{
    setRemoveOption(false);

}
LInfoMng::~LInfoMng()
{
    //qDebug() << this << size() << "clear -------------------------------bg\n";
    clear(); //  we cannot delete ,because the PTR Info in the infoMng mybe a local address; cannot delete;
    //qDebug() << this << size() <<  "clear1 -------------------------------bg\n";
}
/**
 *  b= true: default,  delete LInfo * pointer when rmInfo(): \n
 *      when we add  LInfo * from new :
 *      info = new LInfo();add(info).
 *      otherwise: Abort (core dumped)
 *  
 *  
 *  b= false:    donnot delete LInfo * pointer when rmInfo() :\n
 *  when we use LInfo * locally: LInfo info;add(&info).
 *  otherwise: delete the LInfo * pointer mannually,when
 *  rmInfo()
 *  
 * 
 *  
 */
void LInfoMng::setRemoveOption(bool b)
{
    _removeOption = b;

}
/// get name = value string lines,of all info in infom=mng, according given array names: cc
QString LInfoMng::getText(char ** cc)
{
    QString str,ret;
    int i;
    LInfo *info;
    ret = "";
    for (i = 0; i <size(); i++) 
    {
        info = get(i);
        str = info->getText(cc);
        ret  = ret + str;
    }
    return ret;
}
QString LInfoMng::getText()
{
    QString str,ret;
    int i;
    LInfo *info;
    ret = "";
    qDebug() << "size = " << size();
    for (i = 0; i <size(); i++) 
    {
        qDebug() << i;
        info = get(i);
        str = info->getText();
        ret  = ret + str;
    }
    return ret;
}
/// remove by Info,return index of the info 
int LInfoMng::rmInfo(LInfo *info)
{
    int i;
    LInfo *inf;
    for (i = 0; i <size();i++) 
    {
        inf = get(i);
        //qDebug() << "rmInfo inf =" <<inf <<info;
        if (inf == info) 
        {
            rmInfo(i);
            return i;
        }
    }
    return -1;
}
/// rm info by nameid  
int LInfoMng::rmInfo(QString name)
{
    int idx;
    idx = find(name.Q2CH);
    return rmInfo(idx);
}
/// rm info by nameid  
int LInfoMng::rmInfo(char *name)
{
    int idx;
    idx = find(name);
    return rmInfo(idx);
}
/// rm info by idx; 
///@return the size of Mng
int LInfoMng::rmInfo(int idx)
{
   LInfo *info;
   if (size() == 0)  return -1;
   if (idx >=0 && idx < size() )   
   {
       
       info = m_list[idx]; 
       //qDebug() << "info =" << info;
       m_list.removeAt(idx);
       //qDebug() << "1111\n";
       if (removeOption())
       {
           if (info != NULL)  
               delete info; // is dangrous if the info is local; not from new;
       }
       //qDebug() << "222\n";
       return size();
   }
   else 
       return -1;
}
/// empty the mng
int LInfoMng::clear()
{
    return rmAll();
}
/// empty the mng
int LInfoMng::rmAll()
{
   
   int i,sz;
   int len;
   len = size();
   //qDebug() << "sz= " << len;
   for(i = 0; i < len ; i++)
   {
       if (size() <=0) break;
       //qDebug() << "i = " << i;
       sz = rmInfo(0);   
       //qDebug() << "i1 = " << i;  
   }
   //qDebug() << "ii = " ;
   return size();

}
/// number of info in the infoMng
int LInfoMng::size()
{
   return m_list.size();
}
/// find index  of info by name=v && name = v && name =v  infoMng
int LInfoMng::find(QString n,QVariant v,QString n1,QVariant v1,QString n2,QVariant v2) 
{
   int i,idx;
   LInfo *info;
   idx = -1;
   for(i = 0; i < size() ; i++)
   {
       info = m_list.at(i) ;
       //qDebug() << info->isEQ(n, v) << info->isEQ(n1, v1) << info->isEQ(n2, v2);

       if (n1 == QString(""))  
       {
           if (info->isEQ(n, v))  
               idx = i;
       }
       else if (n2 == QString(""))
       {
           if (info->isEQ(n, v) && info->isEQ(n1, v1))  
               idx = i;
       }
       else
       {
           if (info->isEQ(n, v) && info->isEQ(n1, v1) && info->isEQ(n2, v2))  
               idx = i;
       }
        
        
       if (idx != -1)  break;
   }
   return idx;
}
/// find index of LInfo by nameid;
int LInfoMng::find(QString name)
{
    return find(name.Q2CH);
}
/// find LInfo by nameid;
int LInfoMng::find(char *name)
{
   int i,idx;
   QString str;
   str = name;
   LInfo *info;
   idx = -1;
   for(i = 0; i < size() ; i++)
   {
       info = m_list.at(i) ;
       if (info->nameid == str)  
       {
           idx = i;
           break;
       }
   }
   return idx;
}
/// add a LInfo  to infoMng;  the info cannot be local &info, must be info = new LInfo(); otherwise it will be core down;
int LInfoMng::add(LInfo *info)
{
   m_list.append(info);
   return m_list.size();
}
/// get LInfo*  by index;
LInfo* LInfoMng::get(int i)
{
    if (i >=0 &&i < size()) 
    { 
         return m_list[i];
    }
    else
        return NULL;
}
/// get LInfo*  by nameid;
LInfo* LInfoMng::get(QString name)
{
    return get(name.Q2CH);

}
/// get LInfo*  by nameid;
LInfo* LInfoMng::get(char *name)
{
   return get(find(name));// no need to if
}
 /// the number of char in array :cc
int LInfoMng::lenOf(char ** cc)
{
   char **dirs= cc;
   int i = 0;
   while (dirs[i] !=NULL) 
      i++;
   return i;
}
/// if the key in array :array
bool LInfoMng::isKeyValid(char * key,char ** array)
{
    QString str,str1;
    char **ch = array;
    int len;     
    int i,iret ;

    len = lenOf(ch);
    iret = -1;
    str = key;

    for (i = 0; i <len; i++) 
    {
        str1 = ch[i];
        if (str1 == str)  
        {
            iret = i;
            break;
        }
    }
    if (iret == -1) return false;
    else return true;
}

/// if all keys(names) in array:ch  exists in the infoMng; all  the info in the infoMng have the key : mains the key exist
bool LInfoMng::isKeysExist( char ** ch)
{
    int i,j,len,len1 ;
    bool b;
    QString str;

    LInfo *iv;
    b = true;

 
    len1 = lenOf(ch);
    len = size();
    //qDebug() << "len,len1(char) = " <<  len << len1;

    for (i = 0 ; i < len1; i++) //size of infoMng
    { 
        str = ch[i];
        for (j = 0; j <len; j++) // len of keys
        {
            
            iv = get(j);
            if (!iv->isKey(str)) 
            {
                b = false;
                cout << KEY_NOT_EXIST << str.Q2CH <<endl;
            }                        
        }
    }
    return b;
}
LInfoMng & LInfoMng::operator= (LInfoMng  &mng)
{    
   int i;
   int len;
   //LInfoMng *ret;
   LInfo *info,*infoNew,infoo;
   //ret = new LInfoMng();

   len = mng.size();
   //qDebug() << "mng size = " << len << &mng;
   for(i = 0; i < len ; i++)
   {
         //qDebug() << "infoget0 " ;
          info = mng.get(i);
         // qDebug() << "infoget1 " ;
          infoNew = new LInfo();
          // qDebug() << "infoget2 " ;
          *(infoNew) = *info;
          //infoo = *info;
          //qDebug() << "info = " <<info<< info->nameList();
          //qDebug() << "infoN = " <<infoNew << infoNew->nameList();
          add(infoNew);
        
   }
   return *this;
    //qDebug() << "ret size = " << ret.size();
    //qDebug() << "ret  " << ret.size();
 
}
int LInfoMng::encode(QDataStream &ds)
{
    int i;
    LInfo *info;

    ds << size();
    for(i = 0; i < size();i++)
    {
        info = get(i);
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
int LInfoMng::decode(QDataStream &ds)
{
    int i,sz;
    LInfo *info;

    ds >>sz;
    for(i = 0; i <sz ;i++)
    {
        info = new LInfo();
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
#if 0 //  if one of the info has the key : means the key exist
bool LInfoMng::isKeysExist( char ** ch)
{
    int i,j,len,len1 ;
    bool b,bb;
    QString str;

    LInfo *iv;
    b = true;

 
    len1 = lenOf(ch);
    len = size();
    //qDebug() << "len,len1(char) = " <<  len << len1;

    for (i = 0 ; i < len1; i++) //size of infoMng
    { 
        str = ch[i];
        bb = false;
        for (j = 0; j <len; j++) // len of keys
        {
            
            iv = get(j);
            if (iv->isKey(str)) 
                bb = true;
            
        }
        if (!bb)
        {
                cout << KEY_NOT_EXIST << str.Q2CH <<endl;
                b = false;         
        }
    }
    return b;
}
#endif
