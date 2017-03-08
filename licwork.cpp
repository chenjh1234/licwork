#include <iostream>
#include "LLicFile.h"
#include "testG.h"
#include "LLicEncrypt.h"
#include "SLicMng.h"

using namespace std;

#define TEST_UNIT
#include "createlic.cpp"
#include "testCrypt.cpp"
#include "mid.cpp"
#include "uuid.cpp"
#include "createkey.cpp"



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
    sPub = SERVER_PUB;
    vPri = VENDER_PRI;

    infoV = lic.getLicFile()->vender();
    vSeed = VENDER_SEED;
//create lic:
    str = lic.encryptVenderKey( sPub,  vPri, infoV,vSeed);
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

    b = lic.isVenderKeyValid(QString(SERVER_PRI),infoV);
    //qDebug() << "venderSign0  len=" <<  lic.getVenderSign().length();
    REM(lic.isVenderKeyValid(QString(SERVER_PRI),infoV));
    EQ(b,true);

    b = lic.isPackageKeyValid(venderKey,infoP); 
    REM(lic.isPackageKeyValid(venderKey,infoP));
    EQ(b,true);


// app:
    QString venderPubChar,serverPubChar;
    QString venderSign;

    venderPubChar = lic.getPKeyText(VENDER_PUB);
    serverPubChar = lic.getPKeyText(SERVER_PUB);
    venderSign = lic.getVenderSign();//  isVenderKeyValid() make it availibale
    //qDebug() << "venderSign len=" <<venderSign.length();

   
    b = lic.verifyVenderSeed(venderPubChar, venderSign,  vSeed,  serverPubChar);
    REM("lic.verifyVenderSeed(venderPubChar, venderSign,  vSeed,  serverPubChar);");
    EQ(b,true);
// getMid();
    str = lic.getMid();
    qDebug() << "MID= " << str;
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
U_END
U_START(SLicMngLoadFile)

    SLicMng sm;
    int i;
    QString file,str;
    file = "lic_bgp_02c0cc5b-4105-4acd-b12b-2cf93c5ab2fd_.lic";
    i = sm.loadFile(file,SERVER_PRI);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    GT(i ,0);

    i = sm.loadFile(file,SERVER_PRI);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    LT(i ,0);

    i = sm.unloadFile(file,SERVER_PRI);
    qDebug() << "ret = "<< i << sm.mapLoadFile[i];
    EQ(i ,2);
    EQ(sm.data->mapPack.size(),0)
    EQ(sm.data->packRemoved.size(),2)

U_END
M_START
#if 0
    //testCrypt();// in .cpp
    U_TEST(Qlist_remove_ptr)
    U_TEST(basInfo)
    U_TEST(licfile)
    U_TEST(licapi)
    U_TEST(fileapi)
    U_TEST(testCreateLic)
    U_TEST(testMid)
    U_TEST(testUUID)
    U_TEST(testCREATEKEY)
#endif 
U_TEST(SLicMngLoadFile)
 
     
      
M_END
