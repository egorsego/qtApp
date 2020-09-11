/*
 * config.h
 */

#ifndef SRC_APPL_INCLUDE_CONFIG_H_
#define SRC_APPL_INCLUDE_CONFIG_H_

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define DREAMKAS_RF
#define BIOS_RF
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include "loggerdaemon.h"
#include "convert.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include <string.h>
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <QString>
#include <QVector>


#include "dynamic_mask.h"
#include "libs/armeabi-v7a/qFisGoPirit/PiritLib.h"



#define YEAR_CONSTANT       1900
#define YEAR_MINIMUM        2020
#define YEAR_MAXIMUM        2022

static const int CONFIG_VERSION = 32;

static const std::string fisgo_cur_version =      "2.21.0";
static const std::string kkt_reg_version   =        "001";
static const std::string VERSION_FOR_STAT  =  "100.6.1.3";
static const std::string TIMESTAMP_FILE    =  "timestamp";

#ifdef DREAMKAS_RF
    const std::string modelName   = "Kassa_F";
    const std::string upModelName = "KASSA_F";
    const std::string product     = "kassaf";
#elif defined SPUTNIK_F
    const std::string modelName   = "SPUTNIK_F";
    const std::string upModelName = "SPUTNIK_F";
    const std::string product     = "sputnikF";
#else
    const std::string modelName   = "Kassa_RB";
    const std::string upModelName = "DREAMKAS_RB";
    const std::string product     = "dreamkasRB";
#endif



//=============================================================
//=============================================================
// Œ áª¨ ¤«ï à¥¦¨¬®¢
//=============================================================
static DYNAMIC_MASK
        //-----------------------------------------------------
        SETTINGS_AGENTS_MASK       = CURRENT_AGENT | AGENT_MASK,
        //-----------------------------------------------------
        CABINET_CONNECT_MASK       = UUID                | FISGO_VERSION         |
                                     IS_CABINET_ENABLE,
        //-----------------------------------------------------
        SETTINGS_TAXS_MASK         = TAX_SYSTEMS         | CUR_TAX_SYSTEM,
        //-----------------------------------------------------
        ETHERNET_SETTINGS_MASK     = ETH_UP | IP_ADDRESS | MASK                  |
                                     ROUTER | DNS_MAIN   | DNS_ADD               |
                                     ETH_MANUAL,
        //-----------------------------------------------------
        WI_FI_SETTINGS_MASK        = WIFI_UP             | NET_WIFI_NAME         |
                                     NET_WIFI_KEY        | NET_WIFI_SIGNAL       |
                                     NET_PASS,
        //-----------------------------------------------------
        RECEIPT_CORRECTION_MASK    = TAX_SYSTEMS,
        //-----------------------------------------------------
        SETTINGS_ID_OFD_FS_DAMAGED = SHIFT_TIMER         | OPEN_SHIFT_DATE       |
                                     FS_REPLACE_MODE,
        //-----------------------------------------------------
        FS_REPLACE_MASK            = FS_REPLACE_MODE,
        //-----------------------------------------------------
        FS_CONFIG_MASK             = FS_NUMBERS_TABLE | FS_NUMBER_COUNT | STAGE,
        //-----------------------------------------------------
        FABRICATION_MASK           = UUID | ARTICLE | KKT_PLANT_NUM,
        //-----------------------------------------------------
        REG_KKT_MASK               = IS_CABINET_ENABLE      | ORGANIZATION_NAME     |
                                     CALCULATION_ADDRESS    | ORGANIZATION_INN      |
                                     SETTINGS_TAXS_MASK     | CHECK_RECEIPT_ADDRESS |
                                     SETTINGS_AGENTS_MASK   | FS_REPLACE_MODE       |
                                     FS_NUMBER_COUNT        | FS_NUMBERS_TABLE      |
                                     CLOUD_PIN              | CLOUD_KKT_FNS_NUM     |
                                     CLOUD_ORG_FNS_NUM      | CLOUD_TEL             |
                                     CLOUD_REG_STATUS       | STAGE                 |
                                     KKT_PLANT_NUM          | CALCULATION_PLACE     |
                                     REC_SENDER_EMAIL       | ORGANIZATION_KPP,
        //-----------------------------------------------------
        OFD_SETTINGS               = OFD_CHOOSE         | OFD_INN               |
                                     OFD_SERVER_ADDRESS | OFD_NAME              |
                                     OFD_SERVER_PORT    | CHECK_RECEIPT_ADDRESS |
                                     OFD_SERVER_IP,
        //-----------------------------------------------------
        CLOUD_REG_KKT              = CLOUD_REG_STATUS       | CLOUD_PIN             |
                                     CLOUD_TEL              | CLOUD_ORG_FNS_NUM     |
                                     CLOUD_KKT_FNS_NUM      | CHECK_RECEIPT_ADDRESS |
                                     ORGANIZATION_INN       | ORGANIZATION_NAME     |
                                     CALCULATION_ADDRESS    | SETTINGS_TAXS_MASK    |
                                     KKT_PLANT_NUM          | CALCULATION_PLACE,

        CONFIG_FIELDS              = CONFIG_VER              | TERMINAL_MODE         | FS_REPLACE_MODE      |
                                     SHIFT_TIMER             | FISGO_VERSION         | ARTICLE              |
                                     UUID                    | ETH_UP                | IP_ADDRESS           |
                                     MASK                    | ROUTER                | WIFI_UP              |
                                     NET_WIFI_NAME           | NET_WIFI_KEY          | NET_WIFI_SIGNAL      |
                                     NET_PASS                | FS_NUMBER_COUNT       | FS_NUMBERS_TABLE     |
                                     CASHIER_INN             | ORGANIZATION_NAME     | CALCULATION_ADDRESS  |
                                     ORGANIZATION_INN        | ORGANIZATION_KPP      | CHECK_RECEIPT_ADDRESS|
                                     TAX_SYSTEMS             | CUR_TAX_SYSTEM        | OPEN_SHIFT_DATE      |
                                     AGENT_MASK              | CURRENT_AGENT         | LOG_TYPE             |
                                     IS_CABINET_ENABLE       | START_TIME            | LAST_MSG_OFD         |
                                     NDS_FREE_PRICE          | SCALE_TYPE            | SCALE_SPEED          |
                                     SCALE_PORT_NAME         | SCALE_OPTIONS         | SCANNER_TYPE         |
                                     SCANNER_SPEED           | SCANNER_PORT_NAME     | SCANNER_OPTIONS      |
                                     DNS_MAIN                | DNS_ADD               | ETH_MANUAL           |
                                     AUTO_CASH_OUT           | AUTO_OPEN_SHIFT       | STAGE                |
                                     INTERNET_RECIEPT        | CONNECT_TO            | CLOUD_REG_STATUS     |
                                     CLOUD_TEL               | CLOUD_PIN             | CLOUD_ORG_FNS_NUM    |
                                     CLOUD_KKT_FNS_NUM       | CABINET_REG_EMAIL     | KKT_PLANT_NUM        |
                                     CALCULATION_PLACE       | CUR_DEPARTMENT        | REC_SENDER_EMAIL     |
                                     INC_POS                 | SOUND_BUTTON          | CURRENT_ROUTE        |
                                     CLOUD_GOODS             | IS_2G_UP              | IMAGE_SHIFT          |
                                     LAST_FD                 | TARE_DEFAULT          | IS_TARE_USE          |
                                     FREE_PRICE_SALE         | FREE_PRICE_SALE_MAX_SUM | BT_STATE           |
                                     BT_PAIR                 | REVERT_VER            | SALE_ALCOHOL         |
                                     UTM_ADDRESS             | UTM_PORT              | UTM_TIMEOUT          |
                                     PRINT_EGAIS             | CHECK_ALC_SALE_TIME   | ALC_SALE_HR_START    |
                                     ALC_SALE_HR_FINISH      | ENTER_WEIGHT          | TOBACCO_COST_FROM,

        ALL_FIELDS_PREV            = ALL_FIELDS & (~(TOBACCO_COST_FROM | ENTER_WEIGHT | SALE_ALCOHOL | UTM_ADDRESS | UTM_PORT | UTM_TIMEOUT |
                                                     PRINT_EGAIS | CHECK_ALC_SALE_TIME | ALC_SALE_HR_START | ALC_SALE_HR_FINISH));
    //-----------------------------------------------------
        constexpr int                         //    ‘â âãáë ®¡« ç­®© à¥£¨áâà æ¨¨
        NOT_CLOUD_REG              = 0,       //          ¥£¨áâà æ¨ï ¢­¥ ®¡« ª 
        CLOUD_OFD_REGISTER         = 1,       //             ‚ë¡à ­ ®¡« ç­ë© Š
        CLOUD_WAITING              = 2,       //         à®¨§¢¥¤¥­  ‘Œ‘-¯®¤¯¨áì
        CLOUD_NEED_SIGN            = 3,
        CLOUD_NEED_REPORT          = 4,       //   ‡ ï¢«¥­¨¥ ãá¯¥è­® à áá¬®âà¥­®
                                              //   ¥®¡å®¤¨¬  § ¯¨áì ®âçñâ  ¢ ”
        CLOUD_REGISTERED           = 5;       // ŠŠ’  ªâ¨¢¨à®¢ ­ 
//=============================================================
    // Œ áª¨ Ž”„
    static constexpr uint8_t           FIRST_OFD = 0x01,   // ¥à¢ë© Ž”„
                                          TAXCOM = 0x02,   // ’ ªáª®¬
                                       OFD_YARUS = 0x03,   // Ÿàãá
                                        SBIS_OFD = 0x04,   // ‘ˆ‘ Ž”„
                                   KALUGA_ASTRAL = 0x05,   // Š€‹“ƒ€-€‘’€‹
                                           KORUS = 0x06,   // Š®àãá
                                         EXPRESS = 0x07,   // ‹…Š’Ž›‰ Š‘…‘‘
                                          EVOTOR = 0x08,   // ¢®â®à
                                          KONTUR = 0x09,   // ‘Š Š®­âãà
                                          YANDEX = 0x10,   // Ÿ„…Š‘ Ž”„
                                          E_OFD  = 0x11,
                                          MANUAL = 0x20,   // „“ƒŽ‰
                                          OFD_RU = 0x40,   // Ž”„.“
                                          DREAMKAS_OFD = 0x60;   // „à¨¬ª á Ž”„
        struct ScaleConfig
        {
            ScaleConfig() : type(0), speed(0)
            {
                memset( options, 0x00, sizeof(options));
            }
	        int type;          // â¨¯ ¢¥á®¢
	                           // 0 - ¢¥á®¢ ­¥â
	                           // 1 - ˜âà¨å ‘«¨¬
	        int speed;         // áª®à®áâì ¯®àâ 
            std::string portName; // ¨¬ï ¯®àâ
	        char options[64];  // ®¯æ¨¨
        };

        static const unsigned char  SHIFT_TIMER_ON = 0x01,
                                   SHIFT_TIMER_OFF = 0x00;

        struct Config
        {
                Config();
               ~Config();
                //  áâà®©ª¨
                int                 config_version; // ‚¥àá¨ï ä ©«  ª®­ä¨£ãà æ¨¨
                unsigned char             kkt_mode; // €¢â®­®¬­ë© à¥¦¨¬ 0-­¥ ¢â, 1 - ¢â®­®¬­ë©  // PIRIT OK ¥á«¨ FISCAL
                unsigned char        terminal_mode;
                unsigned char      fs_replace_mode;
                unsigned char          shift_timer;
                // ‚¥àá¨ï Ž
                std::string               fisgo_version;
                std::string                   revertVer;    // last saved version for revert
                std::string             kkt_reg_version;                                             // PIRIT OK ¥á«¨ FISCAL
                // €àâ¨ªã« ãáâà®©áâ¢ 
                std::string                     article;
                // UUID
                std::string                        uuid;
                // ‘¥âì
                bool                        eth_up;
                bool                    eth_manual;
                std::string                   ip_adress;
                std::string                        mask;
                std::string                      router;
                std::string                    dns_main;
                std::string                     dns_add;
                bool                       wifi_up;
                std::string               net_wifi_name; // ˆ¬ï á¥â¨
                int                   net_wifi_key; // ’¨¯ ª«îç  á¥â¨
                int                net_wifi_signal; // “à®¢¥­ì á¨£­ «  á¥â¨
                std::string                    net_pass; //  à®«ì á¥â¨
                // ¥ª¢¨§¨âë ¯®«ì§®¢ â¥«ï
                std::string                   fs_number;                                             // PIRIT OK ¥á«¨ FISCAL
                unsigned short            fs_number_count;
                QVector<QString>         fs_numbers_table;
                std::string                 cashier_inn;
                std::string           organization_name; //  ¨¬¥­®¢ ­¨¥ ®à£ ­¨§ æ¨¨                 // PIRIT OK ­¥®¡å®¤¨¬ ¤«ï …‚„
                std::string         calculation_address; // €¤à¥á à áçñâ®¢                           // PIRIT OK ­¥®¡å®¤¨¬ ¤«ï …‚„
                std::string           calculation_place; // Œ¥áâ® à áçñâ®¢                           // PIRIT OK ¥á«¨ FISCAL
                std::string            organization_inn; // ˆ ®à£ ­¨§ æ¨¨                          // PIRIT OK ­¥®¡å®¤¨¬ ¤«ï …‚„
                std::string            organization_kpp; // Š ®à£ ­¨§ æ¨¨
                std::string                 kkt_reg_num; // ¥£¨áâà æ¨®­­ë© ­®¬¥à ŠŠ’                // PIRIT OK ¥á«¨ FISCAL
                std::string               kkt_plant_num; // ‡ ¢®¤áª®© ­®¬¥à ŠŠ’                      // PIRIT OK
                //  «®£¨
                unsigned char          tax_systems;                                             // PIRIT OK ­¥®¡å®¤¨¬ ¤«ï …‚„
                unsigned char       cur_tax_system;
                unsigned char       cur_department;
                // à¨§­ ª¨
                bool               encryption_sign; // à¨§­ ª è¨äà®¢ ­¨ï                       // PIRIT OK ¥á«¨ FISCAL
                bool                excisable_sign; // à¨§­ ª ¯®¤ ªæ¨§­®£® â®¢ à               // PIRIT OK ¥á«¨ FISCAL
                bool             clc_services_sign; // à¨§­ ª à áçñâ  §  ãá«ã£¨                // PIRIT OK ¥á«¨ FISCAL
                bool                 gambling_sign; // à¨§­ ª ¯à®¢¥¤¥­¨ï  § àâ­ëå ¨£à          // PIRIT OK ¥á«¨ FISCAL
                bool                  lottery_sign; // à¨§­ ª ¯à®¢¥¤¥­¨ï «®â¥à¥¨               // PIRIT OK ¥á«¨ FISCAL
                bool             paying_agent_sign; // à¨§­ ª ¯« âñ¦­®£®  £¥­â                 // PIRIT OK ¥á«¨ FISCAL
                // Ž”„
                unsigned char           ofd_choose;
                std::string                     ofd_inn; // Ž”„, ˆ                                 // PIRIT OK ¥á«¨ FISCAL ¨ ­¥ ¢â®­®¬­ë© à¥¦¨¬
                std::string          ofd_server_address; // Ž”„, €¤à¥á á¥à¢¥à                        // PIRIT OK ¥á«¨ FISCAL ¨ ­¥ ¢â®­®¬­ë© à¥¦¨¬
                std::string                    ofd_name; // Ž”„,  §¢ ­¨¥                            // PIRIT OK ¥á«¨ FISCAL ¨ ­¥ ¢â®­®¬­ë© à¥¦¨¬
                int                ofd_server_port; // Ž”„, ®àâ á¥à¢¥à                         // PIRIT OK ¥á«¨ FISCAL ¨ ­¥ ¢â®­®¬­ë© à¥¦¨¬
                std::string       check_receipt_address; // Ž”„, €¤à¥á ¯à®¢¥àª¨ ç¥ª
                std::string               ofd_server_ip;
                // ‘¬¥­ 
                std::string             open_shift_date;                                             // PIRIT OK ­¥®¡å®¤¨¬ ¤«ï …‚„
                // €£¥­âë
                unsigned char           agent_mask;                                             // PIRIT OK ¥á«¨ FISCAL
                int32_t              current_agent;
                uint8_t                    logType; // ’¨¯ «®££¨à®¢ ­¨ï
                bool               isCabinetEnable; // ‡ à¥£¨áâà¨à®¢ ­ «¨ ¢ Š ¡¨­¥â¥
                time_t                   startTime; // ‚à¥¬ï áâ àâ  ª ááë
                time_t                  lastMsgOFD; // ‚à¥¬ï ¯®á«¥¤­¥© ãá¯¥è­®© ®â¯à ¢ª¨ ¤®ªã¬¥­â  ¢ Ž”„
                int                 nds_free_price; // „‘ ¯à¨ ¯à®¤ ¦¥ ¯® á¢®¡®¤­®© æ¥­¥
                ScaleConfig            scaleConfig; // ‚¥áë
                ScaleConfig          scannerConfig; // ‘ª ­­¥à
                bool              is_auto_cash_out; // €¢â®¨§êïâ¨¥
                unsigned char          ffd_kkt_ver; // ‚¥àá¨ï ””„ ŠŠ’                           // PIRIT OK ¥á«¨ FISCAL
                unsigned char           fs_ffd_ver; // ‚¥àá¨ï ””„ ”­                            // PIRIT OK ¥á«¨ FISCAL
                uint16_t          fs_validity_date; // ¥áãàá ª«îç¥© ”                         // PIRIT OK ¥á«¨ FISCAL
                bool                           bso; // ‘Ž                                      // PIRIT OK ¥á«¨ FISCAL
                unsigned char            kkt_signs; // à¨§­ ª¨ ªªâ                             // PIRIT OK ¥á«¨ FISCAL
                unsigned char        add_kkt_signs; // „®¯®«­¨â¥«ì­ë¥ ¯à¨§­ ª¨ ªªâ              // PIRIT OK ¥á«¨ FISCAL
                std::string               automatNumber; // ®¬¥à  ¢â®¬ â                            // PIRIT OK ¥á«¨ FISCAL
                bool         isAutoOpenShiftEnable; // ”« £  ¢â®¬ â¨ç¥áª®£® ®âªàëâ¨ï/§ ªàëâ¨ï á¬¥­ë
                uint64_t                     stage; // ”« £, ­¥®¡å®¤¨¬ë© ¤«ï …‚„               // PIRIT OK ­¥®¡å®¤¨¬ ¤«ï …‚„
                int               internet_reciept; // ”« £  ¢â® ¯¥ç â¨ ç¥ª®¢ ¨§ ¨­­¥â 
                int                      connectTo; // Šã¤  å®¤¨âì §  ®¡­®¢«¥­¨ï¬¨ ¨ ¢ ª ¡¨­¥â
                                                    // 0 - production, >= 0 - beta
                int                        inc_pos; // ‘å«®¯ë¢ âì ¯®§¨æ¨¨ ¢ ç¥ª¥ 1 - ¤  0 - ­¥â
                bool                  sound_button; // ‡¢ãª ª« ¢¨è 1 - ¤  0 - ­¥â
                bool                   cloud_goods; //  ®¤£àã§ª  ­¥­ ©¤¥­­®£® â®¢ à  ¨§ ®¡« ª  ¤ /­¥â
                std::string               current_route; // ’¥ªãé¨© ¢ë¡à ­­ë© ¬ àèàãâ ¤«ï âà ­á¯®àâ
                int               cloud_reg_status; // ¥£¨áâà æ¨ï ç¥à¥§ Ÿ­¤¥ªá
                std::string                   cloud_tel;
                std::string                   cloud_pin;
                std::string           cloud_org_fns_num;
                std::string           cloud_kkt_fns_num;

                std::string             cabinetRegEmail;                       // Œë«®, ª ª®â®à®¬ã ¯à¨¢ï§ ­ â¥ªãé¨©  ªª ã­â
                                                                          // ª ¡¨­¥â 
                uint32_t         registration_mask;                       // Š®¤ ¯à¨ç¨­ë ¨§¬¥­¥­¨ï á¢¥¤¥­¨© ® ŠŠ’
                uint32_t                 time_zone;                       // — á®¢®© ¯®ïá
                std::string              recSenderEmail = "kassa@dreamkas.ru"; // Email ®â¯à ¢¨â¥«ï ç¥ª
                std::string                 fs_validity;                       // ‘à®ª á«ã¦¡ë ”
                //-------------------------------------------------------------------------------
                // ‘â â¨áâ¨ª 
                //-------------------------------------------------------------------------------
                uint64_t               goodsDbSize; // Š®«¨ç¥áâ¢® â®¢ à®¢ ¢ ¡ §¥ â®¢ à®¢
                uint64_t                  usersCnt; // Š®«¨ç¥áâ¢® ¯®«ì§®¢ â¥«¥©
                uint64_t                 hotKeyCnt; // Š®«¨ç¥áâ¢® £®àïç¨å ª« ¢¨è
                //-------------------------------------------------------------------------------
                int                 batteryVoltage; // ­ ¯àï¦¥­¨¥ ­  ¡ â à¥©ª¥

                uint8_t            tobaccoCostFrom;
                enum COST_FROM
                {
                    CAB_COST = 0x00,
                    MRC_COST
                };
                //-------------------------------------------------------------------------------
                // à¨ç¨­  ¯¥à¥à¥£¨áâà æ¨¨
                //-------------------------------------------------------------------------------
                uint32_t           reRegReason = 0;
                bool          fnRegistered = false;
                //-------------------------------------------------------------------------------
                unique_ptr<Convert> _convert;


                bool  immediate_replacement = false;
                bool  resources_exhastion   = false;
                bool  mem_overflow          = false;
                bool  critical_fn_err       = false;
                bool is2Gup {false};
                bool enterWeight {true};
                uint32_t imageShift{0};
                uint32_t needSendToOfd = 0;
                uint64_t lastFd {0};
                uint64_t tareDefault {0};
                bool isTareUse {false};
                bool freePriceSale{true};
//                uint64_t freePriceSaleMaxSum {Convert::DIVISIOR_THOUSAND * Convert::DIVISIOR_RUBL};
                bool btState {false};   // á®áâ®ï­¨¥ ¬®¤ã«ï bluetooth
                std::string btPair;     // MAC- ¤à¥á ¯®¤ª«îç¥­­®£® BT-ãáâà®©áâ¢ 
                bool saleAlcohol {false};       // �த��� ��������
                std::string utmAddress;         // ���� �ࢥ� ���
                uint32_t utmPort {0};           // ���� �ࢥ� ���
                long utmTimeout {60};           // ⠩���� ������ � ���
                bool printEgais {false};        // ����� ᫨�� �����
                bool checkAlcSaleTime {false};  // ����஫� �த��� ��������
                int alcSaleHrStart {10};        // ��� ��砫� �த��� ��������
                int alcSaleHrFinish {22};          // ��� ����砭�� �த��� ��������
        };
        void createBackupConfig  ();
        bool isBackupConfigExist ();
        void restoreConfig       ();

        int config_clear(Config *);
        static const unsigned   FS_NUMBER_LEN = 16; // Œ ªá¨¬ «ì­ ï ¤«¨­­  ­®¬¥à  ”

#endif /* SRC_APPL_INCLUDE_CONFIG_H_ */
