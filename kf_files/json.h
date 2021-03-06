/*
 * json.h
 */

#ifndef SRC_APPL_INCLUDE_JSON_H_
#define SRC_APPL_INCLUDE_JSON_H_

#include <sys/time.h>

#include <string>
#include <cstdint>
#include <iostream>

#include "config.h"
#include "license.h"
#include "jsonParser.h"
#include "convert.h"
#include "filesfg.h"

using namespace std;
using JsonParser = nlohmann::json;

enum CAB_ERRORS
{
	CAB_OK              =   0x00, // ����� �믮���� �ᯥ譮
	CAB_NEED_REGISTERED =   0x01, // ���� �� ��ॣ����஢���
	CAB_NEED_HANDSHAKE  =   0x02, // ���� ����室��� �믮����� handshake � ������� �����
	CAB_NEED_UPDATE     =   0x03, // ����室��� �������� �����
	CAB_USER_FAILURE    =   0x04, // �訡�� ���짮��⥫� (���ࠢ���� ��� �� ॣ����樨, etc.)
	CAB_REQUEST_FAILURE =   0x05, // ����� ����祭 �ࢥ஬, �� �� ����� ���� ��ࠡ�⠭ (����� �� ���४��)
	NET_ERROR           =   0x06, // ����� ����祭 �ࢥ஬, �� �� ����� ���� ��ࠡ�⠭ (����� �� ���४��)
	CAB_OTHER_ERR       =   0xBB,
	CAB_CRITICAL_ERROR  =   0xFF
};

enum JSON_ERRORS
{
	JSON_NO_ERROR       = 0x00,
	JSON_OTHER_ERR      = 0xAA,
	JSON_LENGTH_ERR     = 0xAF,
    JSON_INCORRECT_ERR  = 0xFA,
	JSON_CRIRICAL_ERROR = 0xFF
};

#define FMT_STR                              " %-17s %15s "
#define FMT_LLU                            " %-17s %15llu "
#define FMT_FLOAT                            " %-17s %15f "

#define BIND_PRODUCT_TYPE                    "bind_product"
#define DELETE_PRODUCT_TYPE                "delete_product"
#define UPSERT_PRODUCT_TYPE                "upsert_product"
#define UPSERT_DISCOUNT_TYPE              "upsert_discount"
#define DELETE_DISCOUNT_TYPE              "delete_discount"
#define EXTERNAL_PURCHASE_TYPE          "external_purchase"
#define KEY_ACTIVATION_TYPE                "key_activation"
#define REGISTRATION_TASK                    "registration"
#define PURCHASE_DOCUMENT_RESULT "purchase_document_result"
#define UPSERT_CASHIER                     "upsert_cashier"
#define DELETE_CASHIER                     "delete_cashier"
#define UPDATE_MRP_TABLES               "update_mrp_tables"
//=================================================================
//                  �������� ���� �⢥⮢
//=================================================================
#define _OK_                                  "OK"
#define NO_CLOUD_SIGN              "NO_CLOUD_SIGN" // � ���짮��⥫� ��� ����筮� ������,
                                                   // ����室��� �믮����� ॣ������ � ���筮� ०���
#define _ERROR_                            "ERROR"
#define WAITING                          "WAITING" // ������/���� � ॣ����樨 ��ࠡ��뢠���� � ���,
                                                   // ���� ������ �����
#define REGISTERED                    "REGISTERED" // ���� ��ॣ����஢���, ���� � ॣ����樨 ����祭 ��� � �����ᠭ.
                                                   // ����� ��祣� ������ �� �ॡ����
#define NEED_REPORT                  "NEED_REPORT" // ������ ����祭� � �����ᠭ�, �뤠� ��� (kkt_registration_info/registry_number).
                                                   // ����室��� �������� ���� � ॣ����樨 (�᫨ �� �� �����⠭),
                                                   // ��ࠢ��� ��� � ��� � ����� ᬥ�� �����.
#define NEED_CLOUD_SIGN          "NEED_CLOUD_SIGN" // ������ �� ॣ������/���� � ॣ����樨 �ਭ���,
                                                   // �ॡ���� �������� �१ ������� ������� (��ࠢ��� ��� �� ��)
#define NOT_REGISTERED            "NOT_REGISTERED" // ���� �� ��ॣ����஢��� � ���. ����室��� �믮����� ofd_register
#define INVALID_CODE                "INVALID_CODE" // ������ ���ࠢ���� ���
//=================================================================
#include "jsonParser.h"


#ifdef DREAMKAS_RF
    #define KKT_REGISTRY_NAME "���� �"
#elif defined(SPUTNIK_F)
    #define KKT_REGISTRY_NAME "���⭨� �"
#elif defined(DREAMKAS_RB)
    #define KKT_REGISTRY_NAME "���� ��"
#endif

using JsonParser      =  nlohmann::json;

enum E_JSON
{
	JSON_RESULT                  = 1,  //�������
	JSON_UPDATE                  = 2,  //����������
	JSON_RESULT_INT              = 3,  //�������
	JSON_KKT_REGISTR             = 4,  //��������� ���
	JSON_POLL                    = 5,  //����㧪� ⮢��
	JSON_OFD_REGISTER            = 6,  // ��� �� ॣ������ � ���
	JSON_OFD_REGISTRATION_STATUS = 7,  // ����� ॣ����樨 ���
	JSON_OFD_SIGN                = 8   // ������� ���㬥��
};

struct JSONUpdateStr
{
	   string version;
	   string url;
	   string md5;
	   int status;
	   long fileSize;
};

struct JSONUpdate
{
       JSONUpdate();
	   JSONUpdate           (const JSONUpdate& ) = delete;
       JSONUpdate& operator=(const JSONUpdate& );
       char version[20];
	   char url[400];
	   char md5[80];
	   int status;
	   long fileSize;
};


struct JSONResult
{
       int result = 0;
};

struct JSONKKTRegInfoStr
{
	   string organizationName;
	   string address;
	   string realAddress;
	   string inn;
       std::string kpp;
       int cashNumber       = 0;
	   string regNumber;
	   //taxes
       bool isAutonomicMode = false;
       string ofdName;
       string ofdServerHost;
       int ofdServerPort    = 0;
       string ofdInn;
       string ofdUrl;
       string senderEmail;
       //workMode
       int result           = 0;
};


struct JSONKKTRegInfo
{
    JSONKKTRegInfo(){}
    JSONKKTRegInfo(const JSONKKTRegInfo &regInfo)
    {
        if (this != &regInfo)
        {
            memcpy(organization_name, regInfo.organization_name, sizeof(organization_name));
            memcpy(calculation_address, regInfo.calculation_address, sizeof(calculation_address));
            memcpy(calculation_place, regInfo.calculation_place, sizeof(calculation_place));
            memcpy(organization_inn, regInfo.organization_inn, sizeof(organization_inn));
            memcpy(kkt_reg_num, regInfo.kkt_reg_num, sizeof(kkt_reg_num));
            memcpy(ofd_name, regInfo.ofd_name, sizeof(ofd_name));
            memcpy(ofd_server_address, regInfo.ofd_server_address, sizeof(ofd_server_address));
            ofd_server_port = regInfo.ofd_server_port;
            memcpy(ofd_inn, regInfo.ofd_inn, sizeof(ofd_inn));
            memcpy(check_receipt_address, regInfo.check_receipt_address, sizeof(check_receipt_address));
            isAutonomicMode = regInfo.isAutonomicMode;
            memcpy(senderEmail, regInfo.senderEmail, sizeof(senderEmail));
            tax_systems = regInfo.tax_systems;
            encryption_sign   = regInfo.encryption_sign;
            excisable_sign    = regInfo.excisable_sign;
            clc_services_sign = regInfo.clc_services_sign;
            gambling_sign     = regInfo.gambling_sign;
            lottery_sign      = regInfo.lottery_sign;
            paying_agent_sign = regInfo.paying_agent_sign;
            bso               = regInfo.bso;
            internet          = regInfo.internet;
            automatic         = regInfo.automatic;
            memcpy(automaticDeviceNumber, regInfo.automaticDeviceNumber, sizeof(automaticDeviceNumber));
            agent_types = regInfo.agent_types;
            memcpy(kpp, regInfo.kpp, sizeof(kpp));
            result = regInfo.result;
        }
    }

    char organization_name[256]     = {0}; // ������������ �࣠����樨
    char calculation_address[256]   = {0}; // ���� ����⮢
    char calculation_place[256]     = {0}; // ���� ����⮢
    char organization_inn[16]       = {0}; // ��� �࣠����樨
    char kkt_reg_num[20]            = {0}; // �������樮��� ����� ���
    char ofd_name[256]              = {0}; // ���, ������������
    char ofd_server_address[256]    = {0}; // ���, ���� �ࢥ�
    int  ofd_server_port            =   0; // ���, ���� �ࢥ�
    char ofd_inn[16]                = {0}; // ���, ���
    char check_receipt_address[256] = {0}; // ���, ���� �஢�ન 祪�
    bool isAutonomicMode          = false; // ��⮭���� ०�� ����祭
    unsigned char tax_systems       =   0; // ��� ��⥬� ���������������
    char senderEmail[256]           = {0}; // ���� ��ࠢ�⥫� 祪�
    bool encryption_sign        = false; // �ਧ��� ��஢����
    bool excisable_sign         = false; // �ਧ��� �����樧���� ⮢��
    bool clc_services_sign      = false; // �ਧ��� ����� �� ��㣨
    bool gambling_sign          = false; // �ਧ��� �஢������ ������� ���
    bool lottery_sign           = false; // �ਧ��� �஢������ ���२
    bool paying_agent_sign      = false; // �ਧ��� ����񦭮�� �����
    bool bso                    = false; // ���
    bool internet               = false; // ��� ��� ���୥�
    bool automatic              = false; // ��⮬���᪨� ०��
    char automaticDeviceNumber[256] = {0};  // ����� ��⮬��
    unsigned char agent_types       = 0;    // ���� �ᯮ��㥬�� ����⮢
    char kpp[9]                     = {0};  // ���
    int result                      = 0;
};

class JSON
{
public:
    JSON()
    {
        fg.reset(new FilesFG);
        convert.reset( new Convert );
    }
    virtual ~JSON()
    {
        fg.reset(nullptr);
        convert.reset(nullptr);
    }
public:
    static int getAnswerError(string jsonResStr);
    //=================================================================
	//             JSON ���㬥�⮢ ���ᥭ��/������
	//=================================================================
	const string MONEY_IN  =  "MONEY_IN"; // ���ᥭ��
	const string MONEY_OUT = "MONEY_OUT"; // ����⨥
    //=================================================================
    // ����䥩� ��饭�� � 䠩����
    //=================================================================
    unique_ptr<FilesFG>  fg;
    //=================================================================
    unique_ptr<Convert> convert;
    static const uint8_t FS_VALIDITY_LEN = 6;
	struct MONEY_DOCUMENT_REPORT
	{
		MONEY_DOCUMENT_REPORT();
		~MONEY_DOCUMENT_REPORT();
		uint64_t        shift, // ����� ᬥ��
		               number; // ����� 祪�
		string           date, // ��� � �६� (ISO8601) ॣ����樨
		             doc_type; // ��� ���㬥�� MONEY_IN ��� MONEY_OUT
		uint64_t          sum; // �㬬� ���㬥�� � ��������
		string   cashier_name; // ��� �����
	};
	struct MONEY_DOCUMENT_REPORT_JSON
	{
		MONEY_DOCUMENT_REPORT_JSON();
		~MONEY_DOCUMENT_REPORT_JSON();
		string           uuid;                   // �����䨪��� �����
		vector<MONEY_DOCUMENT_REPORT> documents; // ���㬥��� ���ᥭ�� � ������
	};
	static int money_doc_rep_clear( MONEY_DOCUMENT_REPORT_JSON * );
    //=================================================================
	//             JSON ���㬥�⮢ ������/������� ᬥ��
    //=================================================================
    static const std::string OPEN_SHIFT;        // ����⨥ ᬥ��
    static const std::string CLOSE_SHIFT;       // �����⨥ ᬥ��
    static const std::string SALE;              // ��室
    static const std::string REFUND;            // ������ ��室�
    static const std::string OUTFLOW;           // ���室
    static const std::string OUTFLOW_REFUND;    // ������ ��室�
    static const std::string SALE_ANNUL;        // ��室 ���㫨஢����
    static const std::string REFUND_ANNUL;      // ������ ���㫨஢����
    static const std::string CASH;              // ������
    static const std::string CASHLESS;          // �����஭��
    static const std::string PREPAID;           // �����
    static const std::string CREDIT;            // �।��
    static const std::string CONSIDERATION;     // ����筮� �।�⠢�����

	struct PAYMENTS
	{
		PAYMENTS();
		~PAYMENTS();
        string    type; // CASH / CASHLESS
		uint64_t count, // ������⢮ �����
		           sum; // �㬬� �� ⨯� ������
	};
	struct COUNTERS
	{
		COUNTERS();
		~COUNTERS();
        string                type; // SALE / REFUND/ OUTFLOW / OUTFLOW_REFUND
		vector<PAYMENTS>  payments; // ������
		uint64_t         total_sum; // �㬬� �� ⨯� ���㬥��
	};
	struct SHIFTS
	{
		SHIFTS();
		~SHIFTS();
		uint64_t            shift, // ����� ᬥ��
		                   number; // ����� 祪�
		string               date, // ��� � �६� (ISO8601) ॣ����樨
		                 doc_type; // OPEN_SHIFT ��� CLOSE_SHIFT
		uint64_t   sum_cash_begin, // �㬬� �� ��砫� ᬥ��
		             sum_cash_end, // �㬬� �� ����� ᬥ�� (��易⥫쭮 ��� CLOSE_SHIFT)
		           count_cash_out, // ������⢮ ����⨩ �� ᬥ�� (��易⥫쭮 ��� CLOSE_SHIFT)
		            count_cash_in; // ������⢮ ���ᥭ�� �� ᬥ�� (��易⥫쭮 ��� CLOSE_SHIFT)
		string       cashier_name; // ��� �����
		vector<COUNTERS> counters; // ����稪�
	};
	struct SHIFT_DOCUMENT_REPORT_JSON
	{
		SHIFT_DOCUMENT_REPORT_JSON();
		~SHIFT_DOCUMENT_REPORT_JSON();
		string uuid; // �����䨪��� �����
		vector<SHIFTS> shifts; // ���㬥��� ������/������� ᬥ�
	};
	static int shift_doc_rep_clear( SHIFT_DOCUMENT_REPORT_JSON * );
	//=================================================================
    virtual int jsonParser(int type, string jsonCode, void *retStruct);
	static string fs_table_JSON( vector<string> *fs_table );
//	static string agents_JSON_create( AGENTS &agents );
//	static AGENTS agents_get( string JSON );
	static vector<string> fs_table_get( string JSON );

public:
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    enum METHODS_VER
    {
        // OUT
        HANDSHAKE_VER                = 2,
        OFD_REGISTER_VER             = 1,
        UPLOAD_PRODUCT               = 1, //�� �ᯮ������
        TEST_OFD_RESPONSE            = 1,
        MONEY_DOCUMENT_REPORT_VER    = 1,
        UNREGISTER_VER               = 1,
        SHIFT_DOCUMENT_REPORT_VER    = 1,
        REGISTRATION_REPORT_VER      = 1,
        CASH_INFO_REPORT_VER         = 1,
        POLL_VER                     = 3,
        KKT_REGISTER_INFO_VER        = 1,
        COUNTERS_REPORT_VER          = 1,
        OFD_REGISTRATION_STATUS      = 1,
        SEARCH_PRODUCT               = 1,
        PURCHASE_DOCUMENT_REPORT_VER = 3,
        OFD_SIGN                     = 1,
        RELOAD_PRODUCTS_VER          = 1,
        REQUEST_MRP_TABLES_VER       = 1,
        REGISTER_VER                 = 1,

        // IN
        UPSERT_PRODUCT_VER           = 6,
        EXTERNAL_PURCHASE            = 2,
        BIND_PRODUCT_VER             = 1,
        UPDATE_SOFTWARE              = 1,
        DELETE_PRODUCT_VER           = 1,
        DELETE_DISCOUNT_VER          = 1,
        UPSERT_DISCOUNT_VER          = 1,
        WHO_AM_I_VER                 = 1,
        REGISTRATION_VER             = 2,
        KEY_ACTIVATION_VER           = 1,
        PURCHASE_DOCUMENT_RES_VER    = 1,
        UPSERT_CASHIER_VER           = 1,
        DELETE_CASHIER_VER           = 1,
        UPDATE_MRP_TABLES_VER        = 1
    };
//===================================================================================
//                           ���� JSON (DreamkasF <-> HUB)
//===================================================================================
	enum class JSON_TYPE
	{
		// Viki API
        HANDSHAKE = 1,            // ��⠭���� ��砫� ������ � �����
        REGISTER,                 // ��������� ����� � �������
        UNREGISTER ,              // �⪫�祭�� ����� �� �������
        KKT_REGISTER_INFO,        // ����� ������ ��� ॣ����樨 ��� �� �������
        CASH_INFO_REPORT,         // ��ࠢ�� ���७��� ������ � ���� � �������
        COUNTERS_REPORT,          // ���� � ���ᮢ�� ���稪��
        PURCHASE_DOCUMENT_REPORT, // ��ࠢ�� 祪��
       // MONEY_DOCUMENT_REPORT,    // ��ࠢ�� ���㬥�⮢ ���ᥭ��/������
        SHIFT_DOCUMENT_REPORT,    // ��ࠢ�� ���㬥�⮢ �� ᬥ�� (������/�������)
        SEARCH_PRODUCT,           // ���� ⮢�஢ � �������
		// Poll API
        POLL,                     // ������ ��⮤ ��ᯮ��
        BIND_PRODUCT,             // �ਢ易�� ������� ⮢�� ����� � ⮢��� � �������
        DELETE_PRODUCT,           // ������� ⮢�� � �����
        UPSERT_PRODUCT,           // �������� ��� ����㧨�� ⮢�� �� �����
        UPSERT_DISCOUNT,          // ����㧨�� ᪨��� �� �����
        DELETE_DISCOUNT,          // ������� ᪨��� � �����
		EXTERNAL_PURCHASE,        // ��᪠����஢��� 祪 �� ���譥� ��⥬�
		KEY_ACTIVATION            // ������� ���� ��⨢�樨 ��業���
	};
//===================================================================================
//                       �������� JSON (DreamkasF <-> HUB)
//===================================================================================
//-----------------------------------------------------------------------------------
//                                ��ꥪ��
//-----------------------------------------------------------------------------------
	struct JSON_VERSION
	{
		string                        full_version,
                                      short_version;
	};
//-----------------------------------------------------------------------------------
	struct JSON_METHOD
	{
		string                        name,
                                      version;
	};
//-----------------------------------------------------------------------------------
	struct JSON_CASHIER
	{
		string                        name,
                                      tab_number,
                                      inn;
	};
//-----------------------------------------------------------------------------------
	struct JSON_OFD_PROVIDER
	{
		string                        name,
                                      inn,
                                      server_host,
                                      server_port,
                                      check_url;
	};
//-----------------------------------------------------------------------------------
	struct JSON_SHOP_INFO
	{
		string                  	  legal_name,
                                      shop_name,
                                      address,
                                      inn,
                                      kpp;
	};
//-----------------------------------------------------------------------------------
	struct JSON_KKT_REGISTRATION_INFO
	{
		string             			  registry_number;
		unsigned char            	  tax_modes = 0x00;
        bool                          autonomic = false;
		JSON_OFD_PROVIDER             ofd_provider;
		unsigned char                 work_mode = 0x00;
	};
//-----------------------------------------------------------------------------------
	struct JSON_KKT_INFO
	{
		string          			  kkt_factory_number,
                                      kkt_registry_name,
                                      fn_number,
                                      fn_registry_name,
                                      ffd_version;
		JSON_KKT_INFO()
		{
            kkt_registry_name = "KKT_REGISTRY_NAME"; //ec.CP866toUTF8(KKT_REGISTRY_NAME, &errP);
		}
	};
//-----------------------------------------------------------------------------------
    struct JSON_ALCOHOL_META
	{
        std::string alc_code;           // ������� ⮢��
        std::string alc_type_code;      // ��� ���� �������쭮� �த�樨�

        long double capacity {.00};     // 񬪮���
        long double alc_content {.00};  // ᮤ�ঠ��� ��������
        bool keg {false};               // �ਧ��� ��୮�� � �����
	};
//-----------------------------------------------------------------------------------
struct JSON_WEAR_PARENT_META
{
	string                        parentRemID,
							       parentName;
};
//-----------------------------------------------------------------------------------
	struct JSON_META
	{
		string                        rem_id,
									  name,
									  item;
		uint64_t                      price = 0,
                                      nds   = 0,
                                      type  = 0;
        double                        nds_manual = 0.0;
        bool                          alcohol    = false;
		JSON_ALCOHOL_META             alcohol_meta;
        bool                          scale      = false;
        float                         precision  = 0.0;
		string                        measure,
									  article,
				                      barcode,
                                      depart_name;
        std::string originCountryCode;
        std::string customEntryNum;
        uint32_t exciseDuty = 0;
        bool marked {false};
		//WEAR
		string 						  variety_name;
		uint32_t                      sort_order = 0;
		JSON_WEAR_PARENT_META         parent;
	};
//-----------------------------------------------------------------------------------
	struct JSON_PRODUCT
	{
		string                        index;
		JSON_META                     meta;
	};
//-----------------------------------------------------------------------------------
	struct JSON_POSITION
	{
        uint64_t                      number   = 0,
                                      quantity = 0,
                                      price    = 0,
				                      discount = 0;
		JSON_PRODUCT                  product;
	};
//-----------------------------------------------------------------------------------
    struct TAG_VALUE
    {
        enum TAG_TYPE
        {
            TAG_TYPE_NUMBER,
            TAG_TYPE_STRING,
            TAG_TYPE_BOOLEAN,
            TAG_TYPE_MAX
        };

        TAG_VALUE()
        {
            type = TAG_TYPE_MAX;
            number = uint64_t();
            str = string();
            boolean = bool();
        }

        explicit TAG_VALUE(const uint64_t& n)
        {
            type = TAG_TYPE_NUMBER;
            number = n;
            str = string();
            boolean = bool();
        }

        explicit TAG_VALUE(const string& s)
        {
            type = TAG_TYPE_STRING;
            number = uint64_t();
            str = s;
            boolean = bool();
        }

        explicit TAG_VALUE(const bool& b)
        {
            type = TAG_TYPE_BOOLEAN;
            number = uint64_t();
            str = string();
            boolean = b;
        }

        bool operator==(const TAG_VALUE& other) const
        {
            bool result = (getType() == other.getType());
            result = result && (getNumber() == other.getNumber());
            result = result && (getString() == other.getString());
            result = result && (getBoolean() == other.getBoolean());
            return result;
        }

		uint64_t getNumber()    const { return number; }
        string   getString() const    { return str; }
        bool    getBoolean()  const   { return boolean; }
        TAG_TYPE getType() const      { return type; }

    private:
        TAG_TYPE type;
		uint64_t number;
        string str;
        bool boolean;
    };

    struct JSON_POSITION_EXTERNAL
    {
        std::string name;
        std::string type;
        uint64_t quantity {0};
        uint64_t price    {0};
        uint64_t total    {0};
        uint64_t tax_sum  {0};
        std::string tax;
        std::string remId;
        std::map<int32_t, TAG_VALUE> tags;
    };
//-----------------------------------------------------------------------------------
	struct JSON_PAYMENT
	{
		string                        type;
		uint64_t                      count = 0,
                                      sum   = 0;
	};
//-----------------------------------------------------------------------------------
	struct JSON_PURCHASE
	{
		string                        rem_id;
		uint64_t                      shift  = 0,
				                      number = 0;
		string                        date,
				                      type;
		uint64_t                      total_sum             = 0,
				                      sum_without_discounts = 0,
				                      discount_sum          = 0;
		JSON_CASHIER                  cashier;
		vector <JSON_POSITION>        positions;
		vector <JSON_PAYMENT>         payments;
	};
//-----------------------------------------------------------------------------------
	struct JSON_DOCUMENT
	{
		uint64_t                      shift  = 0,
				                      number = 0;
		string                        date,
				                      type;
		JSON_CASHIER                  cashier;
		uint64_t                      sum = 0;
	};
//-----------------------------------------------------------------------------------
	struct JSON_COUNTERS
	{
		string                        type;
		vector <JSON_PAYMENT>         payments;
		uint64_t                      total_sum = 0;
	};
//-----------------------------------------------------------------------------------
	struct JSON_SHIFT
	{
		uint64_t                      shift  = 0,
				                      number = 0;
		string                        date,
				                      type;
		uint64_t                      sum_cash_begin = 0,
				                      sum_cash_end   = 0,
				                      count_cash_out = 0,
				                      count_cash_in  = 0;
		JSON_CASHIER                  cashier;
		vector <JSON_COUNTERS>        counters;
	};

//-----------------------------------------------------------------------------------
    /**
     * @brief The TASK_TYPE enum - ⨯� �᮪ �� HUB
     */
    enum TASK_TYPE
    {
        TASK_UNKNOWN = 0,
        TASK_BIND_PRODUCT,
        TASK_DELETE_PRODUCT,
        TASK_UPSERT_PRODUCT,
        TASK_UPSERT_DISCOUNT,
        TASK_DELETE_DISCOUNT,
        TASK_EXTERNAL_PURCHASE,
        TASK_KEY_ACTIVATION,
        TASK_REGISTRATION,
        TASK_PURCHASE_DOCUMENT_RESULT,
        TASK_UPSERT_CASHIER,
        TASK_DELETE_CASHIER,
        TASK_UPDATE_MRP_TABLES
    };
    struct JSON_TASK_RESULTS
	{
        JSON_TASK_RESULTS(uint64_t taskId_ = 0, int result_ = 0, string message_ = "", TASK_TYPE type_ = TASK_UNKNOWN)
        {
            task_id = taskId_;
            result  = result_;
            message = message_;
            type    = type_;
        }
        uint64_t  task_id = 0;
        int       result = 0;
        string    message;
        TASK_TYPE type = TASK_UNKNOWN;
	};
    /**
     * @brief The PURCHASE_DOCUMENT_CODE enum ���� १���� �᪠����樨 祪� (�ନ���)
     */
    enum class PURCHASE_DOCUMENT_CODE : uint8_t
    {
        UNKNOWN_ERR = 0,
        EMPTY_DEVICE_POOL
    };
    const std::unordered_map<std::string, PURCHASE_DOCUMENT_CODE> purchaseCodes = {{"UNKNOWN ERROR", PURCHASE_DOCUMENT_CODE::UNKNOWN_ERR},
                                                                                   {"E_EMPTY_DEVICE_POOL", PURCHASE_DOCUMENT_CODE::EMPTY_DEVICE_POOL}};
    /**
     * @brief The JSON_PURCHASE_DOCUMENT_RESULT struct १���� �᪠����樨 祪�
     */
    struct JSON_PURCHASE_DOCUMENT_RESULT
    {
        uint64_t taskId = 0;                                                    // ����� �������
        int status = JSON::IN_PROGRESS;                                         // ����� �᪠����樨
        uint64_t number = 0;                                                    // ����� 祪�
        uint64_t shift = 0;                                                     // ����� ᬥ��
        time_t utc {0};                                                         // ��� unix UTC
        long double sum = 0.00;                                                 // �㬬� 祪�
        std::string fn;                                                         // ����� ��
        std::string i;
        std::string fp;                                                         // ���
        uint64_t fd = 0;                                                        // ��
        PURCHASE_DOCUMENT_CODE code = PURCHASE_DOCUMENT_CODE::UNKNOWN_ERR;      // ��� �訡��
        std::string msg;                                                        // ᮮ�饭��
        std::string phone;                                                      // ⥫�䮭 ���㯠⥫�
    };

//-----------------------------------------------------------------------------------
    struct JSON_TASK
	{
		uint64_t                      task_id = 0;
        TASK_TYPE                     type = TASK_UNKNOWN;
        int                           status = JSON::WARNING;
		string                        message;
	};
//-----------------------------------------------------------------------------------
	struct JSON_BIND_PRODUCT
	{
		uint64_t                      task_id = 0;
		string                        item_id,
				                       rem_id;
	};
//-----------------------------------------------------------------------------------
	struct JSON_DELETE_PRODUCT
	{
		uint64_t                      task_id = 0;
		string                         rem_id;
	};
//-----------------------------------------------------------------------------------
	struct JSON_UPSERT_PRODUCT
	{
		uint64_t                      task_id = 0;
		JSON_PRODUCT                  product;
	};
//-----------------------------------------------------------------------------------
    /**
     * @brief The JSON_UPSERT_CASHIER struct ����� ���������� �����
     */
    struct JSON_UPSERT_CASHIER
    {
        void reset()
        {
            task_id = 0;
            tabNumber.clear();
            name.clear();
            inn.clear();
            password.clear();
        }
        uint64_t task_id {0};
        std::string tabNumber;  // ⠡���� ����� �����
        std::string name;       // ��� �����
        std::string inn;        // ���
        std::string password;   // ��� ��஫�
    };
//-----------------------------------------------------------------------------------
    /**
     * @brief The JSON_DELETE_CASHIER struct ����� 㤠����� �����
     */
    struct JSON_DELETE_CASHIER
    {
        void reset()
        {
            task_id = 0;
            tabNumber.clear();
        }
        uint64_t task_id {0};
        std::string tabNumber;  // ⠡���� ����� �����
    };
//-----------------------------------------------------------------------------------
    struct JSON_UPDATE_MRP_TABLES
    {
        uint64_t task_id {0};
//        MRP::MRPtables mrpTables;

        void reset()
        {
            task_id = 0;
//            mrpTables.reset();
        }
    };
//-----------------------------------------------------------------------------------
    struct JSON_KEY_ACTIVATION
    {
        uint64_t                      task_id = 0;
        string                         rem_id,
                                          key;
    };
//-----------------------------------------------------------------------------------
	struct JSON_ATTRIBUTES
	{
		string                        phone,
				                      email;
	};
	struct JSON_TAXES
	{
		uint64_t                      nds_no     = 0,
				                      nds_0      = 0,
				                      nds_10     = 0,
				                      nds_18     = 0,
				                      nds_10_110 = 0,
				                      nds_18_118 = 0;
	};
	struct JSON_TOTAL
	{
		uint64_t                      total_sum = 0;
		JSON_TAXES                    taxes;
	};

	struct JSON_EXTERNAL_PURCHASE
	{
        uint64_t                        task_id = 0;
        string                          rem_id;
        string                          tax_mode;
        string                          type;
        vector <JSON_POSITION_EXTERNAL> positions_ex;
        vector <JSON_PAYMENT>           payments;
        JSON_ATTRIBUTES                 attributes;
        JSON_TOTAL                      total;
        bool                            isValid = false;
        map<int32_t, TAG_VALUE>         tags;
	};
    //--------------------------------------------------------
    //--------------------------------------------------------
    // ��� ����⮢ ॣ����樨/���ॣ����樨
    //--------------------------------------------------------
    static const string             REGISTRATION;
    static const string        CHANGE_PARAMETERS;
    static const string CHANGE_PARAMETERS_AND_FN;
    static const string                 CLOSE_FN;
    enum REGISTRATION_TYPE
    {
        REG_NONE = 0,
        REG_REGISTRATION,                   // ��������� ��� ������ ��
        REG_CHANGE_PARAMETERS,              // ���ॣ������ ��� ������ ��
        REG_CHANGE_PARAMETERS_AND_FN,       // ���ॣ������ � ������� ��
        REG_CLOSE_FN                        // ������� ��
    };
    enum REG_REPORT_TYPE
    {
        NEW_REG_REP = 1,
        OLD_REG_REP
    };

    enum EX_REG_ERR
    {
        EX_REG_OK = 1,
        EX_REG_CABINET_IS_DISCONNECTED,     // ��� �裡 � ������⮬
        EX_REG_KKT_NOT_FISCAL,              // ��� �� ��ॣ����஢���
        EX_REG_NOT_AVAILABLE,               // ��������� �� ����㯭�
        EX_REG_ALREADY_FISCAL,              // ��� 㦥 ��ॣ����஢���
        EX_REG_FN_CLOSED,                   // �� ������
        EX_REG_FN_NOT_REGISTERED,           // �� �� ��ॣ����஢��
        EX_REG_CFG_ERROR,                   // �訡�� �⥭�� ���䨣��樨
        EX_REG_DATE_ERROR,                  // �訡�� ���� � �६���
        EX_REG_KKT_REG_NUM_EMPTY,           // ��������� �� ���
        EX_REG_INVALID_KKT_REG_NUM,         // ������ �� ���
        EX_REG_KKT_INN_EMPTY,               // ��������� ��� �࣠����樨
        EX_REG_ADDR_EMPTY,                  // ��������� ���� ����⮢
        EX_REG_REAL_ADDR_EMPTY,             // ��������� ���� ����⮢
        EX_REG_TAX_SYSTEMS_EMPTY,           // �� ��࠭� ���
        EX_REG_ORGANIZATION_NAME_EMPTY,     // ��������� ������������ �࣠����樨
        EX_REG_FS_ERROR,                    // ���� �裡 � ��
        EX_REG_CLOUD_REGISTRATION_STARTED,  // ���� ����筠� ॣ������
        EX_REG_SHIFT_IS_OPEN,               // ����� �����
        EX_REG_HAS_UNSENT_DOCUMENTS,        // ���� ����।���� ���㬥��� � ���
        EX_REG_INFO_GET_ERR,                // �訡�� �⥭�� ������ ॣ����樨
        EX_REG_KKT_ERR,                     // �訡�� ॣ����樨 ��
        EX_REG_FS_CLOSE_ERR,                // �訡�� ������� ��
        EX_REG_DATA_WRONG_FS_NUMBER,        // ������ ����� ��
        EX_REG_EMPTY_AUTOMAT_NUMBER,        // ���⮩ ����� ��⮬��
        EX_REG_SUCCESS,                     // ���������/���ॣ������ �ᯥ譮 �����襭�
        EX_REG_TIME_NOT_SYNC                // �訡�� ᨭ�஭���樨 �६���
    };


    /**
     * @brief The EX_TAGS enum ALARM!!!
     *        ��᫥ ���������� ������ ⥣� ����� ������� ��� ⥣ � TAGS_LIST
     */
    enum EX_TAGS
    {
        EX_TAGS_POSITION_SIGN_METHOD_CALC = 1214,
        EX_TAGS_POSITION_PREPAYMENT_SUM   = 777,
        EX_TAGS_POSITION_TYPE             = 1212,
        EX_TAGS_POSITION_NAME             = 1030,
        EX_TAGS_RECEIPT_ADVANCE           = 1215,

        EX_TAGS_PAYMENT_AGENT_SIGN        = 1057,
        EX_TAGS_TRANSFER_OPERATOR_PHONE   = 1075,
        EX_TAGS_PAYMENT_AGENT_OPERATION   = 1044,
        EX_TAGS_PAYMENT_AGENT_PHONE       = 1073,
        EX_TAGS_TRANSFER_OPERATOR_NAME    = 1026,
        EX_TAGS_TRANSFER_OPERATOR_ADDRESS = 1005,
        EX_TAGS_TRANSFER_OPERATOR_INN     = 1016,
        EX_TAGS_PROVIDER_PHONE            = 1171,   // ⥫. ����� ���⠢騪�
        EX_TAGS_PAYMENT_OPERATOR_PHONE    = 1074,
        EX_TAGS_PROVIDER_NAME             = 1225,   // ������������ ���⠢騪�
        EX_TAGS_PROVIDER_INN              = 1226,   // ��� ���⠢騪�

        EX_TAGS_EXCISE_DUTY               = 1229,   // ��樧
        EX_TAGS_ORIGIN_COUNTRY_CODE       = 1230,   // ��� ��࠭� �ந�宦����� ⮢��
        EX_TAGS_CUSTOM_ENTRY_NUM          = 1231,   // ����� ⠬������� ������樨

        EX_TAGS_PRODUCT_CODE              = 1162,   // ��� ⮢��

        EX_TAGS_BUYER_NAME                = 1227,   // ������������ ���㯠⥫�
        EX_TAGS_BUYER_INN                 = 1228,   // ��� ���㯠⥫�

        EX_TAGS_ADD_USER_NAME             = 1085,   // ������������ �������⥫쭮�� ४����� ���짮��⥫�
        EX_TAGS_ADD_USER_VALUE            = 1086,   // ���祭�� �������⥫쭮�� ४����� ���짮��⥫�

        EX_TAGS_ADD_POS_REQ               = 1191,   // �������⥫�� ४����� �।��� �����
        EX_TAGS_ADD_REC_REQ               = 1192,   // �������⥫�� ४����� 祪� ���

        EX_TAGS_CLC_PLACE                 = 1187,   // ���� �����

        EX_TAGS_MAX
    };




    enum SIGN_METHOD_CALC
    {
		SIGN_METHOD_CALC_FULL_PREPAYMENT = 1,
		SIGN_METHOD_CALC_PREPAYMENT      = 2,
		SIGN_METHOD_CALC_ADVANCE         = 3,
		SIGN_METHOD_CALC_FULL_CALC       = 4,
		SIGN_METHOD_CALC_CREDIT          = 5,
		SIGN_METHOD_CALC_FULL_CREDIT     = 6,
		SIGN_METHOD_CALC_PAY_CREDIT      = 7
    };

    std::vector<EX_TAGS> TAGS_POSITION_LIST = { EX_TAGS_POSITION_SIGN_METHOD_CALC,
                                        EX_TAGS_POSITION_PREPAYMENT_SUM,
                                        EX_TAGS_POSITION_TYPE,
                                        EX_TAGS_POSITION_NAME,
                                        EX_TAGS_PAYMENT_AGENT_SIGN,
                                        EX_TAGS_TRANSFER_OPERATOR_PHONE,
                                        EX_TAGS_PAYMENT_AGENT_OPERATION,
                                        EX_TAGS_PAYMENT_AGENT_PHONE,
                                        EX_TAGS_TRANSFER_OPERATOR_NAME,
                                        EX_TAGS_TRANSFER_OPERATOR_ADDRESS,
                                        EX_TAGS_TRANSFER_OPERATOR_INN,
                                        EX_TAGS_PROVIDER_PHONE,
                                        EX_TAGS_PAYMENT_OPERATOR_PHONE,
                                        EX_TAGS_PROVIDER_NAME,
                                        EX_TAGS_PROVIDER_INN,
                                        EX_TAGS_EXCISE_DUTY,
                                        EX_TAGS_ORIGIN_COUNTRY_CODE,
                                        EX_TAGS_CUSTOM_ENTRY_NUM,
                                        EX_TAGS_PRODUCT_CODE,
                                        EX_TAGS_ADD_POS_REQ};

    vector<EX_TAGS> TAGS_RECEIPT_LIST  = { EX_TAGS_RECEIPT_ADVANCE,
                                        EX_TAGS_PAYMENT_AGENT_SIGN,
                                        EX_TAGS_TRANSFER_OPERATOR_PHONE,
                                        EX_TAGS_PAYMENT_AGENT_OPERATION,
                                        EX_TAGS_PAYMENT_AGENT_PHONE,
                                        EX_TAGS_TRANSFER_OPERATOR_NAME,
                                        EX_TAGS_TRANSFER_OPERATOR_ADDRESS,
                                        EX_TAGS_TRANSFER_OPERATOR_INN,
                                        EX_TAGS_PROVIDER_PHONE,
                                        EX_TAGS_PROVIDER_NAME,
                                        EX_TAGS_PAYMENT_OPERATOR_PHONE,
                                        EX_TAGS_PROVIDER_INN,
                                        EX_TAGS_BUYER_NAME,
                                        EX_TAGS_BUYER_INN,
                                        EX_TAGS_ADD_USER_NAME,
                                        EX_TAGS_ADD_USER_VALUE,
                                        EX_TAGS_ADD_REC_REQ,
                                        EX_TAGS_CLC_PLACE};
	//--------------------------------------------------------
    // ������� ������� ॣ����樨/���ॣ����樨
    //--------------------------------------------------------
    struct JSON_REGISTRATION
    {
        uint64_t          taskId =        0; // ID �������
        REGISTRATION_TYPE   type = REG_NONE; // ��� �����
        string                  cashierName; // ��� �����
        string                   cashierInn; // ��� �����
        string                     fnNumber; // ����� ��
    };
    //--------------------------------------------------------
    //--------------------------------------------------------
    // ������� JSON �� ����� ��ࠢ�� ����
    //--------------------------------------------------------
    struct JSON_LOGS
    {
        string       url;   // URL ��� ������� 䠩�� ����
        string      ssuk;   // ��� 䠩�� ����
        string        id;   // ����� ���� ��� ��
        uint32_t del = 0;   // �६� ����� � ᥪ㭤��
    };

    /**
     * @brief The JSON_IMAGE struct - ������� ������ ��� ��ࠢ�� "��ࠧ� ��⥬�"
     */
    struct JSON_IMAGE
    {
        string url;   // URL ��� ������� 䠩��
        string ssuk;

        string uuid;
        string fnNumber;
        string urlDonwload;
    };

//-----------------------------------------------------------------------------------
    // ����㧪� ᪨��� �� �����

    struct JSON_UPSDISC_RESULT
    {
        uint32_t type = 0;
        string  value;
    };

    struct JSON_UPSDISC_PRODUCT
    {
        string rem_id;
    };

    struct JSON_UPSERT_DISCOUNT
    {
        uint64_t   task_id = 0;

        string      rem_id;
        string        name;
        uint32_t      mode = 0;
        time_t  start_time = 0;
        time_t    end_time = 0;

        JSON_UPSDISC_RESULT result;
        vector<JSON_UPSDISC_PRODUCT> products;
    };

    struct JSON_DELETE_DISCOUNT
    {
        uint64_t task_id = 0;

        string rem_id;
    };

//-----------------------------------------------------------------------------------
//                        ��⠭���� ��砫� ������ � �����
//-----------------------------------------------------------------------------------
	struct JSON_HANDSHAKE
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
			string                uuid;
			JSON_VERSION          version;
			// �����ন����� ��⮤� � �� ��ਨ

			vector <JSON_METHOD>  out,
					              in;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
			int                result = 0;
		}response;
	};
//-----------------------------------------------------------------------------------
//                        ��������� ����� � �������
//-----------------------------------------------------------------------------------
	struct JSON_REGISTER
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
            std::string uuid;
            std::string model;
            std::string readable_model;
            std::string pin;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
            void reset()
            {
                result = 0;
                message.clear();
                owner.clear();
                kktName.clear();
                shopName.clear();
                displayInn.clear();
                displayCompany.clear();
                goodsCount = 0;
            }
            int result {0};
            uint64_t goodsCount {0};
            std::string owner;
            std::string kktName;
            std::string shopName;
            std::string displayInn;
            std::string displayCompany;
            std::string message;
		}response;
	};
//-----------------------------------------------------------------------------------
//                        �⪫�祭�� ����� �� �������
//-----------------------------------------------------------------------------------
	struct JSON_UNREGISTER
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
			string            uuid;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
			int             result = 0;
		}response;
	};
//-----------------------------------------------------------------------------------
//                  ����� ������ ��� ॣ����樨 ��� �� �������
//-----------------------------------------------------------------------------------
	struct JSON_KKT_REGISTER_INFO
	{

		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
			string                     uuid;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
			string                     result;
			JSON_SHOP_INFO             shop_info;
			JSON_KKT_REGISTRATION_INFO kkt_registration_info;
		}response;
	};
//-----------------------------------------------------------------------------------
//                  ��ࠢ�� ���७��� ������ � ���� � �������
//-----------------------------------------------------------------------------------
	struct JSON_CASH_INFO_REPORT
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
			string                     uuid;
			JSON_SHOP_INFO             shop_info;
			JSON_KKT_INFO              kkt_info;
			JSON_KKT_REGISTRATION_INFO kkt_registration_info;

			string                     jacarta_expire_date;
			bool                       is_fn;
			string                     kkt_expire_date,
					                   local_time;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
			int                         result = 0;
		}response;
	};
//-----------------------------------------------------------------------------------
//                          ���� � ���ᮢ�� ���稪��
//-----------------------------------------------------------------------------------
	struct JSON_COUNTERS_REPORT
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
			string                      uuid;
			int64_t                     cash     = 0,
					                    cashless = 0,
					                    receipts = 0,
					                     balance = 0;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
			int                           result = 0;
		}response;
	};
//-----------------------------------------------------------------------------------
//                          ��ࠢ�� 祪��
//-----------------------------------------------------------------------------------
	struct JSON_PURCHASE_DOCUMENT_REPORT
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
			string                      uuid;
			vector <JSON_PURCHASE>      purchases;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
			int                            result = 0;
		}response;
	};
//-----------------------------------------------------------------------------------
//                  ��ࠢ�� ���㬥�⮢ ���ᥭ��/������
//-----------------------------------------------------------------------------------
	struct JSON_MONEY_DOCUMENT_REPORT
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
			string                      uuid;
			vector <JSON_DOCUMENT>      documents;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
			int                            result = 0;
		}response;
	};
//-----------------------------------------------------------------------------------
//                 ��ࠢ�� ���㬥�⮢ �� ᬥ�� (������/�������)
//-----------------------------------------------------------------------------------
	struct JSON_SHIFT_DOCUMENT_REPORT
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
			string                      uuid;
			vector <JSON_SHIFT>         shifts;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
			int                         result = 0;
		}response;
	};
//-----------------------------------------------------------------------------------
//                      ���� ⮢�஢ � �������
//-----------------------------------------------------------------------------------
	struct JSON_SEARCH_PRODUCT
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
			string                      uuid,
					                    barcode;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
			vector <JSON_PRODUCT>       products;
		}response;
	};
//===================================================================================
//===================================================================================
//===================================================================================
//                             ������ ���
//===================================================================================
//-----------------------------------------------------------------------------------
//                 ��������� ��� � ��� � ��� ����஭�� �����
//-----------------------------------------------------------------------------------
    struct JSON_OFD_REGISTER
	{
		JSON_OFD_REGISTER();
		~JSON_OFD_REGISTER();
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        string                        uuid; // UUID
		// shop_info
		string                   shop_name, // ������������ ��������
			                   realAddress, // ���� ����⮢
                          organization_inn, // ���
                                       pin, // ��. ����
			                         phone, // ����䮭
                               org_fns_num; // ��� �࣠����樨
		// kkt_info
		string          kkt_factory_number, // �� ���
			             kkt_registry_name, // ������������ �த�� (���)
			                     fn_number; // ��
		// kkt_registration_info
		bool             paying_agent_sign; // �ਧ��� ����񦭮�� �����
		unsigned char            tax_modes, // ���
                                 kkt_signs, // ����� ࠡ��� ���
		                     add_kkt_signs; // �������⥫�� �ਧ���� ࠡ��� ���
        string                 kkt_fns_num; // � ��� ���
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        string                  HUB_result, // �⢥� HUB
		                        OFD_result, // �⢥� ���
		                                    // OK             ���㬥�� �ਭ��, �ॡ���� ��������
                                            //                �१ ������� ������� (��ࠢ��� ��� �� ��)
                                            // NO_CLOUD_SIGN  � ���짮��⥫� ��� ����筮� ������, ����室���
                                            //                �믮����� ॣ������ � ���筮� ०���
                                            // ERROR          �訡�� � �������� ������, ����஢�� � ���� error_message"
		                     error_message; // �����஢�� �訡��
	};
    // �������� �������� JSON_OFD_REGISTER
    static int JSON_OFDRegisterCreate( Config *, JSON_OFD_REGISTER * );
//-----------------------------------------------------------------------------------
//             ����祭�� ����� ॣ����樨 � ��� ����஭�� �����
//-----------------------------------------------------------------------------------
	struct JSON_OFD_REGISTRATION_STATUS
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        string            uuid; // UUID
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        string      HUB_result, // �⢥� HUB
		                status, // ����� ॣ����樨 � ���
		                        // WAITING	        ������/���� � ॣ����樨 ��ࠡ��뢠���� � ���, ���� ������ �����
		                        // REGISTERED	    ���� ��ॣ����஢���, ���� � ॣ����樨 ����祭 ��� � �����ᠭ.
		                        //                  ����� ��祣� ������ �� �ॡ����
		                        // NEED_REPORT	    ������ ����祭� � �����ᠭ�, �뤠� ��� (kkt_registration_info/registry_number).
		                        //                  ����室��� �������� ���� � ॣ����樨 (�᫨ �� �� �����⠭),
		                        //                  ��ࠢ��� ��� � ��� � ����� ᬥ�� �����.
		                        // NEED_CLOUD_SIGN	������ �� ॣ������/���� � ॣ����樨 �ਭ���,
		                        //                  �ॡ���� �������� �१ ������� ������� (��ࠢ��� ��� �� ��)
		                        // NOT_REGISTERED	���� �� ��ॣ����஢��� � ���. ����室��� �믮����� ofd_register
		                        // ERROR	        �訡��, ����஢�� � ���� error_message".
		                        //                  ����室��� ������ �믮����� ofd_register
		         error_message; // �����஢�� �訡��
		// kkt_registration_info
		string registry_number; // �� ���
	};
    // �������� �������� JSON_OFD_REGISTRATION_STATUS
    static int JSON_OFDRegistrationStatusCreate( Config *, JSON_OFD_REGISTRATION_STATUS * );
//-----------------------------------------------------------------------------------
//   �������� ���㬥�� ����筮� �������� (��ࠢ�� ���� �� ���) ����஭�� �����
//-----------------------------------------------------------------------------------
	struct JSON_OFD_SIGN
	{
		JSON_OFD_SIGN();
		~JSON_OFD_SIGN();
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        string          uuid, // UUID
		                code; // ��� �� ���
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        string    HUB_result, // �⢥� HUB
		          OFD_result, // �⢥� ���
		       error_message; // �����஢�� �訡��
	};
    // �������� �������� JSON_OFD_SIGN
    static int JSON_OFD_Sign( Config *, string, JSON_OFD_SIGN * );
    //===================================================================================
//===================================================================================
//===================================================================================
//-----------------------------------------------------------------------------------
//                          ������ ��⮤ ��ᯮ��
//-----------------------------------------------------------------------------------
    struct TASK_DATA
    {
        std::vector <JSON::JSON_TASK> tasks;

        std::vector <JSON::JSON_BIND_PRODUCT> bind_products;
        std::vector <JSON::JSON_DELETE_PRODUCT> delete_products;
        std::vector <JSON::JSON_UPSERT_PRODUCT> upsert_products;
        std::vector <JSON::JSON_UPSERT_DISCOUNT> upsert_discounts;
        std::vector <JSON::JSON_DELETE_DISCOUNT> delete_discounts;
        JSON_EXTERNAL_PURCHASE external_purchase;
        std::vector<JSON::JSON_EXTERNAL_PURCHASE> external_purchases;
        std::vector <JSON::JSON_REGISTRATION> registrations;
        JSON_KEY_ACTIVATION key_activation;
        std::vector<JSON_PURCHASE_DOCUMENT_RESULT> purchaseResults;
        std::vector<JSON_UPSERT_CASHIER> upsertCashiers;
        std::vector<JSON_DELETE_CASHIER> deleteCashiers;
        std::vector<JSON_UPDATE_MRP_TABLES> updateMRPtables;
    };
    //===============================================================================
    struct JSON_POLL_
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ��室�� �����
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct REQUEST
		{
			string                          uuid;
			vector <JSON_TASK_RESULTS>      task_results;
		}request;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		// �室�� �����
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		struct RESPONSE
		{
			vector <JSON_TASK>              tasks;
            TASK_DATA                       task_data;
            int                             result = 1;
		}response;
	};
    static const int POLL_RESPONSE     =      1,
                     POLL_REQUEST      =      2;
    static const uint8_t OFFSET_OUTPUT =   0x04;
    /**
     * @brief The STATISTICS_ERR enum - �訡�� �� �ନ஢���� ����⨪�
     */
    enum STAT_ERR
    {
        STAT_ERR_SUCCESS = 0,
        STAT_ERR_JSON_COMPLITE = 1,
        STAT_ERR_NULL_PTR = -1,
        STAT_ERR_EMPTY_JSON = -2,
        STAT_ERR_SEND_FAILED = -3,
        STAT_ERR_HUB_ERROR = -4,
        STAT_ERR_EMPTY_UUID = -5
    };
    //===============================================================================
    //                              ��⮤� ���⪨
    //===============================================================================
    static void json_poll_clear          ( JSON_POLL_                          *json_poll            );
    static void json_poll_clear          ( JSON_POLL_                          *json_poll, int field );
    static void task_data_clear          ( TASK_DATA                           *task_data            );
    static void task_clear               ( JSON_TASK                           *task                 );
    static void tasks_clear              ( vector<JSON_TASK>                   *tasks                );
    static void task_result_clear        ( JSON_TASK_RESULTS                   *task_result          );
    static void task_results_clear       ( vector<JSON_TASK_RESULTS>           *task_results         );
    static void key_activation_clear     ( JSON_KEY_ACTIVATION                 *key_activation       );
    static void upsert_product_clear     ( JSON_UPSERT_PRODUCT                 *upsert_product       );
    static void upsert_products_clear    ( vector<JSON_UPSERT_PRODUCT>         *upsert_products      );
    static void bind_product_clear       ( JSON_BIND_PRODUCT                   *bind_product         );
    static void bind_products_clear      ( vector<JSON_BIND_PRODUCT>           *bind_products        );
    static void delete_product_clear     ( JSON_DELETE_PRODUCT                 *delete_product       );
    static void delete_products_clear    ( vector<JSON_DELETE_PRODUCT>         *delete_products      );
    static void attributes_clear         ( JSON_ATTRIBUTES                     *attributes           );
    static void taxes_clear              ( JSON_TAXES                          *taxes                );
    static void total_clear              ( JSON_TOTAL                          *total                );
    static void payment_clear            ( JSON_PAYMENT                        *payment              );
    static void payments_clear           ( vector<JSON_PAYMENT>                *payments             );
    static void position_clear           ( JSON_POSITION                       *position             );
    static void positions_clear          ( vector<JSON_POSITION>               *positions            );
    static void position_ex_clear        ( JSON_POSITION_EXTERNAL              *position_ex          );
    static void positions_ex_clear       ( vector<JSON_POSITION_EXTERNAL>      *positions_ex         );
    static void external_purchase_clear  ( JSON_EXTERNAL_PURCHASE              *external_purchase    );
    static void product_clear            ( JSON_PRODUCT                        *product              );
    static void meta_clear               ( JSON_META                           *meta                 );
    static void upsert_discount_clear    ( JSON_UPSERT_DISCOUNT&                                     );
    static void delete_discount_clear    ( JSON_DELETE_DISCOUNT&                                     );
    //===============================================================================
    static const int      IN_PROGRESS = -1,
                          SUCCESS      =  0,
                          WARNING      =  1,
                          ERROR        =  2;
    static JSON_TASK*     NextTaskGet( vector<JSON_TASK> *tasks );
	static int jsonPreAnalizeErrors(string jsonCode);
//===============================================================================
//                ��⮤� ����஢���� ������� � ��ꥪ⮢ JSON
//===============================================================================
    static int product_display     ( JSON_PRODUCT                   *product            );
    static int attributes_display  ( JSON_ATTRIBUTES                *attributes         );
    static int taxes_display       ( JSON_TAXES                     *taxes              );
    static int position_display    ( JSON_POSITION                  *position           );
    static int position_ex_display ( JSON_POSITION_EXTERNAL         *position_ex        );
    static int payment_display     ( JSON_PAYMENT                   *payment            );
    static int total_display       ( JSON_TOTAL                     *total              );
    //-------------------------------------------------------------------------
    static int task_display      ( JSON_TASK                      *task               );
	static int tasks_display     ( vector<JSON_TASK>              *tasks              );
    //-------------------------------------------------------------------------
    static int task_display      ( JSON_BIND_PRODUCT              *bind_product       );
	static int tasks_display     ( vector<JSON_BIND_PRODUCT>      *bind_products      );
    //-------------------------------------------------------------------------
    static int task_display      ( JSON_DELETE_PRODUCT            *delete_product     );
	static int tasks_display     ( vector<JSON_DELETE_PRODUCT>    *delete_products    );
    //-------------------------------------------------------------------------
    static int task_display      ( JSON_UPSERT_PRODUCT            *upsert_product     );
	static int tasks_display     ( vector<JSON_UPSERT_PRODUCT>    *upsert_products    );
    //-------------------------------------------------------------------------
    static int task_display      ( JSON_EXTERNAL_PURCHASE         *external_purchase  );
    static int all_tasks_display ( TASK_DATA                      *task_data          );
	//===============================================================================
	//                            ����� JSON
	//===============================================================================
    virtual int jsonPollParser( string JSON, JSON_POLL_                             * );
    static string jsonPollStrCreate( JSON_POLL_                                     * );
    static int jsonRegCabinetParser(string JSON, JSON_REGISTER& jsonCabRegister);
	                           //      ����� �⢥� �� ������ ����筮� ॣ����樨
	static int        jsonOFDRegisterParser( string,            JSON_OFD_REGISTER * );
	                          // ����� �⢥� �� ����� ����� ����筮� ॣ����樨
	static int jsonRegistrationStatusParser( string, JSON_OFD_REGISTRATION_STATUS * );
	                          //                         ����� �⢥� �� ���-�������
	static int            jsonOfdSignParser( string,                JSON_OFD_SIGN * );
    static int      jsonSearchProductParser( string JSON, JSON_PRODUCT &product     );
    static int               jsonLogsParser( string JSON,                JSON_LOGS& );

	// �஢�ઠ �� ����稥 ����뫥� � ᪨����� �� ���譨� 祪��
	static bool             isValidPosition( string   	     name,
											 string   	     type,
											 uint64_t 	 quantity,
											 uint64_t 	    price,
											 uint64_t 	    total );
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    struct UpdateInfo
    {
        UpdateInfo(const string &status_, const string &version_) :
                   status(status_), version(version_) {}

        const string status;
        const string version;
    };
    virtual STAT_ERR jsonCfgCreate(Config *cfg, const License &license, const UpdateInfo &updateInfo, string &);
	static string addToJSON(string name, string   data, bool isLast = false);
	static string addToJSON(string name, bool     data, bool isLast = false);
	static string addToJSON(string name, int      data, bool isLast = false);
    static string addToJSON(int      data, bool isLast = false);
    static string addToJSON(double   data, bool isLast = false);
	static string addToJSON(string name, uint64_t data, bool isLast = false);
    static string addToJSON(string name, double data, bool isLast = false);
    static string addStructToJSON(string name, string data, bool isLast = false);
    static string addStructToJSON(string data, bool isLast = false);
    static string getStageName(KKT_STAGE stage);

	struct POLL_PARSE_ERRORS
	{
		enum ERRORS: int
		{
            BAD_RESULT                      = -0x01,
            OK                              =  0x00,
            BAD_JSON                        =  0xAA,
            BAD_UPSERT                      =  0xB1,
            BAD_BIND                        =  0xB2,
            BAD_DELETE                      =  0xB3,
            BAD_ACTIVATE                    =  0xB4,
            BAD_EXTERNAL_PURCHASE           =  0xAB,
            EXEPTION_JSON                   =  0xB5,
            BAD_UPSERT_DISCOUNT             =  0xB6,
            BAD_DELETE_DISCOUNT             =  0xB7,
            BAD_PURCHASE_DOCUMENT_RESULT    =  0xB8,
            BAD_UPDATE_MRP_TABLES           =  0xB9,
            BAD_TASK_TYPE
		};
	};

    enum DISCOUNT_ERR
    {
       DISCOUNT_ERR_SUCCESS               =  0,
       DISCOUNT_ERR_DATA_EMPTY            = -1,
       DISCOUNT_ERR_RESULT_EMPTY          = -2,
       DISCOUNT_ERR_TARGET_DATA_EMPTY     = -3,
       DISCOUNT_ERR_TARGET_PRODUCTS_EMPTY = -4,
       DISCOUNT_ERR_NULL_PTR              = -5,
       DISCOUNT_ERR_EMPTY_PRODUCT_REM_ID  = -6,
       DISCOUNT_ERR_EMPTY_JSON            = -7,
       DISCOUNT_ERR_EMPTY_MODE            = -8
    };

    enum EXTERNAL_PURCHASE_ERR
    {
        EXTERNAL_PURCHASE_ERR_SUCCESS                  = 0,
        EXTERNAL_PURCHASE_ERR_DATA_EMPTY                  ,
        EXTERNAL_PURCHASE_ERR_REM_ID_MISSING              ,
        EXTERNAL_PURCHASE_ERR_TYPE_MISSING                ,
        EXTERNAL_PURCHASE_ERR_POSITIONS_MISSING           ,
        EXTERNAL_PURCHASE_ERR_PAYMENTS_MISSING            ,
        EXTERNAL_PURCHASE_ERR_TOTAL_MISSING               ,
        EXTERNAL_PURCHASE_ERR_TOTAL_SUM_MISSING           ,
        EXTERNAL_PURCHASE_ERR_PAYMENTS_SUM_MISSING        ,
        EXTERNAL_PURCHASE_ERR_POSITIONS_NAME_MISSING      ,
        EXTERNAL_PURCHASE_ERR_POSITIONS_TYPE_MISSING      ,
        EXTERNAL_PURCHASE_ERR_POSITIONS_QUANTITY_MISSING  ,
        EXTERNAL_PURCHASE_ERR_POSITIONS_PRICE_MISSING     ,
        EXTERNAL_PURCHASE_ERR_INVALID_POSITION            ,
        EXTERNAL_PURCHASE_ERR_DATA_MISSING                ,
        EXTERNAL_PURCHASE_ERR_TAGS_TAG                    ,
        EXTERNAL_PURCHASE_ERR_TAGS_VALUE                  ,
        EXTERNAL_PURCHASE_ERR_TAGS_NOT_AVAILABLE          ,
        EXTERNAL_PURCHASE_ERR_REQUIRED_FIELD_EMPTY
    };
    /**
     * @brief parseTags - ���� ⥣�� �� ���譥�� 祪�
     * @param jsonTags - json � ⥣��� ([{"tag":1214, "value":0}, {"tag":1214, "value":1}])
     * @param src - ������� � ���ﬨ |map<int32_t, TAG_VALUE> tags|
     * @return ��� �訡�� �� EXTERNAL_PURCHASE_ERR
     */
    template<typename T>
    EXTERNAL_PURCHASE_ERR parseTags(const JsonParser& jsonTags, T& src);
    /**
     * @brief jsonImageUploadParser - ���� ��뫪� ��� ��ࠢ�� ��ࠧ� ��
     * @param JSON - json � url ��� ��ࠢ�� ��ࠧ�
     * @return rue - �᫨ 㤠���� ࠯�����, ���� - false
     */
    virtual bool jsonImageUploadParser(const string &JSON, JSON_IMAGE&);

    /**
     * @brief jsonImageDownloadParser - ���� ��뫪� ��� ����祭�� ��ࠧ� ��
     * @param JSON - json � url ��� ����祭�� ��ࠧ�
     * @return rue - �᫨ 㤠���� ࠯�����, ���� - false
     */
    virtual bool jsonImageDownloadParser(const string &JSON, JSON_IMAGE&);
//    /**
//     * @brief parsePurchaseDocumentResult ���� १���� �᪠����樨 祪�
//     * @param task json १���� �᪠����樨 祪�
//     * @param purchaseResult ������� १���� �᪠����樨 祪�
//     * @return १���� �믮������ ����
//     */
//    virtual bool parsePurchaseDocumentResult(const JsonParser &task, JSON_PURCHASE_DOCUMENT_RESULT &purchaseResult) const;
    /**
     * @brief parseUpsertCashier ���� ���������� �����
     * @param task json
     * @param upsertCashier ����� �����
     * @return १���� �믮������
     */
    virtual bool parseUpsertCashier(const JsonParser &task, JSON_UPSERT_CASHIER &upsertCashier) const;
    /**
     * @brief parseDeleteCashier ���� 㤠����� �����
     * @param task json
     * @param deleteCashier ����� �����
     * @return १���� �믮������
     */
    virtual bool parseDeleteCashier(const JsonParser &task, JSON_DELETE_CASHIER &deleteCashier) const;
    /**
     * @brief parseUpdateMRPtables ���� ⠡��� ��� �������쭮� �த�樨
     * @param task json
     * @param updateMRPtables ����� ⠡��� ��� �������쭮� �த�樨
     * @return १���� �믮������
     */
    bool parseUpdateMRPtables(const JsonParser &task, JSON_UPDATE_MRP_TABLES &updateMRPtables) const;
protected:
    static bool isTaskProcessed(const uint64_t &taskId, const JSON_POLL_ &json_poll);

    // ���� ���� �᪨ upsert_product
    virtual bool parseUpsertProduct( JsonParser        jsonUpsertTask,
                                     JSON_UPSERT_PRODUCT     &upsertStruct);
    // ���� ���� �᪨ bind_product
    virtual bool   parseBindProduct( const JsonParser        &jsonBindTask,
                                     JSON_BIND_PRODUCT       &bindStruct);
	// ���� ���� �᪨ delete_product
    virtual bool parseDeleteProduct( const JsonParser        &jsonDelTask,
                                     JSON_DELETE_PRODUCT     &delStruct);
	// ���� ���� �᪨ key_activation
    virtual bool parseActivationKey( const JsonParser        &jsonActTask,
                                     JSON_KEY_ACTIVATION     &actStruct);
//    /**
//     * @brief parseKktRegistr - ���� �⢥� �� ������ ॣ����樨
//     * @param jsonAnswer - ��ப� json � �⢥⮬ �� �ࢥ�
//     * @param regInfoStr - ������� ��� ���������� ����묨
//     * @param regInfo - ������� ��� ���������� ����묨
//     * @return ��� �訡��
//     */
//    virtual int parseKktRegistr(const string &jsonAnswer, JSONKKTRegInfoStr &regInfoStr, JSONKKTRegInfo &regInfo);
    /**
     * @brief ���� �᪨ �� ���譨� 祪
     * @param[in] jsonExPurchaseTask - �᪠ � ���譨� 祪��
     * @param[in] exPurchaseStruct - ������� ���譥�� 祪�
     * @return true �᫨ �ᯥ譮
     */
    virtual EXTERNAL_PURCHASE_ERR parseExternalPurchase( const JsonParser &jsonExPurchaseTask,
                                                         JSON_EXTERNAL_PURCHASE &exPurchaseStruct );
    /**
     * @brief pushTags - �������� ⥣ � ����� ⥣��
     * @param tag - json � ⥣�� ({"tag":1214, "value":0})
     * @param src - ������� � ���ﬨ |map<int32_t, TAG_VALUE> tags|
     * @return true - �᫨ 㤠���� ࠯�����, ���� - false
     */
    template<typename T>
    EXTERNAL_PURCHASE_ERR pushTags(const JsonParser& tag, T& src);
    /**
     * @brief parseUpsertDiscount - ���� �᪨ �� ���������� ᪨���
     * @param jsonUpsertDiscount - json � ᪨����
     * @param upsertDiscount - ������� ᪨���
     * @return DISCOUNT_ERR
     */
    virtual DISCOUNT_ERR parseUpsertDiscount(const JsonParser &jsonUpsertDiscount, JSON_UPSERT_DISCOUNT &upsertDiscount);
    /**
     * @brief parseDeleteDiscount - ���� �᪨ �� 㤠����� ᪨���
     * @param jsonDeleteDiscount - json � ᪨���� �� 㤠�����
     * @param deleteDiscount - ������� ᪨��� �� 㤠�����
     * @return DISCOUNT_ERR
     */
    virtual DISCOUNT_ERR parseDeleteDiscount(const JsonParser &jsonDeleteDiscount, JSON_DELETE_DISCOUNT &deleteDiscount);

public:
    static constexpr char TAX_DEFAULT[]   = "DEFAULT";
    static constexpr char TAX_SIMPLE[]    = "SIMPLE";
    static constexpr char TAX_SIMPLE_WO[] = "SIMPLE_WO";
    static constexpr char TAX_ENVD[]      = "ENVD";
    static constexpr char TAX_AGRICULT[]  = "AGRICULT";
    static constexpr char TAX_PATENT[]    = "PATENT";

    static constexpr char WORK_MODE_CIPHER[]     = "CIPHER";
    static constexpr char WORK_MODE_EXCISE[]     = "EXCISE";
    static constexpr char WORK_MODE_SERVICES[]   = "SERVICES";
    static constexpr char WORK_MODE_GAMBLING[]   = "GAMBLING";
    static constexpr char WORK_MODE_LOTTERY[]    = "LOTTERY";
    static constexpr char WORK_MODE_AUTOMATIC[]  = "AUTOMATIC";
    static constexpr char WORK_MODE_PAY_AGENTS[] = "PAY_AGENTS";
    static constexpr char WORK_MODE_BSO[]        = "BSO";
    static constexpr char WORK_MODE_INTERNET[]   = "INTERNET";

    static constexpr char AGENT_BANK_PAY_AGENT[]     = "BANK_PAY_AGENT";
    static constexpr char AGENT_BANK_PAY_SUB_AGENT[] = "BANK_PAY_SUB_AGENT";
    static constexpr char AGENT_PAY_AGENT[]          = "PAY_AGENT";
    static constexpr char AGENT_PAY_SUB_AGENT[]      = "PAY_SUB_AGENT";
    static constexpr char AGENT_ATTORNEY[]           = "ATTORNEY";
    static constexpr char AGENT_COMMISSIONAIRE[]     = "COMMISSIONAIRE";
    static constexpr char AGENT_AGENT[]              = "AGENT";
};

#endif /* SRC_APPL_INCLUDE_JSON_H_ */
