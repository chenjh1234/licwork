#include "LLicFile.h"
LLicFile::LLicFile()
{
    _vender.setRemoveOption(true);
    _package.setRemoveOption(true);
}
LLicFile::~LLicFile()
{
    clear();
}
/// get the packageInfo i
LInfo* LLicFile::package(int i)
{
   // no need to if
   return _package.get(i);
}
/// get venderInfo
LInfo* LLicFile::vender()
{
   int i = 0;
   return _vender.get(i);

}
/// get vender Info i(i ==0 infact)
LInfo* LLicFile::vender(int i)
{

   return _vender.get(i);

}
/// remove all info in _venderMng and _packageMng
void LLicFile::clear()
{
   _vender.clear();
   _package.clear();
}
int LLicFile::writeFile(QString str)
{
    return writeFile(str.Q2CH);
}
int LLicFile::readFile(QString str)
{
    return readFile(str.Q2CH);
}
/// read a license file ,get venderMng ,packagemng Information; clear before read;
int LLicFile::readFile(char *filen)
{
   QFile f;
   int ret;
   clear();

   f.setFileName(filen);
   if (f.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      QTextStream in(&f);
      ret = readStream(&in);
      f.close();
   }
   else
   {
      qDebug() << " openfile error file =" << filen;
      ret =  -1;
   }
   return ret;
}
int LLicFile::readStr(QString text)
{    
   int ret;
   clear();
 
   if (!text.isEmpty())
   {
      QTextStream in(&text);
      ret = readStream(&in);
   }
   else
   {
      qDebug() << " Input text is NULL =";
      ret =  -1;
   }
   return ret;

}
///write the licFile ,acocording to th venderMng and packagemng
int LLicFile::writeFile(char *filen)
{
   QFile f;
   int ret;

   f.setFileName(filen);
   if (f.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      QTextStream outs(&f);
      ret = writeStream(&outs);
      f.close();
   }
   else
   {
      qDebug() << " openWrite file error file =" << filen;
      ret =  -1;
   }
   return ret;
}
QString LLicFile::writeStr()
{
   QString text;
      QTextStream outs(&text);
      writeStream(&outs);
   return text;
}

/// ckeck if important key in the file is exists(according venderDef,packageDef)
bool LLicFile::isKeysExist()
{
   bool b, b1;
   b = _vender.isKeysExist();
   b1 = _package.isKeysExist();
   return b & b1;

}
/// read file internal used
int LLicFile::readStream(QTextStream *in)
{
   QFile f;
   LEncrypt en;
   QString str, str1, s1, s2,s;
   QStringList slist;
   int i, line,len;
   int part;
   LInfo *infoVender;
   LInfo *infoPackage;
   infoVender = NULL;
   infoPackage  = NULL;
   //QTextStream in;
   line = 0;
   len = 10; // if pborrow.length >len , we decode Hex;
   bool bMline;
   bMline = false;
   string ss1,ss2;
   do
   {
//read
      line++;
      str = in->readLine();
      #if 0
      if (bMline) // continueline
      {
          if (str.right(1) == "\"")  str = str.left(str.length()-1);
          {
              s = s+str;
              bMline = false;
              s2 = s;
              goto gogo;
          }
          else
          {
              s = s + str + "\n"; 
              continue;
          }      
      }
      #endif
      //qDebug() << "read line = " << str << str.length() ;
//encode
      i = str.indexOf(REM_CHAR);
      str1 = str.mid(0, i);  // left of #
                             //qDebug() << "left of # = " << str1 << str1.length() ;
      slist = str1.split(SPE_CHAR);
      if (slist.size() < 2)  // no "=" in the line
      {
         //qDebug() << " no = " << str;
         continue;
      }

      //qDebug()<< " left = " << slist[0] << slist[0].length() << slist[1] << slist[1].length() ;
      s1 = slist[0].trimmed().toLower();
      s2 = slist[1].trimmed();
#if 0
      if (s1 == PBORROW) 
      {
          if (s2.length() >len) // decode hex
          {
              ss1 = s2.Q2CH;
              ss2 = en.hex2Bin(ss1);
              s2 = ss2.c_str();
          }
      }
#endif
#if 0
      if (s2.left(1) == "\"") 
      {
           bMline = true;
           s = s2.mid(1) + "\n";
           continue;
      }
gogo:      
    #endif
      //qDebug()<< " s1,s2 = " << s1 << s1.length() << s2 << s2.length() ;
      // save
      //vender start:
      if (s1 == VENDERNAME)
      {
         part  = 1;
         infoVender = new LInfo();
      }
      //package start:
      if (s1 == PACKAGENAME)
      {
         if (part == 1)
         {
            _vender.add(infoVender);   //save vender
         }
         else if (part == 2)
         {
            //qDebug() << "  package size = " << infoPackage << infoPackage->size() << infoPackage->m_map;
            _package.add(infoPackage); // save last package
         }
         else
         {
            cout << NO_VENDERNAME << "line = " << line << endl;
            return -2; // no venderName message
         }

         part  = 2;
         infoPackage = new LInfo();

      }


// set info:
      if (part == 1)
      {
         //if (isVenderKeyValid(s1.Q2CH))
         if (_vender.isKeyValid(s1.Q2CH))
         {
            infoVender->set(s1, s2);
         }
         else
         {
            cout << INVALID_KEY << s1.Q2CH <<  " line =" << line << endl;
         }

      }
      if (part == 2)
      {
         //if (isPackageKeyValid(s1.Q2CH))
         if (_package.isKeyValid(s1.Q2CH))
         {
            infoPackage->set(s1, s2);
         }
         else
         {
            cout << INVALID_KEY << s1.Q2CH <<  " line =" << line << endl;
         }
      }
   }
   while (!str.isNull());

// last package:
   //qDebug() << " last package size = " <<  infoPackage << infoPackage->size() << infoPackage->m_map;
   if (part == 2)  _package.add(infoPackage);
   else return -2;
// ret:
    
   len = _package.size();
   if (!isKeysExist())
   {
      cout << "some keys is not exist!!\n";
      len = 0;
   }

   return len;
}
/// write lic file ,internal use
int LLicFile::writeStream(QTextStream *outs)
{
   QFile f;
   QString str, str1;
   //QTextStream in;
   int len;
   if (_vender.size() != 1 || _package.size() <= 0)  return -1;

   str = VENDER_REM;
   *outs << str;

   str  = _vender.getText();
   *outs << str;

   str1 = PACKAGE_REM;
   *outs << str1; 
   str1 = _package.getText();
   *outs << str1; 
   //qDebug() << "len = " << outs->pos();
   len = str.length() + str1.length();
   return len;
}
int LLicFile::size()
{
   return packageMng().size();
};
