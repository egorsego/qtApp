//
// Created by krazh on 03.11.17.
//

#ifndef LOGDB_LOGDB_H
#define LOGDB_LOGDB_H

// Created by krazh on 02.10.17.
//
// 4 �஢�� �����஢����(�롮� INFO, WARNING, ERROR, DEBUG)
// ����饭�� ⠪ �� ������� �� ॣ�����
// ������ � 1 ��
// �⤥��� ��⮪ �� �����
// �������᪨� ࠧ��� ����(ᮮ�饭�� � ���������� ���� �� ������ � �� � ����������)
// � ���ᯥ�⨢�, ��� ���誨:
// (�����஢���� ᯨ᪠ ����ᮢ? �� ������?), 䨪�஢��� ���� ���ॡ����� ����� � %?
// ��娢�஢���� ��ண� ����?
//
// �������
// ������ �஢��� �����஢���� (ERROR, WARNING, INFO, DEBUG)
// �������� ᮮ�饭�� � ��।� � 㪠������ ॣ���� � �஢��
// ������ ���� ��㡨�� ��(���� ���-�� ����ᥩ). �᫨ ����ᥩ ����� ����, � ����頥� ���� (����-(����/�))

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "sqlite3.h"
#include <string.h>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

#include "logdb_c_cpp.h" // ��� ����㯠 � ����

#include <QDebug>

using namespace std;


//=============================================================================================================
// �������� �� char � string
string _charToString(const char *source);


//=============================================================================================================
// ������� ����� ����� ����(��� ��।�)
struct LOG_MESSAGE
{
    LOG_LEVELS levelOfMess;         // �஢��� ᮮ�饭��
    LOG_REGIONS regionOfMess;       // �������, � ���ன �⭮���� ᮮ�饭��
    string mess;                    // ⥫� ᮮ�饭��
};


//=============================================================================================================
// ����� ������
// � ������ ᮮ�饭�� �������� ���।�⢮� ��।� ������� ⨯� "����饭��" messagesQuery,
// �ନ�㥬�� �맮���� �����

class Log_DB
{

private:
    atomic<bool> _work;             // ����� ������
    //---------------
    // ��⥪��
    mutex          mutexQuery;      // ��⥪� �� ����� � ��।�(������) �� ������ � ��
    //---------------
    sqlite3          *loggerDb;     // �����⥫� �� ��
    string          sqlRequest;     // ��ப� SQL-����� � ��
    string             dB_Name;     // ���  �� ����
    LOG_LEVELS        logLevel;     // �஢��� ����(�� ��襬, � �� ���)
    unsigned int  maxQuerySize;     // ���� ����� ��।� ����(��� ����� �� ��९������� �����)
    unsigned int     maxDBSize;     // ���� ������⢮ ����ᥩ � �� ����
    unsigned int writeDBPeriod;     // ��ਮ� ����� ᮮ�饭�� � �� ���� � ����ᥪ㭤��

    vector<LOG_MESSAGE> messagesQuery;    // ��।� ᮮ�饭��, �������� ������ � �� ����

    // ������ ᮮ�饭�� � �� ���� (�� 䠪�� ���������� � ��।� ��� ��᫥���饣� �����஢����)
    bool _log_in_sql(LOG_LEVELS lvl, LOG_REGIONS region, string mess);

    //---------------
    // ����� � ��
    bool _makeLoggerRequest();                  // �믮������ ����ᮢ � �� (����⨥+�����+����祭�� �⢥�)

    bool _createLoggerTable();                 // ��⮤ ᮧ����� ⠡���� ������
    bool _isLogDBExist();                      // ��⮤, �஢�ન ������� ��  ⠡���(��) ������
    int _countOfTables();                      // ��⮤, �������騩 ������⢮ �� ������
    bool _writeMessQToDB();                    // ��⮤ ���������� ᮮ�饭�� �� ��।� � �� ����
    int _sizeOfLogDB();                        // ��⮤, �������騩 ࠧ��� ⠡���� ����
    bool _deleteFromLogDB(int nRecords);       // ��⮤ 㤠����� �� �� ���� nRecords ����� ����ᥩ(���� �� ���)
    //---------------
public:

     Log_DB();
    ~Log_DB();


    // ������/������ ��� �� ����
    void   setDBName(string name) { dB_Name = name; };
    string getDBName()            { return dB_Name; };

    // ������/������ ���� ࠧ��� ����� ��।� ᮮ�饭�� �� ������ � ��
    void setMaxQuerySize(unsigned int size) { maxQuerySize = size; }
    unsigned int getMaxQuerySize() { return maxQuerySize; }

    // ������/������ ��ਮ� ����� � ���� ��।�  ᮮ�饭��
    void setWriteDBPeriod(unsigned usPer) { writeDBPeriod = usPer; };
    unsigned getWriteDBPeriod() { return writeDBPeriod; };

    // ������/������ ���� �᫮ ����ᥩ � ��
    void setMaxDBSize(unsigned sz) { maxDBSize = sz; };
    unsigned int getMaxDBSize() { return maxDBSize; };

    // ������/������ �஢��� ����
    void setLogLevel(LOG_LEVELS ll) { logLevel = ll; };
    LOG_LEVELS getLogLevel() { return logLevel; };

    // ��⮢� ����䥩� ������ � �ଠ�஢���� ������
    bool log_ERR (const LOG_REGIONS region,  string  strMess );
    bool log_WARN(const LOG_REGIONS region,  string  strMess );
    bool log_INFO(const LOG_REGIONS region,  string  strMess );
    bool log_DBG (const LOG_REGIONS region,  string  strMess );

    //------------------------------------------------------------------------
    // ��⮤ daemon, ����� ������ ���� ���饭 � ��. ��⮪�,
    // �㤥� ����� ᮮ�饭�� � �� �� ��।�(�����) ᮮ�饭��. ��横���.
    void logDaemon();
    //------------------------------------------------------------------------
    /**
     * @brief stopLogger - �몫���� ������
     */
    void stopLogger();
    /**
     * @brief setTermColor - ��⠭���� 梥� �뢮�� � �ନ���
     * @param lvl - �஢��� ����
     */
    void setTermColor(LOG_LEVELS lvl);
};

// ��������� ����� ������
static Log_DB logger;

#endif //LOGDB_LOGDB_H
