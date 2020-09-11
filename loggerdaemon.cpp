#include "loggerdaemon.h"

LoggerDaemon:: LoggerDaemon()
{
}

LoggerDaemon::~LoggerDaemon()
{
}


#include<QFile>
void LoggerDaemon::start()
{
    qDebug() << __func__ << "******* daemon start!" << endl;
    // Уровень лога
    setLogLevel_c(LOG_LEVELS::INFO_L);
    // Макс длина очереди на запись в БД лога
    setMaxQuerySize_c( 200 );
    // Макс число записей в БД лога
    setMaxDBSize_c(20000);
    // Частота записи в БД
    setWriteDBPeriod_c(500000);

#ifdef Q_OS_ANDROID   // push logs in /download/ in Android OS
    QString logFilePath{QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).at(0) + "/logDb.db"};
    qDebug() << "ANDROID LOG FILE PATH = " << logFilePath << endl;
    setDBName_c(logFilePath.toLocal8Bit());
#endif

    setLogDBcode_UTF8();

    //emit started();
    // Имя БД
    runLogDaemon();
}
