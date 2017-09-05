#ifndef LIC_API_H
#define LIC_API_H


#include "LLicFile.h"
#include "LFileDate.h"
#include "LEncrypt.h"

#define MID_LEN 35
//#define PASSWD_SIGN "1234567890"

#define VENDER_DELIMIT "%%"

#define VENDER_SEED "0123456789" // 10 length of char;
/**
 * lic create: 
 *      venderPri:signed(Seed+serverPub);-->app verify
 *      ServerPub:encrypt venderKey;--->server decrypt,and check
 *      vendername,venderSeed as input
 * server:      
 *      serverPri:decrypt venderKey,get signedSeed, and check if key is valid:
 *      venderPub: send to app;
 * app: 
 *      venderSeed,vendername,packageName,version; request message
 *      venderPub:(char)->verify signedSeed;
 *  
 */
class LLicEncrypt
{
public:
    LLicEncrypt();
    ~LLicEncrypt();
 
    int openLicFile(QString file);
    LLicFile * getLicFile();


//createLic:   venderPri,venderSeed
#if 0 // donot use server pub now
   //venderKey:
    /** @brief   encrypt vender message with serverPubKey and
                 encodeHex , getvenderKey;
        @param infoVender  is from LLicFile
    */ 
    QString encryptVenderKey(QString ServerPub, QString venderPri, LInfo *infoVender,QString venderSeed);
    QString encryptVenderKey(QString ServerPub,LInfo *infoVender,QString venderSign);// for borrow

     /// sign venderSeed+serverPubKey with venderPriKeyfile and encodeHex : Vpri(Spub,VenderSeed)  
    QString signVenderSeed(QString venderPri,QString venderSeed,QString serverPub); 
     /** @brief  compose the vender messages to encodeMsg as the
      *          encrypt input
     */
    QString encodeVender(QString venderName,QString ServerID,QString uuid, QString borrow ,QString enSeed);//5     
#endif
   /** @brief    venderKey; =
                 digest(serverMsg) + signSeed
                 signSeed = venderSeed+serverID
        @param infoVender  is from LLicFile
    */ 
    QString encodeVenderKey(QString venderPri, LInfo *infoVender,QString venderSeed);
    QString encodeVenderKey(LInfo *infoVender,QString venderSign);// for borrow

     /// sign venderSeed+serverID with venderPriKeyfile and encodeHex : Vpri(Spub,VenderID)  
    QString signVenderSeed(QString venderPri,QString venderSeed,QString serverID); 
     /** @brief  compose the vender messages to encodeMsg as the
      *          digest input
     */
    QString encodeVender(QString venderName,QString ServerID,QString uuid, QString borrow ,QString venderSign);//5  
    QString encodeSign(QString sign,QString pass);    

    //packageKey: 
   /** @brief   digest venderKey+encodedPackageMsg  
        @param infoPackage is from LLicFile
    */ 
    QString digestPackageKey(QString venderKey,LInfo *infoPackage); 
    ///@brief  compose the vender+Package messages to encodeMsg as the encrypt input
    QString encodePackge(QString venderkey, QString pName,QString version,QString type,QString limit,QString borrow,QString start,QString end);// 8 
    ///@brief   get server public key ; 
    QString getPKeyText(QString file);

//server: serverPri,serverPub,venderPub;borrowInPub,borrwOutPub
    // vender:
    /// if venderKey is valid load when load file, getvenderSign to _venderSign;
    //bool isVenderKeyValid(QString ServerPri,LInfo * infoVender);
    bool isVenderKeyValid(LInfo * infoVender);
    QString decodeSign(QString sign,QString pass);  
    QString getVenderSign(){return _venderSign;};//  isVenderKeyValid() make it availibale
 #if 0
 
    ///   decrypt vender message with serverPriKey (hexDecode and decryptPri)
    QString decryptVenderKey(QString ServerPri,QString venderKey);
    /// decode vender Message 
    void decodeVender(QString venderKey,QString &venderName,QString &ServerID,QString &uuid, QString &borrow,QString &enSeed);//5   
 #endif 
    // package:  
    /// if PackageKey is valid , when load file
    bool isPackageKeyValid(QString venderKey,LInfo * infoPackage); 
    /// decode package Message
    //QString decodePackge(QString key ,QString &venderEncode, QString &pName,QString &version,QString &type,QString &limit,QString &borrow,QString &start,QString &end);//9 parameters
     
// app: venderSeed ,venderPubChar
    /// verify signed VenderSeed&serverPub
    //bool verifyVenderSeed(QString venderPubChar ,QString venderSign, QString vendSeed,QString serverPub); //4 verify venderSeed & serverPub    app use it
    bool verifyVenderSeed(QString venderPubChar ,QString venderSign, QString vendSeed,QString serverID); //4 verify venderSeed & serverPub    app use it
/// geMid
    QString getMid();
    QString getMidMark();


    QString _venderKey,_venderSign;
private:
    void init();
    LLicFile *_licFile;
    QString _fileIn;
    //QString _fileOut;

};


#endif
