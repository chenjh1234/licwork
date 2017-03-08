#include "LLicEncrypt.h"
#define MID_HELP  "mid\n"\
"function:  identify of this machine \n"
#define GETMID_ERR "Error:  getMid error \n"
 
//========================================start main=======================

void exitMID(int id)
{
    char help[] = MID_HELP;
    cout << help;
    #ifndef TEST_UNIT
    exit(id);
    #else
    return;
    #endif

}
#ifndef TEST_UNIT
main(int argc, char **argv)
{
#else
U_START(testMid)
#endif
   LLicEncrypt lic;
   QString str;
   str = lic.getMid();
  
#ifdef TEST_UNIT
   REM("str = lic.getMid();");
   GT(str.length(), 0);
#endif

   if (str.length() <= 0)
   {
      cout << GETMID_ERR;
      exitMID(1);
   }
   cout << str.Q2CH << endl;
   #ifdef TEST_UNIT
        str = lic.getMidMark();
        qDebug() << " midMark = " << str;
        cout << str.Q2CH << endl;
        REM("str = lic.getMidMark();");
        GT(str.length(), 0);
   #else
   if (argc >=2) 
   {
       str = lic.getMidMark();
       cout <<str.Q2CH << endl;
   }
   #endif
#ifndef TEST_UNIT
}
#else
   U_END
#endif

