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
QString LFileDate::inode(QString file)
{
   QString str ;
   struct stat buf;
   char *filen;
   int ia;
   filen = (char *)file.Q2CH;

   stat(filen, &buf);
   ia = (unsigned int)buf.st_ino;
   str.sprintf("%08X",ia);
   //printf("inode = %08X\n", ia);
   return str;
}
/// modify of file
QString LFileDate::TID(QString file)
{
   QString str;
   struct stat buf;
   char *filen;
   QDateTime dt;

   filen = (char *)file.Q2CH;

   stat(filen, &buf);
 //  int tc[2];
   long long lt  = buf.st_mtime;
   //memcpy(tc, &lt, 8);
   //return tc[0] + tc[1];
   //str.sprintf("%08X%08X",tc[0],tc[1]);
   //qDebug() << " modify = " <<lt;
   //printf("inode =%08X%08X",tc[0],tc[1]);
   dt.setMSecsSinceEpoch((qint64)lt*1000) ;
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
    if(fi == NULL) return NULL;
    QDateTime  dt;
    QString str;
    dt = fi->lastModified();
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
       QTextStream in(&f);     
       do
       {
           str = in.readLine();
           rstr +=  str + "\n"; 
       }while(!str.isNull());
       f.close();
   }
   return rstr; 
}
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
QString LFileDate::curDT()
{
    QDateTime dt;
    dt = dt.currentDateTime();
    return DT(dt);
}
/// current   date of yyyymmdd 
QString LFileDate::curD()
{
    return curDT().left(8);
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
    return curDT( ).mid(8,6);
}
/// counter of S since EP
qint64 LFileDate::sEP()
{  
    return msEP()/1000;
}
/// counter of MS since EP
qint64 LFileDate::msEP()
{
    QDateTime dt;
    dt = dt.currentDateTime();
    return dt.toMSecsSinceEpoch();
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