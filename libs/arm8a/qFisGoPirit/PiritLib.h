#ifndef PIRIT_LIB_H_
#define PIRIT_LIB_H_

#pragma once
#if defined(__GNUC__)
#define LINUX 7
#endif

#include <stdio.h>   /* �⠭����� ��� �����  �����/�뢮�� */
#include <fcntl.h>   /* ��� �����  �ࠢ����  䠩���� */
#include <errno.h>   /* ��� �����  ����� �訡�� */
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <resolv.h>
#include <unistd.h>  /* ��� �����  �⠭������ �㭪権 UNIX */
#include <termios.h> /* ��� �����  �ࠢ����  POSIX-�ନ����� */
#include <inttypes.h>
#include <typeinfo>
#include <typeindex>
#include <mutex>

#include "pirit_structs.h"
#include "pirit_low_func.h"
#include "biosCounters.h"

using namespace std;

#define DRIVER_VERSION (unsigned long long)2
#define DRIVER_BUILD   (unsigned long long)120

#define MAX_PATH 260

#define CORRECTION_INDEPENDENT  0x00
#define CORRECTION_PRESCRIPTION 0x01
#define CORRECTION_INCOMING     0x00
#define CORRECTION_OUTGOING     0x02
#define CORRECTION_TAX_1        0x00
#define CORRECTION_TAX_2        0x04
#define CORRECTION_TAX_4        0x08
#define CORRECTION_TAX_8        0x0C
#define CORRECTION_TAX_16       0x10
#define CORRECTION_TAX_32       0x14

#define DEVICE_MODE_DATA_ENCRYPTION                0x00000001UL
#define DEVICE_MODE_OFFLINE                        0x00000002UL
#define DEVICE_MODE_AUTOMATIC                      0x00000004UL
#define DEVICE_MODE_SERVICE_SIGN                   0x00000008UL
#define DEVICE_MODE_BSO_SING                       0x00000010UL
#define DEVICE_MODE_CALC_ONLINE_SIGN               0x00000020UL
#define DEVICE_MODE_SALE_EXCISABLE_GOODS           0x00000040UL
#define DEVICE_MODE_SIGN_OF_GAMBLING               0x00000080UL
#define DEVICE_MODE_SIGN_OF_BANK_PAYMENT_AGENTS    0x00000100UL
#define DEVICE_MODE_SIGN_OF_BANK_PAYMENT_SUBAGENTS 0x00000200UL
#define DEVICE_MODE_SIGN_OF_PAYMENT_AGENTS         0x00000400UL
#define DEVICE_MODE_SIGN_OF_PAYMENT_SUBAGENTS      0x00000800UL
#define DEVICE_MODE_SIGN_OF_ATTORNEY               0x00001000UL
#define DEVICE_MODE_SIGN_OF_COMISSIONERS           0x00002000UL
#define DEVICE_MODE_SIGN_OF_AGENTS                 0x00004000UL
#define DEVICE_MODE_SIGN_OF_LOTTERY                0x00008000UL

// ��ꥪ� ��� ��饭�� �� ��⮪��� ����, ��⮤� ���� ��� ��
extern PIRIT_IOCLASS pirit_io;

//==============================================================================================
// �஢��� ���筮�� �訡�� �� �⢥⭮�� �����
enum FB_CRITICAL_LEVEL
{
    CL_WARNING           = 0,    // �訡�� �� ���筠, ������ �믮�����, �� � ��୨����
    CL_FISCAL            = 1,    // �訡�� ���筠 ��� ��        ����権
    CL_FISC_AND_NOT_FISC = 2,    // �訡�� ���筠 ��� ��/���� ����権
    CL_ALL               = 3     // �訡�� ���筠 ��� ��� ⨯�� ����権 (�� 䠪��, ��� ᮥ�������)
};

enum PIRIT_LOCAL_LICENSE_OPTIONS_MASK
{
    OPTION_1200     = 0x01,
    OPTION_FFD11    = 0x02,
    OPTION_SELL_SIZ = 0x04
};

//==============================================================================================
// ��ࠢ����� ���⠬� (��⮢�� ��᪠)
enum BIOS_CALC_BIT_MASK
{
    CALC_CASH_CONTROL        = 0x01,   //	0 - ����祭 ����஫� ������� � �������� �騪�
    CALC_CHECK_ANNUL_CHECKS  = 0x02,   //   0 - ���뢠�� 祪�, ���㫨஢���� �� ����祭�� ��⠭��
    CALC_AUTO_INCAS          = 0x04,   //   0 - ��⮬���᪠� �������� �몫�祭�
    CALC_RASHOD              = 0x08,   //   0 - ���稪� ���㯮�(��室��) �몫�祭�
    CALC_AUTO_PRINT_SCL      = 0x10,   //   0 - ��⮬���᪠� ����� ��� �몫�祭�
    CALC_SCL                 = 0x20,   //   0 - ��� �⪫�祭�
    CALC_ITOG_PROD           = 0x40,   //   0 - ����� �㬬� ������饣� �⮣� �த��(��室��)/���㯮�(��室��) �� X-����� � ����� � �����⨨ ᬥ�� �⪫�祭�
    CALC_ITOG_VOZVR          = 0x80    //   0 - ����� �㬬� ������饣� �⮣� �����⮢ (��室� � ��室�) �� X-����� � ����� � �����⨨ ᬥ�� �⪫�祭�
};


// ��� �஢������ ����樨 ��� ��।������ ���筮�� ����祭��� �訡��
enum FB_OPERATION_CRITICAL_LEVEL
{
    OP_FISCAL            = 1,
    OP_NOT_FISCAL        = 2,
    OP_CONNECTION        = 3

};

enum FB_ERROR_CODE
{
    FB_NOT_ERROR = 0x00, // ������� �믮����� ��� �訡��

    // �訡�� �믮������ ������
    FB_ERROR_FUNCTION_IS_NOT_FEASIBLE_WITH_STATUS = 0x01,   // �㭪�� ���믮����� �� ������ ����� ���
    FB_ERROR_INVALID_FUNCTION_NUMBER_IN_COMMAND = 0x02,     // � ������� 㪠��� ������ ����� �㭪樨
    FB_ERROR_INVALID_COMMAND_FORMAT_OR_PARAMETER = 0x03,    // �����४�� �ଠ� ��� ��ࠬ��� �������

    // �訡�� ��⮪��� ��।�� ������
    FB_ERROR_COMMUNICATION_PORT_BUFFER_OWERFLOW = 0x04,     // 	��९������� ���� ����㭨��樮����� ����
    FB_ERROR_TIMEOUT_WHEN_TRANSMITTING_INFORMATION = 0x05,  // ������� �� ��।�� ���� ���ଠ樨
    FB_ERROR_INCORRECT_PASSWORD = 0x06,                     // 	� ��⮪��� 㪠��� ������ ��஫�
    FB_ERROR_COMMAND_CHECKSUM = 0x07,                       // �訡�� ����஫쭮� �㬬� � �������

    // �訡�� �����饣� ���ன�⢠
    FB_ERROR_END_OF_PAPER = 0x08,       // ����� �㬠��
    FB_ERROR_PRINTER_NOT_READY = 0x09,  // �ਭ�� �� ��⮢

    // �訡�� ����/�६���
    FB_ERROR_CURRENT_SHIFT_MORE_24 = 0x0A,              // ������ ᬥ�� ����� 24 �ᮢ
    FB_ERROR_DIFFERENT_TIME_MORE_8_MIN = 0x0B,          // ������ �� �६���, ��� � 㪠������ � ������� ��砫� ࠡ���, ����� 8 �����
    FB_ERROR_ENTERED_DATE_EARLIER_LAST_FISCAL = 0x0C,   // �������� ��� ����� ࠭���, 祬 ��� ��᫥���� �᪠�쭮� ����樨

    // ��稥 �訡��
    FB_ERROR_NEGATIVE_RESULT = 0x0E,        // ����⥫�� १����
    FB_ERROR_NEED_CLOSE_SHIFT = 0x0F,       // ��� �믮������ ������� ����室��� ������� ᬥ��
    FB_ERROR_NO_DATA_IN_JOURNAL = 0x10,     // ��� ������ � ��ୠ��
    FB_ERROR_CONTROL_TAPE = 0x11,           // �訡�� ����஫쭮� �����
    FB_ERROR_SENDING_DATA_TO_OFD = 0x12,    // �訡�� ���뫪� ������ � ���

    // ��⠫�� �訡��
    FB_ERROR_FATAL = 0x20,  // ��⠫쭠� �訡�� ���. ��稭� ������������� ������ �訡�� ����� ��筨�� � ?����� �⠫��� �訡�� ���?

    // �訡�� ��
    FB_ERROR_INVALID_FORMAT_OR_PARAMETER_FN = 0x41,     // �����४�� �ଠ� ��� ��ࠬ��� ������� ��
    FB_ERROR_INCORRECT_STATE_FN = 0x42,                 // �����४⭮� ���ﭨ� ��
    FB_ERROR_FN = 0x43,                                 // �訡�� ��
    FB_ERROR_CRYPTOGRAPHIC_COPROCESSOR = 0x44,          // �訡�� �� (�ਯ⮣���᪮�� ᮯ�����) � ��⠢� ��
    FB_ERROR_EXHAUSTED_TIME_RESOURCE_FN = 0x45,         // ���௠� �६����� ����� �ᯮ�짮����� ��
    FB_ERROR_FN_OVERFLOW = 0x46,                        // �� ��९�����
    FB_ERROR_INVALID_DATE_OR_TIME = 0x47,               // ������ ��� ��� �६�
    FB_ERROR_NO_DATA_REQUESTED = 0x48,                  // ��� ����襭��� ������
    FB_ERROR_INVALID_DATA_PARAMETERS = 0x49,            // �����४�� ��ࠬ���� �������
    FB_ERROR_TLV_DATA_SIZE_EXCEEDED = 0x50,             // �ॢ�襭 ࠧ��� ������ TLV
    FB_ERROR_NO_TRANSPORT_CONNECTION = 0x51,            // ��� �࠭ᯮ�⭮�� ᮥ�������
    FB_ERROR_EXHAUSTED_RESOURCE_COP = 0x52,             // ���௠� ����� ��
    FB_ERROR_MEMORY_TO_OFD_EXHAUSTED = 0x54,            // ���௠�� ������ �࠭���� ���
    FB_ERROR_TIME_TO_OFD_MORE_30_DAYS = 0x55,           // �६� ��宦����� � ��।� ᠬ��� ��ண� ᮮ�饭�� �� �뤠�� ����� 30 ���������� ����.
    FB_ERROR_DURATION_SHIFT_IN_FN_MORE_24 = 0x56,       // �த����⥫쭮��� ᬥ�� �� ����� 24 �ᮢ
    FB_ERROR_DIFFERENT_TIME_MORE_5_MINUTS_IN_FN = 0x57, // ������ ����� 祬 �� 5 ����� �⫨砥��� �� ࠧ����, ��।������� �� ����७���� ⠩���� ��.
    FB_ERROR_INVALID_MESSAGE_FROM_OFD = 0x60,           // ����୮� ᮮ�饭�� �� ���
    FB_ERROR_NO_CONNECTION_EITH_FN = 0x61,              // ��� �裡 � ��
    FB_ERROR_EXHANGE_ERROR_WITH_FN = 0x62,              // �訡�� ������ � ��
    FB_ERROR_TOO_LONG_COMMAND_TO_FN = 0x63,             // ���誮� ������� ������� ��� ���뫪� � ��

    FB_ERROR_ALL
};

// ������� ��� �࠭���� ����� �訡�� �� �⢥⭮�� ����� �㭨��
struct FB_err
{
    int                     code;     // ��� �訡��
    string                  mess;     // �����஢�� ���� �訡��
    FB_CRITICAL_LEVEL       cLvl;     // �訡�� ���� ����᪮� ��� ������� �஢�� � ���

    FB_err(int c, const char* m, FB_CRITICAL_LEVEL newCLvl)
    {
        code =         c;
        mess = string(m);
        cLvl =   newCLvl;
    }

    FB_err(const FB_err& other) : code(other.code), mess(other.mess), cLvl(other.cLvl) {}
};


// ===================================== �訡�� ��� �� ���� =================================
const vector <FB_err> FB_errs_RF
        {
/*                     ��� |  �����������                          | ������� ����������� ������                                 */

                // �訡�� �믮������ ������
                FB_err(FB_ERROR_FUNCTION_IS_NOT_FEASIBLE_WITH_STATUS, "⥪.����� ���: �㭪�� ���믮�����", FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_INVALID_FUNCTION_NUMBER_IN_COMMAND, "������ ����� �㭪樨",                FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_INVALID_COMMAND_FORMAT_OR_PARAMETER, "�����४�� �ଠ�/���-� �������",    FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(0x40, "������ ��ࠬ��� �������",           FB_CRITICAL_LEVEL:: CL_ALL           ),
                // �訡�� ��⮪��� ��।�� ������
                FB_err(FB_ERROR_COMMUNICATION_PORT_BUFFER_OWERFLOW, "��९������� ���� ����㭨�. ����",  FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_TIMEOUT_WHEN_TRANSMITTING_INFORMATION, "������� �� ��।�� ���-樨",      FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_INCORRECT_PASSWORD, "������ ��஫� � �����",                             FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_COMMAND_CHECKSUM, "�訡�� �����.�㬬� � �������",                           FB_CRITICAL_LEVEL:: CL_ALL),
                // �訡�� �����饣� ���ன�⢠
                FB_err(FB_ERROR_END_OF_PAPER, "����� �㬠��",           FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC),
                FB_err(FB_ERROR_PRINTER_NOT_READY, "�ਭ�� �� ��⮢",  FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC),
                // 	�訡�� ����/�६���
                FB_err(FB_ERROR_CURRENT_SHIFT_MORE_24, "������ ᬥ�� ����� 24 �ᮢ",             FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_DIFFERENT_TIME_MORE_8_MIN, "������ �� �६��� >8���.",             FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_ENTERED_DATE_EARLIER_LAST_FISCAL, "���<���� ���. ��.��-樨",    FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_NEGATIVE_RESULT, "����⥫�� १����",                         FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC),
                FB_err(FB_ERROR_NEED_CLOSE_SHIFT, "����室��� ������� ᬥ��",                       FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_NO_DATA_IN_JOURNAL, "��� ������ � ��ୠ��",                         FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_CONTROL_TAPE, "�訡�� ����஫쭮� �����",                           FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_SENDING_DATA_TO_OFD, "�訡�� ���.������ � ���",                    FB_CRITICAL_LEVEL:: CL_FISCAL),
                // 	��⠫�� �訡��
                FB_err(FB_ERROR_FATAL, "��⠫쭠� �訡�� ���", FB_CRITICAL_LEVEL:: CL_ALL),
                // �訡�� ��
                FB_err(0x21, "�� ��९�����", FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(0x30, "������ ��",   FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_INVALID_FORMAT_OR_PARAMETER_FN, "�����४�� �ଠ� ������� ��",   FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_INCORRECT_STATE_FN, "�����४⭮� ���ﭨ� ��",                    FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_FN, "�訡�� ��",                                                    FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_CRYPTOGRAPHIC_COPROCESSOR, "�訡�� �� � ��",                        FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_EXHAUSTED_TIME_RESOURCE_FN, "���௠� �६����� ����� ��",         FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_FN_OVERFLOW, "�� ��९�����",                                       FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_INVALID_DATE_OR_TIME, "������ ��� ��� �६�",                    FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_NO_DATA_REQUESTED, "��� ����襭��� ������",                        FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_INVALID_DATA_PARAMETERS, "�����४�� ��ࠬ���� �������",          FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_TLV_DATA_SIZE_EXCEEDED, "�ॢ�襭 ࠧ��� ������ TLV",               FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_NO_TRANSPORT_CONNECTION, "��� �࠭ᯮ�⭮�� ᮥ�������",            FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_EXHAUSTED_RESOURCE_COP, "���௠� ����� ��",                       FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_MEMORY_TO_OFD_EXHAUSTED, "���௠�� ������ �࠭���� ���",           FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_TIME_TO_OFD_MORE_30_DAYS, "�६� �࠭���� >30 ����",                FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_DURATION_SHIFT_IN_FN_MORE_24, "C���� �� >24 �ᮢ",                 FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_DIFFERENT_TIME_MORE_5_MINUTS_IN_FN, "������ >5 ����� ࠧ���� ��",  FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_INVALID_MESSAGE_FROM_OFD, "����୮� ᮮ�饭�� �� ���",              FB_CRITICAL_LEVEL:: CL_WARNING),
                FB_err(FB_ERROR_NO_CONNECTION_EITH_FN, "��� �裡 � ��",                            FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_EXHANGE_ERROR_WITH_FN, "�訡�� ������ � ��",                        FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_TOO_LONG_COMMAND_TO_FN, "����.����. ������� ��� ��",               FB_CRITICAL_LEVEL:: CL_WARNING),

                FB_err(1001, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ���� ���� �� STX";
                FB_err(1002, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "�� ���: �� ᮢ������ ID �����";
                FB_err(1003, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "�� ���: �� ᮢ������ ����� ������� 1-� ����";
                FB_err(1004, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "�� ���: �� ᮢ������ ����� ������� 2-�� ����";
                FB_err(1005, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ��᫥���� ���� �� ETX";
                FB_err(1006, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: �訡�� ����஫쭮� �㬬� CRC";
                FB_err(1007, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ��ࠢ�� ������ ����� ��ࠢ����� �� ���������";
                FB_err(1008, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ᫨誮� ���⪨� �����";
                FB_err(1009, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ����� �裡 � ��";
                FB_err(1010, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ��ࠡ�⪥ �⢥�: ���-�� ��ࠬ��஢ �� ᮮ⢥����� ��������";
                FB_err(1011, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� �� �⢥砥� �� ������� Ping";

                FB_err(2000, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "���� �� �����";
                FB_err(2001, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "���������� ������ ����";
                FB_err(2002, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ���� �� ����⨨";
                FB_err(2003, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ���� �� ���樠����樨";
                FB_err(2010, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ᮥ������� � ����";
                FB_err(2011, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ᮧ����� ᮪��";

                FB_err(2222, "�訡�� ������ � ��",                FB_CRITICAL_LEVEL:: CL_ALL            )  // ���� ��ࠬ��஢ �� ���� ����� parseAnswerN<double>()

//                FB_err(1001, "�訡�� ���: 1001",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ���� ���� �� STX";
//                FB_err(1002, "�訡�� ���: 1002",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "�� ���: �� ᮢ������ ID �����";
//                FB_err(1003, "�訡�� ���: 1003",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "�� ���: �� ᮢ������ ����� ������� 1-� ����";
//                FB_err(1004, "�訡�� ���: 1004",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "�� ���: �� ᮢ������ ����� ������� 2-�� ����";
//                FB_err(1005, "�訡�� ���: 1005",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ��᫥���� ���� �� ETX";
//                FB_err(1006, "�訡�� ���: 1006",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: �訡�� ����஫쭮� �㬬� CRC";
//                FB_err(1007, "�訡�� ���: 1007",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ��ࠢ�� ������ ����� ��ࠢ����� �� ���������";
//                FB_err(1008, "�訡�� ���: 1008",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ᫨誮� ���⪨� �����";
//                FB_err(1009, "�訡�� ���: 1009",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ����� �裡 � ��";
//                FB_err(1010, "�訡�� ���: 1010",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ��ࠡ�⪥ �⢥�: ���-�� ��ࠬ��஢ �� ᮮ⢥����� ��������";
//                FB_err(1011, "�訡�� ���: 1011",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� �� �⢥砥� �� ������� Ping";
//
//                FB_err(2000, "�訡�� ���: 2000",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "���� �� �����";
//                FB_err(2001, "�訡�� ���: 2001",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "���������� ������ ����";
//                FB_err(2002, "�訡�� ���: 2002",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ���� �� ����⨨";
//                FB_err(2003, "�訡�� ���: 2003",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ���� �� ���樠����樨";
//                FB_err(2010, "�訡�� ���: 2010",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ᮥ������� � ����";
//                FB_err(2011, "�訡�� ���: 2011",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ᮧ����� ᮪��";
//
//                FB_err(2222, "�訡�� ���: 2222",                FB_CRITICAL_LEVEL:: CL_ALL            )  // ���� ��ࠬ��஢ �� ���� ����� parseAnswerN<double>()
        };


// ===================================== �訡�� ��� �������᪮�� ���� =================================
const vector <FB_err> FB_errs_RB
        {
/*                     ��� |  �����������                          | ������� ����������� ������                                 */

                //-----------------------------------------------------------------------------------------
                // �訡�� �믮������ ������
                //-----------------------------------------------------------------------------------------
                FB_err(0x01, "⥪.����� ���: �㭪�� ���믮�����", FB_CRITICAL_LEVEL:: CL_ALL           ),
                FB_err(0x02, "������ ����� �㭪樨",              FB_CRITICAL_LEVEL:: CL_ALL           ),
                FB_err(0x03, "�����४�� �ଠ�/���-� �������",   FB_CRITICAL_LEVEL:: CL_ALL           ),
                //-----------------------------------------------------------------------------------------
                // �訡�� ��⮪��� ��।�� ������
                //-----------------------------------------------------------------------------------------
                FB_err(0x04, "��९������� ���� ����㭨�. ����", FB_CRITICAL_LEVEL:: CL_ALL           ),
                FB_err(0x05, "������� �� ��।�� ���-樨",        FB_CRITICAL_LEVEL:: CL_ALL           ),
                FB_err(0x06, "������ ��஫� � �����",            FB_CRITICAL_LEVEL:: CL_ALL           ),
                FB_err(0x07, "�訡�� �����.�㬬� � �������",        FB_CRITICAL_LEVEL:: CL_ALL           ),
                //-----------------------------------------------------------------------------------------
                // �訡�� �����饣� ���ன�⢠
                //-----------------------------------------------------------------------------------------
                FB_err(0x08, "����� �㬠��",                        FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x09, "�ਭ�� �� ��⮢",                    FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                //-----------------------------------------------------------------------------------------
                // 	�訡�� ����/�६���
                //-----------------------------------------------------------------------------------------
                FB_err(0x0A, "������ ᬥ�� ����� 24 �ᮢ",       FB_CRITICAL_LEVEL:: CL_ALL            ),
                FB_err(0x0B, "������ �� �६��� >8���.",           FB_CRITICAL_LEVEL:: CL_FISCAL         ),
                FB_err(0x0C, "���<���� ���. ��.��-樨",         FB_CRITICAL_LEVEL:: CL_FISCAL         ),
                FB_err(0x0D, "������ ��஫� ���",                 FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x0E, "����⥫�� १����",             FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x0F, "����室��� ������� ᬥ��",            FB_CRITICAL_LEVEL:: CL_FISCAL         ),
                FB_err(0x10, "��������� ������ � ᬥ��",           FB_CRITICAL_LEVEL:: CL_FISCAL         ),
                FB_err(0x48, "��� ����襭��� ������",              FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x20, "��⠫쭠� �訡�� ���",                FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x21, "������ ��� ��९������",              FB_CRITICAL_LEVEL:: CL_FISCAL         ),
                FB_err(0x80, "���� �訡�� ����",                   FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x91, "��� �裡 � ����",                    FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x92, "��������� ����",                    FB_CRITICAL_LEVEL:: CL_ALL ),
                FB_err(0x93, "���� ����ࠢ��" ,                    FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x94, "�訡�� ������-��" ,                  FB_CRITICAL_LEVEL:: CL_ALL ),
                FB_err(0x96, "����䨪�� ���� ����祭",             FB_CRITICAL_LEVEL:: CL_ALL ),
                FB_err(0x97, "Z ������ ����।����",                FB_CRITICAL_LEVEL:: CL_ALL ),
                FB_err(0x98, "������ ���� ��९������",             FB_CRITICAL_LEVEL:: CL_ALL ),
                //-----------------------------------------------------------------------------------------
                FB_err(1001,  "�訡�� �����.�㬬� � ������",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ���� ���� �� STX";
                FB_err(1002,  "�訡�� �����.�㬬� � ������",               FB_CRITICAL_LEVEL:: CL_WARNING        ), // "�� ���: �� ᮢ������ ID �����";
                FB_err(1003,  "�訡�� �����.�㬬� � ������",               FB_CRITICAL_LEVEL:: CL_WARNING        ), // "�� ���: �� ᮢ������ ����� ������� 1-� ����";
                FB_err(1004,  "�訡�� �����.�㬬� � ������",               FB_CRITICAL_LEVEL:: CL_WARNING        ), // "�� ���: �� ᮢ������ ����� ������� 2-�� ����";
                FB_err(1005,  "�訡�� �����.�㬬� � ������",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ��᫥���� ���� �� ETX";
                FB_err(1006,  "�訡�� �����.�㬬� � ������",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: �訡�� ����஫쭮� �㬬� CRC";
                FB_err(1007,  "�訡�� �����.�㬬� � ������",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ��ࠢ�� ������ ����� ��ࠢ����� �� ���������";
                FB_err(1008,  "�訡�� �����.�㬬� � ������",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ᫨誮� ���⪨� �����";
                FB_err(1009, "������� �� ��।�� ���-樨",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ���: ����� �裡 � ��";
                FB_err(1010,  "�訡�� �����.�㬬� � ������",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� ��ࠡ�⪥ �⢥�: ���-�� ��ࠬ��஢ �� ᮮ⢥����� ��������";
                FB_err(1011, "������� �� ��।�� ���-樨",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�� �� �⢥砥� �� ������� Ping";
                FB_err(2000, "������� �� ��।�� ���-樨",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "���� �� �����";
                FB_err(2001, "������� �� ��।�� ���-樨",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "���������� ������ ����";
                FB_err(2002, "������� �� ��।�� ���-樨",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ���� �� ����⨨";
                FB_err(2003, "������� �� ��।�� ���-樨",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ���� �� ���樠����樨";
                FB_err(2010, "������� �� ��।�� ���-樨",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ᮥ������� � ����";
                FB_err(2011, "������� �� ��।�� ���-樨",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "�訡�� ᮧ����� ᮪��";
                FB_err(2222,  "�訡�� �����.�㬬� � ������",               FB_CRITICAL_LEVEL:: CL_ALL            )  // ���� ��ࠬ��஢ �� ���� ����� parseAnswerN<double>()
        };
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
// ������ ����
//------------------------------------------------------------------
enum BIOS_STATUS
{
    BIOS_STATUS_KKT_NOT_INITED         = 0x0001,             //  0-� ���    �� �뫠 �맢��� �㭪�� ?��砫� ࠡ���?
    BIOS_STATUS_KKT_NOT_FISCAL         = 0x0002,             //  1-� ��� 	 ���᪠��� ०��
    BIOS_STATUS_KKT_IS_SHIFT_OPEN      = 0x0004,             //  2-� ��� 	 ����� �����
    BIOS_STATUS_KKT_IS_SHIFT_24H       = 0x0008,             //  3-� ���    ����� ����� 24 �ᮢ
    BIOS_STATUS_KKT_FS_CLOSE           = 0x0010,             //  4-� ���    ��娢 �� ������
    BIOS_STATUS_KKT_NOT_REGISTERED     = 0x0020,             //  5-� ���    �� �� ��ॣ����஢�� (postFiscal =   kkt_notFiscal (???) kkt_notRegistered)
    BIOS_STATUS_KKT_CLOSE_SHIFT_NEED   = 0x0100,             //  8-� ���    �� �뫮 �����襭� �����⨥ ᬥ��, ����室��� ������� ������
    BIOS_STATUS_KKT_CL_ERR             = 0x0200,             //  9-� ���    �訡�� ����஫쭮� �����

    BIOS_STATUS_FATAL_WRONG_SUM_NVR    = 0x0001,             // 1-� ��� ����ୠ� ����஫쭠� �㬬� NVR
    BIOS_STATUS_FATAL_WRONG_SUM_KONF   = 0x0002,             // 2-� ��� ����ୠ� ����஫쭠� �㬬� � ���䨣��樨
    BIOS_STATUS_FATAL_FS_NOT_ATH       = 0x0020,             // 6-� ��� ��� �� ���ਧ�����
    BIOS_STATUS_FATAL_FS_ERR           = 0x0040,             // 7-� ��� ��⠫쭠� �訡�� ��
    BIOS_STATUS_FATAL_SD_ERR           = 0x0100              // 9-� ��� 	SD ���� ��������� ��� ����ࠢ��
};

//-------------------------------------------------
// ����� ��� �ਭ�� BIOS
enum FONTS
{
    FONT_NORM               = 0x01,               //  ���� 9�17
    FONT_NORM_DBL_H         = 0x11,               //  ���� 9�17, ������� �����
    FONT_NORM_DBL_W         = 0x21,               //  ���� 9�17, ������� �ਭ�
    FONT_NORM_DBL_H_W       = 0x31,               //  ���� 12�24, ������� �����, ������� �ਭ�

    FONT_BIG                = 0x00,               //  ���� 12�24
    FONT_BIG_DBL_H          = 0x10,               //  ���� 12�24, ������� �����
    FONT_BIG_DBL_W          = 0x20,               //  ���� 12�24, ������� �ਭ�
    FONT_BIG_DBL_H_W        = 0x30,               //  ���� 12�24, ������� �����, ������� �ਭ�
};

//-------------------------------------------------
// ⨯ ���뢠����� ���㬥��
enum DOC_TYPES
{
    DOC_TYPE_SERVICE              = 0x01,               //  ��ࢨ�� ���㬥��
    DOC_TYPE_SALE                 = 0x02,               //  ��� �� �த��� (��室)
    DOC_TYPE_SALE_RET             = 0x03,               //  ��� �� ������ (������ ��室�)
    DOC_TYPE_INSERTION            = 0x04,               //   ���ᥭ�� � �����
    DOC_TYPE_RESERVE              = 0x05,               //  ��������
    DOC_TYPE_CONSUMPTION          = 0x06,               //  ��� �� ���㯪� (��室)
    DOC_TYPE_CONSUMPTION_RET      = 0x07                //  ��� �� ������ ���㯪� (������ ��室�)
};

//-------------------------------------------------
// ⨯ ���뢠����� ���㬥�� ��
enum DOC_TYPES_BY
{
    DOC_TYPE_BY_SERVICE        = 0x01,               //  ��ࢨ�� ���㬥��
    DOC_TYPE_BY_SALE           = 0x02,               //  ��� �� �த��� (��室)
    DOC_TYPE_BY_SALE_RET       = 0x03,               //  ��� �� ������ (������ ��室�)
    DOC_TYPE_BY_INSERTION      = 0x04,               //  ���ᥭ�� � �����
    DOC_TYPE_BY_RESERVE        = 0x05,               //  ��������
    DOC_TYPE_BY_ORDER_CREATE   = 0x06,               // ��� ��ଫ���� ������
    DOC_TYPE_BY_ORDER_CLOSE    = 0x07,               // ��� ������� ������
    DOC_TYPE_BY_ORDER_CANCEL   = 0x08,               // ��� �⬥�� ������
    DOC_TYPE_BY_ORDER_ADD      = 0x09,               // ��� ���������� ������
    DOC_TYPE_BY_ORDER_COR      = 0x0a,               // ��� ���४樨 ������
};

//-------------------------------------------------
// ०�� �ନ஢���� ���㬥��
enum DOC_FORMATS
{
    DOC_FORMAT_NORMAL        = 0x00,               //  ����� ०�� �ନ஢���� ���㬥��
    DOC_FORMAT_PACKET        = 0x10,               //   ������ ०�� �ନ஢���� ���㬥��
    DOC_FORMAT_FAST          = 0x90                //   ������ ०�� �ନ஢���� ���㬥��
};

//-------------------------------------------------
//  ०�� ���� ४����⮢
enum DOC_REQ_PRINT
{
    DOC_REQ_PRINT_NORMAL     = 0x00,               //  ����� ०�� ���� ४����⮢
    DOC_REQ_PRINT_DELAYED    = 0x20               //   ����� �⫮������ ���� ४����⮢
};

//-------------------------------------------------
//  ⨯� ��⥬ ���������������
enum SNO_TYPES
{
    SNO_OSN          = 0x00,               //  �᭮����
    SNO_USN          = 0x01,               //  ���饭��� ��室
    SNO_USN_DOH_RASH = 0x02,               //  ���饭��� ��室 ����� ���室
    SNO_ENVD         = 0x03,               //  ����� ����� �� �������� ��室
    SNO_ESN          = 0x04,               //  ����� ᥫ�᪮宧��⢥��� �����
    SNO_PATENT       = 0x05                //  ��⥭⭠� ��⥬� ���������������
};
//-------------------------------------------------
//  ������� PIRIT RB
enum PIRIT_RB_COMMAND
{
    PIRIT_RB_CANCEL_DOC        = 0x35, // ���㫨஢��� ���㬥��
    PIRIT_RB_DOC_FROM_KL_PRINT = 0x70, // ��ᯥ���� ���㬥�� �� �� �� ������
    PIRIT_RB_DOC_FROM_KL_READ  = 0xf0  // ������ ���㬥�� �� �� �� ������
};

//  ��ࠬ���� ��� PIRIT_RECEIPT_DATA
enum PIRIT_RECEIPT_DATA_PARAM
{
    PIRIT_RECEIPT_DATA_CUR_DOC  = 0x01, // ����� �� ⥪�饬� 祪�
    PIRIT_RECEIPT_DATA_LAST_DOC = 0x02  // ����� �� �����⮬� 祪�
};

//  ��ࠬ���� ��� PIRIT_RECEIPT_DATA
enum PIRIT_REG_DOC_TYPE
{
    PIRIT_REG_DOC_TYPE_UNDEFINED = 0x00, // BAD
    PIRIT_REG_DOC_TYPE_REG       = 0x01, // registration
    PIRIT_REG_DOC_TYPE_REREG     = 0x0B  // reregistration
};

//-------------------------------------------------
//  ������� ��� ⨯�� ���⥦��
enum PIRIT_RECEIPT_INDEX
{
    PIRIT_RECEIPT_CASH      = 0x00,    // ���������
    PIRIT_RECEIPT_CASHLESS  = 0x01,    // ������������
    PIRIT_RECEIPT_USER_0    = 0x02,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_USER_1    = 0x03,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_USER_2    = 0x04,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_USER_3    = 0x05,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_USER_4    = 0x06,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_USER_5    = 0x07,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_USER_6    = 0x08,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_USER_7    = 0x09,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_USER_8    = 0x0a,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_USER_9    = 0x0b,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_USER_10   = 0x0c,    // ���짮��⥫�᪨� ��ப� ������������ ���⥦�
    PIRIT_RECEIPT_ADVANCE   = 0x0d,    // ��������������� ������ (�����)
    PIRIT_RECEIPT_CREDIT    = 0x0e,    // ����������� ������ (������)
    PIRIT_RECEIPT_EXCHANGE  = 0x0f     // ���� ����� ������
};

//-------------------------------------------------
//  ��ࠬ���� ��� ������� PIRIT_KKT_INFO
enum PIRIT_KKT_INFO_PARAM
{
    PIRIT_KKT_INFO_KKT_PLANT_NUM        = 0x01,     //  ������ �����᪮� ����� ���
    PIRIT_KKT_INFO_ID_FIRMWARE          = 0x02,     //  ������ �����䨪��� ��訢��
    PIRIT_KKT_INFO_INN                  = 0x03,     //	������ ���
    PIRIT_KKT_INFO_PLANT_NUM            = 0x04,     //  ������ ॣ����樮��� ����� ���
    PIRIT_KKT_INFO_DATE_TIME_LAST_FD    = 0x05,     //	������ ���� � �६� ��᫥���� �᪠�쭮� ����樨
    PIRIT_KKT_INFO_DATE_TIME_REG        = 0x06,     //  ������ ���� ॣ����樨 / ���ॣ����樨
    PIRIT_KKT_INFO_CASH_IN_DROWER       = 0x07,     //  ������ �㬬� ������� � �������� �騪�
    PIRIT_KKT_INFO_NEXT_DOC             = 0x08,     //  ������ ����� ᫥���饣� ���㬥��
    PIRIT_KKT_INFO_FS_VALIDITY_DATE     = 0x0E,     //  ������ ����� ���祩 ��
    PIRIT_KKT_INFO_DATE_TIME_OPEN_SHIFT = 0x11,     //  ������ ���� � �६� ������ ᬥ��
    PIRIT_KKT_INFO_CNO_SIGNS_AGENT      = 0x17      //  ������ ��⥬� ��������������� � ०�� ࠡ��� � ��
};

//-------------------------------------------------
//  ��ࠬ���� ��� ������� PIRIT_READ_TABLE_SETTINGS
enum PIRIT_TABLE_SETTINGS
{
    PIRIT_TABLE_SETTINGS_KKT        = 0x01,     //  ��ࠬ���� ��
    PIRIT_TABLE_SETTINGS_PURCHASE   = 0x02,     //  ��ࠬ���� 祪�
    PIRIT_TABLE_CALC_MASK           = 0x05,     //  ��ࠢ����� ���⠬� (��⮢�� ��᪠)
    PIRIT_TABLE_NAME_ADDRESS_ORG    = 0x1E,     //  ������������ � ���� �࣠����樨
    PIRIT_TABLE_AUTOMAT_NUM         = 0x46,     //  ����� ��⮬��
    PIRIT_TABLE_OFD_INN             = 0x47,     //  ��� ���
    PIRIT_TABLE_OFD_IP              = 0x4D,     //  ���� �ࢥ� ��� ��� ��ࠢ�� ���㬥�⮢
    PIRIT_TABLE_OFD_PORT            = 0x4E,     //  ���� �ࢥ� ���
    PIRIT_TABLE_OFD_NAME            = 0x51,     //  ������������ ���
    PIRIT_REC_SENDER_EMAIL          = 0x52,     //  �����஭��� ���� ��ࠢ�⥫� 祪�
    PIRIT_TABLE_CALCULATION_PLACE   = 0x55,     //  ���� ���⮢
    PIRIT_TABLE_LIC_OPTIONS         = 0x58      //  ������� ���� �������� ��業���(���, ��権)
};

//-------------------------------------------------
//  ��ࠬ���� ��� ������� PIRIT_TABLE_SETTINGS_KKT
enum PIRIT_SETTINGS_KKT
{
    PIRIT_SET_KKT_PRINT_INTERV = 0x01, // ����� � 㬥��襭�� �������� ���ࢠ���
    PIRIT_SET_KKT_FULL_CUT     = 0x02, // ������ ��१�� 祪�
    PIRIT_SET_KKT_PRINT_LOGO   = 0x04, // ������ ����᪨� ����⨯
    PIRIT_SET_KKT_PRINT_QR     = 0x20, // ������ QR ��� �� 祪� (�ᢥ��� 1)
    PIRIT_SET_KKT_AUTO_SHIFT   = 0x20, // ��⮬���᪨ ��८��뢠�� ᬥ�� �� ����⨨ ���㬥��, �᫨ ᬥ�� ��⥪��
    PIRIT_SET_KKT_PRINT_DEPART = 0x40, // ������ �⤥�� �� 祪��
    PIRIT_SET_KKT_NOT_PRINT    = 0x80  // �� ������ ���㬥��� �� 祪���� ����
};

//-------------------------------------------------
//  ���� ४����⮢ ��� ������� PIRIT_PRINT_REQUISIT_OFD
enum PIRIT_REQUISIT_OFD
{
    PIRIT_REQ_OFD_SIGN_AGENT       = 0x0040,      // �ਧ��� ����� � 祪�

    PIRIT_REQ_OFD_TRANS_OPER_ADR   = 0x03ED,    // ���� ������ ��ॢ���
    PIRIT_REQ_OFD_TRANS_OPER_INN   = 0x03F8,    // ��� ������ ��ॢ���
    PIRIT_REQ_OFD_TRANS_OPER_NAME  = 0x0402,    // ������������ ������ ��ॢ���
    PIRIT_REQ_OFD_PAYING_AGENT_OP  = 0x0414,    // ������ ���⥦���� �����
    PIRIT_REQ_OFD_AGENT_SIGN       = 0x0421,    // �ਧ��� �����
    PIRIT_REQ_OFD_AGENT_TEL_NUM    = 0x0431,    // ����䮭 ���⥦���� �����
    PIRIT_REQ_OFD_BILLING_TEL_NUM  = 0x0432,    // ����䮭 ����. �� �ਥ�� ���⥦��
    PIRIT_REQ_OFD_TRANSFER_TEL_NUM = 0x0433,    // ����䮭 ������ ��ॢ���
    PIRIT_REQ_OFD_PROVIDER_TEL_NUM = 0x0493,    // ����䮭 ���⠢騪�
    PIRIT_REQ_OFD_PROVIDER_INN     = 0x04CA,    // ��� ���⠢騪�
    PIRIT_REQ_OFD_ADD_REC_REQ      = 0x04A8     // �������⥫�� ४����� 祪� (���)
};

//-------------------------------------------------
//  ��ࠬ���� ��� ������� PIRIT_GET_INFO_FROM_FN
enum PIRIT_INFO_FN
{
    PIRIT_INFO_FN_VERSION       = 0x0E,     // ������ ���ᨨ �� � ���
    PIRIT_INFO_FN_DATETIME      = 0xCA,     // ������ ���� � �६� �� ��
    PIRIT_INFO_FN_SET_DATETIME  = 0xCB      // ��⠭����� ���� � �६� ��
};

//-------------------------------------------------
//  ����� ��ࠬ��஢ PIRIT_INFO_FN
enum PIRIT_VERSION_INFO
{
    PIRIT_VERSION_FIRMWARE_FN   = 0x01,     // ����� ��訢�� ��
    PIRIT_VERSION_FN            = 0x02,     // ����� �� (0 - �⫠���� ��, 1 - �਩�� ��)
    PIRIT_VERSION_FFD           = 0x03,     // ����� ���
    PIRIT_VERSION_KKT           = 0x04,     // ����� ���
    PIRIT_VERSION_FFD_KKT       = 0x05,     // ����� ��� ���
    PIRIT_VERSION_REG_FFD_FN    = 0x06,     // ��ॣ����஢����� ����� ��� ��
    PIRIT_VERSION_MAX_FFD_FN    = 0x07      // ���ᨬ��쭠� ����� ��� ��
};

//-------------------------------------------------
// ����� ��ࠬ��஢ PIRIT_INFO_FN_DATETIME
enum PIRIT_DATETIME_INFO
{
    PIRIT_DATETIME_DATE         = 0x01,     // ��� �� ��
    PIRIT_DATETIME_TIME         = 0x02      // �६� �� ��
};

//-------------------------------------------------
// ����� ��ࠬ��஢ ��� ����祭�� �ࢨ᭮� ���ଠ樨
enum PIRIT_SERVICE_INFORMATION
{
    PIRIT_BATTERY_VOLTAGE       = 0x07,     // ����殮��� �� ���३��
    PIRIT_SKNO_DATETIME         = 0x64      // ��� � �६� �� ����
};

enum PIRIT_DREAMKAS_KEY_CMD
{
    PIRIT_DREAMKAS_KEY_SET          = 0x01, // ��⠭����� �ਬ���-���� � ����
    PIRIT_DREAMKAS_KEY_GET_INFO     = 0x02, // ������� ���ଠ�� � ��࠭��� ���⭮� ��
    PIRIT_DREAMKAS_KEY_PRINT_ALL    = 0x03, // ��ᯥ���� �� ������� ��
    PIRIT_DREAMKAS_KEY_GET_ALL      = 0x04  // ������� ���ଠ�� � ��� ������ ���
};

//========================================================================================
// ��騥 �������
//========================================================================================
void libSetIP      (std::string newIP);
void libSetPort    (int       newPort);
void libSetPasswd (Country newCountry);
void libSetErrors (Country newCountry);

int   commandStart          ();  //��砫� ࠡ���   ����������� ��� ������!
void  setDebugLevel(int level);  // �஢��� �⫠���

//========================================================================================
// �ਭ��
//========================================================================================
int   getPrinterStatus(int* result);               //����� ���ﭨ�  �����饣� ���ன�⢠ (��)
int   scrollPaper                ();               // �஬�⪠ �㬠�� �2
int   libCutDocument             ();               // �ਭ㤨⥫쭠� ��१�� ���㬥�� � �।������
int   libPrintServiceData        ();               // ��ᯥ���� �ࢨ�� �����
int   libPrintServiceDataSKNO_By ();               // ��ᯥ���� �ࢨ�� ����� ���� (⮫쪮 ��)

// ����� ���� � ⥪�饬 ���ﭨ� ���⮢
int   libCurrentStatusReport(       char* nameCashier);
int   libPrintXReport        (const char* nameCashier);        // ����� � �����
int   libPrintCopyLastZReport                       ();        // ����� ����� ��᫥����� Z-����
int   libPrintCopyReportFiscalization               ();        // ����� ����� 祪� �᪠����樨/���ॣ����樨

//����� ⥪�� ��� � ������묨 ���ਡ�⠬�
                int libPrintString(char               *textStr,    // ���⠥�� ⥪�� 0..72 ᨬ����
                                   FONTS                   fnt);   // ���ਡ��� �� enum FONTS
// ����� ����-����
int   libPrintBarCode             (unsigned char       posText,
                                   unsigned char  widthBarCode,
                                   unsigned char heightBarCode,
                                   unsigned char   typeBarCode,
                                   const    char      *barCode);
// ����� ��䨪� � ���㬥��
int   libPrintPictureInDocument   (int                   width,
                                   int                  height,
                                   int                      sm,
                                   unsigned char         *data);
// ����� ����㦥���� ࠭�� ���⨭��
int   libPrintPreloadedPicture    (int                      sm,
                                   int                  number);
// ��ᯥ���� �᪠��� ���� �� ᬥ���
int   libPrintFiscalReportByShifts(unsigned char     typeReport,
                                   int         startShiftNumber,
                                   int           endShiftNumber,
                                   const    char      *password);
// ��ᯥ���� �᪠��� ���� �� ��⠬
int   libPrintFiscalReportByDate  (unsigned char     typeReport,
                                   PLDate             startDate,
                                   PLDate               endDate,
                                   const char         *password,
                                   const char         *cashier  );
// ����� ४�����
int   libPrintRequisite           (unsigned char codeReq, unsigned char attributeText,
                                   const char* str1,           const char* str2 = nullptr,
                                   const char* str3 = nullptr, const char* str4 = nullptr);
// ����� ४����� ��� ���
int   libPrintRequsitOFD          (int                  codeReq,
                                   unsigned char  attributeText,
                                   const    char       *reqName,
                                   const    char        *reqStr);


//========================================================================================
// ����
//========================================================================================
int   libOpenShift          (const    char  *nameCashier);              // ������ ᬥ��
int   libOpenShift_By                                  ();              // ������ ᬥ�� ��� ��
/**
 * @brief The CloseDocParam struct �室�� ��ࠬ���� ������� ���㬥��
 */
struct CloseDocParam
{
    CloseDocParam() : cutPaper(0x00), flags(0) {}

    unsigned char cutPaper;                         // (����� �᫮) ���� ��१��
    std::string buyerAddress;                       // (��ப�) ���� ���㯠⥫�
    uint32_t flags;                                 // (��᫮) ����� 䫠��
    std::string clcPlace;                           // (��ப�) ���� ����⮢
    std::string recSendAddress;                     // (��ப�) ���� ��ࠢ�⥫� 祪��
    std::string automaticNumber;                    // (��ப�) ����� ��⮬��
    std::pair<std::string, std::string> addUserReq; // (��ப�) �������� �������⥫쭮�� ४����� ���짮��⥫�
                                                    // (��ப�) ���祭�� �������⥫쭮�� ४����� ���짮��⥫�
    std::string buyerName;                          // (��ப�)[0..128] ���㯠⥫�
    std::string buyerInn;                           // (��ப�)[0..12] ��� ���㯠⥫�
};
/**
 * @brief libCloseDocument ������� ���㬥��
 * �᫨ ����� ����� ����� �⢥��� ��ࠬ��஢
 * ����� ����� �⢥��� ��ࠬ��஢ �����頥��� ⮫쪮 ��� �᪠���� 祪�� ��室�, ������ ��室�, ��室� � ������ ��室�.
 * ��� ��⠫��� ���㬥�⮢ ���������� ⮫쪮 ᪢����� ����� ���㬥�� � ����樮��� ����稪.
 * @param[in] cutPaper 䫠� ��१��
 * @param[in] buyer_address ���� ���㯠⥫�
 * @param[in] internetSign ��⠭����� �ਧ��� ���⮢ � ���୥�
 * @return ������� �⢥��� ������
 */
CloseDocResult libCloseDocument(const CloseDocParam &closeDocParam);

/**
 * @brief libCloseDocumentLite
 * �᫨ ����� ����� �⢥��� ��ࠬ��஢ �� �����!!!
 * ����� ����� �⢥��� ��ࠬ��஢ �����頥��� ⮫쪮 ��� �᪠���� 祪�� ��室�, ������ ��室�, ��室� � ������ ��室�.
 * ��� ��⠫��� ���㬥�⮢ ���������� ⮫쪮 ᪢����� ����� ���㬥�� � ����樮��� ����稪.
 * @param[in] buyer_address 䫠� ��१��
 * @param[in] buyer_address ���� ���㯠⥫�
 * @param[in] internetSign ��⠭����� �ਧ��� ���⮢ � ���୥�
 * @return ������� �⢥��� ������
 */
CloseDocResult libCloseDocumentLite   (unsigned         char cutPaper,
                                       string           buyerAddress,
                                       unsigned char    internetSign);


// ������� �������� ��������� �� �������
MData libCloseDocument_By(unsigned       char   cutPaper);              //  ���� ��१��
//--------
int   libEmergencyCloseShift                           ();              // ���਩��� �����⨥ ᬥ��

int   libPrintZReport       (const    char   *nameCashier,
                             int                 options);              // ������� ᬥ�� + Z �����
int   libPrintZReport_By    (const    char   *nameCashier);             // ������� ᬥ�� + Z ����� ��� ��

int   libCancelDocument                                ();              // ���㫨஢��� ���㬥��
int   libCancelDocument_By(int, string, double (&arr)[PIRIT_PARAM_16]); // ���㫨஢��� ���㬥�� ��� ��
int   libPostponeDocument   (const char            *info);              // �⫮���� ���㬥��
/**
 * @brief libGetDocFromKL �⥭�� ���㬥�� �� ��
 * @param docNum ����� ���㬥��
 * @param doc ���㬥�� �� ��
 * @return ��� �訡��
 */
int libGetDocFromKL(const uint32_t &docNum, DocFromKL &doc);
//--------
// ������� �� ����⨥ ���㬥��(祪�) ���䮢��
int libOpenDocument       (DOC_TYPES                   type,   //  ⨯ ���뢠����� ���㬥��
                           DOC_FORMATS                  fmt,   //  ०�� �ନ஢���� ���㬥��
                           DOC_REQ_PRINT            doc_req,   //  ०�� ���� ४����⮢
                           int              numOfDepartment,   //  ����� �⤥��
                           string            nameOfOperator,   //  ��� ������
                           int                     numOfDoc,   //  ��易⥫�� �� ��� 䫠�� "�㬥� 祪�� � ��஭� ����. �ணࠬ��"
                           SNO_TYPES                    sno);  //  ���⥬� ���������������

// ������� �� ����⨥ ���㬥��(祪�) ���䮢�� ��� ��
int libOpenDocument_By    (DOC_TYPES_BY                type,   //  ⨯ ���뢠����� ���㬥��
                           DOC_FORMATS                  fmt,   //  ०�� �ନ஢���� ���㬥��
                           DOC_REQ_PRINT            doc_req,   //  ०�� ���� ४����⮢
                           int              numOfDepartment,   //  ����� �⤥��
                           string            nameOfOperator,   //  ��� ������
                           int                     numOfDoc);  //  ��易⥫�� �� ��� 䫠�� "�㬥� 祪�� � ��஭� ����. �ணࠬ��"

// ������ ���㬥�� 1
int   libOpenDocument       (unsigned char           type,
                             unsigned char      numDepart,
                             char*            nameCashier,
                             long               docNumber);
// ������ ���㬥�� 2
int   libOpenDocumentEx     (unsigned char           type,
                             unsigned char      numDepart,
                             char*            nameCashier,
                             long               docNumber,
                             unsigned char          taxN);
// ����� ������ �� 祪�
MData libGetReceiptData     (unsigned char    numRequest);
int   libSubTotal();                                                   // ����⮣

//  =========== ����樨 ============

/**
 * @brief libSetPosAddReq - ��⠭����� �������⥫�� ४������ ����樨
 * @param requisite -  ��� ⮢�୮� ������������
 * @return १���� �믮������
 */
int libSetPosAddReq(string requisite);

// ������ ������� "��⠭����� �������⥫�� ४������ ����樨"
int libSetPosAddReq( const AdditionalReq &ar );

// �������� ⮢���� ������
int   libAddPosition        (const char*        goodsName,
                             const char*          barcode,
                             double              quantity,
                             double                 price,
                             unsigned char      taxNumber,
                             int              numGoodsPos,
                             unsigned char      numDepart,
                             unsigned char       coefType,
                             const char         *coefName,
                             double             coefValue);
namespace PiritLib
{
    enum LIMIT
    {
        LIMIT_GOODS_NAME = 256,
        LIMIT_ARTICLE_BARCODE = 18,
        LIMIT_POSITION_NUMBER = 4,
        LIMIT_ORIGIN_COUNTRY_CODE = 3,
        LIMIT_CUSTOM_ENTRY_NUM = 32
    };
    /**
     * @brief The Position struct ������� ������ ����樨
     */
    struct Position
    {
        std::string goodsName;                    // (��ப�[0...224]) �������� ⮢��
        std::string barcode;                      // (��ப�[0..18]) ��⨪� ��� ���客�� ��� ⮢��/����� ���
        double quantity {.00};                    // (�஡��� �᫮) ������⢮ ⮢�� � ⮢�୮� ����樨
        double price{.00};                        // (�஡��� �᫮) ���� ⮢�� �� ������� ��⨪��
        unsigned char taxNumber {0x00};           // (����� �᫮) ����� �⠢�� ������
        int numGoodsPos {0};                      // (��ப�[0..4]) ����� ⮢�୮� ����樨
        unsigned char numDepart {0x00};           // (����� �᫮ 1..16) ����� ᥪ樨
        unsigned char coefType {0x00};            // 0x00
        std::string coefName;                     // 0x00
        double coefValue {.00};                   // 0
        int32_t signMethodCalculation {0x00};     // ����� �᫮) �ਧ��� ᯮᮡ� ����  (1 - �।����� 100%)
        int32_t signCalculationObject {0x00};     // ����� �᫮) �ਧ��� �।��� ���� (1 - ????)
        std::string originCountryCode;            // ��� ��࠭� �ந�宦����� ⮢��
        std::string customEntryNum;               // ॣ����樮��� ����� ⠬������� ������樨
        double exciseDuty {.00};                  // �㬬� ��樧�
        std::string addPosReq;                    // �������⥫�� ४����� �।��� �����
    };
    /**
     * @brief libAddPosition �������� ⮢���� ������
     * @param position ������
     * @return १���� �믮������
     */
    int libAddPosition(const Position &position);
}

// �������� ⮢���� ������ ��
int libAddPositionLarge_By   (const char      *goodsName,  // (��ப�[0...224])   �������� ⮢��
                              const char        *barcode,  // (��ப�[0..18])     ��⨪� ��� ���客�� ��� + A-�� GTIN, B-GTIN, C-��㣠
                              double            quantity,  // (�஡��� �᫮)     ������⢮ ⮢�� � ⮢�୮� ����樨
                              double               price,  // (�஡��� �᫮)     ���� ⮢�� �� ������� ��⨪��
                              unsigned char    taxNumber,  // (����� �᫮)       ����� �⠢�� ������
                              int            numGoodsPos,  // (��ப�[0..4])      ����� ⮢�୮� ����樨
                              unsigned char    numDepart); // (����� �᫮ 1..16) ����� ᥪ樨

//----------------------------------------------
//                     ��
//----------------------------------------------
// ���㧨�� ���㬥�� �� ������ �� ��
//----------------------------------------------
int libKLDocRead( long doc_num, std::vector<std::string> &strs );
//----------------------------------------------
// ��ᯥ���� ���㬥�� �� ������ �� ��
//----------------------------------------------
int libKLDocPrint( long doc_num );
//----------------------------------------------

// ��୨஢��� ⮢���� ������
int   libDelPosition        (const char          *goodsName,
                             const char            *barcode,
                             double                quantity,
                             double                   price,
                             unsigned char        taxNumber,
                             int                numGoodsPos,
                             unsigned char        numDepart);

int   libAddDiscount    (unsigned char typeDiscount, const char* nameDiscount, long sum); // �������� ᪨���
int   libAddMargin      (unsigned char typeMargin,   const char* nameMargin,   long sum); // �������� ��業��

int   libAddPayment     (unsigned char typePayment,     double sum, const char* infoStr); // �������� ⨯ ������
int   libAddPaymentD    (unsigned char typePayment,     double sum, const char* infoStr); // �������� ⨯ ������

int   libAddPayment_By  (unsigned char typePayment,     double sum, const char* infoStr); // �������� ⨯ ������ ���������

unsigned long libSetBuyerAddress(const char *buyerAddress);                   //��⠭����� ���� ���㯠⥫�
unsigned long libGetBuyerAddress(char *buyerAddress, unsigned long baLength); //������� ���� ���㯠⥫�

// �ନ��� 祪 ���४樨 � ������� � ��
int   libDoCheckCorrection(const char         *nameCashier,
                           double                     cash,
                           double                 cashless,
                           unsigned char   correctionFlags);
// �ନ��� 祪 ���४樨 � ������� � ��
int   libDoCheckCorrectionEx(const char       *nameCashier,
                             double                   cash,
                             double               cashless,
                             double                   sum1,
                             double                   sum2,
                             double                   sum3,
                             unsigned char correctionFlags,
                             const PLDate         *docDate,
                             const char         *docNumber,
                             const char    *correctionName,
                             double                 sum_18,
                             double                 sum_10,
                             double                  sum_0,
                             double                 sum_WT,
                             double             sum_18_118,
                             double             sum_10_110);
//������ ����� 祪�
int   libOpenCopyReceipt    (unsigned char            type,
                             unsigned char       numDepart,
                             const char       *nameCashier,
                             int                  numCheck,
                             int                   numCash,
                             PLDate                 mpDate,
                             PLTime                 mpTime);
//����� ���� ���㯠⥫�
void  libCleanBuyerAddress                              ();
//���ᥭ�� / ����⨥ �㬬�
int   libCashInOut          (const char           *infoStr,
                             double                   sum);
//��ॣ����஢��� �㬬� �� �⤥��
int   libRegisterSumToDepart(unsigned char  typeOperation,
                             unsigned char   numberDepart,
                             long                     sum);
//��ॣ����஢��� �㬬� ������
int   libRegisterTaxSum     (unsigned char      numberTax,
                             long                     sum);
//�ࠢ���� �㬬� �� 祪�
int   libCompareSum         (long                     sum);


//========================================================================================
// ��� � �६�
//========================================================================================
int   libSetPiritDateTime(PLDate  mpDate,
                          PLTime  mpTime);  //������ ����/�६��� ���

int   getPiritDateTime   (int     *cDate,
                          int     *cTime);  //�⥭�� ����/�६��� ��� 1

int   getCurMPTime       (PLDate *mpDate,
                          PLTime *mpTime);  // ������� ⥪ ������� ��� � �६�

//========================================================================================
// �訡�� ��⮪��� ����
//========================================================================================
MData libGetExErrorInfo    (unsigned char numRequest);       // ����� �������⥫쭮� ���ଠ樨 � �訡��� �2
/**
 * @brief libIsErrorCritical ��⮤ ��� ��।������, �뫠 �� �믮����� ������
 * @param errCode ��� ����祭��� �訡��
 * @param opType ⨯ ����樨, � 室� ���ன ����祭� �訡��
 * @param retMess ⥪�� �訡�� (�����頥� �� ��⮤�), ����易⥫쭮
 * @return १���� �믮������
 */
bool libIsErrorCritical(const int errCode, FB_OPERATION_CRITICAL_LEVEL opType, std::string& retMess);
//========================================================================================
// ���
//========================================================================================
int   command0x9A                                     ();   //��饭�� �᪠�쭮� �����
int   libTechnologicalReset(const PLDateTime  *dateTime);   // ! ��� ���㫥��� !
MData libReadSettingsTable (unsigned char        number,
                            int                   index);   // �⥭�� ⠡���� ����஥�

int getStrFromSettingsTable(string &dataTable,uint8_t number, int32_t index, uint16_t numParam = 0);   // �⥭�� ⠡���� ����஥� � �����⮬ ��ப�
int getIntFromSettingsTable(int &data, uint8_t number, int32_t index, uint16_t numParam = 0);          // �⥭�� ⠡���� ����஥� � �����⮬ �᫠ ��� ��⮢�� ��᪨

int getStrFromInfoFN(uint8_t number, int32_t index, string &dataFN); // �⥭�� ���ଠ樨 �� �� � �����⮬ ��ப�
int getIntFromInfoFN(uint8_t number, int32_t index, int &data);      // �⥭�� ���ଠ樨 �� �� � �����⮬ �᫠ ��� ��⮢�� ��᪨

int getStrServiceInfoSKNO(uint8_t number, uint32_t index, string &dataSKNO); // �⥭�� ���ଠ樨 �� ���� � �����⮬ ��ப�
int getIntServiceInfoSKNO(uint8_t number, uint32_t index, int &data);        // �⥭�� ���ଠ樨 �� ���� � �����⮬ �᫠ ��� ��⮢�� ��᪨

int libGetStatusOFD(int &result );  // �஢�ઠ ����㯭��� �ࢥ� ���

int   libWriteSettingsTable(unsigned char        number,
                            int                   index,
                            const char            *data);   // ������ ⠡���� ����஥�

int   libWriteSettingsTable(unsigned char        number,
                            int                   index,
                            uint8_t data);   // ������ ⠡���� ����஥�
/**
 * @brief The SetNVRcmd struct NVR-wrapper
 */
struct SetNVRcmd
{
    SetNVRcmd() = delete;
    SetNVRcmd(int _address, char *_data, uint16_t _size)
    {
        if ((_data != nullptr) && (_size <= MAX_SIZE))
        {
            address = _address;
            size = _size;
            data = new char[size];
            memcpy(data, _data, size);
        }
    }
    ~SetNVRcmd()
    {
        if (data != nullptr)
        {
            delete [] data;
        }
    }

    static const uint16_t MAX_SIZE = 128;       // ���ᨬ��쭠� ������ �����
    static const uint16_t ASCII_BYTE_LEN = 2;   // ������ � ����� ASCII-����
    static const int TYPE = 3;                  // ���
    int address = 0xf000;                       // ����
    uint16_t size = 0;                          // ࠧ��� ������
    char *data = nullptr;                       // �����
};
/**
 * @brief libSetNVR ������ NVR
 * @param setNVRcmd NVR-wrapper
 * @return १���� �믮������
 */
int libSetNVR(const SetNVRcmd &setNVRcmd);


int   saveLogoToFile       (char              *fileName);   // ������ � ����� ����⨯稪
int   libLoadLogo          (int                    size,
                            unsigned char         *data);   // ����㧨�� ����⨯ � ��
int   libDeleteLogo                                   ();   // ������� ����⨯

// ����� 䫠��� ����� ���
int   getStatusFlags       (int        *fatalStatus,
                            int *currentFlagsStatus,
                            int     *documentStatus);

// ����� ᢥ����� � ���, getKKTInfoNum(numParam = 1)
int   getKKTInfo(           unsigned char numRequest,
                            char              *data);
// ����� ????
int   getKKTInfoNum        (unsigned char numRequest,
                            int             numParam,
                            char             *param);

MData libGetServiceInfo    (unsigned char numRequest);       // ����� �ࢨ᭮� ���ଠ樨 �2

int libGetBatteryVoltage               (int &voltage);       // ������ ����殮��� �� ���३��

int   libLoadReceiptDesign( int                size,
                            unsigned char*     data);         // ����㧨�� ������ 祪�
// ����㧨��(��������?) ����᪮� ����ࠦ����
int   libLoadPicture      ( int               width,
                            int              height,
                            int                  sm,
                            const char*        name,
                            int              number,
                            unsigned char     *data);
// ����� �ࠩ���
long long getDriverVersion();

// �������� ᮮ�饭�� �� ���� �訡��
unsigned long libFormatMessage(int           errorCode,
                               char         *msgBuffer );

//========================================================================================
// ��. ������ ����஥� ���
//========================================================================================
// ������ ���祭�� � ⠡���� ����஥� ���
int libSetSettingsKSA_By(int   number,         // ����� ����
                         int    index,         // ������(��� ���ᨢ�)
                         char   *data);        // ���祭��

// �⥭�� ���祭�� �� ⠡���� ����஥� ���
int libGetSettingsKSA_By(int   number,          // ����� ����
                         int    index,          // ������(��� ���ᨢ�)
                         char   *data);         // ���祭��

//========================================================================================
// ��. ����� ���ﭨ�
//========================================================================================
// ��. ����� �������⥫쭮� ���ଠ樨 � �訡���
int libGetInfoErrorsExt_By( int *errs                                            );

// ��. ����� 䫠��� ����� ���
int libGetStatusFlags_By  ( int *statusFatal, int *statusCurrent, int *statusDoc, int *statusSKNO );

//========================================================================================
// ����稪�
//========================================================================================
// �����(� �����-� �� requestNumber) ᬥ���� ���稪�� � ॣ���஢
int   getCountersAndRegisters         (int        requestNumber,
                                       int                *data);

// ����� ᬥ���� ���稪�� � ॣ���஢
MData libGetCountersAndRegisters      (unsigned char numRequest);

// ����� ᬥ���� ���稪�� � ॣ���஢ ���  12 ⨯�
int   libGetCountersAndRegisters_12   (int               data[],
                                       int           maxElement,
                                       char                *str);

//����� ᬥ���� ���稪�� � ॣ���஢ ret errCode
int   libGetCountersAndRegisters_intXd(int         requestNumber,
                                       int                data[],
                                       int            maxElement,
                                       int                 type);


//========================================================================================
// ������� �騪
//========================================================================================
int   libSetToZeroCashInCashDrawer                  ();   // ���㫨�� ������ � ��������  騪�
int   libOpenCashDrawer            (int pulseDuration);   // ������ �������  騪
int   getCashDrawerStatus          (int *drawerStatus);   // ������� ����� ���������  騪� 1


//========================================================================================
// ������
//========================================================================================
int   libBeep(int duration);

//========================================================================================
// ��
//========================================================================================
// ������� ��
int   libCloseFN (const char *cashierName, uint32_t &fd, uint32_t &fpd, string &dateTime);

// ���਩��� �����⨥ �� (0xA4)
int libEmegencyCloseFN();

// 	����� ����� �� � ��饩 ����� ���-�������� ��᫥���� ॣ/���ॣ (��� ��᫥���饩 ���㧪�)
int libGetRegistrSTLVProps( int   &numberOfFD,    // ����� ��, ��� ���ண� �ॡ���� ������� ���
                            int     &lenTotal,    // ����� ����祭���� ��� �����
                            int     &docType );   // ��� ���㬥��

int libGetDocSTLV         ( const int   &numberOfFD,    // ����� ��, ��� ���ண� �ॡ���� ������� ���
                                  int     &lenTotal,    // ����� ����祭���� ��� �����
                                  int     &docType );   // ��� ���㬥��

int libGetRegSTLV         ( PIRIT_REG_DOC_TYPE &regType );      // ��� ���㬥�� 1- reg, 11 - rereg
int libGetLastRegData     ( uint32_t& fd, uint32_t& fpd );

// 	����� ���㬥�� �� ��娢� � �ଠ� TLV, �� �訡�� !! 48h !!, ����� ������� (78 - 15/20, libGetRegistrSTLVProps!!!!)
int libGetRegistrTLV( char       *tlv,                 // ���� ��� �����             (���������� ��饩 ����)
                      int     &lenTLV ,                // ����� ����祭���� ��� �����(���������� ��饩 ����)
                      const bool   isRegTLV = false);  // true - send command 78/20, false - 78/16

// 	����� ���㬥�� �� ��娢� �� ������
int libGetFnDocBin(  int        docNum,   // ����� ���㬥��
                     char       **data,   // ����  ���㬥��
                     int      &dataLen,   // ����� ���㬥��
                     bool     &isCheck,   // �� ���⠭��( ���⢥ত� � ��� )
                     int      &docType);  // ��� ���㦠����� ���㬥��
/**
 * @brief libSetFnDateTime - ��⠭���� ���� � �६��� � ��
 * @param dateTime - ��� � �६�
 * @return १���� �믮������ �������
 */
int libSetFnDateTime(const DateTime &dateTime);
//========================================================================================
// ������� ���ଠ�� �� ��᫥����� 祪�
//========================================================================================
int libGetLastRecieptData(  int                &type,   // ⨯ 祪� (��� ���㫨஢. = 0)
                            string       &cur_op_cnt,   // ⥪�騩 �����. ���稪
                            int             &rec_num,   // ����� 祪�
                            int             &doc_num,   // ����� ���㬥��
                            double              &sum,   // �㬬� 祪�
                            double         &discount,   // �㬬� ᪨���
                            string              &fpd,   // ��ப� �� (��. �ਧ���)
                            int                  &fd ); // ����� ��

int libGetLastRecieptData_By(  int                &type,   // ⨯ 祪� (��� ���㫨஢. = 0)
                               string       &cur_op_cnt,   // ⥪�騩 �����. ���稪
                               int             &rec_num,   // ����� 祪�
                               int             &doc_num,   // ����� ���㬥��
                               double              &sum,   // �㬬� 祪�
                               double         &discount,   // �㬬� ᪨��� �� 祪�
                               double          &markup);   // �㬬� ��業�� �� 祪�

//========================================================================================
// ����������� ���������������
//========================================================================================

//����� ��� ��� � �� (�㯥�ᥪ��� �������)
int   libAuthorization(PLDate mpDate, PLTime mpTime, const char *numKKT);

//��᪠������ /���ॣ������
int   libFiscalization(const char    *oldPassword,
                       const char      *regNumber,
                       const char            *INN,
                       const char    *newPassword);

//��᪠����� /���ॣ������
int   libRegistration(unsigned char          type,
                      const char       *regNumber,
                      const char             *INN,
                      int               systemTax,
                      int                     rej,
                      const char     *cashierName);

//��᪠����� /���ॣ������
int   libRegistrationEx(unsigned char         type,
                        const char      *regNumber,
                        const char            *INN,
                        int              systemTax,
                        int                    rej,
                        const char    *cashierName,
                        PLDateTime     regDateTime,
                        const char      *userName1,
                        const char      *userName2,
                        const char *addressSettle1,
                        const char *addressSettle2,
                        const char    *placeSettle,
                        const char     *autoNumber,
                        const char        *OFDName,
                        const char         *OFDINN,
                        const char    *senderEmail,
                        const char       *FNSEmail,
                        bool             automatic);


// 	������ ����� �� ��᫥���� ॣ����樨
int libGetRegFDNumber( int &fdNumber );

// 	������ ���� � �६� ॣ����樨
int libGetRegDateTime( char *regDateTime );


//======================================================================================
// ��ࠡ�⪠ �ᥫ
//======================================================================================
void  setAmountDecimalPlaces     (int decimalPlaces);
void  setQuantityDecimalPlaces   (int decimalPlaces);
void  setPercentageDecimalPlaces (int decimalPlaces);
int   getAmountDecimalPlaces                      ();
int   getQuantityDecimalPlaces                    ();
int   getPercentageDecimalPlaces                  ();




// =====================================
// ������� ������ ����� ��
int libGetFN_Number       (char                      *fnNum);  // ������� ����� �� �� ����
// ������� ������ ����� ��᫥����� �᪠�쭮�� ���㬥��
int libLastFiscDoc_Number (char            *fiscalDocNumber);  // ������ ����� ��᫥����� �᪠�쭮�� ���㬥��
// ������� ������ ���ﭨ� ⥪�饩 ᬥ��
int libGetCurShiftState   (int                    *shiftNum,   // ����� ᬥ��
                           int                    *isOpened,   // ����� �� ᬥ��
                           int           *numRecieptInShift);  // ����� 祪� � ᬥ��
// ������ ���ﭨ� ��
int libGetCurFNState(int     *fnState,                         // ����ﭨ� ��
                     int *curDocState,                         // ����ﭨ� ⥪�饣� ���㬥��
                     int   *warnFlags);                        // ����� �।�०�����

// ������ ���ﭨ� ��
int libGetStatusOFD( int            *ofdState,                     // ����� ������
                     int     *numOfDocsToSend,                     // ������⢮ ���㬥�⮢ ��� ��।��     � ���
                     int       *numFirstToSnd,                     // ����� ��ࢮ�� ���㬥�� ��� ��।��   � ���
                     char      *firstDateTime                      // ���/�६� ��ࢮ�� ���-� ��� ��।�� � ���
                    );

// =====================================



//======================================================================================
//============ ������ ��� ������ ������������ ���������� � �������� ������ =============
//======================================================================================

// ������� �� ����⨥ ���㬥�� ��� ���� ���᪠���� ������ � ����⭮� ०���
int libOpeNotFiscalDoc(const bool header = false);

// ������� �� �����⨥ ���㬥��
int libCloseNotFiscalDoc(uint8_t cutPaper); //  ���� ��१��


//����� ⥪�� � ������묨 ���ਡ�⠬� � ����⭮� ०���
int libPrintNotFiscalStr(char  *textStr,   // ���⠥�� ⥪�� 0..72 ᨬ����
                         FONTS     fnt);   // ���ਡ��� �� enum FONTS


//======================================================================================


//======================================================================================
//==========================  ������� �������-���� =====================================
//======================================================================================
/**
 * @brief libDreamkasKeySet - ������� ���� � punix
 * @param strKey - ���� �� �������
 * @return 0 ��� ��� �訡��
 */
int libDreamkasKeySet(string &strKey);
/**
 * @brief libDreamkasKeyGetInfo - ������� ���ଠ�� � ��࠭��� ��㣥
 * @param index - ����� ���㣨
 * @param keyInfo - ������� � �⢥⮬
 * @return 0 ��� ��� �訡��
 */
int libDreamkasKeyGetInfo(const uint8_t &index, DreamkasKey &keyInfo);
/**
 * @brief libDreamkasKeyPrintAll - ����� ��� ���
 * @return 0 ��� ��� �訡��
 */
int libDreamkasKeyPrintAll();
/**
 * @brief libDreamkasKeyGetInfo - ������� ���ଠ�� � ��� ��㣠�
 * @param keyInfoList - ����� ������� ���
 * @return 0 ��� ��� �訡��
 */
int libDreamkasKeyGetInfoList(vector<DreamkasKey> &keyInfoList);
/**
 * @brief libDreamkasKeyParseInfoList - ���� ���ଠ樨 �� ��㣥 � ��������
 * @param keyInfoList - ����� ������� ���
 * @param infoStr - ��ப� � ����묨 ��� ����
 * @return �����
 */
int libDreamkasKeyParseInfoList(vector<DreamkasKey> &keyInfoList, const string &infoStr);
/**
 * @brief libSplitString - ᯫ�� ��ப� �� ࠧ����⥫�
 * @param divisibleStr - ��室��� ��ப�
 * @param delimiter - ࠧ����⥫�
 * @return ����� ��ப
 */
vector<string> libSplitString(string divisibleStr, char delimiter);

/**
 * @brief libRestartKKT - ��२��� ���� ��� ��१���᪠ �����
 * @return 0 ��� ��� �訡��
 */
int libRestartKKT();

//======================================================================================


//======================================================================================
//======================================================================================
//======================================================================================
//===========================  ������ �� ��������  =====================================
//======================================================================================
//======================================================================================

//======================================================================================
// �� ����, �� ����
MData libGetStatusFlags  ();                                    //����� 䫠��� ����� ���
MData libGetKKTInfo (unsigned char numRequest);                 //����� ᢥ����� � ���
MData libGetPiritDateTime();                                    //�⥭�� ����/�६��� ��� 2
MData libGetCashDrawerStatus();                                 // ������� ����� ���������  騪� 2
MData libReadMemoryBlock(unsigned char type, long startAdress, long numBytes); //�⥭�� ����� ��� � ���
MData libGetPrinterStatus();                                    // ����� ���ﭨ� �����饣� ���ன�⢠ (��)


//======================================================================================
// ����, ���५�

int   libActivizationECT();
int   libCloseArchiveECT();
MData libGetInfoFromECT(unsigned char number, long dataL1, long dataL2);

int   libEnableServiceChannelToECT();                  // ����祭�� �ࢨ᭮�� ������ � ����
int   command0x9B();                                   // ��饭�� ����
int   libPrintControlTapeFromECT(int shiftNumber);     // ��ᯥ���� ����஫��� ����� �� ����
int   libPrintDocumentFromECT   (int   KPKNumber);     // ��ᯥ���� ���㬥�� �� ����
int   libGetInfoFromECT_NumberFP(char *data);          // ����� ���ଠ樨 �� ����
int   libGetInfoFromECT_FP(int numDoc, char *data);    // ����� ���ଠ樨 �� ����

int   libPrintReportFromECTByShifts(unsigned char typeReport,
                                    int     startShiftNumber,
                                    int       endShiftNumber); //��ᯥ���� ���� �� ᬥ��� �� ����

int   libPrintReportFromECTByDate(unsigned char typeReport,
                                  PLDate         startDate,
                                  PLDate           endDate);   // ��ᯥ���� ���� �� ��⠬ �� ����

int   libPrintReportActivizationECT();                         // ��ᯥ���� ���� �� ��⨢���樨 ����
int   libPrintReportFromECTByShift(int shiftNumber);           // ��ᯥ���� ���� �� ᬥ��� �� ����

int   libPrintDocsFromECTSDByNumberDoc(int startNumber, int endNumber);
int   libPrintDocsFromECTSDByNumberShift(int startNumber, int endNumber);
int   libPrintDocsFromECTSDByDate(PLDate mpDateStart, PLDate mpDateEnd);
int   libGetInfoFromECT_NumberDoc(int *numDoc);

int   libGetInfoFromECT_ShiftNumber(int *shiftNumber);
int   libBLRPrintControlTapeFromECT();
/**
 * @brief The KSA_INFORMATION enum
 *        ����� ᢥ����� � ���
 */
enum KSA_INFORMATION
{
    KSA_INF_PLANT_NUM = 1,          // �����᪮� ����� ���
    KSA_INF_HARDWARE_ID,            // �����䨪��� ��訢��
    KSA_INF_UNP,                    // ���
    KSA_INF_REG_NUM,                // ॣ����樮��� ����� ���
    KSA_INF_LAST_BEP_OP_DATE,       // ��� � �६� ��᫥����  ����樨 ���
    KSA_INF_REG_DATE,               // ��� ॣ����樨 / ���ॣ����樨
    KSA_INF_CASH_IN_DRAWER,         // �㬬� ������� � �������� �騪�
    KSA_INF_NEXT_DOC_NUM,           // ����� ᫥���饣� ���㬥��
    KSA_INF_SHIFT_REG_REREG_NUM,    // ����� ᬥ�� ॣ����樨/ ���ॣ����樨
    KSA_INF_NEXT_X_REP_NUM,         // ����� ᫥���饣� X ����
    KSA_INF_OPERATION_CNT,          // ⥪�騩 ����樮��� ���稪
    KSA_INF_TOTAL_CNT,              // ������騩 �⮣
    KSA_INF_SHIFTS_BEP_AVAILABLE,   // ������⢮ ᢮������ ᬥ� � ���
    KSA_INF_HARDWARE_TYPE = 15,     // ⨯ ��訢��
    KSA_INF_PAPER_DESIGN,           // ࠧ��� �㬠�� ⥪�饣� �������
    KSA_INF_SKNO_CUR_STATE = 30     // ⥪�饥 ���ﭨ� ����
};
/**
 * @brief INT_HASH_CODE - int hash code
 */
const size_t INT_HASH_CODE = typeid(int).hash_code();
/**
 * @brief valueParse - ���� ���祭�� ��ࠬ���
 * @param value      - ��뫪� �� ���祭��
 * @return
 */
template<typename T>
int valueParse( T &value )
{
    int err = -1;
    // RTTI
    if(typeid(value).hash_code() == INT_HASH_CODE)
    {
        err = pirit_io.parseAnswerN<T>( PIRIT_PARAM_1, value);
    }
    else
    {
         return err;
    }

    return err;
}
/**
 * @brief ksaInfGet - ����� ᢥ����� � ���
 * @param param     - ����� ��ࠬ���
 * @param value     - ��뫪� �� ���祭�� ��ࠬ���
 * @return
 */
template<typename T>
int ksaInfGet( KSA_INFORMATION param, T &value )
{
    int err = -1;

    pirit_io.makeFirstPartPacketToSend( PIRIT_KKT_INFO );
    pirit_io.addByte( static_cast<unsigned char>(param) );
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock();
    if( err != 0 )
    {
        printf("%s:: Error while connecting with Pilarus!", __PRETTY_FUNCTION__ );
        return err;
    }
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    else
    {
        printf("%s::sendData FAILED!!!", __PRETTY_FUNCTION__ );
    }

    pirit_io.disconnectSock();


    if(err == 0)
    {
        err = valueParse(value);
    }

    return err;
}

#endif







