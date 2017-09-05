#include "LLicEncrypt.h"
#include <QString>
#define UUID SHOWPROOF

#define UUID_HELP  "showproof proofFile\n"\
"function:  show the information of proof file \n"
#define UUID_ERR "Error:  showproof error \n"
 
//========================================start main=======================

void exitUUID(int id)
{
    char help[] = UUID_HELP;
    cout << help;
    #ifndef TEST_UNIT
    exit(id);
    #else
    return;
    #endif

}
#define PASSWD_PROOF "0123456"
#define DELIMIT_PROOF "%%"

QString showProof(QString proofs)
{
   int i, ii,ic;
   QString  proof, str, rstr;
   LEncrypt en;
   string inStr, outStr;

   QStringList sList, slist;

   sList = proofs.split("\n");

   //qDebug() << " size =" << sList.size();
   //str = QString("proof number = %1\n").arg(sList.size());
   rstr ="";
   ic = 0;
   //qDebug() << "11111\n";
   for (ii = 0; ii < sList.size(); ii++)
   {
       //add:
      str = sList[ii];
      //qDebug() << "line =" << str;
      if (str.split(":").size() < 2) continue;
      proof = str.split(":")[1]; 

      inStr = proof.Q2CH;
      outStr = en.decodePassHex(inStr, PASSWD_PROOF);
      str =  outStr.c_str();
      slist = str.split(DELIMIT_PROOF);
      if (slist.size() < 5)  continue;
      ic++;
      i = 0;
      rstr += " ===============================\n";
      rstr += "packID = " + slist[i] + "\n"; i++;
      rstr += "type   = " + slist[i] + "\n"; i++;
      rstr += "limit  = " + slist[i] + "\n"; i++;
      rstr += "start  = " + slist[i] + "\n"; i++;
      rstr += "end    = " + slist[i] + "\n"; i++;
      rstr += "sinID  = " + slist[i] + "\n"; i++;
      rstr += "soutID = " + slist[i] + "\n"; i++;
      rstr += "uuid   = " + slist[i] + "\n"; i++;
      rstr += "sign   = " + slist[i] + "\n"; i++;
   }
   str = QString("proof number = %1\n").arg(ic);
   rstr = str+rstr;
   return rstr;
}
#ifndef TEST_UNIT
main(int argc, char **argv)
{
#else
U_START(testUUID)
#endif

   LFileDate lf;
   QString str,file;
   if (argc>=2)  file = argv[1];
   else
       exitUUID(1);
   str = lf.getText(file);
   if (str.length() == 0)  
   {
       cout << "file error \n";
       exitUUID(1);
   }
   str =  showProof(str);
   
#ifdef TEST_UNIT
   REM("str = showProof(str)");
   GT(str.length(), 0);
#endif

   if (str.length() <= 0)
   {
      cout << UUID_ERR;
      exitUUID(1);
   }
   cout << str.Q2CH<< endl;
   
#ifndef TEST_UNIT
}
#else
   U_END
#endif
