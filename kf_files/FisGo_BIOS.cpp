//
// Created by krazh on 23.01.18.
//
#include <time.h>

#include "FisGo_BIOS.h"
#include "loggerdaemon.h"

bool          fb_wasErr     = false;    // Флаг для BackgroundEvents, говорящий, что произошла крит. ошибка при выполнении команды в пирит
string        fb_errMess    =    "";
int           fb_errCode    =     0;
bool          fb_wasWarning = false;    // Флаг для BackgroundEvents, говорящий, что команда выполнена с ворнингом
string        fb_warnMess   =    "";
int           fb_warnCode   =     0;

const string BIOS::BEL::NOT_GTIN = "A";
const string BIOS::BEL::GTIN     = "B";
const string BIOS::BEL::SERVICE  = "C";


//=====================================================================
BIOS::BIOS()
{
    ip          =   "127.0.0.1";
    port        =         50005;
    isConnected.store(false);

    RF = make_shared<RUS>();
    RF->mutex_set( &mutex_bios );

    RB =  make_shared<BEL>();
    RB->mutex_set( &mutex_bios );

    fb_wasErr =     false;
    fb_errMess.   clear();
    fb_errCode =        0;
    fb_wasWarning = false;
    fb_warnMess.  clear();
    fb_warnCode =       0;

    convert.reset( new Convert() );

}

BIOS::~BIOS()
{

}
//=====================================================================
BIOS::RUS::FS_VERSION::FS_VERSION()
{
    version.     clear();
    program_type = false;
}
//=====================================================================
BIOS::RUS::FS_VERSION::~FS_VERSION()
{

}
//=====================================================================
BIOS::RUS::FS_FORMAT::FS_FORMAT()
{
    supported = 0x00;
    used      = 0x00;
}
//=====================================================================
BIOS::RUS::FS_FORMAT::~FS_FORMAT()
{

}
//=====================================================================
BIOS::RUS::FS_VALIDITY::FS_VALIDITY()
{
    memset( date, 0x00, sizeof(date) );
    time_left                      = 0;
}
//=====================================================================
BIOS::RUS::FS_VALIDITY::~FS_VALIDITY()
{
    memset( date, 0x00, sizeof(date) );
    time_left                      = 0;
}
//=====================================================================
BIOS::RUS::FS_FREE_MEM::FS_FREE_MEM()
{
    data_resource_5Y  = 0;
    data_resource_10Y = 0;
}
//=====================================================================
BIOS::RUS::FS_FREE_MEM::~FS_FREE_MEM()
{

}
//=====================================================================
BIOS::RUS::RUS()
{

    convert.reset( new Convert );

    isConnected               =       false;
    IS_FLK_ERROR              =       false;
    IS_NEED_CFG_KKT           =       false;
    IS_OFD_CLOSE              =       false;
    fs_number.                      clear();
    last_fd_num               =           0;
    fd_number                 =           0;
    fpd_number                =           0;
    shift_num                 =           0;
    reciept_num               =           0;
    isShiftOpen               =       false;
    isReadyToFiscal           =       false;
    isFiscalMode              =       false;
    isPostFiscalMode          =       false;
    undelegatedDocsCnt        =           0;
    firstUndelegatedDocNum    =           0;
    IMMEDIATE_REPLACEMENT     =       false;
    imm_rep_en                =        true;
    RESOURCES_EXHAUSTION      =       false;
    res_exh_en                =        true;
    MEMORY_OVERFLOW           =       false;
    mem_over_en               =        true;
    CRITICAL_FN_ERROR         =       false;
    crit_fn_err_en            =        true;
    fs_replace_mode           =       false;
    reg_data_change           =       false;
    ofd_data_change           =       false;
    fs_change_kkt_mode        =       false;
    fs_offline_kkt_mode       =       false;
    is_shift_24h              =       false;
    fs_timer                  =       false;
    isDocOpen                 =       false;
    OFD_DISCONNECT_05D        =       false;
    OFD_DISCONNECT_20D        =       false;
    OFD_DISCONNECT_30D        =       false;
    REG_UNAVAILABLE           =       false;

    memoryToOfdExhausted.store(false);
}
//=====================================================================
BIOS::RUS::~RUS()
{

}

void BIOS::RUS::powerOnFS()
{
#ifdef SPUTNIK_F
    logWARN("Включение питания ФН (Спутник Ф)");
    std::system("echo 164 > /sys/class/gpio/export");
    std::system("echo out > /sys/class/gpio/gpio164/direction");
    std::system("echo 1 > /sys/class/gpio/gpio164/value");
    std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
}

uint32_t BIOS::RUS::getLastFdNum() const
{
    return last_fd_num;
}

string BIOS::RUS::getExtendedErrorCode()
{
    MData result = libGetExErrorInfo(PIRIT_PARAM_1);
    if (isCommandComplete(result.errCode, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL))
    {
        return Convert::charToString(result.data, sizeof(result.data));
    }

    return {};
}

//=====================================================================
void BIOS::RUS::info( FS_INFO mask )
{
    string          FLG,
            YES = "YES",
             NO =  "NO";

    logDBG( "<<<<<<<<<<<<<<<<<<<RF INFO>>>>>>>>>>>>>>>>>>>" );

    if(mask & FS_NUMBER)
    {
        logDBG( "RF NUMBER %s", fs_number.c_str() );
    }
    if(mask & LAST_FD_NUMBER)
    {
        logDBG( "LAST FD NUMBER %lu", fd_number );
    }
    if(mask & SHIFT_NUM)
    {
        logDBG( "SHIFT NUMBER %u", shift_num );
    }
    if(mask & IS_READY_TO_FISCAL)
    {
        (isReadyToFiscal) ? FLG = YES : FLG = NO ;
        logDBG( "IS READY TO FISCAL %s", FLG.c_str() );
    }
    if(mask & IS_FISCAL_MODE)
    {
        (isFiscalMode) ? FLG = YES : FLG = NO ;
        logDBG( "IS FISAL MODE %s", FLG.c_str() );
    }
    if(mask & IS_POST_FISCAL_MODE)
    {
        (isPostFiscalMode) ? FLG = YES : FLG = NO ;
        logDBG( "IS POST FISAL MODE %s", FLG.c_str() );
    }
    if(mask & IS_DOC_OPEN)
    {
        (isDocOpen) ? FLG = YES : FLG = NO ;
        logDBG( "IS DOC OPEN %s", FLG.c_str() );
    }
    if(mask & IS_SHIFT_24H)
     {
        (is_shift_24h) ? FLG = YES : FLG = NO ;
        logDBG( "IS SHIFT 24H %s", FLG.c_str() );
    }
    if(mask & IS_CONNECTED)
    {
        (isConnected) ? FLG = YES : FLG = NO ;
        logDBG( "IS CONNECT %s", FLG.c_str() );
    }
    if(mask & RECEIPT_NUM)
    {
        logDBG( "RECEIPT NUM %d", reciept_num );
    }
    if(mask & IS_SHIFT_OPEN)
    {
        (isShiftOpen) ? FLG = YES : FLG = NO ;
        logDBG( "IS SHIFT OPEN %s", FLG.c_str() );
    }
    if(mask & IMMEDIATE_REPLACEMENT_FLG)
    {
        (IMMEDIATE_REPLACEMENT) ? FLG = YES : FLG = NO ;
        logDBG( "IMMEDIATE REPLACEMENT %s", FLG.c_str() );
    }
    if(mask & RESOURCES_EXHAUSTION_FLG)
    {
        (RESOURCES_EXHAUSTION) ? FLG = YES : FLG = NO ;
        logDBG( "RESOURCES EXHAUSTION %s", FLG.c_str() );
    }
    if(mask & MEMORY_OVERFLOW_FLG)
    {
        (MEMORY_OVERFLOW) ? FLG = YES : FLG = NO ;
        logDBG( "MEMORY OVERFLOW FLG %s", FLG.c_str() );
    }
    if(mask & UNDELEGATED_DOCS_CNT)
    {
        logDBG( "UNDELEGATED DOCS CNT %u", undelegatedDocsCnt );
    }
    if(mask & firstUndelegatedDocNum)
    {
        logDBG( "FIRST UNDELEGATED DOC NUM %u", firstUndelegatedDocNum );
    }
    if(mask & FIRST_UNDELEGATED_DOC_DATE)
    {
        logDBG( "FIRST UNDELEGATED DOC DATE %s", undelegatedDocDate.c_str() );
    }
    if(mask & OFD_DISCONNECT_05D_FLG)
    {
        (OFD_DISCONNECT_05D) ? FLG = YES : FLG = NO ;
        logDBG( "OFD DISCONNECT 05D %s", FLG.c_str() );
    }
    if(mask & OFD_DISCONNECT_20D_FLG)
    {
        (OFD_DISCONNECT_20D) ? FLG = YES : FLG = NO ;
        logDBG( "OFD DISCONNECT 20D %s", FLG.c_str() );
    }
    if(mask & OFD_DISCONNECT_30D_FLG)
    {
        (OFD_DISCONNECT_30D) ? FLG = YES : FLG = NO ;
        logDBG( "OFD DISCONNECT 30D %s", FLG.c_str() );
    }

    logDBG( "<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>" );
}
//=====================================================================
bool BIOS::RUS::getStatusOfd(OFD_STATUS &ofdStatus)
{
    #ifdef BIOS_RB
        return true;
    #endif

    logINFO("%s", __PRETTY_FUNCTION__);
    lock_guard<mutex> locker(*mutex_rf);
    int rv(-1);
    char date[100] = {};

    if (!convert)
    {
        logERR("%s, convert == nullprt!!!", __PRETTY_FUNCTION__);
        return false;
    }
    else if (!isFiscalMode_get())
    {
        logERR("%s\t\tKKT IS NOT FISCAL", __PRETTY_FUNCTION__);
        return false;
    }
    else
    {
        rv = libGetStatusOFD(&ofdStatus.stageOFD, &ofdStatus.countUndelegated, &ofdStatus.firstUndelegated, date);
    }

    bool ok = (rv == 0) ?  true :  isCommandComplete(rv, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logINFO("%s\t\t%sВЫПОЛНЕНО", (ok ? " " : "НЕ "), __PRETTY_FUNCTION__);
    if (ok)
    {
         ofdStatus.date = convert->charToString(date, sizeof(date));
         logDBG("%s\t\tOFD stage\t\t%d", __PRETTY_FUNCTION__, ofdStatus.stageOFD);
         logDBG("%s\t\tUndelegated doc count\t\t%d", __PRETTY_FUNCTION__, ofdStatus.countUndelegated);
         logDBG("%s\t\tFirst undelegated doc numbert\t\t%d", __PRETTY_FUNCTION__, ofdStatus.firstUndelegated);
         logDBG("%s\t\tFirst undelegated doc date\t\t%s", __PRETTY_FUNCTION__, ofdStatus.date.c_str());
    }

    return ok;
}
//=====================================================================
bool BIOS::RUS::getOfdDisconnectStatus()
{
    if (!convert)
    {
        logERR("%s, convert == nullprt!!!", __PRETTY_FUNCTION__);
        return false;
    }

    #ifdef BIOS_RB
        return true;
    #endif
    lock_guard<mutex> locker(*mutex_rf);

    logINFO("OFD DISCONNECT STATUS");

    if (!isFiscalMode_get())
    {
        logERR("%s KKT IS NOT FISCAL", __PRETTY_FUNCTION__);
        return false;
    }

    char dateStr[100];
    int res = libGetStatusOFD(&ofdStage, &undelegatedDocsCnt, &firstUndelegatedDocNum, dateStr);
    bool ok = (res == 0) ?  true : isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    OFD_DISCONNECT_05D = false;
    OFD_DISCONNECT_20D = false;
    OFD_DISCONNECT_30D = false;
    REG_UNAVAILABLE    = false;

    logINFO("getOfdDisconnectStatus() %sВЫПОЛНЕНО. dateStr = '%s'",(ok ? " " : "НЕ "), dateStr);

    if (ok)
    {
        undelegatedDocDate     = dateStr;
        undelegatedDocDate     += "0000";
        string dateCur         = convert->time_str_get(convert->cur_unixtime_get());

        logERR( "UNDELEGATES DOCS COUNT %d", undelegatedDocsCnt );
        if(undelegatedDocsCnt == 0)
        {
            logDBG("NOTHING SEND TO OFD");
            return true;
        }

        logDBG("UNDELEGATED DOC DATE %s", undelegatedDocDate.c_str());
        logDBG("CURRENT DATE %s", dateCur.c_str());

        if (Convert::date_difference(dateCur, undelegatedDocDate, D05))
        {
            logERR("OFD_DISCONNECT_05D!!!");
            OFD_DISCONNECT_05D = true;
        }
        if (Convert::date_difference(dateCur, undelegatedDocDate, D20))
        {
            logERR("OFD_DISCONNECT_20D!!!");
            OFD_DISCONNECT_20D = true;
        }
        if (Convert::date_difference(dateCur, undelegatedDocDate, D30))
        {
            logERR("OFD_DISCONNECT_30D!!!");
            OFD_DISCONNECT_30D = true;
            REG_UNAVAILABLE    = true;
        }
        if ((!OFD_DISCONNECT_20D) && (!OFD_DISCONNECT_30D))
        {
            OFD_DISCONNECT_TEST_EN = true;
        }
    }

    return ok;
}
//=====================================================================
int BIOS::RUS::fs_reset()
{
    logERR("%s RESET FISCAL MEMORY ...", __PRETTY_FUNCTION__);
    lock_guard<mutex> locker(*mutex_rf);
    return command0x9B();
}

//=====================================================================
bool BIOS::RUS::dreamkasKeyActivate(string &key)
{
    if ( key.empty() )
    {
        logERR("%s Пустой ключ при активации Дримкас-ключ!!!", __PRETTY_FUNCTION__);
        return false;
    }

    lock_guard<mutex> locker(*mutex_rf);
    int rc = libDreamkasKeySet(key);

    bool ok = ((rc == 0) ?  true : isCommandComplete(rc, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL));
    if (!ok)
    {
        logERR("%s Ошибка активации Дримкас-ключ!!!", __PRETTY_FUNCTION__);
        return false;
    }

    logDBG("%s Активация Дримкас-ключ завершилась успешно!", __PRETTY_FUNCTION__);

    return true;
}

bool BIOS::RUS::dreamkasKeyPrintAll()
{
    lock_guard<mutex> locker(*mutex_rf);
    int rc = libDreamkasKeyPrintAll();

    bool ok = ((rc == 0) ?  true : isCommandComplete(rc, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL));
    if (!ok)
    {
        logERR("%s Ошибка печати списка услуг Дримкас-ключ!!!", __PRETTY_FUNCTION__);
        return false;
    }

    logDBG("%s Печать списка услуг Дримкас-ключ завершилась успешно!", __PRETTY_FUNCTION__);

    return true;
}

//=====================================================================
// Фаза жизни ФН
bool BIOS::RUS::fs_status_get()
{
    #ifdef BIOS_RB
        return true;
    #endif
    // Запрос номера ФН
    fs_number_ask();

    lock_guard<mutex> locker(*mutex_rf);

    int     fnState, curDocState, warnFlags;
    int res = libGetCurFNState( &fnState, &curDocState, &warnFlags );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if (!ok)
    {
        return false;
    }
    unsigned char phaseOfLife =  fnState & 0x0F;
    //--
    isReadyToFiscal       = (phaseOfLife & 0x01 ? true : false);    // Маска для состояния "Готовность к фискализации"
    isFiscalMode          = (phaseOfLife & 0x02 ? true : false);    // Маска для состояния "Фискальный режим"
    isPostFiscalMode      = (phaseOfLife & 0x04 ? true : false);    // Маска для состояния "Постфиксальный режим."
    //HZ_CHTO             = (phaseOfLife == 0x0F ? true : false);    // Маска для состояния "Чтение данных из архива ФН"
    isShiftOpen           = (fnState      & 0x20);                   // Маска для состояния "смена открыта"
    //--
    isDocOpen             = (curDocState == 0x00 ? false : true);    // Маска для состояния "Документ открыт в ФН"
    //--
    IMMEDIATE_REPLACEMENT = (warnFlags & BIOS::BIT_IMMEDIATE_REPLACEMENT);  // Маска для состояния "Срочная замена КС"
    RESOURCES_EXHAUSTION  = (warnFlags & BIOS::BIT_RESOURCES_EXHAUSTION);   // Маска для состояния "Исчерпание ресурса КС"
    MEMORY_OVERFLOW       = (warnFlags & BIOS::BIT_MEMORY_OVERFLOW);        // Маска для состояния "Переполнение памяти ФН"
    CRITICAL_FN_ERROR     = (warnFlags & BIOS::BIT_CRITICAL_FN_ERROR);      // Маска для состояния "Переполнение памяти ФН"
    info( ALL_FLGS );
    return true;
}

//=====================================================================
//                  Чтение отделов
bool BIOS::RUS::readPayTypes()
{
    logDBG( "%s:: Чтение всех типов платежей...", __func__ );

    payTypes.clear();
    payTypes.push_back( PAY_TYPE(PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH,        PAY_TYPE_NAME_CASH    ) );
    payTypes.push_back( PAY_TYPE(PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASHLESS,    PAY_TYPE_NAME_CASHLESS) );
    payTypes.push_back( PAY_TYPE(PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_ADVANCE,     PAY_TYPE_NAME_ADVANCE ) );
    payTypes.push_back( PAY_TYPE(PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CREDIT,      PAY_TYPE_NAME_CREDIT  ) );
    payTypes.push_back( PAY_TYPE(PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_EXCHANGE,    PAY_TYPE_NAME_OTHER   ) );

    logDBG( "%s:: Все платежи успешно считаны", __func__ );
    return true;
}

//=====================================================================
void BIOS::setPort     (int newPort)
{
    lock_guard<mutex> locker(mutex_bios);
    port = newPort;
    libSetPort(port);
}

//=====================================================================
void BIOS::setPass     (Country newCountry)
{
    lock_guard<mutex> locker(mutex_bios);
    libSetPasswd(newCountry);
}

//=====================================================================
void BIOS::setErrors    (Country newCountry)
{
    lock_guard<mutex> locker(mutex_bios);
    libSetErrors(newCountry);
}
//=====================================================================
// Задать айпи адрес
//=====================================================================
void BIOS::setIP      (string newIP)
{
    lock_guard<mutex> locker(mutex_bios);
    ip = newIP;
    libSetIP(ip);
}
//=====================================================================
// Проверка кода результата от БИОС, для уточнения был ли выполнен запрос с предупреждением, либо невыполнен с ошибкой
//=====================================================================
bool isCommandComplete(int res, FB_OPERATION_CRITICAL_LEVEL op_lvl)
{
    bool isCriticalError = false;
    // Если res == 0 :
    if(res == 0) return true;
    // Если res != 0 :
    string eMess;
    isCriticalError = libIsErrorCritical(res, op_lvl, eMess);
    if (isCriticalError)
    {
        fb_wasErr =  true;
        fb_wasWarning = false;
        fb_errMess = eMess;
        fb_warnMess.clear();
        setLogDBcode_CP866();
        fb_errCode = res;
        fb_warnCode = 0;
        logDBG( "isCommandComplete:: eMess = %s\nisCommandComplete:: fb_errMess = %s\n code = %d",
                                                                        eMess.c_str(),
                                                                    fb_errMess.c_str(),
                                                                           fb_errCode);
    }
    else
    {
        fb_wasWarning =  true;
        fb_wasErr = false;
        fb_warnMess = eMess;
        fb_errMess.clear();
        fb_warnCode =   res;
        fb_errCode = 0;
        logDBG( "isCommandComplete:: warnMess = %s\nisCommandComplete:: fb_warnMess = %s\n code = %d",
                                                                        eMess.c_str(),
                                                                    fb_warnMess.c_str(),
                                                                           fb_warnCode);
    }
    return !isCriticalError;
}
//=====================================================================
// Создание соединения с БИОСом
//=====================================================================
bool BIOS::initConnection()
{
    unique_lock<mutex> locker(mutex_bios);

    // Если соединение уже установлено, то просто выходим
    if( isConnected.load() == true )
    {
        logDBG("%s:: Соединение уже установлено, выход", __func__);
        return true;
    }

    if(fg == nullptr)
    {
        logERR( "%s Null Fg object ptr!!!", __PRETTY_FUNCTION__ );
        return false;
    }

//    if(fg->isFileExist("debugBios") == FilesFG::FILE_IS_EXIST) // если при старте фисго есть такой файл от отладка
//    {
//        setDebugLevel(1);
//    }
//    else
    {
        setDebugLevel(0);
    }
    libSetPort(port);
    libSetIP(ip);

    int res = commandStart();
    isConnected.store((res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_CONNECTION));    // если результат != 0, то проверяем, критическая ли ошибка

    if (isConnected.load())
    {
        initBiosCv.notify_all();
    }

    #ifdef BIOS_RF
        RF->isConnected = isConnected.load();
    #else
        RB->isConnected = isConnected.load();
    #endif
    logDBG("initConnection():: Соединение с BIOS %sустановлено. \n\tКод результата = %02x      \n\tisConnected = %01x",
                                                                                              (res == 0 ? " " : "не "),
                                                                                                                   res,
                                                                                                           isConnected.load() );
    if(!isConnected.load())
    {
        logERR("initConnection():: Сообщение об ошибке = '%s'", fb_errMess.c_str() );
    }
    return isConnected.load();
}

//=====================================================================
// Открытие смены в БИОС
//=====================================================================
bool BIOS::RUS::openShift(string cashier)
{
    lock_guard<mutex> locker(*mutex_rf);

    logDBG("Открытие смены.... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("BIOS::openShift()  Ошибка! Соединение не проинициализированно!");
        return false;
    }

    if (isDocOpen_get())
    {
        // Закрытие ранее открытого документа при его наличии
        cancelDoc();
    }

    int res = libOpenShift(cashier.c_str());

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("Смена %sоткрыта! Код ответа = %d",(ok ? " " : "не "), res);

    return ok;
}
//=====================================================================
// Позиция
//=====================================================================
BIOS::RUS::Position::Position()
{
    punix.goodsName = "Товар не задан";
    punix.barcode = "000000000000000000";
    punix.quantity = 0.0;
    punix.price = 0.0;
    punix.taxNumber = SNO_OSN;
    punix.numGoodsPos = 1;
    punix.numDepart = 1;
    punix.coefType = 0x00;
    punix.coefName.clear();
    punix.coefValue = 0.0;
    punix.signMethodCalculation = 4;
    punix.signCalculationObject = 1;

    discountRubSum = 0;
    marginRubSum = 0;
    productCode.clear();
}

BIOS::BEL::POSITION::POSITION()
{
    name                   =         "Товар не задан";
    barcode                =     "000000000000000000";
    quantity               =                      0.0;
    price                  =                      0.0;
    taxNumber              =                  SNO_OSN;
    numGoodsPos            =                        1;
    numDepart              =                        1;

    discountRubSum         =                        0;
    discountType           = /*Positions::DISCOUNT_TYPE_ABSOLUTE;*/0;
    marginRubSum           =                        0;
}
BIOS::BEL::POSITION::~POSITION()
{

}
//=====================================================================
//=====================================================================
BIOS::PAYMENT::PAYMENT()
{
    typePayment =                      0;
    sum         =                    0.0;
    infoStr     = "";
}
BIOS::PAYMENT::~PAYMENT()
{

}
//=====================================================================
// Открытие Открыть документ на заполнение
//=====================================================================
BIOS::RUS::OPEN_DOC::OPEN_DOC()
{
    cashier =                            "Кассир";
    type    =            DOC_TYPES::DOC_TYPE_SALE;
    fmt     =      DOC_FORMATS::DOC_FORMAT_NORMAL;
    req     = DOC_REQ_PRINT::DOC_REQ_PRINT_NORMAL;
    otdel   =                                   1;
    sno     =                  SNO_TYPES::SNO_OSN;
}
BIOS::RUS::OPEN_DOC::~OPEN_DOC()
{

}
//=====================================================================
BIOS::BEL::OPEN_DOC::OPEN_DOC()
{
    cashier =                            "Кассир";
    type    =      DOC_TYPES_BY::DOC_TYPE_BY_SALE;
    fmt     =      DOC_FORMATS::DOC_FORMAT_NORMAL;
    req     = DOC_REQ_PRINT::DOC_REQ_PRINT_NORMAL;
    otdel   =                                   1;
}
BIOS::BEL::OPEN_DOC::~OPEN_DOC()
{

}
int BIOS::setAutoOpenShift()
{
    logWARN("%s", __PRETTY_FUNCTION__);
    int mask(0);

    int rv(getPurchaseMode(mask));
    if (rv == 0)
    {
        mask |= PIRIT_SET_KKT_AUTO_SHIFT;
        rv = setPurchaseMode(static_cast<uint8_t>(mask));
    }

    logWARN("%s\t\t%s", __PRETTY_FUNCTION__, (rv == 0) ? "Success" : "Failed!");
    return rv;
}

bool BIOS::waitInitConnection()
{
    unique_lock<mutex> locker(initBiosMutex);

    auto now      = chrono::system_clock::now();
    auto wakeTime = now + chrono::seconds(300);

    if (initBiosCv.wait_until(locker, wakeTime, [&]{return isConnected.load();}))
    {
        return true;
    }

    return isConnected.load();
}
//=====================================================================
// Печатаем чеки из ИМ
//=====================================================================
void BIOS::setPrinterExPrint()
{
    int mask_print = 0;
    getPrinterMode( mask_print );
    mask_print &= ~PIRIT_SET_KKT_NOT_PRINT;

    setPrinterMode( mask_print );
}
//=====================================================================
// Не печатаем чеки из ИМ
//=====================================================================
void BIOS::setPrinterExNotPrint()
{
    int mask_print = 0;
    getPrinterMode( mask_print );
    mask_print |= PIRIT_SET_KKT_NOT_PRINT;

    setPrinterMode( mask_print );
}
//=====================================================================
// Отправка чека в BIOS RF
//=====================================================================
bool BIOS::receipt_send_rf_bios(RUS::RECEIPT *receipt, Config &config)
{
    logDBG( "RECEIPT SEND IN BIOS RUS ..." );

    if (receipt == nullptr)
    {
        logERR( "%s: NULL RECEIPT POINTER!!!", __PRETTY_FUNCTION__ );
        return false;
    }

    lock_guard<mutex> locker(mutex_bios);

    lastErrorExtMessage.clear();
    //---------------------------------------------------
    // Закрытие ранее открытого документа при его наличии
    if (RF->isDocOpen_get())
    {
        logERR( "%s: Закрытие ранее открытого документа!", __PRETTY_FUNCTION__ );
        RF->cancelDoc();
    }
    //---------------------------------------------------
    //--
    // Открыть чек
    // Пакетный режим
    receipt->doc.fmt = DOC_FORMAT_PACKET;

    if(RF->openDoc( &receipt->doc ) == false)
    {
        logERR( "%s: Не удалось открыть документ!", __PRETTY_FUNCTION__ );
        return false;
    }

    bool ok = true;
    //--
    // Добавить позиции
    double totalPosDiscount = 0;
    for (RUS::Position &pos : receipt->positions)
    {
        ok = RF->addPositionToCheck( &pos );
        if(!ok)
        {
            break;
        }
        // Если есть имя скидки на позицию, значит есть скидка, печатаем на ПФ
        if( pos.discountRubSum != 0 )
        {
        ok = printDiscount( pos.discountRubSum, " Скидка" );
        if(  !ok )
        {
            logERR( "%s: Ошибка добавления скидки на позицию: '%s'", __PRETTY_FUNCTION__ , pos.discountRubName.c_str() );
            break;
        }
        totalPosDiscount+=pos.discountRubSum;
        }
    }
    //--
    // Добавить платёж
    if( ok )
    {
        PAYMENT addingPayment;
        // Одиночная оплата любым типом
        // В РФ typePayment всегда == MAX_PAY_TYPES
        if( receipt->payment.typePayment < BIOS::MAX_PAY_TYPES )
        {
            addingPayment.typePayment = receipt->payment.typePayment;
            addingPayment.sum = RF->convert->cop2rub( receipt->payment.sum );

            ok = addPaymentToCheck(&addingPayment);

            logINFO("%s:: Добавление простой оплаты::    номер = %d сумма = %f",
                    __func__,
                    addingPayment.typePayment,
                    addingPayment.sum);

            if( !ok )
            {
                logERR("%s:: ОШИБКА ПРИ ДОБАВЛЕНИИ ПРОСТОЙ ОПЛАТЫ!!!!! номер = %d сумма = %f",
                       __func__,
                       addingPayment.typePayment,
                       addingPayment.sum);
            }
        }
        else if( !RF->recPayTypes.empty() )
        {
            for(const pair<int, uint64_t>& rpt : RF->recPayTypes)
            {
                // Скипаем оплату налом, чтобы добавить её в конце(чтобы сдача была)
                if( (rpt.first == PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH)  || (rpt.second == 0) )
                {
                    continue;
                }
                addingPayment.typePayment = rpt.first;
                addingPayment.sum = RF->convert->cop2rub( rpt.second );
                logINFO("%s:: Добавление смешанной оплаты::    номер = %d сумма = %f",
                       __func__,
                       addingPayment.typePayment,
                       addingPayment.sum);
                ok = addPaymentToCheck( &addingPayment );
                if( !ok )
                {
                    logERR("%s:: ОШИБКА ПРИ ДОБАВЛЕНИИ СМЕШАННОЙ ОПЛАТЫ!!!!! номер = %d сумма = %f",
                           __func__,
                           addingPayment.sum,
                           addingPayment.typePayment);
                    break;
                }
            }

            // Добавляем оплату налом в конце, если есть
            int indexCash = PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH;
            const uint64_t ZERO_SUMMA = 0;
            if( ok && (RF->recPayTypes.count( indexCash ) != 0)  && (RF->recPayTypes.at( indexCash ) != 0)  )
            {
                addingPayment.typePayment =  PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH;
                addingPayment.sum = RF->convert->cop2rub( RF->recPayTypes.at(indexCash) );
                logINFO("%s:: Добавление наличных к смешанной оплате:: сумма = %f",  __func__,
                       addingPayment.sum);
                ok = addPaymentToCheck( &addingPayment );
                if( !ok )
                {
                    logERR("%s:: ОШИБКА ПРИ ДОБАВЛЕНИИ НАЛИЧНЫХ К СМЕШАННОЙ ОПЛАТЕ!!!!! номер = %d сумма = %f",
                           __func__,
                           addingPayment.typePayment,
                           addingPayment.sum);
                }
            }
            // Если есть единственная оплата с суммой ноль(скидка вов всю позицию)
            else if( ok && (RF->recPayTypes.size() == 1)  && (RF->recPayTypes.begin()->second == ZERO_SUMMA)  )
            {
                addingPayment.typePayment =  RF->recPayTypes.begin()->first;
                addingPayment.sum = ZERO_SUMMA;
                logINFO("%s:: Добавление ФИКТИВНОЙ оплаты на 0",  __func__ );
                ok = addPaymentToCheck( &addingPayment );
                if( !ok )
                {
                    logERR("%s:: ОШИБКА ПРИ ДОБАВЛЕНИИ НАЛИЧНЫХ(=НОЛЬ)!!!!! номер = %d сумма = %f",
                           __func__,
                           addingPayment.typePayment,
                           addingPayment.sum);
                }
            }
        }
        // Если не одиночная и не смешанная оплата, то ошибка
        else
        {
            ok = false;
            logERR("%s:: НЕИЗВЕСТНЫЙ ТИП ОПЛАТЫ! Некорректный индекс оплаты = %d",__func__, receipt->payment.typePayment );
        }
    }
    // Скидка
    if( ok && (totalPosDiscount!=0) )
    {
        ok = printDiscount( totalPosDiscount, "ВАША СКИДКА" );
        if(  !ok )
        {
            logERR( "%s: Ошибка добавления скидки на чек: '%s'",      __PRETTY_FUNCTION__ , receipt->discountRubName.c_str() );
        }
    }
#ifdef BIOS_RF
    //--
//    // Агенты на чек
//    if ( ok )
//    {
//        ok = (RF->addAgentToCheck(receipt->agent) == EXIT_SUCCESS);
//        if(!ok)
//        {
//            logERR("%s: Ошибка добавления агентов на чек!!!", __PRETTY_FUNCTION__ );
//        }
//    }
    // дополнительный реквизит чека (БСО)
    if (!receipt->addRecReq.empty())
    {
        constexpr size_t MAX_LEN {16};
        if (receipt->addRecReq.length() > MAX_LEN)
        {
            receipt->addRecReq = receipt->addRecReq.substr(0, MAX_LEN);
        }
        ok = (libPrintRequsitOFD(PIRIT_REQ_OFD_ADD_REC_REQ, FONT_NORM, "", receipt->addRecReq.c_str()) == EXIT_SUCCESS);
        logWARN("%s print add purchase requisite %s", __FUNCTION__, ok ? "success" : "falied!");
    }
    // место расчётов
    if (!receipt->clcPlace.empty())
    {
        constexpr size_t MAX_LEN {256};
        receipt->clcPlace = receipt->clcPlace.substr(0, MAX_LEN);
    }
#endif
    //--
    // Завершить документ
    if(ok)
    {
        ok = RF->closeDoc( *receipt );
    }
    //----------------------------------------------------------
    // Отменить документ если что то пошло не так после открытия
    if ( !ok )
    {
        lastErrorExtMessage = RF->getExtendedErrorCode();
        logERR( "%s:: Расширенное сообщение об ошибке: '%s'", __func__, Convert::escapingOfQuotes(lastErrorExtMessage).c_str() );
        RF->cancelDoc();
    }
    if(ok)
    {
        logDBG( "УСПЕШНАЯ ОТПРАВКА ЧЕКА В БИОС" );
    }
    else
    {
        logERR( "ОШИБКА ОТПРАВКИ ЧЕКА В БИОС!!!" );
    }

    return ok;
}


//=====================================================================
// Отправка чека в BIOS RB
//=====================================================================
bool BIOS::receipt_send_rb_bios( BEL::RECEIPT *receipt )
{
    lock_guard<mutex> locker(mutex_bios);

    logDBG( "RECEIPT SEND IN BIOS RB ..." );

    if(receipt == NULL)
    {
        logERR( "%s: NULL RECEIPT POINTER!!!", __PRETTY_FUNCTION__ );
        return false;
    }
    //---------------------------------------------------
    //--
    // Открыть чек
    // Пакетный режим
    receipt->doc.fmt = DOC_FORMAT_PACKET;
    if (!RB->openDoc(&receipt->doc))
    {
        logERR( "%s: Не удалось открыть документ!" );
        return false;
    }

    bool ok = true;
    //--
    // Добавить позиции
    for ( BEL::POSITION pos : receipt->positions )
    {
        ok = RB->addPositionToCheck( &pos );
        if( !ok )
        {
            logERR( "%s: Ошибка добавления позиции в чек! '%s'",      __PRETTY_FUNCTION__ , pos.name.c_str() );
            break;
        }
        // Если есть имя скидки на позицию, значит есть скидка
        if( !pos.discountRubName.empty() )
        {
            //ok = RB->addDiscountRubles(pos.discountRubName.c_str(), pos.discountRubSum, pos.discountType );
            if(  !ok )
            {
                logERR( "%s: Ошибка добавления скидки на позицию: '%s'",      __PRETTY_FUNCTION__ ,
                                                                       pos.discountRubName.c_str() );
                break;
            }
        }

        // Если есть имя надбавки на позицию, значит есть надбавка
        if( ok && !pos.marginRubName.empty() )
        {
            ok = RB->addMarginRubles(pos.marginRubName.c_str(), pos.marginRubSum);
            if( !ok )
            {
                logERR( "%s: Ошибка добавления надбавки на позицию: '%s'",      __PRETTY_FUNCTION__ ,
                                                                         pos.marginRubName.c_str() );
                break;
            }
        }
    }
    //--
    // Подытог
    if( ok )
    {
        ok = RB->subTotal();
    }
    //--
    // Скидка
    if( ok && !receipt->discountRubName.empty() )
    {
        //ok = RB->addDiscountRubles(receipt->discountRubName.c_str(), receipt->discountRubSum, receipt->discountType );
    }
    //--
    // надбавка
    if( ok && !receipt->marginRubName.empty() )
    {
        ok = RB->addMarginRubles(receipt->marginRubName.c_str(), receipt->marginRubSum);
    }
    //--
    // Добавить платёж РБ
    if( ok )
    {
        // Одиночная оплата любым типом
        if( receipt->payment.typePayment < BIOS::MAX_PAY_TYPES )
        {
            ok = addPaymentToCheck(&receipt->payment);
            logERR("%s:: Добавление простой оплаты::    номер = %d сумма = %f",
                                                               __func__,
                                                               receipt->payment.typePayment,
                                                               receipt->payment.sum);
        }
        // Смешанная оплата Если есть мапа заполненная
        else if( !RB->recPayTypes.empty() )
        {
            PAYMENT addingPayment;
            for(pair<int, uint64_t> rpt : RB->recPayTypes)
            {
                // Скипаем оплату налом, чтобы добавить её в конце(чтобы сдача была)
                if( (rpt.first == PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH) || (rpt.second == 0) )
                {
                    continue;
                }
                addingPayment.typePayment = rpt.first;
                addingPayment.sum = RB->convert->cop2rub( rpt.second );
                logERR("%s:: Добавление смешанной оплаты::    номер = %d сумма = %f",
                                                                           __func__,
                                                                           addingPayment.typePayment,
                                                                           addingPayment.sum);
                ok = addPaymentToCheck( &addingPayment );
                if( !ok )
                {
                    logERR("%s:: ОШИБКА ПРИ ДОБАВЛЕНИИ СМЕШАННОЙ ОПЛАТЫ!!!!! номер = %d сумма = %f",
                                                                                   __func__,
                                                                                   addingPayment.sum,
                                                                                   addingPayment.typePayment);
                    break;
                }
            }

            // Добавляем оплату налом в конце, если есть
            int indexCash = PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH;
            if( ok && RB->recPayTypes.count( indexCash ) != 0 )
            {
                addingPayment.typePayment =  PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH;
                addingPayment.sum = RB->convert->cop2rub( RB->recPayTypes.at(indexCash) );
                logERR("%s:: Добавление наличных к смешанной оплате:: сумма = %f",  __func__,
                                                                         addingPayment.sum);
                ok = addPaymentToCheck( &addingPayment );
                if( !ok )
                {
                    logERR("%s:: ОШИБКА ПРИ ДОБАВЛЕНИИ НАЛИЧНЫХ К СМЕШАННОЙ ОПЛАТЕ!!!!! номер = %d сумма = %f",
                                                                                       __func__,
                                                                                       addingPayment.sum,
                                                                                       addingPayment.typePayment);
                }
            }
        }
        // Если не одиночная и не смешанная оплата, то ошибка
        else
        {
            ok = false;
            logERR("%s:: НЕИЗВЕСТНЫЙ ТИП ОПЛАТЫ! Некорректный индекс оплаты = %d",__func__, receipt->payment.typePayment );
        }
    }
    //--
    // Завершить документ
    if( ok )
    {
        ok = RB->closeDoc();
    }

    //----------------------------------------------------------
    // Отменить документ если что то пошло не так после открытия
    if ( !ok )
    {
        RB->cancelDoc();
    }
    if(ok)
    {
        logDBG( "УСПЕШНАЯ ОТПРАВКА ЧЕКА В БИОС" );
    }
    else
    {
        logERR( "ОШИБКА ОТПРАВКИ ЧЕКА В БИОС!!!" );
    }
    return ok;
}


//=====================================================================
bool BIOS::RUS::openDoc( OPEN_DOC *open_doc )

{
    logDBG("BIOS::openDoc()  Открытие Документа на заполнение...");
    if( !isConnected )
    {
        logERR("BIOS::openDoc()  Ошибка! Соединение не проинициализированно!");
        return false;
    }

    if(open_doc == NULL)
    {
        logERR( "OPEN DOC NULL POINTER!!!" );
        return false;
    }
    int res = libOpenDocument( open_doc->type,
                               open_doc->fmt,
                               open_doc->req,
                               open_doc->otdel,
                               open_doc->cashier,
                               11,
                               open_doc->sno );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if (!ok)
    {
        fillErrorCodes(static_cast<FB_ERROR_CODE>(res));
    }

    logDBG("Документ %sоткрыт! Код ответа = %d",(ok ? " " : "не "), res);
    return ok;
}
//=====================================================================
bool BIOS::BEL::getDocFromKL(const uint32_t &docNum, DocFromKL &doc)
{
    logDBG("%s", __PRETTY_FUNCTION__);
    bool ok(false);

    if (!isConnected || (mutex_rb == nullptr))
    {
        logERR("%s\tОшибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return ok;
    }

    lock_guard<mutex> locker(*mutex_rb);

    int res(libGetDocFromKL(docNum, doc));
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("%s\tКод ответа = %d", __PRETTY_FUNCTION__, res);
    return ok;
}
//=====================================================================
bool BIOS::BEL::openDoc( OPEN_DOC *open_doc )
{
    logDBG("BIOS::BEL::openDoc()  Открытие Документа на заполнение...");
    if( !isConnected )
    {
        logERR("BIOS::openDoc()  Ошибка! Соединение не проинициализированно!");
        return false;
    }

    if(open_doc == NULL)
    {
        logERR( "OPEN DOC NULL POINTER!!!" );
        return false;
    }

    int res = libOpenDocument_By( open_doc->type,
                                  open_doc->fmt,
                                  open_doc->req,
                                  open_doc->otdel,
                                  open_doc->cashier,
                                  11 );
    bool ok = (res == 0);
    if (!ok)
    {
        isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    }

    logDBG("Документ %sоткрыт! Код ответа = %d",(ok ? " " : "НЕ "), res);
    return ok;
}


//=====================================================================
// Получение данных по последнему чеку
//=====================================================================
bool BIOS::RUS::rec_fis_data_get(BIOS::RUS::RECEIPT_FIS_DATA &recieptData)
{
    lock_guard<mutex> locker(*mutex_rf);
    int res  = libGetLastRecieptData(  recieptData.type,         // тип чека (для аннулиров. = 0)
                                       recieptData.cur_op_cnt,   // текущий операц. счетчик
                                       recieptData.rec_num,      // номер чека
                                       recieptData.doc_num,      // номер документа
                                       recieptData.sum,          // сумма чека
                                       recieptData.discount,     // сумма скидки
                                       recieptData.fpd,          // строка ФП (фиск. признак)
                                       recieptData.fd         ); // номер ФД

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("RUS rec_fis_data_get():: %sВЫПОЛНЕНО! fpd = %s \t fd = %d", (ok ? " " : "НЕ "), recieptData.fpd.c_str(), recieptData.fd);

    logWARN("%s", __FUNCTION__);
    logWARN("\tType: %d", recieptData.type);
    logINFO("\tCurrent operation counter: %s", recieptData.cur_op_cnt.c_str());
    logINFO("\tPurchase number: %d", recieptData.rec_num);
    logWARN("\tDocument number: %d", recieptData.doc_num);
    logINFO("\tSum: %f", recieptData.sum);
    logINFO("\tFPD: %s", recieptData.fpd.c_str());
    logINFO("\tFD: %d", recieptData.fd);

    return ok;
}
//=====================================================================
bool BIOS::BEL::KLDocRead( int doc_num, vector<string> &strs )
{
    lock_guard<mutex> locker(*mutex_rb);
    int res = libKLDocRead( doc_num, strs );

    logDBG("BEL KLDocRead:: %sВЫПОЛНЕНО!", (res == 0) ? " " : "НЕ ");

    return (res == 0) ?  true : false;
}
//=====================================================================
bool BIOS::BEL::KLDocPrint( int doc_num )
{
    lock_guard<mutex> locker(*mutex_rb);

    int res = libKLDocPrint( doc_num );

    logDBG("BEL KLDocPrint:: %sВЫПОЛНЕНО!", (res == 0) ? " " : "НЕ ");

    return (res == 0) ?  true : false;
}
//=====================================================================
bool BIOS::BEL::getSKNODateTime(Convert::DATE_ISO_8601 &dateIso)
{
    if ( !convert )
    {
        logERR("%s\t\t convert nullptr!",  __PRETTY_FUNCTION__ );
        return false;
    }

    unique_lock<mutex> locker(*mutex_rb);

    string strDate;
    int res = getStrServiceInfoSKNO(PIRIT_SKNO_DATETIME, PIRIT_DATETIME_DATE, strDate);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR("%s\t\tНЕ ВЫПОЛНЕНО!",  __PRETTY_FUNCTION__ );
        return false;
    }

    string strTime;
    res = getStrServiceInfoSKNO(PIRIT_SKNO_DATETIME, PIRIT_DATETIME_TIME, strTime);
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR("%s\t\tНЕ ВЫПОЛНЕНО!",  __PRETTY_FUNCTION__ );
        return false;
    }

    locker.unlock();

    string strDateTime = strDate + strTime;
    if ( convert->DateISO8601Create(dateIso, strDateTime, true) )
    {
        logERR("%s\t\tВЫПОЛНЕНО!  Дата и время СКНО = %s", __PRETTY_FUNCTION__, strDateTime.c_str());
        return true;
    }
    else
    {
        logERR("%s\t\tОшибка! Некорректные Дата и время СКНО = %s", __PRETTY_FUNCTION__, strDateTime.c_str());
        return false;
    }

    return (res == 0) ?  true : false;
}
//=====================================================================
bool BIOS::BEL::rec_fis_data_get(BIOS::BEL::RECEIPT_FIS_DATA &recieptData)
{
    lock_guard<mutex> locker(*mutex_rb);
    int res  = libGetLastRecieptData_By(  recieptData.type,         // тип чека (для аннулиров. = 0)
                                          recieptData.cur_op_cnt,   // текущий операц. счетчик
                                          recieptData.rec_num,      // номер чека
                                          recieptData.doc_num,      // номер документа
                                          recieptData.sum,          // сумма чека
                                          recieptData.discount,     // сумма скидки
                                          recieptData.markup);      // сумма наценки

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("BEL rec_fis_data_get():: %sВЫПОЛНЕНО!", (ok ? " " : "НЕ "));

    logWARN("%s", __FUNCTION__);
    logWARN("\tType: %d", recieptData.type);
    logINFO("\tCurrent operation counter: %s", recieptData.cur_op_cnt.c_str());
    logINFO("\tPurchase number: %d", recieptData.rec_num);
    logWARN("\tDocument number: %d", recieptData.doc_num);
    logINFO("\tSum: %f", recieptData.sum);
    logINFO("\tDiscount: %f", recieptData.discount);
    logINFO("\tMarkup: %f", recieptData.markup);

    return ok;
}
//=====================================================================
// Печать отчета по данным из БЭП
//=====================================================================
bool BIOS::BEL::print_report_fixed_mem_unit(string password, string dateReg, string cashier)
{
    lock_guard<mutex> locker(*mutex_rb);

    if (convert == nullptr)
    {
        logERR("%s, nullptr!", __PRETTY_FUNCTION__);
        return false;
    }

    time_t curTime = convert->cur_unixtime_get();
    tm *timePtr = localtime(&curTime);

    PLDate stopDate;
    stopDate.year  = timePtr->tm_year + 1900;   // Year - 1900
    stopDate.month = timePtr->tm_mon + 1;       // Month (0-11)
    stopDate.day   = timePtr->tm_mday;          // Day of the month (1-31)

    PLDate startDate;
    startDate.day   = static_cast<uint8_t>( stoi(dateReg.substr(0, 2)) );   // 1-31
    startDate.month = static_cast<uint8_t>( stoi(dateReg.substr(2, 2)) );   // 1-12
    startDate.year  = stoi(dateReg.substr(4, 2));                           // xx

    startDate.year += 2000;

    const uint8_t TYPE_FULL    = 1;
//    const uint8_t TYPE_TOT_SUM = 2;

    int res = libPrintFiscalReportByDate(TYPE_FULL, startDate, stopDate, password.c_str(), cashier.c_str());

    const int WRONG_PSWD = 13;
    bool ok;
    if(res != WRONG_PSWD)
    {
        ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    }
    else
    {
        ok = false;
    }

    logDBG("BEL print_report_bep():: %sВЫПОЛНЕНО!", (ok ? " " : "НЕ "));

    return ok;
}
//=====================================================================
// Распечатать буфер контрольной ленты
//=====================================================================
bool BIOS::BEL::print_report_ctrl_tape()
{
    lock_guard<mutex> locker(*mutex_rb);

    int res = libBLRPrintControlTapeFromECT();

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);

    logDBG("BEL print_report_kl():: %sВЫПОЛНЕНО!", (ok ? " " : "НЕ "));
    return ok;
}

BIOS::BEL::KCA_SCNO_STATE BIOS::BEL::getScno_state() const
{
    return scno_state;
}
//=====================================================================
// Вернуть сумму наличных в денежном ящике (в копейках)
//=====================================================================
bool BIOS::getCashInDrawer (uint64_t &cash)
{
    lock_guard<mutex> locker(mutex_bios);
    char dataSum[100] = {0x00};
    int res = getKKTInfo(0x07, dataSum);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    double cashPubli = QString(dataSum).toFloat();
    cash = static_cast<uint64_t>(cashPubli * 100 + .5);

    logDBG("getCashInDrawer:: %sВЫПОЛНЕНО! Сумма в денежном ящике в копейках = %llu", (ok ? " " : "НЕ "), cash);
    return ok;
}
//=====================================================================
// Вернуть заводской номер ККТ
//=====================================================================
bool BIOS::getKKTPlantNum(string &plantNum)
{
    #ifdef BIOS_RB
        return true;
    #endif
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfo( PIRIT_KKT_INFO_KKT_PLANT_NUM, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);

    if ( ok )
    {
        plantNum = data;
        plantNum = Convert::end_symbols_delete( plantNum, 0x20 );
    }

    logDBG("%s\t\t %sВЫПОЛНЕНО!   Заводской номер ККТ = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), plantNum.c_str());
    return ok;
}
//=====================================================================
// Вернуть ИНН организации
//=====================================================================
bool BIOS::getOrganizationINN(string &inn)
{
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfo( PIRIT_KKT_INFO_INN, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if ( ok )
    {
        inn = data;
        inn = Convert::end_symbols_delete( inn, 0x20 );
    }

    logDBG("%s\t %sВЫПОЛНЕНО!   ИНН организации = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), inn.c_str());
    return ok;
}
//=====================================================================
// Вернуть регистрационный номер ККТ
//=====================================================================
bool BIOS::getKKTRegNum(string &regNum)
{
    #ifdef BIOS_RB
        return true;
    #endif
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfo( PIRIT_KKT_INFO_PLANT_NUM, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if ( ok )
    {
        regNum = data;
        regNum = Convert::end_symbols_delete( regNum, 0x20 );
    }

    logDBG("%s\t\t %sВЫПОЛНЕНО!   Регистрационный номер ККТ = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), regNum.c_str());
    return ok;
}
//=====================================================================
// Вернуть дату и время последней фискальной операции
//=====================================================================
bool BIOS::getDateTimeLastFiscalDoc(string &dateTime)
{
    #ifdef BIOS_RB
        return true;
    #endif
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfoNum( PIRIT_KKT_INFO_DATE_TIME_LAST_FD, PIRIT_PARAM_1, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if ( ok)
    {
        dateTime = data;
    }

    memset(data, 0, sizeof(data));
    res = getKKTInfoNum( PIRIT_KKT_INFO_DATE_TIME_LAST_FD, PIRIT_PARAM_2, data);
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if ( ok )
    {
        dateTime += data;
    }

    logERR("%s\t%sВЫПОЛНЕНО!   Дата и время последней фискальной операции = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), dateTime.c_str());
    return ok;
}
//=====================================================================
// Вернуть дату последней регистрации/перерегистрации
//=====================================================================
bool BIOS::getDateLastRegistration(string &date)
{
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfo( PIRIT_KKT_INFO_DATE_TIME_REG, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if ( ok )
    {
        date = data;
    }

    logDBG("%s\t%sВЫПОЛНЕНО!   Дата последней регистрации/перерегистрации = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), date.c_str());
    return ok;
}


//=====================================================================
// Дата окончания временного ресурса  ФН
//=====================================================================
bool BIOS::getDateFNValidBefore(string &date)
{
#ifdef BIOS_RB
    date = "000000";
    return false;
#endif
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfo( PIRIT_KKT_INFO_FS_VALIDITY_DATE, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if ( ok )
    {
        date = data;
    }
    logDBG("%s\t%sВЫПОЛНЕНО!    Дата окончания временного ресурса ФН = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), date.c_str());
    return ok;
}


//=====================================================================
// Вернуть дату и время открытия смены
//=====================================================================
bool BIOS::getDateTimeOpenShift(string &dateTime)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfoNum( PIRIT_KKT_INFO_DATE_TIME_OPEN_SHIFT, PIRIT_PARAM_1, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if ( ok )
    {
        dateTime = data;
    }

    memset(data, 0, sizeof(data));
    res = getKKTInfoNum( PIRIT_KKT_INFO_DATE_TIME_OPEN_SHIFT, PIRIT_PARAM_2, data);
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if ( ok )
    {
        dateTime += data;
    }

    logDBG("%s\t%sВЫПОЛНЕНО!   Дата и время открытия смены = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), dateTime.c_str());
    return ok;
}
//=====================================================================
// Вернуть признаки и статус агентов
//=====================================================================
bool BIOS::getSignAndAgents(uint16_t &status)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfoNum( PIRIT_KKT_INFO_CNO_SIGNS_AGENT, PIRIT_PARAM_2, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if ( ok )
    {
         status =  QString(data).toLong();
    }

    logDBG("%s\t\t%sВЫПОЛНЕНО!   Признаки и статус агентов = %d", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), status);
    return ok;
}
//=====================================================================
// Вернуть доступные СНО
//=====================================================================
bool BIOS::getTaxSystems(uint8_t &taxes)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfoNum( PIRIT_KKT_INFO_CNO_SIGNS_AGENT, PIRIT_PARAM_1, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if ( ok )
    {
        taxes = QString(data).toLong();
    }

    logDBG("%s\t\t%sВЫПОЛНЕНО!   СНО = %d", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), taxes);
    return ok;
}
//=====================================================================
// Вернуть признак наличия принтера
//=====================================================================
bool BIOS::getPrinterMachine(uint16_t &status)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfoNum( PIRIT_KKT_INFO_CNO_SIGNS_AGENT, PIRIT_PARAM_3, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if ( ok )
    {
        status = QString(data).toULong();
    }

    logDBG("%s\t\t%sВЫПОЛНЕНО!   Установка принтера в авт режиме = %d", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), status);
    return ok;
}
//=====================================================================
// Вернуть наименование организации
//=====================================================================
bool BIOS::getOrganizationAddress(string &orgAddress)
{
    lock_guard<mutex> locker(mutex_bios);
    string tmp;

    int res = getStrFromSettingsTable(tmp, PIRIT_TABLE_NAME_ADDRESS_ORG, PIRIT_PARAM_2);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if(!ok)
    {
        return false;
    }
    orgAddress  = tmp;

    tmp.clear();
    res = getStrFromSettingsTable(tmp, PIRIT_TABLE_NAME_ADDRESS_ORG, PIRIT_PARAM_3);
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    orgAddress += tmp;

    logDBG("%s\t%sВЫПОЛНЕНО!   Адрес организации = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), orgAddress.c_str());

    return ok;
}
//=====================================================================
// Вернуть наименование организации
//=====================================================================
bool BIOS::getOrganizationName(string &orgName)
{
    lock_guard<mutex> locker(mutex_bios);
    string tmp;

    int res = getStrFromSettingsTable(tmp, PIRIT_TABLE_NAME_ADDRESS_ORG, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if(!ok)
    {
        return false;
    }
    orgName  = tmp;

    tmp.clear();
    res = getStrFromSettingsTable(tmp, PIRIT_TABLE_NAME_ADDRESS_ORG, PIRIT_PARAM_1);
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    orgName += tmp;

    logDBG("%s\t%sВЫПОЛНЕНО!   Наименование организации = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), orgName.c_str());

    return ok;
}
//=====================================================================
// Вернуть место расчетов
//=====================================================================
bool BIOS::getCalculationPlace(string &calcPlace)
{
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(calcPlace, PIRIT_TABLE_CALCULATION_PLACE, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("%s\t%sВЫПОЛНЕНО!   Место расчетов = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), calcPlace.c_str());

    return ok;
}
//=====================================================================
// Вернуть состояние доступа к ОФД
//=====================================================================
bool BIOS::getOFDStatus(int &status)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = libGetStatusOFD( status );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("%s\t%sВЫПОЛНЕНО!", __PRETTY_FUNCTION__, (ok ? " " : "НЕ ") );

    if ( status == 0 )
    {
        logERR("%s\t\t ОФД доступен", __PRETTY_FUNCTION__);
    }
    else if ( status == 1 )
    {
        logERR("%s\t\t ОФД не доступен", __PRETTY_FUNCTION__);
    }
    else
    {
        logERR("%s\t\t ОФД не настроен", __PRETTY_FUNCTION__ );
    }

    return ok;
}
//=====================================================================
// Вернуть наименование ОФД
//=====================================================================
bool BIOS::getOFDName(string &ofdName)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(ofdName, PIRIT_TABLE_OFD_NAME, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t%sВЫПОЛНЕНО!Наименование ОФД = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), ofdName.c_str());

    return ok;
}
//=====================================================================
// Вернуть порт ОФД
//=====================================================================
bool BIOS::getOFDPort(int &ofdPort)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getIntFromSettingsTable(ofdPort, PIRIT_TABLE_OFD_PORT, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t\t%sВЫПОЛНЕНО!Порт ОФД = %d", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), ofdPort);

    return ok;
}
//=====================================================================
// Вернуть IP ОФД
//=====================================================================
bool BIOS::getOFDAddress(string &ofdAddress)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(ofdAddress, PIRIT_TABLE_OFD_IP, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t%sВЫПОЛНЕНО!  Адрес ОФД = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), ofdAddress.c_str());

    return ok;
}
//=====================================================================
// Вернуть ИНН ОФД
//=====================================================================
bool BIOS::getOFDINN(string &ofdINN)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(ofdINN, PIRIT_TABLE_OFD_INN, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t%sВЫПОЛНЕНО!  ИНН ОФД = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), ofdINN.c_str());

    return ok;
}
//=====================================================================
// Вернуть адрес отправителя чеков
//=====================================================================
bool BIOS::getRecSenderEmail(std::string& recSenderEmail)
{
#ifdef BIOS_RB
    return true;
#endif
    std::lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(recSenderEmail, PIRIT_REC_SENDER_EMAIL, PIRIT_PARAM_0);
    bool ok = (res == 0) ? true : isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t%sВЫПОЛНЕНО!Адрес отправителя чеков = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), recSenderEmail.c_str());

    return ok;
}
//=====================================================================
// Вернуть дату окончания ресурса ФН
//=====================================================================
bool BIOS::getFSValidityDate(uint16_t &date)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);

    char data[32];
    memset(data, 0, sizeof(data));

    int res = getKKTInfo( PIRIT_KKT_INFO_FS_VALIDITY_DATE, data);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if ( ok )
    {
        date =QString(data).toULong();
    }

    logDBG("%s\t\t%sВЫПОЛНЕНО!  Ресурс ключей ФН = %d", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), date);
    return ok;
}
//=====================================================================
// Вернуть номер автомата
//=====================================================================
bool BIOS::getAutomatNum(string &number)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(number, PIRIT_TABLE_AUTOMAT_NUM, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t%sВЫПОЛНЕНО!  Номер автомата = %s", __PRETTY_FUNCTION__, (ok ? " " : "НЕ "), number.c_str());

    return ok;
}
//=====================================================================
// Версия ФФД ККТ
//=====================================================================
bool BIOS::getFfdKktVer(uint8_t &ffdKktVer)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int tmp = 0;

    int res = getIntFromInfoFN( PIRIT_INFO_FN_VERSION, PIRIT_VERSION_FFD_KKT, tmp);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR("%s\t\tНЕ ВЫПОЛНЕНО!",  __PRETTY_FUNCTION__ );
        return false;
    }
    ffdKktVer = static_cast<uint8_t>(tmp);
    logDBG("%s\t\t\tВЫПОЛНЕНО!  Версия ФФД ККТ = %d", __PRETTY_FUNCTION__, ffdKktVer);

    return true;
}
//=====================================================================
// Зарегистрированная версия ФФД ФН
//=====================================================================
bool BIOS::getFsFfdVer(uint8_t &fsFfdVer)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int tmp = 0;

    int res = getIntFromInfoFN( PIRIT_INFO_FN_VERSION, PIRIT_VERSION_REG_FFD_FN, tmp);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR("%s\t\tНЕ ВЫПОЛНЕНО!",  __PRETTY_FUNCTION__ );
        return false;
    }
    fsFfdVer = static_cast<uint8_t>(tmp);

    logDBG("%s\t\t\tВЫПОЛНЕНО!  Версия ФФД ФН = %d", __PRETTY_FUNCTION__, fsFfdVer);

    return true;
}
//=====================================================================
// Версия ККТ
//=====================================================================
bool BIOS::getKktVer(string &kktVer)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromInfoFN( PIRIT_INFO_FN_VERSION, PIRIT_VERSION_KKT, kktVer);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR("%s\t\tНЕ ВЫПОЛНЕНО!",  __PRETTY_FUNCTION__ );
        return false;
    }
    logDBG("%s\t\tВЫПОЛНЕНО!  Версия ККТ = %s", __PRETTY_FUNCTION__, kktVer.c_str());

    return true;
}
//=====================================================================
// Дата и время в ФН
//=====================================================================
bool BIOS::RUS::setFNDateTime(const time_t &date)
{
    logDBG("%s", __PRETTY_FUNCTION__);
    if(!convert)
    {
        logERR( "%s Null ptr!", __PRETTY_FUNCTION__ );
        return false;
    }

    lock_guard<mutex> locker(*mutex_rf);
    logDBG("%s Set date to FS\t:\t%s", __PRETTY_FUNCTION__, convert->getStrFullDateTime(date).c_str());

    int res = libSetFnDateTime(convert->getDateTime(&date));

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR("%s\t\tНЕ ВЫПОЛНЕНО!",  __PRETTY_FUNCTION__ );
        return false;
    }

    return ok;
}
//=====================================================================
// Дата и время из ФН
//=====================================================================
bool BIOS::RUS::getFNDateTime(Convert::DATE_ISO_8601 &dateIso)
{
    if(convert == nullptr)
    {
        logERR( "%s Null ptr!", __PRETTY_FUNCTION__ );
        return false;
    }
    unique_lock<mutex> locker(*mutex_rf);

    string _date;
    int res = getStrFromInfoFN( PIRIT_INFO_FN_DATETIME, PIRIT_DATETIME_DATE, _date);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR("%s\t\tНЕ ВЫПОЛНЕНО!",  __PRETTY_FUNCTION__ );
        return false;
    }

    string _time;
    res = getStrFromInfoFN( PIRIT_INFO_FN_DATETIME, PIRIT_DATETIME_TIME, _time);
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR("%s\t\tНЕ ВЫПОЛНЕНО!",  __PRETTY_FUNCTION__ );
        return false;
    }

    locker.unlock();

    string strDateTime = _date + _time;

    if ( convert->DateISO8601Create(dateIso, strDateTime, true) )
    {
        logDBG("%s\t\tВЫПОЛНЕНО!  Дата и время ФН = %s", __PRETTY_FUNCTION__, strDateTime.c_str());
        return true;
    }

    return false;
}

// Заполнить конфиг из PIRIT
bool BIOS::getSettingsFromPirit(Config &config)
{
    #ifdef BIOS_RB

        getOrganizationINN          ( config.organization_inn );
        getOrganizationAddress   ( config.calculation_address );
        getOrganizationName        ( config.organization_name );
        return true;
    #endif
    getOrganizationINN          ( config.organization_inn );
    getKKTRegNum                     ( config.kkt_reg_num );
    getTaxSystems                    ( config.tax_systems );
    getOrganizationAddress   ( config.calculation_address );
    getOrganizationName        ( config.organization_name );
    getCalculationPlace        ( config.calculation_place );
    getAutomatNum                    ( config.automatNumber );
    getOFDName                          ( config.ofd_name );
    getOFDPort                   ( config.ofd_server_port );
    getOFDAddress             ( config.ofd_server_address );
    getOFDINN                            ( config.ofd_inn );
    getRecSenderEmail(config.recSenderEmail);
    getFfdKktVer                     ( config.ffd_kkt_ver );
    getFsFfdVer                       ( config.fs_ffd_ver );
    getKktVer                    ( config.kkt_reg_version );
    getDateTimeOpenShift         ( config.open_shift_date );
    getFSValidityDate           ( config.fs_validity_date );
    getSignSettingsFromPirit                     ( config );
    // Записываем если фискальный или постфискальный режим
    if ( (RF->isFiscalMode_get() == true) || (RF->isPostFiscalMode_get() == true) )
    {
        config.stage = FISCAL;
    }

    return true;
}
//=====================================================================
// Заполнить признаки и статус агентов
//=====================================================================
bool BIOS::getSignSettingsFromPirit(Config &config)
{
#ifdef BIOS_RB
    return true;
#endif
    config.kkt_mode             =  0x00;
    config.kkt_signs            =  0x00;
    config.add_kkt_signs        =  0x00;
    config.agent_mask           =  0x00;
    config.encryption_sign      = false;
    config.clc_services_sign    = false;
    config.excisable_sign       = false;
    config.gambling_sign        = false;
    config.paying_agent_sign    = false;
    config.lottery_sign         = false;

    uint16_t signs = 0;
    getSignAndAgents( signs );

    uint16_t printer = 0;
    getPrinterMachine( printer );

    if ( printer != 0 )
    {
        config.add_kkt_signs |= RUS::PRINTER_MACHINE;
    }

    config.agent_mask = 0x01; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    for ( uint16_t bit = 1; bit != 0; bit = bit << 1 )
    {
        switch (signs & bit)
        {
        case 0x00001:
        {
            config.encryption_sign              = true;
            config.kkt_signs                   |= RUS::ENCRYPTION;
            break;
        }
        case 0x00002:
        {
            config.kkt_mode                     = 0x01;
            config.kkt_signs                   |= RUS::AUTONOMOUS;
            break;
        }
        case 0x00004:
        {
            config.kkt_signs                   |= RUS::AUTOMATIC;
            break;
        }
        case 0x00008:
        {
            config.clc_services_sign            = true;
            config.kkt_signs                   |= RUS::SERVICES;
            break;
        }
        case 0x00010:
        {
            config.bso                          = true;
            config.kkt_signs                   |= RUS::BCO;
            break;
        }
        case 0x00020:
        {
            config.kkt_signs                   |= RUS::INTERNET;
            break;
        }
        case 0x00040:
        {
            config.excisable_sign               = true;
            config.add_kkt_signs               |= RUS::EXCISABLE_GOODS;
            break;
        }
        case 0x00080:
        {
            config.gambling_sign                = true;
            config.add_kkt_signs               |= RUS::GAMBLING;
            break;
        }
//        case 0x00100: config.agent_mask        |= 0x01BANK_PAYING_AGENT;    break;
//        case 0x00200: config.agent_mask        |= BANK_PAYING_SUBAGENT; break;
//        case 0x00400: config.agent_mask        |= PAYING_AGENT;         break;
//        case 0x00800: config.agent_mask        |= PAYING_SUBAGENT;      break;
//        case 0x01000: config.agent_mask        |= ATTORNEY_AGENT;       break;
//        case 0x02000: config.agent_mask        |= COMMISION_AGENT;      break;
//        case 0x04000:
//        {
//            config.paying_agent_sign            = true;
//            config.agent_mask                  |= SIMPLE_AGENT;         break;
//        }
//        case 0x08000:
//        {
//            config.lottery_sign                 = true;
//            config.add_kkt_signs               |= RUS::LOTTERY;
//            break;
//        }
        default: break;
        }
    }

    return true;
}


bool BIOS::getBatteryVoltage(int &voltage)
{
    if( !isConnected.load() )
    {
        logERR("%s:  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    lock_guard<mutex> locker(mutex_bios);
    logDBG("getBatteryVoltage:: Запрос напряжения батарейки...");

    voltage = 0;
    int res = libGetBatteryVoltage(voltage);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);

    logWARN("%s:\t\tVбатарейки = %s mV", __PRETTY_FUNCTION__, to_string(voltage).c_str());

    return ok;

}

bool BIOS::restartKKT()
{
    lock_guard<mutex> locker(mutex_bios);
    logERR("%s Переинициализация БИОС...", __PRETTY_FUNCTION__);

    int res = libRestartKKT();
    return (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
}
//=====================================================================
// Запрос документа из архива
//=====================================================================
bool BIOS::getDocFromFN (int fdNumber, char *hexData, int lenBuf)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    logDBG("getDocFromFN:: Запрос документа №%d из архива ФН...", fdNumber);
    int res = libGetInfoFromECT_FP(fdNumber, hexData);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR("getDocFromFN::НЕ ВЫПОЛНЕНО! Запрос документа №%d из архива ФН... -- ERROR! -- длина = ", lenBuf);
        return false;
    }
    logDBG("getDocFromFN:: Запрос документа №%d из архива ФН... ОK! длина = ", lenBuf);

    return ( res == 0 ? true : false );
}


//=====================================================================
//  Отчёт о текущем состоянии расчётов
//=====================================================================
bool BIOS::RUS::calcStateReport( string cashier_name )
{
    lock_guard<mutex> locker(*mutex_rf);

    if (isDocOpen_get())
    {
        // Закрытие ранее открытого документа при его наличии
        cancelDoc();
    }

    int res = libCurrentStatusReport( (char*)cashier_name.c_str() );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if ( !ok )
    {
        logERR( "CALC STATE REPORT ERROR!!!" );
    }
    return ok;
}


//=====================================================================
//  Внесение / изъятие суммы
//=====================================================================
bool BIOS::RUS::insRes(OPEN_DOC *doc, double sum  )
{
    if(doc == nullptr)
    {
        logERR( "NULL DOC POINTER!!!" );
        return false;
    }

    lock_guard<mutex> locker(*mutex_rf);
    //---------------------------------------------------
    // Закрытие ранее открытого документа при его наличии
    if ( isDocOpen_get() )
    {
        cancelDoc();
        logWARN( "%s: Закрытие ранее открытого документа!", __PRETTY_FUNCTION__ );
    }
    //---------------------------------------------------
    if( openDoc( doc ) == false)
    {
        logERR( "OPEN DOC ERROR!!!" );
        return false;
    }
    bool ok = true;
    // Внесение/изъятие
    if (libCashInOut( "рубли", sum ) != 0x00)
    {
        logERR( "libCashInOut ERROR!!!" );
        ok =  false;
    }
    // Закрыть документ
    if(ok)
    {
        ok = closeDoc("buyerAddress@dreamkas.ru");
    }
    // если что то пошло не так аннулируем
    if( !ok )
    {
        cancelDoc();
    }
    return ok;
}


//=====================================================================
//  Внесение / изъятие суммы РБ
//=====================================================================
bool BIOS::BEL::insRes(BEL::OPEN_DOC *doc, double sum  )
{
    if(doc == nullptr)
    {
        logERR( "NULL DOC POINTER!!!" );
        return false;
    }

    lock_guard<mutex> locker(*mutex_rb);
    //---------------------------------------------------
    // Закрытие ранее открытого документа при его наличии
    if ( isDocOpenedGet() )
    {
        cancelDoc();
        logWARN( "%s: Закрытие ранее открытого документа!", __PRETTY_FUNCTION__ );
    }

    if( openDoc( doc ) == false)
    {
        logERR( "OPEN DOC ERROR!!!" );
        return false;
    }

    bool ok = true;
    // Внесение/изъятие
    if (libCashInOut( "рубли", sum ) != 0x00)
    {
        logERR( "libCashInOut ERROR!!!" );
        ok =  false;
    }
    // Закрыть документ
    if(ok)
    {
        ok = closeDoc();
    }
    // если что то пошло не так аннулируем
    if( !ok )
    {
        cancelDoc();
    }
    return ok;
}


//=====================================================================
// Закрыть ФН
//=====================================================================
bool BIOS::RUS::closeFS( string cashier )
{
    lock_guard<mutex> locker(*mutex_rf);
    uint32_t fd = 0;
    uint32_t fpd = 0;
    string dateTimeCloseFN;
    int res = libCloseFN (cashier.c_str(), fd, fpd, dateTimeCloseFN);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR( "CLOSE RF ERROR!!!" );
    }
    return ok;
}

void BIOS::RUS::fillErrorCodes(const FB_ERROR_CODE& code)
{
    switch (code)
    {
        case FB_ERROR_MEMORY_TO_OFD_EXHAUSTED: logERR("----- SET FLAG OFD MEMORY EXHAUSTED !!!!! "); memoryToOfdExhausted.store(true); break;
        default:
            break;
    }
}

//=====================================================================
// Закрытие Документа
//=====================================================================
bool BIOS::RUS::closeDoc(RUS::RECEIPT& receipt)
{
    logDBG("Закрытие Документа...");

    if (!isConnected)
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }
    CloseDocParam closeDocParam;
    closeDocParam.cutPaper = 0x01;
    closeDocParam.buyerAddress = receipt.buyerAddress;
    closeDocParam.clcPlace = receipt.clcPlace;
    closeDocParam.buyerName = receipt.buyerName;
    closeDocParam.buyerInn = receipt.buyerInn;
    closeDocParam.addUserReq.first = receipt.addUserReqName;
    closeDocParam.addUserReq.second = receipt.addUserReqValue;
    CloseDocResult closeRes(libCloseDocument(closeDocParam));

    bool ok((closeRes.errCode == 0) ?  true :  isCommandComplete(closeRes.errCode, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL));

    if (ok)
    {
        logDBG("Документ закрыт! Код ответа = %d. Сообщение ='%s'", closeRes.errCode, strFromErrCode(closeRes.errCode).c_str());
        receipt.fd         = closeRes.fd;
        receipt.fpd        = closeRes.fpd;
        receipt.shiftNum   = closeRes.shiftNum;
        receipt.docInShift = closeRes.docInShift;
        receipt.dateDoc    = closeRes.dateDoc;
        receipt.timeDoc    = closeRes.timeDoc;
    }
    else
    {
        logERR("Документ НЕ закрыт! Код ответа = %d. Сообщение ='%s'", closeRes.errCode, strFromErrCode(closeRes.errCode).c_str());
        fillErrorCodes(static_cast<FB_ERROR_CODE>(closeRes.errCode));
    }

    return ok;
}


bool BIOS::RUS::closeDoc(const string &buyerAddress)
{
    logDBG("Закрытие Документа...");

    if (!isConnected)
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    CloseDocResult closeRes(libCloseDocumentLite(1, buyerAddress, 0));

    bool ok((closeRes.errCode == 0) ?  true :  isCommandComplete(closeRes.errCode, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL));

    if (ok)
    {
        logDBG("Документ закрыт! Код ответа = %d. Сообщение ='%s'", closeRes.errCode, strFromErrCode(closeRes.errCode).c_str());
    }
    else
    {
        logERR("Документ НЕ закрыт! Код ответа = %d. Сообщение ='%s'", closeRes.errCode, strFromErrCode(closeRes.errCode).c_str());
        fillErrorCodes(static_cast<FB_ERROR_CODE>(closeRes.errCode));
    }

    return ok;
}

bool BIOS::BEL::closeDoc()
{
    logDBG("Закрытие Документа...");
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }
    int res = libCloseDocument_By( 1 ).errCode;    //  Флаг отрезки
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("Документ %s закрыт! Код ответа = %d. Сообщение ='%s'", (ok ? " " : "НЕ "), res, strFromErrCode(res).c_str());
    return ok;
}
//=====================================================================
// Аннулирование документа
//=====================================================================
bool BIOS::RUS::cancelDoc()
{
    logWARN("Аннулирование Документа...");
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }
    int res = libCancelDocument();
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if(ok)
    {
        logDBG("Документ закрыт! Код ответа = %d", res);
    }
    else
    {
        logERR("Документ НЕ закрыт! Код ответа = %d", res);
    }

    return ok;
}
bool BIOS::BEL::cancelDoc()
{
    logWARN("Аннулирование Документа...");
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }
    int res = libCancelDocument();
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("Документ %s закрыт! Код ответа = %d", (ok ? " " : "НЕ "), res);
    return ok;
}
//=====================================================================
// Дата регистрации
//=====================================================================
bool BIOS::RUS::getDateOfReg( string &dateTime )
{
    lock_guard<mutex> locker(*mutex_rf);

    logDBG("Получение даты регистрации...");
    if( !isConnected )
    {
        logERR("BIOS::getDateOfReg()::  Ошибка! Соединение не проинициализированно!");
        return false;
    }
    char dt[40];
    memset(dt, 0, sizeof(dt));
    int res = libGetRegDateTime(dt);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    dateTime = dt;

    if(ok)
    {
        logDBG("BIOS::getDateOfReg()::ВЫПОЛНЕНО Дата регистрации:'%s'! Код ответа = %d",  dateTime.c_str(), res);
    }
    else
    {
        logERR("BIOS::getDateOfReg()::НЕ ВЫПОЛНЕНО Дата регистрации:'%s'! Код ответа = %d", dateTime.c_str(), res);
    }

    return ok;
}
//=====================================================================
// Вернуть номер ФД последней регистрации
//=====================================================================
bool BIOS::RUS::getNumFDofReg( int &fdNumberReg )
{
    lock_guard<mutex> locker(*mutex_rf);

    logDBG("Получение номера ФД регистрации...");
    if( !isConnected )
    {
        logERR("BIOS::getNumFDofReg()::  Ошибка! Соединение не проинициализированно!");
        return false;
    }
    int res = libGetRegFDNumber(fdNumberReg);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("BIOS::getDateOfReg()::%s Номер ФД регистрации = %d. Код ответа = %d", (ok ? " " : "НЕ ВЫПОЛНЕНО!!!!"), fdNumberReg, res);
    return ok;
}


//=====================================================================
// Выгрузка ФД по номру из ФН
//=====================================================================
bool BIOS::RUS::fiscalDocBinUnload         (  int          fd_number,
                                              char         **doc_data,
                                              int              &size,
                                              bool          &isCheck,
                                              int           &docType)
{
    lock_guard<mutex> locker(*mutex_rf);
    logDBG("BIOS::fiscalDocBinUnload():: Получение документа из ФН по номеру(%d)...", fd_number);
    if( !isConnected )
    {
        logERR("BIOS::fiscalDocBinUnload():: Ошибка! Соединение не проинициализированно!");
        return false;
    }
    int res = libGetFnDocBin(   fd_number,   // Номер документа
                                 doc_data,   // Тело  документа
                                     size ,  // Длина документа
                                  isCheck,   // Это квитанция
                                  docType);  // Тип выгружаемого документа
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    return ok;
}

bool BIOS::RUS::setPosAddReq(string requisite)
{
    lock_guard<mutex> locker(*mutex_rf);
    logDBG("%s", __PRETTY_FUNCTION__);

    if(!isConnected)
    {
        logERR("%s Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libSetPosAddReq(requisite);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    return ok;
}

//=====================================================================
// insert 4 bytes LE <тип><тип><длина><длина>
bool BIOS::RUS::addSTLVheader(std::vector <uint8_t>& dest, const uint16_t type, const uint16_t len) const
{
    if( (convert == nullptr) )
    {
        logERR( "%s Null ptr!", __PRETTY_FUNCTION__ );
        return false;
    }


    logINFO("INSERT type = %u, LEN = %u",  static_cast<uint32_t>(type), static_cast<uint32_t>(len) );
    dest.insert(dest.begin(), convert->getCharsLE(len).begin(),    convert->getCharsLE(len).end() );
    dest.insert(dest.begin(), convert->getCharsLE(type).begin(),   convert->getCharsLE(type).end() );
    return true;
}

//=====================================================================
// Вернуть STLV последней регистрации/перерегистрации
bool BIOS::RUS::getSTLVofReg(vector <uint8_t>& stlvReg, int&  typeFd)
{
    logINFO("%s:: Получение TLV структуры регистрации/перерегистрации...", __func__);
    if( !stlvReg.empty() )
    {
        stlvReg.clear();
    }
#ifdef BIOS_RB
    logERR("%s:: Выход! Метод только для РФ!", __func__);
    return false;
#endif
    if (mutex_rf == nullptr)
    {
        logERR("%s, null pointer!", __PRETTY_FUNCTION__);
        return false;
    }

    int     lenOfTLV  {0};
    char    tlv[1000];
    const bool READ_REGISTRATION{true};
    PIRIT_REG_DOC_TYPE regType{PIRIT_REG_DOC_TYPE::PIRIT_REG_DOC_TYPE_UNDEFINED};

    lock_guard<mutex> locker(*mutex_rf);
    if( !isConnected )
    {
        logERR("%s::  Ошибка! Соединение не проинициализированно!", __func__);
        return false;
    }

    //--------------------------------
    // 1 Читаем признаки ТЛВ структуры
    int res = libGetRegSTLV( regType );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok  || (regType == PIRIT_REG_DOC_TYPE::PIRIT_REG_DOC_TYPE_UNDEFINED) )
    {
        logERR("%s:: Ошибка! Не удалось выгрузить признаки СТЛВ структуры регистрации!", __func__);
        return false;
    }
    typeFd = static_cast<int>(regType);
    logWARN("%s:: Answer = %d, registration type = %d", __func__, res, typeFd );

    //--------------------------------
    // 2 Читаем сами ТЛВ до 0х48 - конец из ФН
    // Пока можно читать
    while( libGetRegistrTLV(tlv, lenOfTLV, READ_REGISTRATION) == 0 )
    {
        if(lenOfTLV !=0 )
        {
            ok = Convert::convertASCIImassToHex(tlv, lenOfTLV);
            if( !ok )
            {
                logERR("%s:: Ошибка во время конверта 2б хекса в 1б хекс", __func__ );
                stlvReg.clear();
                return false;
            }
            std::vector <uint8_t> newTLV = {tlv, tlv + lenOfTLV};
            stlvReg.insert( stlvReg.end(), newTLV.begin(), newTLV.end() );
            lenOfTLV = 0;
        }
    }


    logINFO("%s:: registration STLV readed! len = %u", __func__, stlvReg.size() );
    return true;
}

//=====================================================================
// Вернуть номер ФД и ФПД последней регистрации/перерегистрации
//=====================================================================
bool BIOS::RUS::getFDandFPDofReg( int      &numberOfFD,
                                  uint32_t     &FPDnum)
{
    vector<Requisites::Requisite>      reqList;
    int     lenOfTLV   = 0;
    int     lenTotal   = 0;
    char    tlv[1000];

    lock_guard<mutex> locker(*mutex_rf);
    logDBG("Получение TLV структуры регистрации/перерегистрации...");
    if( !isConnected )
    {
        logERR("BIOS::getFDandFPDofReg()::  Ошибка! Соединение не проинициализированно!");
        return false;
    }

    //--------------------------------
    // 1 Читаем признаки ТЛВ структуры
    int typeFd{0};
    int res = libGetRegistrSTLVProps( numberOfFD, lenTotal, typeFd );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("%s:: Длина ОБЩАЯ TLV структуры  = %d. Номер ФД = %d. Код ответа = %d", __func__, lenTotal, numberOfFD, res);
    if( !ok )
    {
        return false;
    }

    //--------------------------------
    // 2 Читаем сами ТЛВ до 0х48 - конец из ФН
    int i = 0;
    // Пока можно читать
    while( res == 0 )
    {
        i++;
        res = libGetRegistrTLV(tlv, lenOfTLV);

        ok = Convert::convertASCIImassToHex(tlv, lenOfTLV);
        if( !ok )
        {
            return false;
        }
        //--------------------------------
        // Парсим ТЛВ в вектор реквизитов(+1 элемент)
        uint16_t offs = 0;
        if( !tlv_parse((unsigned char*)tlv, &reqList, offs) )
        {
            logERR( "%s:: Выход, ошибка во время парсинга ТЛВ!", __func__ );
            return false;
        }

        //--------------------------------
        // Если выгрузили нужный тег, то прерываемся, тк тратим кучк времени
        if(reqList.back().tag == 1077) // FPD tag
        {
            res = 8888; // break;
        }
        //--------------------------------
    }
    // Поиск реквизита в таблице
    int size = reqList.size();

    FPDnum = 0;
    for (int i = 0; i < size; i++)
    {
        if(reqList.at(i).tag == 1077) // FPD tag
        {
            unsigned char *a = (unsigned char*)&FPDnum;
            a[0] = reqList.at(i).value_byte[3];
            a[1] = reqList.at(i).value_byte[2];
            a[2] = reqList.at(i).value_byte[1];
            a[3] = reqList.at(i).value_byte[0];
        }
    }
    reqList.clear();
    return FPDnum > 0 ? true : false;
}

//=====================================================================
//bool BIOS::RUS::setNVR(NVR &nvr)
//{
//    uint16_t offset = 0;

//    char *src = (char*)&nvr;
//    const uint16_t SIZE = sizeof(nvr);
//    const uint16_t INIT_ADDRESS = 0xf000;
//    const uint16_t ASCII_MAX_SIZE = SetNVRcmd::MAX_SIZE / SetNVRcmd::ASCII_BYTE_LEN;
//    logWARN("%s:: Необходимо отправить NVR размером %u байт!", __func__, SIZE);

//    while (offset < SIZE)
//    {
//        uint16_t address = INIT_ADDRESS + offset;
//        uint16_t size = SIZE - offset;
//        if (size > ASCII_MAX_SIZE)
//        {
//            size = ASCII_MAX_SIZE;
//        }
//        logWARN("%s Address: 0x%04x", __FUNCTION__, address);
//        logWARN("%s Offset: %u", __FUNCTION__, offset);
//        logWARN("%s Size: %u", __FUNCTION__, size);

//        const uint16_t REAL_SIZE = size * SetNVRcmd::ASCII_BYTE_LEN;
//        char sendStr[REAL_SIZE];
//        memcpy(sendStr, convert->hex2ASCII(src + offset, size).c_str(), REAL_SIZE);
//        SetNVRcmd setNVRcmd(address, sendStr, REAL_SIZE);

//        if (libSetNVR(setNVRcmd) != 0)
//        {
//            logERR("%s:: Ошибка! выход из цикла отправки пакетов NVR в Пуникс!", __func__);
//            return false;
//        }
//        offset += size;
//    }

//    logWARN("%s:: Отправка NVR успешно выполнена!", __func__);
//    return true;
//}
//=====================================================================
// Вернуть ФД и ФПД документа из ФН по номеру
//=====================================================================
bool BIOS::RUS::getDocSTLV( const int                     &numberOfFD,
                            vector<Requisites::Requisite> &reqList)
{
    int     lenOfTLV   = 0;
    int     lenTotal   = 0;
    char    tlv[1000] = {0};

    if (mutex_rf == nullptr)
    {
        logERR("%s, null pointer!", __PRETTY_FUNCTION__);
        return false;
    }

    lock_guard<mutex> locker(*mutex_rf);
    logDBG("%s, Получение STLV документа", __PRETTY_FUNCTION__);
    if( !isConnected )
    {
        logERR("%s, Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    //--------------------------------
    // 1 Читаем признаки CТЛВ структуры
    int typeFd{0};
    int res = libGetDocSTLV( numberOfFD, lenTotal, typeFd );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("%s, Длина ОБЩАЯ STLV структуры  = %d. Номер ФД = %d. Код ответа = %d", __PRETTY_FUNCTION__, lenTotal, numberOfFD, res);
    if( !ok )
    {
        return false;
    }

    //--------------------------------
    // 2 Читаем сами ТЛВ до 0х48 - конец из ФН
    // Пока можно читать
    while (libGetRegistrTLV(tlv, lenOfTLV) == 0)
    {
        if (!Convert::convertASCIImassToHex(tlv, lenOfTLV))
        {
            return false;
        }
        //--------------------------------
        // Парсим ТЛВ в вектор реквизитов(+1 элемент)
        uint16_t offs(0);
        if (!tlv_parse((unsigned char*)tlv, &reqList, offs))
        {
            logERR( "%s:: Выход, ошибка во время парсинга ТЛВ!", __func__ );
            return false;
        }
        //--------------------------------
    }

    return true;
}

//=====================================================================
// Вернуть STLV документа из ФН по номеру
//=====================================================================
bool BIOS::RUS::getDocSTLV( const uint32_t&   numberOfFD,
                            vector <uint8_t>&       stlv,
                            int&                  typeFd)
{
    logINFO("%s:: Получение TLV структуры документа по ФД( #%u )...", __func__, numberOfFD);
    stlv.clear();
#ifdef BIOS_RB
    logERR("%s:: Выход! Метод только для РФ!", __func__);
    return false;
#endif
    int     lenOfTLV  {0};
    int     lenTotal  {0};
    char    tlv[1000];

    if (mutex_rf == nullptr)
    {
        logERR("%s, null pointer!", __PRETTY_FUNCTION__);
        return false;
    }

    lock_guard<mutex> locker(*mutex_rf);
    logINFO("%s, Получение STLV документа", __PRETTY_FUNCTION__);
    if( !isConnected )
    {
        logERR("%s, Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    //--------------------------------
    // 1 Читаем признаки CТЛВ структуры
    int res = libGetDocSTLV( numberOfFD, lenTotal, typeFd );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logINFO("%s, Длина ОБЩАЯ STLV структуры  = %d. Номер ФД = %d. Код ответа = %d", __PRETTY_FUNCTION__, lenTotal, numberOfFD, res);
    if( !ok )
    {
        logERR("%s:: Ошибка! Не удалось выгрузить признаки СТЛВ структуры документа(ФД = %d)!", __func__, numberOfFD);
        return false;
    }

    //--------------------------------
    // 2 Читаем сами ТЛВ до 0х48 - конец из ФН
    // Пока можно читать
    while (libGetRegistrTLV(tlv, lenOfTLV) == 0)
    {
        if (!Convert::convertASCIImassToHex(tlv, lenOfTLV))
        {
            return false;
        }
        //--------------------------------
        // Парсим ТЛВ в вектор реквизитов(+1 элемент)
        std::vector <uint8_t> newTLV = {tlv, tlv + lenOfTLV};
        stlv.insert( stlv.end(), newTLV.begin(), newTLV.end() );
        lenOfTLV = 0;
    }

    return true;
}

//=====================================================================
// Прас в вектор реквизитов ТЛВ структуры из биос из ФН
//=====================================================================
bool BIOS::RUS::stlv_parse ( unsigned char *stlv,
                               vector <Requisites::Requisite> *requisites )
{
    const int     TAG_OFFSET   = 0,
                  LEN_OFFSET   = 2,
                  VALUE_OFFSET = 4;
    // Парсер STLV структуры
    logDBG( "STLV PARSE ..." );

    uint32_t                   offset; // Смещение
    uint16_t                      tag, // Тэг
            len; // Длинна

    Requisites::Requisite   requisite; // Реквизит

    if (stlv == NULL)
    {
        logERR( "NULL STLV POINTER!" );
        return EXIT_FAILURE;
    }

    if (requisites == NULL)
    {
        logERR( "NULL REQUISITES LIST POINTER!" );
        return EXIT_FAILURE;
    }

    // Парсинг тэга
    offset = TAG_OFFSET;
    memcpy( &tag, &stlv[offset], sizeof(tag) );
    logDBG( "STLV TAG = %u", tag );
    // Поиск реквизита в таблице
    if (REQ.req_search( tag, &requisite ) == EXIT_FAILURE)
    {
        // Реквизит не найден
        return EXIT_FAILURE;
    }
    // Парсинг длинны
    offset = LEN_OFFSET;
    memcpy( &len, &stlv[offset], sizeof(len) );
    logDBG( "TLV LEN = %u", len );
    offset = VALUE_OFFSET;

    // Тест на тип реквизита
    if (requisite.type != Requisites::STLV)
    {
        // Неверный тип реквизита
        logERR( "WRONG REQUISITE TYPE!" );
        return EXIT_FAILURE;
    }

    std::vector<Requisites::Requisite> tlvReq;

    while( offset <= len  )
    {
        // Парсинг данных (TLV)
        uint16_t offs = 0;
        if (!tlv_parse(&stlv[offset], &tlvReq, offs))
        {
            logERR( "%s:: Выход, ошибка во время парсинга ТЛВ!", __func__ );
            return EXIT_FAILURE;
        }
        offset +=offs;
        offset += /*TAG_LEN + LEN_LEN*/2 + 2;
    }

    if (offset != (unsigned int)(/*TAG_LEN + LEN_LEN*/2 + 2 + len))
    {
        logERR( "STLV PARSE FAILED!" );
        return EXIT_FAILURE;
    }

    logDBG( "STLV PARSE FINISHED" );
    requisite.requisites = tlvReq;
    requisites->push_back(requisite);

    return EXIT_SUCCESS;
}


//=====================================================================
// Прас в вектор реквизитов ТЛВ структуры из биос из ФН
//=====================================================================
bool BIOS::RUS::tlv_parse  ( unsigned char  *tlv,
                             vector <Requisites::Requisite> *requisites, uint16_t &retOffset )
{
    const int     TAG_OFFSET   = 0,
                  LEN_OFFSET   = 2,
                  VALUE_OFFSET = 4;

    // Парсер TLV структуры
    logDBG( "TLV PARSE ..." );

    uint32_t                        offset; // Смещение
    uint16_t                           tag, // Тэг
                                       len; // Длина
    const uint16_t          FPD_TAG = 1077;

    Requisites::Requisite        requisite; // Реквизит

    if (tlv == NULL)
    {
        logERR( "NULL TLV POINTER!" );
        return false;
    }

    if (requisites == NULL)
    {
        logERR( "NULL REQUISITES LIST POINTER!" );
        return false;
    }
//    requisites->clear();

    retOffset = 0;
    // Парсинг тэга
    offset = TAG_OFFSET;
    memcpy( &tag, &tlv[offset], sizeof(tag) );
    logDBG( "TLV TAG = %u", tag );

    // Поиск реквизита в таблице
    if (REQ.req_search( tag, &requisite ) == EXIT_FAILURE)
    {
        // Реквизит не найден
        logERR( "REQUISITE MISSED! %ud", tag );
        return false;
    }

    // Парсинг длинны
    offset = LEN_OFFSET;
    memcpy( &len, &tlv[offset], sizeof(len) );
    logDBG( "TLV LEN = %u", len );
    offset = VALUE_OFFSET;

    // Парсинг данных
    // Заполнение реквизита
    requisite.cur_len = len;

    switch (requisite.type)
    {
        case Requisites::ASCII:
            //Сохранение строки
            logDBG( "PARSING INTO ASCII ... " );
            requisite.value_str = new unsigned char[requisite.cur_len];
            memset( requisite.value_str,            0, requisite.cur_len );
            memcpy( requisite.value_str, &tlv[offset], requisite.cur_len );
            break;

        case  Requisites::INT_32LE:
        case Requisites::UNIX_TIME:
            //Сохранение числа int32 (4 байта)
            logDBG( "PARSING INTO INT32LE OR UNIXTIME ... " );
            if (requisite.cur_len < sizeof(int32_t))
            {
                logERR( "WRONG LEN! REQUISITE MISSED!" );
                retOffset = requisite.cur_len;
                return true;
            }
            memcpy( &requisite.value_int, &tlv[offset], sizeof(int32_t) );
            break;

        case Requisites::INT_16LE:
            //Сохранение числа int16 (2 байта)
            logDBG( "PARSING INTO INT16LE ... " );
            if (requisite.cur_len < sizeof(int16_t))
            {
                logERR( "WRONG LEN! REQUISITE MISSED!" );
                retOffset = requisite.cur_len;
                return true;
            }
            memcpy( &requisite.value_int, &tlv[offset], sizeof(int16_t) );
            break;

        case Requisites::BYTE:
        case  Requisites::VLN:
        case Requisites::FVLN:
            logDBG( "PARSING INTO BYTES OR VLN OR FVLN ... " );
            unsigned short    len_b,
                           offset_b;
            if (tag == FPD_TAG)
            {
                len_b    = requisite.cur_len - 2;
                offset_b =      VALUE_OFFSET + 2;
            }
            else
            {
                len_b    =     requisite.cur_len;
                offset_b =          VALUE_OFFSET;
            }
            requisite.value_byte =       new unsigned char[len_b];
            memset( requisite.value_byte,              0, len_b );
            memcpy( requisite.value_byte, &tlv[offset_b], len_b );
            break;

        case Requisites::STLV:
            // TLV не может содержать STLV
            logDBG( "STLV REQUISITE FOUNDED");
            stlv_parse( tlv, requisites );
            break;

        default:
            logERR( "UNKNOWN REQUISITE TYPE! REQUISITE MISSED!" );
            retOffset = requisite.cur_len;
            return true;
    }

    // Сохранение реквизита в лист
    if (requisite.type != Requisites::STLV)
    {
        requisites->push_back( requisite );
    }
    logDBG( "TLV PARSE %u bytes FINISHED", requisite.cur_len );

    retOffset = requisite.cur_len;
    return true;
}


//=====================================================================
// get FD and FPD of last reg/rereg report
//=====================================================================
bool BIOS::RUS::getLastRegInfo(uint32_t &fd, uint32_t &fpd )
{
    if (mutex_rf == nullptr)
    {
        logERR("%s:: null pointer!", __func__);
        return false;
    }
    lock_guard<mutex> locker(*mutex_rf);

    logDBG("getLastRegInfo... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libGetLastRegData(fd, fpd);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if(ok)
    {
        logDBG("Данные выгружены! ");
    }
    else
    {
        logERR("Данные Не выгружены! Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());
        fd = 0;
        fpd = 0;
    }
    return ok;
}

//=====================================================================
// Закрытие смены в БИОС + Z отчёт
//=====================================================================
bool BIOS::RUS::closeShift(string cashier)
{
    lock_guard<mutex> locker(*mutex_rf);

    logDBG("Закрытие смены... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    if (isDocOpen_get())
    {
        // Закрытие ранее открытого документа при его наличии
        cancelDoc();
    }

    int res = libPrintZReport(cashier.c_str(), 0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if(ok)
    {
        logDBG("Смена закрыта!Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());
    }
    else
    {
        logERR("Смена НЕ закрыта!Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());
    }
    return ok;
}
//=====================================================================
bool BIOS::BEL::closeShift( string cashier )
{
    lock_guard<mutex> locker(*mutex_rb);

    logDBG("Закрытие смены... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }
    int res = libPrintZReport_By(cashier.c_str());
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("Смена %s закрыта!Код ответа = %d. Сообщение ='%s'","НЕ ", res, strFromErrCode(res).c_str());
    return ok;
}

//=====================================================================
// Аннулирование документа РБ
//=====================================================================
bool BIOS::BEL::cancelDocBel(const uint32_t& docNum, string cashier, double (&payType)[BIOS::MAX_PAY_TYPES])
{
    lock_guard<mutex> locker(*mutex_rb);

    logDBG("Аннулирование документа... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libCancelDocument_By( docNum, cashier, payType );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("Документ %s аннулирован! Код ответа = %d. Сообщение ='%s'", (ok ? "" : "НЕ"), res, strFromErrCode(res).c_str());

    return ok;
}
//=====================================================================
// Получить номер текущей смены
//=====================================================================
bool BIOS::BEL::curShiftNum(uint64_t &shiftNum)
{
    lock_guard<mutex> locker(*mutex_rb);

    logDBG("Запрос номера текущей смены... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libGetCounters_CurShiftNum(shiftNum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("Номер смены %s получен! Код ответа = %d. Сообщение ='%s'", (ok ? "" : "НЕ"), res, strFromErrCode(res).c_str());

    return ok;
}

//=====================================================================
// Добавить позицию в открытый документ
//=====================================================================
bool BIOS::RUS::addPositionToCheck( Position *pos )
{
    logDBG("Добавление позиции...");
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }
    if(pos == NULL)
    {
        logERR( "NULL POSITION ERROR!!!" );
        return false;
    }

    int  res =    0;
    bool ok  = true;

    if (!pos->productCode.empty()/* || (pos->agent.getAgentType() != AGENT_TYPE::OFF)*/ || !pos->punix.addPosReq.empty() )
    {
        //ok = addExtraReq( *pos );
    }

    if( ok )
    {
        res = libAddPosition(pos->punix);

        ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    }
    logDBG("Добавление позиции %s завершено!Код ответа = %d. Сообщение ='%s'", (ok ? "" :"НЕ"), res, strFromErrCode(res).c_str());
    return ok;
}


//=====================================================================
// Подытог  заканчивает ввод товаров в чеке. Далее мы можем распечатать дополнительные реквизиты и т.д.
//=====================================================================
bool BIOS::RUS::subTotal()
{
    logDBG("Подытог чека...");
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libSubTotal();
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("Подытог чека %s завершен!Код ответа = %d. Сообщение ='%s'","НЕ ", res, strFromErrCode(res).c_str());
    return ok;
}


//=====================================================================
// Добавить скидку в рублях на чек
//=====================================================================
bool BIOS::RUS::addDiscountRubles(string nameDiscount, uint64_t sum )
{
    logDBG("Добавление скидки на чек... ");

    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }
    if( sum < 0 )
    {
        logERR("%s  Ошибка! Неверная сумма = %d", __PRETTY_FUNCTION__, sum);
        return false;
    }
    if( nameDiscount.empty() )
    {
        logERR("%s  Ошибка! Отсутствует название скидки!", __PRETTY_FUNCTION__);
        return false;
    }


    int res = 0;//libAddDiscount    (Positions::DISCOUNT_TYPE_ABSOLUTE, nameDiscount.c_str(), sum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("Скидка %s добавлена! Код ответа = %d. Сообщение ='%s'", ok ?  "" : "НЕ ", res, strFromErrCode(res).c_str());
    return ok;
}


//=====================================================================
// Добавить наценку в рублях на чек
//=====================================================================
bool BIOS::RUS::addMarginRubles(string nameMargin, int sum )
{
    logDBG("Добавление наценки на чек... ");
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }
    if( sum < 0 )
    {
        logERR("%s  Ошибка! Неверная сумма = %d", __PRETTY_FUNCTION__, sum);
        return false;
    }
    if( nameMargin.empty() )
    {
        logERR("%s  Ошибка! Отсутствует название наценки!", __PRETTY_FUNCTION__);
        return false;
    }

    // const int MARGIN_PERSENTS = 0;  для РФ нет
    const int MARGIN_RUBLES   = 1;

    int res = libAddMargin    (MARGIN_RUBLES, nameMargin.c_str(), sum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("Наценка %s добавлена! Код ответа = %d. Сообщение ='%s'", ok ?  "" : "НЕ ", res, strFromErrCode(res).c_str());
    return ok;
}


//=====================================================================
// Добавить позицию в белорусский чек
//=====================================================================
bool BIOS::BEL::addPositionToCheck( POSITION *pos )
{
    logDBG("Добавление позиции...");
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }
    if(pos == NULL)
    {
        logERR( "NULL POSITION ERROR!!!" );
        return false;
    }

    int res = libAddPositionLarge_By( pos->name.c_str(),
                                      pos->barcode.c_str(),
                                      pos->quantity,
                                      pos->price,
                                      pos->taxNumber,
                                      pos->numGoodsPos,
                                      pos->numDepart );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("Добавление позиции %s завершено!Код ответа = %d. Сообщение ='%s'", ok ?  "" : "НЕ ", res, strFromErrCode(res).c_str());
    return ok;
}


//=====================================================================
// Подытог  заканчивает ввод товаров в чеке. Далее можно установить скидки/наценки на весь чек
//=====================================================================
bool BIOS::BEL::subTotal()
{
    logDBG("Подытог чека...");
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libSubTotal();
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("Подытог чека %s завершен!Код ответа = %d. Сообщение ='%s'", ok ?  "" : "НЕ ", res, strFromErrCode(res).c_str());
    return ok;
}


////=====================================================================
//// Добавить скидку на позицию(после доб позиции0x42 ) или чек(после Подытог0x44 ) в РБ
////=====================================================================
//bool BIOS::BEL::addDiscountRubles(string nameDiscount, uint64_t sum, Positions::DISCOUNT_TYPE_BIOS       discType )
//{
//    logDBG("Добавление скидки на позицию/чек... ");
//    if( !isConnected )
//    {
//        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
//        return false;
//    }

//    if( nameDiscount.empty() )
//    {
//        logERR("%s  Ошибка! Отсутствует название скидки!", __PRETTY_FUNCTION__);
//        return false;
//    }

//    int res = libAddDiscount    ((unsigned char)discType, nameDiscount.c_str(), sum );

//    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

//    logDBG("Скидка %s добавлена! Код ответа = %d. Сообщение ='%s'", ok ?  "" : "НЕ ", res, strFromErrCode(res).c_str());
//    return ok;
//}

//=====================================================================
// Добавить наценку на позицию(после доб позиции0x42 ) или чек(после Подытог0x44 ) в РБ
//=====================================================================
bool BIOS::BEL::addMarginRubles(string nameMargin, uint64_t sum )
{
    logDBG("Добавление наценки на позицию/чек... ");
    if( !isConnected )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    if( nameMargin.empty() )
    {
        logERR("%s  Ошибка! Отсутствует название наценки!", __PRETTY_FUNCTION__);
        return false;
    }

    // const int MARGIN_PERSENTS = 0;
    const int MARGIN_RUBLES   = 1;

    int res = libAddMargin(MARGIN_RUBLES, nameMargin.c_str(), sum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("Наценка %s добавлена! Код ответа = %d. Сообщение ='%s'", ok ?  "" : "НЕ ", res, strFromErrCode(res).c_str());
    return ok;
}

//=====================================================================
// Добавить оплату  в открытый документ
//=====================================================================
bool BIOS::addPaymentToCheck(PAYMENT *payment)
{
    logDBG("Добавление оплаты... ");
    if( !isConnected.load() )
    {
        logERR("%s  Ошибка! Соединение не проинициализированно!", __PRETTY_FUNCTION__);
        return false;
    }

    if(payment == NULL)
    {
        logERR( "NULL PAYMENT POINTER!!!" );
        return false;
    }

    int res = libAddPayment( payment->typePayment,
                             payment->sum,
                             payment->infoStr.c_str() );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);


    logERR("Оплата %s добавлена! Код ответа = %d. Сообщение ='%s'",ok ?  "" : "НЕ ", res, strFromErrCode(res).c_str());
    return ok;
}
//=====================================================================
// Print ascii text like QR code
//=====================================================================
bool BIOS::RUS:: printQRcode(const std::string& data) const
{
    constexpr uint8_t REG    {0x01};
    constexpr uint8_t WIDTH  {0x08};
    constexpr uint8_t HEIGHT {0xFF};
    constexpr uint8_t TYPE_QR{0x08};

    lock_guard<mutex> locker(*mutex_rf);

    int res{libPrintBarCode( REG, WIDTH, HEIGHT, TYPE_QR, data.c_str())};
    scrollPaper();
    scrollPaper();
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    return ok;
}
//=====================================================================
// Запись параметров чека
//=====================================================================
int BIOS::setPurchaseMode(const uint8_t &mask)
{
    lock_guard<mutex> locker(mutex_bios);
    logDBG("%s", __PRETTY_FUNCTION__);
    int res(libWriteSettingsTable(PIRIT_TABLE_SETTINGS_PURCHASE, PIRIT_PARAM_0, mask));

    logDBG("Установка параметров чека закончена! Код ответа = %d", res);
    return res;
}
//=====================================================================
// Установить параметры печатного устройства
//=====================================================================
int BIOS::setPrinterMode(int mask)
{
    lock_guard<mutex> locker(mutex_bios);
    logDBG("Установка параметров принтера");

    int res = libWriteSettingsTable( PIRIT_TABLE_SETTINGS_KKT, PIRIT_PARAM_0, mask );

    logDBG("Установка параметров принтера закончена! Код ответа = %d", res);
    return res;
}
//=====================================================================
// Чтение параметров чека
//=====================================================================
int BIOS::getPurchaseMode(int &mask)
{
    lock_guard<mutex> locker(mutex_bios);
    logDBG("%s", __PRETTY_FUNCTION__);
    mask = 0;
    int res(getIntFromSettingsTable(mask, PIRIT_TABLE_SETTINGS_PURCHASE, PIRIT_PARAM_0));

    logDBG("%s\t\tПолучение параметров чека закончена! Код ответа = %d", __PRETTY_FUNCTION__, res);
    return res;
}

//=====================================================================
// Чтение параметров чека
//=====================================================================
bool  BIOS::printDiscount( const double &discountSumRubl, const string &text )
{
    logERR("%s:: Добавление в ПФ суммарной скидки по всем позициям = %f", __func__, discountSumRubl);

    if( text.empty() )
    {
        logERR("%s:: Ошибка! текст скидки не был указан",__func__);
        return false;
    }

    if( discountSumRubl < 0.01 )
    {
        logERR("%s:: Ошибка! Неадекватная сумма скидки = '%f'",__func__, discountSumRubl);
        return false;
    }
    const uint8_t SHIFT     =     16,
                  MAX_LEN   =     40;

    char cstr1[MAX_LEN]     = {0x00};

    sprintf (cstr1,"%-*s %*.2f", MAX_LEN -SHIFT , text.c_str(), SHIFT, discountSumRubl);

    int res = libPrintRequisite(DOC_REQ_PRINT_NORMAL, FONT_NORM, cstr1);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    logERR("%s:: Добавление в ПФ скидки %sвыполнено! Код ответа = %d",__func__, (ok ? "" : " НЕ "), res);
    return ok;
}
//=====================================================================
// Получить параметры печатного устройства
//=====================================================================
int BIOS::getPrinterMode(int &mask)
{
    lock_guard<mutex> locker(mutex_bios);
    logDBG("Получение параметров принтера");

    int res = getIntFromSettingsTable( mask, PIRIT_TABLE_SETTINGS_KKT, PIRIT_PARAM_0 );

    logDBG("Получение параметров принтера закончена! Код ответа = %d", res);
    return res;
}

//=====================================================================
// Получить опций лицензии
//=====================================================================
bool BIOS::getLicenseOptionsTags(int &mask)
{
    lock_guard<mutex> locker(mutex_bios);
    logDBG("Получение опций лицензий");

    int res = getIntFromSettingsTable( mask, PIRIT_TABLE_LIC_OPTIONS, PIRIT_PARAM_0 );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);

    if(!ok)
    {
        mask = 0;
        logERR("Ошибка при выгрузке опций лицензии! res = %d", res);
    }
    return ok;
}
//=====================================================================
// Печать X отчёта
//=====================================================================
bool BIOS::printXReport(string name)
{
    lock_guard<mutex> locker(mutex_bios);
    int res = libPrintXReport(name.c_str());
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    return ok;
}
//=====================================================================
// Распечатать документ из ФН. Если номер документа 0, то печатается
// список всех регистраций/перерегистраций
//=====================================================================
bool BIOS::printFnDocs(int numberOfDoc)
{
    lock_guard<mutex> locker(mutex_bios);
    int res = libPrintDocumentFromECT(numberOfDoc);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if(ok)
    {
        logDBG("Печать регистарций/перерегистраций ВЫПОЛНЕНО. Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());
    }
    else
    {
        logERR("Печать регистарций/перерегистраций НЕ ВЫПОЛНЕНО. Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());
    }

    return ok;
}
//=====================================================================
// Тех обнуление БИОС
//=====================================================================
bool BIOS::techZeroing  (string zavNum)
{
    lock_guard<mutex> locker(mutex_bios);
    // ЗН 0494123477
    if ( zavNum.size() != 14 )
    {
        logERR("techZeroing(): Ошибка! Неверная длина зав. номера = %d",  zavNum.size() );
        return false;
    }

    PLDate mpDate;
    PLTime mpTime;

    getCurMPTime(&mpDate, &mpTime);

    int res = libAuthorization(mpDate, mpTime, zavNum.c_str());
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logERR("techZeroing() %s Тех. обнуление BIOS. Зав. номер = '%s'. Результат = %d", (ok ? " " : " НЕ ВЫПОЛНЕНО!"), zavNum.c_str(), res);
    return ok;
}
//=====================================================================
// Установить дату и время в БИОС как на ККТ(текущую)
//=====================================================================
bool BIOS::setDateTimeToBIOS()
{
    lock_guard<mutex> locker(mutex_bios);
    PLDate mpDate;
    PLTime mpTime;

    getCurMPTime(&mpDate, &mpTime);

    int res = libSetPiritDateTime(mpDate, mpTime);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("setDateTimeToBIOS() Установка в БИОС новой Даты и Времени.Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());

    return ok;
}

//=====================================================================
// Установить дату и время в БИОС как на ККТ(произвольную)
//=====================================================================
bool BIOS::setDateTimeToBIOS(const time_t &unixTime )
{
    lock_guard<mutex> locker(mutex_bios);

    struct tm *ti = localtime(&unixTime);
    PLDate mpDate;

    mpDate.day   = ti->tm_mday;
    mpDate.month = ti->tm_mon + 1;
    mpDate.year  = ti->tm_year - 100;

    PLTime mpTime;

    mpTime.hours    = ti->tm_hour;
    mpTime.minutes  = ti->tm_min;
    mpTime.seconds  = ti->tm_sec;

    getCurMPTime(&mpDate, &mpTime);

    int res = libSetPiritDateTime(mpDate, mpTime);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("setDateTimeToBIOS() Установка в БИОС новой Даты и Времени.Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());

    return ok;
}
//=====================================================================
// Печать последнего отчёта о регистрации/перерегистрации
//=====================================================================
void BIOS::printLastFiscalReport()
{
    lock_guard<mutex> locker(mutex_bios);
    int res = libPrintCopyReportFiscalization();
    bool ok [[gnu::unused]] = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("printLastFiscalReport(): Печать последнего отчёта о регистрации/перерегистрации. Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());
}
//=====================================================================
// Печать техотчёта
//=====================================================================
void BIOS::printBIOSTechReport()
{
    lock_guard<mutex> locker(mutex_bios);
    int res = libPrintServiceData();
    bool ok [[gnu::unused]] = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
#ifdef BIOS_RB  // Для РБ дополнительно печатаем сервисные данные СКНО
    if(ok)
    {
        res = libPrintServiceDataSKNO_By ();
        ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    }
#endif
    logDBG("printBIOSTechReport(): Печать Тех. отчёта BIOS. Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());
}

void BIOS::cancelDocBIOS()
{
#ifdef BIOS_RB
    RB->kca_flgs_get();
    if (RB->isDocOpenedGet())
    {
        RB->cancelDoc();
    }
#else
    RF->fs_status_get();
    if (RF->isDocOpen_get())
    {
        RF->cancelDoc();
    }
#endif
}

bool BIOS::isDocCanceled()
{
#ifdef BIOS_RB
    // Выгрузка данных крайнего чека
    BEL::RECEIPT_FIS_DATA data;
    constexpr int canceled {0};
    const bool rc {RB->rec_fis_data_get(data) && (data.type == canceled)};
#else
    const bool rc {false};
#endif
    logWARN("%s BIOS doc is %scanceled", __FUNCTION__, rc ? "" : "not ");
    return rc;
}

bool BIOS::cancelDoc()
{
    bool rc {false};
    std::lock_guard<std::mutex> locker(mutex_bios);
#ifdef BIOS_RF
    rc = RF->cancelDoc();
#elif BIOS_RB
    rc = RB->cancelDoc();
#endif
    return rc;
}
//=====================================================================
// регистрационный номер ФН
//=====================================================================
bool BIOS::RUS::fs_number_ask()
{
    #ifdef BIOS_RB
        return true;
    #endif
    lock_guard<mutex> locker(*mutex_rf);

    char fnNum[100];
    int res = libGetFN_Number(fnNum);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("getFnNumber(): Результат операции = %d", res);
    if( !ok )
    {
        logDBG( "GET RF NYMBER ERROR!!!" );
        return false;
    }

    fs_number = Convert::charToString( fnNum, FS_NUMBER_LEN );
    logDBG("getFnNumber(): Номер ФН = %s", fs_number.c_str());

    return true;
}
//=====================================================================
// Вернуть номер последнего фискального документа
//=====================================================================
bool BIOS::RUS::last_fd_num_ask()
{
    #ifdef BIOS_RB
        return true;
    #endif
    lock_guard<mutex> locker(*mutex_rf);

    char fiscalDocNumber[100];
    int res = libLastFiscDoc_Number(fiscalDocNumber);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("getLastFiscDocNumber(): Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());
    if( !ok )
    {
        logERR( "GET LAST FISC DOC NUMBER ERROR!!!" );
        return false;
    }
    last_fd_num = stoul( fiscalDocNumber, NULL, 10);
    logDBG("getLastFiscDocNumber(): Номер  Последнего фискального документа = '%u'", last_fd_num);

    return true;
}
//=====================================================================
// Получить Номер смены, открыта ли и № чека в смене
//=====================================================================
bool BIOS::RUS::getFSshiftParam()
{
    #ifdef BIOS_RB
        return true;
    #endif
    lock_guard<mutex> locker(*mutex_rf);

    int          isOpened;

    int res = libGetCurShiftState( &shift_num, &isOpened, &reciept_num );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("getShiftStatus(): Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());
    if( !ok )
    {
        logERR( "GET SHIFT STATUS ERROR!!!" );
        return false;
    }

    logDBG("getShiftStatus(): Номер тек смены    = %d", shift_num);
    logDBG("getShiftStatus(): Смена открыта      = %d", isOpened);
    logDBG("getShiftStatus(): Номер чека в смене = %d", reciept_num);

    isShiftOpen = (bool)isOpened;

    return true;
}
//=====================================================================
// Получить все флаги статусов из БИОСа
//=====================================================================
bool BIOS::getStatusBIOS()
{
    #ifdef BIOS_RB
        return true;
    #endif
    lock_guard<mutex> locker(mutex_bios);

    int     fatalStatus        = 0,
            currentFlagsStatus = 0,
            documentStatus     = 0;

    int res = getStatusFlags(&fatalStatus, &currentFlagsStatus, &documentStatus);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("getStatusBIOS():  fatalStatus = %d \n\tcurrentFlagsStatus = %d \n\tdocumentStatus = %d",
                                                                                   fatalStatus,
                                                                                   currentFlagsStatus,
                                                                                   documentStatus);
    logDBG("getStatusBIOS(): Получение флагов BIOS. Код ответа = %d. Сообщение ='%s'", res, strFromErrCode(res).c_str());


    statusBios.fatal_wrongSummNVR  = fatalStatus &  BIOS_STATUS_FATAL_WRONG_SUM_NVR;
    statusBios.fatal_wrongSummKonf = fatalStatus &  BIOS_STATUS_FATAL_WRONG_SUM_KONF;
    statusBios.fatal_FnNotAuth     = fatalStatus &  BIOS_STATUS_FATAL_FS_NOT_ATH;
    statusBios.fatal_FnFatalErr    = fatalStatus &  BIOS_STATUS_FATAL_FS_ERR;
    statusBios.fatal_errSD         = fatalStatus &  BIOS_STATUS_FATAL_SD_ERR;

    statusBios.kkt_notInited       = currentFlagsStatus &  BIOS_STATUS_KKT_NOT_INITED;
    statusBios.kkt_notFiscal       = currentFlagsStatus &  BIOS_STATUS_KKT_NOT_FISCAL;
    statusBios.kkt_isShiftOpened   = currentFlagsStatus &  BIOS_STATUS_KKT_IS_SHIFT_OPEN;
    statusBios.kkt_isShift24h      = currentFlagsStatus &  BIOS_STATUS_KKT_IS_SHIFT_24H;
    statusBios.kkt_fsClose         = currentFlagsStatus &  BIOS_STATUS_KKT_FS_CLOSE;
    statusBios.kkt_notRegistered   = currentFlagsStatus &  BIOS_STATUS_KKT_NOT_REGISTERED;
    statusBios.kkt_colseShiftNeed  = currentFlagsStatus &  BIOS_STATUS_KKT_CLOSE_SHIFT_NEED;
    statusBios.kkt_controlLentaErr = currentFlagsStatus &  BIOS_STATUS_KKT_CL_ERR;

    statusBios.documentType = documentStatus % 16;
    statusBios.documentState = documentStatus - statusBios.documentType;
    logDBG( "FATAL_WRONG_SUM_NVR \t%01x\n", statusBios.fatal_wrongSummNVR );
    logDBG( "FATAL_WRONG_SUM_KONF \t%01x\n", statusBios.fatal_wrongSummKonf );
    logDBG( "FATAL_FS_NOT_ATH \t%01x\n", statusBios.fatal_FnNotAuth );
    logDBG( "FATAL_FS_ERR \t%01x\n", statusBios.fatal_FnFatalErr );
    logDBG( "FATAL_SD_ERR \t%01x\n", statusBios.fatal_errSD );
    logDBG( "KKT_NOT_INITED \t%01x\n", statusBios.kkt_notInited );
    logDBG( "KKT_NOT_FISCAL \t%01x\n", statusBios.kkt_notFiscal  );
    logDBG( "KKT_IS_SHIFT_OPEN \t%01x\n", statusBios.kkt_isShiftOpened );
    logDBG( "KKT_IS_SHIFT_24H \t%01x\n", statusBios.kkt_isShift24h );
    logDBG( "KKT_FS_CLOSE \t%01x\n", statusBios.kkt_fsClose );
    logDBG( "KKT_NOT_REGISTERED \t%01x\n", statusBios.kkt_notRegistered );
    logDBG( "KKT_CLOSE_SHIFT_NEED \t%01x\n", statusBios.kkt_colseShiftNeed );
    logDBG( "KKT_CL_ERR \t%01x\n", statusBios.kkt_controlLentaErr );

    logDBG( "\ndocumentType \t%01x\n", statusBios.documentType );
    logDBG( "documentState \t%01x\n", statusBios.documentState );
    RF->is_connect_set     ( !statusBios.fatal_FnFatalErr );
    RF->is_shift_24h_set(statusBios.kkt_isShift24h);
    RF->isDocOpen_set( (statusBios.documentState==0) ? false : true );
    return ok;
}
//=====================================================================
// Получение расшифровки кода ошибки либы.
//=====================================================================
string BIOS::strFromErrCode (int errorCode)
{
    string mess;
    char chMess[200];
    memset(chMess, 0x00, 200);

    libFormatMessage(errorCode, chMess);
    mess = chMess;
    return mess;
}


//=====================================================================
// установить адрес и порт ОФД
bool BIOS::RUS::setOFDSettings             ( string urlOFD, uint32_t portOFD  )
{
#ifdef BIOS_RB
    return false;
#endif
    lock_guard<mutex> locker(*mutex_rf);

    if(portOFD < 1 || portOFD > 65534)
    {
        logERR( "setOFDSettings()::Ошибка!  Некорректный порт ОФД =  %d", portOFD );
        return false;
    }
    if( urlOFD.empty() || urlOFD.length() > 64 )
    {
        logERR( "setOFDSettings()::Ошибка! Некорректный адрес ОФД = '%s'", urlOFD.c_str() );
        return false;
    }
    int res = libWriteSettingsTable(PIRIT_TABLE_OFD_IP, 0, urlOFD.c_str() );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        return false;
    }
    string port = to_string(portOFD);

    res = libWriteSettingsTable(PIRIT_TABLE_OFD_PORT, 0, port.c_str());
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    return ok;
}
//=====================================================================
// Тип ФН
//=====================================================================
bool BIOS::RUS::fsTypeGet( int &fsType )
{
    #ifdef BIOS_RB
        return false;
    #endif
    lock_guard<mutex> locker(*mutex_rf);
    int res = getIntFromInfoFN( PIRIT_INFO_FN_VERSION, PIRIT_VERSION_FN, fsType);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR("%s\t\tНЕ ВЫПОЛНЕНО!",  __PRETTY_FUNCTION__ );
        return false;
    }
    logERR( "%s\t\t\tВЫПОЛНЕНО!  Тип ФН = '%d'", __PRETTY_FUNCTION__, fsType );
    return true;
}
//=====================================================================
// Версия ККТ
//=====================================================================
bool BIOS::RUS::getFNVer ( string &fnVer )
{
    #ifdef BIOS_RB
        return false;
    #endif
    lock_guard<mutex> locker(*mutex_rf);

    int res = getStrFromInfoFN( PIRIT_INFO_FN_VERSION, PIRIT_VERSION_FIRMWARE_FN, fnVer);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR("%s\t\tНЕ ВЫПОЛНЕНО!",  __PRETTY_FUNCTION__ );
        return false;
    }
    logDBG( "%s\t\t\tВЫПОЛНЕНО!  Версия прошивки FN = '%s'", __PRETTY_FUNCTION__, fnVer.c_str() );
    return true;
}


//=====================================================================
// Процедура закрытия ФН
//=====================================================================
bool BIOS::RUS::closeFN (string cashierName, string& dateTime, uint32_t& fd, uint32_t& fpd)
{
    #ifdef BIOS_RB
        return false;
    #endif

    lock_guard<mutex> locker(*mutex_rf);

    if (isDocOpen_get())
    {
        // Закрытие ранее открытого документа при его наличии
        cancelDoc();
    }

    int res = libCloseFN( cashierName.c_str(), fd, fpd, dateTime);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR( "closeFN():: \tERROR, Не удалось закрыть ФН!"  );
        return false;
    }
    logDBG( "closeFN():: \t ******** ФН УСПЕШНО ЗАКРЫТ! ********"  );
    return true;
}
//=====================================================================
// Процедура аварийного закрытия ФН
//=====================================================================
bool BIOS::RUS::closeEmegencyFN()
{
    #ifdef BIOS_RB
        return false;
    #endif

    lock_guard<mutex> locker(*mutex_rf);

    int res = libEmegencyCloseFN();
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR( "closeFN():: \tERROR, АВАРИЙНО ЗАКРЫТЬ ФН НЕ УДАЛОСЬ!!!"  );
        return false;
    }
    logDBG( "closeFN():: \t ******** ФН ЗАКРЫТ АВАРИЙНО! ********"  );
    return true;
}
//=====================================================================
// Аварийное закрытие смены
//=====================================================================
bool BIOS::RUS::closeEmegencyShift()
{
    #ifdef BIOS_RB
        return false;
    #endif

    lock_guard<mutex> locker(*mutex_rf);

    int res = libEmergencyCloseShift                           ();

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR( "closeFN():: \tERROR, АВАРИЙНО ЗАКРЫТЬ СМЕНУ НЕ УДАЛОСЬ!!!"  );
        return false;
    }
    logDBG( "closeFN():: \t ******** СМЕНА ЗАКРЫТА АВАРИЙНО! ********"  );

    return true;
}

//=====================================================================
//=====================================================================
//==================    Счётчики    ===================================
//=====================================================================
//=====================================================================

//=====================================================================
// Счётчики. 0x01 Номер тек смены
//=====================================================================
bool BIOS::getCounters_CurShiftNum     ( uint64_t               &shiftNum )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_CurShiftNum(shiftNum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_CurShiftNum():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x02 Вернуть номер следующего чека
//=====================================================================
bool BIOS::getCounters_NextRecNum     ( uint64_t               &nextRecNum )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_NextRecNum(nextRecNum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_NextRecNum():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x03 Вернуть суммы продаж по типам платежа
//=====================================================================
bool BIOS::getCounters_SaleSumByPay     (OPERATION<double>          &ssbp )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_SaleSumByPay(ssbp);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_SaleSumByPay():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x04 Вернуть суммы продаж по типам платежа
//=====================================================================
bool BIOS::getCounters_SaleCntByPay     (OPERATION<uint64_t>          &scbp )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_SaleCntByPay(scbp);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_SaleCntByPay():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x05 Вернуть суммы возвратов по типам платежа
//=====================================================================
bool BIOS::getCounters_RetSaleSumByPay     (OPERATION<double>         &rssbp )
{

    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_RetSaleSumByPay(rssbp);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_SaleCntByPay():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x06 Вернуть количество оплат по возвратам
//=====================================================================
bool BIOS::getCounters_RetSaleCntByPay ( OPERATION<uint64_t>       &rscbp )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_RetSaleCntByPay(rscbp);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_RetSaleCntByPay():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x07 Количество оформленных чеков
//=====================================================================
bool BIOS::getCounters_RecCntsByType ( REC_CNTS                &recCnts )
{

    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_RecCntsByType(recCnts);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_RecCntsByType():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x08 Суммы по оформленным чекам
//=====================================================================
bool BIOS::getCounters_RecSumsByType ( REC_SUMS                &recSums )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_RecSumsByType(recSums);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_RecSumsByType():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x09 Суммы по скидкам
//=====================================================================
bool BIOS::getCounters_DiscountSums ( DISCOUNT_SUMS &discSums )
{

    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_DiscountSums(discSums);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_DiscountSums():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x10 Суммы налогов  по продажам
//=====================================================================
bool BIOS::getCounters_TaxSumsSale ( TAX_SUMS &taxSums )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_TaxSumsSale(taxSums);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_TaxSumsSale():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x11 Суммы налогов  по возвратам
//=====================================================================
bool BIOS::getCounters_TaxSumsRets ( TAX_SUMS  &taxSums )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_TaxSumsRets(taxSums);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_TaxSumsRets():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x12 Данные по последнему X отчету или отчету о закрытии смены
//=====================================================================
bool BIOS::getCounters_XZ ( X_Z_DATA                 &xzData )
{

    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_XZ(xzData);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_XZ():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x13 Вернуть суммы по секциям/отделам по продажам
//=====================================================================
bool BIOS::getCounters_depSaleSum  ( double         (&depSaleSum)[NUM_OF_SECTOINS] )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_depSaleSum (depSaleSum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_depSaleSum():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x14 Вернуть суммы по секциям/отделам по возвратам
//=====================================================================
bool BIOS::getCounters_depRetSum  ( double         (&depRetSum)[NUM_OF_SECTOINS] )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_depRetSum (depRetSum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_depRetSum():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x15  Кол-во чеков расходов + Кол-во чеков возвратов расхода
//=====================================================================
bool BIOS::getCounters_RecCount  ( uint64_t &purchaseCnt, uint64_t &retPurchaseCnt )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_RecCount ( purchaseCnt, retPurchaseCnt );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_RecCount():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x16 Вернуть суммы покупок по типам платежа
//=====================================================================
bool BIOS::getCounters_SaleSumByType( OPERATION<double> &purchSums )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_SaleSumByType( purchSums );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_SaleSumByType():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x17 Вернуть суммы покупок по типам платежа
//=====================================================================
bool BIOS::getCounters_RetSumByType( OPERATION<double>  &purchSumsRet )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_RetSumByType( purchSumsRet );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_RetSumByType():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// Счётчики. 0x18 Вернуть данные по коррекциям
//=====================================================================
bool BIOS::getCounters_CorData ( COR_DATA &corData )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_CorData( corData );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_CorData():: \tERROR, Ошибка выгрузки счётчиков!"  );
        return false;
    }
    return true;
}

//=====================================================================
// получить адрес и порт ОФД
//=====================================================================
bool BIOS::RUS::getOFDSettings             ( string &urlOFD, int &portOFD  )
{
    #ifdef BIOS_RB
        return false;
    #endif
    lock_guard<mutex> locker(*mutex_rf);
    //--------
    int res = getStrFromSettingsTable(urlOFD, PIRIT_TABLE_OFD_IP, 0, 0);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        return false;
    }
    logDBG( "getOFDSettings():: \turl OFD = '%s'", urlOFD.c_str() );
    //--------
    res = getIntFromSettingsTable(portOFD, PIRIT_TABLE_OFD_PORT, 0, 0);
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG( "getOFDSettings():: \tport OFD = %d", portOFD );
    //--------
    return ok;
}


//=====================================================================
// Управление расчетами (битовая маска) ККТ.  ПРОЧИТАТЬ
bool BIOS::RUS::getCalcMask ( int &mask )
{
#ifdef BIOS_RB
    return false;
#endif
    lock_guard<mutex> locker(*mutex_rf);
    //--------
    int res = getIntFromSettingsTable(mask, PIRIT_TABLE_CALC_MASK, 0, 0);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_CONNECTION);
    if( !ok )
    {
        return false;
    }
    logDBG( "getCalcMask():: \told mask = %d", mask );
    //--------
    return ok;
}


//=====================================================================
// Управление расчетами (битовая маска) ККТ.  ЗАДАТЬ
bool BIOS::RUS::setCalcMask ( int mask )
{
#ifdef BIOS_RB
    return false;
#endif
    if( mask < 0 || mask > 255 )
    {
        logERR( "setCalcMask():: ERROR! Кривая маска(0..255) = '%d'", mask );
        return false;
    }
    string strMask = to_string(mask);
    lock_guard<mutex> locker(*mutex_rf);
    //--------
    int res = libWriteSettingsTable(PIRIT_TABLE_CALC_MASK, 0, strMask.c_str());
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_CONNECTION);
    if( !ok )
    {
        return false;
    }
    logDBG( "setCalcMask():: \tnew mask = '%s'", strMask.c_str() );
    //--------
    return ok;
}


//=====================================================================
// Обертка записи данных в таблицу настроек
bool BIOS::RUS::writeSettingsTableWrapper(unsigned char number, int index, const char *data)
{
    lock_guard<mutex> locker(*mutex_rf);
    int res = libWriteSettingsTable(number, index, data);
    return ( res == 0 ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_CONNECTION) );
}


//=====================================================================
// Задать наименование организации и адрес для режима енвд(печать на чеках)
bool BIOS::RUS::setENVDRegData (const string &orgName, const string &orgAdress )
{
#ifdef BIOS_RB
    return false;
#endif
    int INDEX_NAME = 0;
    int INDEX_ADRR = 2;

    if(orgName.empty() || orgAdress.empty() )
    {
        logERR( "%s:: ERROR! Не данных намен орг = '%s', адрес орг = '%s'",
                                                      __func__,
                                                      orgName.c_str(),
                                                      orgAdress.c_str() );
        return false;
    }
    //--------
    // Наименование организации, 1-ая строка
    if( !writeSettingsTableWrapper(PIRIT_TABLE_NAME_ADDRESS_ORG, INDEX_NAME, orgName.c_str()) )
    {
        logERR( "%s:: ERROR! Не удалось изменить данные Наименование организации! ", __func__ );
        return false;
    }
    //--------
    //  Адрес организации, 1-ая строка
    if( !writeSettingsTableWrapper(PIRIT_TABLE_NAME_ADDRESS_ORG, INDEX_ADRR, orgAdress.c_str()) )
    {
        logERR( "%s:: ERROR! Не удалось изменить данные Адрес организации! ", __func__ );
        return false;
    }
    //--------
    logDBG( "%s():: \tНаименование и адрес организации успешно изменены!", __func__ );
    return true;
}

//=====================================================================
// Первичная регистрация ККТ
//=====================================================================
bool BIOS::RUS::registerKKT_BIOS(registrationStruct &rs)
{
    lock_guard<mutex> locker(*mutex_rf);
    //---------
    // Проверки
    //---------
    if( rs.type != ACTIVATION_NEW && rs.type != ACTIVATION_REPLACE  )
    {
        logERR("registerKKT_BIOS():: Ошибка! Неверное значение поля type = 0x%02x, требуется 0x00 или 0x01", rs.type);
        return false;
    }
    if( rs.regNumber.length() != FS_NUMBER_LEN )
    {
        logERR("registerKKT_BIOS():: Ошибка! Неверная длина поля regNumber = %d, Требуется = 16", rs.regNumber.length());
        return false;
    }
    if( rs.INN.empty() )
    {
        logERR("registerKKT_BIOS():: Ошибка! Поле INN Не заполнено!");
        return false;
    }
    if( rs.systemTax < 0 || rs.systemTax > 63 )
    {
        logERR("registerKKT_BIOS():: Ошибка! Неверная маска СНО! systemTax = '%d'", rs.systemTax);
        return false;
    }
    if( rs.rej < 0 || rs.rej > 65535 )
    {
        logERR("registerKKT_BIOS():: Ошибка! Неверная маска режимов! rej = '%d'", rs.rej);
        return false;
    }
    if( rs.cashierName.empty()  || rs.cashierName.length() > CASHIER_NAME_MAX_SIZE )
    {
        logERR("registerKKT_BIOS():: Ошибка! Поле cashierName Не заполнено или неверная длина строки:'%s'", rs.cashierName.c_str());
        return false;
    }
    if( rs.userName1.empty()  || rs.userName1.length() > USER_NAME_MAX_SIZE )
    {
        logERR("registerKKT_BIOS():: Ошибка! Поле userName1 Не заполнено или неверная длина строки:'%s'", rs.userName1.c_str());
        return false;
    }
    if( rs.addressSettle1.empty()  || rs.addressSettle1.length() > ADDR_MAX_SIZE )
    {
        logERR("registerKKT_BIOS():: Ошибка! Поле addressSettle1 Не заполнено или неверная длина строки:'%s'", rs.addressSettle1.c_str());
        return false;
    }
    if( rs.placeSettle.empty()  || rs.placeSettle.length() > PLACE_MAX_SIZE )
    {
        logERR("registerKKT_BIOS():: Ошибка! Поле placeSettle Не заполнено или неверная длина строки:'%s'", rs.placeSettle.c_str());
        return false;
    }
    if( rs.OFDName.size() > 64 )
    {
        logERR("registerKKT_BIOS():: Ошибка! Поле OFDName Не заполнено или неверная длина строки:'%s'", rs.OFDName.c_str());
        return false;
    }
    if( rs.OFD_INN.empty()  || rs.OFD_INN.length() > OFD_INN_MAX_SIZE )
    {
        logERR("registerKKT_BIOS():: Ошибка! Поле OFD_INN Не заполнено или неверная длина строки:'%s'", rs.OFD_INN.c_str());
        return false;
    }
    if( rs.senderEMAIL.empty() || rs.senderEMAIL.length() > SENDER_EMAIL_MAX_SIZE)
    {
        logERR("registerKKT_BIOS():: Ошибка! Поле senderEMAIL Не заполнено или неверная длина строки:'%s'", rs.senderEMAIL.c_str());
        return false;
    }
    if( rs.FNS_Site.empty() || rs.FNS_Site.length() > FNS_SITE_MAX_SIZE)
    {
        logERR("registerKKT_BIOS():: Ошибка! Поле FNS_Site Не заполнено или неверная длина строки:'%s'", rs.FNS_Site.c_str());
        return false;
    }

    // Формирование даты и времени регистрации

    time_t t;
    struct tm *t_m;
    time(&t);
    t_m = localtime(&t);
    logDBG("registerKKT_BIOS()        Date: %02d.%02d.%4d",  t_m->tm_mday, t_m->tm_mon + 1, t_m->tm_year + 1900);
    logDBG("registerKKT_BIOS()        Time: %02d:%02d:%02d", t_m->tm_hour, t_m->tm_min,     t_m->tm_sec);

    PLDateTime dt;
    dt.year    = t_m->tm_year + 1900 - 2000;
    dt.month   =            t_m->tm_mon + 1;
    dt.day     =               t_m->tm_mday;
    dt.hours   =               t_m->tm_hour;
    dt.minutes =                t_m->tm_min;
    dt.seconds =                t_m->tm_sec;

    if (isDocOpen_get())
    {
        // Закрытие ранее открытого документа при его наличии
        cancelDoc();
    }

    // Если всё ок - регистрируемся
    int resultReg =    libRegistrationEx(
                                          rs.type,                     // unsigned char type,
                                          rs.regNumber.c_str(),        // const char *regNumber,
                                          rs.INN.c_str(),              // const char *INN,
                                          rs.systemTax,                // int systemTax,
                                          rs.rej,                      // int rej,
                                          rs.cashierName.c_str(),      // const char *cashierName,
                                          dt,                          // PLDateTime regDateTime,
                                          rs.userName1.c_str(),        // const char *userName1,
                                          rs.userName2.c_str(),        // const char *userName2,   0494123477
                                          rs.addressSettle1.c_str(),   // const char *addressSettle1,
                                          rs.addressSettle2.c_str(),   // const char *addressSettle2,
                                          rs.placeSettle.c_str(),      // const char *placeSettle,
                                          rs.autoNumber.c_str(),       // const char *autoNumber,
                                          rs.OFDName.c_str(),          // const char *OFDName,
                                          rs.OFD_INN.c_str(),          // const char *OFDINN,
                                          rs.senderEMAIL.c_str(),      // const char *senderEmail,
                                          rs.FNS_Site.c_str(),         // const char *FNSEmail,
                                          rs.isAutomatic);             // bool automatic);

    bool ok = (resultReg == 0) ?  true :  isCommandComplete(resultReg, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logWARN("=======================================================");
    logWARN("=======================================================");
    logWARN("============ Результат регистрации = %d. Сообщение ='%s'", resultReg, strFromErrCode(resultReg).c_str());
    logWARN("=======================================================");
    logWARN("=======================================================");

    if( !ok )
    {
        logERR( "REGISTER KKT BIOS ERROR!!!" );
        return false;
    }

    return true;
}
//=====================================================================
//=====================================================================
//=====================================================================
BIOS::BEL::BEL()
{
    convert.reset(new Convert());

    NDSS.       clear();
    DEPS.       clear();
    recPayTypes.clear();
}
//=====================================================================
BIOS::BEL::~BEL()
{

}

//=====================================================================
//                      Запрос флагов КСА
//=====================================================================
bool BIOS::BEL::kca_flgs_get()
{
    lock_guard<mutex> locker(*mutex_rb);
    int statusFatal, statusCurrent, statusDoc, statusSKNO;
    int docOpenedType, docOpenedState;

    int res = libGetStatusFlags_By( &statusFatal, &statusCurrent, &statusDoc, &statusSKNO );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if( !ok )
    {
        logERR( "kca_flgs_get():: Ошибка! результат команды libGetStatusFlags_By()  = %d ", res );
        return false;
    }

    logDBG("kca_flgs_get():: \n\t\tstatusFatal = %d \tstatusCurrent = %d \tstatusDoc = %d ", statusFatal, statusCurrent, statusDoc);

    is_start_failed        = statusCurrent & KCA_CUR_FLGS::START_FAILED;
    is_study_mode          = statusCurrent & KCA_CUR_FLGS::STUDY_MODE;
    is_shift_open          = statusCurrent & KCA_CUR_FLGS::SHIFT_OPEN;
    is_shift_24h           = statusCurrent & KCA_CUR_FLGS::SHIFT_24H;
    is_buffer_is_not_empty = statusCurrent & KCA_CUR_FLGS::BUFFER_IS_NOT_EMPTY;
    is_close_shift_mem_err = statusCurrent & KCA_CUR_FLGS::CLOSE_SHIFT_MEM_ERR;
    is_close_shift_err     = statusCurrent & KCA_CUR_FLGS::CLOSE_SHIFT_ERR;

    is_wrong_NVR_sum  = statusFatal & KCA_FATAL_FLGS::WRONG_NVR_SUM;
    is_wrong_conf_sum = statusFatal & KCA_FATAL_FLGS::WRONG_CONF_SUM;
    is_BEP_err        = statusFatal & KCA_FATAL_FLGS::BEP_IFACE_ERROR;
    is_wrong_BEP_sum  = statusFatal & KCA_FATAL_FLGS::WRONG_BEP_SUM;
    is_BEP_write_err  = statusFatal & KCA_FATAL_FLGS::BEP_WRITE_ERR;
    is_BEP_not_auth   = statusFatal & KCA_FATAL_FLGS::BEP_NOT_AUTH;
    is_KL_mem_err     = statusFatal & KCA_FATAL_FLGS::KL_MEM_ERR;

    docOpenedType =       statusDoc &   0x0F;          // перые биты 0,1,2,3
    docOpenedState =      statusDoc >> 4;       // последующие биты 4,5,6,7

    if(docOpenedState == 0)
    {
        isDocOpened = false;
    }
    else
    {
        isDocOpened = true;
    }
    //---------------------------- Тип текущего открытого документа
    if( docOpenedType > KCA_DOC_TYPE::KCA_TYPE_RECIEPT_INVOCIE )
    {
        logERR("Ошибка! Некорректный docOpenedType = %d", docOpenedType);
        return false;
    }
    doc_type  =  (KCA_DOC_TYPE)   docOpenedType;

    //---------------------------- Состояние документа
    if( (docOpenedState < 0) || (docOpenedState > KCA_DOC_STATE::KCA_STATE_NEED_CLOSE) )
    {
        logERR("Ошибка! Некорректный docOpenedState = %d", docOpenedState);
        return false;
    }
    doc_state =  (KCA_DOC_STATE) docOpenedState;

    //---------------------------- Статус флагов работы с СКНО
    if( (statusSKNO < 0) || (statusSKNO > KCA_SCNO_STATE::KCA_SCNO_Z_REP_ERR) )
    {
        logERR("Ошибка! Некорректный statusSKNO = %d", statusSKNO);
        return false;
    }
    scno_state = (KCA_SCNO_STATE)   statusSKNO;




    logDBG("kca_flgs_get():: \n\t\tТЕКУЩИЕ:"
                            "\n\tis_start_failed           = %d "
                            "\n\tis_study_mode             = %d "
                            "\n\tis_shift_open             = %d "
                            "\n\tis_shift_24h              = %d "
                            "\n\tis_buffer_is_not_empty    = %d "
                            "\n\tis_close_shift_mem_err    = %d "
                            "\n\tis_close_shift_err        = %d ",
                                                 is_start_failed,
                                                 is_study_mode,
                                                 is_shift_open,
                                                 is_shift_24h,
                                                 is_buffer_is_not_empty,
                                                 is_close_shift_mem_err,
                                                 is_close_shift_err );

    logDBG("\t\tФАТАЛЬНЫЕ:"
                           "\n\tis_wrong_NVR_sum   = %d "
                           "\n\tis_wrong_conf_sum  = %d "
                           "\n\tis_BEP_err         = %d "
                           "\n\tis_wrong_BEP_sum   = %d "
                           "\n\tis_BEP_write_err   = %d "
                           "\n\tis_BEP_not_auth    = %d "
                           "\n\tis_KL_mem_err      = %d ",
                                                   is_wrong_NVR_sum,
                                                   is_wrong_conf_sum,
                                                   is_BEP_err,
                                                   is_wrong_BEP_sum,
                                                   is_BEP_write_err,
                                                   is_BEP_not_auth,
                                                   is_KL_mem_err );
    logDBG("\t\tДОКУМЕНТ:"
                   "\n\tdoc_type   = %d "
                   "\n\tdoc_state  = %d ",
                                                   doc_type,
                                                   doc_state );

    logDBG("\t\tСКНО: \n\tscno_state   = %d " , scno_state );

    return true;
}


//=====================================================================
// Открытие смены в БИОС
//=====================================================================
bool BIOS::BEL::openShift()
{
    lock_guard<mutex> locker(*mutex_rb);
    logDBG("Открытие смены.... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("BIOS::openShift()  Ошибка! Соединение не проинициализированно!");
        return false;
    }
    int res = libOpenShift_By();
    logDBG("Смена %sоткрыта! Код ответа = %d",(res == 0 ? " " : "не "), res);
    return res == 0 ? true : false;
}
//=====================================================================
bool BIOS::is_email_valid(const string& email)
{
    // {C}@{C}.{C}
    if (email.empty())
        return false;

    size_t at = email.find('@');
    if ( (at == string::npos) || (at == 0) || (at == email.size()-1))
    {
        return false;
    }

    size_t dot = email.find('.', at+1);
    if ( (dot == string::npos) || (dot == at+1) || (dot == email.size()-1))
    {
        return false;
    }

    return true;
}
//=====================================================================
bool BIOS::leap_valid( int day, int month, int year )
{
    if (((month == 2) && (year % 4 == 0) && (day > 29))
                || ((month == 2) && (year % 4 != 0) && (day > 28)))
    {
        return false;
    }
    return true;
}
//=====================================================================
bool BIOS::day_month_valid( int day, int month )
{
    if ((month == 4) || (month == 6) || (month == 9) || (month == 11))
    {
        if (day > 30)
        {
            return false;
        }
    }
    else
    {
        if (day > 31)
        {
            return false;
        }
    }
    return true;
}
//=====================================================================
// Считать НДС из BIOS
//=====================================================================
bool BIOS::BEL::nds_read( int i, NDS &nds )
{
    logDBG( "NDS %d READ ...", i );

    if(i < 0 || i > 5)
    {
        logERR( "WRONG NDS INDEX!!!" );
        return false;
    }

    lock_guard<mutex> locker(*mutex_rb);

    char name[40];
    char  val[20];
    int  res1 = 0;
    int  res2 = 0;

    // Обнуляем массивы
    memset(name, 0 ,40 );
    memset(val,  0 ,20 );

    res1 = libGetSettingsKSA_By(40, i, name);   // Читаем имена НДСов
    res2 = libGetSettingsKSA_By(41, i,  val);   // Читаем сами ставки

    // Аккумулируем ошибку
    res1 |= res2;

    if( res1 !=0 )
    {
        logERR("ndss_read():: Ошибка чтения НДСов! :-(");
        return false;
    }

    nds.name  = string(name);
    nds.value = QString(val).toFloat();

    logDBG("\ti = %d, \tname = '%s', \tval = %lf", i, nds.name.c_str(), nds.value);
    logDBG("ndss_read():: Чтение НДС прошло успешно");

    return true;
}
//=====================================================================
int BIOS::BEL::getNDSTableSize()
{
    const uint8_t SPACE = 0x20;
    return count_if(begin(NDSS), end(NDSS), [&](const NDS &nds){return (!(Convert::end_symbols_delete(nds.name, SPACE).empty()));});
}
//=====================================================================
// Считать НДСы из BIOS
//=====================================================================
bool BIOS::BEL::ndss_read()
{
    logDBG( "NDSS READ ..." );

    NDS nds;

    const u_int8_t NDS_TABLE_SIZE = 6;

    for (int nds_num = 0; nds_num < NDS_TABLE_SIZE; nds_num++)
    {
        if(nds_read( nds_num, nds ))
        {
            NDSS.push_back( nds );
        }
        else
        {
            logERR( "NDSS READ ERROR!!!" );
            NDSS.clear();
            return false;
        }
    }
    return true;
}
//=====================================================================
// Записать НДС в BIOS 0...5
//=====================================================================
bool BIOS::BEL::nds_write(int i, NDS nds)
{
    lock_guard<mutex> locker(*mutex_rb);

    int maxSize = NDSS.size() - 1;
    if(i < 0 || i >  maxSize)
    {
        logERR("ndss_write:: Ошибка! неверный индекс НДСа! i = %d", i);
        return false;
    }

    int res1 = libSetSettingsKSA_By( 40, i,  (char*)nds.name.c_str() );
    int res2 = libSetSettingsKSA_By( 41, i,  (char*)to_string(nds.value).c_str() );

    if( (res1 | res2) != 0 )
    {
        logERR("ndss_write():: Ошибка записи НДС. i = %d, name = %s, val = %s",
                                                                               i,
                                                         nds.name.c_str(),
                                             to_string(nds.value).c_str());
        return false;
    }
    logDBG("ndss_write():: запись НДСа прошла успешно");

    return true;
}
//=====================================================================
// Записать НДС в BIOS 0...5
//=====================================================================
bool BIOS::BEL::ndss_write(int i)
{
    lock_guard<mutex> locker(*mutex_rb);

    int maxSize = NDSS.size()-1;
    if(i < 0 || i >  maxSize)
    {
        logERR("ndss_write:: Ошибка! неверный индекс НДСа! i = %d", i);
        return false;
    }

    int res1 = libSetSettingsKSA_By( 40, i,  (char*)NDSS.at(i).name.c_str() );
    int res2 = libSetSettingsKSA_By( 41, i,  (char*)to_string(NDSS.at(i).value).c_str() );

    if( (res1 | res2) != 0 )
    {
        logERR("ndss_write():: Ошибка записи НДС. i = %d, name = %s, val = %s",
                                                                               i,
                                                         NDSS.at(i).name.c_str(),
                                             to_string(NDSS.at(i).value).c_str());
        return false;
    }
    logDBG("ndss_write():: запись НДСа прошла успешно");
    return true;
}

//=====================================================================
//                  Запись типа платежа в BIOS
bool BIOS::BEL::writePayType( const int num, const  string &name )
{
    const int GET_PAY_TYPE = 32;
    lock_guard<mutex> locker(*mutex_rb);

    // Налик нельзя переписать
    if(num <  0                                             ||
       num >  (BIOS::MAX_PAY_TYPES - 1)                     ||
       num ==  PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH      ||
       num ==  PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASHLESS   )
    {
        logERR("writePayType():: Ошибка! неверный индекс Типа платежа! num = %d", num);
        return false;
    }

    if( name.length() > MAX_PAY_TYPE_NAME_LEN )
    {
        logERR("writePayType():: Ошибка! Длина Имени Типа платежа > %d ! \n\t name = '%s' \n\t len = '%d'",
               MAX_PAY_TYPE_NAME_LEN,
               name.c_str(),
               name.length() );
        return false;
    }

    int res = libSetSettingsKSA_By( GET_PAY_TYPE, num,  (char*)name.c_str() );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if( !ok )
    {
        logERR("writePayType():: Ошибка записи Типа платежа. \n\tnum = %d, \n\tname = %s,",
               num,
               name.c_str() );
        return false;
    }
    logDBG("writePayType():: запись Типа платежа прошла успешно");

    return true;
}


//=====================================================================
//                  Чтение типа платежа из BIOS
bool BIOS::BEL::readPayType( const int num, string &name )
{
    const int GET_PAY_TYPE = 32;

    lock_guard<mutex> locker(*mutex_rb);

    if( num <  0 ||
        num >  (MAX_PAY_TYPES - 1) )
    {
        logERR("readPayType():: Ошибка! неверный индекс Типа платежа! num = %d", num);
        return false;
    }

    char tmpMass[ MAX_PAY_TYPE_NAME_LEN + 1 ];
    memset(tmpMass, 0x00, MAX_PAY_TYPE_NAME_LEN + 1 );

    int res = libGetSettingsKSA_By( GET_PAY_TYPE, num, tmpMass );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if( !ok )
    {
        logERR("readPayType():: Ошибка чтения Типа платежа № %d ", num );
        return false;
    }

    // На всякий случай
    if( strlen(tmpMass) > MAX_PAY_TYPE_NAME_LEN )
    {
        logERR("readPayType():: Караул! Имя Типа платежа № %d только что насрало в память на %d байт",
               num,
               ( strlen(tmpMass) - MAX_PAY_TYPE_NAME_LEN) );
        return false;
    }

    name = string(tmpMass); // Сохранение в классе, если удачно
    logDBG("readPayType()::    запись Типа платежа № %d прошла успешно", num );

    return true;
}

//=====================================================================
//                  Чтение отделов из BIOS
bool BIOS::BEL::readPayTypes()
{
    logDBG( "readPayTypes():: Чтение всех типов платежей ..." );

    PAY_TYPE pt;

    payTypes.clear();

    for( uint8_t  num = 0; num < MAX_PAY_TYPES; num++ )
    {
        logDBG( "readPayTypes():: Чтение типа № %d ...", num );
        pt.num = num;
        if( readPayType( pt.num, pt.name ) )
        {
            if( !(pt.name.empty()) )
            {
                while( ( pt.name.back() == ' ' )  && ( pt.name.length() > 1  ) )
                {
                    pt.name.pop_back();
                }
                payTypes.push_back( pt );
            }
        }
        else
        {
            logERR( "readPayTypes():: ОШИБКА! Не удалось считать тип платежа № ", num );
            return false;
        }
        if( !pt.name.empty() )
        {
            logERR( " \tИндекс = %d \t Тип оплаты = '%s'", pt.num, pt.name.c_str() );
        }
    }

    logDBG( "readPayTypes():: Все платежи успешно считаны" );
    return true;
}

int BIOS::BEL::getNextDocNum()
{
    int fd {-1};
    if (ksaInformationGet(KSA_INF_NEXT_DOC_NUM, fd))
    {
        logINFO("%s %d", __FUNCTION__, fd);
    }
    return fd;
}

//=====================================================================
//                  Запись номера отдела в BIOS
//=====================================================================
bool BIOS::BEL::dep_name_write( string name, int num )
{
    lock_guard<mutex> locker(*mutex_rb);

    if(num < 1 || num >  16)
    {
        logERR("dep_name_write:: Ошибка! неверный индекс Наименования отдела! num = %d", num);
        return false;
    }

    int res = libSetSettingsKSA_By( 50, num,  (char*)name.c_str() );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if( !ok )
    {
        logERR("dep_name_write():: Ошибка записи Наименования отдела. num = %d, name = %s,",
                                                                                        num,
                                                                                name.c_str() );
        return false;
    }
    logDBG("dep_name_write():: запись Наименования отдела прошла успешно");

    return true;
}
//=====================================================================
//                  Чтение номера отдела из BIOS  по умолчанию 1-го из 16
//=====================================================================
bool BIOS::BEL::dep_name_read( int num, string &dep_name )
{
    lock_guard<mutex> locker(*mutex_rb);

    if(num < 1 || num >  16)
    {
        logERR("dep_name_read:: Ошибка! неверный индекс Наименования отдела! num = %d", num);
        return false;
    }

    char name[100];
    memset(name, 0x00,100);

    int res = libGetSettingsKSA_By( 50, num, name );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if( !ok )
    {
        logERR("dep_name_read():: Ошибка чтения Наименования %d отдела.", num );
        return false;
    }
    dep_name = string(name); // Сохранение в классе, если удачно
    logDBG("dep_name_read()::    запись Наименования %d отдела прошла успешно", num );

    return true;
}
//=====================================================================
//                  Чтение отделов из BIOS
//=====================================================================
bool BIOS::BEL::deps_read()
{
    logDBG( "DEPARTMENTS READ ..." );

    DEP dep;
    DEPS.clear();
    bool result = true;

    const u_int8_t DEP_TABLE_SIZE = 16;

    for(u_int8_t dep_num = 1; dep_num <= DEP_TABLE_SIZE; dep_num ++)
    {
        logDBG( "%s %u DEPARTMENT READ ...", __PRETTY_FUNCTION__, dep_num );
        dep.num = dep_num;
        if(dep_name_read( dep_num, dep.name ))
        {
            if((dep_num <= DEFAULT_DEPS_CNT) && (dep.name.empty()))
            {
                dep.name = "Отдел " + to_string( dep_num );
                dep_name_write( dep.name, dep_num );
            }
            DEPS.push_back( dep );
        }
        else
        {
            result = false;
            break;
        }
    }
    logDBG( "READ %u DEPARTMENTS FROM BEL::BIOS", DEPS.size() );

    return result;
}
//=====================================================================
