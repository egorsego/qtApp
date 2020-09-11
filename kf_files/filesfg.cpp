/*
 * filesfg.cpp
 */

#include "filesfg.h"
#include <string>

int FilesFG::save_in_file( string filename, char *data, size_t size, bool sync )
{
    logDBG("SAVE IN FILE");
    FILE *fp;
    if ((fp = fopen( filename.c_str(), "wb" )) == NULL)
    {
        logINFO( "ERROR OPENING FILE!\n" );
        return -1;
    }
    if (fwrite( data, 1, size, fp ) != size)
    {
        logERR( "ERROR WRITE TO FILE!" );
        fclose( fp );
        return -2;
    }
    fclose( fp );
    logDBG("SAVE IN FILE COMPLETE");
    if(sync)
    {
        string tmp_str = "sync ";
        tmp_str += filename;
//        system( tmp_str.c_str() );
//        system( tmp_str.c_str() );
    }
    return 0;
}

int FilesFG::save_in_file( string filename, const uint8_t *data, size_t size, bool sync )
{
    logINFO( "SAVE IN FILE\n" );
    FILE *fp;
    if ((fp = fopen( filename.c_str(), "wb" )) == NULL)
    {
        logINFO( "ERROR OPENING FILE!\n" );
        return -1;
    }
    if (fwrite( data, 1, size, fp ) != size)
    {
        logINFO( "ERROR WRITE TO FILE!\n" );
        fclose( fp );
        return -2;
    }
    fclose( fp );
    logINFO( "SAVE IN FILE COMPLETE\n" );
    if(sync)
    {
        string tmp_str = "sync ";
        tmp_str += filename;
//        system( tmp_str.c_str() );
//        system( tmp_str.c_str() );
    }
    return 0;
}

int FilesFG::save_in_file( string filename, string strSource )
{
    logINFO( "SAVE IN FILE\n" );
    size_t SIZE = strSource.size();
    FILE *fp;
    if ((fp = fopen( filename.c_str(), "wb" )) == NULL)
    {
        logINFO( "ERROR OPENING FILE!\n" );
        return -1;
    }

    if (fwrite( strSource.c_str(), 1, SIZE, fp ) != SIZE)
    {
        logINFO( "ERROR WRITE TO FILE!\n" );
        fclose( fp );
        return -2;
    }

    fclose( fp );
    logINFO( "SAVE IN FILE COMPLETE\n" );
    string tmp_str = "sync ";
    tmp_str += filename;
//    system( tmp_str.c_str() );
//    system( tmp_str.c_str() );
    return 0;
}

void FilesFG::print_array_of_char( char *mass, unsigned int size )
{
  //  if(getLogLevel() >= LOG_INFO)
    {
	    for (unsigned int i = 0; i < size; i++)
	    {
            logDBG( "%c", mass[i] );
	    }
        logDBG( "\n" );
    }
}

int FilesFG::cfg_data_copy( Config *dest, Config *source, DYNAMIC_MASK &mask )
{
    logDBG( "CONFIG DATA COPY" );

    if ((source == NULL) || (dest ==NULL))
    {
        logERR( "NULL SOURCE OR DEST POINTER!" );
        return -1;
    }

    if (mask && CONFIG_VER)
        dest->config_version = source->config_version;

    if (mask && KKT_MODE)
        dest->kkt_mode = source->kkt_mode;

    if (mask && TERMINAL_MODE)
        dest->terminal_mode = source->terminal_mode;

    if (mask && FS_REPLACE_MODE)
        dest->fs_replace_mode = source->fs_replace_mode;

    if (mask && SHIFT_TIMER)
        dest->shift_timer = source->shift_timer;

    if (mask && FISGO_VERSION)
        dest->fisgo_version = source->fisgo_version;

    if (mask && KKT_REG_VERSION)
        dest->kkt_reg_version = source->kkt_reg_version;

    if (mask && ARTICLE)
        dest->article = source->article;

    if (mask && UUID)
        dest->uuid = source->uuid;

    if (mask && ETH_UP)
        dest->eth_up = source->eth_up;

    if (mask && IP_ADDRESS)
        dest->ip_adress = source->ip_adress;

    if (mask && MASK)
        dest->mask = source->mask;

    if (mask && ROUTER)
        dest->router = source->router;

    if (mask && WIFI_UP)
        dest->wifi_up = source->wifi_up;

    if (mask && NET_WIFI_NAME)
        dest->net_wifi_name = source->net_wifi_name;

    if (mask && NET_WIFI_KEY)
        dest->net_wifi_key = source->net_wifi_key;

    if (mask && NET_WIFI_SIGNAL)
        dest->net_wifi_signal = source->net_wifi_signal;

    if (mask && NET_PASS)
        dest->net_pass = source->net_pass;

    if (mask && FS_NUMBER)
        dest->fs_number = source->fs_number;

    if (mask && FS_NUMBER_COUNT)
        dest->fs_number_count = source->fs_number_count;

    if (mask && FS_NUMBERS_TABLE)
        dest->fs_numbers_table = source->fs_numbers_table;

    if (mask && CASHIER_INN)
        dest->cashier_inn = source->cashier_inn;

    if (mask && ORGANIZATION_NAME)
        dest->organization_name = source->organization_name;

    if (mask && CALCULATION_ADDRESS)
        dest->calculation_address = source->calculation_address;

    if (mask && CALCULATION_PLACE)
        dest->calculation_place = source->calculation_place;

    if (mask && ORGANIZATION_INN)
        dest->organization_inn = source->organization_inn;

    if (mask && ORGANIZATION_KPP)
        dest->organization_kpp = source->organization_kpp;

    if (mask && KKT_REG_NUM)
        dest->kkt_reg_num = source->kkt_reg_num;

    if (mask && KKT_PLANT_NUM)
        dest->kkt_plant_num = source->kkt_plant_num;

    if (mask && TAX_SYSTEMS)
        dest->tax_systems = source->tax_systems;

    if (mask && CUR_TAX_SYSTEM)
        dest->cur_tax_system = source->cur_tax_system;

    if (mask && ENCRYPTION_SIGN)
        dest->encryption_sign = source->encryption_sign;

    if (mask && EXCISABLE_SIGN)
        dest->excisable_sign = source->excisable_sign;

    if (mask && CLC_SERVICE_SIGN)
        dest->clc_services_sign = source->clc_services_sign;

    if (mask && GAMBLING_SIGN)
        dest->gambling_sign = source->gambling_sign;

    if (mask && LOTTERY_SIGN)
        dest->lottery_sign = source->lottery_sign;

    if (mask && PAYING_AGENT_SIGN)
        dest->paying_agent_sign = source->paying_agent_sign;

    if (mask && OFD_CHOOSE)
        dest->ofd_choose = source->ofd_choose;

    if (mask && OFD_INN)
        dest->ofd_inn = source->ofd_inn;

    if (mask && OFD_SERVER_ADDRESS)
        dest->ofd_server_address = source->ofd_server_address;

    if (mask && OFD_NAME)
        dest->ofd_name = source->ofd_name;

    if (mask && OFD_SERVER_PORT)
        dest->ofd_server_port = source->ofd_server_port;

    if (mask && CHECK_RECEIPT_ADDRESS)
        dest->check_receipt_address = source->check_receipt_address;

    if (mask && OFD_SERVER_IP)
        dest->ofd_server_ip = source->ofd_server_ip;

    if (mask && OPEN_SHIFT_DATE)
        dest->open_shift_date = source->open_shift_date;

    if (mask && AGENT_MASK)
        dest->agent_mask = source->agent_mask;

    if (mask && CURRENT_AGENT)
        dest->current_agent = source->current_agent;

    if (mask && LOG_TYPE)
        dest->logType = source->logType;

    if (mask && IS_CABINET_ENABLE)
        dest->isCabinetEnable = source->isCabinetEnable;

    if (mask && START_TIME)
        dest->startTime = source->startTime;

    if (mask && LAST_MSG_OFD)
        dest->lastMsgOFD = source->lastMsgOFD;

    if (mask && NDS_FREE_PRICE)
        dest->nds_free_price = source->nds_free_price;

    if (mask && SCALE_TYPE)
        dest->scaleConfig.type = source->scaleConfig.type;

    if (mask && SCALE_SPEED)
        dest->scaleConfig.speed = source->scaleConfig.speed;

    if (mask && SCALE_PORT_NAME)
        dest->scaleConfig.portName = source->scaleConfig.portName;

    if (mask && SCALE_OPTIONS)
        memcpy( dest->scaleConfig.options, source->scaleConfig.options, sizeof(source->scaleConfig.options));

    if (mask && SCANNER_TYPE)
        dest->scannerConfig.type = source->scannerConfig.type;

    if (mask && SCANNER_SPEED)
        dest->scannerConfig.speed = source->scannerConfig.speed;

    if (mask && SCANNER_PORT_NAME)
        dest->scannerConfig.portName = source->scannerConfig.portName;

    if (mask && SCANNER_OPTIONS)
        memcpy( dest->scannerConfig.options, source->scannerConfig.options, sizeof(source->scannerConfig.options));

    if (mask && DNS_MAIN)
        dest->dns_main = source->dns_main;

    if (mask && DNS_ADD)
        dest->dns_add = source->dns_add;

    if (mask && ETH_MANUAL)
        dest->eth_manual = source->eth_manual;

    if (mask && AUTO_CASH_OUT)
        dest->is_auto_cash_out = source->is_auto_cash_out;

    if (mask && FFD_KKT_VER)
        dest->ffd_kkt_ver = source->ffd_kkt_ver;

    if (mask && KKT_SIGNS)
        dest->kkt_signs = source->kkt_signs;

    if (mask && ADD_KKT_SIGNS)
        dest->add_kkt_signs = source->add_kkt_signs;

    if (mask && AUTO_PRINTER_NUM)
        dest->automatNumber = source->automatNumber;

    if (mask && AUTO_OPEN_SHIFT)
        dest->isAutoOpenShiftEnable = source->isAutoOpenShiftEnable;

    if (mask && STAGE)
        dest->stage = source->stage;

    if (mask && BSO)
        dest->bso = source->bso;

    if (mask && INTERNET_RECIEPT)
        dest->internet_reciept = source->internet_reciept;

    if (mask && CONNECT_TO)
        dest->connectTo = source->connectTo;

    if (mask && CLOUD_REG_STATUS)
        dest->cloud_reg_status = source->cloud_reg_status;

    if (mask && CLOUD_TEL)
        dest->cloud_tel = source->cloud_tel;

    if (mask && CLOUD_PIN)
        dest->cloud_pin = source->cloud_pin;

    if (mask && CLOUD_ORG_FNS_NUM)
        dest->cloud_org_fns_num = source->cloud_org_fns_num;

    if (mask && CLOUD_KKT_FNS_NUM)
        dest->cloud_kkt_fns_num = source->cloud_kkt_fns_num;

    if (mask && CABINET_REG_EMAIL)
        dest->cabinetRegEmail = source->cabinetRegEmail;

    if (mask && CUR_DEPARTMENT)
        dest->cur_department = source->cur_department;

    if (mask && REC_SENDER_EMAIL)
        dest->recSenderEmail = source->recSenderEmail;

    if (mask && INC_POS)
        dest->inc_pos = source->inc_pos;

    if (mask && SOUND_BUTTON)
        dest->sound_button = source->sound_button;

    if (mask && CURRENT_ROUTE)
        dest->current_route = source->current_route;

    if (mask && CLOUD_GOODS)
        dest->cloud_goods = source->cloud_goods;

    if (mask && IS_2G_UP)
        dest->is2Gup = source->is2Gup;

    if (mask && IMAGE_SHIFT)
        dest->imageShift = source->imageShift;

    if (mask && LAST_FD)
    {
        dest->lastFd = source->lastFd;
    }

    if (mask && TARE_DEFAULT)
    {
        dest->tareDefault = source->tareDefault;
    }

    if (mask && IS_TARE_USE)
    {
        dest->isTareUse = source->isTareUse;
    }

    if (mask && FREE_PRICE_SALE)
    {
        dest->freePriceSale = source->freePriceSale;
    }

    if (mask && FREE_PRICE_SALE_MAX_SUM)
    {
        //dest->freePriceSaleMaxSum = source->freePriceSaleMaxSum;
    }

    if (mask && BT_STATE)
    {
        dest->btState = source->btState;
    }

    if (mask && BT_PAIR)
    {
        dest->btPair = source->btPair;
    }

    if (mask && REVERT_VER)
    {
        dest->revertVer = source->revertVer;
    }

    if (mask && SALE_ALCOHOL)
    {
        dest->saleAlcohol = source->saleAlcohol;
    }

    if (mask && UTM_ADDRESS)
    {
        dest->utmAddress = source->utmAddress;
    }

    if (mask && UTM_PORT)
    {
        dest->utmPort = source->utmPort;
    }

    if (mask && UTM_TIMEOUT)
    {
        dest->utmTimeout = source->utmTimeout;
    }

    if (mask && PRINT_EGAIS)
    {
        dest->printEgais = source->printEgais;
    }

    if (mask && CHECK_ALC_SALE_TIME)
    {
        dest->checkAlcSaleTime = source->checkAlcSaleTime;
    }

    if (mask && ALC_SALE_HR_START)
    {
        dest->alcSaleHrStart = source->alcSaleHrStart;
    }

    if (mask && ALC_SALE_HR_FINISH)
    {
        dest->alcSaleHrFinish = source->alcSaleHrFinish;
    }

    if (mask && ENTER_WEIGHT)
    {
        dest->enterWeight = source->enterWeight;
    }

    if (mask && TOBACCO_COST_FROM)
    {
        dest->tobaccoCostFrom = source->tobaccoCostFrom;
    }

    logDBG( "CONFIG DATA COPY SUCCESS" );

    return 0;
}

FilesFG::FILE_STATUS FilesFG::isFileExist( string file_name )
{
    if( access( file_name.c_str(), F_OK ) != -1 )
    {
        logDBG( "FILE : %s IS EXISTS!", file_name.c_str() );
        return FILE_IS_EXIST;
    }

    logDBG( "FILE : %s IS NOT EXISTS!", file_name.c_str() );
    return FILE_IS_NOT_EXIST;
}

FilesFG::FILE_STATUS FilesFG::fileCreate( string file_name )
{
    logDBG( "FILE : %s CREATE", file_name.c_str() );

    ofstream fout( file_name.c_str() );

    if(fout.is_open())
    {
        fout.close();
        sync();
        logDBG( "SUCCESS" );
        return FILE_CREATE_SUCCESS;
    }

    logERR( "FILE : %s CREATE FAILED!!!", file_name.c_str() );
    return FILE_CREATE_ERR;
}

FilesFG::FILE_STATUS FilesFG::fileCreateIfNotExist( string file_name )
{
    if(isFileExist( file_name ) == FILE_IS_NOT_EXIST)
    {
        return fileCreate( file_name );
    }

    return FILE_IS_EXIST;
}

FilesFG::FILE_STATUS FilesFG::str_to_file( string file_name, string str )
{
    if( isFileExist( file_name ) == FILE_IS_EXIST )
    {
        return FILE_IS_ALREADY_EXIST;
    }
    else
    {
        logDBG( "FILE : %s CREATE", file_name.c_str() );
        ofstream fout( file_name.c_str() );
        if(fout.is_open())
        {
            logDBG( "SAVE IN FILE" );
            fout << str;
            fout.flush();
            fout.close();
            sync();
            sync();
            sync();
            logDBG( "SAVE IN FILE SUCCESS" );
            return FILE_WRITE_SUCCESS;
        }
        else
        {
            logERR( "FILE : %s OPEN ERROR!!!", file_name.c_str() );
            return FILE_OPEN_ERR;
        }
    }

    return SUCCESS;
}

FilesFG::FILE_STATUS FilesFG::strFromFile( string file_name, string &str )
{
    str.clear();

    if( isFileExist( file_name ) == FILE_IS_NOT_EXIST )
    {
        return FILE_IS_NOT_EXIST;
    }

    ifstream fin( file_name.c_str() );

    if(fin.is_open())
    {
        logDBG( "READ FROM FILE" );

        string tmp_str;
        while(getline(fin, tmp_str))
        {
            str += tmp_str;
            tmp_str.clear();
        }
        fin.close();
        sync();
        sync();
        sync();
        logDBG( "READ FROM FILE SUCCESS" );
        return FILE_READ_SUCCESS;
    }
    else
    {
        logERR( "FILE : %s OPEN ERROR!!!", file_name.c_str() );
        return FILE_OPEN_ERR;
    }

    return FAILED;
}

//-----------------------------------------
// Чтение из файла (string)
//-----------------------------------------
template <> FilesFG::FILE_STATUS FilesFG::read<string>( string filename, string &value )
{
    logERR( "READ STRING FROM FILE : %s", filename.c_str() );

    if( isFileExist( filename ) == FILE_IS_NOT_EXIST )
    {
        return FILE_IS_NOT_EXIST;
    }
    //-----------------------------------------
    ifstream fin( filename.c_str() );
    //-----------------------------------------
    value.clear();
    if(fin.is_open())
    {
        logERR( "READ FROM FILE" );

        string tmp_str = "";
        while ( !fin.eof() )
        {
            getline(fin, tmp_str, '\0');
            value += tmp_str;
            tmp_str.clear();
        }

        fin.close();
        sync();
        sync();
        logERR( "READ FROM FILE SUCCESS" );
        return FILE_READ_SUCCESS;
    }
    else
    {
        logERR( "FILE : %s OPEN ERROR!!!", filename.c_str() );
        return FILE_OPEN_ERR;
    }

    return SUCCESS;
}
//-----------------------------------------------------
// Тест на наличие директории
//-----------------------------------------------------
FilesFG::DIR_STATUS FilesFG::isDirExist( string path )
{
    logDBG( "IS DIRECTORY :s EXIST ?", path.c_str() );
    struct stat st;
    if(stat( path.c_str(), &st) != 0)
    {
        // Каталога не существеут
        logERR( "DIRECTORY : %s IS NOT EXIST!", path.c_str() );
        return DIR_IS_NOT_EXIST;
    }

    return DIR_IS_EXIST;
}
//-----------------------------------------------------
// Создание директории
//-----------------------------------------------------
FilesFG::DIR_STATUS FilesFG::dirCreate( string path )
{
    // Создание каталога
    logDBG( "CREATE DIRECTORY : %s", path.c_str() );
    if(mkdir( path.c_str(), DEFFILEMODE ) == 0)
    {
         logDBG( "SUCCESS" );
         return MKDIR_SUCCESS;
    }
    logERR( "CREATE DIRECTORY : %s FAILED!!!", path.c_str() );

    return MKDIR_ERR;
}
//-----------------------------------------------------
// Создание директории в случае отсутствия
//-----------------------------------------------------
FilesFG::DIR_STATUS FilesFG::dirCreateIfNotExist( string path )
{
    if(isDirExist(path) != DIR_IS_EXIST)
    {
        return dirCreate(path);
    }
    return DIR_IS_EXIST;
}

size_t FilesFG::readCurlCallback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t rv = 0;

    if ( (ptr != nullptr) && (stream != nullptr) )
    {
        rv = fread(ptr, size, nmemb, stream);
    }

    return rv;
}

void FilesFG::searchFiles(const FilesFG::PATH &dir, const FilesFG::PATH &filenames, FilesFG::PATHS &files)
{
//    try
//    {
        logINFO("%s: \"%s\"", __FUNCTION__, filenames.c_str());
//        files.clear();
//        boost::filesystem::path p {dir};
//        if (boost::filesystem::exists(p))
//        {
//            if (boost::filesystem::is_directory(p))
//            {
//                for (boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(p))
//                {
//                    const std::string file = x.path().filename().native();
//                    if (file.find(filenames) != std::string::npos)
//                    {
//                        logINFO("\t%s", file.c_str());
//                        files.push_back(file);
//                    }
//                }
//            }
//        }
//    }
//    catch (const boost::filesystem::filesystem_error& ex)
//    {
//        logERR("%s Ops! %s", __FUNCTION__, ex.what());
//    }
//    logINFO("%s found %lu files", __FUNCTION__, files.size());
}
