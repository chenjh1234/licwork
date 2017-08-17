 

TEMPLATE = app
INCLUDEPATH += . \
	../crypt \
	../datatool
DESTDIR =Debug
CONFIG += debug

LIBS += -lssl

# Input
HEADERS +=  LInfoMng.h LLicFile.h LPackageMng.h LVenderMng.h LLicEncrypt.h LFileDate.h\
	../crypt/testG.h \
	../crypt/LEncrypt.h \ 
	../crypt/LSummary.h 

SOURCES +=  createlic.cpp\
           LInfoMng.cpp \
           LLicFile.cpp \
	   LLicEncrypt.cpp \
           LPackageMng.cpp \
           LVenderMng.cpp \
	   LFileDate.cpp \
	../crypt/LEncrypt.cpp 

