/*
 * https_exchange.h
 */

#ifndef SRC_APPL_INCLUDE_HTTPS_EXCHANGE_H_
#define SRC_APPL_INCLUDE_HTTPS_EXCHANGE_H_

#include "libs/libcurl/inc/curl.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <mutex>
#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>

#include "../loggerdaemon.h"
#include "json.h"
#include "config.h"
#include "FisGo_BIOS.h"
#include "jsonParser.h"
#include "convert.h"
#include "EncodeConvertor.h"
#include "Md5Sum.h"
#include "license.h"
#include "filesfg.h"

#include <QRegExp>
#include <QString>


using namespace std;
using JsonParser      =  nlohmann::json;
using RequestData     =  pair<string, string>;
using JsonSnifferData =  pair<string, RequestData>;
using JsonSniffer     =  list<JsonSnifferData>;

class HttpsExchange
{
public:
	HttpsExchange();
	virtual ~HttpsExchange();
    unique_ptr<JSON>  json;
    unique_ptr<FilesFG> Fg;
    unique_ptr<Convert> convert;
    const char* UPDATE_FILE = "/download/fisGoUpdate.tar";

    enum METHODS
    {
        HANDSHAKE = 1,
        UPDATE,
        DOWNLOAD,
        ADDTOCABINET,
        DELFROMCABINET,
        FACTORYSTART,
        FACTORYFINISH,
        CASHINFOREPORT,
        KKTREGISTRINFO,
        POLL,
        COUNTERS_REPORT,
        RECEIPT,
        MONEY_DOCUMENT,
        SHIFT_DOCUMENT,
        REGISTRATION_REPORT_SUCCESS,
        REGISTRATION_REPORT_FAILED,
        GET_URL_LOG,
        SEARCHPRODUCT,
        GET_KEY_FABRICATION,
        GET_URL_IMAGE_UPLOAD,
        GET_URL_IMAGE_DOWNLOAD
    };
    /**
     * @brief The PURCHASE_ERR enum - ошибки при формировании чека
     */
    enum PURCHASE_ERR
    {
        PURCHASE_ERR_SUCCESS                  =    0,
        PURCHASE_ERR_NULL_PTR                 = 0xff,
        PURCHASE_ERR_WRONG_SHIFT_NUM          =   -1,
        PURCHASE_ERR_WRONG_RECEIPT_NUM        =   -2,
        PURCHASE_ERR_WRONG_FISCAL_DATA        =   -3,
        PURCHASE_ERR_UNKNOWN_DOC_TYPE         =   -4,
        PURCHASE_ERR_EMPTY_POSITIONS_LIST     =   -5,
        PURCHASE_ERR_WRONG_POS_QUANTITY       =   -6,
        PURCHASE_ERR_UNKNOWN_POSITION_MODE    =   -7,
        PURCHASE_ERR_EMPTY_GOODS_NAME         =   -8,
        PURCHASE_ERR_UNKNOWN_PAYMENT_TYPE     =   -9,
        PURCHASE_ERR_UNAVAILABLE_PAYMENT_TYPE =  -10,
        PURCHASE_ERR_SEND_DATA_FAILED         =  -11
    };

    static bool jsonSnifferEnable;
    static const char NOT_FIRST_ELEMENT                =   0x01;
    static const char NOT_LAST_ELEMENT                 =   0x02;
    static const char MID_ELEMENT                      =   0x03;
private:
	mutex mutexCurl;
    mutex mutexResolve;
	int seconds;
    int money_rep_sec;
	JSON::TASK_DATA task_data;
    const string hub                      = "hub";
    const string hub_beta                 = "hub-beta";
    const string update_server            = "update";
    const string update_server_beta       = "update-beta";

#ifdef BIOS_RF
		const string siteServer               = "dreamkas.ru";
		const string siteServerUpdate         = "dreamkas.ru";
#else
		const string siteServer               = "dreamkas.by";
		const string siteServerUpdate         = "dreamkas.ru";
#endif

    string hub_name                       =  hub;
    string update_server_name             =  update_server;
    /**
     * @brief snifferMutex  - мьютекс сниффера Json
     */
    mutex snifferMutex;
    /**
     * @brief JsonSniffer   - лист данных сниффера Json
     */
    JsonSniffer snifferData;
    /**
     * @brief snifferMapIns - добавление данных
     *                        сниффера Json
     * @param url           - адрес запроса
     * @param RequestData   - пара запроса и ответа
     */
    void snifferDataIns( string url, RequestData requestData );
    /**
     * @brief methodNameGet - получить наименование метода с версией по url
     * @param url - адрес запроса
     * @return наименование метода c версией
     */
    virtual string methodNameGet( string url );
public:
    void urlsInit(const bool &production = true);
    /**
     * @brief snifferDataGet - получить копию данных
     *                         сниффера Json
     * @return
     */
    JsonSniffer snifferDataGet();
    /**
     * @brief sendFile - отправка файла по url
     * @param url - адрес отправки
     * @param filePath - файл
     * @return результат отправки файла
     */
    static int sendFile(string url, string filePath);
    /**
     * @brief sendBuffer http post буфера данных
     * @param url адрес
     * @param request буфер данных
     * @param timeout таймаут обмена с сервером
     * @param response буфер ответа
     * @return результат выполнения
     */
    static int sendBuffer(const std::string &url,
                          const std::string &request,
                          const long timeout,
                          std::string &response);
    /**
     * @brief checkWebSite тест доступности Web-страницы
     * @param url адрес Web-страницы
     * @param timeout таймаут обмена с сервером
     * @return результат выполнения
     */
    static bool checkWebSite(const std::string &url, const long timeout);
    /**
     * @brief hubModeUrlSet - установить url эмулятора HUB
     * @param url - url эмулятора HUB
     */
    inline void   hubModeUrlSet( string url ){ lock_guard<mutex> locker( mutexCurl ); _hubModeUrl = url; }
    /**
     * @brief hubModeUrlGet - получить url эмулятора HUB
     * @return url эмулятора HUB
     */
    inline string hubModeUrlGet(           ) { lock_guard<mutex> locker( mutexCurl ); return _hubModeUrl; }
    /**
     * @brief hubModeAddRealUrl - добавление в json поля real_url
     * @param postData - оригинальный json
     * @param url - оригинальный url для отправки запроса
     * @return true - в случае успеха, иначе false
     */
    bool hubModeAddRealUrl(string &postData, string &url);

private:
    std::string _hubModeUrl;
    std::string urlStatistics;
    std::string urlSearchProduct;
    std::string urlLogs;
    std::string urlImage;
    std::string urlUpdate;
    std::string urlImageUpdate;
    std::string urlHandshake;
    std::string urlRegistrationReport;
    std::string urlKKTRegistrInfo;
    std::string urlPoll;
    std::string urlCountersReport;
    std::string urlReceipt;
    std::string urlMoneyDocumentReport;
    std::string urlShiftDocumentReport;
    std::string urlOFDRegister;
    std::string urlOFDRegistrationStatus;
    std::string urlOFDSign;
    std::string urlReloadProducts;
    std::string urlRequestMRPtables;
public:
    std::string reqestPoll;
    std::string urlCashInfoReport;
    std::string urlAddToCabinet;
    std::string urlDelFromCabinet;
    std::string urlGetKeyFabrication;
    std::string urlWhoAmI;

    //Распарсенный ответ при регистрации кассы в кабинете
	JSON::JSON_REGISTER registerInCabinetAnswer;
    struct EX_REG_RESULT
    {
        JSON::REGISTRATION_TYPE    type =  JSON::REG_REGISTRATION; // Тип регистрации
        JSON::EX_REG_ERR                    err = JSON::EX_REG_OK; // Код ошибки
        string                                           err_name, // Имя ошибки
                                                              msg; // Расшифровка ошибки
        string                                             fd_num, // ФД
                                                              fpd; // ФПД
        Convert::DATE_ISO_8601                               date; // Дата регистрации
        uint32_t                                       reason = 0; // Расширенный код причины перерегистрации
    };
    static int err_reg_res_init( EX_REG_RESULT * );

    JsonParser getTaxesArray(unsigned char taxModes);
    string getSnoNameForHub(unsigned char taxMode);
    JsonParser getWorkModeArray(unsigned char kktSigns, unsigned char kktAddSigns, bool isPayingAgentSign);
    JsonParser getAgents(unsigned char agent_mask);

    virtual int   sendRequest            (METHODS, string sendUrl, const Config *configData = nullptr, License *license = nullptr);
    virtual int   sendRequest            ( JSON::MONEY_DOCUMENT_REPORT_JSON *   );
    virtual int   sendRequest            ( JSON::SHIFT_DOCUMENT_REPORT_JSON *   );
    virtual int   sendRequest            ( JSON::JSON_OFD_REGISTER *            );
    virtual int   sendRequest            ( JSON::JSON_OFD_REGISTRATION_STATUS * );
    virtual int   sendRequest            ( JSON::JSON_OFD_SIGN *                );
    virtual int   downloadInThread       (                         string urlStr);
    string   RegistrationReportJsonCreate(EX_REG_RESULT * , Config *configData);
    virtual int      sendRequest(string json);
    int   sendRequestWhoAmI                                            ();
    /**
     * @brief sendReloadProducts запрос на перезагрузку товаров из Кабинета Дримкас
     * @param uuid уникальный сетевой идентификатор
     * @return результат выполнения
     */
    int sendReloadProducts(const std::string &uuid);
    /**
     * @brief sendRequestMRPtables запрос данных для расчета минимальной розничной цены алкогольной продукции на кассе
     * @param uuid уникальный сетевой идентификатор
     * @return результат выполнения
     */
    int sendRequestMRPtables(const std::string &uuid);
    int   sendHandshake          (Config *configData, std::string &req, std::string &answer);
    int   sendCashInfoReport     (Config *configData, License &license);
	int   sendKKTRegistrInfo     (Config *configData, JSONKKTRegInfo *jsonKKTRegistrInfo);
    int   sendWhoAmI             (string uuid, JSON::JSON_REGISTER& jsonRegister);
    virtual int   sendRegistrationReport ( Config*, EX_REG_RESULT* );
//    int   sendSearchProduct      (string barcode, Goods &good, Position_new &posNew);
    int   sendUpdate             (string url, bool *fisgo_update,  JSONUpdate &json, Config *config_data = nullptr);
	//int sendPoll               (Config *configData);
	int   analizeAnswer          (string jsonCode);
//    virtual int sendCountersReport(Config *configData, shared_ptr<Counters> counters);
    virtual int   sendCountersReport_bios(Config *configData, OPERATION <double>&, uint64_t&, REC_SUMS& );
//    virtual int sendReceipt(Config *configData, Receipt_new *receipt);
    int   sendMoneyDocumentReport( JSON::MONEY_DOCUMENT_REPORT_JSON *   ); // Отослать отчёт о внесении / изъятии в кабинет
    int   sendShiftDocumentReport( JSON::SHIFT_DOCUMENT_REPORT_JSON *   ); // Отослать отчёт об открытии / закрытии смены в кабинет
    int           sendOFDRegister( JSON::JSON_OFD_REGISTER *            ); // Регистрация ККТ в ОФД и ФНС
    int sendOFDRegistrationStatus( JSON::JSON_OFD_REGISTRATION_STATUS * ); // Получение статуса регистрации в ОФД
    int               sendOFDSign( JSON::JSON_OFD_SIGN *                ); // Подписать документ облачной подписью (отправка кода из СМС)
    int sendPollNew             ( Config *configData, JSON::JSON_POLL_ *json_poll, std::string &req, std::string &answer );
//    int sendReceiptRequest       ( Receipt_new *receipt );
	string getJSONStrForWorkMode(unsigned char kktSigns, unsigned char kktAddSigns, bool isPayingAgentSign);

    // Отправка логов на сервер
    int sendLog                  ( Config &configData, string &code );
    bool sendLogToServer         ( const JSON::JSON_LOGS &logsData  );
    // ---------------------------------------------------------------


    /**
     * @brief sendImage - формирование и отправка образа системы на сервер
     * @param configData - конфиг ПО
     * @return CAB_OK - в случае успеха, иначе CAB_CRITICAL_ERROR
     */
    int uploadImage(const Config &configData);
    /**
     * @brief sendImageToServer - отправка образа на сервер
     * @param imagesData - структура с данными для отправки
     * @return true - в случае успеха, иначе - false
     */
    bool uploadImageToServer(const JSON::JSON_IMAGE& imagesData);

    bool downloadImage(const Config &configData);


    /**
     * @brief sendKeyFabricationRequest - запрос ключа активации при производстве
     * @param key - ключ от сервера
     * @param configData - конфиг с заполненными article и kkt_plant_num
     * @return true - в случае успеха, иначе - false
     */
    virtual bool sendKeyFabricationRequest(string &key, Config &configData);
    /**
     * @brief sendData отправить данный через CURL
     * @param post_data данные
     * @param send_url URL
     * @param answer чтение ответа
     * @return результат выполнения
     */
    virtual int sendData(string post_data, string send_url, const bool &useAnswer = true);
    virtual JSON::STAT_ERR sendStatistics(Config *, const License&, const JSON::UpdateInfo &updateInfo = JSON::UpdateInfo("",""));
    bool isSiteServerResolved();

	string getJSONStrForTaxes(unsigned char tax_modes);
	string getTaxNameForCabinet(unsigned char taxMode);

	string getJsonArray(string, vector<string>, const char);
	vector<string> getSplitStringWithSeparator(string, char);
    virtual string getHandshakeJson();

	bool isHandshake;

	const string& getCode() const
	{
		return code;
	}

	void setCode(const string& code)
	{
		this->code = code;
	}

	const string& getFactoryNumber() const
	{
		return factoryNumber;
	}

	void setFactoryNumber(const string& factoryNumber)
	{
		this->factoryNumber = factoryNumber;
	}

	const string& getFNNnumber() const
	{
		return FNnumber;
	}

	void setFNNnumber(const string& nnumber)
	{
		FNnumber = nnumber;
	}

	const string& getUuid() const
	{
		return uuid;
	}

	void setUuid(const string& uuid)
	{
		this->uuid = uuid;
	}

	const string& getUrl() const
	{
		return url;
	}

    virtual const string& getAnswer()
    {
		return answer;
	}

	const string& getPartNumber() const
	{
		return partNumber;
	}

	void setPartNumber(const string& partNumber)
	{
		this->partNumber = partNumber;
	}

	const string& getVersion() const
	{
		return version;
	}

	void setVersion(const string& version)
	{
		this->version = version;
	}

	const string& getAddress() const
	{
		return address;
	}

	void setAddress(const string& address)
	{
		this->address = address;
	}

	const string& getInn() const
	{
		return inn;
	}

	void setInn(const string& inn)
	{
		this->inn = inn;
	}

	const string& getKpp() const
	{
		return kpp;
	}

	void setKpp(const string& kpp)
	{
		this->kpp = kpp;
	}

	const string& getOrganizationName() const
	{
		return organizationName;
	}

	void setOrganizationName(const string& organizationName)
	{
		this->organizationName = organizationName;
	}

	const string& getPlace() const
	{
		return place;
	}

	void setPlace(const string& place)
	{
		this->place = place;
	}

	const string& getRegistryNumber() const
	{
		return registryNumber;
	}

	void setRegistryNumber(const string& registryNumber)
	{
		this->registryNumber = registryNumber;
	}

	bool isIsAutonomic() const
	{
		return isAutonomic;
	}

	void setIsAutonomic(bool isAutonomic)
	{
		this->isAutonomic = isAutonomic;
	}

	int getTaxes() const
	{
        return taxes;
	}

    void setTaxes(int newTaxes)
    {
        taxes = newTaxes;
    }

	const string& getPollStr() const
	{
		return pollStr;
	}

	void setPollStr(const string& pollStr)
	{
		this->pollStr = pollStr;
	}

	const string& getOfdCheckUrl() const
	{
		return ofdCheckUrl;
	}

	void setOfdCheckUrl(const string& ofdCheckUrl)
	{
		this->ofdCheckUrl = ofdCheckUrl;
	}

	const string& getOfdInn() const
	{
		return ofdInn;
	}

	void setOfdInn(const string& ofdInn)
	{
        this->ofdInn = ofdInn;
        if (!this->ofdInn.empty())
        {
            this->ofdInn.erase(remove(this->ofdInn.begin(), this->ofdInn.end(), ' '), this->ofdInn.end());
        }
	}

	const string& getOfdName() const
	{
		return ofdName;
	}

	void setOfdName(const string& ofdName)
	{
		this->ofdName = ofdName;
	}

	const string& getOfdServerAddress() const
	{
		return ofdServerAddress;
	}

	void setOfdServerAddress(const string& ofdServerAddress)
	{
		this->ofdServerAddress = ofdServerAddress;
	}

	int getOfdServerPort() const
	{
		return ofdServerPort;
	}

	void setOfdServerPort(int ofdServerPort)
	{
		this->ofdServerPort = ofdServerPort;
	}

	uint64_t getCrCash() const
	{
		return CRCash;
	}

	void setCrCash(uint64_t crCash)
	{
		CRCash = crCash;
	}

	uint64_t getCrCashless() const
	{
		return CRCashless;
	}

	void setCrCashless(uint64_t crCashless)
	{
		CRCashless = crCashless;
	}

	uint64_t getCrMoney() const
	{
		return CRMoney;
	}

	void setCrMoney(uint64_t crMoney)
	{
		CRMoney = crMoney;
	}

	uint64_t getCrCanceled() const
	{
		return CRCanceled;
	}

	void setCrCanceled(uint64_t crMoney)
	{
		CRCanceled = crMoney;
	}


	const string& getFfdVer() const
	{
		return ffdVer;
	}

	void setFfdVer(const string& ffdVer)
	{
		this->ffdVer = ffdVer;
	}

	void setKktSigns(unsigned char kktSigns)
	{
		this->kktSigns = kktSigns;
	}

	void setKktAddSigns(unsigned char kktAddSigns)
	{
		this->kktAddSigns = kktAddSigns;
	}

	void setIsPayingAgent(bool isPayingAgent)
	{
		this->isPayingAgent = isPayingAgent;
	}

    string getSiteServer() const;

    string getUrlImageUpdate() const;

    string getUrlUpdate() const;

    string getMd5Sum( string value );
    string getUpModelName() const;

    string getUrlReceipt() const;

    string resultParse( const string & );

    string getUrlStatistics() const;

    void urlDisplay(string urlName, string url);
    void urlsDisplay();

    string getSenderEmail() const;
    void setSenderEmail(const string &value);

    JsonParser getJSONForTaxes(unsigned char tax_modes);

    string getTaxNameForCabinetWithoutEscapingQuotes(unsigned char taxMode);
    string getUrlRegistrationReport() const;

private:
    string uuid;
    string partNumber;
    string factoryNumber;
    string FNnumber;
	string code;
	string url;
	string answer;
	string version;
	string registryNumber;
    int taxes;
    bool isAutonomic;
	unsigned char kktSigns;
	unsigned char kktAddSigns;
	bool isPayingAgent;

    string senderEmail;

	string organizationName;
	string address;
	string place;
	string inn;
	string kpp;
	string ffdVer;

	string ofdName;
	string ofdServerAddress;
	int ofdServerPort;
	string ofdInn;
	string ofdCheckUrl;

    string pollStr;

    uint64_t CRCash;      //Сумма наличных продаж за вычетом возвратов
    uint64_t CRCashless;  //Сумма безналичных продаж за вычетом возвратов
    uint64_t CRMoney;     //Сумма наличных в кассе
	uint64_t CRCanceled;     //Сумма анулир

//    Receipt_new *receiptData;
};

#endif /* SRC_APPL_INCLUDE_HTTPS_EXCHANGE_H_ */

