#include "SCycleThread.h"

SCycleThread::SCycleThread():QThread()
{
    rHB.setRun(HEARTBEAT_INT);
    rDbSave.setRun(DB_SAVE_INT);
    rDayCheck.setRun(DAY_CHECK_INT);
    rAppExp.setRun(APP_EXP_INT);
    setDown(0);// not down
    init();
    
}
int SCycleThread::init()
{
     
    _dbSave =  lfd.sEP(); 
    _appExp =  lfd.sEP();
     //start of the day
  #ifdef TEST_CYCLE
    _dayCheck = lfd.sEP();
  #else
    _dayCheck =  lfd.curD().toLong(); 
  #endif
#if 0
    QDateTime dt;
    dt.setTimeSpec(Qt::UTC);
    dt.setTime_t(0);
    dt.setTimeSpec(Qt::LocalTime);
    

    qDebug() << " app_pack = " << _dbSave << _packExp << lfd.EP2DT(_dbSave) << lfd.EP2DT(_packExp)<< lfd.EP2DT(0) << dt<<dt.toLocalTime();
    qDebug() << " curDT== " << lfd.curDT() << lfd.EP2DT(_dbSave) ;

    QDateTime UTC(QDateTime::currentDateTimeUtc());
    QDateTime local(UTC.toLocalTime());
    qDebug() << "UTC time is:" << UTC;
    qDebug() << "Local time is:" << local;
    qDebug() << "No difference between times:" << UTC.secsTo(local);

#endif
}
int SCycleThread::setDown(int d)
{
    _down = d;
    return _down;
}
int SCycleThread::setDown()
{
    _down = CYCLE_DOWN;
    return _down;
}
bool SCycleThread::isDown()
{
    return _down == CYCLE_DOWN;
}
void SCycleThread::run()
{
    long dc;//daycheck
    int ic;
    init();

    while (1) 
    {
        ic = 0;
        qDebug() << "HB_INT =" << HEARTBEAT_INT ;//<< TEST_CYCLE;
        while (ic < HEARTBEAT_INT) 
        {
            sleep(1); // delay maxmun = 1;
            ic++;
            if (isDown()) break;
        }
        if (isDown())  
        {
            qDebug() <<  "i down----------------------";
            break;
        }
        //HB:
        runHB();
        // dbsave:
        if (lfd.sEP() - _dbSave >= DB_SAVE_INT) 
        {
            _dbSave = lfd.sEP();
            runDBSave();
        }
        //appExp:
        if (lfd.sEP() - _appExp >= APP_EXP_INT) 
        {
            _appExp = lfd.sEP();
            runAppExp();
        }
        //day check: packExp;log
        #ifdef TEST_CYCLE
             dc = lfd.sEP();
        #else
             dc =  lfd.curD().toLong(); 
        #endif
        if (dc - _dayCheck >= DAY_CHECK_INT) 
        {
            _dayCheck = dc;
            runDayCheck();
        }
 
    }
    rHB.wait();
    rDbSave.wait();
    rAppExp.wait();
    rDayCheck.wait();

    //qDebug() <<  "i go out----------------------";
}
// -------------run ------------------
int SCycleThread::runHB()
{
    
   rHB.start();
   
   return 1;
}
int SCycleThread::runDBSave()
{
    rDbSave.start();
    return 1;
}
int SCycleThread::runDayCheck()
{
    rDayCheck.start();
    return 1;

}
int SCycleThread::runAppExp()
{
   rAppExp.start();
   return 1; 
}
 
//============runThread========================
runThread::runThread():QThread()
{
#ifdef TEST_CYCLE 
    stt = lfd.sEP();
#endif
}
int runThread::setRun(int mode)
{
    _who = mode;
    return mode;
}

QString  runThread::who()
{
    QString str;
    if (_who == HEARTBEAT_INT)  str = "HB";
    if (_who == DB_SAVE_INT)  str = "DBSAVE";
    if (_who == DAY_CHECK_INT)  str = "PACK_EXP";
    if (_who == APP_EXP_INT)  str = "APP_EXP";
    return str;
}
void runThread::run()
{
    SLicMng mng;
    int i;
    qDebug() << "who = " << who() ;

#ifdef TEST_CYCLE 
    qDebug() << "who = " << who()  << lfd.sEP() - stt;
    ilist << lfd.sEP() - stt;
    //return;
#endif

    if (_who == HEARTBEAT_INT) 
    {
        i = mng.data->checkHB(HEARTBEAT_INT);
    }
    #if 1
    else if (_who == DB_SAVE_INT) 
    {
        i = mng.data->saveDB();    
    }
    else if (_who == DAY_CHECK_INT) 
    {
        i = mng.data->changeLogNames();
        i = mng.data->checkPackExp();
    }
    else if (_who == APP_EXP_INT) 
    {
        i = mng.data->checkAppExp(APP_EXP_INT);
    }
    #endif
    #ifdef TEST_CYCLE 
    nList << i;
    #endif
    qDebug() << "thread ok ===" << who() << i;
}
      
