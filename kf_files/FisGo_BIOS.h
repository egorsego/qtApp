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
    bool fatal_wrongSummNVR,        // 1-й бит Неверная контрольная сумма NVR
         fatal_wrongSummKonf,       // 2-й бит Неверная контрольная сумма в конфигурации
    //   reserved,                  // 3-й бит Зарезервировано
    //   reserved;                  // 4-й бит Зарезервировано
    //   reserved;                  // 5-й бит Зарезервировано
         fatal_FnNotAuth,           // 6-й бит ККТ не авторизовано
         fatal_FnFatalErr,          // 7-й бит Фатальная ошибка ФН
    //   reserved;                  // 8-й бит Зарезервировано
         fatal_errSD;               // 9-й бит 	SD карта отсутствует или неисправна

    bool kkt_notInited,              //  1-й бит     Не была вызвана функция ?Начало работы?
         kkt_notFiscal,              //  2-й бит 	 Нефискальный режим
         kkt_isShiftOpened,          //  3-й бит 	 Смена открыта
         kkt_isShift24h,             //  4-й бит     Смена больше 24 часов
         kkt_fsClose,                //  5-й бит     Архив ФН закрыт
         kkt_notRegistered,          //  6-й бит     ФН не зарегистрирован (postFiscal =   kkt_notFiscal (???) kkt_notRegistered)
    //   reserved,                   //  7-й бит     Зарезервировано
    //   reserved,                   //  8-й бит     Зарезервировано
         kkt_colseShiftNeed,         //  9-й бит     Не было завершено закрытие смены, необходимо повторить операцию
         kkt_controlLentaErr;        // 10-й бит     Ошибка контрольной ленты

    int  documentType;               // Тип текущего открытого документа
    int  documentState;              // Состояние документа

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
    unsigned char         type;    // 0x00 - изм. параметров без замены ФН, 0x01 - рег ККТ или перерег с заменой ФН
    string           regNumber;    //  Строка[16] Регистрационный номер ровно 16 символов!
    string                 INN;    // Строка[12] ИНН пользователя
    int              systemTax;    //     Системы налогообложения:
                                   // 1 - ОСН, 2 - УСН,
                                   // 4 - ЕНВД, 8 - ЕСХН 16 - ПАТЕНТ
                                   // !!!! ДОДХОД-РАСХОД НЕТ!
    int                    rej;    // bit   Режим работы:
                                   // 0  - Шифрование
                                   // 1  - Режим связи с ОФД
                                   // 2  - Автоматический режим
                                   // 3  - Применение в сфере услуг
                                   // 4  - БСО
                                   // 5  - Применение в Интернет
                                   // 6  - Подакцизные товары
                                   // 7  - Азартные игры
                                   // 8  - Применение банковскими платежными агентами
                                   // 9  - Применение банковскими платежными субагентами
                                   // 10 - Применение платежными агентами
                                   // 11 - Применение платежными субагентами
                                   // 12 - Применение поверенными
                                   // 13 - Применение комиссионерами
                                   // 14 - Применение агентами
                                   // 15 - Лотерея
    string         cashierName;    // Строка[0..64]  Имя кассира
    string           userName1;    // Строка[0..44]  Наименование пользователя строка 1
    string           userName2;    // Строка[0..44]  Наименование пользователя строка 2(необязательная)
    string      addressSettle1;    // Строка[0..44]  Адрес расчетов(пользователя) строка 1
    string      addressSettle2;    // Строка[0..44]  Адрес расчетов(пользователя) строка 2 (Необязательная)
    string         placeSettle;    // Строка[0..112] Место расчётов
    string          autoNumber;    // Строка[0..12]  Номер автомата
    string             OFDName;    // Строка[0..64]  Наименование ОФД
    string             OFD_INN;    // Строка[0..12]  ИНН ОФД
    string         senderEMAIL;    // Строка[0..44]  Адрес электронной почты отправителя чека.
    string            FNS_Site;    // Строка[0..64]  Адрес сайта ФНС.
    bool           isAutomatic;    // Признак установки принтера в автомате.

    registrationStruct()
    {
        systemTax      =                      0x00;
        type           =                      0x00;
        rej            =                      0x00;
        isAutomatic    =                     false;
    }
};


//--------------------------------------------------------------------------------------------------------------
// в FisFo_BIOS.cpp
//--------------------------------------------------------------------------------------------------------------
extern bool          fb_wasErr;         // Флаг для BackgroundEvents, говорящий, что произошла крит. ошибка при выполнении команды в пирит
extern string        fb_errMess;
extern int           fb_errCode;
extern bool          fb_wasWarning;    // Флаг для BackgroundEvents, говорящий, что команда выполнена с ворнингом
extern string        fb_warnMess;
extern int           fb_warnCode;


bool isCommandComplete(int res, FB_OPERATION_CRITICAL_LEVEL op_lvl);

//=====================================================================================
//=====================================================================================
//===================== Интерфейсный класс для работы с БИОС ==========================
//=====================================================================================
//=====================================================================================
class BIOS
{
private:
    string               ip;      // IP BIOS
    int                port;      // Port BIOS
    atomic_bool isConnected;      // Наличие связи с БИОС
    statusBIOS   statusBios;      // Структура состояний БИОСа
    mutex        mutex_bios;      // Общий мьютекс BIOS (RF и RB)
    std::string lastErrorExtMessage; // Последнее запрошенное расширенное сообщение об ошибке
public:
       BIOS();
       virtual ~BIOS();
       unique_ptr<FilesFG>      fg;
       unique_ptr<Convert>      convert;
       mutex initBiosMutex;           // mutex для флага инициализации BIOS
       condition_variable initBiosCv; // cv для флага инициализации BIOS
       #ifdef BIOS_RF
           const string FS_NAME = "ФН";
       #else
           const string FS_NAME = "СКНО";
       #endif
       inline const std::string &getLastErrorExtMessage() const {return lastErrorExtMessage;}
       inline mutex* mutex_get()      { return &mutex_bios; }
       void   beep                                (int microSec);   // бипер (Целое число 10...2000) Длительность в мкс
       virtual bool initConnection();   // Создание соединения с БИОСом
       bool   techZeroing     (string zavNum = "0497000001    ");   // Тех обнуление, как вы понимаете
       bool   printXReport                         (string name);   // Печать X отчёта
       void   printLastFiscalReport                           ();   // Печать последнего отчёта о рег/перерег
       bool   printFnDocs                      (int numberOfDoc);   // Распечатать документ из ФН. 0=все рег-ии и перерег-ии
       void   printBIOSTechReport                             ();   // Печать тех. отчёта BIOS
       /**
        * @brief cancelDocBIOS отмена документа BIOS
        */
       void cancelDocBIOS();
       /**
        * @brief isDocCanceled тест на отмену крайнего документа
        * @return результат выполнения
        */
       bool isDocCanceled();
       /**
        * @brief cancelDoc отмена открытого документа
        * @return результат выполнения
        */
       bool cancelDoc();
       /**
        * @brief setDateTimeToBIOS Установить дату и время в БИОС как на ККТ(текущую)
        * @return результат выполнения
        */
       bool   setDateTimeToBIOS                               ();
       /**
        * @brief setDateTimeToBIOS Установить дату и время в БИОС (произвольная)
        * @param[in] unixTime дата
        * @return результат выполнения
        */
       virtual bool   setDateTimeToBIOS(const time_t &unixTime);
       bool   getStatusBIOS                                   ();   // Получить все флаги статусов из БИОСа
       template<typename T>                                         // Считать счётчики из BIOS
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
    // Счётчики
    //--------------------------------------------------------------------------------------------------------------
    bool getCounters_CurShiftNum     ( uint64_t               &shiftNum );  // 0x01 Номер тек смены
    bool getCounters_NextRecNum      ( uint64_t             &nextRecNum );  // 0x02 Вернуть номер следующего чека
    virtual bool getCounters_SaleSumByPay    ( OPERATION<double>          &ssbp );  // 0x03 Вернуть суммы продаж по типам платежа
    bool getCounters_SaleCntByPay    ( OPERATION<uint64_t>        &scbp );  // 0x04 Вернуть количество оплат по продажам
    bool getCounters_RetSaleSumByPay ( OPERATION<double>         &rssbp );  // 0x05 Вернуть суммы возвратов по типам платежа
    bool getCounters_RetSaleCntByPay ( OPERATION<uint64_t>       &rscbp );  // 0x06 Вернуть количество оплат по возвратам
    bool getCounters_RecCntsByType   ( REC_CNTS                &recCnts );  // 0x07 Количество оформленных чеков
    virtual bool getCounters_RecSumsByType   ( REC_SUMS                &recSums );  // 0x08 Суммы по оформленным чекам
    bool getCounters_DiscountSums    ( DISCOUNT_SUMS          &discSums );  // 0x09 Суммы по скидкам
    bool getCounters_TaxSumsSale     ( TAX_SUMS                &taxSums );  // 0x10 Суммы налогов  по продажам
    bool getCounters_TaxSumsRets     ( TAX_SUMS                &taxSums );  // 0x11 Суммы налогов  по возвратам
    bool getCounters_XZ              ( X_Z_DATA                 &xzData );  // 0x12 Данные по последнему X отчету или отчету о закрытии смены
    bool getCounters_depSaleSum      ( double         (&depSaleSum)[NUM_OF_SECTOINS] );  // 0x13 Вернуть суммы по секциям/отделам по продажам
    bool getCounters_depRetSum       ( double          (&depRetSum)[NUM_OF_SECTOINS] );  // 0x14 Вернуть суммы по секциям/отделам по возвратам
    bool getCounters_RecCount        ( uint64_t            &purchaseCnt ,   // 0x15 Кол-во чеков расходов
                                       uint64_t         &retPurchaseCnt );  // 0x15 Кол-во чеков возвратов расхода
    bool getCounters_SaleSumByType   ( OPERATION<double>     &purchSums );  // 0x16 Вернуть суммы покупок по типам платежа
    bool getCounters_RetSumByType    ( OPERATION<double>  &purchSumsRet );  // 0x17 Вернуть суммы покупок по типам платежа
    bool getCounters_CorData         ( COR_DATA                &corData );  // 0x18 Вернуть данные по коррекциям
    //--------------------------------------------------------------------------------------------------------------
    //                               Общие методы для RUS и BEL прошивок BIOS
    //--------------------------------------------------------------------------------------------------------------
    enum DISCOUNT_TYPE_BIOS : uint8_t
    {
        DISCOUNT_TYPE_PERCENT = 0,
        DISCOUNT_TYPE_ABSOLUTE = 1
    };

    enum BIOS_FN_WARNINGS  : uint8_t
    {
        BIT_IMMEDIATE_REPLACEMENT = 0x01,  // Маска для состояния "Срочная замена КС"
        BIT_RESOURCES_EXHAUSTION  = 0x02,  // Маска для состояния "Исчерпание ресурса КС"
        BIT_MEMORY_OVERFLOW       = 0x04,  // Маска для состояния "Переполнение памяти ФН"
        BIT_CRITICAL_FN_ERROR     = 0x80   // Маска для состояния "Критическая ошибка ФН"
    };
    /**
     * @brief The BIOS_ERR enum ошибки BIOS
     */
    enum BIOS_ERR  : int
    {
        BIOS_ERR_FS = 2222                 // Ошибка ФН
    };

    struct PAYMENT
    {
        PAYMENT();
        ~PAYMENT();
        unsigned char typePayment; // (Целое число 0..15) Код типа платежа
        double                sum; // (Дробное число) Сумма, принятая от покупателя по данному платежу
        string            infoStr; // (Строка[0..40]) Дополнительный текст
    };
    virtual bool   addPaymentToCheck             ( PAYMENT *payment );   // Добавить оплату  в открытый документ
    int setPrinterMode(int mask);   // Установить параметры печатного устройства (маска из enum PIRIT_SETTINGS_KKT)
    int getPrinterMode(int &mask);  // Получить параметры печатного устройства (маска из enum PIRIT_SETTINGS_KKT)
    bool getLicenseOptionsTags(int &mask); // Получить опций лицензии
    void setPrinterExPrint();       // Печатаем чеки из ИМ
    void setPrinterExNotPrint();    // Не печатаем чеки из ИМ
    /**
     * @brief getPurchaseMode чтение параметров чека
     * @param mask параметры чека (битовая маска)
     * @return результат выполнения
     */
    int getPurchaseMode(int &mask);
    /**
    * @brief printPosDiscount  печать скидки на позицию в чеке
     * @param discountSumRubl сумма скидки в рублях
     * @param text текст скидки
    * @return  bool результат добавления в ПФ скидки
    */
    bool printDiscount(const double &discountSumRubl, const string &text);
    /**
     * @brief setPurchaseMode запись параметров чека
     * @param mask параметры чека (битовая маска)
     * @return результат выполнения
     */
    int setPurchaseMode(const uint8_t &mask);
    /**
     * @brief setAutoOpenShift автооткрытие / автозакрытие смены
     * @return результат выполнения
     */
    int setAutoOpenShift();

    /**
     * @brief waitInitConnection - ожидание инициализации BIOS
     */
    bool waitInitConnection();
    //--------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------
       //=================================================================================================================
       static string strFromErrCode             (int errorCode);   // Получение расшифровки кода ошибки либы.
       virtual bool getCashInDrawer            (uint64_t &cash);   // Вернуть сумму наличных в денежном ящике (в копейках)
       bool getKKTPlantNum                   (string &plantNum);   // Вернуть заводской номер ККТ
       bool getKKTRegNum                       (string &regNum);   // Вернуть регистрационный номер ККТ
       virtual bool getDateTimeLastFiscalDoc (string &dateTime);   // Вернуть дату и время последней фискальной операции
       bool getDateLastRegistration              (string &date);   // Вернуть дату последней регистрации/перерегистрации
       virtual bool getDateFNValidBefore         (string &date);   // Дата окончания временного ресурса  ФН
       bool getDateTimeOpenShift             (string &dateTime);   // Вернуть дату и время открытия смены
       bool getSignAndAgents                 (uint16_t &status);   // Вернуть признаки и статус агентов
       bool getTaxSystems                      (uint8_t &taxes);   // Вернуть доступные СНО
       bool getPrinterMachine                (uint16_t &status);   // Вернуть признак наличия принтера

       bool getOrganizationAddress         (string &orgAddress);   // Вернуть адрес организации
       bool getOrganizationINN                    (string &inn);   // Вернуть ИНН организации
       bool getOrganizationName               (string &orgName);   // Вернуть наименование организации
       bool getCalculationPlace             (string &calcPlace);   // Вернуть место расчетов

       bool getOFDStatus                          (int &status);   // Вернуть состояние доступа к ОФД
       bool getOFDName                        (string &ofdName);   // Вернуть наименование ОФД
       bool getOFDPort                           (int &ofdPort);   // Вернуть порт ОФД
       bool getOFDAddress                  (string &ofdAddress);   // Вернуть адрес ОФД
       bool getOFDINN                          (string &ofdINN);   // Вернуть ИНН ОФД

       bool getRecSenderEmail(std::string& recSenderEmail);        // Вернуть адрес отправителя чеков

       bool getFSValidityDate                  (uint16_t &date);   // Вернуть дату окончания ресурса ФН
       bool getAutomatNum                      (string &number);   // Вернуть номер автомата

       bool getFfdKktVer                   (uint8_t &ffdKktVer);   // Версия ФФД ККТ
       bool getFsFfdVer                     (uint8_t &fsFfdVer);   // Зарегистрированная версия ФФД ФН
       bool getKktVer                          (string &kktVer);   // Версия ККТ

       bool getSettingsFromPirit             ( Config &config );   // Заполнить конфиг из PIRIT
       bool getSignSettingsFromPirit         ( Config &config );   // Заполнить признаки и статус агентов

       virtual bool getBatteryVoltage(int &voltage );    // Вернуть напряжение на батарейке


       /**
        * @brief restartKKT - переинициализация БИОС без перезапуска
        * @return true - в случае успеха, иначе false
        */
       bool restartKKT();
      //=================================================================================================================

    // Запрос документа из архива
    bool getDocFromFN (int  fdNumber,
                       char *hexData,
                       int    lenBuf);

    // Интерфейсы к полям:
    virtual void setConnected   (const bool &isConn)  {isConnected.store(isConn);}
    void setPort             (int newPort);
    void setPass      (Country newCountry);
    void setErrors    (Country newCountry);
    void setIP         (string newIP);
    bool isConnectedGet            ()         // Есть ли соединение с БИОС
    {
        return isConnected.load();
    }

    //======================================================================================================================
    //======================================================================================================================
    // КОНСТАНТЫ СОСТОЯНИЙ
    //======================================================================================================================
    // Тип текущего открытого документа
    static const int TYPE_DOC_DOC_IS_CLOSED         =  0,
                     TYPE_DOC_SERVICE_DOC           =  1,
                     TYPE_DOC_CHECK_SALE            =  2,
                     TYPE_DOC_CHECK_SALE_RET        =  3,
                     TYPE_DOC_INSERTION             =  4,
                     TYPE_DOC_RESERVED              =  5,
                     TYPE_DOC_CHECK_CONSUMPTION     =  6,
                     TYPE_DOC_CHECK_CONSUMPTION_RET =  7;
    // Состояние документа
    static const int STATE_DOC_DOC_IS_CLOSED        =  0,
                     STATE_DOC_DOC_IS_OPENED        =  1,
                     STATE_DOC_DOC_IS_PRE_RESULT_1  =  2,
                     STATE_DOC_DOC_IS_PRE_RESULT_2  =  3,
                     STATE_DOC_NEED_CLOSE           =  4,
                     STATE_DOC_CLOSE_ERROR          =  8;
    static bool      is_email_valid (const string& email);
    static bool      leap_valid     ( int, int, int    );
    static bool      day_month_valid( int, int         );

    static const u_int8_t PAY_TYPE_NONE          = -1;    // Невалидный номер типа платежа
    static const u_int8_t MAX_PAY_TYPES          = 16;    // максимальное число типов платежей
    static const u_int8_t MAX_PAY_TYPE_NAME_LEN  = 18;    // максимальная длина типов платежей

    //======================================================================================================================
    //======================================================================================================================
    //======================================================================================================================
    //                                                  Модуль ФН
    //======================================================================================================================
        class RUS
        {
            public:
                RUS();
                ~RUS();
                bool            isConnected;
                unique_ptr<Convert> convert;

            private:
                mutex             *mutex_rf; // Мьютекс ФН
            public:
                //--------------------------------------------------------------------------------------------------------------
                //                                          Класс реквизитов
                //--------------------------------------------------------------------------------------------------------------
                Requisites      REQ;
                //--------------------------------------------------------------------------------------------------------------
                inline std::mutex* mutex_get(                  ){ return      mutex_rf; }
                inline void        mutex_set( mutex *mutex_ptr ){ mutex_rf = mutex_ptr; }
                /**
                 * @brief powerOnFS включение питания ФН (Спутник Ф)
                 */
                static void powerOnFS();
                //--------------------------------------------------------------------------------------------------------------
                //                          Коды команд ФН
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
                //                           Коды ответа ФН
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char         SUCCESS_ANSW = 0x00,       // Удачное выполнение команды
                                             WRONG_CMD_FMT_ANSW = 0x01,       // Неверный формат команды
                                            WRONG_FS_STATE_ANSW = 0x02,       // Неверное состояние ФН
                                                     WRONG_DATE = 0x07;       // Неверное дата и время
                /**
                 * @brief The FS_ERR enum - коды ответа ФН
                 */
                enum FS_ERR
                {
                    FS_ERR_MEM_FOR_OFD_OVERFLOW = 0x54 // Исчерпана память хранения ОФД
                };
                //--------------------------------------------------------------------------------------------------------------
                //                          Коды счётчиков ФН
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char       CUR_SHIFT_CNTS = 0x00,       // счетчики текущей смены
                                                        FS_CNTS = 0x01;       // счетчики ФН
                //--------------------------------------------------------------------------------------------------------------
                //                           Типы документов ФН
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char              REC_DOC = 0x00,       // кассовые чеки (БСО)
                                                    REC_COR_DOC = 0x01;       // чеки (БСО) коррекции
                //--------------------------------------------------------------------------------------------------------------
                //                          Типы расчёта ФН
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char                 SALE = 0x01,       // приход (коррекция прихода)
                                                    RETURN_SALE = 0x02,       // возврат прихода
                                                       PURCHASE = 0x03,       // расход (коррекция расхода)
                                                RETURN_PURCHASE = 0x04;       // возврат расхода
                //--------------------------------------------------------------------------------------------------------------
                //                        Режим работы ФН и ККТ
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char           ENCRYPTION = 0x01,       // Шифрование
                                                     AUTONOMOUS = 0x02,       // Автономный режим
                                                      AUTOMATIC = 0x04,       // Автоматический режим
                                                       SERVICES = 0x08,       // Применение в сфере услуг
                                                            BCO = 0x10,       // Режим БСО иначе Режим чеков
                                                       INTERNET = 0x20;       // Применение в Интернет
                //--------------------------------------------------------------------------------------------------------------
                //                   Причина изменения параметров регистрации
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char       FS_REPLACE_REG = 0x01,
                                                 OFD_CHANGE_REG = 0x02,
                                           USER_DATA_CHANGE_REG = 0x03,
                                             KKT_SET_CHANGE_REG = 0x04;
                //--------------------------------------------------------------------------------------------------------------
                //                    Расширенные признаки работы ККТ
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char      EXCISABLE_GOODS = 0x01,       //Подакцизные товары
                                                       GAMBLING = 0x02,       // Азартные игры
                                                        LOTTERY = 0x04,       // Лотерея
                                                PRINTER_MACHINE = 0x08;       // Установка принтера в авт режиме
                //--------------------------------------------------------------------------------------------------------------
                //                                Версия ФФД
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char              FFD_1_0 = 0x01,       // ФФД 1.0
                                                      FFD_1_0_5 = 0x02,       // ФФД 1.0.5
                                                        FFD_1_1 = 0x03;       // ФФД 1.1
                //--------------------------------------------------------------------------------------------------------------
                //                      Сообщение оператора ОФД
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char            FLK_ERROR = 0x02,       // ОШИБКА ФЛК
                                                   NEED_CFG_KKT = 0x40,       // ТРЕБ. НАСТР. ККТ
                                                      OFD_CLOSE = 0x80;       // ОФД АННУЛИРОВАН
                //--------------------------------------------------------------------------------------------------------------
                //                                   Коды ошибок ФН
                //--------------------------------------------------------------------------------------------------------------
                static const int                        SUCCESS =    0,
                                                  CONNECT_ERROR =   -1,
                                                 WRONG_DATE_ERR =   -7,
                                                      SHIFT_24H =  -16,
                                                        TIMEOUT = -100;
                //--------------------------------------------------------------------------------------------------------------
                //                   Расширенные причины изменения параметров регистрации
                //--------------------------------------------------------------------------------------------------------------
                static const uint32_t                FS_REPLACE = 0x00000001, // Замена ФН
                                         //-------------------------------------------------------------------------------------
                                                     OFD_CHANGE = 0x00000002, // Смена ОФД
                                         //-------------------------------------------------------------------------------------
                                               USER_NAME_CHANGE = 0x00000004, // Изменение наименования пользователя
                                                                              // контрольно-кассовой техники
                                         //-------------------------------------------------------------------------------------
                                         ADRESS_OR_PLACE_CHANGE = 0x00000008, // Изменение адреса и (или) места установки
                                                                              // (применения) контрольно-кассовой техники
                                         //-------------------------------------------------------------------------------------
                                                 AUTONOMOUS_OFF = 0x00000010, // Перевод ККТ из автономного режима в режим
                                                                              // передачи данных
                                         //-------------------------------------------------------------------------------------
                                                  AUTONOMOUS_ON = 0x00000020, // Перевод ККТ из режима передачи данных
                                                                              // в автономный режим
                                         //-------------------------------------------------------------------------------------
                                           MODEL_VERSION_CHANGE = 0x00000040, // Изменение версии модели ККТ
                                         //-------------------------------------------------------------------------------------
                                             TAX_SYSTEMS_CHANGE = 0x00000080, // Изменение перечня систем налогообложения,
                                                                              // применяемых при осуществлении расчетов
                                         //-------------------------------------------------------------------------------------
                                          AUTOMAT_NUMBER_CHANGE = 0x00000100, // Изменение номера автоматического устройства
                                                                              // для расчетов, в составе которого применяется ККТ
                                         //-------------------------------------------------------------------------------------
                                                  AUTOMATIC_OFF = 0x00000200, // Перевод ККТ из автоматического режима
                                                                              // в неавтоматический режим (осуществление расчетов
                                                                              // кассиром)
                                         //-------------------------------------------------------------------------------------
                                                   AUTOMATIC_ON = 0x00000400, // Перевод ККТ из неавтоматического режима
                                                                              // (осуществление расчетов кассиром)
                                                                              // в автоматический режим
                                         //-------------------------------------------------------------------------------------
                                                         BSO_ON = 0x00000800, // Перевод ККТ из режима, не позволяющего
                                                                              // формировать БСО, в режим, позволяющий формировать
                                                                              // БСО
                                         //-------------------------------------------------------------------------------------
                                                        BSO_OFF = 0x00001000, // Перевод ККТ из режима, позволяющего формировать
                                                                              // БСО, в режим, не позволяющий формировать БСО
                                         //-------------------------------------------------------------------------------------
                                                   INTERNET_OFF = 0x00002000, // Перевод ККТ из режима расчетов в сети Интернет
                                                                              // (позволяющего не печатать кассовый чек и БСО)
                                                                              // в режим, позволяющий печатать кассовый чек и БСО
                                         //-------------------------------------------------------------------------------------
                                                    INTERNET_ON = 0x00004000, // Перевод ККТ из режима, позволяющего печатать
                                                                              // кассовый чек и БСО, в режим расчетов в сети
                                                                              // Интернет (позволяющего не печатать кассовый чек и
                                                                              // БСО)
                                         //-------------------------------------------------------------------------------------
                                                 BANK_AGENT_OFF = 0x00008000, // Перевод ККТ из режима, позволяющего оказывать
                                                                              // услуги платежного агента (субагента) или
                                                                              // банковского платежного агента, в режим,
                                                                              // не позволяющий оказывать услуги платежного агента
                                                                              // (субагента) или банковского платежного агента
                                         //-------------------------------------------------------------------------------------
                                                  BANK_AGENT_ON = 0x00010000, // Перевод ККТ из режима, не позволяющего оказывать
                                                                              // услуги платежного агента (субагента) или
                                                                              // банковского платежного агента в режим,
                                                                              // позволяющий оказывать услуги платежного агента
                                                                              // (субагента) или банковского платежного агента
                                         //-------------------------------------------------------------------------------------
                                                   GAMBLING_OFF = 0x00020000, // Перевод ККТ из режима, позволяющего применять ККТ
                                                                              // при приеме ставок и выплате денежных средств
                                                                              // в виде выигрыша при осуществлении деятельности
                                                                              // по проведению азартных игр, в режим,
                                                                              // не позволяющий применять ККТ при приеме ставок
                                                                              // и выплате денежных средств в виде выигрыша
                                                                              // при осуществлении деятельности по проведению
                                                                              // азартных игр
                                         //-------------------------------------------------------------------------------------
                                                    GAMBLING_ON = 0x00040000, // Перевод ККТ из режима, не позволяющего применять
                                                                              // ККТ при приеме ставок и выплате денежных средств
                                                                              // в виде выигрыша при осуществлении деятельности
                                                                              // по проведению азартных игр, в режим, позволяющий
                                                                              // применять ККТ при приеме ставок и выплате
                                                                              // денежных средств в виде выигрыша
                                                                              // при осуществлении деятельности по проведению
                                                                              // азартных игр
                                         //-------------------------------------------------------------------------------------
                                                    LOTTERY_OFF = 0x00080000, // Перевод ККТ из режима, позволяющего применять ККТ
                                                                              // при приеме денежных средств при реализации
                                                                              // лотерейных билетов, электронных лотерейных
                                                                              // билетов, приеме лотерейных ставок и выплате
                                                                              // денежных средств в виде выигрыша
                                                                              // при осуществлении деятельности по проведению
                                                                              // лотерей, в режим, не позволяющий применять ККТ
                                                                              // при приеме денежных средств при реализации
                                                                              // лотерейных билетов, электронных лотерейных
                                                                              // билетов, приеме лотерейных ставок и выплате
                                                                              // денежных средств в виде выигрыша
                                                                              // при осуществлении деятельности по проведению
                                                                              // лотерей
                                         //-------------------------------------------------------------------------------------
                                                     LOTTERY_ON = 0x00100000, // Перевод ККТ из режима, не позволяющего применять
                                                                              // ККТ при приеме денежных средств при реализации
                                                                              // лотерейных билетов, электронных лотерейных
                                                                              // билетов, приеме лотерейных ставок и выплате
                                                                              // денежных средств в виде выигрыша
                                                                              // при осуществлении деятельности по проведению
                                                                              // лотерей, в режим, позволяющий применять ККТ
                                                                              // при приеме денежных средств при реализации
                                                                              // лотерейных билетов, электронных лотерейных
                                                                              // билетов, приеме лотерейных ставок и выплате
                                                                              // денежных средств в виде выигрыша
                                                                              // при осуществлении деятельности по проведению
                                                                              // лотерей
                                         //-------------------------------------------------------------------------------------
                                             FFD_VERSION_CHANGE = 0x00200000, // Изменение версии ФФД
                                         //-------------------------------------------------------------------------------------
                                                 ANOTHER_REASON = 0x80000000; // Иные причины
                //--------------------------------------------------------------------------------------------------------------
                //                                             СНО
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char
                                                       FS_TOTAL = 0x01,       // Общая
                                           FS_SIMPLIFIED_INCOME = 0x02,       // Упрощенный
                                          FS_SIMPLIFIED_INC_EXP = 0x04,       // Упрощенный доход-расход
                                       FS_SINGLE_IMPUTED_INCOME = 0x08,       // ЕНВД (Единый налог на вменённый доход)
                                        FS_UNIFIED_AGRICULTURAL = 0x10,       // ЕСХН (ЕСН или Единый сельскохозяийственныий налог)
                                                      FS_PATENT = 0x20;       // Патентный
                //--------------------------------------------------------------------------------------------------------------
                //                                   Типы отчёта о регистрации
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned char       ACTIVATION_NEW = 0x00,
                                             ACTIVATION_REPLACE = 0x01,
                                                REG_DATA_CHANGE = 0x02;
                //--------------------------------------------------------------------------------------------------------------
                //                                        Длинна номера ФН
                //--------------------------------------------------------------------------------------------------------------
                static const unsigned             FS_NUMBER_LEN =   16;
                //--------------------------------------------------------------------------------------------------------------
                //                                       Структуры данных ФН
                //--------------------------------------------------------------------------------------------------------------
                //--------------------------------------------------------------------------------------------------------------
                //                                         Версия ПО ФН
                //--------------------------------------------------------------------------------------------------------------
                struct FS_VERSION
                {
                    FS_VERSION();
                    ~FS_VERSION();
                     string version;
                     bool program_type;
                }fs_version;
                //--------------------------------------------------------------------------------------------------------------
                //                                           Формат ФН
                //--------------------------------------------------------------------------------------------------------------
                struct FS_FORMAT
                {
                    FS_FORMAT();
                    ~FS_FORMAT();
                    unsigned char    supported,
                                          used;
                }fs_format;
                //--------------------------------------------------------------------------------------------------------------
                //                                         Ресурс ключей ФН
                //--------------------------------------------------------------------------------------------------------------
                struct FS_VALIDITY
                {
                    FS_VALIDITY();
                    ~FS_VALIDITY();
                    unsigned char      date[5];
                    uint16_t         time_left;
                }fs_validity;
                //--------------------------------------------------------------------------------------------------------------
                //                                         Ресурс памяти ФН
                //--------------------------------------------------------------------------------------------------------------
                struct FS_FREE_MEM
                {
                    FS_FREE_MEM();
                    ~FS_FREE_MEM();
                    uint32_t  data_resource_5Y,
                             data_resource_10Y;
                }fs_free_mem;
                //--------------------------------------------------------------------------------------------------------------
                //                                         Итог фискализации ФФД 1.05
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
                //                                         Итог фискализации ФФД 1.1
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
                //                                         Отчёт о состоянии расчётов
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
                 * @brief The FS_STAGE enum статус ФН
                 */
                enum class FS_STAGE : uint8_t
                {
                    UNKNOWN = 0x00,
                    READY_TO_FISCAL = 0x01, // готов к фискализации
                    FISCAL = 0x02,          // фискальный режим
                    POST_FISCAL = 0x04      // постфискальный режим
                };
                /**
                 * @brief The KKT_MODE enum режим работы ККТ
                 */
                enum KKT_MODE : uint8_t
                {
                    KKT_MODE_NOT_AUTONOMOUS = 0x00,  // Неавтономный
                    KKT_MODE_AUTONOMOUS = 0x01       // Автономный
                };

                //--------------------------------------------------------------------------------------------------------------
            private:
                bool                                      IS_FLK_ERROR,
                                                       IS_NEED_CFG_KKT,
                                                          IS_OFD_CLOSE;
                //--------------------------------------------------------------------------------------------------------------
                //                                           Иные флаги и данные ФН
                //--------------------------------------------------------------------------------------------------------------
                string                                       fs_number; // Номер Фн                                                 ОК
                uint32_t                                   last_fd_num; // Последний ФД                                             ОК
                unsigned long                                fd_number; // ФД
                unsigned long                               fpd_number; // ФПД
                int                                          shift_num; // Номер смены                                              ОК
                int                                        reciept_num; // Номер текущего чека                                      ОК
                bool                                       isShiftOpen; // Смена открыта                                            ОК
                bool                                   isReadyToFiscal; // ФН настроен (готов к фискализации)                      ОК
                bool                                      isFiscalMode; // Фискальный режим                                        ОК
                bool                                  isPostFiscalMode; // Постфискальный режим                                    ОК
                int                                      ofdStage = -1; // Статус ОФД
                int                                 undelegatedDocsCnt; // Непередано ФД в ОФД      *******
                string                              undelegatedDocDate; // Дата первого непереданного в ОФД ФД      *******
                int                             firstUndelegatedDocNum; // Номер первого непереданного ФД в ОФД      *******
                bool                             IMMEDIATE_REPLACEMENT; // Признак необходимости срочной замены ФН (1051)      *******
                bool                              RESOURCES_EXHAUSTION; // Признак исчерпания ресурса ФН (1050)      *******
                bool                                   MEMORY_OVERFLOW; // Признак переполнения памяти ФН (1052)      *******
                bool                                 CRITICAL_FN_ERROR; // 	Критическая ошибка ФН                     *******
                bool                                   fs_replace_mode; // Режим замены ФН
                bool                                   reg_data_change; // Перерегистрация (Смена данных юр. лица)
                bool                                   ofd_data_change; // Перерегистрация (Смена ОФД)
                bool                                fs_change_kkt_mode; // Перерегистрация (Режим ККТ)
                bool                               fs_offline_kkt_mode; // Перерегистрация (Передача в ОФД)
                bool                                      is_shift_24h; // Смена 24 ч                                                ОК
                bool                                          fs_timer; // Таймер ФН
                bool                                         isDocOpen; // Открыт документ в ФН                                     ОК
                bool                                OFD_DISCONNECT_05D; //      *******
                bool                                OFD_DISCONNECT_20D; //      *******
                bool                                OFD_DISCONNECT_30D; //      *******
                bool                                   REG_UNAVAILABLE;
                atomic_bool memoryToOfdExhausted;   // Исчерпание памяти хранения данных для ОФД
                unsigned                             reg_docs_complite;
                //--------------------------------------------------------------------------------------------------------------
                //                                     Методы доступа к флагам
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
                 * @brief getOfdStage получить статус соединения с ОФД
                 * @return статус соединения с ОФД
                 */
                int getOfdStage() {return ofdStage;}
                /**
                 * @brief getUndelegatedDocsCnt получить количество непереданных документов в ОФД
                 * @return количество непереданных документов в ОФД
                 */
                virtual uint32_t getUndelegatedDocsCnt() {return undelegatedDocsCnt;}
                /**
                 * @brief getUndelegatedDocDate получить дату первого непереданного документа в ОФД
                 * @return дата первого непереданного документа в ОФД
                 */
                inline string getUndelegatedDocDate() {return undelegatedDocDate;}

//                /**
//                * @brief addAgentToCheck добавить агента на чек
//                * @param agentType тип агента
//                * @param agents данные агентов
//                * @return результат выполнения
//                */
//                int addAgentToCheck( const Agent &agent) const;        // Добавить агентов на чек

                /**
                * @brief printQRcode - print ascii data like qr code
                * @param data string for conversion
                * @return ok|fail
                */
                bool printQRcode(const std::string& data) const;
                /**
                 * @brief getFirstUndelegatedDocNum получить номер первого непереданного документа в ОФД
                 * @return номер первого непереданного документа в ОФД
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
                //                                          Служебные методы
                //--------------------------------------------------------------------------------------------------------------
                //**************************************************************************************************************
                //                                       Обмен информацией с ФН
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
                                                                     // Вернуть регистрационный номер ФН
                                                                     //     string           fs_number      Регистрационный номер ФН
                //************************************************
                virtual bool fs_status_get                     ();   // 0x02
                                                                     // Вернуть статус ФН
                                                                     // Фаза жизни ФН
                                                                     //     bool       isReadyToFiscal      Готовность к фискализации
                                                                     //     bool          isFiscalMode      Фискальный режим
                                                                     //     bool      isPostFiscalMode      Постфиксальный режим. Идет передача ФД в ОФД
                                                                     // Состояние смены
                                                                     //     bool           isShiftOpen      Состояние смены
                                                                     // Флаги предупреждения
                                                                     //     bool IMMEDIATE_REPLACEMENT      Срочная замена КС
                                                                     //     bool  RESOURCES_EXHAUSTION      Исчерпание ресурса КС
                                                                     //     bool       MEMORY_OVERFLOW      Переполнение памяти ФН
                //************************************************
                virtual bool last_fd_num_ask();                      // 0x03
                                                                     // Вернуть номер последнего фискального документа
                                                                     //      uint32_t       last_fd_num Номер последнего ФД
                //************************************************
                virtual bool getFSshiftParam                        ();   // 0x06
                                                                     // Вернуть состояние текущей смены
                                                                     //     bool           isShiftOpen      Состояние смены
                                                                     //     unsigned short   shift_num      Номер смены
                                                                     //     unsigned short reciept_num      Номер чека в смене
                /**
                 * @brief The OFD_STATUS struct статус ОФД
                 */
                struct OFD_STATUS
                {
                    int stageOFD = -1;          // Состояние ОФД
                    int countUndelegated = -1;  // Документов не передано
                    int firstUndelegated = -1;  // Номер первого не переданного документа
                    string date;                // Дата первого не переданного документа (DDMMYYHHMM)
                };
                /**
                 * @brief getStatusOfd запрос статуса ОФД
                 * @param ofdStatus статусы ОФД
                 * @return результат выполнения
                 */
                bool getStatusOfd(OFD_STATUS &ofdStatus);
                //************************************************
                /**
                 * @brief getOfdDisconnectStatus Вернуть состояние обмена с ОФД
                 * uint32_t undelegatedDocsCnt Количество документов для передачи в ОФД
                 * uint32_t firstUndelegatedDocNum     Номер первого документа для передачи в ОФД
                 * string undelegatedDocDate Дата/время первого док-та для передачи в ОФД (DDMMYYHHMM)
                 * @return результат выполнения
                 */
                virtual bool getOfdDisconnectStatus();
                /**
                 * @brief getFNDateTime вернуть дату и время из ФН
                 * @param tmt время по шкале unix
                 * @return true в случае успеха, иначе false и tmt устанавливается в -1
                 */
                virtual bool getFNDateTime    (Convert::DATE_ISO_8601 &dateIso);
                /**
                 * @brief setFNDateTime - установить дату и время ФН
                 * @param date - дата и время
                 * @return результат выполнения команды
                 */
                virtual bool setFNDateTime(const time_t &date);

                //**************************************************************************************************************
                struct OPEN_DOC
                {
                     OPEN_DOC();
                     ~OPEN_DOC();
                     string       cashier;
                     DOC_TYPES       type;      //  тип открываемого документа
                     DOC_FORMATS      fmt;      //  режим формирования документа
                     DOC_REQ_PRINT    req;      //  режим печати реквизитов
                     int            otdel;      //  Номер отдела
                     SNO_TYPES        sno;      //  Система налогообложения
                };
                /**
                 * @brief The Position struct данные позиции
                 */
                struct Position
                {
                    Position();
                    ~Position() {}

                    PiritLib::Position punix;                       // позиция в punix
                    std::string discountRubName;                    // Название скидки
                    double discountRubSum {.00};                    // Скидка в рублях
                    /*Positions::DISCOUNT_TYPE_BIOS*/int discountType;     // Тип скидки

                    std::string marginRubName;                      // Название надбавки
                    double marginRubSum {.00};                      // Сумма надбавки в рублях
                    std::string  productCode;                       // Код товарной номенклатуры

//                    Agent agent;                                    // Агент для позиции
                };

                //--------------------------------------------------------------------------------------------------------------
                //                                                ФФД
                //--------------------------------------------------------------------------------------------------------------
                bool registerKKT_BIOS                       ( registrationStruct &rs );   // Регистрация / Перерегистрация
                bool setOFDSettings     ( string  urlOFD,         uint32_t  portOFD  );   // Задать настройки ОФД
                bool getOFDSettings      ( string &urlOFD,             int &portOFD  );   // Получить настройки ОФД


                bool getFNVer                                        ( string &fnVer );   // Вернуть версию прошивки ФН
                bool fsTypeGet                                       (    int &      );   // Вернуть тип ФН
                virtual bool closeFN (string cashierName, string& dateTime, uint32_t& fd, uint32_t& fpd);   // Процедура закрытия ФН
                bool closeEmegencyFN                                                ();   // Аварийное закрытие ФН
                bool closeEmegencyShift                                             ();   // Аварийное закрытие смены


                //--------------------------------------------------------------------------------------------------------------
                // Управление расчетами (битовая маска) ККТ.  ПРОЧИТАТЬ
                //--------------------------------------------------------------------------------------------------------------
                bool getCalcMask ( int &mask );
                bool setCalcMask ( int  mask );

                /**
                * @brief writeSettingsTableWrapper - Обертка записи данных в таблицу настроек
                * @param number - Номер
                * @param index  - Индекс (для массивов)
                * @param data   - Новое строковое значение элемента таблицы
                * @return Операция успешно проведена/не проведена
                */
                bool writeSettingsTableWrapper(unsigned char number, int index, const char *data);

                /**
                * @brief setENVDRegData - Задать поля наименование и адрес орг-ии для ЕНВД режима
                * @param orgName - наименование орг-ии
                * @param orgAdress - адрес орг-ии
                * @return Операция успешно проведена/не проведена
                */
                bool setENVDRegData (const string &orgName, const string &orgAdress );
                /**
                 * @brief The RECEIPT struct структура данных фискального чека
                 */
                struct RECEIPT
                {
                    OPEN_DOC doc;                               // DOC_TYPE_SALE            Чек на продажу (приход)
                                                                // DOC_TYPE_SALE_RET        Чек на возврат (возврат прихода)
                                                                // DOC_TYPE_CONSUMPTION     Чек на покупку (расход)
                                                                // DOC_TYPE_CONSUMPTION_RET Чек на возврат покупки (возврат расхода)
                    std::vector<RUS::Position> positions;            // Список позиций
                    PAYMENT payment;                            // Платёж
                    std::string buyerAddress;                   // Адрес покупателя
                    std::string buyerName;                      // Наименование покупателя
                    std::string buyerInn;                       // ИНН покупателя

                    uint32_t fd         = 0;                    // ФД документа
                    uint32_t fpd        = 0;                    // ФПД документа
                    uint32_t shiftNum   = 0;                    // номер смены
                    uint32_t docInShift = 0;                    // номер документа в смене
                    std::string dateDoc;                        // дата документа
                    std::string timeDoc;                        // время документа

                    std::string discountRubName;                // Название скидки на чек
                    double discountRubSum;                      // Скидка в рублях на чек
                    int/*Positions::DISCOUNT_TYPE_BIOS*/ discountType; // Тип скидки

                    std::string marginRubName;                  // Название надбавки на чек
                    double marginRubSum;                        // Сумма надбавки в рублях на чек

//                    Agent            agent;                     // Агент для чека
                    std::string addUserReqName;                 // наименование дополнительного реквизита пользователя
                    std::string addUserReqValue;                // значение дополнительного реквизита пользователя
                    std::string addRecReq;                      // дополнительный реквизит чека (БСО)
                    std::string clcPlace;                       // место расчёта
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
                // Список оплат в чек(смешанная оплата)
                map<int, uint64_t> recPayTypes;
                // Список ДОСТУПНЫХ оплат
                vector<PAY_TYPE>  payTypes;
                // Захардкоженные имена ДОСТУПНЫХ оплат
                const string PAY_TYPE_NAME_CASH     = "НАЛИЧНЫМИ",
                             PAY_TYPE_NAME_CASHLESS = "ЭЛЕКТРОННЫМИ",
                             PAY_TYPE_NAME_ADVANCE  = "ПРЕДВАРИТЕЛЬНАЯ ОПЛАТА (АВАНС)",
                             PAY_TYPE_NAME_CREDIT   = "ПОСЛЕДУЮЩАЯ ОПЛАТА (КРЕДИТ)",
                             PAY_TYPE_NAME_OTHER    = "ИНАЯ ФОРМА ОПЛАТЫ";
                // Прочитать все типы платежей(по факту пока только зарезервированные, соответственно не читаются)
                bool   readPayTypes();

                //--------------------------------------------------------------------------------------------------------------
                // ToDo KPV
                // Данные по чеку
                struct RECEIPT_FIS_DATA
                {
                    int          type       = -1;   // тип чека (для аннулиров. = 0)
                    string       cur_op_cnt = "";   // текущий операц. счетчик
                    int          rec_num    = -1;   // номер чека
                    int          doc_num    = -1;   // номер документа
                    double       sum        = 0.0;  // сумма чека
                    double       discount   = 0.0;  // сумма скидки
                    const double reserved   = 0.0;  // зарезервировано(0)
                    string       fpd        = "";   // строка ФП (фиск. признак)
                    int          fd         = -1;   // номер ФД
                };

                virtual bool rec_fis_data_get(RECEIPT_FIS_DATA&);   // Получение данных по последнему чеку PIRIT_RECEIPT_DATA

                //--------------------------------------------------------------------------------------------------------------
                // Открытие документа на заполнение Фискальный/Сервисный
                //--------------------------------------------------------------------------------------------------------------
                virtual bool openDoc                        ( OPEN_DOC *open_doc );   // Открытие документа
                virtual bool cancelDoc                                          ();  // Аннулирование документа
                //--------------------------------------------------------------------------------------------------------------
                bool openShift                          ( string cashier );   // Открытие смены в БИОС
                bool addExtraReq                   ( const Position &pos );   // Установить дополнительные реквизиты позиции
                bool addPositionToCheck                  ( Position *pos );   // Добавить позицию в открытый документ
                bool subTotal                                           ();   // Подытог
                bool addDiscountRubles    ( string          nameDiscount ,
                                            uint64_t                 sum );   // Добавить скидку в рублях/процентах на чек
                bool addMarginRubles      ( string nameMargin,   int sum );   // Добавить наценку в рублях на чек
                /**
                 * @brief closeDoc завершить предзаполненный документ
                 * @param buyerAddress адрес покупателя
                 * @return результат выполнения
                 */
                bool closeDoc(const string& buyerAddress);
                /**
                 * @brief closeDoc завершить предзаполненный документ
                 * @param receipt структура чека
                 * @return результат выполнения
                 */
                bool closeDoc(RUS::RECEIPT& receipt);
                bool closeShift                         ( string cashier );   // Закрытие смены в БИОС + Z отчёт
                bool calcStateReport               ( string cashier_name );   // Отчёт о текущем состоянии расчётов
                bool insRes                  ( OPEN_DOC *doc, double sum );   // Внесение / изъятие суммы
                bool closeFS                            ( string cashier );   // Закрытие ФН
                virtual bool getDateOfReg           ( string  &dateTime  );   // Вернуть дату и время регистрации
                virtual bool getFDandFPDofReg (int           &numberOfFD ,
                                       uint32_t                  &FPDnum );  // Получить номер ФД и ФПД регистрации/перерегистрации через выгрузку ТЛВ из ФН
                        bool  addSTLVheader (std::vector <uint8_t>  &dest,
                                                  const  uint16_t    type,
                                                  const  uint16_t     len) const;  // insert 4 bytes LE <тип><тип><длина><длина>
                virtual bool getSTLVofReg      (vector <uint8_t>& stlvReg,
                                                int&               typeFd);  // Получить STLV  регистрации/перерегистрации
                bool fiscalDocBinUnload              (  int     fd_number,   // Выгрузка документа по номеру из ФН через БИОС
                                                        char   **doc_data,
                                                          int       &size,
                                                          bool   &isCheck,
                                                          int    &docType);
                /**
                 * @brief setPosAddReq - установить дополнительный реквизит позиции
                 * @param requisite - реквизит
                 * @return - результат выполнения команды
                 */
                bool setPosAddReq(string requisite);
                //--------------------------------------------------------------------------------------------------------------
                int fs_reset();
                //-------------------------------------------------------------------------------------------------------------



                //--------------------------------- Дримкас Ключ ---------------------------------------------------------
                /**
                 * @brief dreamkasKeyActivate
                 * @param key
                 * @return true в случае успеха, иначе false
                 */
                bool dreamkasKeyActivate(string &key);
                /**
                 * @brief dreamkasKeyGetServiceInfo
                 * @param license - структура услуги
                 * @param index - номер индекса
                 * @return true в случае успеха, иначе false
                 */
                bool dreamkasKeyGetServiceInfo(License::LicenseTag &license, const uint8_t &index);
                /**
                 * @brief dreamkasKeyGetServiceInfoList
                 * @param license - вектор услуг
                 * @return true в случае успеха, иначе false
                 */
                bool dreamkasKeyGetServiceInfoList(vector<License::LicenseTag> &license);
                /**
                 * @brief dreamkasKeyPrintAll - печать всех доступных услуг
                 * @return true в случае успеха, иначе false
                 */
                bool dreamkasKeyPrintAll();
                //--------------------------------------------------------------------------------------------------------
//                /**
//                 * @brief setNVR запись данных в NVR
//                 * @param nvr структура NVR
//                 * @return результат выполненя
//                 */
//                bool setNVR(NVR &nvr);
                /**
                 * @brief getDocSTLV - Вернуть реквизиты документа из ФН по номеру
                 * @param numberOfFD - номер документа
                 * @param reqList - вектор реквизитов из ФН
                 * @return результат расчётов
                 */
                bool getDocSTLV(const int      &numberOfFD, vector<Requisites::Requisite> &reqList);
                /**
                * @brief getDocSTLV - Вернуть stlv документа из ФН по номеру
                * @param numberOfFD - номер документа
                * @param stlv       - stlv документа
                * @param typeFd     - тип документа
                * @return результат расчётов
                */
                bool getDocSTLV(const uint32_t   &numberOfFD,
                                vector <uint8_t>       &stlv,
                                int&                  typeFd);

                /**
                 * @brief getLastFdNum - Вернуть номер последнего ФД
                 * @return номер последнего ФД
                 */
                virtual uint32_t getLastFdNum() const;

                /**
                 * @brief getExtendedErrorCode - вернуть сообщение расширенного кода ошибки
                 * @return - строка с сообщением об ошибке
                 */
                string getExtendedErrorCode();

                /**
                 * @brief getMemoryToOfdExhausted - получения флага исчерпания памяти хранения данных для ОФД
                 * @return true - флаг установлен, иначе - false
                 */
                virtual bool getMemoryToOfdExhausted() const {return memoryToOfdExhausted.load();}

                /**
                 * @brief setMemoryToOfdExhausted - установить флаг исчерпания памяти хранения данных для ОФД
                 * @param value - значение флага
                 */
                void setMemoryToOfdExhausted(const bool &value) {memoryToOfdExhausted.store(value);}
                /**
                 * @brief getLastRegInfo - Запрос данных последней регистрации/перерегистрации
                 * @param fd - fd регистрации/перерегистрации
                 * @param fpd - fpd регистрации/перерегистрации
                 * @return результат
                 */
                bool getLastRegInfo   (uint32_t &fd, uint32_t &fpd );

        protected:
                /**
                 * @brief fillErrorCodes - заполнение локальных переменных из кодов ошибок
                 * @param code - код ошибки
                 */
                void fillErrorCodes(const FB_ERROR_CODE &code);

        private:


            /**
             * @brief getNumFDofReg - Вернуть номер ФД последней регистрации
             * @param fdNumberReg - номер ФД последней регистрации
             * @return результат расчётов
             */
            bool getNumFDofReg    ( int &fdNumberReg );
            bool tlv_parse        ( unsigned char  *tlv, vector <Requisites::Requisite> *requisites, uint16_t &retOffset);
            bool stlv_parse       ( unsigned char *stlv, vector <Requisites::Requisite> *requisites );
            //--------------------------------------------------------------------------------------------------------------
        };
        shared_ptr<RUS> RF;
        virtual bool receipt_send_rf_bios (RUS::RECEIPT *receipt , Config &config);                  // Отправка чека в BIOS RF
    //======================================================================================================================
    //======================================================================================================================
    //======================================================================================================================
    //======================================================================================================================
    //                                                     РБ
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
        bool       isConnected;                                          // Флаг соединения
        unique_ptr<Convert> convert;

        //--------------------------------------------------------------------------------------------------------------
        // Тип товара
        static const string NOT_GTIN, // Штрих-код
                                GTIN, // Артикул
                             SERVICE; // Услуга

        //----------------------------------------------------------------------------------------------------------
        struct OPEN_DOC
        {
            OPEN_DOC();
            ~OPEN_DOC();
            string       cashier;
            DOC_TYPES_BY    type;      //  тип открываемого документа
            DOC_FORMATS      fmt;      //  режим формирования документа
            DOC_REQ_PRINT    req;      //  режим печати реквизитов
            int            otdel;      //  Номер отдела
        };

        //----------------------------------------------------------------------------------------------------------
        struct POSITION
        {
            POSITION();
            ~POSITION();
            string                          name; // (Строка[0...224]) Название товара
            string                       barcode; // (Строка[0..18]) Артикул или штриховой код товара/номер ТРК
            double                      quantity; // (Дробное число) Количество товара в товарной позиции
            double                         price; // (Дробное число) Цена товара по данному артикулу
            unsigned char              taxNumber; // (Целое число) Номер ставки налога
            int                      numGoodsPos; // (Строка[0..4]) Номер товарной позиции
            unsigned char              numDepart; // (Целое число 1..16) Номер секции

            string               discountRubName; // Название скидки
            uint64_t              discountRubSum; // Скидка
            /*Positions::DISCOUNT_TYPE_BIOS*/int      discountType; // Тип скидки

            string                 marginRubName; // Название надбавки
            uint64_t                marginRubSum; // Сумма надбавки
        };

        //----------------------------------------------------------------------------------------------------------
        struct RECEIPT
        {
            // DOC_TYPE_SALE                Чек на продажу (приход)
            // DOC_TYPE_SALE_RET            Чек на возврат (возврат прихода)
            // DOC_TYPE_CONSUMPTION         Чек на покупку (расход)
            // DOC_TYPE_CONSUMPTION_RET     Чек на возврат покупки (возврат расхода)
            OPEN_DOC                  doc;
            vector <POSITION>   positions;
            PAYMENT               payment;
            string           buyerAddress;

            string               discountRubName; // Название скидки на чек
            uint64_t              discountRubSum; // Скидка
            /*Positions::DISCOUNT_TYPE_BIOS*/int      discountType; // Тип скидки

            string                 marginRubName; // Название надбавки на чек
            uint64_t                marginRubSum; // Сумма надбавки
        };

        map<int, uint64_t> recPayTypes;    // Список оплат в чек(смешанная оплата)

        //--------------------------------------------------------------------------------------------------------------
        //                                               НДС
        //--------------------------------------------------------------------------------------------------------------
        struct NDS
        {
            string  name; // Название ставки налога (строка длиной 0..18 символов)
            double value; // Процент ставки налога (значение процента с точностью до 4-х знаков после запятой)
        };
        enum TAX
        {
            NDS_20 = 0, // 20%
            NDS_10,     // 10%
            NDS_16_67,  // 16,67%
            NDS_9_09,   // 9,09%
            NDS_0       // 0%
        };

        bool ndss_read();                                       //Считать все НДСы 0..5
        bool nds_read( int i, NDS &nds );
        bool ndss_write(int i = 0);                             // Записать i-й элемент массива 0...5
        bool nds_write(int i, NDS nds);                        // Записать i-й элемент массива 0...5
        inline vector<NDS>*   ndss_get() { return     &NDSS; }
        /**
         * @brief getNDSTableSize получить размер таблицы заполненных НДС
         * @return размер таблицы заполненных НДС
         */
        int getNDSTableSize();
        //----------------------------------------------------------------------------------------------------------
        //  Названия Отделов (массив из 16 в биос)
        //----------------------------------------------------------------------------------------------------------
        bool   dep_name_write      ( string name, int num = 1);   // Задать   название отдела, по умолчанию 1-го (1..16) ИЗ БИОС
        bool   dep_name_read       ( int num, string &name   );   // Получить название отдела, по умолчанию 1-го (1..16) ИЗ БИОС
        struct DEP
        {
            uint8_t  num;
            string  name;
        };
        vector<DEP>  DEPS;
        static const u_int8_t DEFAULT_DEPS_CNT = 4;
        bool   deps_read();                                       // Считать все секции
        inline vector<DEP>*   deps_get() { return     &DEPS; }

        struct PAY_TYPE
        {
            uint8_t  num =  0;
            string  name;
        };
        vector<PAY_TYPE>  payTypes;

        bool   writePayType      ( const int num, const  string &name );   // Задать   название типа платежа, по умолчанию 1-го ( 0..15 ) ИЗ БИОС
        bool   readPayType       ( const int num,        string &name );   // Получить название типа платежа, по умолчанию 1-го ( 0..15 ) ИЗ БИОС

        // Прочитать все типы платежей
        bool   readPayTypes();
        //--------------------------------------------------------------------------------------------------------------
        //                                  ФЛАГИ     КСА
        //--------------------------------------------------------------------------------------------------------------

        // Статус фатального состояния КСА
        enum KCA_FATAL_FLGS
        {
            WRONG_NVR_SUM      =  1,       // Неверная контр сумма НВР
            WRONG_CONF_SUM     =  2,       // Неверная контр сумма конфигурции
            BEP_IFACE_ERROR    =  4,       // Ошибка интерфейса БЭП
            WRONG_BEP_SUM      =  8,       // Неверная контр сумма БЭП
            BEP_WRITE_ERR      = 16,       // Ошибка записи в БЭП
            BEP_NOT_AUTH       = 32,       // БЭП не авторизован
            KL_MEM_ERR         = 64        // Фатальная ошибка памяти контрольной ленты
        };

        // Статус текущих флагов КСА
        enum KCA_CUR_FLGS
        {
            START_FAILED        =   1, // Не была вызвана функция ?Начало работы?
            STUDY_MODE          =   2, // Учебный режим
            SHIFT_OPEN          =   4, // Смена открыта
            SHIFT_24H           =   8, // Смена больше 24 часов
            BUFFER_IS_NOT_EMPTY =  16, // Присутствуют данные в буфере контрольной ленты (буфер не пустой)
            RESERVED_1          =  32, // Зарезервирован
            CLOSE_SHIFT_MEM_ERR =  64, // Нет памяти для закрытия смены в БЭП
            RESERVED_2          = 128, // Зарезервирован
            CLOSE_SHIFT_ERR     = 256  // Не было завершено закрытие смены, необходимо повторить операцию
        };

        // Тип открытого документа
        enum KCA_DOC_TYPE
        {
            KCA_TYPE_CLOSED          =  1,    // Документ закрыт
            KCA_TYPE_SERVICE         =  2,    // Сервисный документ
            KCA_TYPE_SALE            =  3,    // Чек на продажу
            KCA_TYPE_RETURN          =  4,    // Чек на возврат
            KCA_TYPE_INJECT          =  5,    // Внесение в кассу
            KCA_TYPE_RESERVE         =  6,    // Инкассация
            KCA_TYPE_RECIEPT_ORDER   =  7,    // Чек оформление заказа
            KCA_TYPE_RECIEPT_CLOSE   =  8,    // Чек закрытия заказа
            KCA_TYPE_RECIEPT_CANCEL  =  9,    // Чек отмены заказа
            KCA_TYPE_RECIEPT_ADDING  = 10,    // Чек добавления заказа
            KCA_TYPE_RECIEPT_CORRECT = 11,    // Чек коррекции заказа
            KCA_TYPE_RECIEPT_INVOCIE = 12     // Счет на оплату
        };

        // Состояние документа
        enum KCA_DOC_STATE
        {
            KCA_STATE_CLOSED              = 0,   // Документ закрыт
            KCA_STATE_OPENED              = 1,   // Устанавливается после команды ?открыть документ?
            KCA_STATE_FIRST_PODITIGED     = 2,   // Устанавливается после первой команды ?Подитог?. Можно делать скидки/наценки на чек
            KCA_STATE_SECOND_PODITIGED    = 3,   // Устанавливается после второй команды ?Подитог? или после начала команды ?Оплата?
            KCA_STATE_NEED_CLOSE          = 4    // Расчет завершен ? требуется закрыть документ.
        };

        enum KCA_SCNO_STATE
        {
            KCA_SCNO_OK          =  0,    // Всё ОК
            KCA_SCNO_COMMON_ERR  =  1,    // Общая ошибка СКНО
            KCA_SCNO_CONNECT_ERR =  2,    // Отсутствует связь с СКНО
            KCA_SCNO_NO_SKZI     =  4,    // Отсутствует СКЗИ
            KCA_SCNO_BAD_SCNO    =  8,    // Неисправно СКНО
            KCA_SCNO_IDENT_ERR   = 16,    // Идентификация не прошла успешно
            KCA_SCNO_SERTIF_ERR  = 32,    // Запрет обслуживания по окончанию сертификата СКЗИ
            KCA_SCNO_Z_REP_ERR   = 64     // Запрет обслуживания по непереданным Z-отчетам

        };
        /**
         * @brief ksaInfGet - Запрос сведений о КСА
         * @return true - успех
         */
        template<typename T>
        bool ksaInformationGet( KSA_INFORMATION param, T &value )
        {
            lock_guard<mutex> locker(*mutex_rb);
            int res = ksaInfGet<T>( KSA_INF_NEXT_DOC_NUM, value );
            bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
            if( !ok )
            {
                logERR( "kca_flgs_get():: Ошибка! результат команды libGetStatusFlags_By()  = %d ", res );
                return false;
            }

            return true;
        }
        /**
         * @brief getNextDocNum чтение сквозного номера следующего документа
         * @return сквозной номер следующего документа
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
        // Запрос флагов КСА
        //----------------------------------------------------------------------------------------------------------
        bool   kca_flgs_get ();          //Запрос флагов КСА из БИОС

        // Интерфейс для Статусов тек флагов КСА
        inline bool   is_start_failed_get         (                        ) { return        is_start_failed; }
        inline bool   is_study_mode_get           (                        ) { return          is_study_mode; }
        inline bool   isShiftOpen_get             (                        ) { return          is_shift_open; }
        inline void   is_shift_open_set           (   bool isShiftOpen     ) {   is_shift_open = isShiftOpen; }
        inline bool   is_shift_24h_get            (                        ) { return           is_shift_24h; }
        inline void   is_shift_24h_set            (   bool isShift24h      ) {     is_shift_24h = isShift24h; }
        inline bool   is_buffer_is_not_empty_get  (                        ) { return is_buffer_is_not_empty; }
        inline bool   is_close_shift_mem_err_get  (                        ) { return is_close_shift_mem_err; }
        inline bool   is_clos_shift_err_get       (                        ) { return     is_close_shift_err; }
        // Интерфейс для Статусов фатального состояния КСА
        inline bool   is_wrong_NVR_sum_get       (                        ) { return        is_wrong_NVR_sum; }
        inline bool   is_wrong_conf_sum_get      (                        ) { return       is_wrong_conf_sum; }
        inline bool   is_BEP_err_get             (                        ) { return              is_BEP_err; }
        inline bool   is_wrong_BEP_sum_get       (                        ) { return        is_wrong_BEP_sum; }
        inline bool   is_BEP_write_err_get       (                        ) { return        is_BEP_write_err; }
        inline bool   is_BEP_not_auth_get        (                        ) { return         is_BEP_not_auth; }
        inline bool   is_KL_mem_err_get          (                        ) { return           is_KL_mem_err; }

        inline bool   isDocOpenedGet             (                        ) { return             isDocOpened; }
        //----------------------------------------------------------------------------------------------------------
        // Фискальные операции
        //----------------------------------------------------------------------------------------------------------
        bool openShift                   (                              );   // Открытие смены в БИОС
        virtual bool openDoc                     (    OPEN_DOC *open_doc        );   // Открытие документа
        bool addPositionToCheck          (    POSITION *pos             );   // Добавить позицию в открытый документ
        virtual bool subTotal                                                  ();   // Подытог
        bool addDiscountRubles           ( string            nameDiscount,
                                           uint64_t                   sum,
                                           /*Positions::DISCOUNT_TYPE_BIOS*/int    discType = /*Positions::DISCOUNT_TYPE_ABSOLUTE*/0 );   // Добавить скидку на чек/позицию
        bool addMarginRubles             ( string nameMargin,   uint64_t sum );   // Добавить наценку на чек/позицию
        bool closeDoc                    (                              );   // Завершить предзаполненный документ
        bool insRes                      (    OPEN_DOC *doc, double sum );   // Внесение / изъятие суммы
        bool closeShift                  (    string cashier            );   // Закрытие смены
        bool cancelDoc                   (                              );   // Аннулирование документа
        bool cancelDocBel                ( const uint32_t&, string, double (&arr)[MAX_PAY_TYPES] );   // Аннулирование документа
        virtual bool curShiftNum                 (    uint64_t &shiftNum        );   // Получить номер текущей смены
        /**
         * @brief getDocFromKL чтение документа из КЛ
         * @param docNum номер документа
         * @param doc документ из КЛ
         * @return результат выполнения
         */
        bool getDocFromKL(const uint32_t &docNum, DocFromKL &doc);
        // Данные по чеку
        struct RECEIPT_FIS_DATA
        {
            int          type       = -1;   // тип чека (для аннулиров. = 0)
            string       cur_op_cnt = "";   // текущий операц. счетчик
            int          rec_num    = -1;   // номер чека
            int          doc_num    = -1;   // номер документа
            double       sum        = 0.0;  // сумма чека
            double       discount   = 0.0;  // сумма скидки
            double       markup     = 0.0;  // сумма наценки
        };

        virtual bool rec_fis_data_get( RECEIPT_FIS_DATA&);  // Получение данных по последнему чеку PIRIT_RECEIPT_DATA

        bool print_report_fixed_mem_unit(string, string, string); // Печать отчета по данным из БЭП
        bool print_report_ctrl_tape();            // Распечатать буфер контрольной ленты

    private:
        vector<NDS>        NDSS; // Вектор из 6-ти возможных НДС на позицию
        //----------------------------------------------------------------------------------------------------------
        //                                     ФЛАГИ КСА
        //----------------------------------------------------------------------------------------------------------
        // Статус тек флагов КСА
        bool    is_start_failed,        // Не была вызвана функция ?Начало работы?
                is_study_mode,          // Учебный режим
                is_shift_open,          // Смена открыта
                is_shift_24h,           // Смена больше 24 часов
                is_buffer_is_not_empty, // Присутствуют данные в буфере контрольной ленты (буфер не пустой)
                is_close_shift_mem_err, // Нет памяти для закрытия смены в БЭП
                is_close_shift_err;     // Не было завершено закрытие смены, необходимо повторить операцию
        // Статусы состояния документа КСА
        bool    isDocOpened;
        // Статус фатального состояния КСА
        bool    is_wrong_NVR_sum,       // Неверная контр сумма НВР
                is_wrong_conf_sum,      // Неверная контр сумма конфигурции
                is_BEP_err,             // Ошибка интерфейса БЭП
                is_wrong_BEP_sum,       // Неверная контр сумма БЭП
                is_BEP_write_err,       // Ошибка записи в БЭП
                is_BEP_not_auth,        // БЭП не авторизован
                is_KL_mem_err;          // Фатальная ошибка памяти контрольной ленты

        KCA_DOC_TYPE     doc_type;      // Тип    открытого Документа
        KCA_DOC_STATE   doc_state;      // Статус открытого Документа
        KCA_SCNO_STATE scno_state;      // Состояние СКНО 0(KCA_SCNO_OK) - всё ок
    public:
        //----------------------------------------------------------------------------------------------------------
        //                                          Работа с КЛ
        //----------------------------------------------------------------------------------------------------------
        bool KLDocRead ( int doc_num, vector<string> &strs );
        bool KLDocPrint( int doc_num );


        /**
         * @brief getSKNODateTime вернуть дату и время из СКНО
         * @param tmt время по шкале unix
         * @return true в случае успеха, иначе false и tmt устанавливается в -1
         */
        virtual bool getSKNODateTime    (Convert::DATE_ISO_8601 &dateIso);
        /**
         * @brief getScno_state получение статуса работы с СКНО
         * @return scno_state
         */
        KCA_SCNO_STATE getScno_state() const;
    };
    virtual bool receipt_send_rb_bios( BEL::RECEIPT *receipt );                  // Отправка чека в BIOS RB
    //----------------------------------------------------------------------------------------------------------
    shared_ptr<BEL> RB;
    //======================================================================================================================
    //======================================================================================================================
    //======================================================================================================================
};

#endif //FISCAT_FISGO_BIOS_H
