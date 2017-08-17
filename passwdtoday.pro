

TEMPLATE = app
INCLUDEPATH += . \
	../crypt 
DESTDIR =Debug
CONFIG += debug

LIBS += -lssl

# Input
HEADERS +=   LInfoMng.h LLicFile.h LPackageMng.h LVenderMng.h LLicEncrypt.h LFileDate.h\
	../crypt/testG.h \
	../crypt/LEncrypt.h \ 
	../crypt/LSummary.h 

SOURCES +=  passwdtoday.cpp\
	   LLicEncrypt.cpp \
	   LFileDate.cpp \
	   LInfoMng.cpp \
           LLicFile.cpp \
           LPackageMng.cpp \
           LVenderMng.cpp \
	../crypt/LEncrypt.cpp 

