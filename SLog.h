#ifndef SLOGFILE_H
#define SLOGFILE_H

using namespace std;

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <stdarg.h>
#include <iostream>
#include <sys/time.h>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include "LFileDate.h"
#define LINE_MAXCH 102400
#define TSNULL if(_ts ==NULL) return *this;

class SLog   
{
public:
    
    QDateTime tm;
    SLog(QString filen);
    SLog(FILE *);
    SLog();
    //SLog(const SLog &log):_ts(log._ts){};
    ~SLog();
    int setFile(QString filen);
    int setString(QString &str);
    int setFILE(FILE *);
    // thread safe:
    void line(char const* text, ... );
    void tline( char const* text, ... );
    void ts(QString);
    void linen( char const* text, ... );
    void tlinen( char const* text, ... );
    // this is not thread safe:
    QString curTime();
    QString user();
    QString filename();
    QString curUser();
    void setUser(QString s);
    void close();
    QTextStream *_ts;
    
    inline SLog &space() {TSNULL _space = true; *_ts << " "; return *this; }
    inline SLog &nospace() { _space = false; return *this; }
    inline SLog &maybeSpace() {TSNULL if (_space) *_ts << " "; return *this; }
 //   inline SLog & operator<<(QString &t) { *_ts << t ; return maybeSpace(); };
    inline SLog & operator<<(QString t) { TSNULL *_ts << t ; return maybeSpace(); };
    inline SLog & operator<<(int t) {TSNULL *_ts << t ; return maybeSpace(); };
    inline SLog & operator<<(long t) {TSNULL *_ts << t; return maybeSpace(); };
    inline SLog & operator<<(unsigned int t) {TSNULL *_ts << t ; return maybeSpace(); };
    inline SLog & operator<<(unsigned long  t) {TSNULL *_ts << t ; return maybeSpace(); };
    inline SLog & operator<<(float  t) {TSNULL *_ts << t ; return maybeSpace(); };
    inline SLog & operator<<(double t) {TSNULL *_ts << t ; return maybeSpace(); };
    inline SLog & operator<<(char  t) {TSNULL *_ts << t ; return maybeSpace(); };
    inline SLog & operator<<(const char * t) {TSNULL *_ts << QString::fromUtf8(t) ; return maybeSpace(); };
    //inline SLog & operator<<(QString &t) { *_ts << t ; return maybeSpace(); };
    inline SLog & operator<<(const void *t) {TSNULL *_ts << t ; return maybeSpace(); };
    //inline SLog &operator<<(const QStringRef & t) { return operator<<(t.toString()); }
    inline SLog &operator<<(QTextStreamFunction f) {TSNULL *_ts << f;return *this;};
    inline SLog &operator<<(QTextStreamManipulator f) {TSNULL *_ts << f;return *this;};
    //inline SLog &operator=(const SLog &other);
    template <class T>
        inline SLog &operator<<(const QList<T> &list);
    template <class aKey, class aT>
        inline SLog &operator<<(const QMap<aKey, aT> &map);

     
 
private:
    void init();
    QFile _logFile;
    QString _logText;
    QString _filename,_user;
   
   
    QMutex _lock;
    bool _space;
};

#if 0
inline SLog &SLog::operator=(const SLog &other)
{
    #if 1
    if (this != &other) 
    {
        *_ts = *other._ts;
    }
    #endif
    return *this;
}
#endif
#if 1
template <class T>
inline SLog & SLog::operator<<(const QList<T> &list)
{
    #if 1
        //*_ts << nospace() << '(';
    for (typename QList<T>::size_type i = 0; i < list.count(); ++i) 
    {
        *this  << list.at(i);
    }
    #endif
    return *this;
}
#endif
#if 1
template <class aKey, class aT>
inline SLog & SLog::operator<<(const QMap<aKey, aT> &map)
{
     
    for (typename QMap<aKey, aT>::const_iterator it = map.constBegin();
         it != map.constEnd(); ++it) {
        *this  << it.key()  << it.value() ;
    }
    return *this;
}

#endif
#endif
