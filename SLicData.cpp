#include "SLicData.h"
SLicData ::SLicData()
{

}
SLicData::~SLicData()
{

}

int SLicData::addPackage(SPackInfo *info)
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
      //qDebug() << "addPackage yes= " << nameid;
      mng = mapPack[packid];
      if (mng != NULL)
      {
         i = mng->add(info);
      }
   }
   else
   {
      //qDebug() << "addPackage no= " << nameid;
      mng = new SPackMng();
      i = mng->add(info);
   }
   // add to map:
   mapPack[packid] = mng;
   // unlock:
   _lockPack.unlock();
   //qDebug() << "return = " << i;
   return i;

}
/// return i  index of info in mng <0 err
int SLicData::removePackage(QString packid, SPackInfo *info)
{
   int i;
   SPackMng *mng;
   QStringList slist;
   // lock:
   //_lockPack.lock();
   mng =  mapPack[packid];
   //qDebug() << "removeP mng =" << mng << packid<< mng->size();
   i = mng->rmInfo(info);
   //qDebug() << "removeP return ==" << i;
   if (i >= 0)
   {
      packRemoved.add(info); // move to packRemoved
      if (mng->size() == 0)
      {
         mapPack.remove(packid); // remove this mng from map if mng.size() ==0
         delete mng; // remove the mng pointer as well
      }
   }
   // unlock
   //_lockPack.unlock();
   return i; // index of info in mng <0 err
}
/// remove info from uuid: return ret removed info
int SLicData::removePackage(QString uuid)
{
   int i, sz, ret, ret1;
   QMap<QString, SPackInfo *> umap;
   SPackInfo *info;
   QStringList slist;
   // lock:
   _lockPack.lock();
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
            break;
         }
      }
   }
   // unlock:
   _lockPack.unlock();
   return ret;
}

/// if packid in exist in the mapPack
bool SLicData::isPackidInPack(QString packid)
{
   if (mapPack.contains(packid)) return true;
   else return false;
}
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
/// if venderSign is the same in the exist in the packageMng in map
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

QString SLicData::encodePackageName(QString vender, QString package, QString version)
{
   QString str, dl;
   dl = "_";
   str = vender + dl + package + dl +   version;
   return str;
}
//=app ====================================================================
int SLicData::registerApp(QString packid,QString ip,QString pid,QString user,int number)
{
    int i;
    SPackMng *mng;
    if(isPackidInPack(packid))
    {
        mng = mapPack[packid];
        i = mng->request(ip,pid,user,number);
    }
    return i;
}
int SLicData::addApp(SAppInfo& ainfo)
{
   int i,number;
   QString str;
   QString vender, package, version, packid, pid,ip,user;
// check App info
   SAppInfo *info;
   SAppMng *mng;

   info = new  SAppInfo();
   *info = ainfo;

   vender = info->get(APP_VENDER).toString();
   package = info->get(APP_PACKAGE).toString();
   vender = info->get(APP_VERSION).toString();
   number = info->get(APP_NUMBER).toInt();
   ip = info->get(APP_IP).toString();
   pid = info->get(APP_PID).toString();
   user = info->get(APP_USER).toString();

   packid = encodePackageName(vender, package, version);
   i = registerApp(packid, ip,pid,user,number);
   if (i > 0)
   {
// add to map:
      _lockApp.lock();
      //info->startMS =
      info->number = number;

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

      }
      // add to map:
      mapApp[packid] = mng;
      // unlock:
      _lockApp.unlock();
      //qDebug() << "return = " << i;
   }
   return i;
}
