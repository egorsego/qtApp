#ifndef HTTPS_EXCH
#define HTTPS_EXCH
/*
 * https_exchange.cpp
 *
 */

#include "https_exchange.h"
#include <QDebug>

#define TIME_OUT_FOR_CURL 60

bool HttpsExchange::jsonSnifferEnable = false;

HttpsExchange::HttpsExchange ()
{
    urlsInit();
    _hubModeUrl.clear();
    json.reset( new JSON );
    Fg.reset( new FilesFG );
    convert.reset( new Convert );
//    net.reset(new Net);
//    _taxes.reset(new Taxes);
    isHandshake = false;
    seconds = 0;
    mutexCurl.unlock();
}

HttpsExchange::~HttpsExchange ()
{
    seconds = 0;
}

string HttpsExchange::getUpModelName() const
{
    return upModelName;
}

string HttpsExchange::getUrlUpdate() const
{
    return urlUpdate;
}

string HttpsExchange::getUrlImageUpdate() const
{
    return urlImageUpdate;
}

string HttpsExchange::getSiteServer() const
{
    return siteServer;
}

static size_t write_file (char *ptr, size_t size, size_t nmemb, FILE *data)
{
    return fwrite(ptr, size, nmemb, data);
}

static size_t write_data (char *ptr, size_t size, size_t nmemb, string *data)
{
    if (data)
    {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    }
    else
    {
        return 0;  // будет ошибка
    }
}

void HttpsExchange::snifferDataIns( string url, RequestData requestData )
{
    if(!jsonSnifferEnable)
    {
        return;
    }

    lock_guard<mutex> locker( snifferMutex );

    if(snifferData.size() > 100)
    {
        snifferData.pop_front();
    }
    snifferData.push_back( make_pair(url, requestData) );
}

string HttpsExchange::methodNameGet(string url)
{
    logINFO( "%s Url : %s", __func__, url.c_str() );

//    boost::regex methodTemplate("/[[:word:]]{1,}/v[[:digit:]]{1,}$");
//    boost::smatch methodResults;
//    boost::regex_search( url, methodResults, methodTemplate );

 //============
 QRegExp rx("/(\\w+)/v(\\d+){1,}");
 QString str = url.c_str();
 QStringList list;
 int pos = 0;

 while ((pos = rx.indexIn(str, pos)) != -1)
 {
     list << rx.cap(0);
     list << rx.cap(1);
     pos += rx.matchedLength();
 }
 for(size_t i=0;i < list.size();i++)
 {
     qDebug() << "--== i=" << i << ":" << list.at(i) << endl;
 }

//==========+

    if ( url.find("/stats") != string::npos )
    {
        logINFO( "%s Method : stats", __PRETTY_FUNCTION__);
        return "/stats";
    }

    if ( url.find("/get_activation_key") != string::npos )
    {
        logINFO( "%s Method : stats", __PRETTY_FUNCTION__);
        return "/1c/get_activation_key";
    }

    if ( url.find("/start") != string::npos )
    {
        logINFO( "%s Method : stats", __PRETTY_FUNCTION__);
        return "/v2/test/start";
    }

    if ( url.find("/finish") != string::npos )
    {
        logINFO( "%s Method : stats", __PRETTY_FUNCTION__);
        return "/v2/test/finish";
    }

    if (url.find("/update") != string::npos)
    {
        logINFO( "%s Method : update", __PRETTY_FUNCTION__);
        return "/update/v2/projects/fisgo/products/kassaf/updates/";
    }

//    for(const string &method: methodResults)
//    {
//        string methodName( method );
//        logINFO( "%s Method : %s", __PRETTY_FUNCTION__, methodName.c_str() );
//        return methodName;
//    }
    logERR( "%s Method don't found!", __PRETTY_FUNCTION__ );
    return "";
}

//JsonSniffer HttpsExchange::snifferDataGet()
//{
//    lock_guard<mutex> locker( snifferMutex );
//    JsonSniffer snifferDataCopy = snifferData;
//    snifferData.clear();
//    return snifferDataCopy;
//}

//string HttpsExchange::getUrlReceipt() const
//{
//    return urlReceipt;
//}

string HttpsExchange::resultParse(const string &data)
{
    logDBG( "%s", __PRETTY_FUNCTION__ );

    JsonParser jsonAnswer;

    if(!jsonAnswer.accept( data ))
    {
        logERR( "%s Data accept failed!", __PRETTY_FUNCTION__ );
        return "";
    }

    jsonAnswer = JsonParser::parse( data );
    string result = jsonAnswer.value("result", "");
    logINFO( "%s Result : %s", __PRETTY_FUNCTION__, result.c_str() );

    return result;
}

int HttpsExchange::sendData(string post_data, string send_url, const bool &useAnswer)
{
    int err = 0xFF;

    if (!_hubModeUrl.empty())
    {
       if (!hubModeAddRealUrl(post_data, send_url))
       {
            logERR("%s Error add real_url in JSON!", __PRETTY_FUNCTION__);
            return -1;
       }
    }

    curl_version_info_data *data;
    //data = curl_version_info(CURLVERSION_NOW);
    //qDebug() << "===================" << Qt::endl << *data << Qt::endl << "===================" << Qt::endl;
    string answerTmp;
    //if (isSiteServerResolved())
    {
        lock_guard<mutex> locker(mutexCurl);
        CURL *curl_handle = curl_easy_init();

        if(curl_handle)
        {
            struct curl_slist *clist = NULL;
            setLogDBcode_UTF8();
            logINFO("Message to server (URL: %s)(len = %d): %s", send_url.c_str(), post_data.length(), post_data.c_str());
            setLogDBcode_CP866();

            qDebug() << "*****curl****** Message to server:(URL: "<< send_url.c_str() << ")(len = "<< post_data.length() << "):" << post_data.c_str() << endl;
            // Для отладки CURL
            curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL , 1L);
            curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);

            curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0);

            /* keep-alive idle time to 120 seconds */
            //curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPIDLE, 120L);

            /* interval time between keep-alive probes: 60 seconds */
            //curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPINTVL, 60L);


            curl_easy_setopt(curl_handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

            clist = curl_slist_append(clist, "Content-Type: application/json");
            curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, clist);

            curl_easy_setopt(curl_handle, CURLOPT_USERPWD, "viki:Dms6RcU62YhFxKLPKdwYjQBqaZU7V4Wv");

            curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
            curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 0);

            curl_easy_setopt(curl_handle, CURLOPT_URL, send_url.c_str());
            qDebug() << "*****curl****** send_url = " << send_url.c_str() << endl;
            curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, TIME_OUT_FOR_CURL);

            curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &answerTmp);
            // не проверять SSL сертификат
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
            // не проверять Host SSL сертификата
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);


            // POST- запрос c авторизацией ( происходит получение кукисов )
            curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_data.c_str() );
            curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, post_data.length() );

            CURLcode res = curl_easy_perform(curl_handle);
            curl_slist_free_all(clist);

            //snifferDataIns( send_url, make_pair( post_data, answerTmp) );
            if(res == CURLE_OK)
            {
                setLogDBcode_UTF8();
                logINFO("Message from server: %s", answerTmp.c_str());

                setLogDBcode_CP866();
                qDebug() << "*****curl****** Message from server::" << answerTmp.c_str() << endl;
                err = 0;
            }
            else
            {
                logERR("ERROR from server: (%d) %s", res, curl_easy_strerror(res));
                qDebug() << "*****curl****** ERROR from server :" << res << ". curl_easy_strerror = " << curl_easy_strerror(res) << endl;
            }
            curl_easy_cleanup(curl_handle);
        }
    }

    if (useAnswer)
    {
        answer = answerTmp;
    }
    return err;
}

int HttpsExchange::sendHandshake (Config *configData, std::string &req, std::string &answer)
{
    if(!json)
    {
        logERR( "%s nullptr!", __PRETTY_FUNCTION__ );
        return -1;
    }
    int err = 0xFF;
    string jsonCode;
    string nulStr = "";
    JSONResult jsonRes;

    logINFO("Start sendHandshake");
    setUuid(configData->uuid);
    setVersion(configData->fisgo_version);

    req = getHandshakeJson();
    err = sendRequest(HANDSHAKE, urlHandshake);
    if (err == 0)
    {
        err = 0xFE;
        jsonCode = getAnswer();
        logINFO("HANDSHAKE ANSWER: %s", jsonCode.c_str());
        answer = jsonCode;
        if (nulStr.compare(jsonCode) != 0)
        {
            err = 0xFD;
            if (json->jsonParser(JSON_RESULT, jsonCode, &jsonRes) == 0)
            {
                err = jsonRes.result;
                if (err == 0)
                    isHandshake = true;
            }
        }
    }
    logWARN("END HANDSHAKE WITH RESULT: %d", err);
    return err;
}

int HttpsExchange::sendOFDRegister( JSON::JSON_OFD_REGISTER *json_ofd_register )
{
    logDBG("SEND OFD REGISTER");

    if(json_ofd_register == NULL)
    {
        logERR( "NULL OFD REGISTER POINTER!!!" );
        return -1;
    }

    int err = 0xFF;
    string jsonCode;
    string nulStr = "";

    err = sendRequest( json_ofd_register );

    if (err == 0)
    {
        err = 0xFE;
        jsonCode = getAnswer();
        logINFO("SEND OFD REGISTER JSON IS:(%d) %s",jsonCode.length(), jsonCode.c_str());
        if (nulStr.compare(jsonCode) != 0)
        {
            err = 0xFD;
            err = JSON::jsonOFDRegisterParser( jsonCode, json_ofd_register );
        }
    }

    logDBG("END SEND OFD REGISTER WITH RESULT: %d", err);

    return err;
}

int HttpsExchange::sendOFDRegistrationStatus( JSON::JSON_OFD_REGISTRATION_STATUS *json_ofd_registration_status )
{
    logDBG("SEND OFD REGISTRATION STATUS");

    if(json_ofd_registration_status == NULL)
    {
        logERR( "NULL JSON OFD REG STATUS PONTER!!!" );
        return -1;
    }

    int err = 0xFF;
    string jsonCode;
    string nulStr = "";

    err = sendRequest( json_ofd_registration_status );

    if (err == 0)
    {
        err = 0xFE;
        jsonCode = getAnswer();
        logINFO("SEND OFD REGISTRATION STATUS JSON IS:(%d) %s",jsonCode.length(), jsonCode.c_str());
        if (nulStr.compare(jsonCode) != 0)
        {
            err = 0xFD;
            err = JSON::jsonRegistrationStatusParser( jsonCode, json_ofd_registration_status );
        }
    }

    logDBG("END SEND OFD REGISTRATION STATUS WITH RESULT: %d", err);

    return err;
}

int HttpsExchange::sendOFDSign( JSON::JSON_OFD_SIGN *json_ofd_sign )
{
    logDBG("SEND OFD SIGN");

    if(json_ofd_sign == NULL)
    {
        logERR( "NULL JSON OFD SIGN POINTER!!!" );
        return -1;
    }

    int err = 0xFF;
    string jsonCode;
    string nulStr = "";

    err = sendRequest( json_ofd_sign );

    if (err == 0)
    {
        err = 0xFE;
        jsonCode = getAnswer();
        logINFO("SEND OFD SIGN JSON IS:(%d) %s", jsonCode.length(), jsonCode.c_str());
        if (nulStr.compare(jsonCode) != 0)
        {
            err = 0xFD;
            err = JSON::jsonOfdSignParser( jsonCode, json_ofd_sign );
        }
    }

    logDBG("END SEND OFD SIGN WITH RESULT: %d", err);

    return err;
}

int HttpsExchange::sendShiftDocumentReport (JSON::SHIFT_DOCUMENT_REPORT_JSON *shiftDocumentReportJson)
{
    logDBG("SEND SHIFT DOCUMENT REPORT");

    if(json == nullptr)
    {
        logERR( "%s nullptr!", __PRETTY_FUNCTION__ );
        return -1;
    }

    int err = 0xFF;
    string jsonCode;
    string nulStr = "";

    if (shiftDocumentReportJson == NULL)
    {
        logERR("NULL POINTER!");
        return -1;
    }

    err = sendRequest(shiftDocumentReportJson);

    if (err == 0)
    {
        err = 0xFE;
        jsonCode = getAnswer();
        logINFO("SEND SHIFT DOCUMENT REPORT JSON IS: %s", jsonCode.c_str());
        if (nulStr.compare(jsonCode) != 0)
        {
            err = 0xFD;
            JSONResult jsonRes;
            if (json->jsonParser(JSON_RESULT, jsonCode, &jsonRes) == 0)
            {
                err = jsonRes.result;
            }
        }
    }

    logDBG("END SEND SHIFT DOCUMENT REPORT WITH RESULT: %d", err);

    return err;
}

int HttpsExchange::sendMoneyDocumentReport (JSON::MONEY_DOCUMENT_REPORT_JSON *moneyDocumentReportJson)
{
    logDBG("SEND MONEY DOCUMENT REPORT");

    if(json == nullptr)
    {
        logERR( "%s nullptr!", __PRETTY_FUNCTION__ );
        return -1;
    }

    int err = 0xFF;
    string jsonCode;
    string nulStr = "";

    if (moneyDocumentReportJson == NULL)
    {
        logERR("NULL POINTER!");
        return -1;
    }

    err = sendRequest(moneyDocumentReportJson);

    if (err == 0)
    {
        err = 0xFE;
        jsonCode = getAnswer();
        logINFO("SEND MONEY DOCUMENT REPORT JSON IS: %s", jsonCode.c_str());
        if (nulStr.compare(jsonCode) != 0)
        {
            err = 0xFD;
            JSONResult jsonRes;
            if (json->jsonParser(JSON_RESULT, jsonCode, &jsonRes) == 0)
            {
                err = jsonRes.result;
            }
        }
    }

    logDBG("END SEND MONEY DOCUMENT REPORT WITH RESULT: %d", err);

    return err;
}

int HttpsExchange::sendCashInfoReport(Config *configData, License &license)
{
    int err = 0xFF;
    string jsonCode;
    string nulStr = "";

    if((configData == nullptr) || !json)
    {
        logERR( "%s NULL PTR!", __PRETTY_FUNCTION__ );
        return -1;
    }

    logINFO("Start sendCashInfoReport");

    setUuid(configData->uuid);
    setVersion(configData->fisgo_version);
    setFNNnumber(configData->fs_number);
    setOrganizationName(configData->organization_name);
    setAddress(configData->calculation_address);
    setPlace(configData->calculation_place);
    setInn(configData->organization_inn);
    setKpp(configData->organization_kpp);
    setFfdVer("1.05");
    setFactoryNumber(configData->kkt_plant_num);

    setKktSigns(configData->kkt_signs);
    setKktAddSigns(configData->add_kkt_signs);
    setIsPayingAgent(configData->paying_agent_sign);
    setRegistryNumber(configData->kkt_reg_num);
    setTaxes(configData->tax_systems);
    setIsAutonomic((configData->kkt_mode == 1) ? true : false);
    if (isAutonomic == false)
    {
        setOfdName(configData->ofd_name);
        setOfdServerAddress(configData->ofd_server_address);
        setOfdServerPort(configData->ofd_server_port);
        setOfdInn(configData->ofd_inn);
        setOfdCheckUrl(configData->check_receipt_address);
    }

    err = sendRequest(CASHINFOREPORT, urlCashInfoReport, configData, &license);
    if (err == 0)
    {
        err = 0xFE;
        jsonCode = getAnswer();
        logDBG("sendCashInfoReport JSON _STR_ IS: %s", jsonCode.c_str());
        if (nulStr.compare(jsonCode) != 0)
        {
            err = 0xFD;
            JSONResult jsonRes;
            if (json->jsonParser(JSON_RESULT, jsonCode, &jsonRes) == 0)
            {
                err = jsonRes.result;
            }
        }
    }
    logINFO("End sendCashInfoReport with result: %d", err);
    return err;
}

int HttpsExchange::sendRegistrationReport( Config *cfg, EX_REG_RESULT *reg_result )
{
    //------------------------------------------------------
    logDBG( "SENDING REGISTRATION REPOPRT ..." );
    //------------------------------------------------------
    int err       = 0xFF;
    string      jsonCode;
    jsonCode.    clear();
    if((cfg == nullptr) || (reg_result == nullptr) /*|| (Fg == nullptr)*/)
    {
        logERR( "NULL PTR!!!" );
        return -1;
    }
    //------------------------------------------------------
    if(HttpsExchange::err_reg_res_init( reg_result ) == EXIT_FAILURE)
    {
        logERR( "REGISTRATION RESULT INIT FAILED!!!" );
        return -1;
    }
    //------------------------------------------------------
    setUuid(cfg->uuid);
    setVersion(cfg->fisgo_version);
    setOrganizationName(cfg->organization_name);
    setAddress(cfg->calculation_address);
    setPlace(cfg->calculation_place);
    setInn(cfg->organization_inn);
    setKpp(cfg->organization_kpp);
    setFfdVer("1.05");
    setFactoryNumber(cfg->kkt_plant_num);
    setKktSigns(cfg->kkt_signs);
    setKktAddSigns(cfg->add_kkt_signs);
    setIsPayingAgent(cfg->paying_agent_sign);
    setRegistryNumber(cfg->kkt_reg_num);
    setTaxes(cfg->tax_systems);
    setIsAutonomic((cfg->kkt_mode == 1) ? true : false);

    if(!cfg->recSenderEmail.empty())
    {
        setSenderEmail(cfg->recSenderEmail);
    }

    if (isAutonomic == false)
    {
        setOfdName(cfg->ofd_name);
        setOfdServerAddress(cfg->ofd_server_address);
        setOfdServerPort(cfg->ofd_server_port);
        setOfdInn(cfg->ofd_inn);
        setOfdCheckUrl(cfg->check_receipt_address);
    }
    //------------------------------------------------------
    // Удаление старого отчёта, если есть
    logERR( "%s Remove old registration report", __PRETTY_FUNCTION__ );
    //remove( EX_REG_REP_FILE );
    sync();
    // Сохранение нового отчёта
    logERR( "%s Create new registration report", __PRETTY_FUNCTION__ );
    //Fg->str_to_file( EX_REG_REP_FILE, RegistrationReportJsonCreate( reg_result , cfg) );
    err = sendData(RegistrationReportJsonCreate( reg_result , cfg), urlRegistrationReport);
    //------------------------------------------------------
    if (err == 0)
    {
        jsonCode = getAnswer();
        logDBG("SEND REGISTRATION REPORT JSON IS:(%d) %s", jsonCode.length(), jsonCode.c_str());
        //remove( EX_REG_REP_FILE );
        sync();
    }
    //------------------------------------------------------
    logDBG("END SEND REGISTRATION REPORT WITH RESULT: %d", err);

    return err;
}

int HttpsExchange::sendWhoAmI(string uuid, JSON::JSON_REGISTER& jsonRegister)
{
    logERR("%s Отправка запроса WHO_AM_I ...", __PRETTY_FUNCTION__);

    jsonRegister.response.owner.clear();

    if(uuid.empty())
    {
        logERR("%s Пустой uuid при отправке запроса WHO_AM_I", __PRETTY_FUNCTION__);
        return CAB_CRITICAL_ERROR;
    }
    setUuid(uuid);

    int err = sendRequestWhoAmI();
    if (err == CAB_OK)
    {
        const string jsonCode = getAnswer();
        logINFO("%s Отправка запроса WHO_AM_I (%d) : %s", __PRETTY_FUNCTION__, jsonCode.length(), jsonCode.c_str());
        if (!jsonCode.empty())
        {
            err = JSON::jsonRegCabinetParser(jsonCode, jsonRegister);
            if(err == JSON_NO_ERROR)
            {
                // Определяем подключена ли касса к Кабинету Дримкас
                if(jsonRegister.response.owner.empty() || (jsonRegister.response.owner.compare("null") == 0))
                {
                    logERR("%s Запрос WHO_AM_I выполнен, но в ответе отсутствует владелец!", __PRETTY_FUNCTION__);
                    err = JSON_CRIRICAL_ERROR;
                }
                else
                {
                    logERR("%s Запрос WHO_AM_I выполнен, владелец : %s!", __PRETTY_FUNCTION__, jsonRegister.response.owner.c_str());
                }
            }
        }
        else
        {
            err = JSON_LENGTH_ERR;
        }
    }

    logINFO("%s Запрос WHO_AM_I закончен с результатом: %d!", __PRETTY_FUNCTION__, err);

    return err;
}

int HttpsExchange::sendKKTRegistrInfo(Config *configData, JSONKKTRegInfo *jsonKKTRegistrInfo)
{
    int err = 0xFF;
    string jsonCode;
    string nulStr = "";

    if(!json)
    {
        logERR( "%s nullptr!", __PRETTY_FUNCTION__ );
        return -1;
    }

    logERR("SENDING KKT REGISTER INFO ...");

    setUuid(configData->uuid);

    err = sendRequest(KKTREGISTRINFO, urlKKTRegistrInfo);
    if (err == 0)
    {
        err = 0xFE;
        jsonCode = getAnswer();
        logERR("KKT REGISTER INFO FROM HUB: %s", jsonCode.c_str());
        if (nulStr.compare(jsonCode) != 0)
        {
            err = json->jsonParser(JSON_KKT_REGISTR, jsonCode, jsonKKTRegistrInfo);
            if (err == 0)
            {
                err = 0xFD;
                if (jsonKKTRegistrInfo->result == 0)
                {
                    err = 0;
                }
            }
        }
    }

    logDBG("End sendKKTRegistrInfo with result: %d", err);
    return err;
}

int HttpsExchange::sendPollNew( Config *configData, JSON::JSON_POLL_ *json_poll, std::string &req, std::string &answer )
{
    logINFO("SEND POLL");

    int err = 0xFF;
    if(!json)
    {
        logERR("%s Null ptr", __PRETTY_FUNCTION__);
        return err;
    }

    string jsonCode,
            nulStr;

    nulStr.clear();

    setUuid(configData->uuid);

    // Создание отчёта о выполненных заданиях
    setPollStr( JSON::jsonPollStrCreate( json_poll ) );
    reqestPoll.clear();
    err = sendRequest(POLL, urlPoll);
    req = reqestPoll;

    if(err == CAB_OK)
    {
        err = 0xFE;

        jsonCode = getAnswer();
        answer = jsonCode;

        if(nulStr.compare(jsonCode) != 0)
        {
            err = json->jsonPollParser( jsonCode, json_poll );
        }
    }

    logINFO("End sendPoll with result: %d", err);
    return err;
}

//=============================================================================
int HttpsExchange::sendCountersReport_bios( Config *configData, OPERATION <double> &sale_sums, uint64_t &cash_in_final, REC_SUMS& recSums )
{
    int err = 0xFF;
#ifdef DREAMKAS_F
    return err;
#endif
    string jsonCode;
    string nulStr = "";

    if(!json || (configData == nullptr))
    {
        logERR( "%s nullptr!", __PRETTY_FUNCTION__ );
        return -1;
    }

    logINFO("Start sendCountersReport");

    setUuid(configData->uuid);

    setCrCash     ( sale_sums.cash * 100 ); // Перевод в копейки
    setCrCashless ( sale_sums.card * 100 ); // Перевод в копейки
    setCrMoney    ( cash_in_final  );       // Уже в копейках
    setCrCanceled (recSums.canceled * 100); // Перевод в копейки

    err = sendRequest(COUNTERS_REPORT, urlCountersReport);
    if (err == 0)
    {
        err = 0xFE;
        jsonCode = getAnswer();
        logINFO("sendCountersReport_bios JSON _STR_ IS: %s", jsonCode.c_str());
        if (nulStr.compare(jsonCode) != 0)
        {
            JSONResult jsonRes;
            err = json->jsonParser(JSON_RESULT, jsonCode, &jsonRes);
            if (err == 0)
            {
                err = jsonRes.result;
            }
        }
    }
    logINFO("End sendCountersReport_bios with result: %d", err);
    return err;
}

////=============================================================================
//int HttpsExchange::sendCountersReport(Config *configData, shared_ptr<Counters> counters )
//{
//    int err = 0xFF;

//    string jsonCode;
//    string nulStr = "";

//    if(!json || (configData == nullptr) || (!counters))
//    {
//        logERR( "%s nullptr!", __PRETTY_FUNCTION__ );
//        return -1;
//    }

//    logINFO("Start sendCountersReport");

//    setUuid(configData->uuid);
//    setCrCash(counters->get(CASH));
//    setCrCashless(counters->get(CARD));
//    setCrMoney(counters->get(CASH_IN_FINAL));


//    err = sendRequest(COUNTERS_REPORT, urlCountersReport);
//    if (err == 0)
//    {
//        err = 0xFE;
//        jsonCode = getAnswer();
//        logINFO("sendCountersReport JSON _STR_ IS: %s", jsonCode.c_str());
//        if (nulStr.compare(jsonCode) != 0)
//        {
//            JSONResult jsonRes;
//            err = json->jsonParser(JSON_RESULT, jsonCode, &jsonRes);
//            if (err == 0)
//            {
//                err = jsonRes.result;
//            }
//        }
//    }
//    logINFO("End sendCountersReport with result: %d", err);

//    return err;
//}

// Отправка чека в кабинет
//int HttpsExchange::sendReceiptRequest( Receipt_new *receipt )
//{
//    logINFO( "%s Send purchase document report", __PRETTY_FUNCTION__ );

//    if (( receipt == nullptr ) || !convert || !_taxes || !json)
//    {
//        logERR( "%s Null nullptr", __PRETTY_FUNCTION__ );
//        return PURCHASE_ERR_NULL_PTR;
//    }

//    answer.clear();

//    seconds++;
//    if (seconds > 59)
//    {
//        seconds = 0;
//    }

//    EncodeConvertor ec;
//    int errP = 0;

//    JsonParser jsonReceipt;

//    jsonReceipt["uuid"] = getUuid();

//    JsonParser purchase;
//    if(!receipt->rem_id.empty())
//    {
//        // Внешний чек
//        purchase["rem_id"] = receipt->rem_id;
//    }
//    // наименование покупателя
//    if (!receipt->buyerName.empty())
//    {
//        purchase["legal_entity_name"] = ec.CP866toUTF8(receipt->buyerName);
//    }
//    // ИНН покупателя
//    if (!receipt->buyerInn.empty())
//    {
//        purchase["legal_entity_inn"] = ec.CP866toUTF8(receipt->buyerInn);
//    }

//    if (AmI::instance()->aBus() || AmI::instance()->aCourier())
//    {
//        // Только для режима "Автобус"
//        // Признак платёжного терминала для режима "Автобус"
//        purchase["terminal"] = true;
//        purchase["number"] = receipt->reciept_num;
//    }
//    else
//    {
//        // Только для фискального режима
//        if (receipt->shift_num <= 0)
//        {
//            logERR( "%s Wrong shift num!", __PRETTY_FUNCTION__ );
//            return PURCHASE_ERR_WRONG_SHIFT_NUM;
//        }
//        purchase["shift"]  = receipt->shift_num;

//        if (receipt->reciept_num <= 0)
//        {
//            logERR( "%s Wrong receipt num!", __PRETTY_FUNCTION__ );
//            return PURCHASE_ERR_WRONG_RECEIPT_NUM;
//        }
//        purchase["number"] = receipt->reciept_num;

//        if(!receipt->fs_number.empty())
//        {
//            // Только если у нас есть ФН
//            if(receipt->kkt_reg_num.empty() || (receipt->fd_number == 0) || (receipt->fpd_number == 0))
//            {
//                logERR( "%s Wrong fiscal data!", __PRETTY_FUNCTION__ );
//                return PURCHASE_ERR_WRONG_FISCAL_DATA;
//            }
//            purchase["number_fn"]       = receipt->fs_number;
//            purchase["registry_number"] = receipt->kkt_reg_num;
//            purchase["number_fd"]       = to_string(receipt->fd_number);
//            purchase["fiscal_sign"]     = to_string(receipt->fpd_number);
//        }
//    }
//    // Для всех режимов
//    purchase["date"] = convert->intDateTimeToStr(receipt->year,
//                                                 receipt->month,
//                                                 receipt->day,
//                                                 receipt->hours,
//                                                 receipt->minutes,
//                                                 receipt->seconds,
//                                                 receipt->timezone);
//    switch(receipt->type)
//    {
//        case Receipt_new::SALE: purchase["type"] = JSON::SALE; break;
//        case Receipt_new::PURHASE: purchase["type"] = JSON::OUTFLOW; break;
//        case Receipt_new::RETURN: purchase["type"] = JSON::REFUND; break;
//        case Receipt_new::RETURN_PURHASE: purchase["type"] = JSON::OUTFLOW_REFUND; break;
//        case Receipt_new::SALE_ANNUL: purchase["type"] = JSON::SALE_ANNUL; break;
//        case Receipt_new::REFUND_ANNUL: purchase["type"] = JSON::REFUND_ANNUL; break;
//        default: logERR( "%s Unknown doc type!", __PRETTY_FUNCTION__ ); return PURCHASE_ERR_UNKNOWN_DOC_TYPE;
//    }
//#ifdef BIOS_RB
//    purchase["total_sum"] = receipt->total_sum_display_get();
//    Receipt_new::Discounts discounts = receipt->getDiscounts();
//    purchase["discount_sum"] = discounts.discountsSum;
//    purchase["sum_without_discounts"] = discounts.sumWithoutDiscounts;
//#else
//    purchase["total_sum"] = receipt->total_sum_receipt_get();
//    purchase["discount_sum"]          = 0;
//    purchase["sum_without_discounts"] = receipt->total_sum_receipt_get();
//#endif
//    purchase["cashier"]["name"]       = ec.CP866toUTF8(receipt->cashier.name);
//    if(!receipt->cashier.inn.empty())
//    {
//        purchase["cashier"]["inn"]        = receipt->cashier.inn;
//    }
//    if (!receipt->cashier.tabNumber.empty())
//    {
//        purchase["cashier"]["tab_number"] = receipt->cashier.tabNumber;
//    }
//    if (!receipt->cashier.password.empty())
//    {
//        purchase["cashier"]["password"] = receipt->cashier.password;
//    }

//    if (!receipt->ticketNumber.empty())
//    {
//        purchase["ticket_num"] = receipt->ticketNumber;
//    }

//    JsonParser positions;

//    if(receipt->positions.empty())
//    {
//        logERR( "%s Positions list is empty!", __PRETTY_FUNCTION__ );
//        return PURCHASE_ERR_EMPTY_POSITIONS_LIST;
//    }

//    for (const Position_new &pos : receipt->positions)
//    {
//        JsonParser position;
//        position["number"] = pos.pos_num;
//        if(pos.quantity == 0)
//        {
//            logERR( "%s Wrong position quantity!", __PRETTY_FUNCTION__ );
//            return PURCHASE_ERR_WRONG_POS_QUANTITY;
//        }
//        bool isScale = (pos.goods_type & GoodsData::GOODS_TYPE::WEIGHT) ? true : false;
//        if(isScale)
//        {
//            position["quantity"] = pos.quantity;
//        }
//        else
//        {
//            uint64_t qty{pos.quantity * Convert::DIVISIOR_THOUSAND};

//#ifdef BIOS_RB // quantity correction for doc from KL(annul doc)
//            if(receipt->type == Receipt_new::SALE_ANNUL)
//                {
//                    qty /= Convert::DIVISIOR_THOUSAND;
//                }
//#endif
//            position["quantity"] = qty;
//        }
//        position["price"] = pos.unit_price;
//        position["total_sum"] = pos.getFinalPrice();
//    #ifdef BIOS_RB
//        position["discount"] = pos.getDiscount();
//    #else
//        position["discount"] = 0;
//    #endif
//        position["tax"]      = _taxes->get_tax_name_for_hub(E_TAXES (pos.tax_number));
//        #ifdef BIOS_RB
//            position["tax_value"] = int(pos.nds_manual * Convert::DIVISIOR_RUBL);
//        #endif
//        // Баркод или артикул товара
//        switch( pos.mode )
//        {
//            case Position_new::FREE_PRICE_MODE:
//            {
//                break;
//            }
//            case Position_new::BARCODE_MODE:
//            {
//                position["barcode"] = Convert::escapingOfQuotes( pos.mode_value );
//                break;
//            }
//            case Position_new::ARTICLE_MODE:
//            {
//                position["article"] = Convert::escapingOfQuotes( pos.mode_value );
//                break;
//            }
//            default:
//            {
//                logERR("%s Unknown mode!!! Available barcode/article/free_price!", __PRETTY_FUNCTION__ );
//                return PURCHASE_ERR_UNKNOWN_POSITION_MODE;
//            }
//        }
//        if ( !pos.dataMatrix.empty() )
//        {
//            position["excise_barcode"] = pos.dataMatrix;
//        }
//        if( !pos.productCodeBS64.empty() )
//        {
//            position["product_code"] = pos.productCodeBS64;
//        }
//        if( !pos.discountRemId.empty() )
//        {
//            JsonParser discounts;
//            JsonParser  discount;
//            discount["rem_id"]    = pos.discountRemId;
//            discount["sum"]       = to_string( pos.discount );
//            discounts.push_back( discount );
//            position["discounts"] = discounts;
//        }
//        if(pos.goods_name.empty())
//        {
//            logERR( "%s Goods name is empty!", __PRETTY_FUNCTION__ );
//            return PURCHASE_ERR_EMPTY_GOODS_NAME;
//        }

//        if(!pos.barcode.empty())
//        {
//            string indexString = "B_" + pos.barcode;
//            position["product"]["index"] = Convert::escapingOfSlash(indexString);
//        }
//        else if(!pos.article.empty())
//        {
//            string indexString = "V_" + pos.article;
//            position["product"]["index"] = Convert::escapingOfSlash(indexString);
//        }
//        else if(pos.goods_name.compare("Товар") != 0)
//        {
//            string indexString = "I_" + getMd5Sum(pos.goods_name);
//            position["product"]["index"] = Convert::escapingOfSlash(indexString);
//        }
//        if(!pos.rem_id.empty())
//        {
//            position["product"]["meta"]["rem_id"] = pos.rem_id;
//        }
//        else
//        {
//            string goods_name = Convert::escapingOfSlash(pos.goods_name);

//            position["product"]["meta"]["name"]  = ec.CP866toUTF8(Convert::escapingOfQuotes(goods_name));
//            position["product"]["meta"]["scale"] = (isScale) ? "true" : "false";
//        }
//        // Заполнение артикулов
//        if ( !pos.article.empty() )
//        {
//            JsonParser articles;
//            for( string article : getSplitStringWithSeparator(pos.article, ',') )
//            {
//                articles.push_back( article );
//            }
//            position["product"]["meta"]["articles"] = articles;
//        }
//        // Заполнение баркодов
//        if ( !pos.barcode.empty() )
//        {
//            JsonParser barcodes;
//            for( string barcode : getSplitStringWithSeparator(pos.barcode, ',') )
//            {
//                barcodes.push_back( barcode );
//            }
//            position["product"]["meta"]["barcodes"] = barcodes;
//        }
//        if (!pos.dep_name.empty())
//        {
//            string dep_name = Convert::escapingOfSlash(pos.dep_name);
//            position["product"]["meta"]["group_name"] = ec.CP866toUTF8(Convert::escapingOfQuotes(dep_name));
//        }
//        position["product"]["meta"]["price"] = pos.unit_price;
//        position["product"]["meta"]["tax"]   = _taxes->get_tax_name_for_hub(E_TAXES (pos.tax_number));

//        // Акциз
//        if (!pos.originCountryCode.empty())
//        {
//            position["product"]["meta"]["origin_country_code"] = ec.CP866toUTF8(pos.originCountryCode);
//        }
//        if (!pos.customEntryNum.empty())
//        {
//            position["product"]["meta"]["custom_entry_num"] = ec.CP866toUTF8(pos.customEntryNum);
//        }
//        if (pos.exciseDuty != 0)
//        {
//            position["product"]["meta"]["excise_duty"] = pos.exciseDuty;
//        }

//        if (pos.alcohol)
//        {
//            position["product"]["meta"]["alcohol"] = pos.alcohol;
//            position["product"]["meta"]["alcohol_meta"]["alc_code"] = pos.alcCode;
//            position["product"]["meta"]["alcohol_meta"]["alc_type_code"] = pos.alcTypeCode;
//            position["product"]["meta"]["alcohol_meta"]["capacity"] = pos.capacity;
//            position["product"]["meta"]["alcohol_meta"]["alc_content"] = pos.alcContent;
//            position["product"]["meta"]["alcohol_meta"]["keg"] = pos.keg;
//        }

//        position["marked"] = pos.marked;

//        switch(pos.goods_type)
//        {
//            case GoodsData::GOODS:
//            {
//                position["product"]["meta"]["type"] = "COUNTABLE";
//                break;
//            }
//            case GoodsData::SERVICE:
//            {
//                position["product"]["meta"]["type"] = "SERVICE";
//                break;
//            }
//            case GoodsData::WEIGHT:
//            {
//                position["product"]["meta"]["type"] = "SCALABLE";
//                break;
//            }
//            case GoodsData::TOBACCO:
//            {
//                position["product"]["meta"]["type"] = "TOBACCO";
//                break;
//            }
//            case GoodsData::ALCOHOL:
//            {
//                position["product"]["meta"]["type"] = "ALCOHOL";
//                break;
//            }
//            case GoodsData::SHOES:
//            {
//                position["product"]["meta"]["type"] = "SHOES";
//                break;
//            }
//            default:
//            {
//                position["product"]["meta"]["type"] = "COUNTABLE";
//                break;
//            }
//        }
//        positions.push_back( position );
//    }

//    purchase["positions"] = positions;

//    JsonParser payments = JsonParser::array();
//    JsonParser payment;
//    if (receipt->rem_id.empty()
//        && !receipt->isPaidCard
//        && !receipt->isPaidCash
//        && (receipt->payment_type != Receipt_new::PAYMENT_ADVANCE)
//        && (receipt->payment_type != Receipt_new::PAYMENT_CREDIT)
//        && (receipt->payment_type != Receipt_new::PAYMENT_EXCHANGE))
//    {
//        logERR( "%s Unknown payment type!", __PRETTY_FUNCTION__ );
//        return PURCHASE_ERR_UNKNOWN_PAYMENT_TYPE;
//    }

//#ifdef BIOS_RB
//    if(receipt->isPaidCash)
//    {
//        logERR( "%s This is CASH pay!", __PRETTY_FUNCTION__ );
//        payment.clear();
//        payment["type"] =  JSON::CASH;
//        payment["sum"]  = to_string( receipt->isPaidCard ? receipt->getPaymentSum(Receipt_new::PAYMENT_CASH) : receipt->total_sum_display_get() );
//        payments.push_back( payment );
//    }
//    if(receipt->isPaidCard)
//    {
//        logERR( "%s This is ELECTRON pay!", __PRETTY_FUNCTION__ );
//        payment.clear();
//        payment["type"] =  JSON::CASHLESS;
//        payment["sum"]  = to_string( receipt->isPaidCash ? receipt->getPaymentSum(Receipt_new::PAYMENT_CASHLESS) : receipt->total_sum_display_get() );
//        payments.push_back( payment );
//    }
//    purchase["payments"] = payments;
//#else
//    // TODO интегрировать payments в оплату из UI
//    if (receipt->rem_id.empty())
//    {
//        // чек пробит из UI
//        if(receipt->isPaidCard && receipt->isPaidCash)
//        {
//            logERR( "%s This type of payments is not available!", __PRETTY_FUNCTION__ );
//            return PURCHASE_ERR_UNAVAILABLE_PAYMENT_TYPE;
//        }
//        payment["type"] = Receipt_new::getPaymentName(static_cast<Receipt_new::PAYMENT_TYPE>(receipt->payment_type));
//        payment["sum"]  = std::to_string(receipt->total_sum_receipt_get());

//        payments.push_back( payment );
//    }
//    else
//    {
//        // внешний чек
//        if (receipt->payments.empty())
//        {
//            logERR("%s pyments is empty!", __FUNCTION__);
//            return PURCHASE_ERR_UNKNOWN_PAYMENT_TYPE;
//        }

//        for (const auto &pay : receipt->payments)
//        {
//            if (pay.sum > 0)
//            {
//                payment["type"] = Receipt_new::getPaymentName(pay.type);
//                payment["sum"] = pay.sum;
//                payments.push_back( payment );
//            }
//        }

//        if (payments.empty())
//        {
//            payment["type"] = Receipt_new::getPaymentName(static_cast<Receipt_new::PAYMENT_TYPE>(receipt->payment_type));
//            payment["sum"] = receipt->total_sum_receipt_get();
//            payments.push_back(payment);
//        }
//    }

//    purchase["payments"] = payments;
//#endif
//    if (!receipt->tel_number.empty())
//    {
//        purchase["customer"]["phone"] = receipt->tel_number;
//    }
//    else if (!receipt->buyer_email.empty())
//    {
//        purchase["customer"]["email"] = receipt->buyer_email;
//    }

//    JsonParser purchases;
//    purchases.push_back( purchase );

//    jsonReceipt["data"]["purchases"] = purchases;

//    string postData = jsonReceipt.dump( JSON::OFFSET_OUTPUT );
//    return sendData(postData, urlReceipt);
//}

string HttpsExchange::getJsonArray(string name, vector<string> values, const char mask)
{
    string rv;
    rv.clear();

    if ((values.empty()) || (name.empty()))
    {
        logERR( "WRONG REQUEST!!!" );
        return rv;
    }

    if (mask & NOT_FIRST_ELEMENT)
    {
        rv += ",";
    }

    rv += "\"" + name + "\":[";

    for ( string value : values )
        rv += "\"" + value + "\",";

    if(!rv.empty())
    {
        rv.pop_back();
    }
    rv += "]";

    return rv;
}

vector<string> HttpsExchange::getSplitStringWithSeparator(string stringValues, char separator)
{
    vector<string> values;

    string value;
    istringstream stream(stringValues);

    while (getline(stream, value, separator))
    {
        values.push_back( value );
    }

    if (values.empty())
    {
        logERR( "VALUES NOT FOUND!!!" );
    }

    return values;
}

string HttpsExchange::getHandshakeJson()
{
    logDBG("%s", __PRETTY_FUNCTION__);
    JsonParser handshake;
    handshake["uuid"]               = getUuid();

    JsonParser out;
    out["handshake"]                = JSON::HANDSHAKE_VER;
    out["purchase_document_report"] = JSON::PURCHASE_DOCUMENT_REPORT_VER;
    out["money_document_report"]    = JSON::MONEY_DOCUMENT_REPORT_VER;
    out["unregister"]               = JSON::UNREGISTER_VER;
    out["shift_document_report"]    = JSON::SHIFT_DOCUMENT_REPORT_VER;
    out["cash_info_report"]         = JSON::CASH_INFO_REPORT_VER;
    out["poll"]                     = JSON::POLL_VER;
    out["kkt_register_info"]        = JSON::KKT_REGISTER_INFO_VER;
    out["register"]                 = JSON::REGISTER_VER;
    out["registration_report"]      = JSON::REGISTRATION_REPORT_VER;
    out["who_am_i"]                 = JSON::WHO_AM_I_VER;
    out["counters_report"]          = JSON::COUNTERS_REPORT_VER;
    out["search_product"]           = JSON::SEARCH_PRODUCT;
    out["reload_products"]          = JSON::RELOAD_PRODUCTS_VER;
    out["request_mrp_tables"]       = JSON::REQUEST_MRP_TABLES_VER;
    handshake["data"]["methods"]["out"] = out;

    JsonParser in;
    in["upsert_product"]            = JSON::UPSERT_PRODUCT_VER;
    in["bind_product"]              = JSON::BIND_PRODUCT_VER;
    in["delete_product"]            = JSON::DELETE_PRODUCT_VER;
    in["upsert_discount"]           = JSON::UPSERT_DISCOUNT_VER;
    in["delete_discount"]           = JSON::DELETE_DISCOUNT_VER;
    in["registration"]              = JSON::REGISTRATION_VER;
    in["external_purchase"]         = JSON::EXTERNAL_PURCHASE;
    in["upsert_cashier"]            = JSON::UPSERT_CASHIER_VER;
    in["delete_cashier"]            = JSON::DELETE_CASHIER_VER;
    in["update_mrp_tables"]         = JSON::UPDATE_MRP_TABLES_VER;
    #ifndef BIOS_RB
        in["key_activation"] = JSON::KEY_ACTIVATION_VER;
        in["purchase_document_result"] = JSON::PURCHASE_DOCUMENT_RES_VER;
    #endif
    handshake["data"]["methods"]["in"] = in;

    string handshakeJson = handshake.dump(JSON::OFFSET_OUTPUT);
    return handshakeJson;
}

//int HttpsExchange::sendReceipt (Config *configData, Receipt_new *receipt)
//{
//    int err = 0xFF;
//    string jsonCode;
//    string nulStr = "";
//    receiptData = receipt;

//    if(!json)
//    {
//        logERR( "%s nullptr!", __PRETTY_FUNCTION__ );
//        return -1;
//    }

//    logINFO("Start sendReceipt");
//    setUuid(configData->uuid);


//	err = sendReceiptRequest( receipt );

//    if (err == 0)
//    {
//        logFD("SENDING: Shift = %u\t FD = %u\t FPD = %u", receipt->shift_num, receipt->fd_number, receipt->fpd_number);
//        err = 0xFE;
//        jsonCode = getAnswer();
//        logINFO("sendReceipt JSON _STR_ IS: %s", jsonCode.c_str());
//        if (nulStr.compare(jsonCode) != 0)
//        {
//            JSONResult jsonRes;
//            err = json->jsonParser(JSON_RESULT, jsonCode, &jsonRes);
//            if (err == 0)
//            {
//                err = jsonRes.result;
//            }
//        }
//        logFD("RESULT = %d",err);
//    }
//    logINFO("End sendReceipt with result: %d", err);
//    return err;
//}

int HttpsExchange::sendUpdate (string url, bool *fisgo_update, JSONUpdate &jsonUpdate, Config *config_data)
{
    int err = 0xFF;
    string jsonCode;
    string nulStr = "{}";
    string nulStr1 = "";

    if(!json)
    {
        logERR( "%s nullptr!", __PRETTY_FUNCTION__ );
        return -1;
    }

    logINFO("Start sendUpdate");
    err = sendRequest(UPDATE, url + fisgo_cur_version, config_data);

    if (err == 0)
    {
        jsonCode = getAnswer();
        if ( (nulStr.compare(jsonCode) != 0) && (nulStr1.compare(jsonCode) != 0) )
        {
            if (json->jsonParser(JSON_UPDATE, jsonCode, &jsonUpdate) == 0)
            {
                string fisgo_update_version;
                fisgo_update_version = jsonUpdate.version;
                logINFO("Update  version: %s", fisgo_update_version.c_str());
                logINFO("Current version: %s", fisgo_cur_version.c_str());
                *fisgo_update = Convert::isDoUpdateCompareVersions(fisgo_cur_version, fisgo_update_version);
                err = 0;
            }
        }
    }
    logINFO("End sendUpdate with result: %d", err);
    return err;
}

//int HttpsExchange::sendSearchProduct(string barcode, Goods &good, Position_new &posNew)
//{
//    int err = 0xFF;
//    string jsonCode;
//    const string NULLSTR = "{}";

//    logINFO("Start sendSearchProduct");
//    setCode( barcode );
//    err = sendRequest(SEARCHPRODUCT, urlSearchProduct);

//    if (err == 0)
//    {
//        jsonCode = getAnswer();
//        if ( (NULLSTR.compare(jsonCode) != 0) && (!jsonCode.empty()) )
//        {
//            JSON::JSON_PRODUCT product;
//            JSON::product_clear( &product );

//            err = JSON::jsonSearchProductParser(jsonCode, product);
//            if ( err == 0 )
//            {
//                int ecErr = 0;
//                EncodeConvertor ec;

//                string name = ec .UTF8toCP866(product.meta.name, &ecErr );
//                string precision = ec .UTF8toCP866(to_string(product.meta.precision), &ecErr );

//                // ----------------------------- GOODS ------------------------------
//                good.p->cur_pos.goods.goods_name = name;
//                good.p->cur_pos.goods.unit_name = precision;
//                good.p->cur_pos.goods.barcode = product.meta.barcode;

//                good.p->cur_pos.set_pos_price( product.meta.price );
//                good.p->cur_pos.goods.goods_type = product.meta.scale ? GoodsData::GOODS_TYPE::WEIGHT : GoodsData::GOODS_TYPE::GOODS;
//                // ------------------------------------------------------------------

//                good.p->cur_pos.goods.tax_number = nds_0;
//                posNew.tax_number = nds_0;

//                // ---------------------------- POS NEW -----------------------------
//                posNew.goods_name = name;
//                posNew.unit_name  = precision;
//                posNew.barcode    = product.meta.barcode;
//                posNew.mode_value = product.meta.barcode;
//                posNew.mode       = Position_new::BARCODE_MODE;

//                posNew.pos_cost_set( product.meta.price );
//                posNew.unit_price = product.meta.price;
//                posNew.goods_type = good.p->cur_pos.goods.goods_type;
//                // ------------------------------------------------------------------
//            }
//            else
//            {
//                err = 0xFF;
//            }
//        }
//    }

//    logINFO("End sendSearchProduct with result: %d", err);

//    return err;
//}

int HttpsExchange::sendRequest (JSON::SHIFT_DOCUMENT_REPORT_JSON *shiftDocumentReportJson)
{
    int err = 0xFF;
    answer = "";

    if (shiftDocumentReportJson == NULL)
    {
        logERR("NULL POINTER!");
        return -1;
    }
        string json = "{"
                              "\"uuid\" : \"" + shiftDocumentReportJson->uuid + "\","
                              "\"data\" : "
                              "{"
                              "\"shifts\" :"
                              "[";
        for (auto shift = shiftDocumentReportJson->shifts.begin(); shift != shiftDocumentReportJson->shifts.end(); ++shift)
        {
            string cashier = Convert::escapingOfQuotes( (*shift).cashier_name );

            json += "{"
                            "\"shift\" :" + to_string((*shift).shift) + ","
                            "\"number\" : " + to_string((*shift).number) + ","
                            // Дата и время (ISO8601) регистрации
                            "\"date\" : " + "\"" + (*shift).date + "\"" + ","
                            "\"type\" : " + "\"" + (*shift).doc_type + "\"" + ","
                            "\"cashier\" : "
                            "{"
                            "\"name\" : \"" + cashier + "\""
                            "},"
                            "\"sum_cash_begin\" : " + to_string((*shift).sum_cash_begin);
            if ((*shift).doc_type.compare(JSON::CLOSE_SHIFT) == 0)
            {
                json += ",";
                json += "\"sum_cash_end\" : " + to_string((*shift).sum_cash_end) + ","
                        "\"count_cash_out\" : " + to_string((*shift).count_cash_out) + ","
                                "\"count_cash_in\" :  " + to_string((*shift).count_cash_in) + ",";
            }
            if (!(*shift).counters.empty())
            {
                json += "\"counters\" : "
                        "[";
                for (auto counter = (*shift).counters.begin(); counter != (*shift).counters.end(); ++counter)
                {
                    json += "{";
                    json += "\"type\" : ";
                    json += "\"";
                    json += (*counter).type + "\"" + ",";
                    json += "\"payments\" : "
                            "[";
                    for (auto payment = (*counter).payments.begin(); payment != (*counter).payments.end(); ++payment)
                    {
                        json += "{";
                        json += "\"type\" : ";
                        json += "\"" + (*payment).type + "\",";
                        json += "\"count\" : ";
                        json += to_string((*payment).count) + ",";
                        json += "\"sum\" : ";
                        json += to_string((*payment).sum);
                        json += "}";
                        if (payment != (*counter).payments.end() - 1)
                            json += ",";
                    }
                    json += "],"
                                    "\"total_sum\" : " + to_string((*counter).total_sum);
                    json += "}";
                    if (counter != (*shift).counters.end() - 1)
                        json += ",";
                }
                json += "]";
            }
            json += "}";
            if (shift != shiftDocumentReportJson->shifts.end() - 1)
                json += ",";
        }
        json += "]"
                "}";
        json += "}";


        EncodeConvertor ec;
        int errP = 0;
        json = ec.CP866toUTF8(json);

        logDBG("MESSEGE TO SERVER: %s", json.c_str());

        err = sendData(json, urlShiftDocumentReport);

    return err;
}

int HttpsExchange::sendRequest( JSON::JSON_OFD_REGISTER *json_ofd_register )
{
    logDBG( "OFD_REGISTER JSON CREATE" );

    if(json_ofd_register == NULL)
    {
        logERR( "NULL OFD REGISTER POINTER!!!" );
        return -1;
    }

    int err    = 0xFF;
        answer =   "";

    string json =  "{";
           json +=      "\"uuid\":\""+json_ofd_register->uuid+"\",";
           json +=      "\"data\":";
           json +=          "{";
           json +=              "\"shop_info\":";
           json +=                  "{";
           // Наименование магазина
           json +=                      "\"shop_name\":\""   + Convert::escapingOfQuotes(json_ofd_register->shop_name)        + "\",";
           // Адрес расчётов
           json +=                      "\"real_address\":\"" + Convert::escapingOfQuotes(json_ofd_register->realAddress)      + "\",";
           // ИНН
           json +=                      "\"inn\":\""         + Convert::escapingOfQuotes(json_ofd_register->organization_inn) + "\",";
           // № ФНС организации
           json +=                      "\"fns_number\":\""       + json_ofd_register->org_fns_num            + "\",";
           // КЭП ПИН
           json +=                      "\"pin\":\""       + json_ofd_register->pin            + "\",";
           // № Телефона
           json +=                      "\"phone\":\""       + json_ofd_register->phone            + "\"";
           json +=                  "},";
           json +=              "\"kkt_info\":";
           json +=                  "{";
           // ЗН ККТ
           json +=                      "\"kkt_factory_number\":\"" + json_ofd_register->kkt_factory_number + "\",";
           // Наименование устройства
           json +=                      "\"kkt_registry_name\":\""  + json_ofd_register->kkt_registry_name  + "\",";
           // ФН
           json +=                      "\"fn_number\":\""          + json_ofd_register->fn_number   +"\"";
           json +=                  "},";
           json +=              "\"kkt_registration_info\":";
           json +=                  "{";
           // СНО

    //
    json +=                      "\"fns_number\":\""       + json_ofd_register->kkt_fns_num            + "\",";
    //json += getJSONStrForTaxes(json_ofd_register->tax_modes);
    //json += ",";

    unsigned char kkt_signs     =     json_ofd_register->kkt_signs,
                  add_kkt_signs = json_ofd_register->add_kkt_signs;
    // Выделяем нужные HUB признаки ККТ
    kkt_signs     &= ( BIOS::RUS::AUTONOMOUS      | BIOS::RUS::ENCRYPTION | BIOS::RUS::SERVICES );
    add_kkt_signs &= ( BIOS::RUS::EXCISABLE_GOODS | BIOS::RUS::GAMBLING   | BIOS::RUS::LOTTERY  );

    // Автономный режим
    json +=                              "\"autonomic\":";
    if (kkt_signs & BIOS::RUS::AUTONOMOUS)
        json +=                                   "true";
    else
        json +=                                  "false";
    kkt_signs &= ~BIOS::RUS::AUTONOMOUS;

    if (   (kkt_signs != 0x00)
           || (add_kkt_signs != 0x00)
           ||  json_ofd_register->paying_agent_sign )
    {
        json +=                                        ",";
        // Режим работы ККТ
        json += getJSONStrForWorkMode( json_ofd_register->kkt_signs,
                                       json_ofd_register->add_kkt_signs,
                                       json_ofd_register->paying_agent_sign );
    }
    json +=                  "}";
    json +=          "}";
    json += "}";

    EncodeConvertor ec;
    int errP = 0;
    json     = ec.CP866toUTF8(json);
    logDBG("MESSAGE TO SERVER: %s", json.c_str());

    err = sendData(json, urlOFDRegister);

    return err;
}
int HttpsExchange::sendRequest( JSON::JSON_OFD_REGISTRATION_STATUS *json_ofd_registration_status )
{
    logDBG( "OFD_REGISTRATION_STATUS JSON CREATE" );

    if(json_ofd_registration_status == NULL)
    {
        logERR( "NULL JSON OFD REG STATUS PONTER!!!" );
        return -1;
    }

    int err    = 0xFF;
    answer =   "";

    string json =  "{";
           json +=      "\"uuid\":\"" + json_ofd_registration_status->uuid +"\"";
           json += "}";

    EncodeConvertor ec;
    int errP = 0;
    json     = ec.CP866toUTF8(json);
    logDBG("MESSAGE TO SERVER: %s", json.c_str());

    err = sendData(json, urlOFDRegistrationStatus);

    return err;
}

//JsonParser HttpsExchange::getJSONForTaxes( unsigned char tax_modes )
//{
//    JsonParser taxModes = JsonParser::array();

//    unsigned char tax_find = TOTAL;
//    while((tax_modes != 0x00) && (tax_find <= PATENT))
//    {
//        if ((tax_modes & tax_find) == tax_find)
//        {
//            taxModes.push_back(getTaxNameForCabinetWithoutEscapingQuotes(tax_find));
//            tax_modes &= ~tax_find;
//        }
//        tax_find = tax_find << 1;
//    }

//    return taxModes;
//}

string HttpsExchange::RegistrationReportJsonCreate( EX_REG_RESULT *reg_result , Config *configData)
{
    JsonParser jsonRegistrationReport;
    //----------------------------------------------------------------
    logDBG( "REGISTRATION REPORT JSON CREATE" );
    //----------------------------------------------------------------
    if((reg_result == nullptr) || (convert == nullptr))
    {
        logERR( "NULL PTR!!!" );
        return jsonRegistrationReport.dump(JSON::OFFSET_OUTPUT);
    }
    //----------------------------------------------------------------
    // Отчёт о регистрации в HUB
    //----------------------------------------------------------------
    // data
    //----------------------------------------------------------------

    //string tmpJSON;
    jsonRegistrationReport["uuid"] = getUuid();

    JsonParser data;
    //----------------------------------------------------------------
    // type
    //----------------------------------------------------------------
    logERR( "%s REGISTRATION TYPE : %d", __PRETTY_FUNCTION__, reg_result->type );
    string regType;
    switch(reg_result->type)
    {
        case JSON::REG_REGISTRATION:
        {
            regType = JSON::REGISTRATION;
            break;
        }
        case JSON::REG_CHANGE_PARAMETERS:
        {
            regType = JSON::CHANGE_PARAMETERS;
            break;
        }
        case JSON::REG_CHANGE_PARAMETERS_AND_FN:
        {
            regType = JSON::CHANGE_PARAMETERS_AND_FN;
            break;
        }
        case JSON::REG_CLOSE_FN:
        {
            regType = JSON::CLOSE_FN;
            break;
        }
        default:
        {
            logERR( "%s UNKNOWN REGISTRATION TYPE : %d", __PRETTY_FUNCTION__, reg_result->type );
            regType = "UNKNOWN REGISTRATION TYPE";
            break;
        }
    }

    data["type"] =  regType;
    //----------------------------------------------------------------
    // Reason (Tinkoff)
    //----------------------------------------------------------------
    if(reg_result->reason != 0)
    {
        data["reason"] = (uint64_t)reg_result->reason;
    }
    //----------------------------------------------------------------
    // date
    //----------------------------------------------------------------
    if(reg_result->err != JSON::EX_REG_SUCCESS)
    {
        Convert::dateTimeToDDate(convert->cur_unixtime_get(),
                                 &reg_result->date.year,
                                 &reg_result->date.month,
                                 &reg_result->date.day,
                                 &reg_result->date.hours,
                                 &reg_result->date.minutes,
                                 &reg_result->date.seconds);
        reg_result->date.time_zone = convert->getTimeZone();
    }
    data["date"] = convert->DateISO8601Create( &reg_result->date, Convert::PREC_S );
    //----------------------------------------------------------------
    // cashier
    //----------------------------------------------------------------

    EncodeConvertor ec;
    int errP = 0;

    JsonParser cashier;
    cashier["name"] = ec.CP866toUTF8(string("Кабинет Дримкас"));
    data["cashier"] = cashier;

    if(reg_result->err != JSON::EX_REG_SUCCESS)
    {
        //----------------------------------------------------------------
        // Отчёт о регистрации в HUB (не успех)
        //----------------------------------------------------------------
        //----------------------------------------------------------------
        // error
        //----------------------------------------------------------------
        JsonParser error;
        error["code"] = reg_result->err_name;
        error["message"] = ec.CP866toUTF8(reg_result->msg);
        data["error"] = error;
        //----------------------------------------------------------------
    }
    else
    {
        //----------------------------------------------------------------
        // Отчёт о регистрации в HUB (успех)
        //----------------------------------------------------------------
        //----------------------------------------------------------------
        // registration_data
        //----------------------------------------------------------------
        JsonParser registrationData;

        registrationData["number_fd"] = reg_result->fd_num;
        registrationData["fiscal_sign"] = reg_result->fpd;
            //----------------------------------------------------------------
            // shop_info
            //----------------------------------------------------------------
            JsonParser shopInfo;

            if (!getOrganizationName().empty())
            {
                shopInfo["legal_name"]   = ec.CP866toUTF8(getOrganizationName());
            }
            if (!getAddress().empty())
            {
                shopInfo["address"]      = ec.CP866toUTF8(getAddress());
            }
            if (!getPlace().empty())
            {
                shopInfo["real_address"] = ec.CP866toUTF8(getPlace());
                shopInfo["shop_name"]    = ec.CP866toUTF8(getPlace());
            }
            if (!getKpp().empty())
            {
                shopInfo["kpp"]          = getKpp();
            }
            if (!getInn().empty())
            {
                shopInfo["inn"]          = getInn();
            }

            registrationData["shop_info"] = shopInfo;

//            //----------------------------------------------------------------
//            // kkt_registration_info
//            //----------------------------------------------------------------

            JsonParser kktRegInfo;

            if (!getRegistryNumber().empty())
            {
                kktRegInfo["registry_number"] = getRegistryNumber();
            }

           // kktRegInfo["tax_modes"] = getJSONForTaxes(getTaxes());
           // kktRegInfo["autonomic"]    = isAutonomic;
            if (!senderEmail.empty())
            {
                kktRegInfo["sender_email"] = senderEmail;
            }

            if (!isAutonomic)
            {
                JsonParser ofdProvider;

                if (!getOfdName().empty())
                {
                    ofdProvider["name"]        = ec.CP866toUTF8(getOfdName());
                }
                if (!getOfdServerAddress().empty())
                {
                    ofdProvider["server_host"] = getOfdServerAddress();
                }
                ofdProvider["server_port"] = getOfdServerPort();
                if (!getOfdInn().empty())
                {
                    ofdProvider["inn"]         = getOfdInn();
                }
                if (!getOfdCheckUrl().empty())
                {
                    ofdProvider["check_url"]   = getOfdCheckUrl();
                }

                kktRegInfo["ofd_provider"] = ofdProvider;
            }

            kktRegInfo["work_mode"] = getWorkModeArray(kktSigns, kktAddSigns, isPayingAgent);

            if (isPayingAgent)
            {
                //kktRegInfo["agents"] = getAgents(configData->agent_mask);
            }
            if (!configData->automatNumber.empty())
            {
                kktRegInfo["automatic_device_number"] = ec.CP866toUTF8(configData->automatNumber);
            }

            registrationData["kkt_registration_info"]    = kktRegInfo;
            //----------------------------------------------------------------
        data["registration_data"] = registrationData;
    //----------------------------------------------------------------
    }

    jsonRegistrationReport["data"] = data;

    //----------------------------------------------------------------
    return jsonRegistrationReport.dump(JSON::OFFSET_OUTPUT);
}

int HttpsExchange::sendRequest(string json )
{
    int err    = 0xFF;
    answer     =   "";
    EncodeConvertor ec;
    int errP = 0;
    json     = ec.CP866toUTF8(json);
    logERR("MESSAGE TO SERVER: %s", json.c_str());

    //-------------------------------------------------------
    err = sendData(json, urlRegistrationReport);
// TODO Добавить обработку ошибок
//    if(err == 0)
//    {
//        return JSON::getAnswerError( answer );
//    }
    //-------------------------------------------------------
    return err;
}

int HttpsExchange::sendRequestWhoAmI()
{
    logDBG( "%s Создание запроса WHO_AM_I", __PRETTY_FUNCTION__ );

    int convertorError = 0;
    EncodeConvertor ec;
    const string REAL_MODEL_NAME = ec.CP866toUTF8(upModelName);

    JsonParser jsonWhoAmI;
    jsonWhoAmI["uuid"] = getUuid();
    jsonWhoAmI["data"]["model"]["model"] = REAL_MODEL_NAME;
    jsonWhoAmI["data"]["model"]["readable_model"] = REAL_MODEL_NAME;

    const string json = jsonWhoAmI.dump(JSON::OFFSET_OUTPUT);
    logDBG("%s Отправка запроса WHO_AM_I : %s", __PRETTY_FUNCTION__, json.c_str());

    answer.clear();
    return sendData(json, urlWhoAmI);
}

int HttpsExchange::sendReloadProducts(const std::string &uuid)
{
    JsonParser data;
    data["uuid"] = uuid;
    const std::string request = data.dump(JSON::OFFSET_OUTPUT);
    logDBG("%s Отправка запроса RELOAD_PRODUCTS : %s", __FUNCTION__, request.c_str());
    answer.clear();
    return sendData(request, urlReloadProducts);
}

int HttpsExchange::sendRequestMRPtables(const std::string &uuid)
{
    JsonParser data;
    data["uuid"] = uuid;
    const std::string request = data.dump(JSON::OFFSET_OUTPUT);
    logDBG("%s Отправка запроса REQUEST_MRP_TABLES : %s", __FUNCTION__, request.c_str());
    answer.clear();
    return sendData(request, urlRequestMRPtables);
}

int HttpsExchange::sendRequest( JSON::JSON_OFD_SIGN *json_ofd_sign )
{
    logDBG( "OFD_SIGN JSON CREATE" );

    if(json_ofd_sign == NULL)
    {
        logERR( "NULL JSON OFD SIGN POINTER!!!" );
        return -1;
    }

    int err    = 0xFF;
    answer =   "";

    string json = "{";
           json +=       "\"uuid\":\"" + json_ofd_sign->uuid + "\",";
           json +=       "\"data\":";
           json +=          "{";
           json +=                  "\"code\":\"" + json_ofd_sign->code +"\"";
           json +=          "}";
           json += "}";

    EncodeConvertor ec;
    int errP = 0;
    json     = ec.CP866toUTF8(json);
    logDBG("MESSAGE TO SERVER: %s", json.c_str());


    err = sendData(json, urlOFDSign);

    return err;
}

int HttpsExchange::sendRequest (JSON::MONEY_DOCUMENT_REPORT_JSON *moneyDocumentReportJson)
{
    int err = 0xFF;
    answer = "";

    if (moneyDocumentReportJson == NULL)
    {
        logERR("NULL POINTER!");
        return -1;
    }

    string json = "{\"uuid\" : \"" + moneyDocumentReportJson->uuid + "\","
                  "\"data\" : { "
                  "\"documents\": "
                  "[";
        for (auto it = moneyDocumentReportJson->documents.begin(); it != moneyDocumentReportJson->documents.end(); ++it)
        {
            string cashier = Convert::escapingOfQuotes(  (*it).cashier_name );

            json += "{"
                            // Номер смены
                            "\"shift\" : " + to_string((*it).shift) + ","
                            // Номер чека
                            "\"number\" : " + to_string((*it).number) + ","
                            // Дата и время (ISO8601) регистрации
                            "\"date\" : " + "\"" + (*it).date + "\"" + ","
                            // Тип документа
                            "\"type\" : " + "\"" + (*it).doc_type + "\"" + ","
                            // Кассир
                            "\"cashier\" : "
                            "{"
                            // Имя
                            "\"name\" : \"" + cashier + "\""
                            "},"
                            // Сумма документа в копейках
                            "\"sum\" : " + to_string((*it).sum) + "}";
            if (it != moneyDocumentReportJson->documents.end() - 1)
                json += ",";
        }
        json += "]"
                "}"
                "}";

        EncodeConvertor ec;
        int errP = 0;
        json = ec.CP866toUTF8(json);
        logDBG("MESSAGE TO SERVER: %s", json.c_str());


        err = sendData(json, urlMoneyDocumentReport);

    return err;
}

int HttpsExchange::err_reg_res_init( EX_REG_RESULT *err_reg_res )
{
    if(err_reg_res == nullptr)
    {
        logERR( "NULL EX_REG_RESULT PTR!!!" );
        return EXIT_FAILURE;
    }
    if(err_reg_res->err == JSON::EX_REG_SUCCESS)
    {
        return EXIT_SUCCESS;
    }
    switch(err_reg_res->err)
    {
        case JSON::EX_REG_CABINET_IS_DISCONNECTED:
        {
            err_reg_res->msg = "Ошибка связи с Кабинетом";
            err_reg_res->err_name = "CABINET_IS_DISCONNECTED";
            break;
        }
        case JSON::EX_REG_KKT_NOT_FISCAL:
        {
            err_reg_res->msg = "ККТ не зарегистрирована";
            err_reg_res->err_name = "KKT_NOT_FISCAL";
            break;
        }
        case JSON::EX_REG_ALREADY_FISCAL:
        {
            err_reg_res->msg = "ККТ уже зарегистрирована";
            err_reg_res->err_name = "KKT_IS_ALREADY_FISCAL";
            break;
        }
        case JSON::EX_REG_FN_CLOSED:
        {
            err_reg_res->msg = "ФН закрыт";
            err_reg_res->err_name = "FN_CLOSED";
            break;
        }
        case JSON::EX_REG_FN_NOT_REGISTERED:
        {
            err_reg_res->msg = "ФН не зарегистрирован";
            err_reg_res->err_name = "FN_NOT_REGISTERED";
            break;
        }
        case JSON::EX_REG_CLOUD_REGISTRATION_STARTED:
        {
            err_reg_res->msg = "Регистрация уже выполняется";
            err_reg_res->err_name = "CLOUD_REGISTRATION_STARTED";
            break;
        }
        case JSON::EX_REG_CFG_ERROR:
        {
            err_reg_res->msg = "Ошибка настроек ККМ";
            err_reg_res->err_name = "CFG_ERROR";
            break;
        }
        case JSON::EX_REG_INFO_GET_ERR:
        {
            err_reg_res->msg = "Ошибка получения данных о регистрациии";
            err_reg_res->err_name = "KKT_REGISTER_INFO_FAILED";
            break;
        }
        case JSON::EX_REG_DATE_ERROR:
        {
            err_reg_res->msg = "Недопустимое расхождение даты ККТ и ККМ";
            err_reg_res->err_name = "DATE_ERROR";
            break;
        }
        case JSON::EX_REG_KKT_REG_NUM_EMPTY:
        {
            err_reg_res->msg = "Не заполнен регистрационный номер ККТ";
            err_reg_res->err_name = "KKT_REG_NUM_EMPTY";
            break;
        }
        case JSON::EX_REG_INVALID_KKT_REG_NUM:
        {
            err_reg_res->msg = "Указан невалидный регистрационный номер ККТ";
            err_reg_res->err_name = "INVALID_KKT_REG_NUM";
            break;
        }
        case JSON::EX_REG_EMPTY_AUTOMAT_NUMBER:
        {
            err_reg_res->msg = "Отсутствует номер автомата";
            err_reg_res->err_name = "AUTOMAT_NUMBER_IS_EMPTY";
            break;
        }
        case JSON::EX_REG_KKT_INN_EMPTY:
        {
            err_reg_res->msg = "Не заполнен ИНН";
            err_reg_res->err_name = "KKT_INN_EMPTY";
            break;
        }
        case JSON::EX_REG_ADDR_EMPTY:
        {
            err_reg_res->msg = "Не заполнен адрес организации";
            err_reg_res->err_name = "ADDR_EMPTY";
            break;
        }
        case JSON::EX_REG_REAL_ADDR_EMPTY:
        {
            err_reg_res->msg = "Не заполнено место расчетов";
            err_reg_res->err_name = "REAL_ADDR_EMPTY";
            break;
        }
        case JSON::EX_REG_FS_ERROR:
        {
            err_reg_res->msg = "Обрыв связи ФН";
            err_reg_res->err_name = "FS_ERROR";
            break;
        }
        case JSON::EX_REG_NOT_AVAILABLE:
        case JSON::EX_REG_KKT_ERR:
        case JSON::EX_REG_FS_CLOSE_ERR:
        {
            err_reg_res->msg = "Ошибка выполнения операции в ККТ";
            err_reg_res->err_name = "FS_ERROR";
            break;
        }
        case JSON::EX_REG_TAX_SYSTEMS_EMPTY:
        {
            err_reg_res->msg = "Отсутствует СНО";
            err_reg_res->err_name = "KKT_REGISTER_INFO_FAILED";
            break;
        }
        case JSON::EX_REG_ORGANIZATION_NAME_EMPTY:
        {
            err_reg_res->msg = "Отсутствует наименование организации";
            err_reg_res->err_name = "KKT_REGISTER_INFO_FAILED";
            break;
        }
        case JSON::EX_REG_SHIFT_IS_OPEN:
        {
            err_reg_res->msg = "Смена открыта";
            err_reg_res->err_name = "SHIFT_IS_OPENED";
            break;
        }
        case JSON::EX_REG_HAS_UNSENT_DOCUMENTS:
        {
            err_reg_res->msg = "Есть неотправленные документы в ОФД";
            err_reg_res->err_name = "HAS_UNSENT_DOCUMENTS";
            break;
        }
        default:
        {
            err_reg_res->msg = "Неизвестная ошибка!";
            err_reg_res->err_name = "UNKNOWN ERROR!";
            break;
        }
    }

    return EXIT_SUCCESS;
}

//JsonParser HttpsExchange::getTaxesArray(unsigned char taxModes)
//{
//    JsonParser taxes = JsonParser::array();

//    unsigned char taxFind = TOTAL;
//    while((taxModes != 0x00) && (taxFind <= PATENT))
//    {
//        if ((taxModes & taxFind) == taxFind)
//        {
//            taxes.push_back(getSnoNameForHub(taxFind));
//            taxModes &= ~taxFind;
//        }
//        taxFind = taxFind << 1;
//    }
//    return taxes;
//}


//string HttpsExchange::getSnoNameForHub(unsigned char taxMode)
//{
//    switch(taxMode)
//    {
//        case TOTAL:
//        {
//            return "DEFAULT";
//        }
//        case SIMPLIFIED_INCOME:
//        {
//            return "SIMPLE";
//        }
//        case SIMPLIFIED_INC_EXP:
//        {
//            return "SIMPLE_WO";
//        }
//        case SINGLE_IMPUTED_INCOME:
//        {
//            return "ENVD";
//        }
//        case UNIFIED_AGRICULTURAL:
//        {
//            return "AGRICULT";
//        }
//        case PATENT:
//        {
//            return "PATENT";
//        }
//        default:
//        {
//            logERR("%s Unknown sno type!", __PRETTY_FUNCTION__);
//            break;
//        }
//    }
//    return "UNKNOWN_SNO";
//}

JsonParser HttpsExchange::getWorkModeArray(unsigned char kktSigns, unsigned char kktAddSigns, bool isPayingAgentSign)
{
    JsonParser workMode = JsonParser::array();

    if((kktSigns == 0x00) && (kktAddSigns == 0x00) && (!isPayingAgentSign))
    {
        logERR("%s WORK MODE IS EMPTY!", __PRETTY_FUNCTION__);
        return workMode;
    }
    //-------------------------------------------------------
    if((kktSigns & BIOS::RUS::ENCRYPTION) == BIOS::RUS::ENCRYPTION)
    {
        workMode.push_back("CIPHER");
    }
    //-------------------------------------------------------
    if((kktSigns & BIOS::RUS::AUTOMATIC) == BIOS::RUS::AUTOMATIC)
    {
        workMode.push_back("AUTOMATIC");
    }
    //-------------------------------------------------------
    if((kktSigns & BIOS::RUS::SERVICES) == BIOS::RUS::SERVICES)
    {
        workMode.push_back("SERVICES");
    }
    //-------------------------------------------------------
    if((kktSigns & BIOS::RUS::BCO) == BIOS::RUS::BCO)
    {
        workMode.push_back("BSO");
    }
    //-------------------------------------------------------
    if((kktSigns & BIOS::RUS::INTERNET) == BIOS::RUS::INTERNET)
    {
        workMode.push_back("INTERNET");
    }
    //-------------------------------------------------------
    if((kktAddSigns & BIOS::RUS::EXCISABLE_GOODS) == BIOS::RUS::EXCISABLE_GOODS)
    {
        workMode.push_back("EXCISE");
    }
    //-------------------------------------------------------
    if((kktAddSigns & BIOS::RUS::GAMBLING) == BIOS::RUS::GAMBLING)
    {
        workMode.push_back("GAMBLING");
    }
    //-------------------------------------------------------
    if((kktAddSigns & BIOS::RUS::LOTTERY) == BIOS::RUS::LOTTERY)
    {
        workMode.push_back("LOTTERY");
    }
    //-------------------------------------------------------
    if (isPayingAgentSign)
    {
        workMode.push_back("PAY_AGENTS");
    }

    return workMode;
}

//JsonParser HttpsExchange::getAgents(unsigned char agent_mask)
//{
//    JsonParser agents;
//    JsonParser agent;
//    if ((agent_mask & BANK_PAYING_AGENT) == BANK_PAYING_AGENT )
//    {
//        agent["type"] = "BANK_PAY_AGENT";
//        agents.push_back( agent );
//        agent.clear();
//    }

//    if ((agent_mask & BANK_PAYING_SUBAGENT) == BANK_PAYING_SUBAGENT )
//    {
//        agent["type"] = "BANK_PAY_SUB_AGENT";
//        agents.push_back( agent );
//        agent.clear();
//    }

//    if ((agent_mask & PAYING_AGENT) == PAYING_AGENT )
//    {
//        agent["type"] = "PAY_AGENT";
//        agents.push_back( agent );
//        agent.clear();
//    }

//    if ((agent_mask & PAYING_SUBAGENT) == PAYING_SUBAGENT )
//    {
//        agent["type"] = "PAY_SUB_AGENT";
//        agents.push_back( agent );
//        agent.clear();
//    }

//    if ((agent_mask & ATTORNEY_AGENT) == ATTORNEY_AGENT )
//    {
//        agent["type"] = "ATTORNEY";
//        agents.push_back( agent );
//        agent.clear();
//    }

//    if ((agent_mask & COMMISION_AGENT) == COMMISION_AGENT )
//    {
//        agent["type"] = "COMMISSIONAIRE";
//        agents.push_back( agent );
//        agent.clear();
//    }

//    if ((agent_mask & SIMPLE_AGENT) == SIMPLE_AGENT )
//    {
//        agent["type"] = "AGENT";
//        agents.push_back( agent );
//        agent.clear();
//    }

//    return agents;
//}


int HttpsExchange::sendRequest(METHODS reqType, string sendUrl, const Config *configData, License *license)
{
    if(!convert)
    {
        logERR( "%s Null ptr!", __PRETTY_FUNCTION__ );
        return -1;
    }
    int err = 0xFF;
    answer.clear();
    switch(reqType)
    {
        case HANDSHAKE:
        {
            err = sendData(getHandshakeJson(), sendUrl);
            break;
        }
        case POLL:
        {
            string post_data = "{\"uuid\" : \""+ getUuid() +"\",";
                   post_data += "\"data\": {";
                   if(getPollStr().length() > 0)
                   {
                       post_data += getPollStr();
                       setPollStr("");
                   }
                   post_data +=	"}";
                   post_data += "}";
                   reqestPoll = post_data;
             err = sendData(post_data, sendUrl);
            break;
        }
        case COUNTERS_REPORT:
        {
            JsonParser jsonCounters;
            jsonCounters["uuid"]             = getUuid();
            jsonCounters["data"]["cash"]     = getCrCash();
            jsonCounters["data"]["cashless"] = getCrCashless();
            jsonCounters["data"]["receipts"] = getCrCash()+getCrCashless();
            jsonCounters["data"]["balance"]  = getCrMoney();
            #ifdef BIOS_RB
                jsonCounters["data"]["canceled"] = to_string(getCrCanceled());
            #endif
            string post_data = jsonCounters.dump( JSON::OFFSET_OUTPUT );
            err = sendData(post_data, sendUrl);
            break;
        }
        case CASHINFOREPORT:
        {
            if((configData == nullptr) || (license == nullptr))
            {
                logERR("%s Null ptr", __PRETTY_FUNCTION__);
                return -1;
            }

            EncodeConvertor ec;
            int errP = 0;

            JsonParser cashInfoReport;
            cashInfoReport["uuid"] = getUuid();
            JsonParser data;

            JsonParser versions;
            JsonParser versionStruct;
            versionStruct["project"] = "fisgo";
            versionStruct["product"] = product;
            versionStruct["version"] = getVersion();
            versionStruct["type"]    = JSON::CASH;
            versions.push_back(versionStruct);
            data["versions"] = versions;

            if(configData->stage >= FISCAL)
            {
                JsonParser shopInfo;
                JsonParser kktRegistrationInfo;
                shopInfo["legal_name"]            = ec.CP866toUTF8(getOrganizationName());
                #ifndef BIOS_RB
                shopInfo["shop_name"]        = ec.CP866toUTF8(getPlace());
                shopInfo["address"]           = ec.CP866toUTF8(getAddress());
                shopInfo["real_address"]      = ec.CP866toUTF8(getPlace());
                if (!getKpp().empty())
                {
                    shopInfo["kpp"]               = getKpp();
                }

                    kktRegistrationInfo["registry_number"] = getRegistryNumber();
                    //kktRegistrationInfo["tax_modes"]       = getTaxesArray(configData->tax_systems);
                    kktRegistrationInfo["autonomic"]       = isAutonomic;
                    if(!isAutonomic)
                    {
                        JsonParser ofdProvider;

                        ofdProvider["name"]        = ec.CP866toUTF8(getOfdName());
                        ofdProvider["server_host"] = getOfdServerAddress();
                        ofdProvider["server_port"] = getOfdServerPort();
                        ofdProvider["inn"]         = getOfdInn();
                        ofdProvider["check_url"]   = getOfdCheckUrl();
                        kktRegistrationInfo["ofd_provider"] = ofdProvider;
                    }
                    kktRegistrationInfo["work_mode"] = getWorkModeArray(kktSigns, kktAddSigns, isPayingAgent);
//                    if (isPayingAgent)
//                    {
//                        kktRegistrationInfo["agents"] = getAgents(configData->agent_mask);
//                    }

                    if(!configData->recSenderEmail.empty())
                    {
                        kktRegistrationInfo["sender_email"] = ec.CP866toUTF8(configData->recSenderEmail);
                    }
                    if (!configData->automatNumber.empty())
                    {
                        kktRegistrationInfo["automatic_device_number"] = ec.CP866toUTF8(configData->automatNumber);
                    }
                    data["kkt_registration_info"]    = kktRegistrationInfo;
                #endif
                shopInfo["inn"]                   = getInn();
                data["shop_info"]                 = shopInfo;
            }

            JsonParser kktInfo;
            kktInfo["kkt_factory_number"]     = getFactoryNumber();
            kktInfo["kkt_registry_name"]      = ec.CP866toUTF8(KKT_REGISTRY_NAME);
            #ifdef BIOS_RF
                bool isFn = convert->isFnNumberValid(getFNNnumber());
                data["is_fn"] = isFn ? true : false;
                if (isFn)
                {
                    kktInfo["fn_number"]          = configData->fs_number;
                    kktInfo["fn_registered"]      = configData->fnRegistered;
                    kktInfo["fn_registry_name"]   = ec.CP866toUTF8("Шифровальное (криптографическое) средство защиты фискальных данных фискальный накопитель <ФН-1>");
                    if((configData->fs_validity.length() == 6) && (configData->fs_validity.compare( "000000" ) != 0))
                    {
                        string year  = configData->fs_validity.substr(4, 2);
                        string month = configData->fs_validity.substr(2, 2);
                        string day   = configData->fs_validity.substr(0, 2);
                        data["kkt_expire_date"] = "20" + year + "-" + month + "-" + day + "T10:01:21";
                    }
                }
                kktInfo["ffd_version"]        = getFfdVer();
            #endif
            JsonParser services = JsonParser::array();
            for(const License::LicenseTag &feature : license->getLicList())
            {
                if(feature.mask == License::ACTIVE)
                {
                    JsonParser service;
                    service["code"]        = feature.tagNum;
                    if(!feature.expireDate.empty())
                    {
                        vector<string> dateFields = Convert::splitString(feature.expireDate, '.');
                        if(dateFields.size() >= 3)
                        {
                            service["expire_date"] = "20" + dateFields.at(2) + "-" + dateFields.at(1) + "-" + dateFields.at(0);
                        }
                    }
                    services.push_back(service);
                }
            }
            kktInfo["license"]["services"] = services;
            data["kkt_info"] = kktInfo;
            data["local_time"] = convert->dateTimeToStr(convert->cur_unixtime_get());
            cashInfoReport["data"] = data;

            string cashInfoReportStr = cashInfoReport.dump(JSON::OFFSET_OUTPUT);

            logDBG("Message to server: %s", cashInfoReportStr.c_str());

            err = sendData(cashInfoReportStr, sendUrl);
            break;
        }
        case KKTREGISTRINFO:
        {
            JsonParser jsonKktRegistration;
            jsonKktRegistration["uuid"] = getUuid();

            string post_data = jsonKktRegistration.dump(JSON::OFFSET_OUTPUT);

            logINFO( "%s Запрос параметров регистрации: %s", __PRETTY_FUNCTION__, post_data.c_str() );
            err = sendData(post_data, sendUrl);
            break;
        }
        case ADDTOCABINET:
        {
            JsonParser jsonAddToCabinet;
            jsonAddToCabinet["uuid"] = getUuid();
            jsonAddToCabinet["data"]["pin"] = getCode();
            jsonAddToCabinet["data"]["model"]["model"] = upModelName;
            jsonAddToCabinet["data"]["model"]["readable_model"] = "Dreamkas F";

            string post_data = jsonAddToCabinet.dump(JSON::OFFSET_OUTPUT);
            err = sendData(post_data, sendUrl);
            if ( err == 0 )
            {
                err = JSON::jsonRegCabinetParser(getAnswer(), registerInCabinetAnswer);
                if(err > 0)
                {
                    logERR("%s Failed to parse jsonRegCabinetParser err = %d, message = '%s'", __func__, err, registerInCabinetAnswer.response.message.c_str());
                }
            }
            else
            {
                logERR("%s Error send ADDTOCABINET", __PRETTY_FUNCTION__);
            }

            break;
        }
        case DELFROMCABINET:
        {
            JsonParser jsonDelFromCabinet;
            jsonDelFromCabinet["uuid"] = getUuid();

            string post_data = jsonDelFromCabinet.dump(JSON::OFFSET_OUTPUT);
            err = sendData(post_data, sendUrl);

            break;
        }
        case FACTORYSTART:
        {
            JsonParser jsonStartFabrication;
            jsonStartFabrication["partNumber"]   = getPartNumber();
            jsonStartFabrication["serialNumber"] = getFactoryNumber();

            string post_data = jsonStartFabrication.dump(JSON::OFFSET_OUTPUT);
            logINFO( "%s Запрос начала производства: %s", __PRETTY_FUNCTION__, post_data.c_str() );

            err = sendData(post_data, sendUrl);
            break;
        }
        case FACTORYFINISH:
        {
            JsonParser jsonStopFabrication;
            jsonStopFabrication["partNumber"]   = getPartNumber();
            jsonStopFabrication["serialNumber"] = getFactoryNumber();

            JsonParser jsonFields;
            jsonFields["DEVICE_UUID"] = getUuid();
            if (!getFNNnumber().empty())
            {
                jsonFields["FN_REG_NUMBER"] = getFNNnumber();
            }

            string mac;
            if ((Fg->strFromFile( WLAN_MAC_ADDR, mac) == FilesFG::FILE_READ_SUCCESS) && convert->isMacValid(mac))
            {
                 jsonFields["WLAN_MAC"] = mac;
            }
            if ((Fg->strFromFile( ETH_MAC_ADDR, mac) == FilesFG::FILE_READ_SUCCESS) && convert->isMacValid(mac))
            {
                 jsonFields["ETH_MAC"] = mac;
            }

            jsonStopFabrication["fields"] = jsonFields;

            string post_data = jsonStopFabrication.dump(JSON::OFFSET_OUTPUT);
            logINFO( "%s Запрос окончания производства: %s", __PRETTY_FUNCTION__, post_data.c_str() );

            err = sendData(post_data, sendUrl);
            break;
        }
        case UPDATE:
        {
            if (!_hubModeUrl.empty())
            {
               string postData;
               if (!hubModeAddRealUrl(postData, sendUrl))
               {
                    logERR("%s Error add real_url in JSON!", __PRETTY_FUNCTION__);
               }
            }
            //if ( isSiteServerResolved() )
            {
                lock_guard<mutex> locker(mutexCurl);
                CURL *curl_handle = curl_easy_init();
                answer.clear();
                if (curl_handle)
                {
                    struct curl_slist *clist = NULL;
                    clist = curl_slist_append(clist, "X-PRODUCT-TOKEN: 6536871b-46b2-4197-a6c9-ee00e448ecb7");

                    if ( configData != nullptr )
                    {
                        string uuidHeader = "uuid: " + configData->uuid;
                        clist = curl_slist_append(clist, uuidHeader.c_str());
                    }

                    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, clist);

                    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL , 1L);
                    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);

                    curl_easy_setopt(curl_handle, CURLOPT_URL, sendUrl.c_str());
                    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, TIME_OUT_FOR_CURL);

                    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
                    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &answer);

                    // не проверять SSL сертификат
                    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
                    // не проверять Host SSL сертификата
                    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

                    CURLcode res = curl_easy_perform(curl_handle);
                    curl_slist_free_all(clist);
                    if (res == CURLE_OK)
                    {
                        logINFO("Message from Update server: %s", answer.c_str());
                        err = 0;
                    }
                    else
                    {
                        logERR("ERROR from Update server: (%d) %s", res, curl_easy_strerror(res));
                    }
                    curl_easy_cleanup(curl_handle);
                }
            }

            break;
        }
        case DOWNLOAD:
        {
            //if ( isSiteServerResolved() )
            {
                lock_guard<mutex> locker(mutexCurl);

                bool needDownload     = true;
                bool needDownloadPart = false;

                //system("mkdir -p /download");

                double contentLen = 0;
                long int downloadedFileSize = 0;

                while ( needDownload )
                {
                    string modeWrite = needDownloadPart ? "ab+" : "wb+";

                    FILE *body_file = fopen(UPDATE_FILE, modeWrite.c_str());
                    if (body_file == nullptr)
                    {
                        logERR("%s:: body_file == nullptr!", __func__);
                        return 1;
                    }

                    CURL *curl_handle = curl_easy_init();
                    answer.clear();
                    if (curl_handle)
                    {
                        struct curl_slist *clist = NULL;
                        clist = curl_slist_append(clist, "X-PRODUCT-TOKEN: 6536871b-46b2-4197-a6c9-ee00e448ecb7");

                        if ( needDownloadPart )
                        {
                            string content = to_string( (int64_t)contentLen + downloadedFileSize );
                            string range = "Range: bytes=" + to_string(downloadedFileSize) + "-" + content;
                            clist = curl_slist_append(clist, range.c_str());
                        }

                        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, clist);

                        curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL , 1L);
                        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);

                        curl_easy_setopt(curl_handle, CURLOPT_URL, sendUrl.c_str());
                        curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, TIME_OUT_FOR_CURL);

                        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
                        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,     body_file);

                        // не проверять SSL сертификат
                        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
                        // не проверять Host SSL сертификата
                        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

                        CURLcode res = curl_easy_perform(curl_handle);
                        curl_slist_free_all(clist);

                        if (res == CURLE_OK)
                        {
                            //curl_easy_cleanup(curl_handle);
                            downloadedFileSize = ftell(body_file);
                            if (downloadedFileSize > 0)
                            {
                                answer = to_string(downloadedFileSize);
                                err = 0;
                            }

                            needDownload = false;

                            logWARN("FileSize from Update server: %s", answer.c_str());
                        }
                        else if ( res == CURLE_PARTIAL_FILE )
                        {
                            curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &contentLen);

                            downloadedFileSize = ftell(body_file);
                            logWARN("File part size: %d   need download: %f", downloadedFileSize, contentLen);

                            needDownload     = true;
                            needDownloadPart = true;
                        }
                        else
                        {
                            needDownload     = false;
                            logERR("ERROR from Update server: (%d) %s", res, curl_easy_strerror(res));
                        }

                        curl_easy_cleanup(curl_handle);
                    }
                    else
                    {
                        needDownload = false;
                        logERR("ERROR from CURL");
                    }

                    fclose(body_file);
                }
            }

            break;
        }
        case GET_URL_LOG:
        {
            //if ( isSiteServerResolved() )
            {
                lock_guard<mutex> locker(mutexCurl);
                CURL *curl_handle = curl_easy_init();
                answer.clear();
                if (curl_handle)
                {
                    struct curl_slist *clist = nullptr;
                    clist = curl_slist_append(clist, "X-PRODUCT-TOKEN: 6536871b-46b2-4197-a6c9-ee00e448ecb7");

                    sendUrl += "?model="    + modelName;
                    sendUrl += "&uuid="     + uuid;
                    sendUrl += "&version="  + fisgo_cur_version;

                    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, clist);

                    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL , 1L);
                    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);

                    curl_easy_setopt(curl_handle, CURLOPT_URL, sendUrl.c_str());
                    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, TIME_OUT_FOR_CURL);

                    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
                    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,        &answer);

                    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
                    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

                    CURLcode res = curl_easy_perform(curl_handle);
                    curl_slist_free_all(clist);
                    if (res == CURLE_OK)
                    {
                        logINFO("Message from log server: %s", answer.c_str());
                        err = 0;
                    }
                    else
                    {
                        logERR("ERROR from log server: (%d) %s", res, curl_easy_strerror(res));
                    }
                    curl_easy_cleanup(curl_handle);
                }
            }

            break;
        }
        case SEARCHPRODUCT:
        {
            if ( code.empty() )
            {
                break;
            }

            string post_data  = "{\"uuid\":\"" + getUuid() + "\",";
                   post_data += "\"data\":{";
                   post_data += "\"barcode\":\"" + getCode() + "\"";
                   post_data +=	"}";
                   post_data += "}";

            err = sendData(post_data, sendUrl);

            break;
        }
        case GET_KEY_FABRICATION:
        {
            if ((configData != nullptr) && !configData->article.empty() && !configData->kkt_plant_num.empty())
            {
                JsonParser jsonGetKeyFabrication;
                jsonGetKeyFabrication["partNumber"] = configData->article;
                jsonGetKeyFabrication["serialNumber"] = configData->kkt_plant_num;

                string post_data = jsonGetKeyFabrication.dump(JSON::OFFSET_OUTPUT);
                logINFO( "%s Запрос ключа при производстве: %s", __PRETTY_FUNCTION__, post_data.c_str() );

                err = sendData(post_data, sendUrl);
            }
            break;
        }
        case MONEY_DOCUMENT:
        case SHIFT_DOCUMENT:
        case REGISTRATION_REPORT_SUCCESS:
        case REGISTRATION_REPORT_FAILED:
        {
            logERR("Unknown type METHODS!!!");
        }
        case GET_URL_IMAGE_UPLOAD:
        {
            if (!_hubModeUrl.empty())
            {
               string postData;
               if (!hubModeAddRealUrl(postData, sendUrl))
               {
                    logERR("%s Error add real_url in JSON!", __PRETTY_FUNCTION__);
               }
            }

            //if ( isSiteServerResolved() )
            {
                lock_guard<mutex> locker(mutexCurl);
                CURL *curl_handle = curl_easy_init();
                answer.clear();
                if (curl_handle)
                {
                    struct curl_slist *clist = nullptr;
                    clist = curl_slist_append(clist, "X-PRODUCT-TOKEN: 6536871b-46b2-4197-a6c9-ee00e448ecb7");

                    if (configData != nullptr)
                    {
                        sendUrl += "?uuid=" + configData->uuid;
                        sendUrl += "&fn_number=" + configData->fs_number;
                    }

                    logINFO("Message to server (URL: %s)", sendUrl.c_str());

                    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, NULL);

                    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL , 1L);
                    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);

                    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");

                    curl_easy_setopt(curl_handle, CURLOPT_URL, sendUrl.c_str());
                    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, TIME_OUT_FOR_CURL);

                    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
                    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,        &answer);

                    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
                    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

                    CURLcode res = curl_easy_perform(curl_handle);
                    curl_slist_free_all(clist);
                    if (res == CURLE_OK)
                    {
                        logINFO("Message from image server: %s", answer.c_str());
                        err = CAB_OK;
                    }
                    else
                    {
                        logERR("ERROR from image server: (%d) %s", res, curl_easy_strerror(res));
                    }
                    curl_easy_cleanup(curl_handle);
                }
            }
            break;
        }
        case GET_URL_IMAGE_DOWNLOAD:
        {
            if (!_hubModeUrl.empty())
            {
               string postData;
               if (!hubModeAddRealUrl(postData, sendUrl))
               {
                    logERR("%s Error add real_url in JSON!", __PRETTY_FUNCTION__);
               }
            }

            //if ( isSiteServerResolved() )
            {
                lock_guard<mutex> locker(mutexCurl);
                CURL *curl_handle = curl_easy_init();
                answer.clear();
                if (curl_handle)
                {
                    if (configData != nullptr)
                    {
                        sendUrl += "?fn_number=" + configData->fs_number;
                    }

                    logINFO("Message to server (URL: %s)", sendUrl.c_str());

                    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, NULL);

                    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL , 1L);
                    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);

                    curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1);

                    curl_easy_setopt(curl_handle, CURLOPT_URL, sendUrl.c_str());
                    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, TIME_OUT_FOR_CURL);

                    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
                    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,        &answer);

                    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
                    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

                    CURLcode res = curl_easy_perform(curl_handle);

                    if (res == CURLE_OK)
                    {
                        logINFO("Ответ от сервера с образом: %s", answer.c_str());
                        err = CAB_OK;
                    }
                    else
                    {
                        logERR("Ошибка получения образа от сервера: (%d) %s", res, curl_easy_strerror(res));
                    }
                    curl_easy_cleanup(curl_handle);
                }
            }
            break;
        }
        default:
        {
            logERR("%s Unknown request type!", __PRETTY_FUNCTION__);
            break;
        }
    }

    return err;
}

//==============================================================================
// Метод для загрузки обновления В ФОНЕ(отдельный мутекс)
int HttpsExchange::downloadInThread (string url)
{
    int err = 0xFF;
    logERR("%s::  Начало загрузки обновления в фоне.", __PRETTY_FUNCTION__ );
    if( url.empty() )
    {
        logERR("%s:: Ошибка, пустая строка URL!", __PRETTY_FUNCTION__);
        return err;
    }
    //if ( isSiteServerResolved() )
    {
        bool     needDownload       =  true;
        bool     needDownloadPart   = false;
        double   contentLen         =     0;
        long int downloadedFileSize =     0;

        //system("mkdir -p /download");


        while ( needDownload )
        {
            string modeWrite = needDownloadPart ? "ab+" : "wb+";

            FILE *body_file = fopen(UPDATE_FILE, modeWrite.c_str());
            if (body_file == nullptr)
            {
                return 1;
            }

            CURL *curl_handle = curl_easy_init();

            if (curl_handle)
            {
                struct curl_slist *clist = NULL;
                clist = curl_slist_append(clist, "X-PRODUCT-TOKEN: 6536871b-46b2-4197-a6c9-ee00e448ecb7");

                if ( needDownloadPart )
                {
                    string content = to_string( static_cast<int64_t>( contentLen ) + downloadedFileSize );
                    string range = "Range: bytes=" + to_string(downloadedFileSize) + "-" + content;
                    clist = curl_slist_append(clist, range.c_str());
                }

                curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, clist);

                curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL , 1L);
                curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);

                curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, TIME_OUT_FOR_CURL);

                curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
                curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,     body_file);

                // не проверять SSL сертификат
                curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
                // не проверять Host SSL сертификата
                curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

                CURLcode res = curl_easy_perform(curl_handle);
                curl_slist_free_all(clist);

                if (res == CURLE_OK)
                {
                    //curl_easy_cleanup(curl_handle);
                    downloadedFileSize = ftell(body_file);
                    if (downloadedFileSize > 0)
                    {
                        answer = to_string(downloadedFileSize);
                        err = 0;
                    }

                    needDownload = false;

                    logINFO("%s:: FileSize from Update server: %s", __PRETTY_FUNCTION__, answer.c_str());
                }
                else if ( res == CURLE_PARTIAL_FILE )
                {
                    curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &contentLen);

                    downloadedFileSize = ftell(body_file);
                    logERR("%s:: File part size: %d   need download: %f", __PRETTY_FUNCTION__, downloadedFileSize, contentLen);

                    needDownload     = true;
                    needDownloadPart = true;
                }
                else
                {
                    needDownload     = false;
                    logERR("%s:: ERROR from Update server: (%d) %s", __PRETTY_FUNCTION__, res, curl_easy_strerror(res));
                }

                curl_easy_cleanup(curl_handle);
            }
            else
            {
                needDownload = false;
                logERR("%s:: ERROR from CURL", __PRETTY_FUNCTION__);
            }

            fclose(body_file);
        }
    }
    logERR("%s::  КОНЕЦ загрузки обновления в фоне.", __PRETTY_FUNCTION__ );
    return err;
}

JSON::STAT_ERR HttpsExchange::sendStatistics(Config *cfg, const License &license, const JSON::UpdateInfo &updateInfo)
{
    logDBG("%s", __PRETTY_FUNCTION__ );

    JSON::STAT_ERR statErr = JSON::STAT_ERR_SUCCESS;

    if(!json || (cfg == nullptr))
    {
        logERR( "%s Null ptr!!!", __PRETTY_FUNCTION__ );
        return JSON::STAT_ERR_NULL_PTR;
    }

    string jsonStatistics;

    statErr = json->jsonCfgCreate( cfg, license, updateInfo, jsonStatistics );

    if( statErr != JSON::STAT_ERR_JSON_COMPLITE)
    {
        logERR( "%s Json create failed!", __PRETTY_FUNCTION__ );
        return statErr;
    }

    if(jsonStatistics.empty())
    {
        logERR("%s Empty Json!", __PRETTY_FUNCTION__);
        return JSON::STAT_ERR_EMPTY_JSON;
    }

    if( sendData( jsonStatistics, urlStatistics ) != 0)
    {
        logERR( "%s Json send failed!", __PRETTY_FUNCTION__ );
        return JSON::STAT_ERR_SEND_FAILED;
    }
    // TODO Подождать тех времён, когда hub начнёт валидировать и отвечать на статистику
//    string result = resultParse(getAnswer());

//    if(result.compare( "OK" ) != 0)
//    {
//        logERR( "%s Error from HUB : %s", __PRETTY_FUNCTION__, result.c_str() );
//        return JSON::STAT_ERR_HUB_ERROR;
//    }

    logINFO("%s Send statistic success", __PRETTY_FUNCTION__ );

    return statErr;
}

int HttpsExchange::analizeAnswer (string jsonCode)
{
    if(!json)
    {
        logERR( "%s nullptr!", __PRETTY_FUNCTION__ );
        return -1;
    }

    JSONResult jsonRes;

    int err = json->jsonParser(JSON_RESULT, jsonCode, &jsonRes);
    if (err == 0)
    {
        logINFO("JSON RESULT IS: %d", jsonRes.result);
    }
    else
    {
        return err;
    }
    return jsonRes.result;
}

//string HttpsExchange::getJSONStrForTaxes(unsigned char tax_modes)
//{
//    string jsonRes;
//    jsonRes.clear();

//    jsonRes +=    "\"tax_modes\":";
//    jsonRes +=                "[";
//    unsigned char tax_find = TOTAL;
//    while((tax_modes != 0x00) && (tax_find <= PATENT))
//    {
//        if ((tax_modes & tax_find) == tax_find)
//        {
//            jsonRes += getTaxNameForCabinet(tax_find);
//            tax_modes &= ~tax_find;
//            if (tax_modes != 0x00)
//                jsonRes += ",";
//        }
//        tax_find = tax_find << 1;
//    }
//    jsonRes +=                "]";
//    return jsonRes;
//}


//string HttpsExchange::getTaxNameForCabinet(unsigned char taxMode)
//{
//    string resStr;
//    resStr.clear();

//    switch(taxMode)
//    {
//        case TOTAL:
//            resStr = "\"DEFAULT\"";
//            break;
//        case SIMPLIFIED_INCOME:
//            resStr = "\"SIMPLE\"";
//            break;
//        case SIMPLIFIED_INC_EXP:
//            resStr = "\"SIMPLE_WO\"";
//            break;
//        case SINGLE_IMPUTED_INCOME:
//            resStr = "\"ENVD\"";
//            break;
//        case UNIFIED_AGRICULTURAL:
//            resStr = "\"AGRICULT\"";
//            break;
//        case PATENT:
//            resStr = "\"PATENT\"";
//            break;
//        default:
//            logERR("UNKNOWN TAX MODE!");
//            break;
//    }

//    return resStr;
//}

//string HttpsExchange::getTaxNameForCabinetWithoutEscapingQuotes(unsigned char taxMode)
//{
//    string resStr;
//    resStr.clear();

//    switch(taxMode)
//    {
//        case TOTAL:
//            resStr = "DEFAULT";
//            break;
//        case SIMPLIFIED_INCOME:
//            resStr = "SIMPLE";
//            break;
//        case SIMPLIFIED_INC_EXP:
//            resStr = "SIMPLE_WO";
//            break;
//        case SINGLE_IMPUTED_INCOME:
//            resStr = "ENVD";
//            break;
//        case UNIFIED_AGRICULTURAL:
//            resStr = "AGRICULT";
//            break;
//        case PATENT:
//            resStr = "PATENT";
//            break;
//        default:
//            logERR("UNKNOWN TAX MODE!");
//            break;
//    }

//    return resStr;
//}

string HttpsExchange::getJSONStrForWorkMode(unsigned char kktSigns,
                                            unsigned char kktAddSigns,
                                            bool isPayingAgentSign     )
{
    string  workMode;
    workMode.clear();
    uint8_t  cnt = 0;
    //-------------------------------------------------------
    if((kktSigns == 0x00) && (kktAddSigns == 0x00) && (!isPayingAgentSign))
    {
        logERR( "WORK MODE EMPTY!" );
        return "\"workMode\":[]";
    }
    //-------------------------------------------------------
    workMode  = "\"work_mode\":";
    workMode += "[";
    //-------------------------------------------------------
    // Основные признаки
    //-------------------------------------------------------
    if((kktSigns & BIOS::RUS::ENCRYPTION) == BIOS::RUS::ENCRYPTION)
    {
        cnt++;
        workMode += "\"CIPHER\",";
    }
    //-------------------------------------------------------
    if((kktSigns & BIOS::RUS::AUTOMATIC) == BIOS::RUS::AUTOMATIC)
    {
        cnt++;
        workMode += "\"AUTOMATIC\",";
    }
    //-------------------------------------------------------
    if((kktSigns & BIOS::RUS::SERVICES) == BIOS::RUS::SERVICES)
    {
        cnt++;
        workMode += "\"SERVICES\",";
    }
    //-------------------------------------------------------
    if((kktSigns & BIOS::RUS::BCO) == BIOS::RUS::BCO)
    {
        cnt++;
        workMode += "\"BSO\",";
    }
    //-------------------------------------------------------
    if((kktSigns & BIOS::RUS::INTERNET) == BIOS::RUS::INTERNET)
    {
        cnt++;
        workMode += "\"INTERNET\",";
    }
    //-------------------------------------------------------
    if((kktAddSigns & BIOS::RUS::EXCISABLE_GOODS) == BIOS::RUS::EXCISABLE_GOODS)
    {
        cnt++;
        workMode += "\"EXCISE\",";
    }
    //-------------------------------------------------------
    if((kktAddSigns & BIOS::RUS::GAMBLING) == BIOS::RUS::GAMBLING)
    {
        cnt++;
        workMode += "\"GAMBLING\",";
    }
    //-------------------------------------------------------
    if((kktAddSigns & BIOS::RUS::LOTTERY) == BIOS::RUS::LOTTERY)
    {
        cnt++;
        workMode += "\"LOTTERY\",";
    }
    //-------------------------------------------------------
    if (isPayingAgentSign)
    {
        cnt++;
        workMode += "\"PAY_AGENTS\",";
    }
    //-------------------------------------------------------
    if( cnt != 0 )
    {
        workMode.pop_back();
    }
    //-------------------------------------------------------
    workMode += "]";
    //-------------------------------------------------------

    return workMode;
}

bool HttpsExchange::sendLogToServer( const JSON::JSON_LOGS &logsData )
{
    bool rc = false;
    const long HHTP_CREATED  = 201;
    const size_t MD5_LEN     =  32;
    const string md5Log      = "md5log";
    const string logsFile    = "logs.tar";
    const string logsTar     = "logs.tar.gz";
    const string  md5Command = "md5sum " + logsTar + " > " + md5Log;

#ifdef DREAMKAS_F
    const string logsCommand = "tar cfz " + logsTar + " *.db *.log *.sqlite3";
#else
    const string logsCommand = "tar cf " + logsFile + " *.db *.log *.sqlite3; gzip " + logsFile;
#endif

//    system( logsCommand.c_str() );
//    system(  md5Command.c_str() );

    string    md5Sum;
    ifstream fileLog;

    fileLog.open( md5Log.c_str() );
    if ( fileLog.is_open() )
    {
        getline( fileLog,  md5Sum);
        md5Sum = md5Sum.substr(0, MD5_LEN);
        fileLog.close();
        remove( md5Log.c_str() );
    }
    else
    {
        remove( md5Log.c_str() );
        logERR("Error open md5log!!!");
        return rc;
    }

    FILE *fdLogsArch = nullptr;
    struct stat file_info;

    if ( stat(logsTar.c_str(), &file_info) != 0 )
    {
        remove(logsTar.c_str());
        logERR("Error stat %s!!!", logsTar.c_str());
        return rc;
    }

    fdLogsArch = fopen(logsTar.c_str(), "rb");
    if (fdLogsArch != nullptr)
    {
        if (!_hubModeUrl.empty())
        {
           if (sendFile(_hubModeUrl + "/upload", "/FisGo/logs.tar.gz") == 0)
           {
               rc = true;
           }
        }
        else// if (isSiteServerResolved())
        {
            lock_guard<mutex> locker(mutexCurl);
            CURL *curl_handle = curl_easy_init();

            if (curl_handle)
            {
                struct curl_slist *clist = nullptr;
                clist = curl_slist_append(clist, "X-PRODUCT-TOKEN: 6536871b-46b2-4197-a6c9-ee00e448ecb7");

                string delTime = "X-Delete-After:" + to_string(logsData.del);
                clist = curl_slist_append(clist, delTime.c_str());

                string etag = "ETag:" + md5Sum;
                clist = curl_slist_append(clist, etag.c_str());

                string urlLogs = logsData.url + "?_ssuk=" + logsData.ssuk;

                curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL ,      1L   );
                curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER,     clist);
                curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true );

                curl_easy_setopt(curl_handle, CURLOPT_URL,            urlLogs.c_str()  );
                curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, TIME_OUT_FOR_CURL);

                curl_easy_setopt(curl_handle, CURLOPT_PUT,              1L                           );
                curl_easy_setopt(curl_handle, CURLOPT_UPLOAD,           1L                           );
                curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION,     FilesFG::readCurlCallback    );
                curl_easy_setopt(curl_handle, CURLOPT_READDATA,         fdLogsArch                   );
                curl_easy_setopt(curl_handle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);

                curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
                curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

                CURLcode res = curl_easy_perform(curl_handle);
                curl_slist_free_all(clist);

                long http_code = 0;
                curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

                if ( (res == CURLE_OK) && (http_code == HHTP_CREATED) )
                {
                    logINFO("Send log to server OK!");
                    rc = true;
                }
                else
                {
                    logERR("ERROR Send log to server: http:%l  curl:%s (%d)", http_code, curl_easy_strerror(res), res);
                }

                curl_easy_cleanup(curl_handle);
            }
        }

        fclose(fdLogsArch);
    }

    remove(logsTar.c_str());

    return rc;
}

int HttpsExchange::uploadImage(const Config &configData)
{
    if (!json)
    {
        logERR("%s Нулевые указатели при получении ссылки для отправки образа!", __PRETTY_FUNCTION__);
        return CAB_CRITICAL_ERROR;
    }

    int err = CAB_OK;
    string jsonCode;
    JSON::JSON_IMAGE imageData;

    err = sendRequest(GET_URL_IMAGE_UPLOAD, urlImage, &configData);
    if (err == CAB_OK)
    {
        jsonCode = getAnswer();

        if (!jsonCode.empty())
        {
            if(!json->jsonImageUploadParser(jsonCode, imageData))
            {
                err = CAB_CRITICAL_ERROR;
                logERR("%s Ошибка парса ответа от сервера", __PRETTY_FUNCTION__);
            }
            if( (err != CAB_OK) || !uploadImageToServer(imageData))
            {
                err = CAB_CRITICAL_ERROR;
                logERR("%s Ошибка отправки образа на сервер", __PRETTY_FUNCTION__);
            }
        }
    }

    return err;
}

bool HttpsExchange::uploadImageToServer(const JSON::JSON_IMAGE &imagesData)
{
    logDBG("%s:: uploadImageToServer start", __func__);
    bool rc = false;
    constexpr long HHTP_CREATED  = 201;
    constexpr size_t MD5_LEN     =  32;

    const std::string fisgoFiles = "/FisGo/configDb.db /FisGo/timestamp";
    const std::string systemFiles = "/etc/localtime";
    const std::string connectFiles = "/pirit_memory_files/";

    const std::string md5Image       = "md5Image";
    const std::string imageName      = "image.tar";
    const std::string cryptImageName = "image.enc";
    const std::string cryptPass      = "kassafpass";

    const std::string   tarCommand = "tar cvf " + imageName + " " + fisgoFiles + " " + systemFiles + " " + connectFiles;
    const std::string cryptCommand = "openssl enc -aes-256-cbc -salt -in " + imageName + " -out " + cryptImageName + " -pass pass:" + cryptPass;
    const std::string   md5Command = "md5sum " + cryptImageName + " > " + md5Image;

//    system(tarCommand.c_str());
//    system(cryptCommand.c_str());
//    system(md5Command.c_str());

    string    md5Sum;
    ifstream fileImage;

    fileImage.open( md5Image.c_str() );
    if ( fileImage.is_open() )
    {
        getline( fileImage,  md5Sum);
        md5Sum = md5Sum.substr(0, MD5_LEN);
        fileImage.close();
        remove( md5Image.c_str() );
    }
    else
    {
        remove( md5Image.c_str() );
        logERR("%s:: Ошибка получения md5sum образа!", __func__);
        return rc;
    }

    FILE *fdImage = nullptr;
    struct stat file_info;

    if ( stat(cryptImageName.c_str(), &file_info) != 0 )
    {
        remove(cryptImageName.c_str());
        logERR("%s:: Ошибка получения stat архива образа!", __func__);
        return rc;
    }
    //--
    logWARN( "uploading file Size = %llu bytes", static_cast<uint64_t>(file_info.st_size) );
    constexpr int64_t MINSIZE = 200;
    if(file_info.st_size < MINSIZE)
    {
        logERR( "%s::  ERROR TOO SMALL FILE!!!!!  file Size = %llu bytes ", __func__, (uint64_t)file_info.st_size );
        remove(cryptImageName.c_str());
    }
    //--

    fdImage = fopen(cryptImageName.c_str(), "rb");
    if (fdImage != nullptr)
    {
        //if (isSiteServerResolved())
        {
            lock_guard<mutex> locker(mutexCurl);
            CURL *curl_handle = curl_easy_init();

            if (curl_handle)
            {
                const string realUrl = imagesData.url + "?_ssuk=" + imagesData.ssuk;
                logWARN("%s:: Отправка образа на : %s ", __func__, realUrl.c_str());

                curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL ,      1L);
                curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST,     NULL);
                curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

                curl_easy_setopt(curl_handle, CURLOPT_URL,            realUrl.c_str());
                curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, TIME_OUT_FOR_CURL);

                curl_easy_setopt(curl_handle, CURLOPT_PUT,              1L                        );
                curl_easy_setopt(curl_handle, CURLOPT_UPLOAD,           1L                        );
                curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION,     FilesFG::readCurlCallback );
                curl_easy_setopt(curl_handle, CURLOPT_READDATA,         fdImage                   );
                curl_easy_setopt(curl_handle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);

                curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
                curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);

                CURLcode res = curl_easy_perform(curl_handle);
                logWARN( "uploaded image to URL:%s", realUrl.c_str() );

                long http_code = 0;
                curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

                if ( (res == CURLE_OK) && (http_code == HHTP_CREATED) )
                {
                    logWARN("%s:: Отправка образа завершена!", __func__);
                    rc = true;
                }
                else
                {
                    logERR("%s:: Ошибка отправки образа: http:%l  curl:%s (%d)", __func__, http_code, curl_easy_strerror(res), res);
                }

                curl_easy_cleanup(curl_handle);
            }
            else
            {
                logERR( "%s:: Ошибка отправки образа: !curl_handle ", __func__ );
            }
        }
//        else
//        {
//            logERR( "%s:: Ошибка отправки образа: !isSiteServerResolved ", __func__ );
//        }

        fclose(fdImage);
    }
    else
    {
        logERR( "%s:: ERROR! fdImage == nullptr ", __func__  );
    }

    remove(imageName.c_str());
    remove(cryptImageName.c_str());

    return rc;
}

bool HttpsExchange::downloadImage(const Config& configData)
{
    if (!json)
    {
        logERR("%s Нулевые указатели при получении ссылки для получения образа!", __PRETTY_FUNCTION__);
        return CAB_CRITICAL_ERROR;
    }

    string jsonCode;
    JSON::JSON_IMAGE imageData;
    int err = sendRequest(GET_URL_IMAGE_DOWNLOAD, urlImage, &configData);

    if (err == CAB_OK)
    {
        jsonCode = getAnswer();

        if (!jsonCode.empty())
        {
            if(!json->jsonImageDownloadParser(jsonCode, imageData))
            {
                err = CAB_CRITICAL_ERROR;
            }

            if(sendRequest(DOWNLOAD, imageData.urlDonwload) != CAB_OK)
            {
                err = CAB_CRITICAL_ERROR;
            }
        }
    }

    return (err == CAB_OK);
}

bool HttpsExchange::sendKeyFabricationRequest(string &key, Config &configData)
{
  logDBG("%s Запрос ключа при производстве", __PRETTY_FUNCTION__);

  key.clear();
  if (sendRequest(GET_KEY_FABRICATION, urlGetKeyFabrication, &configData) == CAB_OK)
  {
      JsonParser result = JsonParser::parse(getAnswer());
      if ((result.count("key") != 0) && result["key"].is_string())
      {
          key = result["key"];
          return true;
      }
  }

  return false;
}

int HttpsExchange::sendLog(Config &configData, string &code)
{
    logDBG("SEND LOG");

    int err = 0xFF;
    string jsonCode;
    const uint32_t waitTime = 200000;
    JSON::JSON_LOGS logsData;

    code.clear();
    setUuid( configData.uuid );

    err = sendRequest( GET_URL_LOG, urlLogs );
    if (err == CAB_OK)
    {
        jsonCode = getAnswer();

        if ( !jsonCode.empty() )
        {
            err = JSON::jsonLogsParser(jsonCode, logsData);

            if (sendLogToServer(logsData))
            {
                code = logsData.id;
                err = 0;
            }
            else
            {
                err = 0xFF;
            }
        }
    }
    else
    {
        usleep( waitTime );
    }

    return err;
}

//подсчет md5 суммы для индексов товаров
//value - строка для подсчета md5
//возвращает строку, представляющую из себя md5 сумму
string HttpsExchange::getMd5Sum( string value )
{
    CMd5Sum md5Sum;

    //вычисление md5
    //md5Sum.update( static_cast<void*>(&value[0]) , value.size() );
    md5Sum.complete();
    char digest[16];
    md5Sum.getHash( static_cast<char*>(digest) );

    //запись в поток
    stringstream ss;
    for(int j=0; j<16; j++)
    {
        uint8_t big    = (digest[j] >> 4) & 0x0F;
        uint8_t little = digest[j] & 0x0F;
        ss << std::hex << (int)big;
        ss << std::hex << (int)little;
    }

    //формирование результата
    string hash(ss.str());
    return hash;
}


bool HttpsExchange::hubModeAddRealUrl(string &postData, string &url)
{
    logINFO("%s\tHUB mode is ON, routing to url\t:\t%s", __PRETTY_FUNCTION__, _hubModeUrl.c_str());
    if (!postData.empty())
    {
        JsonParser json;
        if(!json.accept(postData))
        {
            logERR( "%s Error! ", __PRETTY_FUNCTION__ );
            return false;
        }
        json = JsonParser::parse( postData );
        json["real_url"] = url;
        postData = json.dump( JSON::OFFSET_OUTPUT );
    }

    url = _hubModeUrl + methodNameGet( url );

    return true;
}

string HttpsExchange::getUrlRegistrationReport() const
{
    return urlRegistrationReport;
}

string HttpsExchange::getUrlStatistics() const
{
    return urlStatistics;
}

void HttpsExchange::urlDisplay(string urlName, string url)
{
    logDBG("\t%-30s:%100s", urlName.c_str(), url.c_str());
}

void HttpsExchange::urlsDisplay()
{
    urlDisplay("Hub name",                  hub_name);
    urlDisplay("Update server name",        update_server_name);
    urlDisplay("Statistics",                urlStatistics);
    urlDisplay("Search product",            urlSearchProduct);
    urlDisplay("Logs",                      urlLogs);
    urlDisplay("Update",                    urlUpdate);
    urlDisplay("Image update",              urlImageUpdate);
    urlDisplay("Handshake",                 urlHandshake);
    urlDisplay("Registration report",       urlRegistrationReport);
    urlDisplay("Cash info report",          urlCashInfoReport);
    urlDisplay("KKT registration info",     urlKKTRegistrInfo);
    urlDisplay("Poll",                      urlPoll);
    urlDisplay("Counters report",           urlCountersReport);
    urlDisplay("Receipt",                   urlReceipt);
    urlDisplay("Money document report",     urlMoneyDocumentReport);
    urlDisplay("Shift dcument report",      urlShiftDocumentReport);
    urlDisplay("OFD register",              urlOFDRegister);
    urlDisplay("OFD registration status",   urlOFDRegistrationStatus);
    urlDisplay("OFD sign",                  urlOFDSign);
    urlDisplay("Add to cabinet",            urlAddToCabinet);
    urlDisplay("Del from cabinet",          urlDelFromCabinet);
    urlDisplay("Who am i",                  urlWhoAmI);
    urlDisplay("Reload products",           urlReloadProducts);
    urlDisplay("Request MRP tables",        urlRequestMRPtables);
}

string HttpsExchange::getSenderEmail() const
{
    return senderEmail;
}

void HttpsExchange::setSenderEmail(const string &value)
{
    senderEmail = value;
}

void HttpsExchange::urlsInit(const bool &production)
{
    hub_name = hub;
    update_server_name = update_server;

    if(!production)
    {
        hub_name = hub_beta;
        update_server_name = update_server_beta;
    }

    urlStatistics            = "https://" + hub_name + "." + siteServer + ":443/dreamkasf/stats";
    urlSearchProduct         = "https://" + hub_name + "." + siteServer + "/viki/v1/search_product/v" + to_string(JSON::SEARCH_PRODUCT);
    urlLogs                  = "https://" + hub_name + "." + siteServer + ":443/viki/upload/v1/";
    urlImage                 = "https://" + update_server + "." + siteServer + ":443/v2/projects/fisgo/products/" + product + "/configs";
    urlUpdate                = "https://" + update_server_name + "." + siteServerUpdate + "/v2/projects/fisgo/products/" + product + "/updates/";
    urlImageUpdate           = "https://" + update_server + "." + siteServer + "/v2/projects/fisgo/products/" + "current_image" + "/updates/";
    urlHandshake             = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/handshake/v"+to_string(JSON::HANDSHAKE_VER);
    urlRegistrationReport    = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/registration_report/v"+to_string(JSON::REGISTRATION_REPORT_VER);
    urlCashInfoReport        = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/cash_info_report/v"+to_string(JSON::CASH_INFO_REPORT_VER);
    urlKKTRegistrInfo        = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/kkt_register_info/v"+to_string(JSON::KKT_REGISTER_INFO_VER);
    urlPoll                  = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/poll/v"+to_string(JSON::POLL_VER);
    urlCountersReport        = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/counters_report/v"+to_string(JSON::COUNTERS_REPORT_VER);
    urlReceipt               = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/purchase_document_report/v"+to_string(JSON::PURCHASE_DOCUMENT_REPORT_VER);
    urlMoneyDocumentReport   = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/money_document_report/v"+to_string(JSON::MONEY_DOCUMENT_REPORT_VER);
    urlShiftDocumentReport   = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/shift_document_report/v"+to_string(JSON::SHIFT_DOCUMENT_REPORT_VER);
    urlOFDRegister           = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/ofd_register/v1";
    urlOFDRegistrationStatus = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/ofd_registration_status/v1";
    urlOFDSign               = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/ofd_sign/v1";
    urlAddToCabinet          = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/register/v"+to_string(JSON::REGISTER_VER);
    urlDelFromCabinet        = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/unregister/v"+to_string(JSON::UNREGISTER_VER);
    urlWhoAmI                = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/who_am_i/v" + to_string(JSON::WHO_AM_I_VER);
    urlReloadProducts        = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/reload_products/v" + std::to_string(JSON::RELOAD_PRODUCTS_VER);
    urlRequestMRPtables      = "https://" + hub_name + "." + siteServer + "/" + "dreamkasf" + "/request_mrp_tables/v" + std::to_string(JSON::REQUEST_MRP_TABLES_VER);
    urlGetKeyFabrication     = "http://192.168.247.85:8089/1c/get_activation_key";

    urlsDisplay();
}

int HttpsExchange::sendFile(string url, string filePath)
{
    logDBG("%s", __PRETTY_FUNCTION__);

    CURL *curl;
    struct curl_httppost *formpost   = nullptr;
    struct curl_httppost *lastptr    = nullptr;
    struct curl_slist    *headerlist = nullptr;

    curl_global_init(CURL_GLOBAL_ALL);

    /* Fill in the file upload field */
    logDBG("%s Path :\t\t%s", __PRETTY_FUNCTION__, filePath.c_str());
    vector<string> path = Convert::splitString(filePath, '/');
    if(path.empty())
    {
        logERR("%s Unresolved file path!", __PRETTY_FUNCTION__);
        return -1;
    }
    string fileName = path.at(path.size() - 1);
    logDBG("%s Filename :\t%s", __PRETTY_FUNCTION__, fileName.c_str());

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "name",
                 CURLFORM_COPYCONTENTS, fileName.c_str(),
                 CURLFORM_END);
    /* Fill in the filename field */
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "file",
                 CURLFORM_FILE, filePath.c_str(),
                 CURLFORM_END);
    curl = curl_easy_init();

    CURLcode res = CURLE_FAILED_INIT;
    if(curl)
    {
        /* what URL that receives this POST */
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        if(getLogDBLevel() >= LOG_LEVELS::INFO_L)
        {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        }
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        long http_code = 0;
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

        const long HHTP_CREATED = 200;
        if ( (res == CURLE_OK) && (http_code == HHTP_CREATED) )
        {
            logDBG("%s\t\tsuccess", __PRETTY_FUNCTION__);
        }
        else
        {
            logERR("%s\t\tFailed : http:%l curl:%s (%d)", __PRETTY_FUNCTION__, http_code, curl_easy_strerror(res), res);
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
        /* then cleanup the formpost chain */
        curl_formfree(formpost);
        /* free slist */
        curl_slist_free_all (headerlist);
    }

    return res;
}

int HttpsExchange::sendBuffer(const std::string &url,
                              const std::string &request,
                              const long timeout,
                              std::string &response)
{
    setLogDBcode_UTF8();
    logINFO("%s request:\n%s", __FUNCTION__, request.c_str());
    setLogDBcode_CP866();

    CURL *curl;
    struct curl_httppost *formpost = nullptr;
    struct curl_httppost *lastptr = nullptr;

    curl_global_init(CURL_GLOBAL_ALL);

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "xml_file",
                 CURLFORM_BUFFER, "xml_file",
                 CURLFORM_BUFFERPTR, request.data(),
                 CURLFORM_BUFFERLENGTH, request.size(),
                 CURLFORM_END);

    curl = curl_easy_init();
    CURLcode res = CURLE_FAILED_INIT;
    response.clear();

    if (curl)
    {
        /* what URL that receives this POST */
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        if (res == CURLE_OK)
        {
            setLogDBcode_UTF8();
            logINFO("%s Response:\n%s", __FUNCTION__, response.c_str());
            setLogDBcode_CP866();
        }
        else
        {
            logERR("%s Failed [curl err code : %d] [curl err msg: %s] ", __FUNCTION__, res, curl_easy_strerror(res));
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
        /* then cleanup the formpost chain */
        curl_formfree(formpost);
    }

    return res;
}

bool HttpsExchange::checkWebSite(const std::string &url, const long timeout)
{
    logDBG("%s", __FUNCTION__);

    curl_global_init(CURL_GLOBAL_ALL);
    CURLcode res = CURLE_FAILED_INIT;
    bool rc {false};
    CURL *curl = curl_easy_init();

    if (curl)
    {
        /* what URL that receives this POST */
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);

        if (getLogDBLevel() >= LOG_LEVELS::INFO_L)
        {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        }
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        constexpr long HHTP_CREATED {200};
        long http_code {0};

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        rc = ((res == CURLE_OK) && (http_code == HHTP_CREATED));

        if (rc)
        {
            logINFO("%s\t\tsuccess", __FUNCTION__);
        }
        else
        {
            logERR("%s\t\tFailed : [http_code: %ld] [curl err code : %d] [curl err msg :%s]", __PRETTY_FUNCTION__, http_code, res, curl_easy_strerror(res));
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return rc;
}


#endif
