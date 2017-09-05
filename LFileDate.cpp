#include "LFileDate.h"
LFileDate::LFileDate()
{
   _fileN = "";
   _fileI = NULL;
}
void LFileDate::setFile(QString file)
{
   if (_fileI != NULL) delete _fileI;
   _fileN = file;
   _fileI = new QFileInfo(file);
   return;
}
/// inode of file
QString LFileDate::inode()
{
    return inode(_fileN);
}
/// inode of file
unsigned long  LFileDate::inodeLong(QString file)
{
   QString str ;
   struct stat buf;
   char *filen;
   unsigned long il; 
   if(!QFile::exists(file)) return 0;
   filen = (char *)file.Q2CH;
   stat(filen, &buf);
   il = (unsigned long)buf.st_ino;
   return il;
}
QString LFileDate::inode(QString file)
{
   QString str ;
   unsigned long il; 
   il = inodeLong(file);
   str.sprintf("%08X",(int)il);
   return str;
}
QString LFileDate::inodeFull(QString file)
{
   QString str ;
   unsigned long il; 
   int *pa1,*pa2;
  
   il = inodeLong(file);
   pa1 = (int *)&il;
   pa2 = (int *) (&il) + 1;
   //str.sprintf("%08X",il);// half;
   str.sprintf("%08X%08X",*pa2,*pa1);//long; 
 
   return str;
}
QString LFileDate::inodeDec(QString file)
{
   QString str ;
   unsigned long il; 
   int *pa1,*pa2;
   il = inodeLong(file);
   pa1 = (int *)&il;
   pa2 = (int *) (&il) + 1;
   str.sprintf("%ld",il);// dec;
   //str.sprintf("%08X08X",*pa2,*pa1);//long; 
   return str;
}
QString LFileDate::path(QString file)
{
    QString str;
    QFileInfo *fi;
    fi = new QFileInfo(file);
    str = fi->absoluteFilePath();
    delete fi;
    return str;
}
/// modify of file
QString LFileDate::TID(QString file)
{
   QString str;
   struct stat buf;
   char *filen;
   QDateTime dt;
   if(!QFile::exists(file)) return "";

   filen = (char *)file.Q2CH;

   stat(filen, &buf);
 //  int tc[2];
   long lt  = buf.st_mtime;
   //memcpy(tc, &lt, 8);
   //return tc[0] + tc[1];
   //str.sprintf("%08X%08X",tc[0],tc[1]);
   //qDebug() << " modify = " <<lt;
   //printf("inode =%08X%08X",tc[0],tc[1]);
   //printf("tid = %ld",lt);
   dt.setMSecsSinceEpoch((qint64)lt*1000) ;
   //qDebug() << "dt in TID =" << dt;
   str = DT(dt);

   return str;

}
/// modify of file
QString LFileDate::modify()
{
    return modify(_fileI);
}
/// modify of file
QString LFileDate::modify(QFileInfo *fi)
{
    if(fi == NULL) return "";
    QDateTime  dt;
    QString str;
    dt = fi->lastModified();
    //qDebug() << "dt = " << dt;
    //qDebug() << "dt.ttc = " << dt.toUTC();
    str = DT(dt);
    return str;
}
/// zise of file
QString LFileDate::modify(QString file)
{
    QString str;
    QFileInfo *fi;
    fi = new QFileInfo(file);
    return modify(fi);
}
/// modify of file
QString LFileDate::size()
{
    return size(_fileI);
}
/// size of file
QString LFileDate::size(QFileInfo *fi)
{
    qint64 len;
    QString str;
    len = fi->size();
    //qDebug() <<"len = " << len;
    str.sprintf("%ld",(long int )len);
    return str;
}
///size of file
QString LFileDate::size(QString file)
{
    QString str;
    QFileInfo *fi;
    fi = new QFileInfo(file);
    return size(fi);
}
/// is file exists
bool LFileDate::isFile(QString s)
{ 
    QFileInfo fi;
    fi.setFile(s);
    return fi.isFile();
}
// return the all test of the file
QString LFileDate::getText(QString s)
{
   QFile f;
   QString str ,rstr;
    
   rstr = "";
   f.setFileName(s);
   if (f.open(QIODevice::ReadOnly |QIODevice::Text )) 
   {
       #if 1
       QTextStream in(&f);     
       while (!in.atEnd()) 
       {
           str = in.readLine();
           rstr +=  str + "\n"; 
       } 
       #endif
     
       f.close();
   }
   return rstr; 
}
int LFileDate::writeText(QString file,QString text)
{
   QFile f;
 
   f.setFileName(file);
   if (f.open(QIODevice::WriteOnly |QIODevice::Text )) 
   {
       QTextStream out(&f);     
       out << text;
       f.flush();  
       f.close();
   }
   else
       return -1;
   return text.length(); 
}
int LFileDate::writeBuffer(QString file,char *buf,int len)
{
   QFile f;
   int i;
   i = -1;
   f.setFileName(file);
   if (f.open(QIODevice::WriteOnly | QIODevice::Truncate )) 
   {
       i = f.write(buf,len);
       f.close();
   }
   else return 0;
   return i; 
}
#ifdef MDS5_DEF
QString LFileDate::mds5(QString filen)
{
   LEncrypt en;
   int i;
   char ch[35];
   QFile f;
   QString str ,rstr;
   QByteArray ba;
   long sz;
   //qDebug() << "modify = " << TID(filen) << modify(filen)<<filen;
    
   rstr = "";
   f.setFileName(filen);

   if (f.open(QIODevice::ReadOnly)) 
   {
       
       ba = f.readAll();// readAll will change the file modify????,so chang to read;
       if (ba.size() >0) 
       {
           i = en.digest(ba.data(),ba.size(),ch);
           rstr = ch;
       }
       f.close();
   }
   //qDebug() << "modify1 = " << TID(filen) << modify(filen)<<filen;

   return rstr; 
}
QString LFileDate::mds5cc(QString filen)
{
#define MAXL 50000
   LEncrypt en;
   int i,ir;
   char buf[MAXL],ch[35];
   QFile f;
   QString str ,rstr;
   QByteArray ba;
   //long sz;
   //qDebug() << "modify = " << TID(filen) << modify(filen)<<filen;
    
   rstr = "";
   f.setFileName(filen);

   if (f.open(QIODevice::ReadOnly)) 
   {
       //sz = f.size();
       while (1)  
       {
           ir = f.read(buf,MAXL);
           if (ir >0)  
               ba.append(buf, ir); 
           if (ir != MAXL) break;
       }
       //ba = f.readAll();// readAll will change the file modify????,so chang to read;
       if (ba.size() >0) 
       {
           i = en.digest(ba.data(),ba.size(),ch);
           rstr = ch;
       }
       f.close();
   }
   //qDebug() << "modify1 = " << TID(filen) << modify(filen)<<filen;

   return rstr; 
}
QString LFileDate::mds5c(QString filen)
{
#define MAXL 50000
   LEncrypt en;
   int i,ir,iff;
   char buf[MAXL],ch[35];
   QFile f;
   QString str ,rstr;
   QByteArray ba;
   //long sz;
   //qDebug() << "modify = " << TID(filen) << modify(filen)<<filen;
    
   rstr = "";
   f.setFileName(filen);
   //qDebug() << "modify01 = " << TID(filen) << modify(filen)<<filen;

   iff = open(filen.Q2CH,O_RDONLY);
   if (iff >0)  
   {
       // qDebug() << "modify02= " << TID(filen) << modify(filen)<<filen;
        #if 1
       //sz = f.size();
       while (1)  
       {
           ir = read(iff,buf,MAXL);
           if (ir >0)  
               ba.append(buf, ir); 
           if (ir != MAXL) break;
       }
       // qDebug() << "modify03 = " << TID(filen) << modify(filen)<<filen;
       //ba = f.readAll();// readAll will change the file modify????,so chang to read;
       if (ba.size() >0) 
       {
           i = en.digest(ba.data(),ba.size(),ch);
           rstr = ch;
       }
       #endif

       close(iff);
   }
   //qDebug() << "modify4 = " << TID(filen) << modify(filen)<<filen;

   return rstr; 
}
QStringList LFileDate::cmd(QString cm)
{
    LEncrypt en;
    QString str;
    QStringList slist;
    string c,r;
    c = cm.Q2CH;
    r = en.cmd(c);
    str = r.c_str();
    slist = str.split("\n");
    return slist;
}
#endif
//===========================================================
/// date of yyyymmdd
QString LFileDate::getDateStr(int y,int m,int d)
{
    QString str;
    str = QString("%1%2%3").arg(y,4,10,QChar('0')).arg(m,2,10,QChar('0')).arg(d,2,10,QChar('0'));
    return str;
}
/// date time of yyyymmddhhmmss
QString LFileDate::DT(QDateTime t)
{
    QString str;
    str = t.toString(DT_FORMAT);
    return str;
}
/// current date time of yyyymmddhhmmss
QString LFileDate::curDT(int addS)
{
    QDateTime dt;
    dt = dt.currentDateTime();
    dt = dt.addSecs(addS);
    return DT(dt);
}
/// current   date of yyyymmdd 
QString LFileDate::curD()
{
    return curDT().left(8);
}
QString LFileDate::curM()
{
    return curDT().left(6);
}
QString LFileDate::curY()
{
    return curDT().left(4);
}
/// current time of hhmmsszzz
QString LFileDate::curTMS()
{
    QString str;
    QDateTime dt;
    dt = dt.currentDateTime();
    str = dt.toString(TMS_FORMAT);
    return str;
}
/// current time of hhmmsszzz
QString LFileDate::curT()
{   
    return curDT().mid(8,6);
}
/// counter of S since EP
long LFileDate::sEP()
{  
    return msEP()/1000;
}
/// counter of MS since EP
long LFileDate::msEP()
{
    QDateTime dt;
    dt = dt.currentDateTime();
    return dt.toMSecsSinceEpoch();
}
long LFileDate::sEP(QDateTime d)
{
    return msEP(d)/1000; 
}
long LFileDate::msEP(QDateTime d)
{
    return d.toMSecsSinceEpoch();  
}
/// counter of S since EP
QString LFileDate::sEPS()
{  
    long d;
    QString str;
    d =msEP()/1000;
    str.setNum(d);
    return str;
}
/// counter of MS since EP
QString LFileDate::msEPS()
{
    long d;
    QString str;
    d = msEP();
    str.setNum(d);
    return str;
}
/// EP(long) to DT(string)
QString LFileDate::EP2DT(long d)
{
    QDateTime dt;
//    dt.setTimeSpec(Qt::UTC);// important
    dt.setTime_t(d);
//    dt.setTimeSpec(Qt::LocalTime);
    //dt.setTime_t(d);
    //qDebug() << "setTime_t =" << dt.toString(); 
    return DT(dt);  
}
/// DT(string) to EP(long)   
long LFileDate::DT2EP(QString d)
{
    QDateTime dt;
    dt = dt.fromString(d,DT_FORMAT);
    return dt.toTime_t();  
}
/// seconds of (t1-t2)
int LFileDate::diffDT(QString t1,QString t2)
{
    long i;
    i = DT2EP(t1) - DT2EP(t2) ;
    return i;
}
int LFileDate::testFileCTime()
{
    QString filen;
    filen = "/tmp/_test_.tmp";
    return testFileCTime(filen);
}
/// test if new write data file ,cdate changed delay: 0 no delay:
int LFileDate::testFileCTime(QString filen)
{  
    QString str,str1,dt1,dt2;
    int len;
    len = 300;
    //filen = "/tmp/_test_.tmp";
    //filen = "/mnt/hgfs/win/_test_.tmp";
    str1.resize(len);
    writeText(filen,str1);
    dt1 = TID(filen); 
    sleep(2);
    dt2 = TID(filen); 
    //qDebug() << dt1 << dt2 << len;

    len = diffDT(dt1,dt2);
    //qDebug() << dt1 << dt2 << len;
    QFile::remove(filen);
    return len;
}
// rand:-----------------------------
/// return rand of 0-10;
int LFileDate::rand10()
{
    srand(time(0));
    return rand()%11;
}
/// return QString of length 0-10;
QString LFileDate::randText10()
{
    QString str;
    str = "111111111111";
    int len;
    len = rand10();
    if (len == 0 ) len = 1;
    str.resize(len);
    return str;
}
//port====================================
int LFileDate::getFreePort(int portStart,int portLen)
{ 
   int i,id;
   int fd ,iret;
   fd= socket(AF_INET, SOCK_STREAM, 0);
   //qDebug() << "fd == " << fd;
   struct sockaddr_in addr;
   bzero(&addr, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl (INADDR_ANY);
   iret = -1;
   for (i = portStart; i < portStart + portLen; i++)
   {

      addr.sin_port = htons(i);
      id = ::bind(fd, (struct sockaddr *)(&addr), sizeof(sockaddr_in));
      //qDebug() << "err=" << strerror (errno) << fd <<id ;
      if ( id < 0)
      {
         printf("port %d has been used.\n", i);
         continue;
      }
      close(fd);
      iret = i;
      break;
   }
   //qDebug() <<"port we used = " << i;
   return iret;
}
//passwd:====================================
QString LFileDate::passwdToday(QString h )
{
    string m;
    int i,n;
    QList<int> ilist;
    LFileDate fd;
    LEncrypt cr;
    QString seed,s,str;
    bool ok;
    seed = "mypasswd";  

    str = fd.curDT();
    str = str.left(8);// day;
    str = str + seed + h;
 
    m = cr.digest(str.Q2CH);
    s = m.c_str();
   
    str = getPassStr(s);
 
    return str ;

}
QString LFileDate::passwd2M(QString h)
{
    QString str;
    str = passwdM(h,60);// next M;
    return str ;

}
QString LFileDate::passwdM(QString h,int add )
{
    string m;
    int i,n;
    QList<int> ilist;
    LEncrypt cr;
    QString seed,s,str,str0;
    bool ok;
    seed = "mypasswd";  

    str = curDT(add);
    str0 = str.left(12);// M;
    //qDebug() << "passwdM " << str << curDT() <<add<< str0;

    str = str0 + seed + h;
    m = cr.digest(str.Q2CH);
    s = m.c_str();
    str = getPassStr(s);
    return str ;

}
QString LFileDate::getPassStr(QString dig )
{
    int i,n;
    QList<int> ilist;
    QString  s,str ;
    bool ok;
  
    s = dig;
    //qDebug() << "dig =" << s;
    for(i = 0; i <32 ;i = i+5)
    {
        str = s.mid(i,5);
        n = str.toInt(&ok, 16); 
        if (n <0) n = -1*n;
        ilist  << n- n/10*10;
    }
    n = ilist[0]*100000 + ilist[1]*10000 + ilist[2]*1000 + ilist[3]*100 + ilist[4]*10 + ilist[5];
    str =QString("%1").arg(n,6,10,QLatin1Char('0'));
    //qDebug() << "pass = " << str;
    return str ;

}
bool LFileDate::isPasswdToday(QString pass ,QString seed )
{
    QString str;
    str = passwdToday(seed);
    return (pass == str);
}
bool LFileDate::isPasswd2M(QString pass ,QString seed )
{
    QString str,str1;
    str = passwdM(seed);
    str1 = passwdM(seed,60);
    //qDebug() << str <<str1 << pass;
    return (pass == str || pass == str1);
}
 
#if 0
void fileTest()
{
    LFileDate f;
    QString str;
    QDateTime dt;
    str = "g.xml";
    qDebug() << " inode = " << f.inode(str);
    qDebug() << " TID = " << f.TID(str);
    qDebug() << " modify = " << f.modify(str);
    qDebug() << " size = " << f.size(str);
    //qDebug() << " TID0 = " <<dt.currentDateTime().toSecsSinceEpoch()   ; 
    qDebug() << " TID1 = " <<dt.currentDateTime().toMSecsSinceEpoch()   ; 
    qDebug() << " currentDT = " << f.curDT();
    qDebug() << " currentD = " << f.curD();
    qDebug() << " currentT = " << f.curT();
    qDebug() << " current sEP = " << f.sEP();
    qDebug() << " current msEP = " << f.msEP();
} 
#endif           