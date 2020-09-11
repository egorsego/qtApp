//
// Created by krazh on 08.11.17.
//

#ifndef LOGDB_LOGDB_C_CPP_H
#define LOGDB_LOGDB_C_CPP_H



#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================================================
// �஢�� ᮮ�饭�� ����(⨯�)
typedef enum
{
    ERROR_L =    1,
    WARNING_L =  2,
    INFO_L =     3,
    DEBUG_L =    4
} LOG_LEVELS;



//=============================================================================================================
//  �������, � ���ன �⭮���� ����뮥 ᮮ�饭�� ����(�������, ��, ����, � � �)
typedef enum
{
    REG_COMMON =            0,

    REG_CONFIG =            1,
    REG_DATABASE =          2,
    REG_FN =                3,
    REG_GOODS =             4,
    REG_JSON =              5,
    REG_KABINET =           6,
    REG_LCD =               7,
    REG_MENU =              8,
    REG_NET =               9,
    REG_OFD =              10,
    REG_RECEIPT =          11,
    REG_REMOTE_CONTROL =   12,
    REG_TESTS =            13,
    REG_THREADS =          14,
    REG_USERS =            15,
    REG_WIFI =             16,
    REG_SCANNER =          17,
    REG_PRINTER =          18,
    REG_KEYPAD =           19,
    REG_TIME   =           20,
    REG_FD   =             21,
    REG_PULSE   =          22,

    OTHER =               100
}LOG_REGIONS;


//=============================================================================================================
// ����䥩� C

#include <stdio.h>
#include <stdarg.h>

// �뢮� � ��� ᮮ�饭��. �� 䠪�� ᮮ�饭�� ���������� � ��।� ��� ��᫥���饩 ����� � ��
void logINFO_c(LOG_REGIONS region, const char *const fmt, ...);
void logWARN_c(LOG_REGIONS region, const char *const fmt, ...);
void logERR_c (LOG_REGIONS region, const char *const fmt, ...);
void logDBG_c (LOG_REGIONS region, const char *const fmt, ...);

// ������/������ �஢��� ����
void            setLogLevel_c ( LOG_LEVELS           lvl );
LOG_LEVELS      getLogLevel_c                           ();

// ������ ��� �� ����
void         setDBName_c( const char          *name);

// ������/������ ���� ࠧ��� ����� ��।� ᮮ�饭�� �� ������ � ��
void         setMaxQuerySize_c(unsigned int   size );
unsigned int getMaxQuerySize_c                    ();

// ������/������ ��ਮ� ����� � ���� ��।� ᮮ�饭��
void         setWriteDBPeriod_c(unsigned int usPer );
unsigned int getWriteDBPeriod_c                   ();

// ������/������ ���� �᫮ ����ᥩ � ��
void         setMaxDBSize_c(unsigned int        sz );
unsigned int getMaxDBSize_c                       ();

// ������ ����஢�� �室���� ⥪��
void   setCode_CP866_c();
void   setCode_UTF8_c ();
//===================================================================================



#ifdef __cplusplus
}
#endif

//C++
// ����� �ᨭ�஭��� ����� � ��. ������ ���� ����饭 � �� ��⮪�. ��横��� �����. ���� ��।� � ��
void runLogDaemon();
// �⮯ ������
void stopLogDaemon();

#endif //LOGDB_LOGDB_C_CPP_H
