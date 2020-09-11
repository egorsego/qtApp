//
// Created by krazh on 23.01.18.
//
#include <time.h>

#include "FisGo_BIOS.h"
#include "loggerdaemon.h"

bool          fb_wasErr     = false;    // ���� ��� BackgroundEvents, ������騩, �� �ந��諠 ���. �訡�� �� �믮������ ������� � ����
string        fb_errMess    =    "";
int           fb_errCode    =     0;
bool          fb_wasWarning = false;    // ���� ��� BackgroundEvents, ������騩, �� ������� �믮����� � ��୨����
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
    logWARN("����祭�� ��⠭�� �� (���⭨� �)");
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
    logINFO("%s\t\t%s���������", (ok ? " " : "�� "), __PRETTY_FUNCTION__);
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

    logINFO("getOfdDisconnectStatus() %s���������. dateStr = '%s'",(ok ? " " : "�� "), dateStr);

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
        logERR("%s ���⮩ ���� �� ��⨢�樨 �ਬ���-����!!!", __PRETTY_FUNCTION__);
        return false;
    }

    lock_guard<mutex> locker(*mutex_rf);
    int rc = libDreamkasKeySet(key);

    bool ok = ((rc == 0) ?  true : isCommandComplete(rc, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL));
    if (!ok)
    {
        logERR("%s �訡�� ��⨢�樨 �ਬ���-����!!!", __PRETTY_FUNCTION__);
        return false;
    }

    logDBG("%s ��⨢��� �ਬ���-���� �����訫��� �ᯥ譮!", __PRETTY_FUNCTION__);

    return true;
}

bool BIOS::RUS::dreamkasKeyPrintAll()
{
    lock_guard<mutex> locker(*mutex_rf);
    int rc = libDreamkasKeyPrintAll();

    bool ok = ((rc == 0) ?  true : isCommandComplete(rc, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL));
    if (!ok)
    {
        logERR("%s �訡�� ���� ᯨ᪠ ��� �ਬ���-����!!!", __PRETTY_FUNCTION__);
        return false;
    }

    logDBG("%s ����� ᯨ᪠ ��� �ਬ���-���� �����訫��� �ᯥ譮!", __PRETTY_FUNCTION__);

    return true;
}

//=====================================================================
// ���� ����� ��
bool BIOS::RUS::fs_status_get()
{
    #ifdef BIOS_RB
        return true;
    #endif
    // ����� ����� ��
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
    isReadyToFiscal       = (phaseOfLife & 0x01 ? true : false);    // ��᪠ ��� ���ﭨ� "��⮢����� � �᪠����樨"
    isFiscalMode          = (phaseOfLife & 0x02 ? true : false);    // ��᪠ ��� ���ﭨ� "��᪠��� ०��"
    isPostFiscalMode      = (phaseOfLife & 0x04 ? true : false);    // ��᪠ ��� ���ﭨ� "����䨪ᠫ�� ०��."
    //HZ_CHTO             = (phaseOfLife == 0x0F ? true : false);    // ��᪠ ��� ���ﭨ� "�⥭�� ������ �� ��娢� ��"
    isShiftOpen           = (fnState      & 0x20);                   // ��᪠ ��� ���ﭨ� "ᬥ�� �����"
    //--
    isDocOpen             = (curDocState == 0x00 ? false : true);    // ��᪠ ��� ���ﭨ� "���㬥�� ����� � ��"
    //--
    IMMEDIATE_REPLACEMENT = (warnFlags & BIOS::BIT_IMMEDIATE_REPLACEMENT);  // ��᪠ ��� ���ﭨ� "��筠� ������ ��"
    RESOURCES_EXHAUSTION  = (warnFlags & BIOS::BIT_RESOURCES_EXHAUSTION);   // ��᪠ ��� ���ﭨ� "���௠��� ����� ��"
    MEMORY_OVERFLOW       = (warnFlags & BIOS::BIT_MEMORY_OVERFLOW);        // ��᪠ ��� ���ﭨ� "��९������� ����� ��"
    CRITICAL_FN_ERROR     = (warnFlags & BIOS::BIT_CRITICAL_FN_ERROR);      // ��᪠ ��� ���ﭨ� "��९������� ����� ��"
    info( ALL_FLGS );
    return true;
}

//=====================================================================
//                  �⥭�� �⤥���
bool BIOS::RUS::readPayTypes()
{
    logDBG( "%s:: �⥭�� ��� ⨯�� ���⥦��...", __func__ );

    payTypes.clear();
    payTypes.push_back( PAY_TYPE(PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH,        PAY_TYPE_NAME_CASH    ) );
    payTypes.push_back( PAY_TYPE(PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASHLESS,    PAY_TYPE_NAME_CASHLESS) );
    payTypes.push_back( PAY_TYPE(PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_ADVANCE,     PAY_TYPE_NAME_ADVANCE ) );
    payTypes.push_back( PAY_TYPE(PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CREDIT,      PAY_TYPE_NAME_CREDIT  ) );
    payTypes.push_back( PAY_TYPE(PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_EXCHANGE,    PAY_TYPE_NAME_OTHER   ) );

    logDBG( "%s:: �� ���⥦� �ᯥ譮 ��⠭�", __func__ );
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
// ������ ���� ����
//=====================================================================
void BIOS::setIP      (string newIP)
{
    lock_guard<mutex> locker(mutex_bios);
    ip = newIP;
    libSetIP(ip);
}
//=====================================================================
// �஢�ઠ ���� १���� �� ����, ��� ��筥��� �� �� �믮���� ����� � �।�०������, ���� ���믮���� � �訡���
//=====================================================================
bool isCommandComplete(int res, FB_OPERATION_CRITICAL_LEVEL op_lvl)
{
    bool isCriticalError = false;
    // �᫨ res == 0 :
    if(res == 0) return true;
    // �᫨ res != 0 :
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
// �������� ᮥ������� � ������
//=====================================================================
bool BIOS::initConnection()
{
    unique_lock<mutex> locker(mutex_bios);

    // �᫨ ᮥ������� 㦥 ��⠭������, � ���� ��室��
    if( isConnected.load() == true )
    {
        logDBG("%s:: ���������� 㦥 ��⠭������, ��室", __func__);
        return true;
    }

    if(fg == nullptr)
    {
        logERR( "%s Null Fg object ptr!!!", __PRETTY_FUNCTION__ );
        return false;
    }

//    if(fg->isFileExist("debugBios") == FilesFG::FILE_IS_EXIST) // �᫨ �� ���� �ᣮ ���� ⠪�� 䠩� �� �⫠���
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
    isConnected.store((res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_CONNECTION));    // �᫨ १���� != 0, � �஢��塞, ����᪠� �� �訡��

    if (isConnected.load())
    {
        initBiosCv.notify_all();
    }

    #ifdef BIOS_RF
        RF->isConnected = isConnected.load();
    #else
        RB->isConnected = isConnected.load();
    #endif
    logDBG("initConnection():: ���������� � BIOS %s��⠭������. \n\t��� १���� = %02x      \n\tisConnected = %01x",
                                                                                              (res == 0 ? " " : "�� "),
                                                                                                                   res,
                                                                                                           isConnected.load() );
    if(!isConnected.load())
    {
        logERR("initConnection():: ����饭�� �� �訡�� = '%s'", fb_errMess.c_str() );
    }
    return isConnected.load();
}

//=====================================================================
// ����⨥ ᬥ�� � ����
//=====================================================================
bool BIOS::RUS::openShift(string cashier)
{
    lock_guard<mutex> locker(*mutex_rf);

    logDBG("����⨥ ᬥ��.... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("BIOS::openShift()  �訡��! ���������� �� �ந��樠����஢����!");
        return false;
    }

    if (isDocOpen_get())
    {
        // �����⨥ ࠭�� ����⮣� ���㬥�� �� ��� ����稨
        cancelDoc();
    }

    int res = libOpenShift(cashier.c_str());

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("����� %s�����! ��� �⢥� = %d",(ok ? " " : "�� "), res);

    return ok;
}
//=====================================================================
// ������
//=====================================================================
BIOS::RUS::Position::Position()
{
    punix.goodsName = "����� �� �����";
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
    name                   =         "����� �� �����";
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
// ����⨥ ������ ���㬥�� �� ����������
//=====================================================================
BIOS::RUS::OPEN_DOC::OPEN_DOC()
{
    cashier =                            "�����";
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
    cashier =                            "�����";
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
// ���⠥� 祪� �� ��
//=====================================================================
void BIOS::setPrinterExPrint()
{
    int mask_print = 0;
    getPrinterMode( mask_print );
    mask_print &= ~PIRIT_SET_KKT_NOT_PRINT;

    setPrinterMode( mask_print );
}
//=====================================================================
// �� ���⠥� 祪� �� ��
//=====================================================================
void BIOS::setPrinterExNotPrint()
{
    int mask_print = 0;
    getPrinterMode( mask_print );
    mask_print |= PIRIT_SET_KKT_NOT_PRINT;

    setPrinterMode( mask_print );
}
//=====================================================================
// ��ࠢ�� 祪� � BIOS RF
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
    // �����⨥ ࠭�� ����⮣� ���㬥�� �� ��� ����稨
    if (RF->isDocOpen_get())
    {
        logERR( "%s: �����⨥ ࠭�� ����⮣� ���㬥��!", __PRETTY_FUNCTION__ );
        RF->cancelDoc();
    }
    //---------------------------------------------------
    //--
    // ������ 祪
    // ������ ०��
    receipt->doc.fmt = DOC_FORMAT_PACKET;

    if(RF->openDoc( &receipt->doc ) == false)
    {
        logERR( "%s: �� 㤠���� ������ ���㬥��!", __PRETTY_FUNCTION__ );
        return false;
    }

    bool ok = true;
    //--
    // �������� ����樨
    double totalPosDiscount = 0;
    for (RUS::Position &pos : receipt->positions)
    {
        ok = RF->addPositionToCheck( &pos );
        if(!ok)
        {
            break;
        }
        // �᫨ ���� ��� ᪨��� �� ������, ����� ���� ᪨���, ���⠥� �� ��
        if( pos.discountRubSum != 0 )
        {
        ok = printDiscount( pos.discountRubSum, " ������" );
        if(  !ok )
        {
            logERR( "%s: �訡�� ���������� ᪨��� �� ������: '%s'", __PRETTY_FUNCTION__ , pos.discountRubName.c_str() );
            break;
        }
        totalPosDiscount+=pos.discountRubSum;
        }
    }
    //--
    // �������� �����
    if( ok )
    {
        PAYMENT addingPayment;
        // �����筠� ����� ��� ⨯��
        // � �� typePayment �ᥣ�� == MAX_PAY_TYPES
        if( receipt->payment.typePayment < BIOS::MAX_PAY_TYPES )
        {
            addingPayment.typePayment = receipt->payment.typePayment;
            addingPayment.sum = RF->convert->cop2rub( receipt->payment.sum );

            ok = addPaymentToCheck(&addingPayment);

            logINFO("%s:: ���������� ���⮩ ������::    ����� = %d �㬬� = %f",
                    __func__,
                    addingPayment.typePayment,
                    addingPayment.sum);

            if( !ok )
            {
                logERR("%s:: ������ ��� ���������� ������� ������!!!!! ����� = %d �㬬� = %f",
                       __func__,
                       addingPayment.typePayment,
                       addingPayment.sum);
            }
        }
        else if( !RF->recPayTypes.empty() )
        {
            for(const pair<int, uint64_t>& rpt : RF->recPayTypes)
            {
                // ������� ������ �����, �⮡� �������� �� � ����(�⮡� ᤠ� �뫠)
                if( (rpt.first == PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH)  || (rpt.second == 0) )
                {
                    continue;
                }
                addingPayment.typePayment = rpt.first;
                addingPayment.sum = RF->convert->cop2rub( rpt.second );
                logINFO("%s:: ���������� ᬥ蠭��� ������::    ����� = %d �㬬� = %f",
                       __func__,
                       addingPayment.typePayment,
                       addingPayment.sum);
                ok = addPaymentToCheck( &addingPayment );
                if( !ok )
                {
                    logERR("%s:: ������ ��� ���������� ��������� ������!!!!! ����� = %d �㬬� = %f",
                           __func__,
                           addingPayment.sum,
                           addingPayment.typePayment);
                    break;
                }
            }

            // ������塞 ������ ����� � ����, �᫨ ����
            int indexCash = PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH;
            const uint64_t ZERO_SUMMA = 0;
            if( ok && (RF->recPayTypes.count( indexCash ) != 0)  && (RF->recPayTypes.at( indexCash ) != 0)  )
            {
                addingPayment.typePayment =  PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH;
                addingPayment.sum = RF->convert->cop2rub( RF->recPayTypes.at(indexCash) );
                logINFO("%s:: ���������� ������� � ᬥ蠭��� �����:: �㬬� = %f",  __func__,
                       addingPayment.sum);
                ok = addPaymentToCheck( &addingPayment );
                if( !ok )
                {
                    logERR("%s:: ������ ��� ���������� �������� � ��������� ������!!!!! ����� = %d �㬬� = %f",
                           __func__,
                           addingPayment.typePayment,
                           addingPayment.sum);
                }
            }
            // �᫨ ���� �����⢥���� ����� � �㬬�� ����(᪨��� ��� ��� ������)
            else if( ok && (RF->recPayTypes.size() == 1)  && (RF->recPayTypes.begin()->second == ZERO_SUMMA)  )
            {
                addingPayment.typePayment =  RF->recPayTypes.begin()->first;
                addingPayment.sum = ZERO_SUMMA;
                logINFO("%s:: ���������� ��������� ������ �� 0",  __func__ );
                ok = addPaymentToCheck( &addingPayment );
                if( !ok )
                {
                    logERR("%s:: ������ ��� ���������� ��������(=����)!!!!! ����� = %d �㬬� = %f",
                           __func__,
                           addingPayment.typePayment,
                           addingPayment.sum);
                }
            }
        }
        // �᫨ �� �����筠� � �� ᬥ蠭��� �����, � �訡��
        else
        {
            ok = false;
            logERR("%s:: ����������� ��� ������! �����४�� ������ ������ = %d",__func__, receipt->payment.typePayment );
        }
    }
    // ������
    if( ok && (totalPosDiscount!=0) )
    {
        ok = printDiscount( totalPosDiscount, "���� ������" );
        if(  !ok )
        {
            logERR( "%s: �訡�� ���������� ᪨��� �� 祪: '%s'",      __PRETTY_FUNCTION__ , receipt->discountRubName.c_str() );
        }
    }
#ifdef BIOS_RF
    //--
//    // ������ �� 祪
//    if ( ok )
//    {
//        ok = (RF->addAgentToCheck(receipt->agent) == EXIT_SUCCESS);
//        if(!ok)
//        {
//            logERR("%s: �訡�� ���������� ����⮢ �� 祪!!!", __PRETTY_FUNCTION__ );
//        }
//    }
    // �������⥫�� ४����� 祪� (���)
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
    // ���� ����⮢
    if (!receipt->clcPlace.empty())
    {
        constexpr size_t MAX_LEN {256};
        receipt->clcPlace = receipt->clcPlace.substr(0, MAX_LEN);
    }
#endif
    //--
    // �������� ���㬥��
    if(ok)
    {
        ok = RF->closeDoc( *receipt );
    }
    //----------------------------------------------------------
    // �⬥���� ���㬥�� �᫨ �� � ��諮 �� ⠪ ��᫥ ������
    if ( !ok )
    {
        lastErrorExtMessage = RF->getExtendedErrorCode();
        logERR( "%s:: ����७��� ᮮ�饭�� �� �訡��: '%s'", __func__, Convert::escapingOfQuotes(lastErrorExtMessage).c_str() );
        RF->cancelDoc();
    }
    if(ok)
    {
        logDBG( "�������� �������� ���� � ����" );
    }
    else
    {
        logERR( "������ �������� ���� � ����!!!" );
    }

    return ok;
}


//=====================================================================
// ��ࠢ�� 祪� � BIOS RB
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
    // ������ 祪
    // ������ ०��
    receipt->doc.fmt = DOC_FORMAT_PACKET;
    if (!RB->openDoc(&receipt->doc))
    {
        logERR( "%s: �� 㤠���� ������ ���㬥��!" );
        return false;
    }

    bool ok = true;
    //--
    // �������� ����樨
    for ( BEL::POSITION pos : receipt->positions )
    {
        ok = RB->addPositionToCheck( &pos );
        if( !ok )
        {
            logERR( "%s: �訡�� ���������� ����樨 � 祪! '%s'",      __PRETTY_FUNCTION__ , pos.name.c_str() );
            break;
        }
        // �᫨ ���� ��� ᪨��� �� ������, ����� ���� ᪨���
        if( !pos.discountRubName.empty() )
        {
            //ok = RB->addDiscountRubles(pos.discountRubName.c_str(), pos.discountRubSum, pos.discountType );
            if(  !ok )
            {
                logERR( "%s: �訡�� ���������� ᪨��� �� ������: '%s'",      __PRETTY_FUNCTION__ ,
                                                                       pos.discountRubName.c_str() );
                break;
            }
        }

        // �᫨ ���� ��� �������� �� ������, ����� ���� ��������
        if( ok && !pos.marginRubName.empty() )
        {
            ok = RB->addMarginRubles(pos.marginRubName.c_str(), pos.marginRubSum);
            if( !ok )
            {
                logERR( "%s: �訡�� ���������� �������� �� ������: '%s'",      __PRETTY_FUNCTION__ ,
                                                                         pos.marginRubName.c_str() );
                break;
            }
        }
    }
    //--
    // ����⮣
    if( ok )
    {
        ok = RB->subTotal();
    }
    //--
    // ������
    if( ok && !receipt->discountRubName.empty() )
    {
        //ok = RB->addDiscountRubles(receipt->discountRubName.c_str(), receipt->discountRubSum, receipt->discountType );
    }
    //--
    // ��������
    if( ok && !receipt->marginRubName.empty() )
    {
        ok = RB->addMarginRubles(receipt->marginRubName.c_str(), receipt->marginRubSum);
    }
    //--
    // �������� ����� ��
    if( ok )
    {
        // �����筠� ����� ��� ⨯��
        if( receipt->payment.typePayment < BIOS::MAX_PAY_TYPES )
        {
            ok = addPaymentToCheck(&receipt->payment);
            logERR("%s:: ���������� ���⮩ ������::    ����� = %d �㬬� = %f",
                                                               __func__,
                                                               receipt->payment.typePayment,
                                                               receipt->payment.sum);
        }
        // ���蠭��� ����� �᫨ ���� ���� �����������
        else if( !RB->recPayTypes.empty() )
        {
            PAYMENT addingPayment;
            for(pair<int, uint64_t> rpt : RB->recPayTypes)
            {
                // ������� ������ �����, �⮡� �������� �� � ����(�⮡� ᤠ� �뫠)
                if( (rpt.first == PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH) || (rpt.second == 0) )
                {
                    continue;
                }
                addingPayment.typePayment = rpt.first;
                addingPayment.sum = RB->convert->cop2rub( rpt.second );
                logERR("%s:: ���������� ᬥ蠭��� ������::    ����� = %d �㬬� = %f",
                                                                           __func__,
                                                                           addingPayment.typePayment,
                                                                           addingPayment.sum);
                ok = addPaymentToCheck( &addingPayment );
                if( !ok )
                {
                    logERR("%s:: ������ ��� ���������� ��������� ������!!!!! ����� = %d �㬬� = %f",
                                                                                   __func__,
                                                                                   addingPayment.sum,
                                                                                   addingPayment.typePayment);
                    break;
                }
            }

            // ������塞 ������ ����� � ����, �᫨ ����
            int indexCash = PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH;
            if( ok && RB->recPayTypes.count( indexCash ) != 0 )
            {
                addingPayment.typePayment =  PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH;
                addingPayment.sum = RB->convert->cop2rub( RB->recPayTypes.at(indexCash) );
                logERR("%s:: ���������� ������� � ᬥ蠭��� �����:: �㬬� = %f",  __func__,
                                                                         addingPayment.sum);
                ok = addPaymentToCheck( &addingPayment );
                if( !ok )
                {
                    logERR("%s:: ������ ��� ���������� �������� � ��������� ������!!!!! ����� = %d �㬬� = %f",
                                                                                       __func__,
                                                                                       addingPayment.sum,
                                                                                       addingPayment.typePayment);
                }
            }
        }
        // �᫨ �� �����筠� � �� ᬥ蠭��� �����, � �訡��
        else
        {
            ok = false;
            logERR("%s:: ����������� ��� ������! �����४�� ������ ������ = %d",__func__, receipt->payment.typePayment );
        }
    }
    //--
    // �������� ���㬥��
    if( ok )
    {
        ok = RB->closeDoc();
    }

    //----------------------------------------------------------
    // �⬥���� ���㬥�� �᫨ �� � ��諮 �� ⠪ ��᫥ ������
    if ( !ok )
    {
        RB->cancelDoc();
    }
    if(ok)
    {
        logDBG( "�������� �������� ���� � ����" );
    }
    else
    {
        logERR( "������ �������� ���� � ����!!!" );
    }
    return ok;
}


//=====================================================================
bool BIOS::RUS::openDoc( OPEN_DOC *open_doc )

{
    logDBG("BIOS::openDoc()  ����⨥ ���㬥�� �� ����������...");
    if( !isConnected )
    {
        logERR("BIOS::openDoc()  �訡��! ���������� �� �ந��樠����஢����!");
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

    logDBG("���㬥�� %s�����! ��� �⢥� = %d",(ok ? " " : "�� "), res);
    return ok;
}
//=====================================================================
bool BIOS::BEL::getDocFromKL(const uint32_t &docNum, DocFromKL &doc)
{
    logDBG("%s", __PRETTY_FUNCTION__);
    bool ok(false);

    if (!isConnected || (mutex_rb == nullptr))
    {
        logERR("%s\t�訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return ok;
    }

    lock_guard<mutex> locker(*mutex_rb);

    int res(libGetDocFromKL(docNum, doc));
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("%s\t��� �⢥� = %d", __PRETTY_FUNCTION__, res);
    return ok;
}
//=====================================================================
bool BIOS::BEL::openDoc( OPEN_DOC *open_doc )
{
    logDBG("BIOS::BEL::openDoc()  ����⨥ ���㬥�� �� ����������...");
    if( !isConnected )
    {
        logERR("BIOS::openDoc()  �訡��! ���������� �� �ந��樠����஢����!");
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

    logDBG("���㬥�� %s�����! ��� �⢥� = %d",(ok ? " " : "�� "), res);
    return ok;
}


//=====================================================================
// ����祭�� ������ �� ��᫥����� 祪�
//=====================================================================
bool BIOS::RUS::rec_fis_data_get(BIOS::RUS::RECEIPT_FIS_DATA &recieptData)
{
    lock_guard<mutex> locker(*mutex_rf);
    int res  = libGetLastRecieptData(  recieptData.type,         // ⨯ 祪� (��� ���㫨஢. = 0)
                                       recieptData.cur_op_cnt,   // ⥪�騩 �����. ���稪
                                       recieptData.rec_num,      // ����� 祪�
                                       recieptData.doc_num,      // ����� ���㬥��
                                       recieptData.sum,          // �㬬� 祪�
                                       recieptData.discount,     // �㬬� ᪨���
                                       recieptData.fpd,          // ��ப� �� (��. �ਧ���)
                                       recieptData.fd         ); // ����� ��

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("RUS rec_fis_data_get():: %s���������! fpd = %s \t fd = %d", (ok ? " " : "�� "), recieptData.fpd.c_str(), recieptData.fd);

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

    logDBG("BEL KLDocRead:: %s���������!", (res == 0) ? " " : "�� ");

    return (res == 0) ?  true : false;
}
//=====================================================================
bool BIOS::BEL::KLDocPrint( int doc_num )
{
    lock_guard<mutex> locker(*mutex_rb);

    int res = libKLDocPrint( doc_num );

    logDBG("BEL KLDocPrint:: %s���������!", (res == 0) ? " " : "�� ");

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
        logERR("%s\t\t�� ���������!",  __PRETTY_FUNCTION__ );
        return false;
    }

    string strTime;
    res = getStrServiceInfoSKNO(PIRIT_SKNO_DATETIME, PIRIT_DATETIME_TIME, strTime);
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR("%s\t\t�� ���������!",  __PRETTY_FUNCTION__ );
        return false;
    }

    locker.unlock();

    string strDateTime = strDate + strTime;
    if ( convert->DateISO8601Create(dateIso, strDateTime, true) )
    {
        logERR("%s\t\t���������!  ��� � �६� ���� = %s", __PRETTY_FUNCTION__, strDateTime.c_str());
        return true;
    }
    else
    {
        logERR("%s\t\t�訡��! �����४�� ��� � �६� ���� = %s", __PRETTY_FUNCTION__, strDateTime.c_str());
        return false;
    }

    return (res == 0) ?  true : false;
}
//=====================================================================
bool BIOS::BEL::rec_fis_data_get(BIOS::BEL::RECEIPT_FIS_DATA &recieptData)
{
    lock_guard<mutex> locker(*mutex_rb);
    int res  = libGetLastRecieptData_By(  recieptData.type,         // ⨯ 祪� (��� ���㫨஢. = 0)
                                          recieptData.cur_op_cnt,   // ⥪�騩 �����. ���稪
                                          recieptData.rec_num,      // ����� 祪�
                                          recieptData.doc_num,      // ����� ���㬥��
                                          recieptData.sum,          // �㬬� 祪�
                                          recieptData.discount,     // �㬬� ᪨���
                                          recieptData.markup);      // �㬬� ��業��

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("BEL rec_fis_data_get():: %s���������!", (ok ? " " : "�� "));

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
// ����� ���� �� ����� �� ���
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

    logDBG("BEL print_report_bep():: %s���������!", (ok ? " " : "�� "));

    return ok;
}
//=====================================================================
// ��ᯥ���� ���� ����஫쭮� �����
//=====================================================================
bool BIOS::BEL::print_report_ctrl_tape()
{
    lock_guard<mutex> locker(*mutex_rb);

    int res = libBLRPrintControlTapeFromECT();

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);

    logDBG("BEL print_report_kl():: %s���������!", (ok ? " " : "�� "));
    return ok;
}

BIOS::BEL::KCA_SCNO_STATE BIOS::BEL::getScno_state() const
{
    return scno_state;
}
//=====================================================================
// ������ �㬬� ������� � �������� �騪� (� ��������)
//=====================================================================
bool BIOS::getCashInDrawer (uint64_t &cash)
{
    lock_guard<mutex> locker(mutex_bios);
    char dataSum[100] = {0x00};
    int res = getKKTInfo(0x07, dataSum);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    double cashPubli = QString(dataSum).toFloat();
    cash = static_cast<uint64_t>(cashPubli * 100 + .5);

    logDBG("getCashInDrawer:: %s���������! �㬬� � �������� �騪� � �������� = %llu", (ok ? " " : "�� "), cash);
    return ok;
}
//=====================================================================
// ������ �����᪮� ����� ���
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

    logDBG("%s\t\t %s���������!   �����᪮� ����� ��� = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), plantNum.c_str());
    return ok;
}
//=====================================================================
// ������ ��� �࣠����樨
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

    logDBG("%s\t %s���������!   ��� �࣠����樨 = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), inn.c_str());
    return ok;
}
//=====================================================================
// ������ ॣ����樮��� ����� ���
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

    logDBG("%s\t\t %s���������!   �������樮��� ����� ��� = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), regNum.c_str());
    return ok;
}
//=====================================================================
// ������ ���� � �६� ��᫥���� �᪠�쭮� ����樨
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

    logERR("%s\t%s���������!   ��� � �६� ��᫥���� �᪠�쭮� ����樨 = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), dateTime.c_str());
    return ok;
}
//=====================================================================
// ������ ���� ��᫥���� ॣ����樨/���ॣ����樨
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

    logDBG("%s\t%s���������!   ��� ��᫥���� ॣ����樨/���ॣ����樨 = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), date.c_str());
    return ok;
}


//=====================================================================
// ��� ����砭�� �६������ �����  ��
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
    logDBG("%s\t%s���������!    ��� ����砭�� �६������ ����� �� = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), date.c_str());
    return ok;
}


//=====================================================================
// ������ ���� � �६� ������ ᬥ��
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

    logDBG("%s\t%s���������!   ��� � �६� ������ ᬥ�� = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), dateTime.c_str());
    return ok;
}
//=====================================================================
// ������ �ਧ���� � ����� ����⮢
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

    logDBG("%s\t\t%s���������!   �ਧ���� � ����� ����⮢ = %d", __PRETTY_FUNCTION__, (ok ? " " : "�� "), status);
    return ok;
}
//=====================================================================
// ������ ����㯭� ���
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

    logDBG("%s\t\t%s���������!   ��� = %d", __PRETTY_FUNCTION__, (ok ? " " : "�� "), taxes);
    return ok;
}
//=====================================================================
// ������ �ਧ��� ������ �ਭ��
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

    logDBG("%s\t\t%s���������!   ��⠭���� �ਭ�� � ��� ०��� = %d", __PRETTY_FUNCTION__, (ok ? " " : "�� "), status);
    return ok;
}
//=====================================================================
// ������ ������������ �࣠����樨
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

    logDBG("%s\t%s���������!   ���� �࣠����樨 = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), orgAddress.c_str());

    return ok;
}
//=====================================================================
// ������ ������������ �࣠����樨
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

    logDBG("%s\t%s���������!   ������������ �࣠����樨 = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), orgName.c_str());

    return ok;
}
//=====================================================================
// ������ ���� ���⮢
//=====================================================================
bool BIOS::getCalculationPlace(string &calcPlace)
{
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(calcPlace, PIRIT_TABLE_CALCULATION_PLACE, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("%s\t%s���������!   ���� ���⮢ = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), calcPlace.c_str());

    return ok;
}
//=====================================================================
// ������ ���ﭨ� ����㯠 � ���
//=====================================================================
bool BIOS::getOFDStatus(int &status)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = libGetStatusOFD( status );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("%s\t%s���������!", __PRETTY_FUNCTION__, (ok ? " " : "�� ") );

    if ( status == 0 )
    {
        logERR("%s\t\t ��� ����㯥�", __PRETTY_FUNCTION__);
    }
    else if ( status == 1 )
    {
        logERR("%s\t\t ��� �� ����㯥�", __PRETTY_FUNCTION__);
    }
    else
    {
        logERR("%s\t\t ��� �� ����஥�", __PRETTY_FUNCTION__ );
    }

    return ok;
}
//=====================================================================
// ������ ������������ ���
//=====================================================================
bool BIOS::getOFDName(string &ofdName)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(ofdName, PIRIT_TABLE_OFD_NAME, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t%s���������!������������ ��� = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), ofdName.c_str());

    return ok;
}
//=====================================================================
// ������ ���� ���
//=====================================================================
bool BIOS::getOFDPort(int &ofdPort)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getIntFromSettingsTable(ofdPort, PIRIT_TABLE_OFD_PORT, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t\t%s���������!���� ��� = %d", __PRETTY_FUNCTION__, (ok ? " " : "�� "), ofdPort);

    return ok;
}
//=====================================================================
// ������ IP ���
//=====================================================================
bool BIOS::getOFDAddress(string &ofdAddress)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(ofdAddress, PIRIT_TABLE_OFD_IP, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t%s���������!  ���� ��� = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), ofdAddress.c_str());

    return ok;
}
//=====================================================================
// ������ ��� ���
//=====================================================================
bool BIOS::getOFDINN(string &ofdINN)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(ofdINN, PIRIT_TABLE_OFD_INN, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t%s���������!  ��� ��� = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), ofdINN.c_str());

    return ok;
}
//=====================================================================
// ������ ���� ��ࠢ�⥫� 祪��
//=====================================================================
bool BIOS::getRecSenderEmail(std::string& recSenderEmail)
{
#ifdef BIOS_RB
    return true;
#endif
    std::lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(recSenderEmail, PIRIT_REC_SENDER_EMAIL, PIRIT_PARAM_0);
    bool ok = (res == 0) ? true : isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t%s���������!���� ��ࠢ�⥫� 祪�� = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), recSenderEmail.c_str());

    return ok;
}
//=====================================================================
// ������ ���� ����砭�� ����� ��
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

    logDBG("%s\t\t%s���������!  ������ ���祩 �� = %d", __PRETTY_FUNCTION__, (ok ? " " : "�� "), date);
    return ok;
}
//=====================================================================
// ������ ����� ��⮬��
//=====================================================================
bool BIOS::getAutomatNum(string &number)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    int res = getStrFromSettingsTable(number, PIRIT_TABLE_AUTOMAT_NUM, PIRIT_PARAM_0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("%s\t\t%s���������!  ����� ��⮬�� = %s", __PRETTY_FUNCTION__, (ok ? " " : "�� "), number.c_str());

    return ok;
}
//=====================================================================
// ����� ��� ���
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
        logERR("%s\t\t�� ���������!",  __PRETTY_FUNCTION__ );
        return false;
    }
    ffdKktVer = static_cast<uint8_t>(tmp);
    logDBG("%s\t\t\t���������!  ����� ��� ��� = %d", __PRETTY_FUNCTION__, ffdKktVer);

    return true;
}
//=====================================================================
// ��ॣ����஢����� ����� ��� ��
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
        logERR("%s\t\t�� ���������!",  __PRETTY_FUNCTION__ );
        return false;
    }
    fsFfdVer = static_cast<uint8_t>(tmp);

    logDBG("%s\t\t\t���������!  ����� ��� �� = %d", __PRETTY_FUNCTION__, fsFfdVer);

    return true;
}
//=====================================================================
// ����� ���
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
        logERR("%s\t\t�� ���������!",  __PRETTY_FUNCTION__ );
        return false;
    }
    logDBG("%s\t\t���������!  ����� ��� = %s", __PRETTY_FUNCTION__, kktVer.c_str());

    return true;
}
//=====================================================================
// ��� � �६� � ��
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
        logERR("%s\t\t�� ���������!",  __PRETTY_FUNCTION__ );
        return false;
    }

    return ok;
}
//=====================================================================
// ��� � �६� �� ��
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
        logERR("%s\t\t�� ���������!",  __PRETTY_FUNCTION__ );
        return false;
    }

    string _time;
    res = getStrFromInfoFN( PIRIT_INFO_FN_DATETIME, PIRIT_DATETIME_TIME, _time);
    ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR("%s\t\t�� ���������!",  __PRETTY_FUNCTION__ );
        return false;
    }

    locker.unlock();

    string strDateTime = _date + _time;

    if ( convert->DateISO8601Create(dateIso, strDateTime, true) )
    {
        logDBG("%s\t\t���������!  ��� � �६� �� = %s", __PRETTY_FUNCTION__, strDateTime.c_str());
        return true;
    }

    return false;
}

// ��������� ���䨣 �� PIRIT
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
    // �����뢠�� �᫨ �᪠��� ��� �����᪠��� ०��
    if ( (RF->isFiscalMode_get() == true) || (RF->isPostFiscalMode_get() == true) )
    {
        config.stage = FISCAL;
    }

    return true;
}
//=====================================================================
// ��������� �ਧ���� � ����� ����⮢
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
        logERR("%s:  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    lock_guard<mutex> locker(mutex_bios);
    logDBG("getBatteryVoltage:: ����� ����殮��� ���३��...");

    voltage = 0;
    int res = libGetBatteryVoltage(voltage);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);

    logWARN("%s:\t\tV���३�� = %s mV", __PRETTY_FUNCTION__, to_string(voltage).c_str());

    return ok;

}

bool BIOS::restartKKT()
{
    lock_guard<mutex> locker(mutex_bios);
    logERR("%s ��२��樠������ ����...", __PRETTY_FUNCTION__);

    int res = libRestartKKT();
    return (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
}
//=====================================================================
// ����� ���㬥�� �� ��娢�
//=====================================================================
bool BIOS::getDocFromFN (int fdNumber, char *hexData, int lenBuf)
{
#ifdef BIOS_RB
    return true;
#endif
    lock_guard<mutex> locker(mutex_bios);
    logDBG("getDocFromFN:: ����� ���㬥�� �%d �� ��娢� ��...", fdNumber);
    int res = libGetInfoFromECT_FP(fdNumber, hexData);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR("getDocFromFN::�� ���������! ����� ���㬥�� �%d �� ��娢� ��... -- ERROR! -- ����� = ", lenBuf);
        return false;
    }
    logDBG("getDocFromFN:: ����� ���㬥�� �%d �� ��娢� ��... �K! ����� = ", lenBuf);

    return ( res == 0 ? true : false );
}


//=====================================================================
//  ����� � ⥪�饬 ���ﭨ� ����⮢
//=====================================================================
bool BIOS::RUS::calcStateReport( string cashier_name )
{
    lock_guard<mutex> locker(*mutex_rf);

    if (isDocOpen_get())
    {
        // �����⨥ ࠭�� ����⮣� ���㬥�� �� ��� ����稨
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
//  ���ᥭ�� / ����⨥ �㬬�
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
    // �����⨥ ࠭�� ����⮣� ���㬥�� �� ��� ����稨
    if ( isDocOpen_get() )
    {
        cancelDoc();
        logWARN( "%s: �����⨥ ࠭�� ����⮣� ���㬥��!", __PRETTY_FUNCTION__ );
    }
    //---------------------------------------------------
    if( openDoc( doc ) == false)
    {
        logERR( "OPEN DOC ERROR!!!" );
        return false;
    }
    bool ok = true;
    // ���ᥭ��/����⨥
    if (libCashInOut( "�㡫�", sum ) != 0x00)
    {
        logERR( "libCashInOut ERROR!!!" );
        ok =  false;
    }
    // ������� ���㬥��
    if(ok)
    {
        ok = closeDoc("buyerAddress@dreamkas.ru");
    }
    // �᫨ �� � ��諮 �� ⠪ ���㫨�㥬
    if( !ok )
    {
        cancelDoc();
    }
    return ok;
}


//=====================================================================
//  ���ᥭ�� / ����⨥ �㬬� ��
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
    // �����⨥ ࠭�� ����⮣� ���㬥�� �� ��� ����稨
    if ( isDocOpenedGet() )
    {
        cancelDoc();
        logWARN( "%s: �����⨥ ࠭�� ����⮣� ���㬥��!", __PRETTY_FUNCTION__ );
    }

    if( openDoc( doc ) == false)
    {
        logERR( "OPEN DOC ERROR!!!" );
        return false;
    }

    bool ok = true;
    // ���ᥭ��/����⨥
    if (libCashInOut( "�㡫�", sum ) != 0x00)
    {
        logERR( "libCashInOut ERROR!!!" );
        ok =  false;
    }
    // ������� ���㬥��
    if(ok)
    {
        ok = closeDoc();
    }
    // �᫨ �� � ��諮 �� ⠪ ���㫨�㥬
    if( !ok )
    {
        cancelDoc();
    }
    return ok;
}


//=====================================================================
// ������� ��
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
// �����⨥ ���㬥��
//=====================================================================
bool BIOS::RUS::closeDoc(RUS::RECEIPT& receipt)
{
    logDBG("�����⨥ ���㬥��...");

    if (!isConnected)
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
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
        logDBG("���㬥�� ������! ��� �⢥� = %d. ����饭�� ='%s'", closeRes.errCode, strFromErrCode(closeRes.errCode).c_str());
        receipt.fd         = closeRes.fd;
        receipt.fpd        = closeRes.fpd;
        receipt.shiftNum   = closeRes.shiftNum;
        receipt.docInShift = closeRes.docInShift;
        receipt.dateDoc    = closeRes.dateDoc;
        receipt.timeDoc    = closeRes.timeDoc;
    }
    else
    {
        logERR("���㬥�� �� ������! ��� �⢥� = %d. ����饭�� ='%s'", closeRes.errCode, strFromErrCode(closeRes.errCode).c_str());
        fillErrorCodes(static_cast<FB_ERROR_CODE>(closeRes.errCode));
    }

    return ok;
}


bool BIOS::RUS::closeDoc(const string &buyerAddress)
{
    logDBG("�����⨥ ���㬥��...");

    if (!isConnected)
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    CloseDocResult closeRes(libCloseDocumentLite(1, buyerAddress, 0));

    bool ok((closeRes.errCode == 0) ?  true :  isCommandComplete(closeRes.errCode, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL));

    if (ok)
    {
        logDBG("���㬥�� ������! ��� �⢥� = %d. ����饭�� ='%s'", closeRes.errCode, strFromErrCode(closeRes.errCode).c_str());
    }
    else
    {
        logERR("���㬥�� �� ������! ��� �⢥� = %d. ����饭�� ='%s'", closeRes.errCode, strFromErrCode(closeRes.errCode).c_str());
        fillErrorCodes(static_cast<FB_ERROR_CODE>(closeRes.errCode));
    }

    return ok;
}

bool BIOS::BEL::closeDoc()
{
    logDBG("�����⨥ ���㬥��...");
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }
    int res = libCloseDocument_By( 1 ).errCode;    //  ���� ��१��
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("���㬥�� %s ������! ��� �⢥� = %d. ����饭�� ='%s'", (ok ? " " : "�� "), res, strFromErrCode(res).c_str());
    return ok;
}
//=====================================================================
// ���㫨஢���� ���㬥��
//=====================================================================
bool BIOS::RUS::cancelDoc()
{
    logWARN("���㫨஢���� ���㬥��...");
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }
    int res = libCancelDocument();
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if(ok)
    {
        logDBG("���㬥�� ������! ��� �⢥� = %d", res);
    }
    else
    {
        logERR("���㬥�� �� ������! ��� �⢥� = %d", res);
    }

    return ok;
}
bool BIOS::BEL::cancelDoc()
{
    logWARN("���㫨஢���� ���㬥��...");
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }
    int res = libCancelDocument();
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("���㬥�� %s ������! ��� �⢥� = %d", (ok ? " " : "�� "), res);
    return ok;
}
//=====================================================================
// ��� ॣ����樨
//=====================================================================
bool BIOS::RUS::getDateOfReg( string &dateTime )
{
    lock_guard<mutex> locker(*mutex_rf);

    logDBG("����祭�� ���� ॣ����樨...");
    if( !isConnected )
    {
        logERR("BIOS::getDateOfReg()::  �訡��! ���������� �� �ந��樠����஢����!");
        return false;
    }
    char dt[40];
    memset(dt, 0, sizeof(dt));
    int res = libGetRegDateTime(dt);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    dateTime = dt;

    if(ok)
    {
        logDBG("BIOS::getDateOfReg()::��������� ��� ॣ����樨:'%s'! ��� �⢥� = %d",  dateTime.c_str(), res);
    }
    else
    {
        logERR("BIOS::getDateOfReg()::�� ��������� ��� ॣ����樨:'%s'! ��� �⢥� = %d", dateTime.c_str(), res);
    }

    return ok;
}
//=====================================================================
// ������ ����� �� ��᫥���� ॣ����樨
//=====================================================================
bool BIOS::RUS::getNumFDofReg( int &fdNumberReg )
{
    lock_guard<mutex> locker(*mutex_rf);

    logDBG("����祭�� ����� �� ॣ����樨...");
    if( !isConnected )
    {
        logERR("BIOS::getNumFDofReg()::  �訡��! ���������� �� �ந��樠����஢����!");
        return false;
    }
    int res = libGetRegFDNumber(fdNumberReg);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("BIOS::getDateOfReg()::%s ����� �� ॣ����樨 = %d. ��� �⢥� = %d", (ok ? " " : "�� ���������!!!!"), fdNumberReg, res);
    return ok;
}


//=====================================================================
// ���㧪� �� �� ����� �� ��
//=====================================================================
bool BIOS::RUS::fiscalDocBinUnload         (  int          fd_number,
                                              char         **doc_data,
                                              int              &size,
                                              bool          &isCheck,
                                              int           &docType)
{
    lock_guard<mutex> locker(*mutex_rf);
    logDBG("BIOS::fiscalDocBinUnload():: ����祭�� ���㬥�� �� �� �� ������(%d)...", fd_number);
    if( !isConnected )
    {
        logERR("BIOS::fiscalDocBinUnload():: �訡��! ���������� �� �ந��樠����஢����!");
        return false;
    }
    int res = libGetFnDocBin(   fd_number,   // ����� ���㬥��
                                 doc_data,   // ����  ���㬥��
                                     size ,  // ����� ���㬥��
                                  isCheck,   // �� ���⠭��
                                  docType);  // ��� ���㦠����� ���㬥��
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    return ok;
}

bool BIOS::RUS::setPosAddReq(string requisite)
{
    lock_guard<mutex> locker(*mutex_rf);
    logDBG("%s", __PRETTY_FUNCTION__);

    if(!isConnected)
    {
        logERR("%s �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libSetPosAddReq(requisite);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    return ok;
}

//=====================================================================
// insert 4 bytes LE <⨯><⨯><�����><�����>
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
// ������ STLV ��᫥���� ॣ����樨/���ॣ����樨
bool BIOS::RUS::getSTLVofReg(vector <uint8_t>& stlvReg, int&  typeFd)
{
    logINFO("%s:: ����祭�� TLV �������� ॣ����樨/���ॣ����樨...", __func__);
    if( !stlvReg.empty() )
    {
        stlvReg.clear();
    }
#ifdef BIOS_RB
    logERR("%s:: ��室! ��⮤ ⮫쪮 ��� ��!", __func__);
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
        logERR("%s::  �訡��! ���������� �� �ந��樠����஢����!", __func__);
        return false;
    }

    //--------------------------------
    // 1 ��⠥� �ਧ���� ��� ��������
    int res = libGetRegSTLV( regType );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok  || (regType == PIRIT_REG_DOC_TYPE::PIRIT_REG_DOC_TYPE_UNDEFINED) )
    {
        logERR("%s:: �訡��! �� 㤠���� ���㧨�� �ਧ���� ���� �������� ॣ����樨!", __func__);
        return false;
    }
    typeFd = static_cast<int>(regType);
    logWARN("%s:: Answer = %d, registration type = %d", __func__, res, typeFd );

    //--------------------------------
    // 2 ��⠥� ᠬ� ��� �� 0�48 - ����� �� ��
    // ���� ����� ����
    while( libGetRegistrTLV(tlv, lenOfTLV, READ_REGISTRATION) == 0 )
    {
        if(lenOfTLV !=0 )
        {
            ok = Convert::convertASCIImassToHex(tlv, lenOfTLV);
            if( !ok )
            {
                logERR("%s:: �訡�� �� �६� ������� 2� 奪� � 1� 奪�", __func__ );
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
// ������ ����� �� � ��� ��᫥���� ॣ����樨/���ॣ����樨
//=====================================================================
bool BIOS::RUS::getFDandFPDofReg( int      &numberOfFD,
                                  uint32_t     &FPDnum)
{
    vector<Requisites::Requisite>      reqList;
    int     lenOfTLV   = 0;
    int     lenTotal   = 0;
    char    tlv[1000];

    lock_guard<mutex> locker(*mutex_rf);
    logDBG("����祭�� TLV �������� ॣ����樨/���ॣ����樨...");
    if( !isConnected )
    {
        logERR("BIOS::getFDandFPDofReg()::  �訡��! ���������� �� �ந��樠����஢����!");
        return false;
    }

    //--------------------------------
    // 1 ��⠥� �ਧ���� ��� ��������
    int typeFd{0};
    int res = libGetRegistrSTLVProps( numberOfFD, lenTotal, typeFd );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("%s:: ����� ����� TLV ��������  = %d. ����� �� = %d. ��� �⢥� = %d", __func__, lenTotal, numberOfFD, res);
    if( !ok )
    {
        return false;
    }

    //--------------------------------
    // 2 ��⠥� ᠬ� ��� �� 0�48 - ����� �� ��
    int i = 0;
    // ���� ����� ����
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
        // ���ᨬ ��� � ����� ४����⮢(+1 �����)
        uint16_t offs = 0;
        if( !tlv_parse((unsigned char*)tlv, &reqList, offs) )
        {
            logERR( "%s:: ��室, �訡�� �� �६� ���ᨭ�� ���!", __func__ );
            return false;
        }

        //--------------------------------
        // �᫨ ���㧨�� �㦭� ⥣, � ���뢠����, � ��⨬ ��� �६���
        if(reqList.back().tag == 1077) // FPD tag
        {
            res = 8888; // break;
        }
        //--------------------------------
    }
    // ���� ४����� � ⠡���
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
//    logWARN("%s:: ����室��� ��ࠢ��� NVR ࠧ��஬ %u ����!", __func__, SIZE);

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
//            logERR("%s:: �訡��! ��室 �� 横�� ��ࠢ�� ����⮢ NVR � �㭨��!", __func__);
//            return false;
//        }
//        offset += size;
//    }

//    logWARN("%s:: ��ࠢ�� NVR �ᯥ譮 �믮�����!", __func__);
//    return true;
//}
//=====================================================================
// ������ �� � ��� ���㬥�� �� �� �� ������
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
    logDBG("%s, ����祭�� STLV ���㬥��", __PRETTY_FUNCTION__);
    if( !isConnected )
    {
        logERR("%s, �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    //--------------------------------
    // 1 ��⠥� �ਧ���� C��� ��������
    int typeFd{0};
    int res = libGetDocSTLV( numberOfFD, lenTotal, typeFd );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("%s, ����� ����� STLV ��������  = %d. ����� �� = %d. ��� �⢥� = %d", __PRETTY_FUNCTION__, lenTotal, numberOfFD, res);
    if( !ok )
    {
        return false;
    }

    //--------------------------------
    // 2 ��⠥� ᠬ� ��� �� 0�48 - ����� �� ��
    // ���� ����� ����
    while (libGetRegistrTLV(tlv, lenOfTLV) == 0)
    {
        if (!Convert::convertASCIImassToHex(tlv, lenOfTLV))
        {
            return false;
        }
        //--------------------------------
        // ���ᨬ ��� � ����� ४����⮢(+1 �����)
        uint16_t offs(0);
        if (!tlv_parse((unsigned char*)tlv, &reqList, offs))
        {
            logERR( "%s:: ��室, �訡�� �� �६� ���ᨭ�� ���!", __func__ );
            return false;
        }
        //--------------------------------
    }

    return true;
}

//=====================================================================
// ������ STLV ���㬥�� �� �� �� ������
//=====================================================================
bool BIOS::RUS::getDocSTLV( const uint32_t&   numberOfFD,
                            vector <uint8_t>&       stlv,
                            int&                  typeFd)
{
    logINFO("%s:: ����祭�� TLV �������� ���㬥�� �� ��( #%u )...", __func__, numberOfFD);
    stlv.clear();
#ifdef BIOS_RB
    logERR("%s:: ��室! ��⮤ ⮫쪮 ��� ��!", __func__);
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
    logINFO("%s, ����祭�� STLV ���㬥��", __PRETTY_FUNCTION__);
    if( !isConnected )
    {
        logERR("%s, �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    //--------------------------------
    // 1 ��⠥� �ਧ���� C��� ��������
    int res = libGetDocSTLV( numberOfFD, lenTotal, typeFd );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logINFO("%s, ����� ����� STLV ��������  = %d. ����� �� = %d. ��� �⢥� = %d", __PRETTY_FUNCTION__, lenTotal, numberOfFD, res);
    if( !ok )
    {
        logERR("%s:: �訡��! �� 㤠���� ���㧨�� �ਧ���� ���� �������� ���㬥��(�� = %d)!", __func__, numberOfFD);
        return false;
    }

    //--------------------------------
    // 2 ��⠥� ᠬ� ��� �� 0�48 - ����� �� ��
    // ���� ����� ����
    while (libGetRegistrTLV(tlv, lenOfTLV) == 0)
    {
        if (!Convert::convertASCIImassToHex(tlv, lenOfTLV))
        {
            return false;
        }
        //--------------------------------
        // ���ᨬ ��� � ����� ४����⮢(+1 �����)
        std::vector <uint8_t> newTLV = {tlv, tlv + lenOfTLV};
        stlv.insert( stlv.end(), newTLV.begin(), newTLV.end() );
        lenOfTLV = 0;
    }

    return true;
}

//=====================================================================
// ��� � ����� ४����⮢ ��� �������� �� ���� �� ��
//=====================================================================
bool BIOS::RUS::stlv_parse ( unsigned char *stlv,
                               vector <Requisites::Requisite> *requisites )
{
    const int     TAG_OFFSET   = 0,
                  LEN_OFFSET   = 2,
                  VALUE_OFFSET = 4;
    // ����� STLV ��������
    logDBG( "STLV PARSE ..." );

    uint32_t                   offset; // ���饭��
    uint16_t                      tag, // ��
            len; // ������

    Requisites::Requisite   requisite; // ��������

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

    // ���ᨭ� ��
    offset = TAG_OFFSET;
    memcpy( &tag, &stlv[offset], sizeof(tag) );
    logDBG( "STLV TAG = %u", tag );
    // ���� ४����� � ⠡���
    if (REQ.req_search( tag, &requisite ) == EXIT_FAILURE)
    {
        // �������� �� ������
        return EXIT_FAILURE;
    }
    // ���ᨭ� ������
    offset = LEN_OFFSET;
    memcpy( &len, &stlv[offset], sizeof(len) );
    logDBG( "TLV LEN = %u", len );
    offset = VALUE_OFFSET;

    // ���� �� ⨯ ४�����
    if (requisite.type != Requisites::STLV)
    {
        // ������ ⨯ ४�����
        logERR( "WRONG REQUISITE TYPE!" );
        return EXIT_FAILURE;
    }

    std::vector<Requisites::Requisite> tlvReq;

    while( offset <= len  )
    {
        // ���ᨭ� ������ (TLV)
        uint16_t offs = 0;
        if (!tlv_parse(&stlv[offset], &tlvReq, offs))
        {
            logERR( "%s:: ��室, �訡�� �� �६� ���ᨭ�� ���!", __func__ );
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
// ��� � ����� ४����⮢ ��� �������� �� ���� �� ��
//=====================================================================
bool BIOS::RUS::tlv_parse  ( unsigned char  *tlv,
                             vector <Requisites::Requisite> *requisites, uint16_t &retOffset )
{
    const int     TAG_OFFSET   = 0,
                  LEN_OFFSET   = 2,
                  VALUE_OFFSET = 4;

    // ����� TLV ��������
    logDBG( "TLV PARSE ..." );

    uint32_t                        offset; // ���饭��
    uint16_t                           tag, // ��
                                       len; // �����
    const uint16_t          FPD_TAG = 1077;

    Requisites::Requisite        requisite; // ��������

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
    // ���ᨭ� ��
    offset = TAG_OFFSET;
    memcpy( &tag, &tlv[offset], sizeof(tag) );
    logDBG( "TLV TAG = %u", tag );

    // ���� ४����� � ⠡���
    if (REQ.req_search( tag, &requisite ) == EXIT_FAILURE)
    {
        // �������� �� ������
        logERR( "REQUISITE MISSED! %ud", tag );
        return false;
    }

    // ���ᨭ� ������
    offset = LEN_OFFSET;
    memcpy( &len, &tlv[offset], sizeof(len) );
    logDBG( "TLV LEN = %u", len );
    offset = VALUE_OFFSET;

    // ���ᨭ� ������
    // ���������� ४�����
    requisite.cur_len = len;

    switch (requisite.type)
    {
        case Requisites::ASCII:
            //���࠭���� ��ப�
            logDBG( "PARSING INTO ASCII ... " );
            requisite.value_str = new unsigned char[requisite.cur_len];
            memset( requisite.value_str,            0, requisite.cur_len );
            memcpy( requisite.value_str, &tlv[offset], requisite.cur_len );
            break;

        case  Requisites::INT_32LE:
        case Requisites::UNIX_TIME:
            //���࠭���� �᫠ int32 (4 ����)
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
            //���࠭���� �᫠ int16 (2 ����)
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
            // TLV �� ����� ᮤ�ঠ�� STLV
            logDBG( "STLV REQUISITE FOUNDED");
            stlv_parse( tlv, requisites );
            break;

        default:
            logERR( "UNKNOWN REQUISITE TYPE! REQUISITE MISSED!" );
            retOffset = requisite.cur_len;
            return true;
    }

    // ���࠭���� ४����� � ����
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
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libGetLastRegData(fd, fpd);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if(ok)
    {
        logDBG("����� ���㦥��! ");
    }
    else
    {
        logERR("����� �� ���㦥��! ��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());
        fd = 0;
        fpd = 0;
    }
    return ok;
}

//=====================================================================
// �����⨥ ᬥ�� � ���� + Z �����
//=====================================================================
bool BIOS::RUS::closeShift(string cashier)
{
    lock_guard<mutex> locker(*mutex_rf);

    logDBG("�����⨥ ᬥ��... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    if (isDocOpen_get())
    {
        // �����⨥ ࠭�� ����⮣� ���㬥�� �� ��� ����稨
        cancelDoc();
    }

    int res = libPrintZReport(cashier.c_str(), 0);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if(ok)
    {
        logDBG("����� ������!��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());
    }
    else
    {
        logERR("����� �� ������!��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());
    }
    return ok;
}
//=====================================================================
bool BIOS::BEL::closeShift( string cashier )
{
    lock_guard<mutex> locker(*mutex_rb);

    logDBG("�����⨥ ᬥ��... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }
    int res = libPrintZReport_By(cashier.c_str());
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("����� %s ������!��� �⢥� = %d. ����饭�� ='%s'","�� ", res, strFromErrCode(res).c_str());
    return ok;
}

//=====================================================================
// ���㫨஢���� ���㬥�� ��
//=====================================================================
bool BIOS::BEL::cancelDocBel(const uint32_t& docNum, string cashier, double (&payType)[BIOS::MAX_PAY_TYPES])
{
    lock_guard<mutex> locker(*mutex_rb);

    logDBG("���㫨஢���� ���㬥��... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libCancelDocument_By( docNum, cashier, payType );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("���㬥�� %s ���㫨஢��! ��� �⢥� = %d. ����饭�� ='%s'", (ok ? "" : "��"), res, strFromErrCode(res).c_str());

    return ok;
}
//=====================================================================
// ������� ����� ⥪�饩 ᬥ��
//=====================================================================
bool BIOS::BEL::curShiftNum(uint64_t &shiftNum)
{
    lock_guard<mutex> locker(*mutex_rb);

    logDBG("����� ����� ⥪�饩 ᬥ��... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libGetCounters_CurShiftNum(shiftNum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("����� ᬥ�� %s ����祭! ��� �⢥� = %d. ����饭�� ='%s'", (ok ? "" : "��"), res, strFromErrCode(res).c_str());

    return ok;
}

//=====================================================================
// �������� ������ � ������ ���㬥��
//=====================================================================
bool BIOS::RUS::addPositionToCheck( Position *pos )
{
    logDBG("���������� ����樨...");
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
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
    logDBG("���������� ����樨 %s �����襭�!��� �⢥� = %d. ����饭�� ='%s'", (ok ? "" :"��"), res, strFromErrCode(res).c_str());
    return ok;
}


//=====================================================================
// ����⮣  �����稢��� ���� ⮢�஢ � 祪�. ����� �� ����� �ᯥ���� �������⥫�� ४������ � �.�.
//=====================================================================
bool BIOS::RUS::subTotal()
{
    logDBG("����⮣ 祪�...");
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libSubTotal();
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("����⮣ 祪� %s �����襭!��� �⢥� = %d. ����饭�� ='%s'","�� ", res, strFromErrCode(res).c_str());
    return ok;
}


//=====================================================================
// �������� ᪨��� � �㡫�� �� 祪
//=====================================================================
bool BIOS::RUS::addDiscountRubles(string nameDiscount, uint64_t sum )
{
    logDBG("���������� ᪨��� �� 祪... ");

    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }
    if( sum < 0 )
    {
        logERR("%s  �訡��! ����ୠ� �㬬� = %d", __PRETTY_FUNCTION__, sum);
        return false;
    }
    if( nameDiscount.empty() )
    {
        logERR("%s  �訡��! ��������� �������� ᪨���!", __PRETTY_FUNCTION__);
        return false;
    }


    int res = 0;//libAddDiscount    (Positions::DISCOUNT_TYPE_ABSOLUTE, nameDiscount.c_str(), sum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("������ %s ���������! ��� �⢥� = %d. ����饭�� ='%s'", ok ?  "" : "�� ", res, strFromErrCode(res).c_str());
    return ok;
}


//=====================================================================
// �������� ��業�� � �㡫�� �� 祪
//=====================================================================
bool BIOS::RUS::addMarginRubles(string nameMargin, int sum )
{
    logDBG("���������� ��業�� �� 祪... ");
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }
    if( sum < 0 )
    {
        logERR("%s  �訡��! ����ୠ� �㬬� = %d", __PRETTY_FUNCTION__, sum);
        return false;
    }
    if( nameMargin.empty() )
    {
        logERR("%s  �訡��! ��������� �������� ��業��!", __PRETTY_FUNCTION__);
        return false;
    }

    // const int MARGIN_PERSENTS = 0;  ��� �� ���
    const int MARGIN_RUBLES   = 1;

    int res = libAddMargin    (MARGIN_RUBLES, nameMargin.c_str(), sum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("��業�� %s ���������! ��� �⢥� = %d. ����饭�� ='%s'", ok ?  "" : "�� ", res, strFromErrCode(res).c_str());
    return ok;
}


//=====================================================================
// �������� ������ � �������᪨� 祪
//=====================================================================
bool BIOS::BEL::addPositionToCheck( POSITION *pos )
{
    logDBG("���������� ����樨...");
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
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

    logDBG("���������� ����樨 %s �����襭�!��� �⢥� = %d. ����饭�� ='%s'", ok ?  "" : "�� ", res, strFromErrCode(res).c_str());
    return ok;
}


//=====================================================================
// ����⮣  �����稢��� ���� ⮢�஢ � 祪�. ����� ����� ��⠭����� ᪨���/��業�� �� ���� 祪
//=====================================================================
bool BIOS::BEL::subTotal()
{
    logDBG("����⮣ 祪�...");
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    int res = libSubTotal();
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("����⮣ 祪� %s �����襭!��� �⢥� = %d. ����饭�� ='%s'", ok ?  "" : "�� ", res, strFromErrCode(res).c_str());
    return ok;
}


////=====================================================================
//// �������� ᪨��� �� ������(��᫥ ��� ����樨0x42 ) ��� 祪(��᫥ ����⮣0x44 ) � ��
////=====================================================================
//bool BIOS::BEL::addDiscountRubles(string nameDiscount, uint64_t sum, Positions::DISCOUNT_TYPE_BIOS       discType )
//{
//    logDBG("���������� ᪨��� �� ������/祪... ");
//    if( !isConnected )
//    {
//        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
//        return false;
//    }

//    if( nameDiscount.empty() )
//    {
//        logERR("%s  �訡��! ��������� �������� ᪨���!", __PRETTY_FUNCTION__);
//        return false;
//    }

//    int res = libAddDiscount    ((unsigned char)discType, nameDiscount.c_str(), sum );

//    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

//    logDBG("������ %s ���������! ��� �⢥� = %d. ����饭�� ='%s'", ok ?  "" : "�� ", res, strFromErrCode(res).c_str());
//    return ok;
//}

//=====================================================================
// �������� ��業�� �� ������(��᫥ ��� ����樨0x42 ) ��� 祪(��᫥ ����⮣0x44 ) � ��
//=====================================================================
bool BIOS::BEL::addMarginRubles(string nameMargin, uint64_t sum )
{
    logDBG("���������� ��業�� �� ������/祪... ");
    if( !isConnected )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
        return false;
    }

    if( nameMargin.empty() )
    {
        logERR("%s  �訡��! ��������� �������� ��業��!", __PRETTY_FUNCTION__);
        return false;
    }

    // const int MARGIN_PERSENTS = 0;
    const int MARGIN_RUBLES   = 1;

    int res = libAddMargin(MARGIN_RUBLES, nameMargin.c_str(), sum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    logDBG("��業�� %s ���������! ��� �⢥� = %d. ����饭�� ='%s'", ok ?  "" : "�� ", res, strFromErrCode(res).c_str());
    return ok;
}

//=====================================================================
// �������� ������  � ������ ���㬥��
//=====================================================================
bool BIOS::addPaymentToCheck(PAYMENT *payment)
{
    logDBG("���������� ������... ");
    if( !isConnected.load() )
    {
        logERR("%s  �訡��! ���������� �� �ந��樠����஢����!", __PRETTY_FUNCTION__);
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


    logERR("����� %s ���������! ��� �⢥� = %d. ����饭�� ='%s'",ok ?  "" : "�� ", res, strFromErrCode(res).c_str());
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
// ������ ��ࠬ��஢ 祪�
//=====================================================================
int BIOS::setPurchaseMode(const uint8_t &mask)
{
    lock_guard<mutex> locker(mutex_bios);
    logDBG("%s", __PRETTY_FUNCTION__);
    int res(libWriteSettingsTable(PIRIT_TABLE_SETTINGS_PURCHASE, PIRIT_PARAM_0, mask));

    logDBG("��⠭���� ��ࠬ��஢ 祪� �����祭�! ��� �⢥� = %d", res);
    return res;
}
//=====================================================================
// ��⠭����� ��ࠬ���� ���⭮�� ���ன�⢠
//=====================================================================
int BIOS::setPrinterMode(int mask)
{
    lock_guard<mutex> locker(mutex_bios);
    logDBG("��⠭���� ��ࠬ��஢ �ਭ��");

    int res = libWriteSettingsTable( PIRIT_TABLE_SETTINGS_KKT, PIRIT_PARAM_0, mask );

    logDBG("��⠭���� ��ࠬ��஢ �ਭ�� �����祭�! ��� �⢥� = %d", res);
    return res;
}
//=====================================================================
// �⥭�� ��ࠬ��஢ 祪�
//=====================================================================
int BIOS::getPurchaseMode(int &mask)
{
    lock_guard<mutex> locker(mutex_bios);
    logDBG("%s", __PRETTY_FUNCTION__);
    mask = 0;
    int res(getIntFromSettingsTable(mask, PIRIT_TABLE_SETTINGS_PURCHASE, PIRIT_PARAM_0));

    logDBG("%s\t\t����祭�� ��ࠬ��஢ 祪� �����祭�! ��� �⢥� = %d", __PRETTY_FUNCTION__, res);
    return res;
}

//=====================================================================
// �⥭�� ��ࠬ��஢ 祪�
//=====================================================================
bool  BIOS::printDiscount( const double &discountSumRubl, const string &text )
{
    logERR("%s:: ���������� � �� �㬬�୮� ᪨��� �� �ᥬ ������ = %f", __func__, discountSumRubl);

    if( text.empty() )
    {
        logERR("%s:: �訡��! ⥪�� ᪨��� �� �� 㪠���",__func__);
        return false;
    }

    if( discountSumRubl < 0.01 )
    {
        logERR("%s:: �訡��! ��������⭠� �㬬� ᪨��� = '%f'",__func__, discountSumRubl);
        return false;
    }
    const uint8_t SHIFT     =     16,
                  MAX_LEN   =     40;

    char cstr1[MAX_LEN]     = {0x00};

    sprintf (cstr1,"%-*s %*.2f", MAX_LEN -SHIFT , text.c_str(), SHIFT, discountSumRubl);

    int res = libPrintRequisite(DOC_REQ_PRINT_NORMAL, FONT_NORM, cstr1);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    logERR("%s:: ���������� � �� ᪨��� %s�믮�����! ��� �⢥� = %d",__func__, (ok ? "" : " �� "), res);
    return ok;
}
//=====================================================================
// ������� ��ࠬ���� ���⭮�� ���ன�⢠
//=====================================================================
int BIOS::getPrinterMode(int &mask)
{
    lock_guard<mutex> locker(mutex_bios);
    logDBG("����祭�� ��ࠬ��஢ �ਭ��");

    int res = getIntFromSettingsTable( mask, PIRIT_TABLE_SETTINGS_KKT, PIRIT_PARAM_0 );

    logDBG("����祭�� ��ࠬ��஢ �ਭ�� �����祭�! ��� �⢥� = %d", res);
    return res;
}

//=====================================================================
// ������� ��権 ��業���
//=====================================================================
bool BIOS::getLicenseOptionsTags(int &mask)
{
    lock_guard<mutex> locker(mutex_bios);
    logDBG("����祭�� ��権 ��業���");

    int res = getIntFromSettingsTable( mask, PIRIT_TABLE_LIC_OPTIONS, PIRIT_PARAM_0 );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);

    if(!ok)
    {
        mask = 0;
        logERR("�訡�� �� ���㧪� ��権 ��業���! res = %d", res);
    }
    return ok;
}
//=====================================================================
// ����� X �����
//=====================================================================
bool BIOS::printXReport(string name)
{
    lock_guard<mutex> locker(mutex_bios);
    int res = libPrintXReport(name.c_str());
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    return ok;
}
//=====================================================================
// ��ᯥ���� ���㬥�� �� ��. �᫨ ����� ���㬥�� 0, � ���⠥���
// ᯨ᮪ ��� ॣ����権/���ॣ����権
//=====================================================================
bool BIOS::printFnDocs(int numberOfDoc)
{
    lock_guard<mutex> locker(mutex_bios);
    int res = libPrintDocumentFromECT(numberOfDoc);
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if(ok)
    {
        logDBG("����� ॣ����権/���ॣ����権 ���������. ��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());
    }
    else
    {
        logERR("����� ॣ����権/���ॣ����権 �� ���������. ��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());
    }

    return ok;
}
//=====================================================================
// ��� ���㫥��� ����
//=====================================================================
bool BIOS::techZeroing  (string zavNum)
{
    lock_guard<mutex> locker(mutex_bios);
    // �� 0494123477
    if ( zavNum.size() != 14 )
    {
        logERR("techZeroing(): �訡��! ����ୠ� ����� ���. ����� = %d",  zavNum.size() );
        return false;
    }

    PLDate mpDate;
    PLTime mpTime;

    getCurMPTime(&mpDate, &mpTime);

    int res = libAuthorization(mpDate, mpTime, zavNum.c_str());
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logERR("techZeroing() %s ���. ���㫥��� BIOS. ���. ����� = '%s'. ������� = %d", (ok ? " " : " �� ���������!"), zavNum.c_str(), res);
    return ok;
}
//=====================================================================
// ��⠭����� ���� � �६� � ���� ��� �� ���(⥪����)
//=====================================================================
bool BIOS::setDateTimeToBIOS()
{
    lock_guard<mutex> locker(mutex_bios);
    PLDate mpDate;
    PLTime mpTime;

    getCurMPTime(&mpDate, &mpTime);

    int res = libSetPiritDateTime(mpDate, mpTime);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("setDateTimeToBIOS() ��⠭���� � ���� ����� ���� � �६���.��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());

    return ok;
}

//=====================================================================
// ��⠭����� ���� � �६� � ���� ��� �� ���(�ந�������)
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
    logDBG("setDateTimeToBIOS() ��⠭���� � ���� ����� ���� � �६���.��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());

    return ok;
}
//=====================================================================
// ����� ��᫥����� ����� � ॣ����樨/���ॣ����樨
//=====================================================================
void BIOS::printLastFiscalReport()
{
    lock_guard<mutex> locker(mutex_bios);
    int res = libPrintCopyReportFiscalization();
    bool ok [[gnu::unused]] = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    logDBG("printLastFiscalReport(): ����� ��᫥����� ����� � ॣ����樨/���ॣ����樨. ��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());
}
//=====================================================================
// ����� ������
//=====================================================================
void BIOS::printBIOSTechReport()
{
    lock_guard<mutex> locker(mutex_bios);
    int res = libPrintServiceData();
    bool ok [[gnu::unused]] = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
#ifdef BIOS_RB  // ��� �� �������⥫쭮 ���⠥� �ࢨ�� ����� ����
    if(ok)
    {
        res = libPrintServiceDataSKNO_By ();
        ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    }
#endif
    logDBG("printBIOSTechReport(): ����� ���. ����� BIOS. ��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());
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
    // ���㧪� ������ �ࠩ���� 祪�
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
// ॣ����樮��� ����� ��
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
    logDBG("getFnNumber(): ������� ����樨 = %d", res);
    if( !ok )
    {
        logDBG( "GET RF NYMBER ERROR!!!" );
        return false;
    }

    fs_number = Convert::charToString( fnNum, FS_NUMBER_LEN );
    logDBG("getFnNumber(): ����� �� = %s", fs_number.c_str());

    return true;
}
//=====================================================================
// ������ ����� ��᫥����� �᪠�쭮�� ���㬥��
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
    logDBG("getLastFiscDocNumber(): ��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());
    if( !ok )
    {
        logERR( "GET LAST FISC DOC NUMBER ERROR!!!" );
        return false;
    }
    last_fd_num = stoul( fiscalDocNumber, NULL, 10);
    logDBG("getLastFiscDocNumber(): �����  ��᫥����� �᪠�쭮�� ���㬥�� = '%u'", last_fd_num);

    return true;
}
//=====================================================================
// ������� ����� ᬥ��, ����� �� � � 祪� � ᬥ��
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
    logDBG("getShiftStatus(): ��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());
    if( !ok )
    {
        logERR( "GET SHIFT STATUS ERROR!!!" );
        return false;
    }

    logDBG("getShiftStatus(): ����� ⥪ ᬥ��    = %d", shift_num);
    logDBG("getShiftStatus(): ����� �����      = %d", isOpened);
    logDBG("getShiftStatus(): ����� 祪� � ᬥ�� = %d", reciept_num);

    isShiftOpen = (bool)isOpened;

    return true;
}
//=====================================================================
// ������� �� 䫠�� ����ᮢ �� �����
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
    logDBG("getStatusBIOS(): ����祭�� 䫠��� BIOS. ��� �⢥� = %d. ����饭�� ='%s'", res, strFromErrCode(res).c_str());


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
// ����祭�� ����஢�� ���� �訡�� ����.
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
// ��⠭����� ���� � ���� ���
bool BIOS::RUS::setOFDSettings             ( string urlOFD, uint32_t portOFD  )
{
#ifdef BIOS_RB
    return false;
#endif
    lock_guard<mutex> locker(*mutex_rf);

    if(portOFD < 1 || portOFD > 65534)
    {
        logERR( "setOFDSettings()::�訡��!  �����४�� ���� ��� =  %d", portOFD );
        return false;
    }
    if( urlOFD.empty() || urlOFD.length() > 64 )
    {
        logERR( "setOFDSettings()::�訡��! �����४�� ���� ��� = '%s'", urlOFD.c_str() );
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
// ��� ��
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
        logERR("%s\t\t�� ���������!",  __PRETTY_FUNCTION__ );
        return false;
    }
    logERR( "%s\t\t\t���������!  ��� �� = '%d'", __PRETTY_FUNCTION__, fsType );
    return true;
}
//=====================================================================
// ����� ���
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
        logERR("%s\t\t�� ���������!",  __PRETTY_FUNCTION__ );
        return false;
    }
    logDBG( "%s\t\t\t���������!  ����� ��訢�� FN = '%s'", __PRETTY_FUNCTION__, fnVer.c_str() );
    return true;
}


//=====================================================================
// ��楤�� ������� ��
//=====================================================================
bool BIOS::RUS::closeFN (string cashierName, string& dateTime, uint32_t& fd, uint32_t& fpd)
{
    #ifdef BIOS_RB
        return false;
    #endif

    lock_guard<mutex> locker(*mutex_rf);

    if (isDocOpen_get())
    {
        // �����⨥ ࠭�� ����⮣� ���㬥�� �� ��� ����稨
        cancelDoc();
    }

    int res = libCloseFN( cashierName.c_str(), fd, fpd, dateTime);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);
    if( !ok )
    {
        logERR( "closeFN():: \tERROR, �� 㤠���� ������� ��!"  );
        return false;
    }
    logDBG( "closeFN():: \t ******** �� ������� ������! ********"  );
    return true;
}
//=====================================================================
// ��楤�� ���਩���� ������� ��
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
        logERR( "closeFN():: \tERROR, �������� ������� �� �� �������!!!"  );
        return false;
    }
    logDBG( "closeFN():: \t ******** �� ������ ��������! ********"  );
    return true;
}
//=====================================================================
// ���਩��� �����⨥ ᬥ��
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
        logERR( "closeFN():: \tERROR, �������� ������� ����� �� �������!!!"  );
        return false;
    }
    logDBG( "closeFN():: \t ******** ����� ������� ��������! ********"  );

    return true;
}

//=====================================================================
//=====================================================================
//==================    ����稪�    ===================================
//=====================================================================
//=====================================================================

//=====================================================================
// ����稪�. 0x01 ����� ⥪ ᬥ��
//=====================================================================
bool BIOS::getCounters_CurShiftNum     ( uint64_t               &shiftNum )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_CurShiftNum(shiftNum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_CurShiftNum():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x02 ������ ����� ᫥���饣� 祪�
//=====================================================================
bool BIOS::getCounters_NextRecNum     ( uint64_t               &nextRecNum )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_NextRecNum(nextRecNum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_NextRecNum():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x03 ������ �㬬� �த�� �� ⨯�� ���⥦�
//=====================================================================
bool BIOS::getCounters_SaleSumByPay     (OPERATION<double>          &ssbp )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_SaleSumByPay(ssbp);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_SaleSumByPay():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x04 ������ �㬬� �த�� �� ⨯�� ���⥦�
//=====================================================================
bool BIOS::getCounters_SaleCntByPay     (OPERATION<uint64_t>          &scbp )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_SaleCntByPay(scbp);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_SaleCntByPay():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x05 ������ �㬬� �����⮢ �� ⨯�� ���⥦�
//=====================================================================
bool BIOS::getCounters_RetSaleSumByPay     (OPERATION<double>         &rssbp )
{

    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_RetSaleSumByPay(rssbp);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_SaleCntByPay():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x06 ������ ������⢮ ����� �� �����⠬
//=====================================================================
bool BIOS::getCounters_RetSaleCntByPay ( OPERATION<uint64_t>       &rscbp )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_RetSaleCntByPay(rscbp);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_RetSaleCntByPay():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x07 ������⢮ ��ଫ����� 祪��
//=====================================================================
bool BIOS::getCounters_RecCntsByType ( REC_CNTS                &recCnts )
{

    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_RecCntsByType(recCnts);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_RecCntsByType():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x08 �㬬� �� ��ଫ���� 祪��
//=====================================================================
bool BIOS::getCounters_RecSumsByType ( REC_SUMS                &recSums )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_RecSumsByType(recSums);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_RecSumsByType():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x09 �㬬� �� ᪨����
//=====================================================================
bool BIOS::getCounters_DiscountSums ( DISCOUNT_SUMS &discSums )
{

    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_DiscountSums(discSums);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_DiscountSums():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x10 �㬬� �������  �� �த����
//=====================================================================
bool BIOS::getCounters_TaxSumsSale ( TAX_SUMS &taxSums )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_TaxSumsSale(taxSums);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_TaxSumsSale():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x11 �㬬� �������  �� �����⠬
//=====================================================================
bool BIOS::getCounters_TaxSumsRets ( TAX_SUMS  &taxSums )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_TaxSumsRets(taxSums);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_TaxSumsRets():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x12 ����� �� ��᫥����� X ����� ��� ����� � �����⨨ ᬥ��
//=====================================================================
bool BIOS::getCounters_XZ ( X_Z_DATA                 &xzData )
{

    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_XZ(xzData);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_XZ():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x13 ������ �㬬� �� ᥪ��/�⤥��� �� �த����
//=====================================================================
bool BIOS::getCounters_depSaleSum  ( double         (&depSaleSum)[NUM_OF_SECTOINS] )
{
    lock_guard<mutex> locker(mutex_bios);

    int res = libGetCounters_depSaleSum (depSaleSum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_depSaleSum():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x14 ������ �㬬� �� ᥪ��/�⤥��� �� �����⠬
//=====================================================================
bool BIOS::getCounters_depRetSum  ( double         (&depRetSum)[NUM_OF_SECTOINS] )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_depRetSum (depRetSum);

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_depRetSum():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x15  ���-�� 祪�� ��室�� + ���-�� 祪�� �����⮢ ��室�
//=====================================================================
bool BIOS::getCounters_RecCount  ( uint64_t &purchaseCnt, uint64_t &retPurchaseCnt )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_RecCount ( purchaseCnt, retPurchaseCnt );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_RecCount():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x16 ������ �㬬� ���㯮� �� ⨯�� ���⥦�
//=====================================================================
bool BIOS::getCounters_SaleSumByType( OPERATION<double> &purchSums )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_SaleSumByType( purchSums );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_SaleSumByType():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x17 ������ �㬬� ���㯮� �� ⨯�� ���⥦�
//=====================================================================
bool BIOS::getCounters_RetSumByType( OPERATION<double>  &purchSumsRet )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_RetSumByType( purchSumsRet );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_RetSumByType():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ����稪�. 0x18 ������ ����� �� ���४��
//=====================================================================
bool BIOS::getCounters_CorData ( COR_DATA &corData )
{
    lock_guard<mutex> locker( mutex_bios );

    int res = libGetCounters_CorData( corData );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_NOT_FISCAL);
    if( !ok )
    {
        logERR( "getCounters_CorData():: \tERROR, �訡�� ���㧪� ����稪��!"  );
        return false;
    }
    return true;
}

//=====================================================================
// ������� ���� � ���� ���
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
// ��ࠢ����� ���⠬� (��⮢�� ��᪠) ���.  ���������
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
// ��ࠢ����� ���⠬� (��⮢�� ��᪠) ���.  ������
bool BIOS::RUS::setCalcMask ( int mask )
{
#ifdef BIOS_RB
    return false;
#endif
    if( mask < 0 || mask > 255 )
    {
        logERR( "setCalcMask():: ERROR! �ਢ�� ��᪠(0..255) = '%d'", mask );
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
// ����⪠ ����� ������ � ⠡���� ����஥�
bool BIOS::RUS::writeSettingsTableWrapper(unsigned char number, int index, const char *data)
{
    lock_guard<mutex> locker(*mutex_rf);
    int res = libWriteSettingsTable(number, index, data);
    return ( res == 0 ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_CONNECTION) );
}


//=====================================================================
// ������ ������������ �࣠����樨 � ���� ��� ०��� ����(����� �� 祪��)
bool BIOS::RUS::setENVDRegData (const string &orgName, const string &orgAdress )
{
#ifdef BIOS_RB
    return false;
#endif
    int INDEX_NAME = 0;
    int INDEX_ADRR = 2;

    if(orgName.empty() || orgAdress.empty() )
    {
        logERR( "%s:: ERROR! �� ������ ����� �� = '%s', ���� �� = '%s'",
                                                      __func__,
                                                      orgName.c_str(),
                                                      orgAdress.c_str() );
        return false;
    }
    //--------
    // ������������ �࣠����樨, 1-�� ��ப�
    if( !writeSettingsTableWrapper(PIRIT_TABLE_NAME_ADDRESS_ORG, INDEX_NAME, orgName.c_str()) )
    {
        logERR( "%s:: ERROR! �� 㤠���� �������� ����� ������������ �࣠����樨! ", __func__ );
        return false;
    }
    //--------
    //  ���� �࣠����樨, 1-�� ��ப�
    if( !writeSettingsTableWrapper(PIRIT_TABLE_NAME_ADDRESS_ORG, INDEX_ADRR, orgAdress.c_str()) )
    {
        logERR( "%s:: ERROR! �� 㤠���� �������� ����� ���� �࣠����樨! ", __func__ );
        return false;
    }
    //--------
    logDBG( "%s():: \t������������ � ���� �࣠����樨 �ᯥ譮 ��������!", __func__ );
    return true;
}

//=====================================================================
// ��ࢨ筠� ॣ������ ���
//=====================================================================
bool BIOS::RUS::registerKKT_BIOS(registrationStruct &rs)
{
    lock_guard<mutex> locker(*mutex_rf);
    //---------
    // �஢�ન
    //---------
    if( rs.type != ACTIVATION_NEW && rs.type != ACTIVATION_REPLACE  )
    {
        logERR("registerKKT_BIOS():: �訡��! ����୮� ���祭�� ���� type = 0x%02x, �ॡ���� 0x00 ��� 0x01", rs.type);
        return false;
    }
    if( rs.regNumber.length() != FS_NUMBER_LEN )
    {
        logERR("registerKKT_BIOS():: �訡��! ����ୠ� ����� ���� regNumber = %d, �ॡ���� = 16", rs.regNumber.length());
        return false;
    }
    if( rs.INN.empty() )
    {
        logERR("registerKKT_BIOS():: �訡��! ���� INN �� ���������!");
        return false;
    }
    if( rs.systemTax < 0 || rs.systemTax > 63 )
    {
        logERR("registerKKT_BIOS():: �訡��! ����ୠ� ��᪠ ���! systemTax = '%d'", rs.systemTax);
        return false;
    }
    if( rs.rej < 0 || rs.rej > 65535 )
    {
        logERR("registerKKT_BIOS():: �訡��! ����ୠ� ��᪠ ०����! rej = '%d'", rs.rej);
        return false;
    }
    if( rs.cashierName.empty()  || rs.cashierName.length() > CASHIER_NAME_MAX_SIZE )
    {
        logERR("registerKKT_BIOS():: �訡��! ���� cashierName �� ��������� ��� ����ୠ� ����� ��ப�:'%s'", rs.cashierName.c_str());
        return false;
    }
    if( rs.userName1.empty()  || rs.userName1.length() > USER_NAME_MAX_SIZE )
    {
        logERR("registerKKT_BIOS():: �訡��! ���� userName1 �� ��������� ��� ����ୠ� ����� ��ப�:'%s'", rs.userName1.c_str());
        return false;
    }
    if( rs.addressSettle1.empty()  || rs.addressSettle1.length() > ADDR_MAX_SIZE )
    {
        logERR("registerKKT_BIOS():: �訡��! ���� addressSettle1 �� ��������� ��� ����ୠ� ����� ��ப�:'%s'", rs.addressSettle1.c_str());
        return false;
    }
    if( rs.placeSettle.empty()  || rs.placeSettle.length() > PLACE_MAX_SIZE )
    {
        logERR("registerKKT_BIOS():: �訡��! ���� placeSettle �� ��������� ��� ����ୠ� ����� ��ப�:'%s'", rs.placeSettle.c_str());
        return false;
    }
    if( rs.OFDName.size() > 64 )
    {
        logERR("registerKKT_BIOS():: �訡��! ���� OFDName �� ��������� ��� ����ୠ� ����� ��ப�:'%s'", rs.OFDName.c_str());
        return false;
    }
    if( rs.OFD_INN.empty()  || rs.OFD_INN.length() > OFD_INN_MAX_SIZE )
    {
        logERR("registerKKT_BIOS():: �訡��! ���� OFD_INN �� ��������� ��� ����ୠ� ����� ��ப�:'%s'", rs.OFD_INN.c_str());
        return false;
    }
    if( rs.senderEMAIL.empty() || rs.senderEMAIL.length() > SENDER_EMAIL_MAX_SIZE)
    {
        logERR("registerKKT_BIOS():: �訡��! ���� senderEMAIL �� ��������� ��� ����ୠ� ����� ��ப�:'%s'", rs.senderEMAIL.c_str());
        return false;
    }
    if( rs.FNS_Site.empty() || rs.FNS_Site.length() > FNS_SITE_MAX_SIZE)
    {
        logERR("registerKKT_BIOS():: �訡��! ���� FNS_Site �� ��������� ��� ����ୠ� ����� ��ப�:'%s'", rs.FNS_Site.c_str());
        return false;
    }

    // ��ନ஢���� ���� � �६��� ॣ����樨

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
        // �����⨥ ࠭�� ����⮣� ���㬥�� �� ��� ����稨
        cancelDoc();
    }

    // �᫨ ��� �� - ॣ�����㥬��
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
    logWARN("============ ������� ॣ����樨 = %d. ����饭�� ='%s'", resultReg, strFromErrCode(resultReg).c_str());
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
//                      ����� 䫠��� ���
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
        logERR( "kca_flgs_get():: �訡��! १���� ������� libGetStatusFlags_By()  = %d ", res );
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

    docOpenedType =       statusDoc &   0x0F;          // ���� ���� 0,1,2,3
    docOpenedState =      statusDoc >> 4;       // ��᫥���騥 ���� 4,5,6,7

    if(docOpenedState == 0)
    {
        isDocOpened = false;
    }
    else
    {
        isDocOpened = true;
    }
    //---------------------------- ��� ⥪�饣� ����⮣� ���㬥��
    if( docOpenedType > KCA_DOC_TYPE::KCA_TYPE_RECIEPT_INVOCIE )
    {
        logERR("�訡��! �����४�� docOpenedType = %d", docOpenedType);
        return false;
    }
    doc_type  =  (KCA_DOC_TYPE)   docOpenedType;

    //---------------------------- ����ﭨ� ���㬥��
    if( (docOpenedState < 0) || (docOpenedState > KCA_DOC_STATE::KCA_STATE_NEED_CLOSE) )
    {
        logERR("�訡��! �����४�� docOpenedState = %d", docOpenedState);
        return false;
    }
    doc_state =  (KCA_DOC_STATE) docOpenedState;

    //---------------------------- ����� 䫠��� ࠡ��� � ����
    if( (statusSKNO < 0) || (statusSKNO > KCA_SCNO_STATE::KCA_SCNO_Z_REP_ERR) )
    {
        logERR("�訡��! �����४�� statusSKNO = %d", statusSKNO);
        return false;
    }
    scno_state = (KCA_SCNO_STATE)   statusSKNO;




    logDBG("kca_flgs_get():: \n\t\t�������:"
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

    logDBG("\t\t���������:"
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
    logDBG("\t\t��������:"
                   "\n\tdoc_type   = %d "
                   "\n\tdoc_state  = %d ",
                                                   doc_type,
                                                   doc_state );

    logDBG("\t\t����: \n\tscno_state   = %d " , scno_state );

    return true;
}


//=====================================================================
// ����⨥ ᬥ�� � ����
//=====================================================================
bool BIOS::BEL::openShift()
{
    lock_guard<mutex> locker(*mutex_rb);
    logDBG("����⨥ ᬥ��.... isConnected = %01x", isConnected);
    if( !isConnected )
    {
        logERR("BIOS::openShift()  �訡��! ���������� �� �ந��樠����஢����!");
        return false;
    }
    int res = libOpenShift_By();
    logDBG("����� %s�����! ��� �⢥� = %d",(res == 0 ? " " : "�� "), res);
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
// ����� ��� �� BIOS
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

    // ����塞 ���ᨢ�
    memset(name, 0 ,40 );
    memset(val,  0 ,20 );

    res1 = libGetSettingsKSA_By(40, i, name);   // ��⠥� ����� �����
    res2 = libGetSettingsKSA_By(41, i,  val);   // ��⠥� ᠬ� �⠢��

    // ����㫨�㥬 �訡��
    res1 |= res2;

    if( res1 !=0 )
    {
        logERR("ndss_read():: �訡�� �⥭�� �����! :-(");
        return false;
    }

    nds.name  = string(name);
    nds.value = QString(val).toFloat();

    logDBG("\ti = %d, \tname = '%s', \tval = %lf", i, nds.name.c_str(), nds.value);
    logDBG("ndss_read():: �⥭�� ��� ��諮 �ᯥ譮");

    return true;
}
//=====================================================================
int BIOS::BEL::getNDSTableSize()
{
    const uint8_t SPACE = 0x20;
    return count_if(begin(NDSS), end(NDSS), [&](const NDS &nds){return (!(Convert::end_symbols_delete(nds.name, SPACE).empty()));});
}
//=====================================================================
// ����� ���� �� BIOS
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
// ������� ��� � BIOS 0...5
//=====================================================================
bool BIOS::BEL::nds_write(int i, NDS nds)
{
    lock_guard<mutex> locker(*mutex_rb);

    int maxSize = NDSS.size() - 1;
    if(i < 0 || i >  maxSize)
    {
        logERR("ndss_write:: �訡��! ������ ������ ����! i = %d", i);
        return false;
    }

    int res1 = libSetSettingsKSA_By( 40, i,  (char*)nds.name.c_str() );
    int res2 = libSetSettingsKSA_By( 41, i,  (char*)to_string(nds.value).c_str() );

    if( (res1 | res2) != 0 )
    {
        logERR("ndss_write():: �訡�� ����� ���. i = %d, name = %s, val = %s",
                                                                               i,
                                                         nds.name.c_str(),
                                             to_string(nds.value).c_str());
        return false;
    }
    logDBG("ndss_write():: ������ ���� ��諠 �ᯥ譮");

    return true;
}
//=====================================================================
// ������� ��� � BIOS 0...5
//=====================================================================
bool BIOS::BEL::ndss_write(int i)
{
    lock_guard<mutex> locker(*mutex_rb);

    int maxSize = NDSS.size()-1;
    if(i < 0 || i >  maxSize)
    {
        logERR("ndss_write:: �訡��! ������ ������ ����! i = %d", i);
        return false;
    }

    int res1 = libSetSettingsKSA_By( 40, i,  (char*)NDSS.at(i).name.c_str() );
    int res2 = libSetSettingsKSA_By( 41, i,  (char*)to_string(NDSS.at(i).value).c_str() );

    if( (res1 | res2) != 0 )
    {
        logERR("ndss_write():: �訡�� ����� ���. i = %d, name = %s, val = %s",
                                                                               i,
                                                         NDSS.at(i).name.c_str(),
                                             to_string(NDSS.at(i).value).c_str());
        return false;
    }
    logDBG("ndss_write():: ������ ���� ��諠 �ᯥ譮");
    return true;
}

//=====================================================================
//                  ������ ⨯� ���⥦� � BIOS
bool BIOS::BEL::writePayType( const int num, const  string &name )
{
    const int GET_PAY_TYPE = 32;
    lock_guard<mutex> locker(*mutex_rb);

    // ����� ����� ��९����
    if(num <  0                                             ||
       num >  (BIOS::MAX_PAY_TYPES - 1)                     ||
       num ==  PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASH      ||
       num ==  PIRIT_RECEIPT_INDEX::PIRIT_RECEIPT_CASHLESS   )
    {
        logERR("writePayType():: �訡��! ������ ������ ���� ���⥦�! num = %d", num);
        return false;
    }

    if( name.length() > MAX_PAY_TYPE_NAME_LEN )
    {
        logERR("writePayType():: �訡��! ����� ����� ���� ���⥦� > %d ! \n\t name = '%s' \n\t len = '%d'",
               MAX_PAY_TYPE_NAME_LEN,
               name.c_str(),
               name.length() );
        return false;
    }

    int res = libSetSettingsKSA_By( GET_PAY_TYPE, num,  (char*)name.c_str() );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if( !ok )
    {
        logERR("writePayType():: �訡�� ����� ���� ���⥦�. \n\tnum = %d, \n\tname = %s,",
               num,
               name.c_str() );
        return false;
    }
    logDBG("writePayType():: ������ ���� ���⥦� ��諠 �ᯥ譮");

    return true;
}


//=====================================================================
//                  �⥭�� ⨯� ���⥦� �� BIOS
bool BIOS::BEL::readPayType( const int num, string &name )
{
    const int GET_PAY_TYPE = 32;

    lock_guard<mutex> locker(*mutex_rb);

    if( num <  0 ||
        num >  (MAX_PAY_TYPES - 1) )
    {
        logERR("readPayType():: �訡��! ������ ������ ���� ���⥦�! num = %d", num);
        return false;
    }

    char tmpMass[ MAX_PAY_TYPE_NAME_LEN + 1 ];
    memset(tmpMass, 0x00, MAX_PAY_TYPE_NAME_LEN + 1 );

    int res = libGetSettingsKSA_By( GET_PAY_TYPE, num, tmpMass );
    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if( !ok )
    {
        logERR("readPayType():: �訡�� �⥭�� ���� ���⥦� � %d ", num );
        return false;
    }

    // �� ��直� ��砩
    if( strlen(tmpMass) > MAX_PAY_TYPE_NAME_LEN )
    {
        logERR("readPayType():: ����! ��� ���� ���⥦� � %d ⮫쪮 �� ���ࠫ� � ������ �� %d ����",
               num,
               ( strlen(tmpMass) - MAX_PAY_TYPE_NAME_LEN) );
        return false;
    }

    name = string(tmpMass); // ���࠭���� � �����, �᫨ 㤠筮
    logDBG("readPayType()::    ������ ���� ���⥦� � %d ��諠 �ᯥ譮", num );

    return true;
}

//=====================================================================
//                  �⥭�� �⤥��� �� BIOS
bool BIOS::BEL::readPayTypes()
{
    logDBG( "readPayTypes():: �⥭�� ��� ⨯�� ���⥦�� ..." );

    PAY_TYPE pt;

    payTypes.clear();

    for( uint8_t  num = 0; num < MAX_PAY_TYPES; num++ )
    {
        logDBG( "readPayTypes():: �⥭�� ⨯� � %d ...", num );
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
            logERR( "readPayTypes():: ������! �� 㤠���� ����� ⨯ ���⥦� � ", num );
            return false;
        }
        if( !pt.name.empty() )
        {
            logERR( " \t������ = %d \t ��� ������ = '%s'", pt.num, pt.name.c_str() );
        }
    }

    logDBG( "readPayTypes():: �� ���⥦� �ᯥ譮 ��⠭�" );
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
//                  ������ ����� �⤥�� � BIOS
//=====================================================================
bool BIOS::BEL::dep_name_write( string name, int num )
{
    lock_guard<mutex> locker(*mutex_rb);

    if(num < 1 || num >  16)
    {
        logERR("dep_name_write:: �訡��! ������ ������ ������������ �⤥��! num = %d", num);
        return false;
    }

    int res = libSetSettingsKSA_By( 50, num,  (char*)name.c_str() );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if( !ok )
    {
        logERR("dep_name_write():: �訡�� ����� ������������ �⤥��. num = %d, name = %s,",
                                                                                        num,
                                                                                name.c_str() );
        return false;
    }
    logDBG("dep_name_write():: ������ ������������ �⤥�� ��諠 �ᯥ譮");

    return true;
}
//=====================================================================
//                  �⥭�� ����� �⤥�� �� BIOS  �� 㬮�砭�� 1-�� �� 16
//=====================================================================
bool BIOS::BEL::dep_name_read( int num, string &dep_name )
{
    lock_guard<mutex> locker(*mutex_rb);

    if(num < 1 || num >  16)
    {
        logERR("dep_name_read:: �訡��! ������ ������ ������������ �⤥��! num = %d", num);
        return false;
    }

    char name[100];
    memset(name, 0x00,100);

    int res = libGetSettingsKSA_By( 50, num, name );

    bool ok = (res == 0) ?  true :  isCommandComplete(res, FB_OPERATION_CRITICAL_LEVEL::OP_FISCAL);

    if( !ok )
    {
        logERR("dep_name_read():: �訡�� �⥭�� ������������ %d �⤥��.", num );
        return false;
    }
    dep_name = string(name); // ���࠭���� � �����, �᫨ 㤠筮
    logDBG("dep_name_read()::    ������ ������������ %d �⤥�� ��諠 �ᯥ譮", num );

    return true;
}
//=====================================================================
//                  �⥭�� �⤥��� �� BIOS
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
                dep.name = "�⤥� " + to_string( dep_num );
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
