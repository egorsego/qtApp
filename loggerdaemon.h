#ifndef LOGGERDAEMON_H
#define LOGGERDAEMON_H

#include <QObject>
#include <QDebug>
#include <QStandardPaths>

#include "/libs/armeabi-v7a/qFisGoLogDb/logdb_c_cpp.h"


#define logINFO( fmt, ... ) logINFO_c( REG_COMMON, fmt, ##__VA_ARGS__ )
#define logWARN( fmt, ... ) logWARN_c( REG_COMMON, fmt, ##__VA_ARGS__ )
#define logERR(  fmt, ... ) logERR_c(  REG_COMMON, fmt, ##__VA_ARGS__ )
#define logDBG(  fmt, ... ) logDBG_c(  REG_COMMON, fmt, ##__VA_ARGS__ )


#define logTIME( fmt, ... ) logERR_c(  REG_TIME,  fmt, ##__VA_ARGS__ )
#define logFD(   fmt, ... ) logERR_c(  REG_FD,    fmt, ##__VA_ARGS__ )

// Задать уровень лога
#define setLogDBLevel( lvl )                setLogLevel_c( lvl )
#define getLogDBLevel()                          getLogLevel_c()

// Задать имя БД лога(с путём, по умолчанию - ./logDb.db)
#define setLogDBName( name )                 setDBName_c( name )

// Задать макс размер очереди на запись в БД(одним запросом)
#define setLogDBMaxQuerySize( size1 ) setMaxQuerySize_c( size1 )
#define getLogDBMaxQuerySize() getMaxQuerySize_c()

// Изменить период записи в БД лога
#define setLogDBWritePeriod( per )     setWriteDBPeriod_c( per )
#define getLogDBWritePeriod()               getWriteDBPeriod_c()

// Задать макс кол-во записей в БД лога(более - самоочистка)
#define setLogDBMaxDBSize( size2 )       setMaxDBSize_c( size2 )
#define getLogDBMaxDBSize()                     getMaxDBSize_c()

// Задать кодировку входящих сообщений
#define setLogDBcode_CP866()                   setCode_CP866_c()
#define setLogDBcode_UTF8()                     setCode_UTF8_c()

#define LOG_ERROR    LOG_LEVELS::ERROR_L
#define LOG_WARNING  LOG_LEVELS::WARNING_L
#define LOG_INFO     LOG_LEVELS::INFO_L
#define LOG_DEBUG    LOG_LEVELS::DEBUG_L



class LoggerDaemon : public QObject
{
    Q_OBJECT
public:
    LoggerDaemon();
    ~LoggerDaemon();

//signals:
//    void started();
public slots:
    void start(); 	/*  запускает log Daemon */
};

#endif // LOGGERDAEMON_H
