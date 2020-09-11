/*
 * json.cpp
 */

#define BOOST_SPIRIT_THREADSAFE
#include "json.h"
#include "loggerdaemon.h"
#include "convert.h"
#include "EncodeConvertor.h"
#include "convert.h"
#include "license.h"


// !!!!!!!!!!!!!!!!!!!!
const string    GOOD_TYPE_COUNTABLE    = "COUNTABLE";  // Штучный
// !!!!!!!!!!!!!!!!!!!!


using namespace std;

const string JSON::REGISTRATION             =             "REGISTRATION";
const string JSON::CHANGE_PARAMETERS        =        "CHANGE_PARAMETERS";
const string JSON::CHANGE_PARAMETERS_AND_FN = "CHANGE_PARAMETERS_AND_FN";
const string JSON::CLOSE_FN                 =                 "CLOSE_FN";

const std::string JSON::OPEN_SHIFT = "OPEN_SHIFT";
const std::string JSON::CLOSE_SHIFT = "CLOSE_SHIFT";
const std::string JSON::SALE = "SALE";
const std::string JSON::REFUND = "REFUND";
const std::string JSON::OUTFLOW = "OUTFLOW";
const std::string JSON::OUTFLOW_REFUND = "OUTFLOW_REFUND";
const std::string JSON::SALE_ANNUL = "SALE_ANNUL";
const std::string JSON::REFUND_ANNUL = "REFUND_ANNUL";
const std::string JSON::CASH = "CASH";
const std::string JSON::CASHLESS = "CASHLESS";
const std::string JSON::PREPAID = "PREPAID";
const std::string JSON::CREDIT = "CREDIT";
const std::string JSON::CONSIDERATION = "CONSIDERATION";

const size_t HASH_PRIME_NUMBER = 10007;
const size_t HASH_SECOND_PRIME_NUMBER = 37;
constexpr size_t hashString(const char* str, size_t h = 0)
{
    return !str[h] ? HASH_PRIME_NUMBER : (hashString(str, h + 1) * HASH_SECOND_PRIME_NUMBER) ^ str[h];
}

JSON::JSON_OFD_REGISTER::JSON_OFD_REGISTER()
{
    EncodeConvertor ec;
    int errP = 0;
    kkt_registry_name = ec.CP866toUTF8(KKT_REGISTRY_NAME);
	paying_agent_sign = false;
	tax_modes = 0x00;
	kkt_signs = 0x00;
	add_kkt_signs = 0x00;
}

JSON::JSON_OFD_REGISTER::~JSON_OFD_REGISTER()
{
}

JSON::JSON_OFD_SIGN::JSON_OFD_SIGN()
{
	uuid.clear();
	code.clear();
	HUB_result.clear();
	OFD_result.clear();
	error_message.clear();
}

JSON::JSON_OFD_SIGN::~JSON_OFD_SIGN()
{
}

//std::string as_json_string(boost::property_tree::ptree const& pt)
//{
//    std::ostringstream oss;
//    write_json(oss, pt);
//    return oss.str();
//}

int JSON::jsonPreAnalizeErrors(string jsonCode)
{
    int err = JSON_INCORRECT_ERR;

	//Тест на соблюдение синтаксиса JSON
    if( jsonCode.empty() )
	{
		logERR( "JSON IS EMPTY!" );
		return err;
	}

    JsonParser parser;

    if (!parser.accept(jsonCode))
	{
		logERR( "JSON INCORRECT!" );
		return err;
	}

	int n = jsonCode.find( "<html>", 0 );

	if (n != -1)
	{
		logERR( "NET ERROR!" );
		return NET_ERROR;
	}

	return 0;
}

int JSON::getAnswerError(string jsonResStr)
{
	int iResult = -1;
	logINFO("JSON_RESULT = %s", jsonResStr.c_str());

	if(jsonResStr.compare("OK") == 0) iResult = CAB_OK;
	else if(jsonResStr.compare("NEED_REGISTERED") == 0) iResult = CAB_NEED_REGISTERED;
	else if(jsonResStr.compare("NEED_HANDSHAKE")  == 0) iResult = CAB_NEED_HANDSHAKE;
	else if(jsonResStr.compare("NEED_UPDATE")     == 0) iResult = CAB_NEED_UPDATE;
	else if(jsonResStr.compare("USER_FAILURE")    == 0) iResult = CAB_USER_FAILURE;
	else if(jsonResStr.compare("REQUEST_FAILURE") == 0) iResult = CAB_REQUEST_FAILURE;

	/*
		OK	            Запрос выполнен успешно
	    NEED_REGISTERED	Касса не зарегистрирована
	    NEED_HANDSHAKE	Кассе необходимо выполнить handshake
	    NEED_UPDATE	    Необходимо обновить кассу
	    USER_FAILURE	Ошибка польователя (не правильны пин при регистрации, etc.)
	    REQUEST_FAILURE	Запрос получен сервером, но не может быть обработан (запрос не корректный)
	*/

    logINFO("Result: %d", iResult);
    return iResult;
}

//string JSON::fs_table_JSON( vector<string> *fs_table )
//{
//    logDBG( "FS TABLE JSON CREATE" );

//    string json;
//    json.clear();
//    unsigned fs_id = 1;

//	std::stringstream ifs;
//	boost::property_tree::ptree FS_TAB_TREE;

//	for (auto it = fs_table->begin(); it != fs_table->end(); ++it)
//	{
//		FS_TAB_TREE.put("FS NUMBER #" + to_string(fs_id++), (*it) );
//	}

//	boost::property_tree::write_json(ifs, FS_TAB_TREE);
//	json = ifs.str();

//	return json;
//}

vector<string> JSON::fs_table_get( string JSON )
{
	logDBG( "FS TABLE FROM JSON GET" );

	vector<string> fs_table;
	fs_table.clear();
	string fs_num;
	fs_num = "X";
	unsigned fs_id = 1;

	std::stringstream ifs(JSON);
//	boost::property_tree::ptree propertyTree;
//	boost::property_tree::read_json(ifs, propertyTree);

//	while(fs_num.length() != 0)
//	{
//		fs_num = propertyTree.get<std::string>("FS NUMBER #" + to_string(fs_id++), "");
//		if(fs_num.length() != 0)
//		{
//			fs_table.push_back( fs_num );
//		}
//	}
	return fs_table;
}

//-------------------------------------------------------------


void JSON::task_data_clear(TASK_DATA* task_data)
{
    tasks_clear(&task_data->tasks);
    upsert_products_clear(&task_data->upsert_products);
    delete_products_clear(&task_data->delete_products);
    bind_products_clear(&task_data->bind_products);
    external_purchase_clear(&task_data->external_purchase);
    task_data->external_purchases.clear();
    key_activation_clear(&task_data->key_activation);
    task_data->registrations.clear();
    task_data->purchaseResults.clear();
    task_data->upsertCashiers.clear();
    task_data->deleteCashiers.clear();
    task_data->updateMRPtables.clear();
}

void JSON::task_clear ( JSON_TASK *task )
{
    //logDBG( "TASK CLEAR" );
	if( task == nullptr )
	{
		logERR( "ERROR! TASK CLEAR NULL PTR!" );
		return;
	}

    task->task_id = 0;
    task->type = TASK_UNKNOWN;
    task->status = IN_PROGRESS;
    task->message.clear();
}

void JSON::tasks_clear( vector<JSON_TASK> *tasks )
{
    //logDBG( "TASKS CLEAR" );

    for (auto it = tasks->begin(); it != tasks->end(); ++it)
        task_clear( &(*it) );

    tasks->clear();
}

void JSON::task_result_clear ( JSON_TASK_RESULTS *task_result )
{
    //logDBG( "TASK RESULT CLEAR" );
    if( task_result == nullptr )
    {
        logERR( "ERROR! TASK RESULT CLEAR NULL PTR!" );
        return;
    }

    task_result->task_id = 0;
    task_result->message.clear();
    task_result->result = IN_PROGRESS;
}
void JSON::task_results_clear ( vector<JSON_TASK_RESULTS> *task_results )
{
    //logERR( "TASK RESULTS CLEAR\nTASK RESULTS: %d", task_results->size() );

    for (auto it = task_results->begin(); it != task_results->end(); ++it)
        task_result_clear( &(*it) );

    task_results->clear();
}

void JSON::upsert_product_clear ( JSON_UPSERT_PRODUCT  *upsert_product )
{
    //logDBG( "UPSERT PRODUCT CLEAR" );
    if( upsert_product == nullptr )
    {
        logERR( "ERROR! UPSERT PRODUCT NULL PTR!" );
        return;
    }
    upsert_product->task_id = 0;
    product_clear( &upsert_product->product );

}

void JSON::upsert_products_clear ( vector<JSON_UPSERT_PRODUCT>  *upsert_products )
{
    //logDBG( "UPSERT PRODUCTS CLEAR\nPRODUCTS: %d", upsert_products->size() );

    for (auto it = upsert_products->begin(); it != upsert_products->end(); ++it)
        upsert_product_clear( &(*it) );

    upsert_products->clear();
}

void JSON::key_activation_clear ( JSON_KEY_ACTIVATION  *key_activate)
{
    //logDBG( "ACTIVATE KEY CLEAR" );
    if( key_activate == nullptr )
    {
        logERR( "ERROR! ACTIVATE KEY NULL PTR!" );
        return;
    }
    key_activate->task_id = 0;
    key_activate->rem_id.clear();
    key_activate->key   .clear();
}

void JSON::bind_product_clear ( JSON_BIND_PRODUCT *bind_product )
{
    //logDBG( "BIND PRODUCT CLEAR" );
    if( bind_product == nullptr )
    {
        logERR( "ERROR! BIND PRODUCT NULL PTR!" );
        return;
    }

    bind_product->task_id = 0;
    bind_product->rem_id.clear();
    bind_product->item_id.clear();
}

void JSON::bind_products_clear ( vector<JSON_BIND_PRODUCT> *bind_products )
{
    //logDBG( "BIND PRODUCTS CLEAR\nPRODUCTS: %d", bind_products->size() );

    for (auto it = bind_products->begin(); it != bind_products->end(); ++it)
        bind_product_clear( &(*it) );

    bind_products->clear();
}

void JSON::delete_product_clear ( JSON_DELETE_PRODUCT *delete_product )
{
    //logDBG( "DELETE PRODUCT CLEAR" );
    if( delete_product == nullptr )
    {
        logERR( "ERROR! DELETE PRODUCT NULL PTR!" );
        return;
    }

    delete_product->task_id = 0;
    delete_product->rem_id.clear();
}

void JSON::delete_products_clear ( vector<JSON_DELETE_PRODUCT> *delete_products )
{
    //logDBG( "DELETE PRODUCTS CLEAR\nPRODUCTS: %d", delete_products->size() );

    for (auto it = delete_products->begin(); it != delete_products->end(); ++it)
        delete_product_clear( &(*it) );

    delete_products->clear();
}

void JSON::attributes_clear ( JSON_ATTRIBUTES *attributes )
{
    //logDBG( "ATTRIBUTES CLEAR" );
    if( attributes == nullptr )
    {
        logERR( "ERROR! ATTRIBUTES CLEAR NULL PTR!" );
        return;
    }

    attributes->phone.clear();
    attributes->email.clear();
}

void JSON::taxes_clear ( JSON_TAXES *taxes )
{
    //logDBG( "TAXES CLEAR" );
    if( taxes == nullptr )
    {
        logERR( "ERROR! TAXES CLEAR NULL PTR!" );
        return;
    }

    taxes->nds_18_118 = 0;
    taxes->nds_18 = 0;
    taxes->nds_10_110 = 0;
    taxes->nds_10 = 0;
    taxes->nds_0 = 0;
    taxes->nds_no = 0;
}

void JSON::total_clear ( JSON_TOTAL *total )
{
    //logDBG( "TOTAL CLEAR" );
    if( total == nullptr )
    {
        logERR( "ERROR! TOTAL CLEAR NULL PTR!" );
        return;
    }

    total->total_sum = 0;
    taxes_clear( &total->taxes );
}

void JSON::payment_clear ( JSON_PAYMENT *payment )
{
    //logDBG( "PAYMENT CLEAR" );
    if( payment == nullptr )
    {
        logERR( "ERROR! PAYMENT CLEAR NULL PTR!" );
        return;
    }

    payment->type.clear();
    payment->sum = 0;
    payment->count = 0;
}

void JSON::payments_clear ( vector<JSON_PAYMENT> *payments )
{
    //logDBG( "PAYMENTS CLEAR\nPAYMENTS: %d", payments->size() );

    for (auto it = payments->begin(); it != payments->end(); ++it)
        payment_clear( &(*it) );

    payments->clear();
}

void JSON::position_clear ( JSON_POSITION *position )
{
    //logDBG( "POSITION CLEAR" );
    if( position == nullptr )
    {
        logERR( "ERROR! POSITION CLEAR NULL PTR!" );
        return;
    }

    product_clear( &position->product );
    position->price = 0;
    position->number = 0;
    position->discount = 0;
    position->quantity = 0;
}

void JSON::positions_clear ( vector<JSON_POSITION> *positions )
{
    //logDBG( "POSITIONs CLEAR\nPOSITIONS: %d", positions->size() );


    for (auto it = positions->begin(); it != positions->end(); ++it)
        position_clear( &(*it) );

    positions->clear();
}

void JSON::position_ex_clear(JSON_POSITION_EXTERNAL* position_ex)
{
    if (position_ex != nullptr)
    {
        position_ex->name.clear();
        position_ex->total = 0;
        position_ex->quantity = 0;
        position_ex->price = 0;
        position_ex->type.clear();
        position_ex->tax_sum = 0;
        position_ex->tax.clear();
        position_ex->tags.clear();
        position_ex->remId.clear();
    }
}

void JSON::positions_ex_clear(std::vector<JSON_POSITION_EXTERNAL>* positions_ex)
{
    if (positions_ex != nullptr)
    {
        for (auto& pos : *positions_ex)
        {
            position_ex_clear(&pos);
        }
    }
    positions_ex->clear();
}


void JSON::external_purchase_clear ( JSON_EXTERNAL_PURCHASE *external_purchase )
{
    if( external_purchase == nullptr )
    {
        logERR( "ERROR! EXTERNAL PURCHASE CLEAR NULL PTR!" );
        return;
    }

    external_purchase->rem_id.clear();
    external_purchase->task_id = 0;
    external_purchase->type.clear();
    external_purchase->tax_mode.clear();
    external_purchase->isValid = true;
    payments_clear( &external_purchase->payments );
    positions_ex_clear( &external_purchase->positions_ex );
    attributes_clear( &external_purchase->attributes );
    total_clear( &external_purchase->total );
}

void JSON::json_poll_clear( JSON_POLL_ *json_poll )
{
    if( json_poll == nullptr )
    {
        logERR( "%s:: ERROR! JSON POLL CLEAR NULL PTR!", __PRETTY_FUNCTION__ );
        return;
    }

    task_results_clear( &json_poll->request.task_results );

    json_poll->response.result = IN_PROGRESS;
    tasks_clear( &json_poll->response.tasks );
    task_data_clear( &json_poll->response.task_data );
}

void JSON::json_poll_clear ( JSON_POLL_ *json_poll, int field )
{
    if( json_poll == nullptr )
    {
        logERR( "%s:: ERROR! JSON POLL CLEAR NULL PTR!", __PRETTY_FUNCTION__ );
        return;
    }

    switch(field)
    {
        case POLL_REQUEST:
        {
            task_results_clear( &json_poll->request.task_results );
                break;
        }
        case POLL_RESPONSE:
        {
            json_poll->response.result = IN_PROGRESS;
            tasks_clear( &json_poll->response.tasks );
            task_data_clear( &json_poll->response.task_data );
            break;
        }
        default:
        {
            break;
        }
    }
}

int JSON::product_display( JSON_PRODUCT *product )
{
	logDBG( "-----------------------------------"                                          );
	logDBG( "-----------------------------------"                                          );
	logDBG( "PRODUCT"                                                                      );
	logDBG( FMT_STR,     "NAME",          product->meta.name.c_str()                       );
	logDBG( FMT_STR,     "REM_ID",        product->meta.rem_id.c_str()                     );
	logDBG( FMT_STR,     "INDEX",         product->index.c_str()                           );
	logDBG( FMT_STR,     "BARCODE",       product->meta.barcode.c_str()                    );
	logDBG( FMT_LLU,     "PRICE",         product->meta.price                              );
	logDBG( FMT_LLU,     "NDS",           product->meta.nds                                );
	logDBG( FMT_STR,     "MEASURE",       product->meta.measure.c_str()                    );
	logDBG( FMT_STR,     "ITEM",          product->meta.item.c_str()                       );
	logDBG( FMT_FLOAT,   "PRECISION",     product->meta.precision                          );

	if(product->meta.alcohol)
	{
		logDBG( FMT_STR, "ALCOHOL", "YES"                                                  );
        logDBG( FMT_STR, "ALC_CODE",      product->meta.alcohol_meta.alc_code .c_str()     );
        logDBG( FMT_STR, "ALC_TYPE_CODE", product->meta.alcohol_meta.alc_type_code.c_str() );
        logDBG( FMT_FLOAT, "ALC_CONTENT",   product->meta.alcohol_meta.alc_content   );
        logDBG( FMT_FLOAT, "CAPACITY",      product->meta.alcohol_meta.capacity      );
        logDBG(FMT_STR, "KEG", product->meta.alcohol_meta.keg ? "YES" : "NO");
	}
	else
		logDBG( FMT_STR, "ALCOHOL", "NO"                                                   );
	(product->meta.scale) ? logDBG( FMT_STR, "SCALE", "YES"                                ):
	                        logDBG( FMT_STR, "SCALE", "NO"                                 );

	return 0;
}

void JSON::product_clear( JSON_PRODUCT *product )
{
    logDBG( "PRODUCT CLEAR" );

    product->index.clear();
    meta_clear( &product->meta );
}

void JSON::meta_clear( JSON_META *meta )
{
    logDBG( "META CLEAR" );

    meta->price = 0;
    meta->rem_id.clear();
    meta->scale = false;
    meta->alcohol_meta.alc_code.clear();
    meta->alcohol_meta.alc_type_code.clear();
    meta->alcohol_meta.capacity = .00;
    meta->alcohol_meta.alc_content = .00;
    meta->alcohol_meta.keg = false;
    meta->alcohol = false;
    meta->barcode.clear();
    meta->article.clear();
    meta->precision = 0.0;
    meta->item.clear();
    meta->measure.clear();
    meta->name.clear();
    meta->nds = 0;
	meta->sort_order = 0;
	meta->variety_name.clear();
	meta->parent.parentName.clear();
    meta->parent.parentRemID.clear();
    meta->originCountryCode.clear();
    meta->customEntryNum.clear();
    meta->exciseDuty = 0;
    meta->marked = false;
}

void JSON::upsert_discount_clear(JSON::JSON_UPSERT_DISCOUNT &upsert_discount)
{
    upsert_discount.mode = 1;
    upsert_discount.name.clear();
    upsert_discount.rem_id.clear();
    upsert_discount.end_time   = 0;
    upsert_discount.start_time = 0;

    upsert_discount.result.type = 0;
    upsert_discount.result.value.clear();
}

void JSON::delete_discount_clear(JSON::JSON_DELETE_DISCOUNT &delete_discount)
{
    delete_discount.rem_id.clear();
}

JSON::JSON_TASK* JSON::NextTaskGet(vector<JSON_TASK> *tasks)
{
    for (auto &task : *tasks)
    {
        if ((task.status == IN_PROGRESS) || (task.status == ERROR))
        {
			logINFO( "NEXT TASK IS FOUND!" );
            return &task;
        }
    }

    return nullptr;
}

int JSON::taxes_display( JSON_TAXES *taxes )
{
	logDBG( "-----------------------------------"    );
	logDBG( "-----------------------------------"    );
	logDBG( "TAXES"                                  );
	logDBG( FMT_LLU, "NDS_NO",     taxes->nds_no     );
	logDBG( FMT_LLU, "NDS_0",      taxes->nds_0      );
	logDBG( FMT_LLU, "NDS_10",     taxes->nds_10     );
	logDBG( FMT_LLU, "NDS_18",     taxes->nds_18     );
	logDBG( FMT_LLU, "NDS_10_110", taxes->nds_10_110 );
	logDBG( FMT_LLU, "NDS_18_118", taxes->nds_18_118 );

	return 0;
}

int JSON::attributes_display( JSON_ATTRIBUTES  *attributes )
{
	logDBG( "-----------------------------------"           );
	logDBG( "-----------------------------------"           );
	logDBG( "ATTRIBUTES"                                    );
	logDBG( FMT_STR, "EMAIL ID", attributes->email.c_str()  );
	logDBG( FMT_STR, "PHONE",    attributes->phone.c_str()  );

	return 0;
}

int JSON::position_display( JSON_POSITION *position )
{
	logDBG( "-----------------------------------"   );
	logDBG( "-----------------------------------"   );
	logDBG( "POSITION"                              );
	logDBG( FMT_LLU, "PRICE",    position->price    );
	logDBG( FMT_LLU, "QUANTITY", position->quantity );
	logDBG( FMT_LLU, "DISCOUNT", position->discount );
	logDBG( FMT_LLU, "NUMBER",   position->number   );
	product_display( &position->product             );

	return 0;
}

int JSON::position_ex_display( JSON_POSITION_EXTERNAL *position_ex )
{
    logDBG( "-----------------------------------"   );
    logDBG( "-----------------------------------"   );
    logDBG( "POSITION EXTERNAL"                              );
    logDBG( FMT_STR, "NAME",    position_ex->name.c_str()    );
    logDBG( FMT_STR, "TAX",    position_ex->tax.c_str()    );
    logDBG( FMT_STR, "TYPE",    position_ex->type.c_str()    );
    logDBG( FMT_LLU, "TAX SUM",    position_ex->tax_sum    );
    logDBG( FMT_LLU, "PRICE",    position_ex->price    );
    logDBG( FMT_LLU, "QUANTITY",    position_ex->quantity    );
    logDBG( FMT_LLU, "TOTAL",    position_ex->total    );

    return 0;
}

int JSON::payment_display (JSON_PAYMENT *payment )
{
	logDBG( "-----------------------------------"   );
	logDBG( "-----------------------------------"   );
	logDBG( "PAYMENT"                               );
	logDBG( FMT_STR, "TYPE",  payment->type.c_str() );
	logDBG( FMT_LLU, "COUNT", payment->count        );
	logDBG( FMT_LLU, "SUM",   payment->sum          );

	return 0;
}

int JSON::total_display( JSON_TOTAL *total )
{
	logDBG( FMT_LLU, "TOTAL SUM", total->total_sum   );
	taxes_display( &total->taxes );

	return 0;
}

int JSON::task_display( JSON_TASK *task )
{
	logDBG( "-----------------------------------"                              );
	logDBG( "-----------------------------------"                              );
	logDBG( FMT_LLU, "TASK ID", task->task_id                                  );
    logDBG( FMT_LLU, "TYPE",    task->type                                     );
    (task->status != IN_PROGRESS) ? logDBG( FMT_STR, "STATUS", "COMPLETE"     ):
                                     logDBG( FMT_STR, "STATUS", "NOT COMPLETE" );
	logDBG( FMT_STR, "MESSAGE", task->message.c_str()                          );

    return 0;
}

int JSON::tasks_display( vector<JSON_TASK> *tasks )
{
	logDBG( "***********************************"   );
	logDBG( "***********************************"   );
	logDBG( "TASKS LIST"                            );
    if (tasks->empty())
		logDBG( "NOTHING"                           );
	for (auto it = tasks->begin(); it != tasks->end(); ++it)
		task_display( &(*it) );
	logDBG( "***********************************"   );

    return 0;
}

int JSON::task_display( JSON_BIND_PRODUCT *bind_product )
{
	logDBG( "-----------------------------------"             );
	logDBG( "-----------------------------------"             );
	logDBG( "BIND PRODUCT"                                    );
	logDBG( FMT_LLU, "TASK ID", bind_product->task_id         );
	logDBG( FMT_STR, "REM ID",  bind_product->rem_id.c_str()  );
	logDBG( FMT_STR, "ITEM ID", bind_product->item_id.c_str() );

	return 0;
}

int JSON::tasks_display( vector<JSON_BIND_PRODUCT> *bind_products )
{
	logDBG( "***********************************"   );
	logDBG( "***********************************"   );
	logDBG( "BIND PRODUCTS LIST"                    );
    if (bind_products->empty())
		logDBG( "NOTHING BIND"                      );
	for (auto it = bind_products->begin(); it != bind_products->end(); ++it)
		task_display( &(*it) );
	logDBG( "***********************************"   );

    return 0;
}

int JSON::task_display( JSON_DELETE_PRODUCT *delete_product )
{
	logDBG( "-----------------------------------"               );
	logDBG( "-----------------------------------"               );
	logDBG( "DELETE PRODUCT"                                    );
	logDBG( FMT_LLU, "TASK ID", delete_product->task_id         );
	logDBG( FMT_STR, "REM ID",  delete_product->rem_id.c_str() );

	return 0;
}

int JSON::tasks_display( vector<JSON_DELETE_PRODUCT> *delete_products )
{
	logDBG( "***********************************"   );
	logDBG( "***********************************"   );
	logDBG( "DELETE PRODUCTS LIST"                  );
    if (delete_products->empty())
		logDBG( "NOTHING DELETE"                    );
	for (auto it = delete_products->begin(); it != delete_products->end(); ++it)
		task_display( &(*it) );
	logDBG( "***********************************"   );

    return 0;
}

int JSON::task_display( JSON_UPSERT_PRODUCT *upsert_product )
{
	logDBG( "-----------------------------------"       );
	logDBG( "-----------------------------------"       );
	logDBG( "UPSERT PRODUCT"                            );
	logDBG( FMT_LLU, "TASK ID", upsert_product->task_id );
	product_display( &upsert_product->product           );

	return 0;
}

int JSON::tasks_display( vector<JSON_UPSERT_PRODUCT> *upsert_products )
{
	logDBG( "***********************************"   );
	logDBG( "***********************************"   );
	logDBG( "UPSERT PRODUCTS LIST"                  );
    if (upsert_products->empty())
		logDBG( "NOTHING UPSERT"                    );
	for (auto it = upsert_products->begin(); it != upsert_products->end(); ++it)
		task_display( &(*it) );
	logDBG( "***********************************"   );

    return 0;
}

int JSON::task_display( JSON::JSON_EXTERNAL_PURCHASE *external_purchase )
{
	logDBG( "-----------------------------------"                    );
	logDBG( "-----------------------------------"                    );
	logDBG( "EXTERNAL PURCHASE"                                      );
	logDBG( FMT_LLU, "TASK ID",  external_purchase->task_id          );
	logDBG( FMT_STR, "REM_ID",   external_purchase->rem_id.c_str()   );
	logDBG( FMT_STR, "TYPE",     external_purchase->type.c_str()     );
    logDBG( FMT_STR, "TAX MODE", external_purchase->tax_mode.c_str() );
    logDBG( FMT_STR, "IS VALID", external_purchase->isValid ? "TRUE" : "FALSE" );

	for (auto it = external_purchase->positions_ex.begin(); it != external_purchase->positions_ex.end(); ++it)
	{
		position_ex_display( &(*it) );
	}

	for (auto it = external_purchase->payments.begin(); it != external_purchase->payments.end(); ++it)
	{
	    payment_display( &(*it) );
	}

	attributes_display( &external_purchase->attributes );
	total_display     ( &external_purchase->total      );

	return 0;
}

int JSON::all_tasks_display( TASK_DATA *task_data )
{
	logDBG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"   );
	logDBG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"   );
	logDBG( "ALL TASKS LIST"                );
	tasks_display( &task_data->tasks );
	tasks_display( &task_data->upsert_products );
	tasks_display( &task_data->delete_products );
	tasks_display( &task_data->bind_products );
    task_display( &task_data->external_purchase );
	logDBG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"   );

    return 0;
}

string JSON::jsonPollStrCreate( JSON_POLL_ *json_poll )
{
    logDBG( "POLL REQUEST JSON CREATE" );

    string jsonPollStr;
    jsonPollStr.clear();

    if (!json_poll->request.task_results.empty())
    {
        jsonPollStr = "\"task_results\":[";

        for (const auto &task : json_poll->request.task_results)
        {
            jsonPollStr += "{\"task_id\":";
            jsonPollStr += to_string(task.task_id);
            jsonPollStr += ",";
            jsonPollStr += "\"result\":";
            switch(task.result)
            {
                case JSON::SUCCESS:
                {
                    jsonPollStr += "\"SUCCESS\"";
                    break;
                }
                case JSON::ERROR:
                {
                    jsonPollStr += "\"ERROR\",";
                    jsonPollStr += "\"error_code\":";
                    jsonPollStr += "\"";
                    jsonPollStr += task.message;
                    jsonPollStr += "\",";
                    jsonPollStr += "\"message\":";
                    jsonPollStr += "\"";
                    jsonPollStr += task.message;
                    jsonPollStr += "\"";
                    break;
                }
                case JSON::WARNING:
                {
                    jsonPollStr += "\"WARNING\",";
                    jsonPollStr += "\"error_code\":";
                    jsonPollStr += "\"";
                    jsonPollStr += task.message;
                    jsonPollStr += "\",";
                    jsonPollStr += "\"message\":";
                    jsonPollStr += "\"";
                    jsonPollStr += task.message;
                    jsonPollStr += "\"";
                    break;
                }
                default:
                {
                    logDBG( "UNKNOWN TASK RESULT!" );
                    jsonPollStr += "\"ERROR\",";
                    jsonPollStr += "\"error_code\":";
                    jsonPollStr += "\"";
                    jsonPollStr += task.message;
                    jsonPollStr += "\",";
                    jsonPollStr += "\"message\":";
                    jsonPollStr += "\"";
                    jsonPollStr += task.message;
                    jsonPollStr += "\"";
                    break;
                }
            }
            jsonPollStr += "},";
        }
        if (!jsonPollStr.empty())
        {
            jsonPollStr.pop_back();
        }
        jsonPollStr += "]";
    }

    return jsonPollStr;
}
//===========================================================================================================
//                          Создание структуры заявление облачной регистрации
//===========================================================================================================
int JSON::JSON_OFDRegisterCreate( Config *config, JSON_OFD_REGISTER *jsonOfdRegister )
{
    logDBG( "JSON OFD REGISTER CREATE" );

	if ((config == NULL) || (jsonOfdRegister == NULL))
	{
		logERR( "NULL POINTER!!!" );
		return EXIT_FAILURE;
	}

	jsonOfdRegister->uuid               =                config->uuid;
	jsonOfdRegister->tax_modes          =         config->tax_systems;
	jsonOfdRegister->paying_agent_sign  =   config->paying_agent_sign;
	jsonOfdRegister->add_kkt_signs      =       config->add_kkt_signs;
	jsonOfdRegister->kkt_signs          =           config->kkt_signs;
	jsonOfdRegister->fn_number          =           config->fs_number;

    EncodeConvertor ec;
    int errP = 0;
    jsonOfdRegister->kkt_registry_name = ec.CP866toUTF8(KKT_REGISTRY_NAME);

	string tmp_str;
	tmp_str = Convert::zero_erase( config->kkt_plant_num, Convert::LEFT );

	if((tmp_str.size() == 9) && (tmp_str.at(0) != '0'))
	{
		tmp_str.insert(0, 1, '0');
	}
	jsonOfdRegister->kkt_factory_number =                     tmp_str;
	jsonOfdRegister->phone              =           config->cloud_tel;
	jsonOfdRegister->pin                =           config->cloud_pin;
	jsonOfdRegister->realAddress        = config->calculation_address;
	jsonOfdRegister->shop_name          =   config->calculation_place;
    jsonOfdRegister->org_fns_num        =   config->cloud_org_fns_num;
    jsonOfdRegister->kkt_fns_num        =   config->cloud_kkt_fns_num;
    jsonOfdRegister->organization_inn   =    config->organization_inn;

	return EXIT_SUCCESS;
}
//===========================================================================================================
//                          Парсер ответа на заявление облачной регистрации
//===========================================================================================================
int JSON::jsonOFDRegisterParser( string JSON, JSON_OFD_REGISTER *jsonOfdRegister )
{
	logDBG( "JSON OFD REGISTER PARSER" );

	if(jsonOfdRegister == NULL)
	{
		logERR( "NULL OFD REGISTER POINTER!!!" );
		return -1;
	}

	int    err = JSON_NO_ERROR;
	string tmp_str;

	//Тест на соблюдение синтаксиса JSON
	err = jsonPreAnalizeErrors(JSON);
	if(err != 0)
	{
		return err;
	}

	logDBG( "JSON CORRECT" );

	try
	{
		if (JSON.length() > 8)
		{
			EncodeConvertor ec;

			std::stringstream JSON_STREAM(JSON);
//			boost::property_tree::ptree JSON_TREE;
//			boost::property_tree::read_json(JSON_STREAM, JSON_TREE);

//			jsonOfdRegister->HUB_result    = JSON_TREE.get<string>("result"            , "");
//			jsonOfdRegister->OFD_result    = JSON_TREE.get<string>("data.result"       , "");
//			jsonOfdRegister->error_message = JSON_TREE.get<string>("data.error_message", "");

			if (   (jsonOfdRegister->HUB_result.length() == 0)
				|| (jsonOfdRegister->OFD_result.length() == 0))
			{
				logERR( "REQUIRED FIELDS DONT FOUND!!! " );
				err = JSON_OTHER_ERR;
			}
		}
	}
    catch(...)
	{
		logERR("JSON PARSER ERROR!!!");
		err = JSON_OTHER_ERR;
	}

	logDBG( "JSON PARS SUCCESS" );

	return err;
}
//===========================================================================================================
//                      Создание структуры запроса статуса облачной регистрации
//===========================================================================================================
int JSON::JSON_OFDRegistrationStatusCreate( Config *config, JSON_OFD_REGISTRATION_STATUS *jsonOfdRegistrationStatus )
{
	logDBG( "JSON OFD REGISTRATION STATUS CREATE" );

	if ((config == NULL) || (jsonOfdRegistrationStatus == NULL))
	{
		logERR( "NULL POINTER!!!" );
		return EXIT_FAILURE;
	}

	jsonOfdRegistrationStatus->uuid = config->uuid;

	return EXIT_SUCCESS;
}
//===========================================================================================================
//                        Парсер ответа на запрос статуса облачной регистрации
//===========================================================================================================
int JSON::jsonRegistrationStatusParser( string JSON, JSON_OFD_REGISTRATION_STATUS *jsonOfdRegistrationStatus )
{
	logINFO( "JSON REGISTRATION STATUS PARSER" );

	if(jsonOfdRegistrationStatus == NULL)
	{
		logERR( "NULL JSON OFD REG STATUS PONTER!!!" );
		return -1;
	}

	int    err = JSON_NO_ERROR;
	string tmp_str;

	//Тест на соблюдение синтаксиса JSON
	err = jsonPreAnalizeErrors(JSON);
	if(err != 0) return err;
	logDBG( "JSON CORRECT" );

	try
	{
		if (JSON.length() > 8)
		{
			EncodeConvertor ec;

			std::stringstream JSON_STREAM(JSON);
//			boost::property_tree::ptree JSON_TREE;
//			boost::property_tree::read_json(JSON_STREAM, JSON_TREE);

//			jsonOfdRegistrationStatus->HUB_result      = JSON_TREE.get<string>(                        "result"            , "");
//			jsonOfdRegistrationStatus->status          = JSON_TREE.get<string>(                        "data.status"       , "");
//			jsonOfdRegistrationStatus->error_message   = JSON_TREE.get<string>(                        "data.error_message", "");
//			jsonOfdRegistrationStatus->registry_number = JSON_TREE.get<string>("data.kkt_registration_info.registry_number", "");

			if (   (jsonOfdRegistrationStatus->HUB_result.length() == 0)
				||     (jsonOfdRegistrationStatus->status.length() == 0))
			{
				logERR( "REQUIRED FIELDS DONT FOUND!!! " );
				err = JSON_OTHER_ERR;
			}
            if (   (jsonOfdRegistrationStatus->status.compare( "NEED_REPORT" ) == 0)
                && (jsonOfdRegistrationStatus->registry_number.length()        == 0))
            {
                logERR( "REG NUMBER IS EMPTY!!!" );
                err = JSON_OTHER_ERR;
            }
		}
	}
    catch(...)
	{
		logERR("JSON PARSER ERROR!!!");
		err = JSON_OTHER_ERR;
	}

	logDBG( "JSON PARS SUCCESS" );

	return err;
}
//===========================================================================================================
//                      Создание структуры запроса статуса облачной регистрации
//===========================================================================================================
int JSON::JSON_OFD_Sign( Config             *config,
						 string                code,
						 JSON_OFD_SIGN *jsonOfdSign  )
{
	logDBG( "JSON OFD SIGN CREATE" );

	if ((config == NULL) || (jsonOfdSign == NULL))
	{
		logERR( "NULL POINTER!!!" );
		return EXIT_FAILURE;
	}

	if(code.length() == 0)
	{
		logERR( "TEH CODE IS EMPTY!!!" );
		return EXIT_FAILURE;
	}

	jsonOfdSign->uuid = config->uuid;
	jsonOfdSign->code =         code;

	return EXIT_SUCCESS;
}
//===========================================================================================================
//                                      Парсер данных для отправки лога
//===========================================================================================================
int JSON::jsonLogsParser(string JSON, JSON::JSON_LOGS &jsonLogs)
{
    //using namespace boost::property_tree;
    logDBG( "JSON LOGS PARSER" );

    int err = 0;

    jsonLogs.del = 0;
    jsonLogs.id.clear();
    jsonLogs.url.clear();
    jsonLogs.ssuk.clear();

    //Тест на соблюдение синтаксиса JSON
    err = jsonPreAnalizeErrors( JSON );
    if ( err != 0 )
    {
        return err;
    }

//    try
//    {
//        ptree JSON_TREE;
//        stringstream JSON_STREAM(JSON);

//        read_json(JSON_STREAM, JSON_TREE);

//        jsonLogs.url  = JSON_TREE.get<string>  ("url",                   "");
//        jsonLogs.id   = JSON_TREE.get<string>  ("id",                    "");
//        jsonLogs.del  = JSON_TREE.get<uint32_t>("headers.X-Delete-After", 0);
//        jsonLogs.ssuk = JSON_TREE.get<string>  ("parameters._ssuk",      "");

//        bool jsonDataError = jsonLogs.url.empty() || jsonLogs.ssuk.empty() || jsonLogs.id.empty();

//        if ( jsonDataError )
//        {
//            logERR( "REQUIRED FIELDS DONT FOUND!!! " );
//            err = JSON_OTHER_ERR;
//        }
//    }
//    catch ( json_parser_error )
//    {
//        logERR("JSON parser error!");
//        err = JSON_OTHER_ERR;
//    }

    return err;
}

bool JSON::jsonImageUploadParser(const string& JSON, JSON::JSON_IMAGE& image)
{
    try
    {
        JsonParser jsonData = JsonParser::parse(JSON);

        if( !jsonData.empty() && (jsonData.count("data") != 0) )
        {
            if (jsonData["data"].count("url") == 0)
            {
                return false;
            }

            image.url = jsonData["data"].value("url", "");

            if (jsonData["data"].count("parameters") != 0)
            {
                JsonParser jsonParams = jsonData["data"]["parameters"];
                if (jsonParams.count("_ssuk") != 0)
                {
                    image.ssuk = jsonParams.value("_ssuk", "");
                    return true;
                }
            }
        }
    }
    catch (...)
    {
        logERR("%s Ops! %s", __FUNCTION__, JSON.c_str());
    }

    return false;
}

bool JSON::jsonImageDownloadParser(const string &JSON, JSON::JSON_IMAGE& image)
{
    JsonParser jsonData = JsonParser::parse(JSON);

    if( !jsonData.empty() && (jsonData.count("data") != 0) )
    {
        image.uuid = jsonData["data"].value("uuid", "");
        image.fnNumber = jsonData["data"].value("fn_number", "");

        if (jsonData["data"].count("url") != 0)
        {
            image.urlDonwload = jsonData["data"].value("url", "");
            return true;
        }
    }

    return false;
}
//===========================================================================================================
//                                      Парсер ответа
//===========================================================================================================
int JSON::jsonSearchProductParser(string JSON, JSON::JSON_PRODUCT &product)
{
    logDBG( "JSON SEARCH PRODUCT PARSER" );

    int    err = JSON_NO_ERROR;
    string  result;

    //Тест на соблюдение синтаксиса JSON
    err = jsonPreAnalizeErrors(JSON);
    if ( err != 0 )
    {
        return err;
    }

    logDBG( "JSON CORRECT" );

//    try
//    {
//        using namespace boost::property_tree;

//        stringstream JSON_STREAM(JSON);
//        ptree JSON_TREE;
//        read_json(JSON_STREAM, JSON_TREE);

//        result = JSON_TREE.get<string>("result", "");

//        boost::optional<ptree& > child;
//        boost::optional<ptree& > child_barcode;
//        child = JSON_TREE.get_child_optional("data.products");

//        if ( child )
//        {
//            for (auto tasks : JSON_TREE.get_child("data.products"))
//            {
//                product.meta.name       = tasks.second.get<string>  ("name",        "");
//                product.meta.price      = tasks.second.get<uint64_t>("price",        0);
//                product.meta.alcohol    = tasks.second.get<bool>    ("alcohol",  false);
//                product.meta.scale      = tasks.second.get<bool>    ("scale",    false);
//                product.meta.precision  = tasks.second.get<float>   ("precision",  0.0);
//                product.meta.measure    = tasks.second.get<string>  ("measure",     "");

//                child_barcode = tasks.second.get_child_optional("barcodes");

//                if ( child_barcode )
//                {
//                    for (auto barcode : tasks.second.get_child("barcodes"))
//                    {
//                        product.meta.barcode += barcode.second.get<string> ("", "");
//                        product.meta.barcode += ",";
//                    }

//                    if ( !product.meta.barcode.empty() )
//                    {
//                        product.meta.barcode.pop_back();
//                    }
//                }
//            }
//        }
//        else
//        {
//            return -1;
//        }

//        if ( result.compare("OK") != 0 )
//        {
//            logERR( "SEARCH PRODUCT FAILED!!!" );
//            return -1;
//        }
//    }
//    catch(boost::property_tree::json_parser_error)
//    {
//        logERR("JSON PARSER ERROR!!!");
//        err = JSON_OTHER_ERR;
//    }

    logDBG( "JSON PARS SUCCESS" );

    return err;
}

//===========================================================================================================
//                                      Парсер ответа на СМС-подпись
//===========================================================================================================
int JSON::jsonOfdSignParser( string JSON, JSON_OFD_SIGN *jsonOfdSign )
{
	logINFO( "JSON OFD SIGN PARSER" );

	if(jsonOfdSign == NULL)
	{
		logERR( "NULL JSON OFD SIGN POINTER!!!" );
		return -1;
	}

	int    err = JSON_NO_ERROR;
	string tmp_str;

	//Тест на соблюдение синтаксиса JSON
	err = jsonPreAnalizeErrors(JSON);
	if(err != 0) return err;
	logDBG( "JSON CORRECT" );

//	try
//	{
//		if (JSON.length() > 8)
//		{
//			EncodeConvertor ec;

//			std::stringstream JSON_STREAM(JSON);
//			boost::property_tree::ptree JSON_TREE;
//			boost::property_tree::read_json(JSON_STREAM, JSON_TREE);

//			jsonOfdSign->HUB_result    = JSON_TREE.get<string>("result"            , "");
//			jsonOfdSign->OFD_result    = JSON_TREE.get<string>("data.result"       , "");
//			jsonOfdSign->error_message = JSON_TREE.get<string>("data.error_message", "");

//			if (    (jsonOfdSign->HUB_result.length() == 0)
//				 || (jsonOfdSign->OFD_result.length() == 0))
//			{
//				logERR( "REQUIRED FIELDS DONT FOUND!!! " );
//				err = JSON_OTHER_ERR;
//			}
//		}
//	}
//	catch(boost::property_tree::json_parser_error)
//	{
//		logERR("JSON PARSER ERROR!!!");
//		err = JSON_OTHER_ERR;
//	}

	logDBG( "JSON PARS SUCCESS" );

	return err;
}

bool JSON::isTaskProcessed(const uint64_t &taskId, const JSON_POLL_ &json_poll)
{
    for ( const JSON_TASK &jsonTask : json_poll.response.tasks )
    {
        if ( jsonTask.task_id == taskId )
        {
            return true;
        }
    }

    return false;
}

//============================================================
// Парс массива разнородных тасок
int JSON::jsonPollParser( string JSON, JSON_POLL_ *json_poll )
{
	logDBG( "%s:: Запуск.", __func__ );

	int    err = POLL_PARSE_ERRORS::OK;

    string      tmp_str;
	JsonParser jsonTree;

	//Тест на соблюдение синтаксиса JSON
    err = jsonPreAnalizeErrors(JSON);
    if( ( err != POLL_PARSE_ERRORS::OK ) || (!jsonTree.accept(JSON)) || (JSON.length() <= 8) )
	{
        logERR( "%s:: Ошибка! Невалидный джейсон!", __func__  );
		logERR( "JSON:'%s'",  JSON.c_str());
		return POLL_PARSE_ERRORS::BAD_JSON;
	}
	logDBG( "%s:: JSON Нормальный по структуре", __func__  );

    JSON_TASK task;
    JSON_KEY_ACTIVATION key_activation;
    JSON_BIND_PRODUCT bind_product;
    JSON_DELETE_PRODUCT delete_product;
    JSON_UPSERT_PRODUCT upsert_product;
    JSON_UPSERT_DISCOUNT upsert_discount;
    JSON_DELETE_DISCOUNT delete_discount;
    JSON_EXTERNAL_PURCHASE external_purchase;
    JSON_REGISTRATION registration;
    JSON_UPSERT_CASHIER upsertCashier;
    JSON_DELETE_CASHIER deleteCashier;
    JSON_UPDATE_MRP_TABLES updateMRPtables;

    bind_product_clear     ( &bind_product      );
    delete_product_clear   ( &delete_product    );
    upsert_product_clear   ( &upsert_product    );
    external_purchase_clear( &external_purchase );
    key_activation_clear   ( &key_activation    );

    json_poll->response.tasks.clear();

	jsonTree  = JsonParser::parse(JSON);

	try
	{
		json_poll->response.result = getAnswerError(jsonTree.value("result",""));

		if(json_poll->response.result == CAB_OK)
		{
			logINFO( "PARSING JSON" );
			JsonParser jsonTasks = jsonTree["data"]["tasks"];

			if( !jsonTasks.empty() )
			{
				for(auto itJsonTask : jsonTasks)
				{
                    err = POLL_PARSE_ERRORS::OK;
                    // tasks[]
                    task_clear(&task);

                    string type    = itJsonTask.value("type"   , "");

                    task.type = TASK_UNKNOWN;
                    if (type.compare(BIND_PRODUCT_TYPE) == 0)
                    {
                        task.type = TASK_BIND_PRODUCT;
                    }
                    else if (type.compare(DELETE_PRODUCT_TYPE) == 0)
                    {
                        task.type = TASK_DELETE_PRODUCT;
                    }
                    else if (type.compare(UPSERT_PRODUCT_TYPE) == 0)
                    {
                        task.type = TASK_UPSERT_PRODUCT;
                    }
                    else if (type.compare(UPSERT_DISCOUNT_TYPE) == 0)
                    {
                        task.type = TASK_UPSERT_DISCOUNT;
                    }
                    else if (type.compare(DELETE_DISCOUNT_TYPE) == 0)
                    {
                        task.type = TASK_DELETE_DISCOUNT;
                    }
                    else if (type.compare(EXTERNAL_PURCHASE_TYPE) == 0)
                    {
                        task.type = TASK_EXTERNAL_PURCHASE;
                    }
                    else if (type.compare(KEY_ACTIVATION_TYPE) == 0)
                    {
                        task.type = TASK_KEY_ACTIVATION;
                    }
                    else if (type.compare(REGISTRATION_TASK) == 0)
                    {
                        task.type = TASK_REGISTRATION;
                    }
                    else if (type.compare(PURCHASE_DOCUMENT_RESULT) == 0)
                    {
                        task.type = TASK_PURCHASE_DOCUMENT_RESULT;
                    }
                    else if (type.compare(UPSERT_CASHIER) == 0)
                    {
                        task.type = TASK_UPSERT_CASHIER;
                    }
                    else if (type.compare(DELETE_CASHIER) == 0)
                    {
                        task.type = TASK_DELETE_CASHIER;
                    }
                    else if (type.compare(UPDATE_MRP_TABLES) == 0)
                    {
                        task.type = TASK_UPDATE_MRP_TABLES;
                    }

					task.task_id = itJsonTask.value("task_id",  0);
					if(task.task_id == 0 )
					{
						logWARN("%s:: WARNING! task_id = 0, ", __func__);
						continue;
					}

                    switch (task.type)
                    {
                        //---------------------------------------------------------------------------------------------
                        // KEY_ACTIVATION
                        //---------------------------------------------------------------------------------------------
                        case TASK_KEY_ACTIVATION:
                        {
                            logINFO("ACTIVATE KEY TASK");
                            if (parseActivationKey(itJsonTask, json_poll->response.task_data.key_activation))
                            {
                                json_poll->response.task_data.key_activation.task_id = task.task_id;
                            }
                            else
                            {
                                err = POLL_PARSE_ERRORS::BAD_ACTIVATE;
                            }
                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // UPSERT_PRODUCT
                        //---------------------------------------------------------------------------------------------
                        case TASK_UPSERT_PRODUCT:
                        {
                            logINFO("UPSERT PRODUCT TASK");
                            if (parseUpsertProduct(itJsonTask, upsert_product))
                            {
                                upsert_product.task_id = task.task_id;
                                json_poll->response.task_data.upsert_products.push_back(upsert_product);
                            }
                            else
                            {
                                err = POLL_PARSE_ERRORS::BAD_UPSERT;
                            }
                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // BIND PRODUCT
                        //---------------------------------------------------------------------------------------------
                        case TASK_BIND_PRODUCT:
                        {
                            logINFO("BIND PRODUCT TASK");
                            if (parseBindProduct(itJsonTask, bind_product))
                            {
                                bind_product.task_id = task.task_id;
                                json_poll->response.task_data.bind_products.push_back(bind_product);
                            }
                            else
                            {
                                err = POLL_PARSE_ERRORS::BAD_BIND;
                            }
                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // DELETE PRODUCT
                        //---------------------------------------------------------------------------------------------
                        case TASK_DELETE_PRODUCT:
                        {
                            logINFO("DELETE PRODUCT TASK");
                            if (parseDeleteProduct(itJsonTask, delete_product))
                            {
                                delete_product.task_id = task.task_id;
                                json_poll->response.task_data.delete_products.push_back(delete_product);
                            }
                            else
                            {
                                err = POLL_PARSE_ERRORS::BAD_DELETE;
                            }
                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // EXTERNAL PURCHASE
                        //---------------------------------------------------------------------------------------------
                        case TASK_EXTERNAL_PURCHASE:
                        {
                            logINFO("EXTERNAL PURCHASE TASK");
                            external_purchase.isValid = (parseExternalPurchase(itJsonTask, external_purchase) == EXTERNAL_PURCHASE_ERR_SUCCESS);
                            task_display(&external_purchase);
                            json_poll->response.task_data.external_purchases.push_back(external_purchase);
                            err = EXTERNAL_PURCHASE_ERR::EXTERNAL_PURCHASE_ERR_SUCCESS;
                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // UPSERT DISCOUNT
                        //---------------------------------------------------------------------------------------------
                        case TASK_UPSERT_DISCOUNT:
                        {
                            logINFO("UPSERT DISCOUNT TASK");
                            if (parseUpsertDiscount(itJsonTask, upsert_discount) == DISCOUNT_ERR_SUCCESS)
                            {
                                upsert_discount.task_id = task.task_id;
                                json_poll->response.task_data.upsert_discounts.push_back( upsert_discount );
                            }
                            else
                            {
                                err = POLL_PARSE_ERRORS::BAD_UPSERT_DISCOUNT;
                            }
                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // DELETE DISCOUNT
                        //---------------------------------------------------------------------------------------------
                        case TASK_DELETE_DISCOUNT:
                        {
                            logINFO("DELETE DISCOUNT TASK");
                            if (parseDeleteDiscount(itJsonTask, delete_discount) == DISCOUNT_ERR_SUCCESS)
                            {
                                delete_discount.task_id = task.task_id;
                                json_poll->response.task_data.delete_discounts.push_back( delete_discount );
                            }
                            else
                            {
                                err = POLL_PARSE_ERRORS::BAD_DELETE_DISCOUNT;
                            }
                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // PURCHASE_DOCUMENT_RESULT
                        //---------------------------------------------------------------------------------------------
                        case TASK_PURCHASE_DOCUMENT_RESULT:
                        {
                            logINFO("PURCHASE DOCUMENT RESULT TASK");
                            JSON_PURCHASE_DOCUMENT_RESULT purchaseResult;
//                            if(parsePurchaseDocumentResult(itJsonTask, purchaseResult))
//                            {
//                                json_poll->response.task_data.purchaseResults.push_back(purchaseResult);
//                            }
//                            else
                            {
                                err = POLL_PARSE_ERRORS::BAD_PURCHASE_DOCUMENT_RESULT;
                                task.message = "POLL_PARSE_ERRORS::BAD_PURCHASE_DOCUMENT_RESULT";
                            }
                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // REGISTRATION
                        //---------------------------------------------------------------------------------------------
                        case TASK_REGISTRATION:
                        {
                            logINFO("%s REGISTRATION TASK", __PRETTY_FUNCTION__ );
                            JsonParser jsonData = itJsonTask["data"];
                            registration.taskId = task.task_id;
                            string registrationType = jsonData.value("type", "");

                            if (registrationType.compare( REGISTRATION ) == 0)
                            {
                                registration.type = REG_REGISTRATION;
                            }
                            else if (registrationType.compare( CHANGE_PARAMETERS ) == 0)
                            {
                                registration.type = REG_CHANGE_PARAMETERS;
                            }
                            else if (registrationType.compare( CHANGE_PARAMETERS_AND_FN ) == 0)
                            {
                                registration.type = REG_CHANGE_PARAMETERS_AND_FN;
                            }
                            else if (registrationType.compare( CLOSE_FN ) == 0)
                            {
                                registration.type = REG_CLOSE_FN;
                            }
                            else
                            {
                                registration.type = REG_NONE;
                            }

                            if (jsonData.count("cashier") != 0)
                            {
                                JsonParser jsonCashier = jsonData["cashier"];
                                registration.cashierInn  = jsonCashier.value("inn",  "");
                                registration.cashierName = jsonCashier.value("name", "");
                            }
                            else
                            {
                                registration.cashierName = "Кабинет Дримкас";
                            }
                            json_poll->response.task_data.registrations.push_back( registration );

                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // UPSERT CASHIER
                        //---------------------------------------------------------------------------------------------
                        case TASK_UPSERT_CASHIER:
                        {
                            logINFO("TASK UPSERT CASHIER");
                            if (parseUpsertCashier(itJsonTask, upsertCashier))
                            {
                                upsertCashier.task_id = task.task_id;
                                json_poll->response.task_data.upsertCashiers.push_back(upsertCashier);
                            }
                            else
                            {
                                err = POLL_PARSE_ERRORS::BAD_UPSERT;
                            }
                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // DELETE CASHIER
                        //---------------------------------------------------------------------------------------------
                        case TASK_DELETE_CASHIER:
                        {
                            logINFO("TASK DELETE CASHIER");
                            if (parseDeleteCashier(itJsonTask, deleteCashier))
                            {
                                deleteCashier.task_id = task.task_id;
                                json_poll->response.task_data.deleteCashiers.push_back(deleteCashier);
                            }
                            else
                            {
                                err = POLL_PARSE_ERRORS::BAD_DELETE;
                            }
                            break;
                        }
                        //---------------------------------------------------------------------------------------------
                        // Получение данных для расчета минимальной розничной цены алкогольной продукции на кассе
                        //---------------------------------------------------------------------------------------------
                        case TASK_UPDATE_MRP_TABLES:
                        {
                            logINFO("TASK UPDATE MRP TABLES");
                            if (parseUpdateMRPtables(itJsonTask, updateMRPtables))
                            {
                                updateMRPtables.task_id = task.task_id;
                                json_poll->response.task_data.updateMRPtables.push_back(updateMRPtables);
                            }
                            else
                            {
                                err = POLL_PARSE_ERRORS::BAD_UPDATE_MRP_TABLES;
                            }
                            err = POLL_PARSE_ERRORS::OK;
                            break;
                        }
                        default:
                        {
                            err = POLL_PARSE_ERRORS::BAD_TASK_TYPE;
                            break;
                        }
                    }
					//---------------------------------------------------------------------------------------------
                    if (err != POLL_PARSE_ERRORS::OK)
                    {
                        logERR("%s:: Ошибка в ходе парса таски task_id = %llu, Код ошибки = %d, " , __func__, task.task_id,  err);
                        task.status = ERROR;
                        err = POLL_PARSE_ERRORS::OK;
                    }
                    json_poll->response.tasks.push_back(task);
				}
                logERR("%d TASKS PARSED FROM CABINET", json_poll->response.tasks.size());
			}
			else
			{
				logINFO( "NO TASKS FROM CABINET" );
			}
		}
		else
		{
            err = json_poll->response.result;
			logERR( "Некорректный result = %d",err );
			return err;
		}
		logINFO( "JSON PARSE SUCCESS" );
	}
	catch(JsonParser::exception& e)
	{
		logERR("\tJSON parser nlohmann exception!");
		logERR("\tmessage: %s  \n\texception id: %d", e.what(), e.id);
		err = POLL_PARSE_ERRORS::EXEPTION_JSON;
	}

    return err;
}

//=======================================================================
// Проверка на наличие костылей со скидками во внешних чеках
bool JSON::isValidPosition( string       name,
							string       type,
							uint64_t quantity,
							uint64_t    price,
							uint64_t    total )
{
	// Проверяем корректность исх данных
	if(type.empty()  ||
	   quantity == 0 ||
	   price    == 0  )
	{
		logERR("isValidPosition:: Неверные входные данные: name = '%s', "
													      "type = '%s', "
											          "quantity = '%llu', "
													     "price = '%llu'",
																		  name.c_str(),
																		  type.c_str(),
																			  quantity,
																				 price);
		return false;
	}

	bool ok = true;
//	// Если надо валидировать
//	if (                            ( total != 0 )  &&
//	    ( type.compare(GOOD_TYPE_COUNTABLE) == 0 )   )
//	{
//		ok = (total == ( price * quantity ) );
//	}
	// Если данные в позиции кривые
	if( !ok )
	{
		logERR("isValidPosition:: данные по позиции не сходятся: name = '%s', "
					                                            "type = '%s', "
					                                        "quantity = '%llu', "
					                                           "price = '%llu', "
					                                           "total = '%llu'",
																				name.c_str(),
																				type.c_str(),
																				quantity,
																				price,
																				total );
	}

	return ok;
}

/*==========================================================================
 * Parse reg cabinet json Parser
 ===========================================================================*/
int JSON::jsonRegCabinetParser(string JSON, JSON_REGISTER &jsonCabRegister)
{
    logINFO("%s", __PRETTY_FUNCTION__);

    int err {jsonPreAnalizeErrors(JSON)};
    if (err != JSON_NO_ERROR)
    {
        return err;
    }

    logDBG("%s Json валидный по структуре!", __PRETTY_FUNCTION__);
    jsonCabRegister.response.reset();

    try
    {
        JsonParser jsonAnswer = JsonParser::parse(JSON);

        bool valid {jsonAnswer.contains("data") && jsonAnswer["data"].contains("owner")};
        if (!valid)
        {
            return JSON_OTHER_ERR;
        }

        EncodeConvertor ec;
        int errP {0};
        JsonParser data = jsonAnswer["data"];
        jsonCabRegister.response.owner = data.value("owner", "");
        jsonCabRegister.response.kktName = ec.UTF8toCP866(data.value("kkt_name", ""));
        jsonCabRegister.response.shopName = ec.UTF8toCP866(data.value("shop_name", ""));
        jsonCabRegister.response.displayInn = data.value("display_inn", "");
        jsonCabRegister.response.displayCompany = ec.UTF8toCP866(data.value("display_company", ""));

        if (data.contains("product_count"))
        {
            if ( jsonAnswer["data"]["product_count"].is_string())
            {
                const string productCount = jsonAnswer["data"]["product_count"];
                jsonCabRegister.response.goodsCount = QString(productCount.c_str()).toULong();
            }

            if ( jsonAnswer["data"]["product_count"].is_number() )
            {
                jsonCabRegister.response.goodsCount = jsonAnswer["data"]["product_count"];
            }
        }

        if (jsonCabRegister.response.owner.empty())
        {
            logERR("%s Отсутствует необходимое поле(owner)!", __PRETTY_FUNCTION__);
            if (jsonAnswer.contains("result"))
            {
                err = getAnswerError(jsonAnswer.value("result",""));
                jsonCabRegister.response.result = err;
                jsonCabRegister.response.message = jsonAnswer.value("message", "");
            }
            else
            {
                err = JSON_OTHER_ERR;
            }
        }
    }
    catch (...)
    {
        // Исключение парса
        logERR("%s Ooops!", __FUNCTION__);
        // Вывод json исключения
        logERR("%s", JSON.c_str());
        err = JSON_OTHER_ERR;
    }

    return err;
}

int JSON::jsonParser(int type, string jsonCode, void *retStruct)
{
	int err = JSON_CRIRICAL_ERROR;

	//Тест на соблюдение синтаксиса JSON
    err = jsonPreAnalizeErrors(jsonCode);
    if(err != 0) return err;
	logDBG( "JSON CORRECT" );

	try
    {
		logINFO("JSON_String = %s", jsonCode.c_str());
		if(jsonCode.length() > 8)
		{
			std::stringstream ifs(jsonCode);
//            boost::property_tree::ptree propertyTree;
//     		boost::property_tree::read_json(ifs, propertyTree);

        if(type == JSON_UPDATE)
        {

			JSONUpdateStr jsonStr;
			JSONUpdate json;
            memset(&json, 0, sizeof(json));

//            jsonStr.status = propertyTree.get<int>("status", 0);
			if(jsonStr.status == 0)
			{
//                #ifdef DREAMKAS_F
//                    for (auto search_result : propertyTree.get_child("updates"))
//                    {
//                       jsonStr.version = search_result.second.get<std::string>("version", "");
//                       jsonStr.url = search_result.second.get<std::string>("url", "");
//                       jsonStr.md5 = search_result.second.get<std::string>("md5", "");
//                       jsonStr.fileSize = search_result.second.get<long>("size", 0);
//                       logINFO("Version to update: %s",jsonStr.version.c_str());

//                       strcpy(json.version, jsonStr.version.c_str());
//                       strcpy(json.url, jsonStr.url.c_str());
//                       strcpy(json.md5, jsonStr.md5.c_str());
//                       json.status = jsonStr.status;
//                       json.fileSize = jsonStr.fileSize;
//                    }
//                #else
//                    if(propertyTree.get_child_optional("data.patches"))
//                    {
//                        boost::optional<boost::property_tree::ptree &> update;
//                        BOOST_FOREACH (boost::property_tree::ptree::value_type &update, propertyTree.get_child("data.patches"))
//                        {
//                            jsonStr.fileSize = update.second.get<long>("file.size", 0);
//                            jsonStr.md5 = update.second.get<string>("file.md5", "");
//                            jsonStr.url = update.second.get<string>("file.url", "");
//                            jsonStr.version = update.second.get<string>("version.to", "");
//                            logINFO("Version to update: %s", jsonStr.version.c_str());

//                            strcpy(json.version, jsonStr.version.c_str());
//                            strcpy(json.url, jsonStr.url.c_str());
//                            strcpy(json.md5, jsonStr.md5.c_str());
//                            json.status = jsonStr.status;
//                        }
//                    }
//                #endif
				memcpy(retStruct, &json, sizeof(JSONUpdate));
				err = JSON_NO_ERROR;
		    }
			else
			{
				logINFO("jsonStr.status != 0");
			}
        }
        else if(type == JSON_KKT_REGISTR)
        {
            JSONKKTRegInfo    jsonKKTreg;
            JSONKKTRegInfoStr jsonKKTregStr;

            err = JSON_CRIRICAL_ERROR;//parseKktRegistr(jsonCode, jsonKKTregStr, jsonKKTreg);
            memcpy(retStruct, &jsonKKTreg, sizeof(JSONKKTRegInfo));
        }
        else if(type == JSON_RESULT)
        {
			JSONResult json;

//			json.result = getAnswerError(propertyTree.get<string>("result", ""));

			memcpy(retStruct, &json, sizeof(JSONResult));
			err = JSON_NO_ERROR;
        }
        else if(type == JSON_RESULT_INT)
        {
			JSONResult json;

//				json.result = propertyTree.get<int>("status", 0);
				logINFO("JSON_RESULT = %d", json.result);
				memcpy(retStruct, &json, sizeof(JSONResult));
				err = JSON_NO_ERROR;
			}
		}
		else // Если длина < 8					//-------------------------------------------------
		{
			err = JSON_LENGTH_ERR;
		}
	}
    catch(...)
    {
    	logERR("JSON parser error!");
        err = JSON_OTHER_ERR;
    }

 return err;

}

JSON::MONEY_DOCUMENT_REPORT::MONEY_DOCUMENT_REPORT()
{
	shift            = 0;
	number           = 0;
	date.        clear();
	doc_type.    clear();
	sum              = 0;
	cashier_name.clear();
}

JSON::MONEY_DOCUMENT_REPORT::~MONEY_DOCUMENT_REPORT()
{
	shift            = 0;
	number           = 0;
	date.        clear();
	doc_type.    clear();
	sum              = 0;
	cashier_name.clear();
}

JSON::MONEY_DOCUMENT_REPORT_JSON::MONEY_DOCUMENT_REPORT_JSON()
{
	uuid.     clear();
	documents.clear();
}

JSON::MONEY_DOCUMENT_REPORT_JSON::~MONEY_DOCUMENT_REPORT_JSON()
{
	uuid.     clear();
	documents.clear();
}

JSON::PAYMENTS::PAYMENTS()
{
	type.clear();
	count    = 0;
	sum      = 0;
}

JSON::PAYMENTS::~PAYMENTS()
{
	type.clear();
	count    = 0;
	sum      = 0;
}

JSON::COUNTERS::COUNTERS()
{
	type.    clear();
	payments.clear();
	total_sum    = 0;
}

JSON::COUNTERS::~COUNTERS()
{
	type.    clear();
	payments.clear();
	total_sum    = 0;
}

JSON::SHIFTS::SHIFTS()
{
	shift            = 0;
	number           = 0;
	date.        clear();
	doc_type.    clear();
	sum_cash_begin   = 0;
	sum_cash_end     = 0;
	count_cash_out   = 0;
	count_cash_in    = 0;
	cashier_name.clear();
	counters.    clear();
}

JSON::SHIFTS::~SHIFTS()
{
	shift            = 0;
	number           = 0;
	date.        clear();
	doc_type.    clear();
	sum_cash_begin   = 0;
	sum_cash_end     = 0;
	count_cash_out   = 0;
	count_cash_in    = 0;
	cashier_name.clear();
	counters.    clear();
}

JSON::SHIFT_DOCUMENT_REPORT_JSON::SHIFT_DOCUMENT_REPORT_JSON()
{
	uuid.  clear();
	shifts.clear();
}

JSON::SHIFT_DOCUMENT_REPORT_JSON::~SHIFT_DOCUMENT_REPORT_JSON()
{
	uuid.  clear();
	shifts.clear();
}

int JSON::money_doc_rep_clear( MONEY_DOCUMENT_REPORT_JSON *moneyDocumentReportJson )
{
	if (moneyDocumentReportJson == NULL)
	{
		logERR( "NULL POINTER!" );
		return -1;
	}
	moneyDocumentReportJson->uuid.clear();
	moneyDocumentReportJson->documents.clear();

	return 0;
}

int JSON::shift_doc_rep_clear( SHIFT_DOCUMENT_REPORT_JSON *shiftDocumentReportJson )
{
	if (shiftDocumentReportJson == NULL)
	{
		logERR( "NULL POINTER!" );
		return -1;
	}
	shiftDocumentReportJson->uuid.clear();
	shiftDocumentReportJson->shifts.clear();

	return 0;
}

string JSON::getStageName(KKT_STAGE stage)
{
    string strStage;

    switch ( stage )
    {
        case STUDY:
        {
            strStage = "STUDY";
            break;
        }
        case ENVD:
        {
            strStage = "ENVD";
            break;
        }
        case FISCAL:
        {
            strStage = "FISCAL";
            break;
        }
        default:
        {
            strStage = "UNKNOWN STAGE";
            break;
        }
    }

    return strStage;
}

JSON::STAT_ERR JSON::jsonCfgCreate(Config *cfg, const License &license, const UpdateInfo &updateInfo, string &jsonStatistics)
{
    logDBG( "%s", __PRETTY_FUNCTION__ );

    jsonStatistics.clear();

    if(cfg == nullptr)
    {
        logERR( "%s Null ptr!", __PRETTY_FUNCTION__ );
        return STAT_ERR_NULL_PTR;
    }

    JsonParser jsonStat;

    EncodeConvertor ec;
    int errP = 0;

    if(cfg->uuid.empty())
    {
        logERR( "%s Uuid is empty!", __PRETTY_FUNCTION__ );
        return STAT_ERR_EMPTY_UUID;
    }

    jsonStat["uuid"]                            = cfg->uuid;
    jsonStat["version"]                         = VERSION_FOR_STAT;
    jsonStat["model"]                           = modelName;
    jsonStat["readable_model"]                  = modelName;
    jsonStat["kkt_model"]                       = modelName;
    jsonStat["kkt_readable_model"]              = modelName;
    JsonParser data;
    //----------------- Main info -----------------------
    JsonParser mainInfo;
    #ifndef BIOS_RB
        mainInfo["stage"]                       = getStageName( static_cast<KKT_STAGE>(cfg->stage) );
        mainInfo["fs_replace_mode"]             = cfg->fs_replace_mode;
    #endif
    mainInfo["start_time"]                      = convert->dateTimeToStr(cfg->startTime);
    mainInfo["config_version"]                  = cfg->config_version;
    mainInfo["kkt_mode"]                        = cfg->kkt_mode;
    mainInfo["terminal_mode"]                   = cfg->terminal_mode;
    mainInfo["fisgo_version"]                   = cfg->fisgo_version;
    data["main_info"]                           = mainInfo;
    //------  Количество неотправленых доков в офд  ------------------
    data["need_send_to_ofd"] = cfg->needSendToOfd;
    //----------------- Флаги ФН               -----------------------

    JsonParser userInfo;
    #ifndef BIOS_RB
        if( cfg->stage >= FISCAL )
        {
            JsonParser fnFlags;
            fnFlags["immediate_replacement"]              = cfg->immediate_replacement; // Срочная замена фн
            fnFlags["resources_exhastion"]                = cfg->resources_exhastion;   // Исчерпание ресурса фн
            fnFlags["mem_overflow"]                       = cfg->mem_overflow;          // Исчерпание памяти фн
            fnFlags["critical_fn_err"]                    = cfg->critical_fn_err;       // Критическая ошибка ФН
            data["fn_flags"] = fnFlags;
        }
        //----------------- Реквизиты пользователя -----------------------
        userInfo["fs_number"]                       = cfg->fs_number;
        userInfo["fs_number_count"]                 = cfg->fs_number_count;
        for(const QString &fsNumber : cfg->fs_numbers_table)
        {
            userInfo["fs_vector"].push_back( fsNumber.toStdString() );
        }
        userInfo["ffd_kkt_ver"]                     = cfg->ffd_kkt_ver;
        userInfo["fs_ffd_ver"]                      = cfg->fs_ffd_ver;
        userInfo["fs_validity_date"]                = cfg->fs_validity_date;
        if((cfg->fs_validity.length() == FS_VALIDITY_LEN) && (cfg->fs_validity.compare( "000000" ) != 0))
        {
            string year  = cfg->fs_validity.substr( 4, 2 );
            string month = cfg->fs_validity.substr( 2, 2 );
            string day   = cfg->fs_validity.substr( 0, 2 );
            data["kkt_expire_date"] = "20" + year + "-" + month + "-" + day + "T10:01:21";
        }
    #endif
    userInfo["organization_name"]               = ec.CP866toUTF8(cfg->organization_name);
    userInfo["calculation_address"]             = ec.CP866toUTF8(cfg->calculation_address);
    userInfo["calculation_place"]               = ec.CP866toUTF8(cfg->calculation_place);
    userInfo["organization_inn"]                = cfg->organization_inn;
    userInfo["organization_kpp"]                = cfg->organization_kpp;
    userInfo["kkt_reg_num"]                     = cfg->kkt_reg_num;
    userInfo["kkt_plant_num"]                   = cfg->kkt_plant_num;
    userInfo["cashier_inn"]                     = cfg->cashier_inn;                                            ;
    userInfo["kkt_reg_version"]                 = cfg->kkt_reg_version;
    userInfo["article"]                         = cfg->article;
    data["user_info"]                          = userInfo;
//    //-------------------------- Network -----------------------------
//    if(net->ethIsReady())
//    {
//        JsonParser eth;
//        eth["eth_manual"]                       = cfg->eth_manual;
//        eth["mask"]                             = cfg->mask;
//        eth["router"]                           = cfg->router;
//        eth["dns_main"]                         = cfg->dns_main;
//        eth["dns_add"]                          = cfg->dns_add;
//        data["eth"]                         = eth;
//    }
//    //-------------------------- WiFi --------------------------------
//    if(net->wlanIsReady())
//    {
//        JsonParser wlan;
//        wlan["net_wifi_name"]                   = ec.CP866toUTF8(cfg->net_wifi_name);
//        wlan["net_wifi_key"]                    = cfg->net_wifi_key;
//        wlan["net_wifi_signal"]                 = cfg->net_wifi_signal;
//        wlan["net_pass"]                        = cfg->net_pass;
//        data["wlan"]                            = wlan;
//    }
//    //-------------------------- Ip --------------------------------
//    data["net_ip"]                              = JsonParser::array({Net::getAvailableIPs()});
    //---------------------------- Налоги -----------------------------
    JsonParser taxInfo;
    taxInfo["tax_systems"]                      = cfg->tax_systems;
    taxInfo["cur_tax_system"]                   = cfg->cur_tax_system;
    data["tax_info"]                        = taxInfo;
    #ifndef BIOS_RB
        //--------------------------- Признаки ----------------------------
        JsonParser kktSignsInfo;
        kktSignsInfo["encryption_sign"]             = cfg->encryption_sign;
        kktSignsInfo["excisable_sign"]              = cfg->excisable_sign;
        kktSignsInfo["clc_services_sign"]           = cfg->clc_services_sign;
        kktSignsInfo["gambling_sign"]               = cfg->gambling_sign;
        kktSignsInfo["lottery_sign"]                = cfg->lottery_sign;
        kktSignsInfo["paying_agent_sign"]           = cfg->paying_agent_sign;
        kktSignsInfo["kkt_signs"]                   = cfg->kkt_signs;
        kktSignsInfo["add_kkt_signs"]               = cfg->add_kkt_signs;
        data["kkt_signs_info"]                      = kktSignsInfo;
        //----------------------------- ОФД -------------------------------
        JsonParser ofdInfo;
        ofdInfo["ofd_choose"]                       = cfg->ofd_choose;
        ofdInfo["ofd_inn"]                          = cfg->ofd_inn;
        ofdInfo["ofd_server_address"]               = cfg->ofd_server_address;
        ofdInfo["ofd_name"]                         = ec.CP866toUTF8(cfg->ofd_name);
        ofdInfo["ofd_server_port"]                  = cfg->ofd_server_port;
        ofdInfo["check_receipt_address"]            = cfg->check_receipt_address;
        ofdInfo["ofd_server_ip"]                    = cfg->ofd_server_ip;
        ofdInfo["last_msg_ofd"]                     = convert->dateTimeToStr(cfg->lastMsgOFD);
        data["ofd_info"]                            = ofdInfo;
    #endif
    //--------------------------- Смены -------------------------------
    JsonParser shiftInfo;
    shiftInfo["open_shift_date"]                = cfg->open_shift_date;
    shiftInfo["shift_timer"]                    = cfg->shift_timer;
    shiftInfo["is_auto_open_shift_enable"]      = cfg->isAutoOpenShiftEnable;
    shiftInfo["is_auto_cash_out"]               = cfg->is_auto_cash_out;
    data["shift_info"]                          = shiftInfo;
    //--------------------------- Агенты ------------------------------
    JsonParser agentsInfo;
    agentsInfo["agent_mask"]                    = cfg->agent_mask;
    agentsInfo["current_agent"]                	= cfg->current_agent;
    data["agents_info"]                         = agentsInfo;
    //------- Количество быстрых переходов в меню ------------------
    uint64_t fastKeyMenuCnt = 0;
    if(fg->read( FAST_KEY_CNT_FILE, fastKeyMenuCnt ) == FilesFG::FILE_READ_SUCCESS)
    {
        data["fast_key_menu_cnt"]               = fastKeyMenuCnt;
    }
    //--------Sell by MRC|Cab price --------
    data["tobacco_cost_from"]                   = (cfg->tobaccoCostFrom == Config::CAB_COST ? "CAB" : "MRC");
    //--------Количество товаров в базе товаров-----------------------
    data["goods_db_size"]                       = cfg->goodsDbSize;
    //---------------Количество пользователей-------------------------
    data["users_db_size"]                       = cfg->usersCnt;
    //--------------Количество горячих клавиш-------------------------
    data["hot_keys_db_size"]                    = cfg->hotKeyCnt;
    //--------------MAC ETH-------------------------
    string mac;
    fg->strFromFile( ETH_MAC_ADDR, mac );
    if(convert->isMacValid(mac))
    {
        data["eth_mac"]                         = mac;
    }
    //--------------MAC WLAN-------------------------
    fg->strFromFile( WLAN_MAC_ADDR, mac );
    if(convert->isMacValid(mac))
    {
        data["wlan_mac"]                        = mac;
    }
    //------------------------- Остальное -----------------------------
    data["log_type"]                            = cfg->logType;
    data["is_cabinet_enable"]                   = cfg->isCabinetEnable;
    data["bso"]                                 = cfg->bso;
    data["automatic_device_number"]             = ec.CP866toUTF8(cfg->automatNumber);
    data["battery_voltage"]                     = cfg->batteryVoltage;
    data["nds_free_price"]                      = cfg->nds_free_price;

    JsonParser services = JsonParser::array();

    for(License::LicenseTag &feature : license.getLicList())
    {
        if(feature.mask == License::ACTIVE)
        {
            JsonParser service;
            service["description"] = ec.CP866toUTF8(feature.name);
            service["code"]        = feature.tagNum;
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
    data["license"]["services"] = services;

    if (!updateInfo.status.empty() && !updateInfo.version.empty())
    {
        data["update"]["status"] = updateInfo.status;
        data["update"]["version"] = updateInfo.version;
    }

    jsonStat["data"] = data;

    jsonStatistics = jsonStat.dump( OFFSET_OUTPUT );

    logDBG( "%s Statistics Json is complite : %s", __PRETTY_FUNCTION__, jsonStatistics.c_str() );

    return STAT_ERR_JSON_COMPLITE;
}

string JSON::addToJSON(string name, string data, bool isLast)
{
	string tmpStr = "";
    if(!data.empty())
	{
		string tmpData = Convert::escapingOfSlash(data);
		tmpData = Convert::escapingOfQuotes(tmpData);
		tmpStr = "\"" + name + "\":\"" + tmpData + "\"" + ((isLast) ? "" : ",");
	}
	return tmpStr;
}
string JSON::addToJSON(string name, bool data, bool isLast)
{
	string tmpStr;
	tmpStr = "\"" + name + "\":" + ((data)?"true":"false") + ((isLast)?"":",");
	return tmpStr;
}
string JSON::addToJSON(string name, int data, bool isLast)
{
	string tmpStr;
	tmpStr = "\"" + name + "\":" + to_string(data) + ((isLast)?"":",");
	return tmpStr;
}
string  JSON::addToJSON(double   data, bool isLast)
{
    string tmpStr;
    tmpStr = "\"" + to_string(data) + "\"" + ((isLast)?"":",");
    return tmpStr;
}
string JSON::addToJSON(int data, bool isLast)
{
    string tmpStr;
    tmpStr = to_string(data) + ((isLast)?"":",");
    return tmpStr;
}
string JSON::addToJSON(string name, double data, bool isLast)
{
    string tmpStr;
    tmpStr = "\"" + name + "\":\"" + to_string(data) + "\"" + ((isLast)?"":",");
    return tmpStr;
}

string JSON::addToJSON(string name, uint64_t data, bool isLast)
{
	string tmpStr;
	tmpStr = "\"" + name + "\":" + to_string(data) + ((isLast)?"":",");
	return tmpStr;
}
string JSON::addStructToJSON(string name, string data, bool isLast)
{
	string tmpStr = "";
    if(!data.empty())
    {
        tmpStr = "\"" + name + "\":{" + data + "}" + ((isLast)?"":",");
    }
	return tmpStr;
}
string JSON::addStructToJSON( string data, bool isLast)
{
    string tmpStr = "";
    if(!data.empty())
    {
        tmpStr = "{" + data + "}" + ((isLast)?"":",");
    }
    return tmpStr;
}

JSONUpdate &JSONUpdate::operator=(const JSONUpdate &other)
{
    if ( this != &other )
    {
        memcpy( version, other.version, sizeof(version) );
        memcpy( url,     other.url, sizeof(url) );
        memcpy( md5,     other.md5, sizeof(md5) );
        status = other.status;
        fileSize = other.fileSize;
    }

    return *this;
}

JSONUpdate::JSONUpdate()
{
	memset( version, 0x00, sizeof(version) );
	memset( url,     0x00, sizeof(url) );
	memset( md5,     0x00, sizeof(md5) );
	status   = -1;
	fileSize =  0;
}




//=================================================================
// Парс полл таски delete_product
bool JSON::parseDeleteProduct(const JsonParser &jsonDelTask,
                              JSON_DELETE_PRODUCT &delStruct)
{
	delete_product_clear( &delStruct );

	JsonParser jsonData = jsonDelTask["data"];

	if( !jsonData.empty() )
	{
		delStruct.rem_id  = jsonData.value("rem_id" , "");
	}

	if( delStruct.rem_id.empty() )
	{
		logERR( "Ошибка в таске delete_product: '%s'",  jsonDelTask.dump().c_str());
		return false;
	}
	return true;
}


//=================================================================
// Парс полл таски key_activation
bool JSON::parseActivationKey( const JsonParser &jsonActTask,
                               JSON_KEY_ACTIVATION&    actStruct)
{
	key_activation_clear( &actStruct );

	JsonParser jsonData = jsonActTask["data"];

	if( !jsonData.empty() )
	{
        actStruct.rem_id  = jsonData.value("rem_id" , "");
        actStruct.key     = jsonData.value("key"    , "");
	}

	if( actStruct.rem_id.empty() || actStruct.key.empty() )
	{
		logERR( "Ошибка в таске key_activation: '%s'",  jsonActTask.dump().c_str());
		return false;
	}
	return true;
}

JSON::DISCOUNT_ERR JSON::parseUpsertDiscount(const JsonParser &jsonUpsertDiscount, JSON_UPSERT_DISCOUNT &upsertDiscount)
{
    logDBG("%s", __PRETTY_FUNCTION__);

    if(!convert)
    {
        logERR("%s Null ptr!", __PRETTY_FUNCTION__);
        return DISCOUNT_ERR_NULL_PTR;
    }

    upsert_discount_clear(upsertDiscount);

    if(jsonUpsertDiscount.is_null())
    {
        logERR("%s DISCOUNT_ERR_EMPTY_JSON!", __PRETTY_FUNCTION__);
        return DISCOUNT_ERR_EMPTY_JSON;
    }

    JsonParser discountData = jsonUpsertDiscount["data"];

    if(discountData.empty())
    {
        logERR("%s DISCOUNT_ERR_DATA_EMPTY!", __PRETTY_FUNCTION__);
        return DISCOUNT_ERR_DATA_EMPTY;
    }

    upsertDiscount.rem_id = discountData.value("rem_id", "");
    upsertDiscount.name   = discountData.value("name"  , "");

    if(upsertDiscount.rem_id.empty() || upsertDiscount.name.empty())
    {
        logERR("%s EMPTY DISCOUNT DATA!", __PRETTY_FUNCTION__);
        return DISCOUNT_ERR_DATA_EMPTY;
    }

    if(discountData.value("mode", "").empty())
    {
        logERR("%s EMPTY DISCOUNT MODE!", __PRETTY_FUNCTION__);
        return DISCOUNT_ERR_EMPTY_MODE;
    }

//    upsertDiscount.mode = DISC_MODE_MANUAL;
//    if(discountData.value("mode", "").compare("AUTO") == 0)
//    {
//        upsertDiscount.mode = DISC_MODE_AUTO;
//    }

    upsertDiscount.start_time = convert->dateToUnix( discountData.value("start_on_date" , ""));
    upsertDiscount.end_time   = convert->dateToUnix( discountData.value("finish_on_date", ""));

    JsonParser discountResult = discountData["result"];
    if(discountResult.empty())
    {
        logERR("%s DISCOUNT_ERR_RESULT_EMPTY!", __PRETTY_FUNCTION__);
        return DISCOUNT_ERR_RESULT_EMPTY;
    }

//    upsertDiscount.result.type = DISC_PER;
//    if(discountResult.value("type", "").compare("SUM") == 0)
//    {
//        upsertDiscount.result.type = DISC_RUB;
//    }
//    else if (discountResult.value("type", "").compare("FIXSUM") == 0)
//    {
//        upsertDiscount.result.type = DISC_FIX_RUB;
//    }
    upsertDiscount.result.value = to_string(discountResult.value("value", 0));

    JsonParser  discountTargets = discountData["targets"];
    JsonParser  discountProducts;
    if(!discountTargets.empty())
    {
        for(const JsonParser &discountTarget : discountTargets)
        {
            JsonParser discountTargetData = discountTarget["data"];
            if(discountTargetData.empty())
            {
                logERR("%s DISCOUNT_ERR_TARGET_DATA_EMPTY!", __PRETTY_FUNCTION__);
                return DISCOUNT_ERR_TARGET_DATA_EMPTY;
            }
            discountProducts = discountTargetData["products"];
            if(discountProducts.empty())
            {
                logERR("%s DISCOUNT_ERR_TARGET_PRODUCTS_EMPTY!", __PRETTY_FUNCTION__);
                return DISCOUNT_ERR_TARGET_PRODUCTS_EMPTY;
            }
            for(const JsonParser &discountProduct : discountProducts )
            {
                JSON_UPSDISC_PRODUCT product;
                product.rem_id = discountProduct.value("rem_id", "");
                if(product.rem_id.empty())
                {
                    logERR("%s DISCOUNT_ERR_EMPTY_PRODUCT_REM_ID!", __PRETTY_FUNCTION__);
                    return DISCOUNT_ERR_EMPTY_PRODUCT_REM_ID;
                }
                upsertDiscount.products.push_back( product );
            }
        }
    }

    EncodeConvertor ec;
    int errP = 0;
    upsertDiscount.name = ec.UTF8toCP866(upsertDiscount.name);

    logDBG("%s Success", __PRETTY_FUNCTION__);

    return DISCOUNT_ERR_SUCCESS;
}

JSON::DISCOUNT_ERR JSON::parseDeleteDiscount(const JsonParser &jsonDeleteDiscount, JSON_DELETE_DISCOUNT &deleteDiscount)
{
    logDBG("%s", __PRETTY_FUNCTION__);

    delete_discount_clear(deleteDiscount);

    if(jsonDeleteDiscount.is_null())
    {
        logERR("%s DISCOUNT_ERR_EMPTY_JSON!", __PRETTY_FUNCTION__);
        return DISCOUNT_ERR_EMPTY_JSON;
    }

    JsonParser delDiscountData = jsonDeleteDiscount["data"];

    if(delDiscountData.empty())
    {
        logERR("%s DISCOUNT_ERR_DATA_EMPTY!", __PRETTY_FUNCTION__);
        return DISCOUNT_ERR_DATA_EMPTY;
    }

    deleteDiscount.rem_id = delDiscountData.value("rem_id", "");

    if(deleteDiscount.rem_id.empty())
    {
        logERR("%s EMPTY DISCOUNT REM ID!", __PRETTY_FUNCTION__);
        return DISCOUNT_ERR_DATA_EMPTY;
    }

    logDBG("%s Success", __PRETTY_FUNCTION__);

    return DISCOUNT_ERR_SUCCESS;
}
//=================================================================
// Парс полл таски bind_product
bool JSON::parseBindProduct( const JsonParser &jsonBindTask,
                             JSON_BIND_PRODUCT &bindStruct)
{
	bind_product_clear( &bindStruct );

	JsonParser jsonData = jsonBindTask["data"];

	// required
	if( (jsonData.count("item_id")  == 0) || (jsonData.count("rem_id") == 0) )
	{
		logERR( "%s:: Ошибка в таске bind_product, не хватает обязательных полей: '%s'", __func__,
				                                                    jsonBindTask.dump().c_str() );
		return false;
	}

    // type
    if( (jsonData.at("item_id").is_string()  == false) || (jsonData.at("rem_id").is_string() == false) )
    {
        logERR( "%s:: Ошибка в таске bind_product, не хватает обязательных полей: '%s'", __func__,
                                                                     jsonBindTask.dump().c_str() );
        return false;
    }

	if( !jsonData.empty() )
	{
		bindStruct.item_id = jsonData.value("item_id", "");
		bindStruct.rem_id  = jsonData.value("rem_id" , "");
	}

	// not empty
	if(bindStruct.rem_id.empty() || bindStruct.item_id.empty())
	{
		logERR( "Ошибка в таске bind_product, пустые значения: '%s'",  jsonBindTask.dump().c_str());
		return false;
	}
	return true;
}

//=================================================================
// Парс полл таски upsert_product
bool JSON::parseUpsertProduct(JsonParser jsonUpsertTask,
                              JSON_UPSERT_PRODUCT&    upsertStruct)
{
	upsert_product_clear( &upsertStruct );

	JsonParser jsonData = jsonUpsertTask["data"];
	JsonParser jsonBarcodes;
	JsonParser jsonArticles;
	JsonParser jsonSizeData;

	// required
    if( (jsonData.count("tax")  == 0) || (jsonData.count("type") == 0) || (jsonData.count("rem_id") == 0))
	{
		logERR( "%s:: Ошибка в таске upsert_product, не хватает обязательных полей: '%s'", __func__,
				jsonUpsertTask.dump().c_str() );
		return false;
	}

	// type
	if( (jsonData.at("tax").is_string()  == false) || (jsonData.at("type").is_string() == false) )
	{
		logERR( "%s:: Ошибка в таске upsert_product, не хватает обязательных полей: '%s'", __func__,
				                                                    jsonUpsertTask.dump().c_str() );
		return false;
	}

	if( !jsonData.empty() )
	{
		upsertStruct.product.index                           = jsonData.value("index",         "");
		upsertStruct.product.meta.rem_id                     = jsonData.value("rem_id",        "");
		upsertStruct.product.meta.alcohol                    = jsonData.value("alcohol",    false);

        if (upsertStruct.product.meta.alcohol && jsonData.contains("alcohol_meta"))
        {
            JsonParser alcoholMeta = jsonData["alcohol_meta"];
            upsertStruct.product.meta.alcohol_meta.alc_code      = alcoholMeta.value("alc_code", "");
            upsertStruct.product.meta.alcohol_meta.alc_type_code = alcoholMeta.value("alc_type_code", "");
            upsertStruct.product.meta.alcohol_meta.alc_content   = alcoholMeta.value("alc_content", .00);
            upsertStruct.product.meta.alcohol_meta.capacity      = alcoholMeta.value("capacity", .00);
            upsertStruct.product.meta.alcohol_meta.keg           = alcoholMeta.value("keg", false);

            if (upsertStruct.product.meta.alcohol_meta.capacity == 0)
            {
                logWARN("%s alchohol capacity equal 0!", __FUNCTION__);
                return false;
            }
        }

		upsertStruct.product.meta.variety_name               = jsonData.value("variety_name",  "");
		upsertStruct.product.meta.sort_order                 = jsonData.value("sort_order",     0);

	}

	jsonSizeData = jsonUpsertTask["data"]["parent"];
	if( !jsonSizeData.empty() )
	{
		upsertStruct.product.meta.parent.parentRemID   = jsonSizeData.value("rem_id", "");
		upsertStruct.product.meta.parent.parentName    = jsonSizeData.value("name",   "");
	}
	jsonArticles = jsonUpsertTask["data"]["articles"];
	if( !jsonArticles.empty() )
	{
		upsertStruct.product.meta.article.clear();

		for ( const auto& iJsonArticle : jsonArticles )
		{
			upsertStruct.product.meta.article.append( iJsonArticle );
			upsertStruct.product.meta.article.append( "," );
		}
		if(!upsertStruct.product.meta.article.empty())
		{
			upsertStruct.product.meta.article.pop_back();
		}
	}

	jsonBarcodes = jsonUpsertTask["data"]["barcodes"];
	if( !jsonBarcodes.empty() )
	{
		upsertStruct.product.meta.barcode.clear();

		for( const auto& iJsonBarcode : jsonBarcodes)
		{
			upsertStruct.product.meta.barcode.append( iJsonBarcode );
			upsertStruct.product.meta.barcode.append( "," );
		}

		if(!upsertStruct.product.meta.barcode.empty())
		{
			upsertStruct.product.meta.barcode.pop_back();
		}
	}
	upsertStruct.product.meta.precision = jsonData.value("precision", 0);
	upsertStruct.product.meta.item      = jsonData.value("item",     "");
	upsertStruct.product.meta.measure   = jsonData.value("measure",  "");
	upsertStruct.product.meta.name      = jsonData.value("name",     "");
#ifdef BIOS_RB
	string tax = jsonData.value("tax", "");
	if (tax.compare( "NDS_MANUAL" ) != 0)
	{
		logWARN("Неверное значение поля 'tax' В джейсоне  tax='%s'", tax.c_str());
	}
	else
	{
        upsertStruct.product.meta.nds        = nds_manual;
        upsertStruct.product.meta.nds_manual = jsonData.value("tax_value", 0.0) / 100;
	}
#else
	string tax = jsonData.value("tax", "NDS_NO_TAX");
//	if (tax.compare( "NDS_0" ) == 0)
//	{
//		upsertStruct.product.meta.nds = nds_0;
//	}
//	else if (tax.compare( "NDS_10" ) == 0)
//	{
//		upsertStruct.product.meta.nds = nds_10;
//	}
//    else if ((tax.compare( "NDS_18" ) == 0) || (tax.compare( "NDS_20" ) == 0))
//	{
//		upsertStruct.product.meta.nds = nds_18;
//	}
//	else if (tax.compare( "NDS_10_CALCULATED" ) == 0)
//	{
//		upsertStruct.product.meta.nds = nds_10_110;
//	}
//    else if ((tax.compare( "NDS_18_CALCULATED" ) == 0) || (tax.compare( "NDS_20_CALCULATED" ) == 0))
//	{
//		upsertStruct.product.meta.nds = nds_18_118;
//	}
//	else
//	{
//		upsertStruct.product.meta.nds = nds_no;
//	}
#endif
	upsertStruct.product.meta.scale = false;
	string product_type =  jsonData.value("type", GOOD_TYPE_COUNTABLE);
//	if ( product_type.compare(GOOD_TYPE_COUNTABLE) == 0)
//	{
//		upsertStruct.product.meta.type = GoodsData::GOODS;
//	}
//	else if ( product_type.compare(GOOD_TYPE_SHOES) == 0 )
//	{
//		upsertStruct.product.meta.type = GoodsData::SHOES;
//	}
//	else if ( product_type.compare(GOOD_TYPE_CLOTHES) == 0 )
//	{
//		upsertStruct.product.meta.type = GoodsData::CLOTHES;
//	}
//	else if ( product_type.compare(GOOD_TYPE_SERVICE) == 0 )
//	{
//		upsertStruct.product.meta.type = GoodsData::SERVICE;
//	}
//    else if ( product_type.compare(GOOD_TYPE_TOBACCO) == 0 )
//    {
//        upsertStruct.product.meta.type = GoodsData::TOBACCO;
//    }
//    else if ( product_type.compare(GOOD_TYPE_ALCOHOL) == 0 )
//    {
//        upsertStruct.product.meta.type = GoodsData::ALCOHOL;
//    }
//	else if ( product_type.compare(GOOD_TYPE_SCALABLE) == 0 )
//	{
//		upsertStruct.product.meta.type  = GoodsData::WEIGHT;
//		upsertStruct.product.meta.scale = true;
//	}
//	else
	{
        upsertStruct.product.meta.type = 1;//GoodsData::GOODS;  !!!!!!!!!!!!!!!!!!!!!!!
	}
	upsertStruct.product.meta.price       = jsonData.value("price"     ,  0);
	upsertStruct.product.meta.depart_name = jsonData.value("group_name", "");

    EncodeConvertor ec;
    int errP = 0;

    upsertStruct.product.meta.originCountryCode = ec.UTF8toCP866(jsonData.value("origin_country_code", ""));
    upsertStruct.product.meta.customEntryNum = ec.UTF8toCP866(jsonData.value("custom_entry_num", ""));
    upsertStruct.product.meta.exciseDuty = jsonData.value("excise_duty", 0);
    upsertStruct.product.meta.marked = jsonData.value("marked", false);

    upsertStruct.product.meta.name        = ec.UTF8toCP866(upsertStruct.product.meta.name       );
    upsertStruct.product.meta.measure     = ec.UTF8toCP866(upsertStruct.product.meta.measure    );
    upsertStruct.product.meta.depart_name = ec.UTF8toCP866(upsertStruct.product.meta.depart_name);

    upsertStruct.product.meta.variety_name      = ec.UTF8toCP866(upsertStruct.product.meta.variety_name);
    upsertStruct.product.meta.parent.parentName = ec.UTF8toCP866(upsertStruct.product.meta.parent.parentName);
	return true;
}

//bool JSON::parsePurchaseDocumentResult(const JsonParser &task, JSON_PURCHASE_DOCUMENT_RESULT &purchaseResult) const
//{
//    logINFO("%s", __FUNCTION__);
//    bool valid = false;
//    if (!convert)
//    {
//        logERR("%s\tnullptr!", __FUNCTION__);
//        return valid;
//    }
//    try
//    {
//        valid =  task.contains("data")
//              && task["data"].contains("origin")
//              && task["data"].contains("result")
//              && task["data"]["origin"].contains("number")
//              && task["data"]["origin"].contains("shift");

//        if (valid)
//        {
//            JsonParser data = task["data"];
//            JsonParser origin = task["data"]["origin"];
//            JsonParser result = task["data"]["result"];

//            purchaseResult.number = origin.value("number", 0);
//            purchaseResult.shift = origin.value("shift", 0);
//            purchaseResult.taskId = task.value("task_id", 0);
//            purchaseResult.status = (data.value("status", "").compare("SUCCESS") == 0) ? SUCCESS : ERROR;

//            if (purchaseResult.status == SUCCESS)
//            {
//                valid =    result.contains("t")
//                        && result.contains("s")
//                        && result.contains("fn")
//                        && result.contains("i")
//                        && result.contains("fp")
//                        && result.contains("n");
//                if (valid)
//                {
//                    // парс даты UTC
//                    purchaseResult.date = convert->parseISO8601_2004(result.value("t", std::string()));
//                    // расчёт текущего часового пояса в секундах
//                    const int8_t tz {Ntpd::instance()->getZoneinfo().at(Ntpd::instance()->getCurrentZoneIndex()).offset};
//                    const time_t offset {tz * Convert::TIME_MIN_IN_HOUR * Convert::TIME_SEC_IN_MIN};
//                    // расчёт UTC unixtime
//                    purchaseResult.utc = Convert::dateIsoToTimet(purchaseResult.date) + offset;
//                    purchaseResult.sum = std::strtold(result.value("s", "").c_str(), nullptr);
//                    purchaseResult.fn = result.value("fn", "");
//                    purchaseResult.i = to_string(result.value("n", 0));
//                    purchaseResult.fp = result.value("fp", "");
//                    purchaseResult.fd = strtoull(result.value("i", "").c_str(), nullptr, 10);
//                }

//                if (data.contains("customer"))
//                {
//                    JsonParser customer = data["customer"];
//                    purchaseResult.phone = customer.value("phone", "");
//                }
//            }
//            else
//            {
//                valid = result.contains("code") && result.contains("message");
//                if (valid)
//                {
//                    EncodeConvertor ec;
//                    int errP = 0;
//                    purchaseResult.code = purchaseCodes.at(result.value("code", ""));
//                    purchaseResult.msg = ec.UTF8toCP866(result.value("message", ""));
//                }
//            }
//        }

//        if (!valid)
//        {
//            logERR("%s: Отсутствуют обязательные поля JSON :\n %s", __FUNCTION__, task.dump(OFFSET_OUTPUT).c_str());
//        }
//    }
//    catch (...)
//    {
//        // Исключение парса
//        logERR("%s Ooops!", __FUNCTION__);
//        // Вывод json исключения
//        logERR("%s", task.dump(OFFSET_OUTPUT).c_str());
//        valid = false;
//    }

//    return valid;
//}

//=================================================================
bool JSON::parseUpsertCashier(const JsonParser &task, JSON_UPSERT_CASHIER &upsertCashier) const
{
    logINFO("%s", __FUNCTION__);

    bool valid {false};

    try
    {
        upsertCashier.reset();

        valid  = task.contains("data")
                 && task["data"].contains("tab_number")
                 && task["data"].contains("name")
                 && task["data"].contains("password");
        if (valid)
        {
            EncodeConvertor ec;
            int errP = 0;

            JsonParser data = task["data"];
            upsertCashier.tabNumber = ec.UTF8toCP866(data.value("tab_number", ""));
            upsertCashier.name = ec.UTF8toCP866(data.value("name", ""));
            upsertCashier.inn = data.value("inn", "");
            upsertCashier.password = data.value("password", "");

            valid = !upsertCashier.tabNumber.empty() && !upsertCashier.name.empty() && !upsertCashier.password.empty();
            if (!valid)
            {
                upsertCashier.reset();
            }
        }
        else
        {
            logERR("%s: Отсутствуют обязательные поля JSON :\n %s", __FUNCTION__, task.dump(OFFSET_OUTPUT).c_str());
        }
    }
    catch (...)
    {
        // Исключение парса
        logERR("%s Ooops!", __FUNCTION__);
        // Вывод json исключения
        logERR("%s", task.dump(OFFSET_OUTPUT).c_str());
        valid = false;
    }

    return valid;
}
//=================================================================
bool JSON::parseDeleteCashier(const JsonParser &task, JSON_DELETE_CASHIER &deleteCashier) const
{
    logINFO("%s", __FUNCTION__);

    bool valid {false};

    try
    {
        deleteCashier.reset();

        valid  = task.contains("data") && task["data"].contains("tab_number");
        if (valid)
        {
            EncodeConvertor ec;
            int errP = 0;

            JsonParser data = task["data"];
            deleteCashier.tabNumber = ec.UTF8toCP866(data.value("tab_number", ""));

            valid = !deleteCashier.tabNumber.empty();
            if (!valid)
            {
                deleteCashier.reset();
            }
        }
        else
        {
            logERR("%s: Отсутствуют обязательные поля JSON :\n %s", __FUNCTION__, task.dump(OFFSET_OUTPUT).c_str());
        }
    }
    catch (...)
    {
        // Исключение парса
        logERR("%s Ooops!", __FUNCTION__);
        // Вывод json исключения
        logERR("%s", task.dump(OFFSET_OUTPUT).c_str());
        valid = false;
    }

    return valid;
}
//=================================================================
// Парс полл таски update_mrp_tables
//=================================================================
bool JSON::parseUpdateMRPtables(const JsonParser &task, JSON_UPDATE_MRP_TABLES &updateMRPtables) const
{
    logINFO("%s", __FUNCTION__);

    bool valid {false};

    try
    {
        updateMRPtables.reset();
        valid = task.contains("data") &&
                task["data"].contains("tables") &&
                task["data"]["tables"].is_array() &&
                !task["data"]["tables"].empty();
        if (!valid)
        {
            logERR("%s: Отсутствуют обязательные поля [data] или [data][tables]:\n%s", __FUNCTION__, task.dump(OFFSET_OUTPUT).c_str());
            return valid;
        }

//        updateMRPtables.mrpTables = task["data"]["tables"];
//        valid = !updateMRPtables.mrpTables.tables.empty();
    }
    catch (...)
    {
        // Исключение парса
        logERR("%s Ooops!", __FUNCTION__);
        // Вывод json исключения
        logERR("%s", task.dump(OFFSET_OUTPUT).c_str());
        valid = false;
    }

    return valid;
}
//=================================================================
// Парс полл таски external_purchase
JSON::EXTERNAL_PURCHASE_ERR JSON::parseExternalPurchase(const JsonParser       &jsonExPurchaseTask,
                                                        JSON_EXTERNAL_PURCHASE &exPurchaseStruct)
{
    logINFO("%s", __PRETTY_FUNCTION__);

    if (!convert)
    {
        logERR("%s\tnullptr!", __PRETTY_FUNCTION__);
        return EXTERNAL_PURCHASE_ERR_DATA_MISSING;
    }

    external_purchase_clear(&exPurchaseStruct);

    if (jsonExPurchaseTask.count("data") == 0 )
    {
        logERR("%s: Отсутствуют обязательное поле data/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
        return EXTERNAL_PURCHASE_ERR_DATA_MISSING;
    }

    exPurchaseStruct.task_id = jsonExPurchaseTask.value("task_id", 0);
    JsonParser jsonData = jsonExPurchaseTask["data"];
    if (jsonData.empty())
    {
        logERR("%s: Поле date пустое!/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
        return EXTERNAL_PURCHASE_ERR_DATA_EMPTY;
    }

    if (jsonData.count("rem_id") == 0 )
    {
        logERR("%s: Отсутствуют обязательное поле rem_id/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
        return EXTERNAL_PURCHASE_ERR_REM_ID_MISSING;
    }
    if (jsonData.count("type") == 0 )
    {
        logERR("%s: Отсутствуют обязательное поле type/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
        return EXTERNAL_PURCHASE_ERR_TYPE_MISSING;
    }
    if (jsonData.count("positions") == 0 )
    {
        logERR("%s: Отсутствуют обязательное поле positions/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
        return EXTERNAL_PURCHASE_ERR_POSITIONS_MISSING;
    }
    if (jsonData.count("payments") == 0 )
    {
        logERR("%s: Отсутствуют обязательное поле payments/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
        return EXTERNAL_PURCHASE_ERR_PAYMENTS_MISSING;
    }
    if (jsonData.count("total") == 0 )
    {
        logERR("%s: Отсутствуют обязательное поле total/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
        return EXTERNAL_PURCHASE_ERR_TOTAL_MISSING;
    }

    JSON_PAYMENT               payment;
    JsonParser jsonTotal    =  jsonData["total"];

    if ( jsonTotal.count("total_sum") == 0 )
    {
        logERR("%s: Отсутствуют обязательное поле total_sum/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
        return EXTERNAL_PURCHASE_ERR_TOTAL_SUM_MISSING;
    }

    exPurchaseStruct.rem_id                 = jsonData.value("rem_id"   , "");
    exPurchaseStruct.tax_mode               = jsonData.value("tax_mode" , "");
    exPurchaseStruct.type                   = jsonData.value("type" , "");

    if ( exPurchaseStruct.rem_id.empty() || exPurchaseStruct.type.empty() )
    {
        logERR("%s: Обязательное поле содержит пустое значение/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
        return EXTERNAL_PURCHASE_ERR_REQUIRED_FIELD_EMPTY;
    }

    if ( jsonData.count("attributes") != 0 )
    {
        exPurchaseStruct.attributes.email       = jsonData["attributes"].value("email" , "");
        if (!convert->isEmailValid(exPurchaseStruct.attributes.email))
        {
            exPurchaseStruct.attributes.email.clear();
        }
        exPurchaseStruct.attributes.phone       = jsonData["attributes"].value("phone" , "");
        if (!convert->isTelNumberValid(exPurchaseStruct.attributes.phone))
        {
            exPurchaseStruct.attributes.phone.clear();
        }
    }

    exPurchaseStruct.total.total_sum        = jsonTotal.value("total_sum", 0);

    if ( jsonTotal.count("taxes_sum") != 0 )
    {
        JsonParser jsonTotalTax =  jsonData["total"]["taxes_sum"];

        exPurchaseStruct.total.taxes.nds_10     = jsonTotalTax.value("NDS_10", 0);
        exPurchaseStruct.total.taxes.nds_18     = jsonTotalTax.value("NDS_18", 0);
        exPurchaseStruct.total.taxes.nds_10_110 = jsonTotalTax.value("NDS_10_CALCULATED", 0);
        exPurchaseStruct.total.taxes.nds_18_118 = jsonTotalTax.value("NDS_18_CALCULATED", 0);
    }

    JsonParser  jsonPositions = jsonData["positions"];

    EncodeConvertor ec;
    int errP = 0;
    if( !jsonPositions.empty() )
    {
        for(const auto &itJsonPos : jsonPositions)
        {
            if (itJsonPos.count("name") == 0)
            {
                logERR("%s: Отсутствуют обязательное поле name из positions / JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
                return EXTERNAL_PURCHASE_ERR_POSITIONS_NAME_MISSING;
            }
            if (itJsonPos.count("type") == 0)
            {
                logERR("%s: Отсутствуют обязательное поле type из positions / JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
                return EXTERNAL_PURCHASE_ERR_POSITIONS_TYPE_MISSING;
            }
            if (itJsonPos.count("quantity") == 0)
            {
                logERR("%s: Отсутствуют обязательное поле quantity из positions / JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
                return EXTERNAL_PURCHASE_ERR_POSITIONS_QUANTITY_MISSING;
            }
            if (itJsonPos.count("price") == 0)
            {
               logERR("%s: Отсутствуют обязательное поле price из positions / JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
                return EXTERNAL_PURCHASE_ERR_POSITIONS_PRICE_MISSING;
            }

            JSON_POSITION_EXTERNAL position_ex;
            position_ex.name     = ec.UTF8toCP866( itJsonPos.value("name" , ""));
            position_ex.type     = itJsonPos.value("type" , "");
            position_ex.quantity = itJsonPos.value("quantity" ,  0);
            position_ex.price    = itJsonPos.value("price"    ,  0);
            position_ex.total    = itJsonPos.value("total"    ,  0);
            position_ex.tax      = itJsonPos.value("tax"      , "");
            position_ex.tax_sum  = itJsonPos.value("tax_sum"  ,  0);

            if (itJsonPos.count("tags") != 0)
            {
                JsonParser jsonPositionsTags = itJsonPos["tags"];
                EXTERNAL_PURCHASE_ERR result = parseTags(jsonPositionsTags, position_ex);
                if (result != EXTERNAL_PURCHASE_ERR_SUCCESS)
                {
                    return result;
                }
            }

            if ( position_ex.name.empty() || position_ex.type.empty() )
            {
                logERR("%s: Обязательное поле содержит пустое значение/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
                return EXTERNAL_PURCHASE_ERR_REQUIRED_FIELD_EMPTY;
            }

            position_ex.remId = itJsonPos.value("rem_id",  "");

            exPurchaseStruct.positions_ex.push_back( position_ex );

            // Проверяем бьётся ли сумма по позиции
            bool isValid = isValidPosition(     position_ex.name,
                                                position_ex.type,
                                                position_ex.quantity,
                                                position_ex.price,
                                                position_ex.total );
            if( !isValid )
            {
                logERR("%s: Не бьется сумма по позиции/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
                return EXTERNAL_PURCHASE_ERR_INVALID_POSITION;
            }
        }
    }

    if (jsonData.count("tags") != 0)
    {
        EXTERNAL_PURCHASE_ERR result = parseTags(jsonData["tags"], exPurchaseStruct);
        if (result != EXTERNAL_PURCHASE_ERR_SUCCESS)
        {
            return result;
        }
    }

    JsonParser  jsonPayments = jsonData["payments"];
    if( !jsonPayments.empty() )
    {
        for(const auto &itJsonPay : jsonPayments)
        {
            if (itJsonPay.count("sum") == 0)
            {
                logERR("%s: Отсутствует обязательное поле sum в payments/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
                return EXTERNAL_PURCHASE_ERR_PAYMENTS_SUM_MISSING;
            }

            payment_clear    ( &payment     );

            payment.sum   = itJsonPay.value("sum",   0);
            payment.type  = itJsonPay.value("type", "");
            payment.count = itJsonPay.value("count", 0);

            exPurchaseStruct.payments.push_back( payment );
        }
    }
    else
    {
        logERR("%s: Отсутствует поле payments/ JSON:'%s'", __PRETTY_FUNCTION__, jsonExPurchaseTask.dump().c_str());
        return EXTERNAL_PURCHASE_ERR_PAYMENTS_MISSING;
    }

    return EXTERNAL_PURCHASE_ERR_SUCCESS;
}

//int JSON::parseKktRegistr(const string &jsonAnswer, JSONKKTRegInfoStr &regInfoStr, JSONKKTRegInfo &regInfo)
//{
//    int rc(POLL_PARSE_ERRORS::OK);

//    if (!JsonParser::accept(jsonAnswer))
//    {
//        logERR("%s Ошибка при попытке парса JSON с данными для регистрации!!!", __PRETTY_FUNCTION__);
//        return POLL_PARSE_ERRORS::BAD_JSON;
//    }

//    JsonParser jsonRegKkt = JsonParser::parse(jsonAnswer);

//    string result = jsonRegKkt["result"];
//    if (result.compare("OK") != 0)
//    {
//        regInfoStr.result = getAnswerError(result);
//        regInfo.result    = regInfoStr.result;
//        logERR("%s Ошибка в JSON с данными для регистрации (result : %s)!!!", __PRETTY_FUNCTION__, result.c_str());
//        return POLL_PARSE_ERRORS::BAD_RESULT;
//    }

//    int rcEncConv = 0;
//    EncodeConvertor encConv;

//    JsonParser jsonData = jsonRegKkt["data"];
//    if (!jsonData.empty())
//    {
//        JsonParser jsonShopInfo = jsonData["shop_info"];
//        if (!jsonShopInfo.empty())
//        {
//            regInfoStr.organizationName   = encConv.UTF8toCP866( jsonShopInfo.value("legal_name",   ""));
//            regInfoStr.address            = encConv.UTF8toCP866( jsonShopInfo.value("address",      ""));
//            regInfoStr.realAddress        = encConv.UTF8toCP866( jsonShopInfo.value("real_address", ""));
//            regInfoStr.inn                = jsonShopInfo.value("inn", "");
//            regInfoStr.kpp                = jsonShopInfo.value("kpp", "");
//            Convert::stringToChar(regInfoStr.kpp, regInfo.kpp, sizeof(regInfo.kpp));
//        }

//        JsonParser jsonRegistrationInfo   = jsonData["kkt_registration_info"];
//        if (!jsonRegistrationInfo.empty())
//        {
//            regInfoStr.regNumber          = jsonRegistrationInfo.value("registry_number", "");
//            regInfoStr.cashNumber         = jsonRegistrationInfo.value("cash_number",      0);
//            regInfoStr.isAutonomicMode    = jsonRegistrationInfo.value("autonomic",     true);

//            regInfo.isAutonomicMode       = regInfoStr.isAutonomicMode;
//            regInfoStr.senderEmail        = jsonRegistrationInfo.value("sender_email", "kassa@dreamkas.ru");
//            Convert::stringToChar(encConv.UTF8toCP866(jsonRegistrationInfo.value("automatic_device_number", ""), &rcEncConv),
//                                  regInfo.automaticDeviceNumber,
//                                  sizeof(regInfo.automaticDeviceNumber));

//            JsonParser jsonOfdProvider = jsonRegistrationInfo["ofd_provider"];
//            if (!jsonOfdProvider.empty())
//            {
//               regInfoStr.ofdName       = encConv.UTF8toCP866( jsonOfdProvider.value("name",        ""));
//               regInfoStr.ofdServerHost = encConv.UTF8toCP866( jsonOfdProvider.value("server_host", ""));
//               regInfoStr.ofdUrl        = encConv.UTF8toCP866( jsonOfdProvider.value("check_url",   ""));
//               regInfoStr.ofdServerPort = jsonOfdProvider.value("server_port",  0);
//               regInfoStr.ofdInn        = jsonOfdProvider.value("inn",         "");

//               regInfo.ofd_server_port = regInfoStr.ofdServerPort;
//            }

//            JsonParser jsonTaxModes = jsonRegistrationInfo["tax_modes"];
//            if ( !jsonTaxModes.empty() )
//            {
//                for ( const auto &tax : jsonTaxModes )
//                {
//                    string taxStr = tax;

//                    switch ( hashString(taxStr.c_str()) )
//                    {
//                        case hashString(TAX_DEFAULT):
//                        {
//                            regInfo.tax_systems |= TOTAL;
//                            break;
//                        }
//                        case hashString(TAX_SIMPLE):
//                        {
//                            regInfo.tax_systems |= SIMPLIFIED_INCOME;
//                            break;
//                        }
//                        case hashString(TAX_SIMPLE_WO):
//                        {
//                            regInfo.tax_systems |= SIMPLIFIED_INC_EXP;
//                            break;
//                        }
//                        case hashString(TAX_ENVD):
//                        {
//                            regInfo.tax_systems |= SINGLE_IMPUTED_INCOME;
//                            break;
//                        }
//                        case hashString(TAX_AGRICULT):
//                        {
//                            regInfo.tax_systems |= UNIFIED_AGRICULTURAL;
//                            break;
//                        }
//                        case hashString(TAX_PATENT):
//                        {
//                            regInfo.tax_systems |= PATENT;
//                            break;
//                        }
//                        default:
//                        {
//                            logERR("%s Неизвестный тип системы налогообложения в задании регистрации!!!", __PRETTY_FUNCTION__);
//                        }
//                    }
//                }
//            }

//            JsonParser jsonWorkMode = jsonRegistrationInfo["work_mode"];
//            if ( !jsonWorkMode.empty() )
//            {
//                for ( const auto &mode : jsonWorkMode )
//                {
//                    string modeStr = mode;

//                    switch ( hashString(modeStr.c_str()) )
//                    {
//                        case hashString(WORK_MODE_CIPHER):
//                        {
//                            regInfo.encryption_sign = true;
//                            break;
//                        }
//                        case hashString(WORK_MODE_EXCISE):
//                        {
//                            regInfo.excisable_sign = true;
//                            break;
//                        }
//                        case hashString(WORK_MODE_SERVICES):
//                        {
//                            regInfo.clc_services_sign = true;
//                            break;
//                        }
//                        case hashString(WORK_MODE_GAMBLING):
//                        {
//                            regInfo.gambling_sign = true;
//                            break;
//                        }
//                        case hashString(WORK_MODE_LOTTERY):
//                        {
//                            regInfo.lottery_sign = true;
//                            break;
//                        }
//                        case hashString(WORK_MODE_AUTOMATIC):
//                        {
//                            regInfo.automatic = true;
//                            break;
//                        }
//                        case hashString(WORK_MODE_PAY_AGENTS):
//                        {
//                            regInfo.paying_agent_sign = true;
//                            break;
//                        }
//                        case hashString(WORK_MODE_BSO):
//                        {
//                            regInfo.bso = true;
//                            break;
//                        }
//                        case hashString(WORK_MODE_INTERNET):
//                        {
//                            regInfo.internet = true;
//                            break;
//                        }
//                        default:
//                        {
//                            logERR("%s Неизвестный режим работы в задании регистрации!!!", __PRETTY_FUNCTION__);
//                        }
//                    }
//                }
//            }

//            JsonParser jsonAgents = jsonRegistrationInfo["agents"];
//            if ( !jsonAgents.empty() )
//            {
//                for ( const auto &agent : jsonAgents )
//                {
//                    string agentStr = agent.value("type", "");

//                    switch ( hashString(agentStr.c_str()) )
//                    {
//                        case hashString(AGENT_BANK_PAY_AGENT):
//                        {
//                            regInfo.agent_types |= BANK_PAYING_AGENT;
//                            break;
//                        }
//                        case hashString(AGENT_BANK_PAY_SUB_AGENT):
//                        {
//                            regInfo.agent_types |= BANK_PAYING_SUBAGENT;
//                            break;
//                        }
//                        case hashString(AGENT_PAY_AGENT):
//                        {
//                            regInfo.agent_types |= PAYING_AGENT;
//                            break;
//                        }
//                        case hashString(AGENT_PAY_SUB_AGENT):
//                        {
//                            regInfo.agent_types |= PAYING_SUBAGENT;
//                            break;
//                        }
//                        case hashString(AGENT_ATTORNEY):
//                        {
//                            regInfo.agent_types |= ATTORNEY_AGENT;
//                            break;
//                        }
//                        case hashString(AGENT_COMMISSIONAIRE):
//                        {
//                            regInfo.agent_types |= COMMISION_AGENT;
//                            break;
//                        }
//                        case hashString(AGENT_AGENT):
//                        {
//                            regInfo.agent_types |= SIMPLE_AGENT;
//                            break;
//                        }
//                        default:
//                        {
//                            logERR("%s Неизвестный агент в задании регистрации!!!", __PRETTY_FUNCTION__);
//                        }
//                    }
//                }
//            }
//        }

//        Convert::stringToChar(regInfoStr.senderEmail,        regInfo.senderEmail,         sizeof(regInfo.senderEmail));
//        Convert::stringToChar(regInfoStr.organizationName,   regInfo.organization_name,   sizeof(regInfo.organization_name));
//        Convert::stringToChar(regInfoStr.address,            regInfo.calculation_address, sizeof(regInfo.calculation_address));
//        Convert::stringToChar(regInfoStr.realAddress,        regInfo.calculation_place,   sizeof(regInfo.calculation_place));
//        Convert::stringToChar(regInfoStr.inn,                regInfo.organization_inn,    sizeof(regInfo.organization_inn));
//        Convert::stringToChar(regInfoStr.regNumber,          regInfo.kkt_reg_num,         sizeof(regInfo.kkt_reg_num));

//        Convert::stringToChar(regInfoStr.ofdName,        regInfo.ofd_name,                sizeof(regInfo.ofd_name));
//        Convert::stringToChar(regInfoStr.ofdInn,         regInfo.ofd_inn,                 sizeof(regInfo.ofd_inn));
//        Convert::stringToChar(regInfoStr.ofdUrl,         regInfo.check_receipt_address,   sizeof(regInfo.check_receipt_address));
//        Convert::stringToChar(regInfoStr.ofdServerHost,  regInfo.ofd_server_address,      sizeof(regInfo.ofd_server_address));
//    }
//    else
//    {
//        rc = POLL_PARSE_ERRORS::BAD_JSON;
//        regInfo.result    = rc;
//        regInfoStr.result = rc;
//    }

//    return rc;
//}

template<typename T>
JSON::EXTERNAL_PURCHASE_ERR JSON::parseTags(const JsonParser& jsonTags, T& src)
{
    if (!jsonTags.empty())
    {
        for (const auto& tag : jsonTags)
        {
            if ((tag.count("tag") == 0) || (tag.count("value") == 0))
            {
                logERR("%s: Отсутствуют поля в тегах ФФД JSON:'%s'", __PRETTY_FUNCTION__, tag.dump().c_str());
                return EXTERNAL_PURCHASE_ERR_TAGS_TAG;
            }

            if (!tag["tag"].is_number())
            {
                logERR("%s: Тег ФФД имеет неверный тип JSON:'%s'", __PRETTY_FUNCTION__, tag.dump().c_str());
                return EXTERNAL_PURCHASE_ERR_TAGS_TAG;
            }

            EXTERNAL_PURCHASE_ERR result = pushTags(tag, src);
            if (result != EXTERNAL_PURCHASE_ERR_SUCCESS)
            {
                logERR("%s: Ошибка парса тегов ФФД JSON:'%s'", __PRETTY_FUNCTION__, tag.dump().c_str());
                return result;
            }
        }
    }

    return EXTERNAL_PURCHASE_ERR_SUCCESS;
}

template<typename T>
JSON::EXTERNAL_PURCHASE_ERR JSON::pushTags(const JsonParser& tag, T& src)
{
    if (!tag.empty())
    {
        int32_t _tag = tag.value("tag", 0);

        if (tag["value"].is_structured() || tag["value"].is_null())
        {
            return EXTERNAL_PURCHASE_ERR_TAGS_TAG;
        }

        auto tagsAvailable = TAGS_RECEIPT_LIST;
        if (typeid(T).hash_code() == typeid(JSON_POSITION_EXTERNAL).hash_code())
        {
            tagsAvailable = TAGS_POSITION_LIST;
        }

//        if ( tagsAvailable.find(static_cast<EX_TAGS>(_tag)) == tagsAvailable.end() )
//        {
//            return EXTERNAL_PURCHASE_ERR_TAGS_NOT_AVAILABLE;
//        }
        if (tag["value"].is_number_unsigned())
        {
            src.tags[_tag] = TAG_VALUE( static_cast<uint64_t>(tag.value("value", 0)) );
            return EXTERNAL_PURCHASE_ERR_SUCCESS;
        }
        if (tag["value"].is_string())
        {
            EncodeConvertor ec;
            int errP(0);
            src.tags[_tag] = TAG_VALUE(ec.UTF8toCP866(tag.value("value", string())));
            return EXTERNAL_PURCHASE_ERR_SUCCESS;
        }

        if (tag["value"].is_boolean())
        {
            src.tags[_tag] = TAG_VALUE(tag.value("value", false));
            return EXTERNAL_PURCHASE_ERR_SUCCESS;
        }
    }
    return EXTERNAL_PURCHASE_ERR_TAGS_TAG;
}
