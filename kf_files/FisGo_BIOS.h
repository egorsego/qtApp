//
// Created by krazh on 23.01.18.
//

#ifndef FISCAT_FISGO_BIOS_H
#define FISCAT_FISGO_BIOS_H

#include "libs/armeabi-v7a/qFisGoPirit/pirit_low_func.h"
#include "libs/armeabi-v7a/qFisGoPirit/PiritLib.h"

#include "convert.h"
#include "config.h"
#include <mutex>
#include <map>

#include "requisites.h"
#include "filesfg.h"
#include "license.h"
#pragma once

struct statusBIOS
{
    bool fatal_wrongSummNVR,        // 1-� ��� ����ୠ� ����஫쭠� �㬬� NVR
         fatal_wrongSummKonf,       // 2-� ��� ����ୠ� ����஫쭠� �㬬� � ���䨣��樨
    //   reserved,                  // 3-� ��� ��१�ࢨ஢���
    //   reserved;                  // 4-� ��� ��१�ࢨ஢���
    //   reserved;                  // 5-� ��� ��१�ࢨ஢���
         fatal_FnNotAuth,           // 6-� ��� ��� �� ���ਧ�����
         fatal_FnFatalErr,          // 7-� ��� ��⠫쭠� �訡�� ��
    //   reserved;                  // 8-� ��� ��१�ࢨ஢���
         fatal_errSD;               // 9-� ��� 	SD ���� ��������� ��� ����ࠢ��

    bool kkt_notInited,              //  1-� ���     �� �뫠 �맢��� �㭪�� ?��砫� ࠡ���?
         kkt_notFiscal,              //  2-� ��� 	 ���᪠��� ०��
         kkt_isShiftOpened,          //  3-� ��� 	 ����� �����
         kkt_isShift24h,             //  4-� ���     ����� ����� 24 �ᮢ
         kkt_fsClose,                //  5-� ���     ��娢 �� ������
         kkt_notRegistered,          //  6-� ���     �� �� ��ॣ����஢�� (postFiscal =   kkt_notFiscal (???) kkt_notRegistered)
    //   reserved,                   //  7-� ���     ��१�ࢨ஢���
    //   reserved,                   //  8-� ���     ��१�ࢨ஢���
         kkt_colseShiftNeed,         //  9-� ���     �� �뫮 �����襭� �����⨥ ᬥ��, ����室��� ������� ������
         kkt_controlLentaErr;        // 10-� ���     �訡�� ����஫쭮� �����

    int  documentType;               // ��� ⥪�饣� ����⮣� ���㬥��
    int  documentState;              // ����ﭨ� ���㬥��

    statusBIOS()
    {
        fatal_wrongSummNVR     = false;
        fatal_wrongSummKonf    = false;
        fatal_FnNotAuth        = false;
        fatal_FnFatalErr       = false;
        fatal_errSD            = false;

        kkt_notInited          = false;
        kkt_notFiscal          = false;
        kkt_isShiftOpened      = false;
        kkt_isShift24h         = false;
        kkt_fsClose            = false;
        kkt_notRegistered      = false;
        kkt_colseShiftNeed     = false;
        kkt_controlLentaErr    = false;

        documentType           = 0;
        documentState          = 0;
    }

};

struct registrationStruct
{
    unsigned char         type;    // 0x00 - ���. ��ࠬ��஢ ��� ������ ��, 0x01 - ॣ ��� ��� ���ॣ � ������� ��
    string           regNumber;    //  ��ப�[16] �������樮��� ����� ஢�� 16 ᨬ�����!
    string                 INN;    // ��ப�[12] ��� ���짮��⥫�
    int              systemTax;    //     ���⥬� ���������������:
                                   // 1 - ���, 2 - ���,
                                   // 4 - ����, 8 - ���� 16 - ������
                                   // !!!! ������-������ ���!
    int                    rej;    // bit   ����� ࠡ���:
                                   // 0  - ���஢����
                                   // 1  - ����� �裡 � ���
                                   // 2  - ��⮬���᪨� ०��
                                   // 3  - �ਬ������ � ��� ���
                                   // 4  - ���
                                   // 5  - �ਬ������ � ���୥�
                                   // 6  - �����樧�� ⮢���
                                   // 7  - ������ ����
                                   // 8  - �ਬ������ ������᪨�� ���⥦�묨 ����⠬�
                                   // 9  - �ਬ������ ������᪨�� ���⥦�묨 �㡠���⠬�
                                   // 10 - �ਬ������ ���⥦�묨 ����⠬�
                                   // 11 - �ਬ������ ���⥦�묨 �㡠���⠬�
                                   // 12 - �ਬ������ ����७�묨
                                   // 13 - �ਬ������ �����ᨮ��ࠬ�
                                   // 14 - �ਬ������ ����⠬�
                                   // 15 - �����
    string         cashierName;    // ��ப�[0..64]  ��� �����
    string           userName1;    // ��ப�[0..44]  ������������ ���짮��⥫� ��ப� 1
    string           userName2;    // ��ப�[0..44]  ������������ ���짮��⥫� ��ப� 2(����易⥫쭠�)
    string      addressSettle1;    // ��ப�[0..44]  ���� ���⮢(���짮��⥫�) ��ப� 1
    string      addressSettle2;    // ��ப�[0..44]  ���� ���⮢(���짮��⥫�) ��ப� 2 (����易⥫쭠�)
    string         placeSettle;    // ��ப�[0..112] ���� ����⮢
    string          autoNumber;    // ��ப�[0..12]  ����� ��⮬��
    string             OFDName;    // ��ப�[0..64]  ������������ ���
    string             OFD_INN;    // ��ப�[0..12]  ��� ���
    string         senderEMAIL;    // ��ப�[0..44]  ���� ���஭��� ����� ��ࠢ�⥫� 祪�.
    string            FNS_Site;    // ��ப�[0..64]  ���� ᠩ� ���.
    bool           isAutomatic;    // �ਧ��� ��⠭���� �ਭ�� � ��⮬��.

    registrationStruct()
    {
        systemTax      =                      0x00;
        type           =                      0x00;
        rej            =                      0x00;
        isAutomatic    =                     false;
    }
};


//--------------------------------------------------------------------------------------------------------------
// � FisFo_BIOS.cpp
//--------------------------------------------------------------------------------------------------------------
extern bool          fb_wasErr;         // ���� ��� BackgroundEvents, ������騩, �� �ந��諠 ���. �訡�� �� �믮������ ������� � ����
extern string        fb_errMess;
extern int           fb_errCode;
extern bool          fb_wasWarning;    // ���� ��� BackgroundEvents, ������騩, �� ������� �믮����� � ��୨����
extern string        fb_warnMess;
extern int           fb_warnCode;


bool isCommandComplete(int res, FB_OPERATION_CRITICAL_LEVEL op_lvl);

//=====================================================================================
//=====================================================================================
//===================== ����䥩�� ����� ��� ࠡ��� � ���� ==========================
//=====================================================================================
//=====================================================================================
class BIOS
{
private:
    string               ip;      // IP BIOS
    int                port;      // Port BIOS
    atomic_bool isConnected;      // ����稥 �裡 � ����
    statusBIOS   statusBios;      // ������� ���ﭨ� �����
    mutex        mutex_bios;      // ��騩 ���⥪� BIOS (RF � RB)
    std::string lastErrorExtMessage; // ��᫥���� ����襭��� ���७��� ᮮ�饭�� �� �訡��
public:
       BIOS();
       virtual ~BIOS();
       unique_ptr<FilesFG>      fg;
       unique_ptr<Convert>      convert;
       mutex initBiosMutex;           // mutex ��� 䫠�� ���樠����樨 BIOS
       condition_variable initBiosCv; // cv ��� 䫠�� ���樠����樨 BIOS
       #ifdef BIOS_RF
           const string FS_NAME = "��";
       #else
           const string FS_NAME = "����";
       #endif
       inline const std::string &getLastErrorExtMessage() const {return lastErrorExtMessage;}
       inline mutex* mutex_get()      { return &mutex_bios; }
       void   beep                                (int microSec);   // ����� (����� �᫮ 10...2000) ���⥫쭮��� � ���
       virtual bool initConnection();   // �������� ᮥ������� � ������
       bool   techZeroing     (string zavNum = "0497000001    ");   // ��� ���㫥���, ��� �� ��������
       bool   printXReport                         (string name);   // ����� X �����
       void   printLastFiscalReport                           ();   // ����� ��᫥����� ����� � ॣ/���ॣ
       bool   printFnDocs                      (int numberOfDoc);   // ��ᯥ���� ���㬥�� �� ��. 0=�� ॣ-�� � ���ॣ-��
       void   printBIOSTechReport                             ();   // ����� ��. ����� BIOS
       /**
        * @brief cancelDocBIOS �⬥�� ���㬥�� BIOS
        */
       void cancelDocBIOS();
       /**
        * @brief isDocCanceled ��� �� �⬥�� �ࠩ���� ���㬥��
        * @return १���� �믮������
        */
       bool isDocCanceled();
       /**
        * @brief cancelDoc �⬥�� ����⮣� ���㬥��
        * @return १���� �믮������
        */
       bool cancelDoc();
       /**
        * @brief setDateTimeToBIOS ��⠭����� ���� � �६� � ���� ��� �� ���(⥪����)
        * @return १���� �믮������
        */
       bool   setDateTimeToBIOS                               ();
       /**
        * @brief setDateTimeToBIOS ��⠭����� ���� � �६� � ���� (�ந����쭠�)
        * @param[in] unixTime ���
        * @return १���� �믮������
        */
       virtual bool   setDateTimeToBIOS(const time_t &unixTime);
       bool   getStatusBIOS                                   ();   // ������� �� 䫠�� ����ᮢ �� �����
       template<typename T>                                         // ����� ����稪� �� BIOS
       bool biosCountersGet( BiosCounters<T> *biosCounters = nullptr )
       {
           lock_guard<mutex> locker(mutex_bios);
           if(biosCounters == nullptr)
           {
               logERR( "%s nullptr!", __PRETTY_FUNCTION__ );
               return false;
           }
           return (biosCounters->read() == 0);
       }
    //--------------------------------------------------------------------------------------------------------------
    // ����稪�
    //--------------------------------------------------------------------------------------------------------------
    bool getCounters_CurShiftNum     ( uint64_t               &shiftNum );  // 0x01 ����� ⥪ ᬥ��
    bool getCounters_NextRecNum      ( uint64_t             &nextRecNum );  // 0x02 ������ ����� ᫥���饣� 祪�
    virtual bool getCounters_SaleSumByPay    ( OPERATION<double>          &ssbp );  // 0x03 ������ �㬬� �த�� �� ⨯�� ���⥦�
    bool getCounters_SaleCntByPay    ( OPERATION<uint64_t>        &scbp );  // 0x04 ������ ������⢮ ����� �� �த����
    bool getCounters_RetSaleSumByPay ( OPERATION<double>         &rssbp );  // 0x05 ������ �㬬� �����⮢ �� ⨯�� ���⥦�
    bool getCounters_RetSaleCntByPay ( OPERATION<uint64_t>       &rscbp );  // 0x06 ������ ������⢮ ����� �� �����⠬
    bool getCounters_RecCntsByType   ( REC_CNTS                &recCnts );  // 0x07 ������⢮ ��ଫ����� 祪��
    virtual bool getCounters_RecSumsByType   ( REC_SUMS                &recSums );  // 0x08 �㬬� �� ��ଫ���� 祪��
    bool getCounters_DiscountSums    ( DISCOUNT_SUMS          &discSums );  // 0x09 �㬬� �� ᪨����
    bool getCounters_TaxSumsSale     ( TAX_SUMS                &taxSums );  // 0x10 �㬬� �������  �� �த����
    bool getCounters_TaxSumsRets     ( TAX_SUMS                &taxSums );  // 0x11 �㬬� �������  �� �����⠬
    bool getCounters_XZ              ( X_Z_DATA                 &xzData );  // 0x12 ����� �� ��᫥����� X ����� ��� ����� � �����⨨ ᬥ��
    bool getCounters_depSaleSum      ( double         (&depSaleSum)[NUM_OF_SECTOINS] );  // 0x13 ������ �㬬� �� ᥪ��/�⤥��� �� �த����
    bool getCounters_depRetSum       ( double          (&depRetSum)[NUM_OF_SECTOINS] );  // 0x14 ������ �㬬� �� ᥪ��/�⤥��� �� �����⠬
    bool getCounters_RecCount        ( uint64_t            &purchaseCnt ,   // 0x15 ���-�� 祪�� ��室��
                                       uint64_t         &retPurchaseCnt );  // 0x15 ���-�� 祪�� �����⮢ ��室�
    bool getCounters_SaleSumByType   ( OPERATION<double>     &purchSums );  // 0x16 ������ �㬬� ���㯮� �� ⨯�� ���⥦�
    bool getCounters_RetSumByType    ( OPERATION<double>  &purchSumsRet );  // 0x17 ������ �㬬� ���㯮� �� ⨯�� ���⥦�
    bool getCounters_CorData         ( COR_DATA                &corData );  // 0x18 ������ ����� �� ���४��
    //--------------------------------------------------------------------------------------------------------------
    //                               ��騥 ��⮤� ��� RUS � BEL ��訢�� BIOS
    //--------------------------------------------------------------------------------------------------------------
    enum DISCOUNT_TYPE_BIOS : uint8_t
    {
        DISCOUNT_TYPE_PERCENT = 0,
        DISCOUNT_TYPE_ABSOLUTE = 1
    };

    enum BIOS_FN_WARNINGS  : uint8_t
    {
        BIT_IMMEDIATE_REPLACEMENT = 0x01,  // ��᪠ ��� ���ﭨ� "��筠� ������ ��"
        BIT_RESOURCES_EXHAUSTION  = 0x02,  // ��᪠ ��� ���ﭨ� "���௠��� ����� ��"
        BIT_MEMORY_OVERFLOW       = 0x04,  // ��᪠ ��� ���ﭨ� "��९������� ����� ��"
        BIT_CRITICAL_FN_ERROR     = 0x80   // ��᪠ ��� ���ﭨ� "����᪠� �訡�� ��"
    };
    /**
     * @brief The BIOS_ERR enum �訡�� BIOS
     */
    enum BIOS_ERR  : int
    {
        BIOS_ERR_FS = 2222                 // �訡�� ��
    };

    struct PAYMENT
    {
        PAYMENT();
        ~PAYMENT();
        unsigned char typePayment; // (����� �᫮ 0..15) ��� ⨯� ���⥦�
        double                sum; // (�஡��� �᫮) �㬬�, �ਭ��� �� ���㯠⥫� �� ������� ���⥦�
        string            infoStr; // (��ப�[0..40]) �������⥫�� ⥪��
    };
    virtual bool   addPaymentToCheck             ( PAYMENT *payment );   // �������� ������  � ������ ���㬥��
    int setPrinterMode(int mask);   // ��⠭����� ��ࠬ���� ���⭮�� ���ன�⢠ (��᪠ �� enum PIRIT_SETTINGS_KKT)
    int getPrinterMode(int &mask);  // ������� ��ࠬ���� ���⭮�� ���ன�⢠ (��᪠ �� enum PIRIT_SETTINGS_KKT)
    bool getLicenseOptionsTags(int &mask); // ������� ��権 ��業���
    void setPrinterExPrint();       // ���⠥� 祪� �� ��
    void setPrinterExNotPrint();    // �� ���⠥� 祪� �� ��
    /**
     * @brief getPurchaseMode �⥭�� ��ࠬ��஢ 祪�
     * @param mask ��ࠬ���� 祪� (��⮢�� ��᪠)
     * @return १���� �믮������
     */
    int getPurchaseMode(int &mask);
    /**
    * @brief printPosDiscount  ����� ᪨��� �� ������ � 祪�
     * @param discountSumRubl �㬬� ᪨��� � �㡫��
     * @param text ⥪�� ᪨���
    * @return  bool १���� ���������� � �� ᪨���
    */
    bool printDiscount(const double &discountSumRubl, const string &text);
    /**
     * @brief setPurchaseMode ������ ��ࠬ��஢ 祪�
     * @param mask ��ࠬ���� 祪� (��⮢�� ��᪠)
     * @return १���� �믮������
     */
    int setPurchaseMode(const uint8_t &mask);
    /**
     * @brief setAutoOpenShift ��⮮���⨥ / ��⮧����⨥ ᬥ��
     * @return १���� �믮������
     */
    int setAutoOpenShift();

    /**
     * @brief waitInitConnection - �������� ���樠����樨 BIOS
     */
    bool waitInitConnection();
    //--------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------
       //=================================================================================================================
       static string strFromErrCode             (int errorCode);   // ����祭�� ����஢�� ���� �訡�� ����.
       virtual bool getCashInDrawer            (uint64_t &cash);   // ������ �㬬� ������� � �������� �騪� (� ��������)
       bool getKKTPlantNum                   (string &plantNum);   // ������ �����᪮� ����� ���
       bool getKKTRegNum                       (string &regNum);   // ������ ॣ����樮��� ����� ���
       virtual bool getDateTimeLastFiscalDoc (string &dateTime);   // ������ ���� � �६� ��᫥���� �᪠�쭮� ����樨
       bool getDateLastRegistration              (string &date);   // ������ ���� ��᫥���� ॣ����樨/���ॣ����樨
       virtual bool getDateFNValidBefore         (string &date);   // ��� ����砭�� �६������ �����  ��
       bool getDateTimeOpenShift             (string &dateTime);   // ������ ���� � �६� ������ ᬥ��
       bool getSignAndAgents                 (uint16_t &status);   // ������ �ਧ���� � ����� ����⮢
       bool getTaxSystems                      (uint8_t &taxes);   // ������ ����㯭� ���
       bool getPrinterMachine                (uint16_t &status);   // ������ �ਧ��� ������ �ਭ��

       bool getOrganizationAddress         (string &orgAddress);   // ������ ���� �࣠����樨
       bool getOrganizationINN                    (string &inn);   // ������ ��� �࣠����樨
       bool getOrganizationName               (string &orgName);   // ������ ������������ �࣠����樨
       bool getCalculationPlace             (string &calcPlace);   // ������ ���� ���⮢

       bool getOFDStatus                          (int &status);   // ������ ���ﭨ� ����㯠 � ���
       bool getOFDName                        (string &ofdName);   // ������ ������������ ���
       bool getOFDPort                           (int &ofdPort);   // ������ ���� ���
       bool getOFDAddress                  (string &ofdAddress);   // ������ ���� ���
       bool getOFDINN                          (string &ofdINN);   // ������ ��� ���

       bool getRecSenderEmail(std::string& recSenderEmail);        // ������ ���� ��ࠢ�⥫� 祪��

       bool getFSValidityDate                  (uint16_t &date);   // ������ ���� ����砭�� ����� ��
       bool getAutomatNum                      (string &number);   // ������ ����� ��⮬��

       bool getFfdKktVer                   (uint8_t &ffdKktVer);   // ����� ��� ���
       bool getFsFfdVer                     (uint8_t &fsFfdVer);   // ��ॣ����஢����� ����� ��� ��
       bool getKktVer                          (string &kktVer);   // ����� ���

       bool getSettingsFromPirit             ( Config &config );   // ��������� ���䨣 �� PIRIT
       bool getSignSettingsFromPirit         ( Config &config );   // ��������� �ਧ���� � ����� ����⮢

       virtual bool getBatteryVoltage(int &voltage );    // ������ ����殮��� �� ���३��


       /**
        * @brief restartKKT - ��२��樠������ ���� ��� ��१���᪠
        * @return true - � ��砥 �ᯥ�, ���� false
        */
       bool restartKKT();
      //=================================================================================================================

    // ����� ���㬥�� �� ��娢�
    bool getDocFromFN (int  fdNumber,
                       char *hexData,
                       int    lenBuf);

    // ����䥩�� � ����:
    virtual void setConnected   (const bool &isConn)  {isConnected.store(isConn);}
    void setPort             (int newPort);
    void setPass      (Country newCountry);
    void setErrors    (Country newCountry);
    void setIP         (string newIP);
    bool isConnectedGet            ()         // ���� �� ᮥ������� � ����
    {
        return isConnected.load();
    }

    //======================================================================================================================
    //======================================================================================================================
    // ��������� ���������
    //======================================================================================================================
    // ��� ⥪�饣� ����⮣� ���㬥��
    static const int TYPE_DOC_DOC_IS_CLOSED         =  0,
                     TYPE_DOC_SERVICE_DOC           =  1,
                     TYPE_DOC_CHECK_SALE            =  2,
                     TYPE_DOC_CHECK_SALE_RET        =  3,
                     TYPE_DOC_INSERTION             =  4,
                     TYPE_DOC_RESERVED              =  5,
                     TYPE_DOC_CHECK_CONSUMPTION     =  6,
                     TYPE_DOC_CHECK_CONSUMPTION_RET =  7;
    // ����ﭨ� ���㬥��
    static const int STATE_DOC_DOC_IS_CLOSED        =  0,
                     STATE_DOC_DOC_IS_OPENED        =  1,
                     STATE_DOC_DOC_IS_PRE_RESULT_1  =  2,
                     STATE_DOC_DOC_IS_PRE_RESULT_2  =  3,
                     STATE_DOC_NEED_CLOSE           =  4,
                     STATE_DOC_CLOSE_ERROR          =  8;
    static bool      is_email_valid (const string& email);
    static bool      leap_valid     ( int, int, int    );
    static bool      day_month_valid( int, int         );

    static const u_int8_t PAY_TYPE_NONE          = -1;    // ��������� ����� ⨯� ���⥦�
    static const u_int8_t MAX_PAY_TYPES          = 16;    // ���ᨬ��쭮� �᫮ ⨯�� ���⥦��
    static const u_int8_t MAX_PAY_TYPE_NAME_LEN  = 18;    // ���ᨬ��쭠� ����� ⨯�� ���⥦��

    //======================================================================================================================
    //======================================================================================================================
    //======================================================================================================================
    //                                                  ����� ��
    //======================================================================================================================
        class RUS
        {
            public:
                RUS();
                ~RUS();
                bool            isConnected;
                unique_ptr<Convert> convert;

            private:
                mutex             *mutex_rf; // ���⥪� ��
            public:
                //--------------------------------------------------------------------------------------------------------------
                //                                          ����� ४����⮢
                //--------------------------------------------------------------------------------------------------------------
                Requisites      REQ;
                //--------------------------------------------------------------------------------------------------------------
                inline std::mutex* mutex_get(                  ){ return      mutex_rf; }
                inline void        mutex_set( mutex *mutex_ptr ){ mutex_rf = mutex_ptr; }
                /**
                 * @brief powerOnFS ����祭�� ��⠭�� �� (���⭨� �)
                 */
                static void powerOnFS();
                //--------------------------------------------------------------------------------------------------------------
                //                          ���� ������ ��
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char           FORMAT_CMD = 0x3A,
                                                   VALIDITY_CMD = 0x3B,
                                                   FREE_MEM_CMD = 0x3D,
                                                        RESTORE = 0x3E,
                                                 SHIFT_CNTS_GET = 0x36,
                                            OPERATIONS_CNTS_GET = 0x37,
                                                   CLC_CNTS_GET = 0x38,
                                           UNDELEGETED_CNTS_GET = 0x39,
                                                      RESET_CMD = 0x60,
                                                      REG_START = 0xA2,
                                                     REG_FINISH = 0xA3;
                //--------------------------------------------------------------------------------------------------------------
                //                           ���� �⢥� ��
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char         SUCCESS_ANSW = 0x00,       // ���筮� �믮������ �������
                                             WRONG_CMD_FMT_ANSW = 0x01,       // ������ �ଠ� �������
                                            WRONG_FS_STATE_ANSW = 0x02,       // ����୮� ���ﭨ� ��
                                                     WRONG_DATE = 0x07;       // ����୮� ��� � �६�
                /**
                 * @brief The FS_ERR enum - ���� �⢥� ��
                 */
                enum FS_ERR
                {
                    FS_ERR_MEM_FOR_OFD_OVERFLOW = 0x54 // ���௠�� ������ �࠭���� ���
                };
                //--------------------------------------------------------------------------------------------------------------
                //                          ���� ����稪�� ��
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char       CUR_SHIFT_CNTS = 0x00,       // ���稪� ⥪�饩 ᬥ��
                                                        FS_CNTS = 0x01;       // ���稪� ��
                //--------------------------------------------------------------------------------------------------------------
                //                           ���� ���㬥�⮢ ��
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char              REC_DOC = 0x00,       // ���ᮢ� 祪� (���)
                                                    REC_COR_DOC = 0x01;       // 祪� (���) ���४樨
                //--------------------------------------------------------------------------------------------------------------
                //                          ���� ����� ��
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char                 SALE = 0x01,       // ��室 (���४�� ��室�)
                                                    RETURN_SALE = 0x02,       // ������ ��室�
                                                       PURCHASE = 0x03,       // ��室 (���४�� ��室�)
                                                RETURN_PURCHASE = 0x04;       // ������ ��室�
                //--------------------------------------------------------------------------------------------------------------
                //                        ����� ࠡ��� �� � ���
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char           ENCRYPTION = 0x01,       // ���஢����
                                                     AUTONOMOUS = 0x02,       // ��⮭���� ०��
                                                      AUTOMATIC = 0x04,       // ��⮬���᪨� ०��
                                                       SERVICES = 0x08,       // �ਬ������ � ��� ���
                                                            BCO = 0x10,       // ����� ��� ���� ����� 祪��
                                                       INTERNET = 0x20;       // �ਬ������ � ���୥�
                //--------------------------------------------------------------------------------------------------------------
                //                   ��稭� ��������� ��ࠬ��஢ ॣ����樨
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char       FS_REPLACE_REG = 0x01,
                                                 OFD_CHANGE_REG = 0x02,
                                           USER_DATA_CHANGE_REG = 0x03,
                                             KKT_SET_CHANGE_REG = 0x04;
                //--------------------------------------------------------------------------------------------------------------
                //                    ����७�� �ਧ���� ࠡ��� ���
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char      EXCISABLE_GOODS = 0x01,       //�����樧�� ⮢���
                                                       GAMBLING = 0x02,       // ������ ����
                                                        LOTTERY = 0x04,       // �����
                                                PRINTER_MACHINE = 0x08;       // ��⠭���� �ਭ�� � ��� ०���
                //--------------------------------------------------------------------------------------------------------------
                //                                ����� ���
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char              FFD_1_0 = 0x01,       // ��� 1.0
                                                      FFD_1_0_5 = 0x02,       // ��� 1.0.5
                                                        FFD_1_1 = 0x03;       // ��� 1.1
                //--------------------------------------------------------------------------------------------------------------
                //                      ����饭�� ������ ���
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char            FLK_ERROR = 0x02,       // ������ ���
                                                   NEED_CFG_KKT = 0x40,       // ����. �����. ���
                                                      OFD_CLOSE = 0x80;       // ��� �����������
                //--------------------------------------------------------------------------------------------------------------
                //                                   ���� �訡�� ��
                //--------------------------------------------------------------------------------------------------------------
                static const int                        SUCCESS =    0,
                                                  CONNECT_ERROR =   -1,
                                                 WRONG_DATE_ERR =   -7,
                                                      SHIFT_24H =  -16,
                                                        TIMEOUT = -100;
                //--------------------------------------------------------------------------------------------------------------
                //                   ����७�� ��稭� ��������� ��ࠬ��஢ ॣ����樨
                //--------------------------------------------------------------------------------------------------------------
                static const uint32_t                FS_REPLACE = 0x00000001, // ������ ��
                                         //-------------------------------------------------------------------------------------
                                                     OFD_CHANGE = 0x00000002, // ����� ���
                                         //-------------------------------------------------------------------------------------
                                               USER_NAME_CHANGE = 0x00000004, // ��������� ������������ ���짮��⥫�
                                                                              // ����஫쭮-���ᮢ�� �孨��
                                         //-------------------------------------------------------------------------------------
                                         ADRESS_OR_PLACE_CHANGE = 0x00000008, // ��������� ���� � (���) ���� ��⠭����
                                                                              // (�ਬ������) ����஫쭮-���ᮢ�� �孨��
                                         //-------------------------------------------------------------------------------------
                                                 AUTONOMOUS_OFF = 0x00000010, // ��ॢ�� ��� �� ��⮭������ ०��� � ०��
                                                                              // ��।�� ������
                                         //-------------------------------------------------------------------------------------
                                                  AUTONOMOUS_ON = 0x00000020, // ��ॢ�� ��� �� ०��� ��।�� ������
                                                                              // � ��⮭���� ०��
                                         //-------------------------------------------------------------------------------------
                                           MODEL_VERSION_CHANGE = 0x00000040, // ��������� ���ᨨ ������ ���
                                         //-------------------------------------------------------------------------------------
                                             TAX_SYSTEMS_CHANGE = 0x00000080, // ��������� ����� ��⥬ ���������������,
                                                                              // �ਬ��塞�� �� �����⢫���� ���⮢
                                         //-------------------------------------------------------------------------------------
                                          AUTOMAT_NUMBER_CHANGE = 0x00000100, // ��������� ����� ��⮬���᪮�� ���ன�⢠
                                                                              // ��� ���⮢, � ��⠢� ���ண� �ਬ������ ���
                                         //-------------------------------------------------------------------------------------
                                                  AUTOMATIC_OFF = 0x00000200, // ��ॢ�� ��� �� ��⮬���᪮�� ०���
                                                                              // � ����⮬���᪨� ०�� (�����⢫���� ���⮢
                                                                              // ����஬)
                                         //-------------------------------------------------------------------------------------
                                                   AUTOMATIC_ON = 0x00000400, // ��ॢ�� ��� �� ����⮬���᪮�� ०���
                                                                              // (�����⢫���� ���⮢ ����஬)
                                                                              // � ��⮬���᪨� ०��
                                         //-------------------------------------------------------------------------------------
                                                         BSO_ON = 0x00000800, // ��ॢ�� ��� �� ०���, �� ��������饣�
                                                                              // �ନ஢��� ���, � ०��, ��������騩 �ନ஢���
                                                                              // ���
                                         //-------------------------------------------------------------------------------------
                                                        BSO_OFF = 0x00001000, // ��ॢ�� ��� �� ०���, ��������饣� �ନ஢���
                                                                              // ���, � ०��, �� ��������騩 �ନ஢��� ���
                                         //-------------------------------------------------------------------------------------
                                                   INTERNET_OFF = 0x00002000, // ��ॢ�� ��� �� ०��� ���⮢ � �� ���୥�
                                                                              // (��������饣� �� ������ ���ᮢ� 祪 � ���)
                                                                              // � ०��, ��������騩 ������ ���ᮢ� 祪 � ���
                                         //-------------------------------------------------------------------------------------
                                                    INTERNET_ON = 0x00004000, // ��ॢ�� ��� �� ०���, ��������饣� ������
                                                                              // ���ᮢ� 祪 � ���, � ०�� ���⮢ � ��
                                                                              // ���୥� (��������饣� �� ������ ���ᮢ� 祪 �
                                                                              // ���)
                                         //-------------------------------------------------------------------------------------
                                                 BANK_AGENT_OFF = 0x00008000, // ��ॢ�� ��� �� ०���, ��������饣� ����뢠��
                                                                              // ��㣨 ���⥦���� ����� (�㡠����) ���
                                                                              // ������᪮�� ���⥦���� �����, � ०��,
                                                                              // �� ��������騩 ����뢠�� ��㣨 ���⥦���� �����
                                                                              // (�㡠����) ��� ������᪮�� ���⥦���� �����
                                         //-------------------------------------------------------------------------------------
                                                  BANK_AGENT_ON = 0x00010000, // ��ॢ�� ��� �� ०���, �� ��������饣� ����뢠��
                                                                              // ��㣨 ���⥦���� ����� (�㡠����) ���
                                                                              // ������᪮�� ���⥦���� ����� � ०��,
                                                                              // ��������騩 ����뢠�� ��㣨 ���⥦���� �����
                                                                              // (�㡠����) ��� ������᪮�� ���⥦���� �����
                                         //-------------------------------------------------------------------------------------
                                                   GAMBLING_OFF = 0x00020000, // ��ॢ�� ��� �� ०���, ��������饣� �ਬ����� ���
                                                                              // �� �ਥ�� �⠢�� � �믫�� �������� �।��
                                                                              // � ���� �먣��� �� �����⢫���� ���⥫쭮��
                                                                              // �� �஢������ ������� ���, � ०��,
                                                                              // �� ��������騩 �ਬ����� ��� �� �ਥ�� �⠢��
                                                                              // � �믫�� �������� �।�� � ���� �먣���
                                                                              // �� �����⢫���� ���⥫쭮�� �� �஢������
                                                                              // ������� ���
                                         //-------------------------------------------------------------------------------------
                                                    GAMBLING_ON = 0x00040000, // ��ॢ�� ��� �� ०���, �� ��������饣� �ਬ�����
                                                                              // ��� �� �ਥ�� �⠢�� � �믫�� �������� �।��
                                                                              // � ���� �먣��� �� �����⢫���� ���⥫쭮��
                                                                              // �� �஢������ ������� ���, � ०��, ��������騩
                                                                              // �ਬ����� ��� �� �ਥ�� �⠢�� � �믫��
                                                                              // �������� �।�� � ���� �먣���
                                                                              // �� �����⢫���� ���⥫쭮�� �� �஢������
                                                                              // ������� ���
                                         //-------------------------------------------------------------------------------------
                                                    LOTTERY_OFF = 0x00080000, // ��ॢ�� ��� �� ०���, ��������饣� �ਬ����� ���
                                                                              // �� �ਥ�� �������� �।�� �� ॠ����樨
                                                                              // ���३��� ����⮢, ���஭��� ���३���
                                                                              // ����⮢, �ਥ�� ���३��� �⠢�� � �믫��
                                                                              // �������� �।�� � ���� �먣���
                                                                              // �� �����⢫���� ���⥫쭮�� �� �஢������
                                                                              // ���३, � ०��, �� ��������騩 �ਬ����� ���
                                                                              // �� �ਥ�� �������� �।�� �� ॠ����樨
                                                                              // ���३��� ����⮢, ���஭��� ���३���
                                                                              // ����⮢, �ਥ�� ���३��� �⠢�� � �믫��
                                                                              // �������� �।�� � ���� �먣���
                                                                              // �� �����⢫���� ���⥫쭮�� �� �஢������
                                                                              // ���३
                                         //-------------------------------------------------------------------------------------
                                                     LOTTERY_ON = 0x00100000, // ��ॢ�� ��� �� ०���, �� ��������饣� �ਬ�����
                                                                              // ��� �� �ਥ�� �������� �।�� �� ॠ����樨
                                                                              // ���३��� ����⮢, ���஭��� ���३���
                                                                              // ����⮢, �ਥ�� ���३��� �⠢�� � �믫��
                                                                              // �������� �।�� � ���� �먣���
                                                                              // �� �����⢫���� ���⥫쭮�� �� �஢������
                                                                              // ���३, � ०��, ��������騩 �ਬ����� ���
                                                                              // �� �ਥ�� �������� �।�� �� ॠ����樨
                                                                              // ���३��� ����⮢, ���஭��� ���३���
                                                                              // ����⮢, �ਥ�� ���३��� �⠢�� � �믫��
                                                                              // �������� �।�� � ���� �먣���
                                                                              // �� �����⢫���� ���⥫쭮�� �� �஢������
                                                                              // ���३
                                         //-------------------------------------------------------------------------------------
                                             FFD_VERSION_CHANGE = 0x00200000, // ��������� ���ᨨ ���
                                         //-------------------------------------------------------------------------------------
                                                 ANOTHER_REASON = 0x80000000; // ��� ��稭�
                //--------------------------------------------------------------------------------------------------------------
                //                                             ���
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char
                                                       FS_TOTAL = 0x01,       // ����
                                           FS_SIMPLIFIED_INCOME = 0x02,       // ���饭��
                                          FS_SIMPLIFIED_INC_EXP = 0x04,       // ���饭�� ��室-��室
                                       FS_SINGLE_IMPUTED_INCOME = 0x08,       // ���� (����� ����� �� ������ ��室)
                                        FS_UNIFIED_AGRICULTURAL = 0x10,       // ���� (��� ��� ����� ᥫ�᪮宧﨩�⢥��먩 �����)
                                                      FS_PATENT = 0x20;       // ��⥭��
                //--------------------------------------------------------------------------------------------------------------
                //                                   ���� ����� � ॣ����樨
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char       ACTIVATION_NEW = 0x00,
                                             ACTIVATION_REPLACE = 0x01,
                                                REG_DATA_CHANGE = 0x02;
                //--------------------------------------------------------------------------------------------------------------
                //                                        ������ ����� ��
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned             FS_NUMBER_LEN =   16;
                //--------------------------------------------------------------------------------------------------------------
                //                                       �������� ������ ��
                //--------------------------------------------------------------------------------------------------------------
                //--------------------------------------------------------------------------------------------------------------
                //                                         ����� �� ��
                //--------------------------------------------------------------------------------------------------------------
                struct FS_VERSION
                {
                    FS_VERSION();
                    ~FS_VERSION();
                     string version;
                     bool program_type;
                }fs_version;
                //--------------------------------------------------------------------------------------------------------------
                //                                           ��ଠ� ��
                //--------------------------------------------------------------------------------------------------------------
                struct FS_FORMAT
                {
                    FS_FORMAT();
                    ~FS_FORMAT();
                    unsigned char    supported,
                                          used;
                }fs_format;
                //--------------------------------------------------------------------------------------------------------------
                //                                         ������ ���祩 ��
                //--------------------------------------------------------------------------------------------------------------
                struct FS_VALIDITY
                {
                    FS_VALIDITY();
                    ~FS_VALIDITY();
                    unsigned char      date[5];
                    uint16_t         time_left;
                }fs_validity;
                //--------------------------------------------------------------------------------------------------------------
                //                                         ������ ����� ��
                //--------------------------------------------------------------------------------------------------------------
                struct FS_FREE_MEM
                {
                    FS_FREE_MEM();
                    ~FS_FREE_MEM();
                    uint32_t  data_resource_5Y,
                             data_resource_10Y;
                }fs_free_mem;
                //--------------------------------------------------------------------------------------------------------------
                //                                         �⮣ �᪠����樨 ��� 1.05
                //--------------------------------------------------------------------------------------------------------------
                struct FISCAL_RESULT
                {
                        string               date;
                        string                inn;
                        string             rn_kkt;
                        unsigned char  tax_system;
                        unsigned char    kkt_mode;
                        unsigned char re_reg_code;
                        uint32_t               fd;
                        uint32_t              fpd;
                } fiscal_result;
                //--------------------------------------------------------------------------------------------------------------
                //                                         �⮣ �᪠����樨 ��� 1.1
                //--------------------------------------------------------------------------------------------------------------
                struct FISCAL_RESULT_1_1
                {
                    string                date;
                    string                 inn;
                    string              rn_kkt;
                    unsigned char   tax_system;
                    unsigned char     kkt_mode;
                    unsigned char add_kkt_mode;
                    string             ofd_inn;
                    uint32_t       re_reg_code;
                    uint32_t                fd;
                    uint32_t               fpd;
                } fiscal_result_1_1;
                //--------------------------------------------------------------------------------------------------------------
                //                                         ����� � ���ﭨ� ����⮢
                //--------------------------------------------------------------------------------------------------------------
                struct QUITTANCE
                {
                        string               date;
                        unsigned char fpd_ofd[18];
                        uint32_t           fd_num;
                }quittance;
                //--------------------------------------------------------------------------------------------------------------
                static const int     D05 =  7200,
                                     D20 = 28800,
                                     D30 = 43200;
                //--------------------------------------------------------------------------------------------------------------
                enum FS_TYPE
                {
                    FS_SERIAL = 1,
                    FS_DEBUG
                };
                /**
                 * @brief The FS_STAGE enum ����� ��
                 */
                enum class FS_STAGE : uint8_t
                {
                    UNKNOWN = 0x00,
                    READY_TO_FISCAL = 0x01, // ��⮢ � �᪠����樨
                    FISCAL = 0x02,          // �᪠��� ०��
                    POST_FISCAL = 0x04      // �����᪠��� ०��
                };
                /**
                 * @brief The KKT_MODE enum ०�� ࠡ��� ���
                 */
                enum KKT_MODE : uint8_t
                {
                    KKT_MODE_NOT_AUTONOMOUS = 0x00,  // ����⮭����
                    KKT_MODE_AUTONOMOUS = 0x01       // ��⮭����
                };

                //--------------------------------------------------------------------------------------------------------------
            private:
                bool                                      IS_FLK_ERROR,
                                                       IS_NEED_CFG_KKT,
                                                          IS_OFD_CLOSE;
                //--------------------------------------------------------------------------------------------------------------
                //                                           ��� 䫠�� � ����� ��
                //--------------------------------------------------------------------------------------------------------------
                string                                       fs_number; // ����� ��                                                 ��
                uint32_t                                   last_fd_num; // ��᫥���� ��                                             ��
                unsigned long                                fd_number; // ��
                unsigned long                               fpd_number; // ���
                int                                          shift_num; // ����� ᬥ��                                              ��
                int                                        reciept_num; // ����� ⥪�饣� 祪�                                      ��
                bool                                       isShiftOpen; // ����� �����                                            ��
                bool                                   isReadyToFiscal; // �� ����஥� (��⮢ � �᪠����樨)                      ��
                bool                                      isFiscalMode; // ��᪠��� ०��                                        ��
                bool                                  isPostFiscalMode; // �����᪠��� ०��                                    ��
                int                                      ofdStage = -1; // ����� ���
                int                                 undelegatedDocsCnt; // ����।��� �� � ���      *******
                string                              undelegatedDocDate; // ��� ��ࢮ�� ����।������ � ��� ��      *******
                int                             firstUndelegatedDocNum; // ����� ��ࢮ�� ����।������ �� � ���      *******
                bool                             IMMEDIATE_REPLACEMENT; // �ਧ��� ����室����� ��筮� ������ �� (1051)      *******
                bool                              RESOURCES_EXHAUSTION; // �ਧ��� ���௠��� ����� �� (1050)      *******
                bool                                   MEMORY_OVERFLOW; // �ਧ��� ��९������� ����� �� (1052)      *******
                bool                                 CRITICAL_FN_ERROR; // 	����᪠� �訡�� ��                     *******
                bool                                   fs_replace_mode; // ����� ������ ��
                bool                                   reg_data_change; // ���ॣ������ (����� ������ ��. ���)
                bool                                   ofd_data_change; // ���ॣ������ (����� ���)
                bool                                fs_change_kkt_mode; // ���ॣ������ (����� ���)
                bool                               fs_offline_kkt_mode; // ���ॣ������ (��।�� � ���)
                bool                                      is_shift_24h; // ����� 24 �                                                ��
                bool                                          fs_timer; // ������ ��
                bool                                         isDocOpen; // ����� ���㬥�� � ��                                     ��
                bool                                OFD_DISCONNECT_05D; //      *******
                bool                                OFD_DISCONNECT_20D; //      *******
                bool                                OFD_DISCONNECT_30D; //      *******
                bool                                   REG_UNAVAILABLE;
                atomic_bool memoryToOfdExhausted;   // ���௠��� ����� �࠭���� ������ ��� ���
                unsigned                             reg_docs_complite;
                //--------------------------------------------------------------------------------------------------------------
                //                                     ��⮤� ����㯠 � 䫠���
                //--------------------------------------------------------------------------------------------------------------
            public:
                inline bool                      is_connect_get() { return            isConnected; }
                inline void            is_connect_set( bool val ) {             isConnected = val; }
                inline bool                    is_flk_error_get() { return           IS_FLK_ERROR; }
                inline bool                 is_need_cfg_kkt_get() { return        IS_NEED_CFG_KKT; }
                inline bool                    is_ofd_close_get() { return           IS_OFD_CLOSE; }
                inline FS_VERSION*               fs_version_get() { return            &fs_version; }
                inline FS_FORMAT*                 fs_format_get() { return             &fs_format; }
                inline FS_VALIDITY*             fs_validity_get() { return           &fs_validity; }
                inline FS_FREE_MEM*             fs_free_mem_get() { return           &fs_free_mem; }
                virtual string                    fs_number_get() { return              fs_number; }
                inline uint32_t                 last_fd_num_get() { return            last_fd_num; }
                virtual unsigned long             fd_number_get() { return              fd_number; }
                virtual unsigned long            fpd_number_get() { return             fpd_number; }
                virtual uint32_t                  shift_num_get() { return static_cast<uint32_t>(shift_num); }
                virtual unsigned short          reciept_num_get() { return            reciept_num; }
                virtual bool                    isShiftOpen_get() { return            isShiftOpen; }
                inline void           isShiftOpen_set( bool val ) {             isShiftOpen = val; }
                virtual bool                isReadyToFiscal_get() { return        isReadyToFiscal; }
                inline void       isReadyToFiscal_set( bool val ) {         isReadyToFiscal = val; }
                virtual bool                   isFiscalMode_get() { return           isFiscalMode; }
                       void          isFiscalMode_set( bool val ) {            isFiscalMode = val; }
                virtual bool               isPostFiscalMode_get() { return       isPostFiscalMode; }
                inline void      isPostFiscalMode_set( bool val ) {        isPostFiscalMode = val; }
                /**
                 * @brief getOfdStage ������� ����� ᮥ������� � ���
                 * @return ����� ᮥ������� � ���
                 */
                int getOfdStage() {return ofdStage;}
                /**
                 * @brief getUndelegatedDocsCnt ������� ������⢮ ����।����� ���㬥�⮢ � ���
                 * @return ������⢮ ����।����� ���㬥�⮢ � ���
                 */
                virtual uint32_t getUndelegatedDocsCnt() {return undelegatedDocsCnt;}
                /**
                 * @brief getUndelegatedDocDate ������� ���� ��ࢮ�� ����।������ ���㬥�� � ���
                 * @return ��� ��ࢮ�� ����।������ ���㬥�� � ���
                 */
                inline string getUndelegatedDocDate() {return undelegatedDocDate;}

//                /**
//                * @brief addAgentToCheck �������� ����� �� 祪
//                * @param agentType ⨯ �����
//                * @param agents ����� ����⮢
//                * @return १���� �믮������
//                */
//                int addAgentToCheck( const Agent &agent) const;        // �������� ����⮢ �� 祪

                /**
                * @brief printQRcode - print ascii data like qr code
                * @param data string for conversion
                * @return ok|fail
                */
                bool printQRcode(const std::string& data) const;
                /**
                 * @brief getFirstUndelegatedDocNum ������� ����� ��ࢮ�� ����।������ ���㬥�� � ���
                 * @return ����� ��ࢮ�� ����।������ ���㬥�� � ���
                 */
                inline uint32_t getFirstUndelegatedDocNum() {return firstUndelegatedDocNum;}
                inline virtual bool getImmediateReplacement() { return  IMMEDIATE_REPLACEMENT; }
                bool                                                                   imm_rep_en;
                inline virtual bool getResourcesExhastion() { return   RESOURCES_EXHAUSTION; }
                bool                                                                   res_exh_en;
                inline virtual bool getMemOverflow() { return        MEMORY_OVERFLOW; }
                bool                                                                  mem_over_en;
                inline virtual bool getCriticalFNErr() { return      CRITICAL_FN_ERROR; }
                bool                                                               crit_fn_err_en;
                inline bool                 fs_replace_mode_get() { return        fs_replace_mode; }
                inline void       fs_replace_mode_set( bool val ) {         fs_replace_mode = val; }
                inline bool                 reg_data_change_get() { return        reg_data_change; }
                inline void       reg_data_change_set( bool val ) {         reg_data_change = val; }
                inline bool                 ofd_data_change_get() { return        ofd_data_change; }
                inline void        ofd_data_change_set( bool val) {         ofd_data_change = val; }
                inline bool              fs_change_kkt_mode_get() { return     fs_change_kkt_mode; }
                inline void     fs_change_kkt_mode_set( bool val) {      fs_change_kkt_mode = val; }
                inline bool             fs_offline_kkt_mode_get() { return    fs_offline_kkt_mode; }
                inline void    fs_offline_kkt_mode_set( bool val) {     fs_offline_kkt_mode = val; }
                inline bool                    is_shift_24h_get() { return           is_shift_24h; }
                inline void          is_shift_24h_set( bool val ) {            is_shift_24h = val; }

                inline bool                        fs_timer_get() { return               fs_timer; }
                inline bool                       isDocOpen_get() { return              isDocOpen; }
                inline void             isDocOpen_set( bool val ) {               isDocOpen = val; }
                bool                                                       OFD_DISCONNECT_TEST_EN;
                inline bool              ofd_disconnect_05d_get() { return     OFD_DISCONNECT_05D; }
                inline void    ofd_disconnect_05d_set( bool val ) {      OFD_DISCONNECT_05D = val; }
                inline bool              ofd_disconnect_20d_get() { return     OFD_DISCONNECT_20D; }
                inline void    ofd_disconnect_20d_set( bool val ) {      OFD_DISCONNECT_20D = val; }
                inline virtual bool      getOFDdisconnect30days() { return     OFD_DISCONNECT_30D; }
                inline void    ofd_disconnect_30d_set( bool val ) {      OFD_DISCONNECT_30D = val; }
                inline bool                 reg_unavailable_get() { return        REG_UNAVAILABLE; }
                inline unsigned           reg_docs_complite_get() { return      reg_docs_complite; }
                inline FISCAL_RESULT*         fiscal_result_get() { return         &fiscal_result; }
                inline FISCAL_RESULT_1_1* fiscal_result_1_1_get() { return     &fiscal_result_1_1; }
                inline QUITTANCE*                 quittance_get() { return             &quittance; }
                //--------------------------------------------------------------------------------------------------------------
                //                                          ��㦥��� ��⮤�
                //--------------------------------------------------------------------------------------------------------------
                //**************************************************************************************************************
                //                                       ����� ���ଠ樥� � ��
                //**************************************************************************************************************
                enum FS_INFO
                {
                    IS_CONNECTED               = 0x00000001,
                    FS_NUMBER                  = 0x00000002,
                    IS_READY_TO_FISCAL         = 0x00000004,
                    IS_FISCAL_MODE             = 0x00000008,
                    IS_POST_FISCAL_MODE        = 0x00000010,
                    IS_DOC_OPEN                = 0x00000020,
                    LAST_FD_NUMBER             = 0x00000040,
                    IS_SHIFT_OPEN              = 0x00000080,
                    SHIFT_NUM                  = 0x00000100,
                    IS_SHIFT_24H               = 0x00000200,
                    RECEIPT_NUM                = 0x00000400,
                    IMMEDIATE_REPLACEMENT_FLG  = 0x00000800,
                    RESOURCES_EXHAUSTION_FLG   = 0x00001000,
                    MEMORY_OVERFLOW_FLG        = 0x00002000,
                    UNDELEGATED_DOCS_CNT       = 0x00004000,
                    FIRST_UNDELEGATED_DOC_NUM  = 0x00008000,
                    FIRST_UNDELEGATED_DOC_DATE = 0x00010000,
                    OFD_DISCONNECT_05D_FLG     = 0x00020000,
                    OFD_DISCONNECT_20D_FLG     = 0x00040000,
                    OFD_DISCONNECT_30D_FLG     = 0x00080000,
                    ALL_FLGS                   = 0xFFFFFFFF
                };
                enum REQUISITE_MAX_SIZE
                {
                    CASHIER_NAME_MAX_SIZE  =  64,
                    USER_NAME_MAX_SIZE     =  42,
                    ADDR_MAX_SIZE          =  42,
                    PLACE_MAX_SIZE         = 112,
                    AUTOMAT_NUM_MAX_SIZE   =  12,
                    OFD_NAME_MAX_SIZE      =  64,
                    SENDER_EMAIL_MAX_SIZE  =  42,
                    FNS_SITE_MAX_SIZE      =  64,
                    KKT_PLANT_NUM_MAX_SIZE =  14,
                    OFD_INN_MAX_SIZE       =  12
                };
                //**************************************************************************************************************
                void info( FS_INFO );
                //************************************************
                virtual bool fs_number_ask                             ();   // 0x01
                                                                     // ������ ॣ����樮��� ����� ��
                                                                     //     string           fs_number      �������樮��� ����� ��
                //************************************************
                virtual bool fs_status_get                     ();   // 0x02
                                                                     // ������ ����� ��
                                                                     // ���� ����� ��
                                                                     //     bool       isReadyToFiscal      ��⮢����� � �᪠����樨
                                                                     //     bool          isFiscalMode      ��᪠��� ०��
                                                                     //     bool      isPostFiscalMode      ����䨪ᠫ�� ०��. ���� ��।�� �� � ���
                                                                     // ����ﭨ� ᬥ��
                                                                     //     bool           isShiftOpen      ����ﭨ� ᬥ��
                                                                     // ����� �।�०�����
                                                                     //     bool IMMEDIATE_REPLACEMENT      ��筠� ������ ��
                                                                     //     bool  RESOURCES_EXHAUSTION      ���௠��� ����� ��
                                                                     //     bool       MEMORY_OVERFLOW      ��९������� ����� ��
                //************************************************
                virtual bool last_fd_num_ask();                      // 0x03
                                                                     // ������ ����� ��᫥����� �᪠�쭮�� ���㬥��
                                                                     //      uint32_t       last_fd_num ����� ��᫥����� ��
                //************************************************
                virtual bool getFSshiftParam                        ();   // 0x06
                                                                     // ������ ���ﭨ� ⥪�饩 ᬥ��
                                                                     //     bool           isShiftOpen      ����ﭨ� ᬥ��
                                                                     //     unsigned short   shift_num      ����� ᬥ��
                                                                     //     unsigned short reciept_num      ����� 祪� � ᬥ��
                /**
                 * @brief The OFD_STATUS struct ����� ���
                 */
                struct OFD_STATUS
                {
                    int stageOFD = -1;          // ����ﭨ� ���
                    int countUndelegated = -1;  // ���㬥�⮢ �� ��।���
                    int firstUndelegated = -1;  // ����� ��ࢮ�� �� ��।������ ���㬥��
                    string date;                // ��� ��ࢮ�� �� ��।������ ���㬥�� (DDMMYYHHMM)
                };
                /**
                 * @brief getStatusOfd ����� ����� ���
                 * @param ofdStatus ������ ���
                 * @return १���� �믮������
                 */
                bool getStatusOfd(OFD_STATUS &ofdStatus);
                //************************************************
                /**
                 * @brief getOfdDisconnectStatus ������ ���ﭨ� ������ � ���
                 * uint32_t undelegatedDocsCnt ������⢮ ���㬥�⮢ ��� ��।�� � ���
                 * uint32_t firstUndelegatedDocNum     ����� ��ࢮ�� ���㬥�� ��� ��।�� � ���
                 * string undelegatedDocDate ���/�६� ��ࢮ�� ���-� ��� ��।�� � ��� (DDMMYYHHMM)
                 * @return १���� �믮������
                 */
                virtual bool getOfdDisconnectStatus();
                /**
                 * @brief getFNDateTime ������ ���� � �६� �� ��
                 * @param tmt �६� �� 誠�� unix
                 * @return true � ��砥 �ᯥ�, ���� false � tmt ��⠭���������� � -1
                 */
                virtual bool getFNDateTime    (Convert::DATE_ISO_8601 &dateIso);
                /**
                 * @brief setFNDateTime - ��⠭����� ���� � �६� ��
                 * @param date - ��� � �६�
                 * @return १���� �믮������ �������
                 */
                virtual bool setFNDateTime(const time_t &date);

                //**************************************************************************************************************
                struct OPEN_DOC
                {
                     OPEN_DOC();
                     ~OPEN_DOC();
                     string       cashier;
                     DOC_TYPES       type;      //  ⨯ ���뢠����� ���㬥��
                     DOC_FORMATS      fmt;      //  ०�� �ନ஢���� ���㬥��
                     DOC_REQ_PRINT    req;      //  ०�� ���� ४����⮢
                     int            otdel;      //  ����� �⤥��
                     SNO_TYPES        sno;      //  ���⥬� ���������������
                };
                /**
                 * @brief The Position struct ����� ����樨
                 */
                struct Position
                {
                    Position();
                    ~Position() {}

                    PiritLib::Position punix;                       // ������ � punix
                    std::string discountRubName;                    // �������� ᪨���
                    double discountRubSum {.00};                    // ������ � �㡫��
                    /*Positions::DISCOUNT_TYPE_BIOS*/int discountType;     // ��� ᪨���

                    std::string marginRubName;                      // �������� ��������
                    double marginRubSum {.00};                      // �㬬� �������� � �㡫��
                    std::string  productCode;                       // ��� ⮢�୮� ������������

//                    Agent agent;                                    // ����� ��� ����樨
                };

                //--------------------------------------------------------------------------------------------------------------
                //                                                ���
                //--------------------------------------------------------------------------------------------------------------
                bool registerKKT_BIOS                       ( registrationStruct &rs );   // ��������� / ���ॣ������
                bool setOFDSettings     ( string  urlOFD,         uint32_t  portOFD  );   // ������ ����ன�� ���
                bool getOFDSettings      ( string &urlOFD,             int &portOFD  );   // ������� ����ன�� ���


                bool getFNVer                                        ( string &fnVer );   // ������ ����� ��訢�� ��
                bool fsTypeGet                                       (    int &      );   // ������ ⨯ ��
                virtual bool closeFN (string cashierName, string& dateTime, uint32_t& fd, uint32_t& fpd);   // ��楤�� ������� ��
                bool closeEmegencyFN                                                ();   // ���਩��� �����⨥ ��
                bool closeEmegencyShift                                             ();   // ���਩��� �����⨥ ᬥ��


                //--------------------------------------------------------------------------------------------------------------
                // ��ࠢ����� ���⠬� (��⮢�� ��᪠) ���.  ���������
                //--------------------------------------------------------------------------------------------------------------
                bool getCalcMask ( int &mask );
                bool setCalcMask ( int  mask );

                /**
                * @brief writeSettingsTableWrapper - ����⪠ ����� ������ � ⠡���� ����஥�
                * @param number - �����
                * @param index  - ������ (��� ���ᨢ��)
                * @param data   - ����� ��ப���� ���祭�� ����� ⠡����
                * @return ������ �ᯥ譮 �஢�����/�� �஢�����
                */
                bool writeSettingsTableWrapper(unsigned char number, int index, const char *data);

                /**
                * @brief setENVDRegData - ������ ���� ������������ � ���� ��-�� ��� ���� ०���
                * @param orgName - ������������ ��-��
                * @param orgAdress - ���� ��-��
                * @return ������ �ᯥ譮 �஢�����/�� �஢�����
                */
                bool setENVDRegData (const string &orgName, const string &orgAdress );
                /**
                 * @brief The RECEIPT struct ������� ������ �᪠�쭮�� 祪�
                 */
                struct RECEIPT
                {
                    OPEN_DOC doc;                               // DOC_TYPE_SALE            ��� �� �த��� (��室)
                                                                // DOC_TYPE_SALE_RET        ��� �� ������ (������ ��室�)
                                                                // DOC_TYPE_CONSUMPTION     ��� �� ���㯪� (��室)
                                                                // DOC_TYPE_CONSUMPTION_RET ��� �� ������ ���㯪� (������ ��室�)
                    std::vector<RUS::Position> positions;            // ���᮪ ����権
                    PAYMENT payment;                            // �����
                    std::string buyerAddress;                   // ���� ���㯠⥫�
                    std::string buyerName;                      // ������������ ���㯠⥫�
                    std::string buyerInn;                       // ��� ���㯠⥫�

                    uint32_t fd         = 0;                    // �� ���㬥��
                    uint32_t fpd        = 0;                    // ��� ���㬥��
                    uint32_t shiftNum   = 0;                    // ����� ᬥ��
                    uint32_t docInShift = 0;                    // ����� ���㬥�� � ᬥ��
                    std::string dateDoc;                        // ��� ���㬥��
                    std::string timeDoc;                        // �६� ���㬥��

                    std::string discountRubName;                // �������� ᪨��� �� 祪
                    double discountRubSum;                      // ������ � �㡫�� �� 祪
                    int/*Positions::DISCOUNT_TYPE_BIOS*/ discountType; // ��� ᪨���

                    std::string marginRubName;                  // �������� �������� �� 祪
                    double marginRubSum;                        // �㬬� �������� � �㡫�� �� 祪

//                    Agent            agent;                     // ����� ��� 祪�
                    std::string addUserReqName;                 // ������������ �������⥫쭮�� ४����� ���짮��⥫�
                    std::string addUserReqValue;                // ���祭�� �������⥫쭮�� ४����� ���짮��⥫�
                    std::string addRecReq;                      // �������⥫�� ४����� 祪� (���)
                    std::string clcPlace;                       // ���� �����
                };

                struct PAY_TYPE
                {
                    PAY_TYPE(uint8_t  newNum, string  newName)
                    {
                        num = newNum;
                        name = newName;
                    }
                    uint8_t  num =  0;
                    string  name;
                };
                // ���᮪ ����� � 祪(ᬥ蠭��� �����)
                map<int, uint64_t> recPayTypes;
                // ���᮪ ��������� �����
                vector<PAY_TYPE>  payTypes;
                // ���प������ ����� ��������� �����
                const string PAY_TYPE_NAME_CASH     = "���������",
                             PAY_TYPE_NAME_CASHLESS = "������������",
                             PAY_TYPE_NAME_ADVANCE  = "��������������� ������ (�����)",
                             PAY_TYPE_NAME_CREDIT   = "����������� ������ (������)",
                             PAY_TYPE_NAME_OTHER    = "���� ����� ������";
                // ������ �� ⨯� ���⥦��(�� 䠪�� ���� ⮫쪮 ��१�ࢨ஢����, ᮮ⢥��⢥��� �� ������)
                bool   readPayTypes();

                //--------------------------------------------------------------------------------------------------------------
                // ToDo KPV
                // ����� �� 祪�
                struct RECEIPT_FIS_DATA
                {
                    int          type       = -1;   // ⨯ 祪� (��� ���㫨஢. = 0)
                    string       cur_op_cnt = "";   // ⥪�騩 �����. ���稪
                    int          rec_num    = -1;   // ����� 祪�
                    int          doc_num    = -1;   // ����� ���㬥��
                    double       sum        = 0.0;  // �㬬� 祪�
                    double       discount   = 0.0;  // �㬬� ᪨���
                    const double reserved   = 0.0;  // ��१�ࢨ஢���(0)
                    string       fpd        = "";   // ��ப� �� (��. �ਧ���)
                    int          fd         = -1;   // ����� ��
                };

                virtual bool rec_fis_data_get(RECEIPT_FIS_DATA&);   // ����祭�� ������ �� ��᫥����� 祪� PIRIT_RECEIPT_DATA

                //--------------------------------------------------------------------------------------------------------------
                // ����⨥ ���㬥�� �� ���������� ��᪠���/��ࢨ��
                //--------------------------------------------------------------------------------------------------------------
                virtual bool openDoc                        ( OPEN_DOC *open_doc );   // ����⨥ ���㬥��
                virtual bool cancelDoc                                          ();  // ���㫨஢���� ���㬥��
                //--------------------------------------------------------------------------------------------------------------
                bool openShift                          ( string cashier );   // ����⨥ ᬥ�� � ����
                bool addExtraReq                   ( const Position &pos );   // ��⠭����� �������⥫�� ४������ ����樨
                bool addPositionToCheck                  ( Position *pos );   // �������� ������ � ������ ���㬥��
                bool subTotal                                           ();   // ����⮣
                bool addDiscountRubles    ( string          nameDiscount ,
                                            uint64_t                 sum );   // �������� ᪨��� � �㡫��/��業�� �� 祪
                bool addMarginRubles      ( string nameMargin,   int sum );   // �������� ��業�� � �㡫�� �� 祪
                /**
                 * @brief closeDoc �������� �।���������� ���㬥��
                 * @param buyerAddress ���� ���㯠⥫�
                 * @return १���� �믮������
                 */
                bool closeDoc(const string& buyerAddress);
                /**
                 * @brief closeDoc �������� �।���������� ���㬥��
                 * @param receipt ������� 祪�
                 * @return १���� �믮������
                 */
                bool closeDoc(RUS::RECEIPT& receipt);
                bool closeShift                         ( string cashier );   // �����⨥ ᬥ�� � ���� + Z �����
                bool calcStateReport               ( string cashier_name );   // ����� � ⥪�饬 ���ﭨ� ����⮢
                bool insRes                  ( OPEN_DOC *doc, double sum );   // ���ᥭ�� / ����⨥ �㬬�
                bool closeFS                            ( string cashier );   // �����⨥ ��
                virtual bool getDateOfReg           ( string  &dateTime  );   // ������ ���� � �६� ॣ����樨
                virtual bool getFDandFPDofReg (int           &numberOfFD ,
                                       uint32_t                  &FPDnum );  // ������� ����� �� � ��� ॣ����樨/���ॣ����樨 �१ ���㧪� ��� �� ��
                        bool  addSTLVheader (std::vector <uint8_t>  &dest,
                                                  const  uint16_t    type,
                                                  const  uint16_t     len) const;  // insert 4 bytes LE <⨯><⨯><�����><�����>
                virtual bool getSTLVofReg      (vector <uint8_t>& stlvReg,
                                                int&               typeFd);  // ������� STLV  ॣ����樨/���ॣ����樨
                bool fiscalDocBinUnload              (  int     fd_number,   // ���㧪� ���㬥�� �� ������ �� �� �१ ����
                                                        char   **doc_data,
                                                          int       &size,
                                                          bool   &isCheck,
                                                          int    &docType);
                /**
                 * @brief setPosAddReq - ��⠭����� �������⥫�� ४����� ����樨
                 * @param requisite - ४�����
                 * @return - १���� �믮������ �������
                 */
                bool setPosAddReq(string requisite);
                //--------------------------------------------------------------------------------------------------------------
                int fs_reset();
                //-------------------------------------------------------------------------------------------------------------



                //--------------------------------- �ਬ��� ���� ---------------------------------------------------------
                /**
                 * @brief dreamkasKeyActivate
                 * @param key
                 * @return true � ��砥 �ᯥ�, ���� false
                 */
                bool dreamkasKeyActivate(string &key);
                /**
                 * @brief dreamkasKeyGetServiceInfo
                 * @param license - ������� ��㣨
                 * @param index - ����� ������
                 * @return true � ��砥 �ᯥ�, ���� false
                 */
                bool dreamkasKeyGetServiceInfo(License::LicenseTag &license, const uint8_t &index);
                /**
                 * @brief dreamkasKeyGetServiceInfoList
                 * @param license - ����� ���
                 * @return true � ��砥 �ᯥ�, ���� false
                 */
                bool dreamkasKeyGetServiceInfoList(vector<License::LicenseTag> &license);
                /**
                 * @brief dreamkasKeyPrintAll - ����� ��� ����㯭�� ���
                 * @return true � ��砥 �ᯥ�, ���� false
                 */
                bool dreamkasKeyPrintAll();
                //--------------------------------------------------------------------------------------------------------
//                /**
//                 * @brief setNVR ������ ������ � NVR
//                 * @param nvr ������� NVR
//                 * @return १���� �믮�����
//                 */
//                bool setNVR(NVR &nvr);
                /**
                 * @brief getDocSTLV - ������ ४������ ���㬥�� �� �� �� ������
                 * @param numberOfFD - ����� ���㬥��
                 * @param reqList - ����� ४����⮢ �� ��
                 * @return १���� ����⮢
                 */
                bool getDocSTLV(const int      &numberOfFD, vector<Requisites::Requisite> &reqList);
                /**
                * @brief getDocSTLV - ������ stlv ���㬥�� �� �� �� ������
                * @param numberOfFD - ����� ���㬥��
                * @param stlv       - stlv ���㬥��
                * @param typeFd     - ⨯ ���㬥��
                * @return १���� ����⮢
                */
                bool getDocSTLV(const uint32_t   &numberOfFD,
                                vector <uint8_t>       &stlv,
                                int&                  typeFd);

                /**
                 * @brief getLastFdNum - ������ ����� ��᫥����� ��
                 * @return ����� ��᫥����� ��
                 */
                virtual uint32_t getLastFdNum() const;

                /**
                 * @brief getExtendedErrorCode - ������ ᮮ�饭�� ���७���� ���� �訡��
                 * @return - ��ப� � ᮮ�饭��� �� �訡��
                 */
                string getExtendedErrorCode();

                /**
                 * @brief getMemoryToOfdExhausted - ����祭�� 䫠�� ���௠��� ����� �࠭���� ������ ��� ���
                 * @return true - 䫠� ��⠭�����, ���� - false
                 */
                virtual bool getMemoryToOfdExhausted() const {return memoryToOfdExhausted.load();}

                /**
                 * @brief setMemoryToOfdExhausted - ��⠭����� 䫠� ���௠��� ����� �࠭���� ������ ��� ���
                 * @param value - ���祭�� 䫠��
                 */
                void setMemoryToOfdExhausted(const bool &value) {memoryToOfdExhausted.store(value);}
                /**
                 * @brief getLastRegInfo - ����� ������ ��᫥���� ॣ����樨/���ॣ����樨
                 * @param fd - fd ॣ����樨/���ॣ����樨
                 * @param fpd - fpd ॣ����樨/���ॣ����樨
                 * @return १����
                 */
                bool getLastRegInfo   (uint32_t &fd, uint32_t &fpd );

        protected:
                /**
                 * @brief fillErrorCodes - ���������� �������� ��६����� �� ����� �訡��
                 * @param code - ��� �訡��
                 */
                void fillErrorCodes(const FB_ERROR_CODE &code);

        private:


            /**
             * @brief getNumFDofReg - ������ ����� �� ��᫥���� ॣ����樨
             * @param fdNumberReg - ����� �� ��᫥���� ॣ����樨
             * @return १���� ����⮢
             */
            bool getNumFDofReg    ( int &fdNumberReg );
            bool tlv_parse        ( unsigned char  *tlv, vector <Requisites::Requisite> *requisites, uint16_t &retOffset);
            bool stlv_parse       ( unsigned char *stlv, vector <Requisites::Requisite> *requisites );
            //--------------------------------------------------------------------------------------------------------------
        };
        shared_ptr<RUS> RF;
        virtual bool receipt_send_rf_bios (RUS::RECEIPT *receipt , Config &config);                  // ��ࠢ�� 祪� � BIOS RF
    //======================================================================================================================
    //======================================================================================================================
    //======================================================================================================================
    //======================================================================================================================
    //                                                     ��
    //======================================================================================================================
    //======================================================================================================================
    //======================================================================================================================
    //======================================================================================================================
    class BEL
    {
    private:
        mutex        *mutex_rb;
    public:
        inline std::mutex* mutex_get(                  ){ return      mutex_rb; }
        inline void        mutex_set( mutex *mutex_ptr ){ mutex_rb = mutex_ptr; }

        BEL( /*mutex**/ );
        ~BEL();
        bool       isConnected;                                          // ���� ᮥ�������
        unique_ptr<Convert> convert;

        //--------------------------------------------------------------------------------------------------------------
        // ��� ⮢��
        static const string NOT_GTIN, // ����-���
                                GTIN, // ��⨪�
                             SERVICE; // ��㣠

        //----------------------------------------------------------------------------------------------------------
        struct OPEN_DOC
        {
            OPEN_DOC();
            ~OPEN_DOC();
            string       cashier;
            DOC_TYPES_BY    type;      //  ⨯ ���뢠����� ���㬥��
            DOC_FORMATS      fmt;      //  ०�� �ନ஢���� ���㬥��
            DOC_REQ_PRINT    req;      //  ०�� ���� ४����⮢
            int            otdel;      //  ����� �⤥��
        };

        //----------------------------------------------------------------------------------------------------------
        struct POSITION
        {
            POSITION();
            ~POSITION();
            string                          name; // (��ப�[0...224]) �������� ⮢��
            string                       barcode; // (��ப�[0..18]) ��⨪� ��� ���客�� ��� ⮢��/����� ���
            double                      quantity; // (�஡��� �᫮) ������⢮ ⮢�� � ⮢�୮� ����樨
            double                         price; // (�஡��� �᫮) ���� ⮢�� �� ������� ��⨪��
            unsigned char              taxNumber; // (����� �᫮) ����� �⠢�� ������
            int                      numGoodsPos; // (��ப�[0..4]) ����� ⮢�୮� ����樨
            unsigned char              numDepart; // (����� �᫮ 1..16) ����� ᥪ樨

            string               discountRubName; // �������� ᪨���
            uint64_t              discountRubSum; // ������
            /*Positions::DISCOUNT_TYPE_BIOS*/int      discountType; // ��� ᪨���

            string                 marginRubName; // �������� ��������
            uint64_t                marginRubSum; // �㬬� ��������
        };

        //----------------------------------------------------------------------------------------------------------
        struct RECEIPT
        {
            // DOC_TYPE_SALE                ��� �� �த��� (��室)
            // DOC_TYPE_SALE_RET            ��� �� ������ (������ ��室�)
            // DOC_TYPE_CONSUMPTION         ��� �� ���㯪� (��室)
            // DOC_TYPE_CONSUMPTION_RET     ��� �� ������ ���㯪� (������ ��室�)
            OPEN_DOC                  doc;
            vector <POSITION>   positions;
            PAYMENT               payment;
            string           buyerAddress;

            string               discountRubName; // �������� ᪨��� �� 祪
            uint64_t              discountRubSum; // ������
            /*Positions::DISCOUNT_TYPE_BIOS*/int      discountType; // ��� ᪨���

            string                 marginRubName; // �������� �������� �� 祪
            uint64_t                marginRubSum; // �㬬� ��������
        };

        map<int, uint64_t> recPayTypes;    // ���᮪ ����� � 祪(ᬥ蠭��� �����)

        //--------------------------------------------------------------------------------------------------------------
        //                                               ���
        //--------------------------------------------------------------------------------------------------------------
        struct NDS
        {
            string  name; // �������� �⠢�� ������ (��ப� ������ 0..18 ᨬ�����)
            double value; // ��業� �⠢�� ������ (���祭�� ��業� � �筮���� �� 4-� ������ ��᫥ ����⮩)
        };
        enum TAX
        {
            NDS_20 = 0, // 20%
            NDS_10,     // 10%
            NDS_16_67,  // 16,67%
            NDS_9_09,   // 9,09%
            NDS_0       // 0%
        };

        bool ndss_read();                                       //����� �� ���� 0..5
        bool nds_read( int i, NDS &nds );
        bool ndss_write(int i = 0);                             // ������� i-� ����� ���ᨢ� 0...5
        bool nds_write(int i, NDS nds);                        // ������� i-� ����� ���ᨢ� 0...5
        inline vector<NDS>*   ndss_get() { return     &NDSS; }
        /**
         * @brief getNDSTableSize ������� ࠧ��� ⠡���� ����������� ���
         * @return ࠧ��� ⠡���� ����������� ���
         */
        int getNDSTableSize();
        //----------------------------------------------------------------------------------------------------------
        //  �������� �⤥��� (���ᨢ �� 16 � ����)
        //----------------------------------------------------------------------------------------------------------
        bool   dep_name_write      ( string name, int num = 1);   // ������   �������� �⤥��, �� 㬮�砭�� 1-�� (1..16) �� ����
        bool   dep_name_read       ( int num, string &name   );   // ������� �������� �⤥��, �� 㬮�砭�� 1-�� (1..16) �� ����
        struct DEP
        {
            uint8_t  num;
            string  name;
        };
        vector<DEP>  DEPS;
        static const u_int8_t DEFAULT_DEPS_CNT = 4;
        bool   deps_read();                                       // ����� �� ᥪ樨
        inline vector<DEP>*   deps_get() { return     &DEPS; }

        struct PAY_TYPE
        {
            uint8_t  num =  0;
            string  name;
        };
        vector<PAY_TYPE>  payTypes;

        bool   writePayType      ( const int num, const  string &name );   // ������   �������� ⨯� ���⥦�, �� 㬮�砭�� 1-�� ( 0..15 ) �� ����
        bool   readPayType       ( const int num,        string &name );   // ������� �������� ⨯� ���⥦�, �� 㬮�砭�� 1-�� ( 0..15 ) �� ����

        // ������ �� ⨯� ���⥦��
        bool   readPayTypes();
        //--------------------------------------------------------------------------------------------------------------
        //                                  �����     ���
        //--------------------------------------------------------------------------------------------------------------

        // ����� �⠫쭮�� ���ﭨ� ���
        enum KCA_FATAL_FLGS
        {
            WRONG_NVR_SUM      =  1,       // ����ୠ� ����� �㬬� ���
            WRONG_CONF_SUM     =  2,       // ����ୠ� ����� �㬬� ���䨣��樨
            BEP_IFACE_ERROR    =  4,       // �訡�� ����䥩� ���
            WRONG_BEP_SUM      =  8,       // ����ୠ� ����� �㬬� ���
            BEP_WRITE_ERR      = 16,       // �訡�� ����� � ���
            BEP_NOT_AUTH       = 32,       // ��� �� ���ਧ����
            KL_MEM_ERR         = 64        // ��⠫쭠� �訡�� ����� ����஫쭮� �����
        };

        // ����� ⥪��� 䫠��� ���
        enum KCA_CUR_FLGS
        {
            START_FAILED        =   1, // �� �뫠 �맢��� �㭪�� ?��砫� ࠡ���?
            STUDY_MODE          =   2, // �祡�� ०��
            SHIFT_OPEN          =   4, // ����� �����
            SHIFT_24H           =   8, // ����� ����� 24 �ᮢ
            BUFFER_IS_NOT_EMPTY =  16, // ���������� ����� � ���� ����஫쭮� ����� (���� �� ���⮩)
            RESERVED_1          =  32, // ��१�ࢨ஢��
            CLOSE_SHIFT_MEM_ERR =  64, // ��� ����� ��� ������� ᬥ�� � ���
            RESERVED_2          = 128, // ��१�ࢨ஢��
            CLOSE_SHIFT_ERR     = 256  // �� �뫮 �����襭� �����⨥ ᬥ��, ����室��� ������� ������
        };

        // ��� ����⮣� ���㬥��
        enum KCA_DOC_TYPE
        {
            KCA_TYPE_CLOSED          =  1,    // ���㬥�� ������
            KCA_TYPE_SERVICE         =  2,    // ��ࢨ�� ���㬥��
            KCA_TYPE_SALE            =  3,    // ��� �� �த���
            KCA_TYPE_RETURN          =  4,    // ��� �� ������
            KCA_TYPE_INJECT          =  5,    // ���ᥭ�� � �����
            KCA_TYPE_RESERVE         =  6,    // ��������
            KCA_TYPE_RECIEPT_ORDER   =  7,    // ��� ��ଫ���� ������
            KCA_TYPE_RECIEPT_CLOSE   =  8,    // ��� ������� ������
            KCA_TYPE_RECIEPT_CANCEL  =  9,    // ��� �⬥�� ������
            KCA_TYPE_RECIEPT_ADDING  = 10,    // ��� ���������� ������
            KCA_TYPE_RECIEPT_CORRECT = 11,    // ��� ���४樨 ������
            KCA_TYPE_RECIEPT_INVOCIE = 12     // ��� �� ������
        };

        // ����ﭨ� ���㬥��
        enum KCA_DOC_STATE
        {
            KCA_STATE_CLOSED              = 0,   // ���㬥�� ������
            KCA_STATE_OPENED              = 1,   // ��⠭���������� ��᫥ ������� ?������ ���㬥��?
            KCA_STATE_FIRST_PODITIGED     = 2,   // ��⠭���������� ��᫥ ��ࢮ� ������� ?����⮣?. ����� ������ ᪨���/��業�� �� 祪
            KCA_STATE_SECOND_PODITIGED    = 3,   // ��⠭���������� ��᫥ ��ன ������� ?����⮣? ��� ��᫥ ��砫� ������� ?�����?
            KCA_STATE_NEED_CLOSE          = 4    // ����� �����襭 ? �ॡ���� ������� ���㬥��.
        };

        enum KCA_SCNO_STATE
        {
            KCA_SCNO_OK          =  0,    // ��� ��
            KCA_SCNO_COMMON_ERR  =  1,    // ���� �訡�� ����
            KCA_SCNO_CONNECT_ERR =  2,    // ��������� ��� � ����
            KCA_SCNO_NO_SKZI     =  4,    // ��������� ����
            KCA_SCNO_BAD_SCNO    =  8,    // ����ࠢ�� ����
            KCA_SCNO_IDENT_ERR   = 16,    // �����䨪��� �� ��諠 �ᯥ譮
            KCA_SCNO_SERTIF_ERR  = 32,    // ����� ���㦨����� �� ����砭�� ���䨪�� ����
            KCA_SCNO_Z_REP_ERR   = 64     // ����� ���㦨����� �� ����।���� Z-���⠬

        };
        /**
         * @brief ksaInfGet - ����� ᢥ����� � ���
         * @return true - �ᯥ�
         */
        template<typename T>
        bool ksaInformationGet( KSA_INFORMATION param, T &value )
        {
            lock_guard<mutex> locker(*mutex_rb);
            int res = ksaInfGet<T>( KSA_INF_NEXT_DOC_NUM, value );
            bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
            if( !ok )
            {
                logERR( "kca_flgs_get():: �訡��! १���� ������� libGetStatusFlags_By()  = %d ", res );
                return false;
            }

            return true;
        }
        /**
         * @brief getNextDocNum �⥭�� ᪢������ ����� ᫥���饣� ���㬥��
         * @return ᪢����� ����� ᫥���饣� ���㬥��
         */
        int getNextDocNum();

        enum DISCOUNT_MARGIN
        {
            DISCOUNT_CHECK = 1,
            DISCOUNT_POS,
            MARGIN_CHECK,
            MARGIN_POS,
            DEPARTMENT
        };

        //----------------------------------------------------------------------------------------------------------
        // ����� 䫠��� ���
        //----------------------------------------------------------------------------------------------------------
        bool   kca_flgs_get ();          //����� 䫠��� ��� �� ����

        // ����䥩� ��� ����ᮢ ⥪ 䫠��� ���
        inline bool   is_start_failed_get         (                        ) { return        is_start_failed; }
        inline bool   is_study_mode_get           (                        ) { return          is_study_mode; }
        inline bool   isShiftOpen_get             (                        ) { return          is_shift_open; }
        inline void   is_shift_open_set           (   bool isShiftOpen     ) {   is_shift_open = isShiftOpen; }
        inline bool   is_shift_24h_get            (                        ) { return           is_shift_24h; }
        inline void   is_shift_24h_set            (   bool isShift24h      ) {     is_shift_24h = isShift24h; }
        inline bool   is_buffer_is_not_empty_get  (                        ) { return is_buffer_is_not_empty; }
        inline bool   is_close_shift_mem_err_get  (                        ) { return is_close_shift_mem_err; }
        inline bool   is_clos_shift_err_get       (                        ) { return     is_close_shift_err; }
        // ����䥩� ��� ����ᮢ �⠫쭮�� ���ﭨ� ���
        inline bool   is_wrong_NVR_sum_get       (                        ) { return        is_wrong_NVR_sum; }
        inline bool   is_wrong_conf_sum_get      (                        ) { return       is_wrong_conf_sum; }
        inline bool   is_BEP_err_get             (                        ) { return              is_BEP_err; }
        inline bool   is_wrong_BEP_sum_get       (                        ) { return        is_wrong_BEP_sum; }
        inline bool   is_BEP_write_err_get       (                        ) { return        is_BEP_write_err; }
        inline bool   is_BEP_not_auth_get        (                        ) { return         is_BEP_not_auth; }
        inline bool   is_KL_mem_err_get          (                        ) { return           is_KL_mem_err; }

        inline bool   isDocOpenedGet             (                        ) { return             isDocOpened; }
        //----------------------------------------------------------------------------------------------------------
        // ��᪠��� ����樨
        //----------------------------------------------------------------------------------------------------------
        bool openShift                   (                              );   // ����⨥ ᬥ�� � ����
        virtual bool openDoc                     (    OPEN_DOC *open_doc        );   // ����⨥ ���㬥��
        bool addPositionToCheck          (    POSITION *pos             );   // �������� ������ � ������ ���㬥��
        virtual bool subTotal                                                  ();   // ����⮣
        bool addDiscountRubles           ( string            nameDiscount,
                                           uint64_t                   sum,
                                           /*Positions::DISCOUNT_TYPE_BIOS*/int    discType = /*Positions::DISCOUNT_TYPE_ABSOLUTE*/0 );   // �������� ᪨��� �� 祪/������
        bool addMarginRubles             ( string nameMargin,   uint64_t sum );   // �������� ��業�� �� 祪/������
        bool closeDoc                    (                              );   // �������� �।���������� ���㬥��
        bool insRes                      (    OPEN_DOC *doc, double sum );   // ���ᥭ�� / ����⨥ �㬬�
        bool closeShift                  (    string cashier            );   // �����⨥ ᬥ��
        bool cancelDoc                   (                              );   // ���㫨஢���� ���㬥��
        bool cancelDocBel                ( const uint32_t&, string, double (&arr)[MAX_PAY_TYPES] );   // ���㫨஢���� ���㬥��
        virtual bool curShiftNum                 (    uint64_t &shiftNum        );   // ������� ����� ⥪�饩 ᬥ��
        /**
         * @brief getDocFromKL �⥭�� ���㬥�� �� ��
         * @param docNum ����� ���㬥��
         * @param doc ���㬥�� �� ��
         * @return १���� �믮������
         */
        bool getDocFromKL(const uint32_t &docNum, DocFromKL &doc);
        // ����� �� 祪�
        struct RECEIPT_FIS_DATA
        {
            int          type       = -1;   // ⨯ 祪� (��� ���㫨஢. = 0)
            string       cur_op_cnt = "";   // ⥪�騩 �����. ���稪
            int          rec_num    = -1;   // ����� 祪�
            int          doc_num    = -1;   // ����� ���㬥��
            double       sum        = 0.0;  // �㬬� 祪�
            double       discount   = 0.0;  // �㬬� ᪨���
            double       markup     = 0.0;  // �㬬� ��業��
        };

        virtual bool rec_fis_data_get( RECEIPT_FIS_DATA&);  // ����祭�� ������ �� ��᫥����� 祪� PIRIT_RECEIPT_DATA

        bool print_report_fixed_mem_unit(string, string, string); // ����� ���� �� ����� �� ���
        bool print_report_ctrl_tape();            // ��ᯥ���� ���� ����஫쭮� �����

    private:
        vector<NDS>        NDSS; // ����� �� 6-� ��������� ��� �� ������
        //----------------------------------------------------------------------------------------------------------
        //                                     ����� ���
        //----------------------------------------------------------------------------------------------------------
        // ����� ⥪ 䫠��� ���
        bool    is_start_failed,        // �� �뫠 �맢��� �㭪�� ?��砫� ࠡ���?
                is_study_mode,          // �祡�� ०��
                is_shift_open,          // ����� �����
                is_shift_24h,           // ����� ����� 24 �ᮢ
                is_buffer_is_not_empty, // ���������� ����� � ���� ����஫쭮� ����� (���� �� ���⮩)
                is_close_shift_mem_err, // ��� ����� ��� ������� ᬥ�� � ���
                is_close_shift_err;     // �� �뫮 �����襭� �����⨥ ᬥ��, ����室��� ������� ������
        // ������ ���ﭨ� ���㬥�� ���
        bool    isDocOpened;
        // ����� �⠫쭮�� ���ﭨ� ���
        bool    is_wrong_NVR_sum,       // ����ୠ� ����� �㬬� ���
                is_wrong_conf_sum,      // ����ୠ� ����� �㬬� ���䨣��樨
                is_BEP_err,             // �訡�� ����䥩� ���
                is_wrong_BEP_sum,       // ����ୠ� ����� �㬬� ���
                is_BEP_write_err,       // �訡�� ����� � ���
                is_BEP_not_auth,        // ��� �� ���ਧ����
                is_KL_mem_err;          // ��⠫쭠� �訡�� ����� ����஫쭮� �����

        KCA_DOC_TYPE     doc_type;      // ���    ����⮣� ���㬥��
        KCA_DOC_STATE   doc_state;      // ����� ����⮣� ���㬥��
        KCA_SCNO_STATE scno_state;      // ����ﭨ� ���� 0(KCA_SCNO_OK) - ��� ��
    public:
        //----------------------------------------------------------------------------------------------------------
        //                                          ����� � ��
        //----------------------------------------------------------------------------------------------------------
        bool KLDocRead ( int doc_num, vector<string> &strs );
        bool KLDocPrint( int doc_num );


        /**
         * @brief getSKNODateTime ������ ���� � �६� �� ����
         * @param tmt �६� �� 誠�� unix
         * @return true � ��砥 �ᯥ�, ���� false � tmt ��⠭���������� � -1
         */
        virtual bool getSKNODateTime    (Convert::DATE_ISO_8601 &dateIso);
        /**
         * @brief getScno_state ����祭�� ����� ࠡ��� � ����
         * @return scno_state
         */
        KCA_SCNO_STATE getScno_state() const;
    };
    virtual bool receipt_send_rb_bios( BEL::RECEIPT *receipt );                  // ��ࠢ�� 祪� � BIOS RB
    //----------------------------------------------------------------------------------------------------------
    shared_ptr<BEL> RB;
    //======================================================================================================================
    //======================================================================================================================
    //======================================================================================================================
};

#endif //FISCAT_FISGO_BIOS_H
