#include <iostream>
#include "LLicFile.h"
#include "testG.h"
#include "LLicEncrypt.h"
#include "SLicMng.h"

#define TEST_CYCLE1
#include "SCycleThread.h"

using namespace std;

#define TEST_UNIT

//#define SERVER_PUB "server.pub"
//#define SERVER_PRI "server.pri"
#define VENDER_PUB "vender.pub"
#define VENDER_PRI "vender.pri"

#define LIC_FILE_SAMPLE "licfile.sample"
#define LIC_FILE "licfile.lic"
#define LIC_FILE1 "licfile1.lic"
#include "../crypt/testCrypt.cpp"
#include "createlic.cpp"
//#include "testCrypt.cpp"
#include "mid.cpp"
#include "uuid.cpp"
#include "createkey.cpp"
#include "log.cpp"



U_START(Qlist_remove_ptr)
    QList <int *> listi;
    int *buf;
    buf = NULL;
    buf = new int[100];
    memset(buf,0,100);
     qDebug() << "buf0 = " << buf;
    listi.append(buf);
    qDebug() << "buf1 append = " << listi.size();
    listi.removeAt(0);
     memset(buf,0,100);
    qDebug() << "buf1 remove list = " << buf << buf[40] <<listi.size() ;
    VALID_PTR(buf);
    delete []buf;
    VALID_PTR(buf);
    //memset(buf,0,100);
    qDebug() << "buf1 delete buf= " << buf  ;
    QVariant v;
    qDebug() << "v= " << v << v.isValid();

U_END
// base info==============================================
// LInfoMng& ,if LInfoMng:no match for ?operator=? in ?im2 = getMng(LInfoMng&)()?
//  void operator= (LInfoMng & mng);
LInfoMng & getMng(LInfoMng &mng) // ok &mng is the same address as caller;
{
    LInfoMng mng1;
   // qDebug() << "mng0=" << mng.size();
    mng1 = mng;
    qDebug() << "getMng" << mng.size() << &mng<<&mng1;
    //exit(0);
    return mng;
}

LInfoMng&  getMng1() //  return a new LInfoMng, not a local is ok
{
// not work;
    #if 0  // this will false ,return local ,return a empty  LLinoMng
    LInfoMng mng1;
    LInfo *info;
    info = new LInfo();
    info->set("111","111");
    info->set("222","111");
    mng1.add(info);   
    return mng1;
    #endif
// this works:
    LInfoMng *mng1;
    LInfo *info;
    info = new LInfo();
    info->set("111","111");
    info->set("222","111");
    mng1 = new LInfoMng();
    mng1->add(info);   
    return *mng1;
}
void getMng2(LInfoMng &mng) //  param must be:  LInfoMng &mng
{
    LInfoMng mng1;
   // qDebug() << "mng0=" << mng.size();
    mng1 = mng;
    qDebug() << "getMng2=" << mng.size() << &mng<<&mng1;
    //exit(0);
    //return mng;
}
LInfoMng & getMng3(LInfoMng mng) // mng is not the same address as caller;
{
    LInfoMng mng1;
    mng1 = mng;
    qDebug() << "getMng3" << mng.size() << &mng<<&mng1;

    qDebug() << "getMng3 info =" << mng.get(0) << mng1.get(0);
    //exit(0);
    return mng;// return mng is ok ,mng1 is not ok:why :we consider: the copy to mng is done before calling it; not use of = 
}

LInfo & getInfo(LInfo &mng) //  param must be:  LInfoMng &mng
{
    LInfo  mng1;
    mng1 = mng;
    qDebug() << "getInfo=" << mng.size() << &mng<<&mng1;
    return mng;
}
LInfo  &getInfo1(LInfo mng) //  return a empty
{
    LInfo  mng1;
    mng1 = mng;
    qDebug() << "getInfo1=" << mng.size() << &mng<<&mng1;
    return mng1;
}

U_START(basInfo)
    LInfo *bf,*bf1,bff;
    LInfoMng im;
    QString str1,str2,v1,v2;
    QString str3,str4,v3,v4;
    str1 = "vender1";v1 = "v1";
    str2 = "vender2"; v2 = "v2";
    str3 = "vender3";v1 = "v3";
    str4 = "vender4"; v2 = "v4";
    bf = new LInfo();
    bf1 = new LInfo();
    bf->set(str1,v1);
    bf->set(str2,v2);
    bf->set(str3,v3);
    bf->set(str4,v4);
    //bool b;
    bf1->set(str1+str1,v2);
    bf1->set(str2+str1,v1);

    EQ(bf->isEQ(str1,v1),true);
    EQ(bf->isEQ(str2,v2),true);
    NEQ(bf->isEQ(str1,v2),true);
    PR("infomng:==============================");
    im.add(bf);
    im.add(bf1);
   
    int idx;
    idx = im.find(str1,v1);
    EQ(idx,0);
    
    idx = im.find(str1,v1,str2,v2);
    REM("im.find(str1,v1,str2,v2)")
    EQ(idx,0);

    idx = im.find(str1,v1,str2,v2,str3,v3);
     REM("im.find(str1,v1,str2,v2,str3,v3")
    EQ(idx,0);
    idx = im.find(str3,v1);
     REM("im.find(str3,v1)")
    EQ(idx,-1);
    idx = im.find(str1,v2);
    REM("im.find(str1,v2)")
    NEQ(idx,1);
    idx = im.find(str3,v1);
    REM("im.find(str3,v1)")
    EQ(idx,-1);
    idx = im.find(str1,v2,str2,v2);
    REM("im.find(str1,v2,str2,v2)")
    EQ(idx,-1);
    float f1,f2;
    f1 = 1.0;
    f2 = 1.0;
    EQF(f1,f2);
    PR("IM==IM1=======================================================");
    LInfoMng im1 ;
 
  // qDebug() << &im <<&im1<< "----000000000000000000\n";
    im1 = im;

 
    string ss;
    ss = " im.size=" + STR(im.size());
    PR(ss);
    ss = " im1.size=" + STR(im1.size());
    PR(ss);
    EQ(im.size(),im1.size());
    //qDebug() << &im <<&im1<< "----111111111111111\n";
      //  im1.clear();
   // im.clear(); 
   
    //exit(0);
   #if 1
     bff.set(str1,v1);
     bff.set(str2,v2);
     im1.clear();
     im1.setRemoveOption(false);
     im1.add(&bff);// ------>Abort (core dumped) if not: im1.setRemoveOption(false);
     ss = "im1.add(&bff)------>Abort (core dumped) if not: im1.setRemoveOption(false);";
     PR(ss);

     PR("IM2==get from function=======================================================");
     LInfoMng im2 ;
     qDebug() << &im <<&im2<< "----00000000000\n";
     im2 = getMng(im);//nomal &mng
     qDebug() << &im <<&im2<< "----111111111111111\n";    
     EQ(im2.size(),im.size());
     LInfoMng im3 ;
     im3 = getMng1();// a new mng
     EQ(im3.size(),1);

     getMng2(im2);// mng just print

     LInfoMng im4 ;
qDebug() << "info bf = " << im.get(0)<< &im;
     im4 = getMng3(im);
      EQ(im4.size(),im.size());
qDebug() << "im4 = " << &im4 <<&im <<im4.get(0);  


     PR("bf=bb=======================================================");
     //LInfo *bf,*bf1,
     LInfo bb,bb1,bb2;
     bb = *bf;
     EQ(bb.size(),bf->size());
     bb1 = getInfo(*bf);
     EQ(bb1.size(),bf->size());
     bb2 = getInfo1(*bf);// return a local
     NEQ(bb2.size(),bf->size());

     PR("bf= bf1 + bf2======================================================");
     LInfo bf3,bf4,bf5;
     bf3 = *bf1 ;
     EQ(bf3.size(),2)
qDebug() <<"bf3 = *bf1 ;"<< bf3.size() << bf1->size();;
     bf3 = *bf;
     EQ(bf3.size(),4)
qDebug()  <<"bf3 = *bf;"<< bf3.size() << bf->size();
     bf3 += *bf1 ;
     EQ(bf3.size(),6)
qDebug()  <<"bf3 += *bf1 ;"<< bf3.size() << bf1->size();
     bf4.set("123","123");
     bf5 = bf4+bf3;
     EQ(bf5.size(),7)
     qDebug() << bf5.map();



     

#endif
U_END
// licfile==============================================
U_START(licfile)
    int len,i;
    LLicFile lf;
    LInfo *it;
    len = lf.readFile((char *)LIC_FILE_SAMPLE);
    PR(STR(len));
    GT(len,0);
    for (i = 0; i < len;i++) 
    {
        it = lf.package(i);
        if (it!= NULL) 
        {
            qDebug() << it->m_map;
        }

    }
    EQ(lf.venderMng().isKeysExist(),true);
    EQ(lf.packageMng().isKeysExist(),true);

    len = lf.writeFile((char *)LIC_FILE1);
    GT(len,0);

    len = lf.readFile((char *)LIC_FILE);
    EQ(len,0);

    EQ(lf.venderMng().isKeysExist(),false);
    EQ(lf.packageMng().isKeysExist(),false);

U_END

U_START(licapi)
    LLicEncrypt lic;
    int i;
    QString str;
    LInfo *infoV,*infoP;
    QString sPub,vPri,vSeed,venderKey,pKey;
    i = lic.openLicFile(LIC_FILE_SAMPLE);
    REM("i = lic.openLicFile(LIC_FILE)")
    GT(i,0);
    //sPub = SERVER_PUB;
    vPri = VENDER_PRI;

    infoV = lic.getLicFile()->vender();
    vSeed = VENDER_SEED;
//create lic:
    //str = lic.encryptVenderKey( sPub,  vPri, infoV,vSeed);
    str = lic.encodeVenderKey(  vPri, infoV,vSeed);
    qDebug() << "venderKey = " << str;
    infoV->set(VENDERKEY,str);
    REM("str = lic.encryptVenderKey( sPub,  vPri, infoV,vSeed);");
    GT(str.length(),0);

    venderKey = str;
    infoP = lic.getLicFile()->package(0);   
    str = lic.digestPackageKey(venderKey,infoP); 
    infoP->set(PKEY,str);
    qDebug() << "pkey = " << str;
    REM("str = lic.digestPackageKey(venderKey,infoP); ")
    GT(str,0);

// server:
    bool b;

   // b = lic.isVenderKeyValid(QString(SERVER_PRI),infoV);
    b = lic.isVenderKeyValid(infoV);
    //qDebug() << "venderSign0  len=" <<  lic.getVenderSign().length();
    REM(lic.isVenderKeyValid(infoV));
    EQ(b,true);

    b = lic.isPackageKeyValid(venderKey,infoP); 
    REM(lic.isPackageKeyValid(venderKey,infoP));
    EQ(b,true);


// app:
    QString venderPubChar,serverID;
    QString venderSign;

    venderPubChar = lic.getPKeyText(VENDER_PUB);
    serverID = infoV->get(SERVERID).toString();
    venderSign = lic.getVenderSign();//  isVenderKeyValid() make it availibale
    qDebug() << "venderSign len=" <<venderSign.length() << venderSign;

   
    b = lic.verifyVenderSeed(venderPubChar, venderSign,  vSeed,  serverID);
    REM("lic.verifyVenderSeed(venderPubChar, venderSign,  vSeed,  serverID);");
    EQ(b,true);
// getMid();
    str = lic.getMid();
    qDebug() << "MID= " << str <<  str.length() << MID_LEN;
    EQ(str.length(),MID_LEN);
 
U_END
U_START(fileapi)

    LFileDate f;
    QString str;
    QDateTime dt;
    str = "LLicEncrypt.cpp";
 
    qDebug() << " inode = " << f.inode(str);
    qDebug() << " TID = " << f.TID(str);
    qDebug() << " modify = " << f.modify(str);
    qDebug() << " size = " << f.size(str);
    //qDebug() << " TID0 = " <<dt.currentDateTime().toSecsSinceEpoch()   ; 
    qDebug() << " TID1 = " <<dt.currentDateTime().toMSecsSinceEpoch()   ; 
    qDebug() << " currentDT = " << f.curDT();
    qDebug() << " currentD = " << f.curD();
    qDebug() << " currentT = " << f.curT();
    qDebug() << " currentTMS = " << f.curTMS();
    qDebug() << " current sEP = " << f.sEP();
    qDebug() << " current msEP = " << f.msEP();
    EQ(f.TID(str),f.modify(str));
    EQ(dt.currentDateTime().toMSecsSinceEpoch() ,f.msEP());
    PR("mds5-------------------------------------------------");
    qDebug() << "mds5 = " << f.mds5("licfile.lic");
    qDebug() << "pwd = "  << f.cmd("pwd");
    qDebug() << "ls = "  << f.cmd("ls -ltr");
    GT(f.mds5("licfile.lic").size(),0)
    GT(f.cmd("pwd").size(),0)
    GT(f.cmd("ls -ltr").size(),0)
    qDebug() << "inodeFull= "  << f.inodeFull("fileDB.db");
    qDebug() << "inodeHalf= "  << f.inode("fileDB.db");
    qDebug() << "inodeDec= "  << f.inodeDec("fileDB.db");
    qDebug() << "mds5= "  << f.mds5("fileDB.db");


U_END
U_START(SLicMngLoadFile)

    SLicMng sm;
    int i;
    QString file,str;
    file = "./test/test_lic_task2.lic";
 // load file:

    i = sm.loadFile(file);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i ,0);
    EQ(sm.data->packSize(),2)
    QString key;
    key = sm.data->packNames()[0];
    qDebug()  << "key = " << key << sm.data->packNames();
    qDebug() << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)
    EQ(sm.data->packMng(key)->userSize(),0)
    EQ(sm.data->packMng(key)->nodeSize(),0)
    EQ(sm.data->packMng(key)->taskLimit(),10)
    EQ(sm.data->packMng(key)->taskUsed(),0)
// re load file:
        i = sm.loadFile(file);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    LT(i ,0);
// unload file
    i = sm.unloadFile(file,"fileu");
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    EQ(i ,2);
    EQ(sm.data->packSize(),0)
    EQ(sm.data->removedSize(),2)

    //   load file after remove:
    i = sm.loadFile(file);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i ,0);

U_END
U_START(SLicMngApp)
    SLicMng sm;
    SAppInfo info,*pf;
    int i;
    QString file,str,key;
    QString ip,pid,user,appid;
    ip = "192.168.157.129";
    pid = "1234";
    user = "cjh";
    appid = ip+"_"+pid;
    //qDebug()  << "key = " << key << sm.data->mapPack.keys();
    
    info.set(APP_IP,ip.Q2CH);
    info.set(APP_USER,user.Q2CH);
    info.set(APP_PID,pid.Q2CH);

    info.set(APP_VENDER,"geoeast");
    info.set(APP_PACKAGE,"pc1");
    info.set(APP_VERSION,"1.0");
    info.set(APP_NUMBER,"5");
    i = sm.loginApp(info);
    GT(i,0)
    QString packid1,packid2,appid1;
    packid1 = sm.data->packNames()[0];
    packid2 = sm.data->packNames()[1];
    appid1 =  ((SAppInfo *)(sm.data->appMng(packid1)->get(0)))->appid;

    key = packid1;
    qDebug() << "m.data->mapPack.keys()"<<sm.data->packNames() << key;
    qDebug() <<"sm.data->mapApp.keys()"<< sm.data->appPacks() << key;
    qDebug() << "sm.data->mapApp[key] "<< sm.data->appMng(packid1);//==0??

    EQ(sm.data->packSize(),2)
    EQ(sm.data->appSize(),1)
    EQ(sm.data->packMng(packid1)->nodeSize(),0)
    EQ(sm.data->packMng(packid1)->userSize(),0)
    EQ(sm.data->packMng(packid1)->taskSize(),1)
    //QString str;
    pf = (SAppInfo *)(sm.data->appMng(packid1)->get(0));
    str = ((SAppInfo *)(sm.data->appMng(packid1)->get(0)))->appid;
    qDebug() << appid << appid1;
 
    EQ(appid.length(),appid1.length())

U_END

U_START(licLoadFiles)
    SLicMng sm;
    int i;
    QString file,file1,file2,file3,str;
    file = "./test/test_lic_task2.lic";
    file1 = "./test/test_lic_node.lic";
    file2 = "./test/test_lic_user.lic";
 // load file:

    i = sm.loadFile(file);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i ,0);
    i = sm.loadFile(file1);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i ,0);
    i = sm.loadFile(file2);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i ,0);
 
    EQ(sm.data->packSize(),4)
    QString key;
    qDebug()  << "packges = " << sm.data->packNames();
    key = "geoeast_pc1_1.0";
    
    qDebug() << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)
    EQ(sm.data->packMng(key)->userSize(),1)
    EQ(sm.data->packMng(key)->nodeSize(),1)

    EQ(sm.data->packMng(key)->taskLimit(),10)
    EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->nodeLimit(),5)
    EQ(sm.data->packMng(key)->nodeUsed(),0)
    EQ(sm.data->packMng(key)->userLimit(),5)
    EQ(sm.data->packMng(key)->userUsed(),0)

        key = "geoeast_pc2_1.0";
    
    qDebug() << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)
    EQ(sm.data->packMng(key)->userSize(),0)
    EQ(sm.data->packMng(key)->nodeSize(),0)
    EQ(sm.data->packMng(key)->taskLimit(),10)
    EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->nodeLimit(),0)
    EQ(sm.data->packMng(key)->nodeUsed(),0)
    EQ(sm.data->packMng(key)->userLimit(),0)
    EQ(sm.data->packMng(key)->userUsed(),0)


U_END

U_START(licAppAdd)
    SLicMng sm;
    SAppInfo info;
    int i;
    QString file,str,key;
    QString ip,pid,user,appid;

    ip = "192.168.157.129";
    pid = "1234";
    user = "cjh";
    appid = ip+"_"+pid;
    //qDebug()  << "key = " << key << sm.data->mapPack.keys();
    
    info.set(APP_IP,ip.Q2CH);
    info.set(APP_USER,user.Q2CH);
    info.set(APP_PID,pid.Q2CH);

    info.set(APP_VENDER,"geoeast");
    info.set(APP_PACKAGE,"pc1");
    info.set(APP_VERSION,"1.0");
    info.set(APP_NUMBER,"1");
    
    int len,ir;
    // node:
    key = "geoeast_pc1_1.0";
    PR("test 5 app spend only 1 resource=================================");
    len = 5;
    for (i = 0; i <len;i++) 
    {
        pid = QString("%1").arg(10+i);
        info.set(APP_PID,pid.Q2CH);
        ir = sm.loginApp(info);
        qDebug() << "node.used =" << sm.data->packMng(key)->nodeUsed();
        EQ(sm.data->packMng(key)->nodeUsed(),1);
        GT(ir,0);
    }
    PR("test 19  app add 19 resource node4,user5task10===========================");

    len = 5+5+10;
    for (i = 0; i <len;i++) 
    {
        pid = QString("%1").arg(10+i);
        info.set(APP_PID,pid.Q2CH);
        pid = QString("%1").arg(10+i);
        info.set(APP_IP,pid.Q2CH);

        ir = sm.loginApp(info);
        qDebug() << "node.used =" << i <<sm.data->packMng(key)->nodeUsed();
        qDebug() << "user.used =" << i <<sm.data->packMng(key)->userUsed();
        qDebug() << "task.used =" << i <<sm.data->packMng(key)->taskUsed();
        if (i < 4) 
        {
            EQ(sm.data->packMng(key)->nodeUsed(), i + 2); 
        }
        else if (i >=4 && i<9)  
        {
            EQ(sm.data->packMng(key)->userUsed(), i -4 +1); 
        }
        else if (i>=9 &&i<19) 
        {
            EQ(sm.data->packMng(key)->taskUsed(), i -9 +1); 
        }
        else if (i == 19)  
        {
            //REM("i = 19")
            LT(ir,0);
            //qDebug()<< "1i =="<< i;     
        }
        //qDebug() <<"i = " << i;
        if (i < 19) 
        {
            //qDebug() <<"i = " << i;
           GT(ir, 0); 
        }

         qDebug() <<"i = " << i;
    }

 
    
U_END
U_START(licAppRemove)
    SLicMng sm;
    SAppInfo info;
    int i ;
    QString file,str,key;
    QString ip,pid,user,appid;
 

    ip = "192.168.157.129";
    pid = "1234";
    user = "cjh";
    appid = ip+"_"+pid;
    //qDebug()  << "key = " << key << sm.data->mapPack.keys();
    
    info.set(APP_IP,ip.Q2CH);
    info.set(APP_USER,user.Q2CH);
    info.set(APP_PID,pid.Q2CH);

    info.set(APP_VENDER,"geoeast");
    info.set(APP_PACKAGE,"pc1");
    info.set(APP_VERSION,"1.0");
    info.set(APP_NUMBER,"1");
    
    int len,ir;
    // node:
    key = "geoeast_pc1_1.0";
  
    #if 1
    PR("test releast 20 app of 4node,5user,10tasts,1err=========================================================");
    // now app:  5+4:node:129_10+i ; 10+i_10+i
    //           5  : user:14+i_14+i
    //           10 : task:19+i_19+i

    len = 5+5+10;
    // len =5;
    for (i = 0; i <len;i++) 
    {
        pid = QString("%1").arg(10+i);
        info.set(APP_PID,pid.Q2CH);
        pid = QString("%1").arg(10+i);
        info.set(APP_IP,pid.Q2CH);

        ir = sm.logoutApp(info);
        qDebug() << "node.used =" << i <<sm.data->packMng(key)->nodeUsed();
        qDebug() << "user.used =" << i <<sm.data->packMng(key)->userUsed();
        qDebug() << "task.used =" << i <<sm.data->packMng(key)->taskUsed();
        if (i < 4) 
        {
            EQ(sm.data->packMng(key)->nodeUsed(), 4-i); 
        }
        else if (i >=4 && i<9)  
        {
            EQ(sm.data->packMng(key)->userUsed(), 9 -1 -i); //????????????????????
        }
        else if (i>=9 && i<19) 
        {
            EQ(sm.data->packMng(key)->taskUsed(), 19-1 -i); 
        }
        else
        {
            LT(ir,0);  
        }
        if (i <19) 
        {
           GT(ir, 0); 
        }
       
    }
    qDebug() << "sm.data->appSize"<< sm.data->appSize();
     EQ(sm.data->appSize(),5);
      // release 5 node:
    PR("test 5 app release only 1 node resource=================================");
    ip = "192.168.157.129";
    pid = "1234";
    user = "cjh";
    appid = ip+"_"+pid;
    //qDebug()  << "key = " << key << sm.data->mapPack.keys();
    info.set(APP_IP,ip.Q2CH);
    info.set(APP_USER,user.Q2CH);
    key = "geoeast_pc1_1.0";
    
    len = 5;
    for (i = 0; i <len;i++) 
    {
        pid = QString("%1").arg(10+i);
        info.set(APP_PID,pid.Q2CH);
        ir = sm.logoutApp(info);
        qDebug() << "node.used =" << sm.data->packMng(key)->nodeUsed();
        if (i == len -1)  
        {
            EQ(sm.data->packMng(key)->nodeUsed(), 0);     
        }
        else
        {
            EQ(sm.data->packMng(key)->nodeUsed(), 1);   
        }
        GT(ir,0);
    }
    #endif
U_END
U_START(CkeckAppAdd)
    SLicMng sm;
    QString key;
    key = "geoeast_pc1_1.0";
    qDebug() << "node.used ="  <<sm.data->packMng(key)->nodeUsed();
    qDebug() << "user.used ="  <<sm.data->packMng(key)->userUsed();
    qDebug() << "task.used ="  <<sm.data->packMng(key)->taskUsed();
    qDebug() << "appPacks = " << sm.data->appPacks();
    qDebug() << "appSize = " << sm.data->appSize();
    EQ(sm.data->packMng(key)->nodeUsed(), 5); 
    EQ(sm.data->packMng(key)->userUsed(), 5);  
    EQ(sm.data->packMng(key)->taskUsed(), 10)
 
U_END
U_START(dataLOG)
    SLicMng sm;
U_END

U_START(licAppUserAddRemove)
    SLicMng sm;
    SAppInfo info;
    int i;
    QString file,str,key;
    QString ip,pid,user,appid;

    ip = "192.168.157.129";
    pid = "1234";
    user = "cjh";
    appid = ip+"_"+pid;
    //qDebug()  << "key = " << key << sm.data->mapPack.keys();
    
    info.set(APP_IP,ip.Q2CH);
    info.set(APP_USER,user.Q2CH);
    info.set(APP_PID,pid.Q2CH);

    info.set(APP_VENDER,"geoeast");
    info.set(APP_PACKAGE,"pc1");
    info.set(APP_VERSION,"1.0");
    info.set(APP_NUMBER,"1");
    
    int len,ir;
    // node:
    key = "geoeast_pc1_1.0";
    PR("test 10 app  spend 5 node and 1 user resource=================================");
    len = 5;
    for (i = 0; i <len;i++) 
    {
        // differnt ip,& pid; spnd 5 node 
        pid = QString("%1").arg(10+i);
        info.set(APP_PID,pid.Q2CH);
        pid = QString("%1").arg(10+i);
        info.set(APP_IP,pid.Q2CH);
        //qDebug() << i <<"1111111";
        ir = sm.loginApp(info);
         qDebug() <<i<<  ir;
        //qDebug() << "node.used =" << sm.data->packMng(key)->nodeUsed();
        EQ(sm.data->packMng(key)->nodeUsed(),i+1);
        GT(ir,0);
    }

    len = 5;
    for (i = 0; i <len;i++) 
    {
        // pid = 100+i,& ip = 100; spnd 5 user
        pid = QString("%1").arg(100+i);
        info.set(APP_PID,pid.Q2CH);
        pid = QString("%1").arg(100);
        info.set(APP_IP,pid.Q2CH);
        ir = sm.loginApp(info);
        qDebug() << "node.used =" << sm.data->packMng(key)->nodeUsed();
        EQ(sm.data->packMng(key)->userUsed(),1);
        GT(ir,0);
    }
    PR("test release 10 app  5 node and 1 user resource=================================");
    len = 5;
    for (i = 0; i <len;i++) 
    {
        // differnt ip,& pid; spnd 5 node 
        pid = QString("%1").arg(10+i);
        info.set(APP_PID,pid.Q2CH);
        pid = QString("%1").arg(10+i);
        info.set(APP_IP,pid.Q2CH);
        ir = sm.logoutApp(info);
        qDebug() << "node.used =" << sm.data->packMng(key)->nodeUsed();
        EQ(sm.data->packMng(key)->nodeUsed(),len -i - 1);
        GT(ir,0);
    }

    len = 5;
    for (i = 0; i <len;i++) 
    {
        // pid = 100+i,& ip = 100; spnd 5 user
        pid = QString("%1").arg(100+i);
        info.set(APP_PID,pid.Q2CH);
        pid = QString("%1").arg(100);
        info.set(APP_IP,pid.Q2CH);
        ir = sm.logoutApp(info);
        qDebug() << "node.used =" << sm.data->packMng(key)->nodeUsed();
        if (i == len -1) 
        {
            EQ(sm.data->packMng(key)->userUsed(), 0);
        }
        else
        {
            EQ(sm.data->packMng(key)->userUsed(), 1);
        }
         
        GT(ir,0);
    }
U_END
U_START(LoadFiles)
    SLicMng sm;
    int i;
    QString file,file1,file2,file3,str;
    file = "./test/test_lic_task2.lic";
    file1 = "./test/test_lic_node.lic";
    file2 = "./test/test_lic_user.lic";
 // load file:

    i = sm.loadFile(file);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i ,0);
    i = sm.loadFile(file1);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i ,0);
    i = sm.loadFile(file2);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i ,0);
U_END

U_START(checkLoadFiles)
    SLicMng sm;
    
    EQ(sm.data->packSize(),4)
    QString key;
    qDebug()  << "packges = " << sm.data->packNames();
    key = "geoeast_pc1_1.0";
    qDebug()  << "key = " <<key;
    
    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)
    EQ(sm.data->packMng(key)->userSize(),1)
    EQ(sm.data->packMng(key)->nodeSize(),1)

    EQ(sm.data->packMng(key)->taskLimit(),10)
    //EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->nodeLimit(),5)
    //EQ(sm.data->packMng(key)->nodeUsed(),0)
    EQ(sm.data->packMng(key)->userLimit(),5)
    //EQ(sm.data->packMng(key)->userUsed(),0)

        key = "geoeast_pc2_1.0";
    qDebug()  << "key = " <<key;
    
    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)
    EQ(sm.data->packMng(key)->userSize(),0)
    EQ(sm.data->packMng(key)->nodeSize(),0)
    EQ(sm.data->packMng(key)->taskLimit(),10)
    EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->nodeLimit(),0)
    EQ(sm.data->packMng(key)->nodeUsed(),0)
    EQ(sm.data->packMng(key)->userLimit(),0)
    EQ(sm.data->packMng(key)->userUsed(),0)

U_END
U_START(saveDB)

    SLicMng sm;
    LFileDate fd;
    int i;
    
    i = sm.data->saveDB();
    GT(i,0);
    qDebug() << "db date = " << fd.TID("fileDB.db") << fd.modify("fileDB.db");

    sm.data->clear();
    EQ(sm.data->packSize(),0)
    EQ(sm.data->appPacks().size(),0)
    EQ(sm.data->appSize(),0)


U_END
U_START(loadDB)
    int i;

    SLicMng sm;
    i = sm.data->loadDB();
    GT(i,0);

U_END
U_START(dataClear)
    SLicMng sm;
    sm.data->clear();
U_END
U_START(borrowOut)
    SLicMng sm;
    LFileDate fd;
    LLicFile lic;
    LLicEncrypt lice;
    LInfo * infoV,*infoP;
    int i;
    QString file,file1,file2,str,str1;
    file = "./test/test_borrowIn.lic";
    file1 = "./test/test_borrowOut.lic";
    file2 = "./test/test_borrowOut1.lic";
     
 // load file:
    QString key;
    key = "geoeast_pc1_1.0";

    i = sm.borrow(file,file1);
    GT(i,0);
    str= lice.getMid();
    lic.readFile(file1);
    infoV = lic.vender();
    infoP = lic.package(0);
    str1 = infoP->get(PBORROW).toString();
    //str1 = sm.data->unHex(str1);

    EQ(str,str1);
    qDebug() << "len" << str.length() << str1.length();
    qDebug() << "||" << str<< "||";
    qDebug() << "||" << str1<< "||";

 
    qDebug()  << "key = " <<key;
   // PR("=======after borrow out====================================================");
    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1+1)
    EQ(sm.data->packMng(key)->userSize(),1)
    EQ(sm.data->packMng(key)->nodeSize(),1)

    EQ(sm.data->packMng(key)->taskLimit(),10-2)
    //EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->nodeLimit(),5)
    //EQ(sm.data->packMng(key)->nodeUsed(),0)
    EQ(sm.data->packMng(key)->userLimit(),5)
    //EQ(sm.data->packMng(key)->userUsed(),0)
    i=sm.loadFile(file1);
    LT(i,0);
    #if 0
    i=sm.loadFile(file2);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i,0);
    #if 1
    PR("=======after borrow out and load borrow In====================================================");
    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1+1+1)
    EQ(sm.data->packMng(key)->userSize(),1)
    EQ(sm.data->packMng(key)->nodeSize(),1)

    EQ(sm.data->packMng(key)->taskLimit(),10-2+2)
    //EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->nodeLimit(),5)
    //EQ(sm.data->packMng(key)->nodeUsed(),0)
    EQ(sm.data->packMng(key)->userLimit(),5)
    #endif 
     #endif
    QStringList slist;
    slist = sm.reportPackage();
    for (i = 0; i < slist.size(); i++) 
    {
        qDebug() << i << slist[i];
    }
  
U_END
U_START(borrowIn)
    SLicMng sm;
    LFileDate fd;
    LLicFile lic;
    LLicEncrypt lice;
    LInfo * infoV,*infoP;
    int i;
    QString file,file1,file2,str,str1;
    file = "./test/test_borrowIn.lic";
    file1 = "./test/test_borrowOut.lic";
     
 // load file:

    QString key;
    key = "geoeast_pc1_1.0";
   
    i=sm.loadFile(file1);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i,0);
    #if 1
    //PR("=======after borrow out and load borrow In====================================================");
    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)  
    EQ(sm.data->packMng(key)->taskLimit(),2)
    #endif
    QStringList slist;
    slist = sm.reportPackage();
    for (i = 0; i < slist.size(); i++) 
    {
        qDebug() << i << slist[i];
    }
U_END
U_START(reportPackApp)
    SLicMng sm;
    QStringList slist;
    int i;
    slist = sm.reportPackage();
    GT(slist.size(),0);

    for (i = 0; i <slist.size();i ++) 
    {
        qDebug() << QString("%1").arg(i,10) << slist[i];
    }
    slist = sm.reportApp(); 
    GT(slist.size(),0);
    for (i = 0; i <slist.size();i ++) 
    {
        qDebug() << QString("%1").arg(i,10) << slist[i];
    }
    
U_END
U_START(fileDate);
    LFileDate fd;
    QString str,str1,filen,dt1,dt2;
    int len;
    len = 300;
    filen = "/tmp/test.db";
    str1.resize(len);
    qDebug() <<  "-------------------";
    fd.writeText(filen,str1);
    int i,ll,i1,l1;
    dt1 = fd.TID(filen); 
    qDebug() << " tm0 = " << dt1; 
    ll = 2000;
    i1 = -1; 
    l1 = -1;
    qDebug() << "loop 0 =" <<  fd.curDT();
    for (i = 0; i <ll; i++)  
    {
        i1 = fd.TID(filen).right(6).toInt();
        if (i1 != l1)  
            qDebug() << " tm i = " << i << i1 << fd.TID(filen);
        l1 = i1; 
        usleep(10);
    }
    qDebug() << "loop 1 =" <<fd.curDT();
    sleep(1);
    dt2 = fd.TID(filen); 
    qDebug() << " tm11 = " << dt2;
    EQ(dt1,dt2);
    len = fd.testFileCTime();
    qDebug() << "cdateMis = " << len; 
    EQ(len, 0); 
U_END
U_START(removeFile)
    SLicMng sm;
    int i;
    QString key,file;
    file = "test/test_lic_task2.lic";

    qDebug()  << "packges = " << sm.data->packNames();
    key = "geoeast_pc1_1.0";
    qDebug()  << "key = " <<key;
    i = sm.removeFile(file);
    GT(i,0);
    
    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),0)
    EQ(sm.data->packMng(key)->userSize(),1)
    EQ(sm.data->packMng(key)->nodeSize(),1)

    EQ(sm.data->packMng(key)->taskLimit(),0)
    //EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->nodeLimit(),5)
    //EQ(sm.data->packMng(key)->nodeUsed(),0)
    EQ(sm.data->packMng(key)->userLimit(),5)
    //EQ(sm.data->packMng(key)->userUsed(),0)

        key = "geoeast_pc2_1.0";
    qDebug()  << "key = " <<key;
    qDebug()  << "packges = " << sm.data->packNames();
    qDebug()  << "removed packges = " << sm.data->removedSize() << sm.data->removedPackages();//<<sm.data->packRemoved;
    #if 0
    // qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() ;
    //qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),0)
    EQ(sm.data->packMng(key)->userSize(),0)
    EQ(sm.data->packMng(key)->nodeSize(),0)
    EQ(sm.data->packMng(key)->taskLimit(),0)
    EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->nodeLimit(),0)
    EQ(sm.data->packMng(key)->nodeUsed(),0)
    EQ(sm.data->packMng(key)->userLimit(),0)
    EQ(sm.data->packMng(key)->userUsed(),0)
    #endif

U_END
U_START(unloadFile)
    SLicMng sm;
    int i;
    QString key,file;
    file = "test/test_lic_task2.lic";

    qDebug()  << "packges = " << sm.data->packNames();
    key = "geoeast_pc1_1.0";
    qDebug()  << "key = " <<key;
    i = sm.unloadFile(file,"fileu1");
    GT(i,0);
    
    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)
    EQ(sm.data->packMng(key)->userSize(),1)
    EQ(sm.data->packMng(key)->nodeSize(),1)

    EQ(sm.data->packMng(key)->taskLimit(),0)
    //EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->nodeLimit(),5)
    //EQ(sm.data->packMng(key)->nodeUsed(),0)
    EQ(sm.data->packMng(key)->userLimit(),5)
    //EQ(sm.data->packMng(key)->userUsed(),0)

    key = "geoeast_pc2_1.0";
    qDebug()  << "key = " <<key;
    qDebug()  << "packges = " << sm.data->packNames();
    qDebug()  << "removed packges = " << sm.data->removedSize() << sm.data->removedPackages();//<<sm.data->packRemoved;
    #if 1
    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)
    EQ(sm.data->packMng(key)->userSize(),0)
    EQ(sm.data->packMng(key)->nodeSize(),0)
    EQ(sm.data->packMng(key)->taskLimit(),0)
    EQ(sm.data->packMng(key)->taskUsed(),0)
    EQ(sm.data->packMng(key)->nodeLimit(),0)
    EQ(sm.data->packMng(key)->nodeUsed(),0)
    EQ(sm.data->packMng(key)->userLimit(),0)
    EQ(sm.data->packMng(key)->userUsed(),0)
    #endif

U_END
U_START(proof)
    SLicMng sm;
    int i;
    QString key,file,file1;
    file = "test/test_borrowOut.lic";
    file1 = "unload_uuid";

    qDebug()  << "packges = " << sm.data->packNames();
    key = "geoeast_pc1_1.0";
    qDebug()  << "key = " <<key;
    i = sm.unloadFile(file,file1);
    GT(i,0);

    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)
    EQ(sm.data->packMng(key)->userSize(),0)
    EQ(sm.data->packMng(key)->nodeSize(),0)
    EQ(sm.data->packMng(key)->taskLimit(),0)

    

    //int createProof(QString filen,SPackInfo * inf);

U_END
U_START(verifyProof)
    SLicMng sm;
    int i;
    QString key,file,file1;
    //QStringList slist;
    file = "test/test_borrowOut.lic";
    file1 = "unload_uuid";
    key = "geoeast_pc1_1.0";

    

    i = sm.borrowReturn(file1);
    qDebug() << "borrow return = " << i;
    GT(i,0);
 
    QStringList slist;
    slist = sm.reportPackage();
    EQ(slist.size(),10);
    EQ(sm.data->packMng(key)->taskSize(),1);
    EQ(sm.data->packMng(key)->taskLimit(),10);// remove the borrowOut;

    qDebug() << "after borrow return size =" << slist.size();
    for (i = 0; i < slist.size(); i++) 
    {
        qDebug() << i << slist[i];
    }
    //int createProof(QString filen,SPackInfo * inf);

U_END
U_START(saveDBPack)
    SLicMng sm;
    int i;
    QString key,file,file1;
  
    i = sm.data->saveDBPackage();
    sm.reportPackage();
    GT(i,0);
U_END
U_START(loadDBPack)
    SLicMng sm;
    int i;
    QString key,file,file1;
    i = sm.data->loadDBPackage();
    GT(i,0);
    key = "geoeast_pc1_1.0";
    
    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)
    EQ(sm.data->packMng(key)->userSize(),1)
    EQ(sm.data->packMng(key)->nodeSize(),1)

    EQ(sm.data->packMng(key)->taskLimit(),10)
    EQ(sm.data->packMng(key)->nodeLimit(),5)
    EQ(sm.data->packMng(key)->userLimit(),5)
    sm.reportPackage();

U_END
U_START(packDB2DB)
    SLicMng sm;
    int i;
    QString key,file,file1;
    i = sm.data->clear();
    GT(i,0);
    i = sm.packDB2DB();
    GT(i,0);
    i = sm.data->clear();
    GT(i,0);
    i = sm.data->loadDB();
    GT(i,0);
    key = "geoeast_pc1_1.0";
    
    qDebug() << "size of task,user,node = " << sm.data->packMng(key)->taskSize() << sm.data->packMng(key)->userSize()  <<sm.data->packMng(key)->nodeSize() ;
    EQ(sm.data->packMng(key)->taskSize(),1)
    EQ(sm.data->packMng(key)->userSize(),1)
    EQ(sm.data->packMng(key)->nodeSize(),1)

    EQ(sm.data->packMng(key)->taskLimit(),10)
    EQ(sm.data->packMng(key)->nodeLimit(),5)
    EQ(sm.data->packMng(key)->userLimit(),5)
    sm.reportPackage();

U_END
U_START(testCycle)


#ifdef TEST_CYCLE

qDebug() << "testCycle";
    SCycleThread cyc;
    cyc.start();
    //cyc.wait(10*1000);
    sleep(10);
    qDebug() << "wake up --------------";
    cyc.setDown();
    qDebug() << "set down  --------------";
    cyc.wait();
     qDebug() << "wait go  --------------";

    qDebug() << "size = " << cyc.rHB.ilist.size() << cyc.rDbSave.ilist.size() << cyc.rDayCheck.ilist.size() << cyc.rAppExp.ilist.size();

    EQ(cyc.rHB.ilist.size(),5-1)
    EQ(cyc.rDbSave.ilist.size(),2)
    EQ(cyc.rDayCheck.ilist.size(),1)
    EQ(cyc.rAppExp.ilist.size(),1)

    EQ(cyc.rHB.nList[1],24) // 2S
    EQ(cyc.rDayCheck.nList[0],0)
    EQ(cyc.rAppExp.nList[0],0)
    qDebug() << cyc.rHB.nList;
    qDebug() << cyc.rDbSave.nList;
    qDebug() << cyc.rDayCheck.nList;
    qDebug() << cyc.rAppExp.nList;
    #endif
   
U_END
U_START(DateTime)
    LFileDate dt;
    QString str;
    string ss,ss1;
 
    long l;
    l = dt.sEP();
    ss =  dt.curDT().Q2CH;
    ss1 = dt.EP2DT(l).Q2CH;
    qDebug() << "currentDT = " << dt.curDT() << dt.EP2DT(l);
    EQ(ss,ss1)
    //QString EP2DT(long s);
    //long DT2EP(QString dt);
    str = dt.EP2DT(0);

    ss = str.Q2CH;
    l = dt.DT2EP(str);
    qDebug() << "EP =" << str << l;
    EQ(l,0);

    QDateTime cd,d,dd;
    cd = QDateTime::currentDateTime();
    d.setTime_t(0);

    dd = d.toUTC();
    qDebug() << "t0=" << d << d.toUTC() << dd;

 
    
U_END
M_START
#if 0// whole test of 
#if 1
// base test of licfiles,load file log;
    testCrypt();// in .cpp
    U_TEST(Qlist_remove_ptr)
    U_TEST(basInfo)
    U_TEST(licfile)
    U_TEST(licapi)
    U_TEST(fileapi)
    U_TEST(testCreateLic)
    U_TEST(testMid)
    U_TEST(testUUID)
    U_TEST(testCREATEKEY)
    U_TEST(SLicMngLoadFile)
    U_TEST(SLicMngApp)
    U_TEST(testLOG)
    //U_TEST(licLoadFiles)
// end of base test:
#endif
#if 1
// test loadfiles app login,app logout;
    U_TEST(dataClear)
    U_TEST(LoadFiles)
    U_TEST(checkLoadFiles)
    U_TEST(licAppAdd)
    U_TEST(CkeckAppAdd)
    U_TEST(reportPackApp)
    //U_TEST(dataLOG)
    U_TEST(licAppRemove)
    U_TEST(licAppUserAddRemove)
// end of test loadfiles app login,app logout;
#endif 
#if 1
// test save and load DB:--------------
     U_TEST(dataClear)
     U_TEST(LoadFiles)
     U_TEST(checkLoadFiles)
     U_TEST(licAppAdd)
     U_TEST(CkeckAppAdd)
     U_TEST(reportPackApp)
 
     U_TEST(saveDB)// save and clear  
     U_TEST(loadDB)
     U_TEST(checkLoadFiles)
     U_TEST(CkeckAppAdd)
     U_TEST(licAppRemove)
     U_TEST(licAppUserAddRemove)
// end of test save load DB---------------
#endif
#if 1 // remove unload licfile test
     U_TEST(dataClear)
     U_TEST(LoadFiles)
     U_TEST(checkLoadFiles)
     U_TEST(removeFile)

     U_TEST(dataClear)
     U_TEST(LoadFiles)
     U_TEST(checkLoadFiles)
     U_TEST(unloadFile)
#endif  
#if 1 // borrowOut ,borrowIn
     U_TEST(dataClear)
     U_TEST(LoadFiles)
     U_TEST(checkLoadFiles)
     U_TEST(borrowOut)
     U_TEST(dataClear)
     U_TEST(borrowIn)
#endif
#if 1 //  borrowIn,unload->proof,and borrowReturn with proof file = unload_uuid;
     U_TEST(dataClear)
     U_TEST(borrowIn)// load a borrow license file: test/test_borrowOut.lic: geoeast_pc1_1.0 task 2;
     U_TEST(proof)// unloadfile and get proof file :unload_uuid;
     U_TEST(dataClear)
     U_TEST(LoadFiles)//   geoeast_node_1.0 :size = 3:task10,node5,user5
     U_TEST(borrowOut)//   geoeast_node_1.0 :size = 4:task10,node5,user5,task -2
     U_TEST(verifyProof)// boorrow return:unload_uuid;geoeast_node_1.0 :size = 3:task10,node5,user5
#endif
#if 1
// test  PackDBsave and load packDB2DB utility--------------
     U_TEST(dataClear)
     U_TEST(LoadFiles)
     U_TEST(checkLoadFiles)
    
     U_TEST(saveDBPack) 
     U_TEST(dataClear)
     U_TEST(loadDBPack)
     U_TEST(packDB2DB)// clear packdb2db loadDB

// end of test save load DB---------------
#endif

#if 1
U_TEST(DateTime)
#endif

#if 1 // test cycle:
#ifdef TEST_CYCLE
 U_TEST(dataClear)
 U_TEST(LoadFiles)
 U_TEST(licAppAdd)
 U_TEST(CkeckAppAdd)
 U_TEST(reportPackApp)
 U_TEST(testCycle)
#endif

#endif
#undef TEST_CYCLE   

#endif // end of whole  test 

  // test cycle:
#ifdef TEST_CYCLE
 U_TEST(dataClear)
 U_TEST(LoadFiles)
 U_TEST(licAppAdd)
 U_TEST(CkeckAppAdd)
 U_TEST(reportPackApp)
 U_TEST(testCycle)
#endif

 
  


M_END
