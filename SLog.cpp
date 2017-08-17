#include "SLog.h"
SLog::SLog()  
{
    init();
     
}
SLog::SLog(FILE *d)
{
    init();
    setFILE(d);
   
}
SLog::SLog(QString filen) 
{
    init();
    setFile(filen);
}
int SLog::setFile(QString filen) 
{  
    _filename = filen;
    _logFile.setFileName(filen);
    
    if (_logFile.open(QFile::WriteOnly | QFile::Append))
    {
        _ts = new QTextStream(&_logFile);
    }
    else
    {
        cout << "Error open log file error " << filen.Q2CH << endl;
        return -1;
    }
    return 0;
}
int SLog::setString(QString &str) 
{ 
    if (_ts != NULL) delete _ts;
    _ts = new QTextStream(&str);
    return 0; 
}
int SLog::setFILE(FILE *f) 
{ 
    if (_ts != NULL) delete _ts;
    _ts = new QTextStream(f);
    return 0;
}
void SLog::close()
{
        qDebug() << "ddddddddd-----dddddd";
    if (_ts != NULL) 
    {
        _ts->flush();
        delete _ts;      
        //qDebug() << "ddddddddddddddd";
    }
    _ts = NULL;
    
    if (_logFile.isOpen()) 
        _logFile.close(); 
     
}
SLog::~SLog()
{
    close(); 
}
void SLog::init()
{
    _filename = "";
    _logText = "";
    _space = true;
    _ts = NULL;
    setUser(curUser());
}
//-----------------------------------------
void SLog::line(char const* text, ...)
{
    _lock.lock();
    char ch[LINE_MAXCH];
    va_list argList;
    va_start(argList,text);
    //qDebug() << "22222\n";
    vsprintf(ch,text,argList);
    //qDebug() << "22222--\n";
    *this << ch << "\n";
    _ts->flush();
    //qDebug() << "22222==\n";
    _lock.unlock();
    // qDebug() << "22222\n";
}

void SLog::linen(char const* text, ...)
{
    _lock.lock();
    char ch[LINE_MAXCH];
    va_list argList;
    va_start(argList,text);
    vsprintf(ch,text,argList);
    *this << ch ;
    _ts->flush();
    _lock.unlock();
}
void SLog::ts(QString s)
{
    tline("%s",s.Q2CH);
}
void SLog::tline(char const* text, ...)
{
    _lock.lock();
    char ch[LINE_MAXCH];
    QString str;
    va_list argList;
    va_start(argList,text);
    vsprintf(ch,text,argList);
    str= curTime();
    //qDebug() << "intline = " << str<< ch;
    *this << str <<  ch << "\n";
    _ts->flush();
    _lock.unlock();
}
void SLog::tlinen(char const* text, ...)
{
    _lock.lock();
    QString str;
    char ch[LINE_MAXCH];
    va_list argList;
    va_start(argList,text);
    vsprintf(ch,text,argList);
    str= curTime();
    *this << str << ch ;
    _ts->flush();
    _lock.unlock();
}
 
QString SLog::curTime()
{
   LFileDate fd;
   return fd.curDT();
}
QString SLog::curUser()
{

#ifdef WIN32
        return getenv("USERNAME");
#else
        return getenv("USER");
#endif
}
QString SLog::user()
{
    return _user;
}
QString SLog::filename()
{
    return _filename;
}
void SLog::setUser(QString s)
{
    _user = s;
}
 
