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
SPackMng* SLicData::packMng(QString packid)
{
   return mapPack[packid];
}
QStringList SLicData::packNames()
{
   QStringList slist;
   slist = mapPack.keys();
   return slist;
}
int SLicData::removedSize()
{
   return packRemoved.size();
}
//------------ -----------------------app-----------------
int SLicData::appSize()
{
   //return mapApp.size();
    int i,sz,ret;
    sz = mapApp.size();
    QStringList slist;
    slist = mapApp.keys();
    ret = 0;
    for (i = 0;i < sz;i++) 
    {
        ret += mapApp[slist[i]]->size();

    }
    return ret;
}
SAppMng* SLicData::appMng(QString packid)
{
   return mapApp[packid];
}
QStringList SLicData::appPacks()
{
   QStringList slist;
   slist = mapApp.keys();
   return slist;
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
   QString packid,err;
   SPackMng *mng;
   SAppMng *appmng;
   packid = info.packid;
   //qDebug() << "packid=" << packid;

   i = -1;
   if (isPackidInPack(packid))
   {
      if (isPackidInApp(packid))// is packid in appMng
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
   else// no this package
   {
       err = "registerApp Error:no this package name = "+packid;
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
          info.err  = "releaseApp Error: no this package in appMap name = "+ packid;         
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
   // qDebug() << "info=" << info->getText();
   // qDebug() << "ainfo=";// << ainfo.getText();
   packid = encodePackageId(vender, package, version);

   info->packid = packid;
   info->number = number;
   info->appid = encodeAppId(ip, pid);
   info->user = user;
   info->nameid = info->appid;
   info->start = fd.sEP();
   //qDebug() <<  "i0 =registerApp( *info); =" << i;

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
   if (info !=NULL)  
   {
       info->ret = i;
       info->end = fd.sEP();
      
       elog(info); 
   }
   //qDebug() << "return2 = " << i;
   return i;// >=0 OK <0 err;
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
   if (rmInfo != NULL)// found it:
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
      delete rmInfo;// remove the appInfo if ok;
   }
   return i;// >=0 OK <0 err;
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
      str =  fun + " OK:" + info->packid + " " + info->get(PTYPE).toString() +" "+ info->get(PLIMIT).toString() +" "+info->get(PSTARTDATE).toString() +" "+info->get(PENDDATE).toString() +" "+info->get(UUID).toString();
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
