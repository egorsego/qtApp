//
// Created by Michael on 01.08.17.
//
#include "config.h"

//===============================================================
// Создание резервной копии конфига
void createBackupConfig()
{
    logINFO("createBackupConfig() START");
//    system("rm -r ./Backup");
//    system("mkdir ./Backup");
//    system("cp configDb.db ./Backup/configDb.db");
    sync();
}

//===============================================================
// Проверка есть ли файл резервной копии конфига
bool isBackupConfigExist()
{
    int res{0};// = system("cp ./Backup/configDb.db ./Backup/configDb1.db");
    if(  res != 0 )
    {
        return false;
    }
    //system("rm ./Backup/configDb1.db");
    sync();
    return true;
}

//===============================================================
// Восстановление из резервной копии конфига
void restoreConfig()
{
    logINFO("restoreConfig() START");
    //system("cp ./Backup/configDb.db configDb.db");
    sync();
}

int config_clear( Config *config )
{
    if (config == NULL)
    {
        logERR( "ERROR! config_clear() NULL POINTER!" );
        return -1;
    }

    config->config_version  =               0;
    config->kkt_mode        =            0x00;
    config->terminal_mode   =            0x00;
    config->fs_replace_mode =            0x00;
    config->shift_timer     = SHIFT_TIMER_OFF;
    // Версия ПО
    config->fisgo_version.clear();
    config->kkt_reg_version.clear();
    config->article.clear();
    config->uuid.clear();

    config->eth_up = false;
    config->eth_manual = false;
    config->ip_adress.clear();
    config->mask.clear();
    config->router.clear();
    config->dns_main.clear();
    config->dns_add.clear();
    config->wifi_up = false;
    config->net_wifi_name.clear();
    config->net_wifi_key = 0;
    config->net_wifi_signal = 0;
    config->net_pass.clear();
    config->fs_number.clear();
    config->fs_number_count = 0;
    config->fs_numbers_table.clear();
    config->cashier_inn.clear();
    config->organization_name.clear();
    config->calculation_address.clear();
    config->calculation_place.clear();
    config->organization_inn.clear();
    config->organization_kpp.clear();
    config->kkt_reg_num.clear();
    config->kkt_plant_num.clear();

    config->tax_systems    = 0x00;
    config->cur_tax_system = 0x00;
    config->cur_department = 0x00;

    config->encryption_sign = false;
    config->excisable_sign = false;
    config->clc_services_sign = false;
    config->gambling_sign = false;
    config->lottery_sign =false;
    config->paying_agent_sign = false;

    config->ofd_choose = 0x00;
    config->ofd_inn.clear();
    config->ofd_server_address.clear();
    config->ofd_name.clear();




    config->ofd_server_port = 0;
    config->check_receipt_address.clear();
    config->ofd_server_ip.clear();
    config->open_shift_date.clear();

    config->agent_mask = 0x00;
    config->current_agent = 0;//AGENT_TYPE::OFF;
    config->logType = 0x03;
    config->isCabinetEnable = false;
    config->startTime = 0;
    config->lastMsgOFD = 0;
    config->nds_free_price =0;// nds_no;
    config->scaleConfig.type = 0;
    memset( config->scaleConfig.options, 0x00, sizeof(config->scaleConfig.options));
    config->scaleConfig.portName.clear();
    config->scaleConfig.speed = 0;

    config->scannerConfig.type = 0;
    memset( config->scannerConfig.options, 0x00, sizeof(config->scannerConfig.options));
    config->scannerConfig.portName.clear();
    config->scannerConfig.speed = 0;

    config->is_auto_cash_out = false;
    config->ffd_kkt_ver = 0x00;
    config->fs_ffd_ver = 0x00;
    config->fs_validity_date = 0;
    config->bso = false;
    config->kkt_signs = 0x00;
    config->add_kkt_signs = 0x00;

    config->automatNumber.clear();
    config->isAutoOpenShiftEnable = false;
    config->internet_reciept = MODE_ON;
    config->connectTo = 0;

    config->cloud_kkt_fns_num.clear();
    config->cloud_org_fns_num.clear();
    config->cloud_tel.clear();
    config->cloud_reg_status = 0x00;
    config->cloud_pin.clear();

    config->time_zone = 0;//config->_convert->getTimeZone();

    config->inc_pos      =      1;
    config->sound_button =  false;
    config->cloud_goods  =   true;
    config->current_route.clear();
    config->imageShift =        0;


    config->immediate_replacement = false;
    config->resources_exhastion   = false;
    config->mem_overflow          = false;
    config->critical_fn_err       = false;

    config->needSendToOfd         =     0;


    return EXIT_SUCCESS;
}

Config::Config() :  config_version(32),
                    kkt_mode(0x01),
                    terminal_mode(0x00),
                    fs_replace_mode(0x00),
                    shift_timer(SHIFT_TIMER_OFF),
                    eth_up(false),
                    eth_manual(false),
                    wifi_up(true),
                    net_wifi_key(0),
                    net_wifi_signal(0),
                    fs_number_count(0),
                    tax_systems(0x3f),
                    cur_tax_system(0x00),
                    cur_department(0x00),
                    encryption_sign(false),
                    excisable_sign(true),
                    clc_services_sign(true),
                    gambling_sign(false),
                    lottery_sign(false),
                    paying_agent_sign(false),
                    ofd_choose(0x00),
                    ofd_server_port(21101),
                    agent_mask(0x00),
                    current_agent(/*AGENT_TYPE::OFF*/0),
                    logType(0x03),
                    isCabinetEnable(false),
                    startTime(1599043189),
                    lastMsgOFD(0),
                    nds_free_price(/*E_TAXES::nds_no*/0),
                    is_auto_cash_out(false),
                    ffd_kkt_ver(0x02),
                    fs_ffd_ver(0x02),
                    fs_validity_date(0),
                    bso(false),
                    kkt_signs(0x28),
                    add_kkt_signs(0x01),
                    isAutoOpenShiftEnable(false),
                    internet_reciept(MODE_ON),
                    connectTo(0),
                    inc_pos(1),
                    sound_button(false),
                    cloud_goods(true),
                    cloud_reg_status(0x00),
                    immediate_replacement(false),
                    resources_exhastion(false),
                    mem_overflow(false),
                    critical_fn_err(false),
                    imageShift(0),
                    needSendToOfd(0)
{
    fisgo_version = "2.21.0";
    article = "1234567890123";
    uuid = " a3f21365-5af3-4991-941b-760499125786";

//    _convert.reset(new Convert);
//    time_zone = _convert->getTimeZone();
}

Config::~Config()
{
    config_clear( this );
}
