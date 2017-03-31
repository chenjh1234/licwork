#include "SLicData.h"
SLicData ::SLicData()
{
   _elog.setFile(ERR_LOG);
   _alog.setFile(APP_LOG);
   _plog.setFile(PACKAGE_LOG);
#if 0
   _elog<< "1234567"<<endl;
   QString s;
   s = "56789111";
   _elog.tline("9999999");
   _elog.ts(s);
   s = "567890";
   elog(s);
   //_elog<< "1234567----"<<endl;
#endif

}
SLicData::~SLicData()
{
   qDebug() << " =======end of licData=======";
}
// ===================pack ===========

int SLicData::packSize()
{
   return mapPack.size();
}
/// package manager identified by pack id:
SPackMng* SLicData::packMng(QString packid)
{
    if (mapPack.contains(packid))  
        return mapPack[packid]; 
    else
        return NULL;
}
/// package names in this db
QStringList SLicData::packNames()
{
   QStringList slist;
   slist = mapPack.keys();
   return slist;
}
/// removed packge size;in this session
int SLicData::removedSize()
{
   return packRemoved.size();
}
//------------ -----------------------app-----------------
/// all apps in the DB
int SLicData::appSize()
{
   //return mapApp.size();
   int i, sz, ret;
   sz = mapApp.size();
   QStringList slist;
   slist = mapApp.keys();
   ret = 0;
   for (i = 0; i < sz; i++)
   {
      ret += mapApp[slist[i]]->size();

   }
   return ret;
}
/// app managers
SAppMng* SLicData::appMng(QString packid)
{
     if (mapApp.contains(packid))  
        return mapApp[packid]; 
    else
        return NULL;
}
/// app packs
QStringList SLicData::appPacks()
{
   QStringList slist;
   slist = mapApp.keys();
   return slist;
}
int SLicData::appPackSize()
{
   return mapApp.size();
}
//=================================borrow=====================
int SLicData::borrow(SPackInfo *pack)
{
    QString str,packid,dt;
    // found package for borrow
    /// packid,borrow,type,uuid,limit,date,left
    int i,sz,left;
    SPackMng* mng;
    mng = packMng(pack->packid);
    if (mng == NULL)  
    {
        pack->ret = -1;
        pack->err = " Error : cant not found packid:" + pack->packid;
        return -1;
    }
    // fonnd mng:
    sz = mng->size();
    SPackInfo *info;
    dt = _dt.curDT();

    for (i = 0; i < sz; i++) 
    {
        info = (SPackInfo*) mng->get(i);
        if (info == NULL ) continue;
        if (info->get(BORROW).toString() != BORROW_YES) continue;
        if (info->get(PTYPE).toString() != pack->get(PTYPE).toString()) continue;
        if (info->get(UUID).toString() != pack->get(UUID).toString()) continue;
        if (info->limit <= pack->limit) continue;
        if (dt < pack->get(PSTARTDATE).toString() || dt > pack->get(PENDDATE).toString() ) continue; // today:
        if (pack->get(PSTARTDATE).toString() < info->get(PSTARTDATE).toString() ||  pack->get(PENDDATE).toString() > info->get(PENDDATE).toString() ) continue; // today:
        left = (mng->getPResource(pack->get(PTYPE).toString()))->limit - (mng->getPResource(pack->get(PTYPE).toString()))->used;
        if (left >= pack->limit) 
        { 
            // yes: found it:
            //limit = limit *-1
            str = pack->get(PLIMIT).toString();
            str = "-"+str;
            pack->set(PLIMIT,str);
            pack->limit = -1*pack->limit;
            // vendersign:
            str = info->get(VENDERSIGN).toString();
            pack->set(VENDERSIGN,str);
            // pborrow:
            str = info->get(PBORROW).toString();
           
            pack->set(PBORROW,str);
            // add a -limit pacakge:
            left = addPackage(pack);
            if (left < 0) 
            {
                pack->err = " Error : cant not found package that have enouph license left:" + pack->packid;
            }
            // log:
            pack->ret = left;
            plog(pack,"borrow");
            return left;
        }
    }
    pack->ret = -1;
    pack->err = " Error : cant not found package that have enouph license left:" + pack->packid;
    plog(pack,"borrow");
    return -2;
}
int SLicData::borrowReturn(SPackInfo *pack)
{

}

// =======add package==============interface=======
/// add a package;
int SLicData::addPackage(SPackInfo *info) // base add
{

   QString packid, str;
   SPackMng *mng;
   int i;
   i = 0;
   // lock;
   _lockPack.lock();
   packid = info->packid;
   //qDebug() << "addPackage ,nameid = " << nameid;
   if (isPackidInPack(packid))
   {
      qDebug() << "addPackage yes= " << packid;
      mng = mapPack[packid];
      if (mng != NULL)
      {
         i = mng->addInfo(info);
      }
   }
   else // new package:
   {
      qDebug() << "addPackage no= " << packid;
      mng = new SPackMng();
      i = mng->addInfo(info);
   }
   // add to map:
   mapPack[packid] = mng;
   // unlock:
   _lockPack.unlock();
   //if (i <=0 ) info->err = "addPackage err";
   info->ret = i;
   plog(info, "addPackage");
   return i;

}
//-------remove packInfo * from packMng:--internal use
/// return i  index of info in mng <0 err
int SLicData::removePackage(QString packid, SPackInfo *info) // base remove
{
   int i;
   SPackMng *mng;
   QStringList slist;
   // lock:
   _lockPack.lock();
   // lock:
   //_lockPack.lock();
   mng =  mapPack[packid];
   //qDebug() << "removeP mng =" << mng << packid<< mng->size();
   i = mng->removeInfo(info);
   //qDebug() << "removeP return ==" << i;
   if (i >= 0)
   {
      //info->nameid = packid;
      packRemoved.add(info); // move to packRemoved
      if (mng->size() == 0)
      {
         mapPack.remove(packid); // remove this mng from map if mng.size() ==0
         delete mng; // remove the mng pointer as well
      }
   }
   // unlock
   _lockPack.unlock();

   info->ret = i;
   plog(info, "removePackage");
   return i; // index of info in mng <0 err
}
//----------remove uuid---------for unload license file------------------------------
/// remove info from uuid: return ret removed info
int SLicData::removePackage(QString uuid)
{
   int i, sz, ret, ret1;
   QMap<QString, SPackInfo *> umap;
   SPackInfo *info;
   QStringList slist;
   QString err, str;

   umap = findPackageInfo(UUID, uuid);

   sz = umap.size();
   ret = sz;

   //qDebug() << "umap.sz = " << sz;
   if (sz > 0)
   {
      slist = umap.keys();
      //qDebug()<< "find uuid ,slist= " << slist ;
      for (i = 0; i < sz; i++) // all uuid in differnt mng;
      {
         info = umap[slist[i]];
         ret1 = removePackage(slist[i], info);
         //qDebug()<< "inloop " << i << ret;
         if (ret1 < 0)
         {
            ret = i;
            //info->ret = ret1;
            //info->err += "removePackage(QString uuid) packid = " + slist[i];
            //plog(info);
            break;
         }
         else
         {
            //str = "OK removePackage(QString uuid) packid = " + slist[i];
            // plog(str);
         }
      } //

   }
   else
   {
      err = "Error:  removePackage(QString uuid): not find uuid";
      plog(err);
   }
   // log
   if (ret == sz && ret != 0)
   {
      err = "OK:  removePackage(QString uuid): is OK removed package number = " + QString("%1 ").arg(sz) + uuid;
      plog(err);
   }
   else
   {
      err = "ERR:  removePackage(QString uuid): is Err removed package number = " + QString("%1 ").arg(ret) + uuid;
      plog(err);
   }
   return ret;
}
//============================================service functions===================
/// if packid in exist in the mapPack
bool SLicData::isPackidInPack(QString packid)
{
   if (mapPack.contains(packid)) return true;
   else return false;
}
/// if packid in exists in the mapApp
bool SLicData::isPackidInApp(QString packid)
{
   if (mapApp.contains(packid)) return true;
   else return false;
}

/// found msg = name  in PackInfo in the PackMng that have the same packid
QMap<QString, SPackInfo *> SLicData::findPackageInfo(QString msg, QString value)
{
   SPackMng *mng;
   SPackInfo *info;
   QStringList slist;
   QString v;
   QMap<QString, SPackInfo *> imap;

   int sz, i, j;
   sz = mapPack.size();
   slist = mapPack.keys();
   //qDebug() << "isVenderSign sz = " << sz;
   if (sz == 0)  return imap; // empty

   for (i = 0; i < sz; i++) // found all SPackMng in map
   {
      mng = mapPack[slist[i]];
      j = mng->find(msg, value);
      if (j >= 0)
      {
         info = (SPackInfo *)mng->get(j);
         imap[slist[i]] = info;
         //qDebug() << "info = " << info;
      }
   }
   return imap;
}

/// found msg = name  in PackInfo in the PackMng that have the same packid
bool SLicData::isPackageMsgFind(QString packid, QString msg, QString value)
{
   // infact it for uuid only:
   SPackMng *mng;
   int i;
   if (!isPackidInPack(packid)) return false;
   mng = mapPack[packid];
   i = mng->find(msg, value);
   //qDebug()<< "msg = " << i;
   if (i >= 0) return true;
   return false;
}
/// if uuid is exist in the packageMng in map
bool SLicData::isUuid(QString packid, QString uuid)
{
   QString msg;
   msg = UUID;
   return  !isPackageMsgFind(packid, msg, uuid); // if found return false ,not
}
/// if venderSign is the same with the exists in the packageMng
bool SLicData::isVenderSign(QString vendername, QString venderSign)
{
   SPackMng *mng;
   SPackInfo *info;
   QStringList slist;
   QString sign, vname;

   int sz, i;
   sz = mapPack.size();
   slist = mapPack.keys();
   //qDebug() << "isVenderSign sz = " << sz;
   if (sz == 0)  return true; // no this vender:

   for (i = 0; i < sz; i++) // found all SPackMng in map
   {
      mng = mapPack[slist[i]]; // in the same packageID, get only one ,all the vendersign in this mng will be the same
      info = (SPackInfo *)mng->get(0);
      sign = info->get(VENDERSIGN).toString();
      vname = info->get(VENDERNAME).toString();
      // qDebug() << "sign = " << sign << venderSign;
      if (vname == vendername)
      {
         if (sign == venderSign) return true;
         else return false;
      }

   }
   return true; // not found;

}
/// packid = vender + package + version:
QString SLicData::encodePackageId(QString vender, QString package, QString version)
{
   QString str, dl;
   dl = "_";
   str = vender + dl + package + dl +   version;
   return str;
}
/// appid = ip+pid;
QString SLicData::encodeAppId(QString ip, QString pid)
{
   QString str, dl;
   dl = "_";
   str = ip + dl + pid;
   return str;
}
//=app =================== =register APP==============================================
/// appInfo request Package resources;
int SLicData::registerApp(SAppInfo& info)
{
   int i;
   QString packid, err;
   SPackMng *mng;
   SAppMng *appmng;
   packid = info.packid;
   qDebug() << "packid=" << packid;

   i = -1;
   if (isPackidInPack(packid))
   {
      if (isPackidInApp(packid)) // is packid in appMng
      {
         appmng = mapApp[packid];
      }
      else
      {
         appmng = NULL;
      }
      //
      mng = mapPack[packid];
      //qDebug() << "appReuest0" << appmng;

      i = mng->appRequest(info, appmng);
      //qDebug() << " i = mng->appRequest(info,appmng)" << i;
   }
   else // no this package
   {
      err = "registerApp Error:no this package name = " + packid;
      info.err = err;

   }
   //qDebug() << "222";
   return i;
}
//--------------------------------------------------release App----
/// appInfo release the  Package resources;
int SLicData::releaseApp(SAppInfo& info)
{

   int i;
   QString packid;
   SPackMng *mng;
   SAppMng *appmng;
   packid = info.packid;
   qDebug() << "packid=" << packid;

   i = -1;
#if 0   // we donot care if apckid exist or removed:
   if (!isPackidInPack(packid))
   {
      err = "releaseApp Error: no this package in packMng name = "+packid;
      info->err = err;
   }
#endif
   if (isPackidInApp(packid)) // is a registerd packid
   {
      appmng = mapApp[packid]; // appmng
      mng = mapPack[packid];   // packmng;
      i = mng->appRelease(info, appmng);
   }
   else
   {
      info.err  = "releaseApp Error: no this package in appMap name = " + packid;
   }
   return i;
}
//===========================add app===========================
/**
 * @brief
 * 1: new a SAppinfo ,and  copy the info data;
2:registerApp(info *)
3:insert the info to appMng

 
 * @return int 
 */
int SLicData::addApp(SAppInfo& ainfo)
{
   int i, number;
   LFileDate fd;
   QString str;
   QString vender, package, version, packid, pid, ip, user;
// check App info
   SAppInfo *info;
   SAppMng *mng;
   mng = NULL;

   info = new  SAppInfo();
   *info = ainfo;

   vender = info->get(APP_VENDER).toString();
   package = info->get(APP_PACKAGE).toString();
   version = info->get(APP_VERSION).toString();
   number = info->get(APP_NUMBER).toInt();
   ip = info->get(APP_IP).toString();
   pid = info->get(APP_PID).toString();
   user = info->get(APP_USER).toString();
   //qDebug() << "packid00=" << vender << package << version ;
   qDebug() << "info=" << info->getText();
   // qDebug() << "ainfo=";// << ainfo.getText();
   packid = encodePackageId(vender, package, version);

   info->packid = packid;
   info->number = number;
   info->appid = encodeAppId(ip, pid);
   info->user = user;
   info->nameid = info->appid;
   info->start = fd.sEP();
  // qDebug() <<  "i0 =registerApp( *info); =" << i;

   i = registerApp(*info);

   //qDebug() <<  "i1 =registerApp( *info); =" << i << packid << info->appid;
   if (i >= 0)
   {
// add to map:
      _lockApp.lock();
      //info->startMS =
      info->number = number;
      // qDebug() << "return = " << i;

      if (isPackidInApp(packid))
      {
         mng = mapApp[packid];
         if (mng != NULL)
         {
            i = mng->add(info);
         }

      }
      else
      {
         mng = new SAppMng();
         i = mng->add(info);
         mapApp[packid] = mng;
      }
      _lockApp.unlock();
      //qDebug() << "return = " << i;
   }
   else // failed
   {
      info->ret = i;
      elog(info);
      delete info;
      info = NULL;
   }

   //qDebug()<< "info   ==" << info->appid << info->err;
//qDebug() << "return1 = " << i;
   if (info != NULL)
   {
      info->ret = i;
      info->end = fd.sEP();

      elog(info);
   }
   //qDebug() << "return2 = " << i;
   return i; // >=0 OK <0 err;
}
/// get packid from input AppInfo, get and remove  *info from appMng ,and call releaseAPP;
int SLicData::rmApp(SAppInfo& ainfo)
{
   int i, number;
   QString str, err;
   QString vender, package, version, packid, pid, ip, user;
// check App info
   SAppInfo * info,*rmInfo;
   SAppMng *mng;
   mng = NULL;
   err = "";

   info = &ainfo;


   vender = info->get(APP_VENDER).toString();
   package = info->get(APP_PACKAGE).toString();
   version = info->get(APP_VERSION).toString();
   number = info->get(APP_NUMBER).toInt();
   ip = info->get(APP_IP).toString();
   pid = info->get(APP_PID).toString();
   user = info->get(APP_USER).toString();
   //qDebug() << "packid00=" << vender << package << version ;
   // qDebug() << "info=" << info->getText();
   // qDebug() << "ainfo=" << ainfo.getText();
   packid = encodePackageId(vender, package, version);
// packid,appid
   info->packid = packid;
   info->number = number;
   info->appid = encodeAppId(ip, pid);
   info->nameid = info->appid;

   _lockApp.lock();
   i = -1;


   rmInfo = (SAppInfo *)((LInfoMng *)(mapApp[info->packid]))->get(info->appid);
   if (rmInfo != NULL) // found it:
   {
      i = ((LInfoMng *)(mapApp[info->packid]))->rmInfo(rmInfo);
      if (i < 0)
      {
         err = QString("Error: cannot remove Info form appMng; ") + " appid= " + info->appid + " packid = " + info->packid;
      }
      else
      {
         i = releaseApp(*rmInfo);
         err = "Error: releaseApp(*rmInfo)" + rmInfo->err + " appid= " + info->appid + " packid = " + info->packid;
      }

   }
   else
   {
      err = "Error: rmApp: not found appid = " + info->appid + " packid = " + info->packid;
   }

   _lockApp.unlock();

   qDebug() <<  "i =releaseApp( *info); =" << i << packid;

   //info->err += err;

   if (i < 0)
   {
      elog(err);
   }
   else
   {
      alog(rmInfo);
      delete rmInfo; // remove the appInfo if ok;
   }
   return i; // >=0 OK <0 err;
}
// ============log==========================================================
/// plog info
void SLicData::plog(SPackInfo *info, QString fun)
{
   QString str;
   if (info->ret <= 0)
   {

      str = "Error:" +  info->packid + ":" + info->err;
      _plog.ts(str);
   }
   else
   {
      str =  fun + " OK:" + info->packid + " " + info->get(PTYPE).toString() + " " + info->get(PLIMIT).toString() + " " + info->get(PSTARTDATE).toString() + " " + info->get(PENDDATE).toString() + " " + info->get(UUID).toString();
      _plog.ts(str);
   }
}
/// plog string
void SLicData::plog(QString s)
{
   _plog.ts(s);
}
//------------------------
/// elog string for app
void SLicData::elog(QString s)
{
   _elog.ts(s);
}
/// elog info;
void SLicData::elog(SAppInfo *info, QString fun)
{
   if (info->ret > 0) return;

   QString str;
   str = "Error: " +  info->packid + " " + info->appid + " " + info->user + " " + info->err;
   //qDebug() << "elog = " <<str ;
   _elog.ts(str);
}

//---------alog---------------
/// app log :when app succsess complete

void SLicData::alog(SAppInfo *info)
{
   _lockLog.lock();
   QString str;
   str = _dt.curDT();
   _alog << str;
   _alog << info->packid << info->appid << info->user << info->start << info->end << info->end - info->start << info->number << info->rtype << endl;
   _lockLog.unlock();
}
//====================================saveDB==============loadDB============
int SLicData::saveDB()
{
   int sz, i;
   QString fileDB;
   QStringList slist;

   fileDB = getDBFile();
   QFile file(fileDB);
   if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return -1;

   QDataStream ds(&file);
// pack:
   SPackMng *pmng;
   slist = packNames();
   ds << slist;

   sz = packSize();
   for (i = 0; i < sz; i++)
   {
      pmng = mapPack[slist[i]];
      pmng->encode(ds);
   }
//qDebug() << " save pack ok sz = " << sz;
// app:
   SAppMng *amng;
   slist = appPacks();
   ds << slist;

   sz = appPackSize();
   //qDebug() << "appsize = " <<sz;
   for (i = 0; i < sz; i++)
   {
     //  qDebug() << i << slist[i];
      amng = mapApp[slist[i]];
     // qDebug() << i << slist[i] << amng;
      if (amng != NULL)  
          amng->encode(ds); 
   }
// close
   file.flush();
   file.close();
qDebug() << " save app ok";
   return saveDBMsg();
}

int SLicData::saveDBMsg()
{
   int sz;
   QString fileDB, filePtr;
   QStringList slist;

   fileDB = getDBFile();
   filePtr = getDBFileIndex();

   QFile ptr(filePtr);

   QString fpath, fcdate, fmds5, finode, fsz;
   QFileInfo fi(fileDB);
//qDebug() << " save DBMsg";
   fpath = fi.absoluteFilePath();
   fcdate = _dt.TID(fileDB);
   fmds5 = _dt.mds5(fileDB);
   finode = _dt.inodeFull(fileDB);
   fsz = _dt.size(fileDB);
#if 1
   qDebug() << fpath;
   qDebug() << fcdate;
   qDebug() << fmds5;
   qDebug() << finode;
   qDebug() << fsz;
#endif
   //open:
   if (!ptr.open(QIODevice::WriteOnly |QIODevice::Truncate)) return -2;
   QDataStream ds(&ptr);
   ds << fpath;
   ds << fcdate;
   ds << fmds5;
   ds << finode;
   ds << fsz;
   ptr.flush();
   // my inode:
   QFileInfo fii(filePtr);
   fpath = fii.absoluteFilePath();
   finode = _dt.inodeFull(filePtr);

   ds << fpath;
   ds << finode;
   fcdate = _dt.TID(filePtr);
   ds << fcdate;
   ptr.flush();
   sz = ptr.size();
   ds << sz + 4;  
   ptr.close();
// ptr Mds5 save:
   fmds5 = _dt.mds5(filePtr);
   QSettings st(ORG_NAME,SAPP_NAME);
   st.setValue(DB_PTR,fmds5);
   
   return sz +4;
}
#define RET(x) {cout << x <<endl; return -1;}
int SLicData::loadDBMsg()
{
   int sz;
   QString fileDB, filePtr;
   QStringList slist;

   fileDB = getDBFile();
   filePtr = getDBFileIndex();

   QFile ptr(filePtr);

   QString fpath, fcdate, fmds5, finode, fsz;
   QFileInfo fi(fileDB);
   // ptr Mds5 load:
   fmds5 = _dt.mds5(filePtr);
   QSettings st(ORG_NAME,SAPP_NAME);
   fsz = st.value(DB_PTR,fmds5).toString();
   if(fsz != fmds5) RET("DB_PTR fileContains error");

   fpath = fi.absoluteFilePath();
   fcdate = _dt.TID(fileDB);
   fmds5 = _dt.mds5(fileDB);
   finode = _dt.inodeFull(fileDB);
   fsz = _dt.size(fileDB);
#if 1
   qDebug() << fpath;
   qDebug() << fcdate;
   qDebug() << fmds5;
   qDebug() << finode;
   qDebug() << fsz;
#endif
   //open:
   int ilen;
   QString str;
   if (!ptr.open(QIODevice::ReadOnly)) return -2;
   QDataStream ds(&ptr);
   ds >> str;
   if(str != fpath) RET("DB filePath error");
   ds >> str;
   
   ilen = abs(str.toLong() - fcdate.toLong());
   qDebug() << " DB Date = " << str << fcdate << ilen;

   //if(str != fcdate) RET("DB fileDate error");
   if(ilen > 1) RET("DB fileDate error");
   ds >> str;
   if(str != fmds5) RET("DB fileContains error");
   ds >> str;
   if(str != finode) RET("DB fileInode error");
   ds >> str;
   if(str != fsz) RET("DB fileSize error");
 
   QFileInfo fii(filePtr);
   int i;
   fpath = fii.absoluteFilePath();
   finode = _dt.inodeFull(filePtr);
   fcdate = _dt.TID(filePtr);
   sz = ptr.size();

   ds >> str;
   if(str != fpath) RET("DB_Ptr filePath error");
   ds >> str;
   if(str != finode) RET("DB_Ptr fileInode error");
   ds >> str;
   ilen = abs(str.toLong() - fcdate.toLong());
   qDebug() << " ptr Date = " << str << fcdate << ilen;

   if(ilen > 1) RET("DB_ptr fileDate error");
   //if(str != fcdate) RET("DB_Ptr fileDate error");
   
   ds >> i;
   if(i != sz) RET("DB_Ptr fileSize error");

   ptr.close();
   return sz;
}

QString SLicData::getDBFile()
{
   return DB_FILE;
}
QString SLicData::getDBFileIndex()
{
   return DB_FILE_PTR;
}
int SLicData::loadDB()
{
   int sz, i;
   QString fileDB;
   QStringList slist;
   if (!isDBRegisted()) 
   {
       i = registerDB();
       return i;
   }
   i = loadDBMsg(); 
   if (i <=0)  return -1;

   fileDB = getDBFile();
   QFile file(fileDB);
   if (!file.open(QIODevice::ReadOnly)) return -1;
   QDataStream ds(&file);
// pack:
   SPackMng *pmng;

   ds >> slist;
   sz = slist.size();
   //qDebug() << "slist in load = " << slist;
   for (i = 0; i < sz; i++)
   {
      pmng = new SPackMng();
      pmng->decode(ds);
      //qDebug() << "inloop = " << i <<   pmng->size() << pmng->getText();
      mapPack[slist[i]] = pmng;
   }

// app:
   SAppMng *amng;
   ds >> slist;

   sz = slist.size();
   for (i = 0; i < sz; i++)
   {
      amng = new SAppMng();
      amng->decode(ds);
      mapApp[slist[i]] = amng;
   }
// close
   file.close();
   return 1;

}
int SLicData::clear()
{
   int sz, i;
   QStringList slist;

// pack:
   SPackMng *pmng;
   slist = mapPack.keys();

   sz = mapPack.size();
   for (i = 0; i < sz; i++)
   {
      pmng = mapPack[slist[i]];
      pmng->clear();
      delete pmng;
   }
   mapPack.clear();

// app:
   SAppMng *amng;
   slist = mapApp.keys();

   sz = mapApp.size();
   for (i = 0; i < sz; i++)
   {
      amng = mapApp[slist[i]];
      amng->setRemoveOption(true);
      amng->clear();
      delete amng;
   }
   mapApp.clear();

   return 1;

}
int SLicData::registerDB()
{
    QSettings st(ORG_NAME,SAPP_NAME);
    st.setValue(DB_REGISTER,"dbname");

}
bool SLicData::isDBRegisted()
{
    QSettings st(ORG_NAME,SAPP_NAME);
    QString  str;
    str = st.value(DB_REGISTER).toString();
    if (str == "dbname") return true;
        else
        return false;
}
//=======================heartBeat:
int SLicData::appHB(SAppInfo &app)
{
    int i, number;
    LFileDate fd;
    QString str;
    QString vender, package, version, packid, pid, ip, user;
 // check App info
    SAppInfo *info;
    SAppMng *mng;
    mng = NULL;

    info = &app;
    
    vender = info->get(APP_VENDER).toString();
    package = info->get(APP_PACKAGE).toString();
    version = info->get(APP_VERSION).toString();
    number = info->get(APP_NUMBER).toInt();
    ip = info->get(APP_IP).toString();
    pid = info->get(APP_PID).toString();
    user = info->get(APP_USER).toString();

    packid = encodePackageId(vender, package, version);
    mng = appMng(packid);
    QString appid;

    appid = encodeAppId(ip,pid);

    info = NULL;
    info = (SAppInfo* )mng->get(appid);

    if (info != NULL)  
        return info->appHB(); 
    else
        return -1;
 
}
int SLicData::ckeckHB()
{
    return 1;
}
//=============================util====================
QString SLicData::hex(QString s)
{
    string ss1,ss2;
    QString str;
    LEncrypt en;
    ss1 =  s.Q2CH;
    ss2 = en.bin2Hex(ss1); 
    str = ss2.c_str();
    return str;   
}
QString SLicData::unHex(QString s)
{
    string ss1,ss2;
    QString str;
    LEncrypt en;
    ss1 =  s.Q2CH;
    ss2 = en.hex2Bin(ss1); 
    str = ss2.c_str();
    return str;   
}
