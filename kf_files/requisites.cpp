/*
 * requisites.cpp
 *
 *  Created on: 18 окт. 2016 г.
 *      Author: user
 */

#include "requisites.h"
#include <string.h>

#include "config.h"    // !!!!!!!!!!!!!!!!!!

using namespace std;

Requisites::Requisites()
{
    req_table_init();
    req_names_table_init();
}

Requisites::~Requisites()
{

}

Requisites::Requisite::Requisite()
{
    name.               clear();
    print_tempale.      clear();
    is_print_tempale    = false;
    electron_tempale.   clear();
    is_electron_tempale = false;
    tag                 =     0;
    is_len_fix          = false;
    cur_len             =     0;
    max_len             =     0;
    type_doc_mask       =  0x00;
    type                =  0x00;
    value_int           =     0;
    value_str           =  NULL;
    value_byte          =  NULL;
}

Requisites::Requisite::~Requisite()
{
    name.               clear();
    print_tempale.      clear();
    is_print_tempale    = false;
    electron_tempale.   clear();
    is_electron_tempale = false;
    tag                 =     0;
    is_len_fix          = false;
    cur_len             =     0;
    max_len             =     0;
    type_doc_mask       =  0x00;
    type                =  0x00;
    value_int           =     0;
}

Requisites::REQ_NAME::REQ_NAME( Requisites::Requisite &requisite )
{
    tag          = requisite.tag;
    req_name_str = requisite.name;
}

Requisites::REQ_NAME::~REQ_NAME()
{
}

// Копирование имени реквизита
Requisites::REQ_NAME Requisites::REQ_NAME::operator = (const Requisites::REQ_NAME &req_name)
{
    this->tag          =          req_name.tag;
    this->req_name_str = req_name.req_name_str;

    return *this;
}

// Копирование реквизита
Requisites::Requisite& Requisites::Requisite::operator=( const Requisites::Requisite &requisite )
{
    if ( this != &requisite )
    {
        tag                 =                 requisite.tag;
        type                =                requisite.type;
        is_len_fix          =          requisite.is_len_fix;
        cur_len             =             requisite.cur_len;
        max_len             =             requisite.max_len;
        value_int           =           requisite.value_int;
        value_str           =           requisite.value_str;
        value_byte          =          requisite.value_byte;
        type_doc_mask       =       requisite.type_doc_mask;
        is_electron_tempale = requisite.is_electron_tempale;
        is_print_tempale    =    requisite.is_print_tempale;
        electron_tempale    =    requisite.electron_tempale;
        print_tempale       =       requisite.print_tempale;
        requisites          = requisite.requisites;
    }

    return *this;
}

//=======================================================================================================
//                                 Очищение реквизита
//=======================================================================================================
int Requisites::req_clear( Requisite& req )
{
    req.cur_len = 0;
    req.electron_tempale.erase( req.electron_tempale.begin(), req.electron_tempale.end() );
    req.is_electron_tempale = false;
    req.is_len_fix = false;
    req.is_print_tempale = false;
    req.max_len = 0;
    req.name.erase( req.name.begin(), req.name.end() );
    req.print_tempale.erase( req.print_tempale.begin(), req.print_tempale.end() );
    req.tag = 0;
    req.type = 0;
    req.type_doc_mask = 0;
    req.value_int = 0;
    req.value_str = NULL;
    req.value_byte = NULL;
    req.requisites.clear();
    return 0;
}
//=======================================================================================================
//                                 Конструктор таблицы реквизитов
//=======================================================================================================
int Requisites::req_table_init()
{
    logINFO( "req_table_init" );
    // Очистка таблицы
    requisites_list.erase( requisites_list.begin(), requisites_list.end() );

    Requisite req;
    //-------------------------------------------------------------------------------------------------------
    //                                   Наименование документа
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "НАИМЕНОВАНИЕ ДОКУМЕНТА";
    req.tag = 1000;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 64;
    req.type_doc_mask = print;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      //Номер версии ФФД
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "НОМЕР ВЕРСИИ ФФД";
    req.tag = 1209;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Код причины перерегистрации
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ИЗМ. СВЕД. О ККТ";
    req.tag = 1101;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
        //-------------------------------------------------------------------------------------------------------
    //                                      // Код товарной номенклатуры
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "КТ";
    req.tag = TAG_NOMENCLATURE_CODE;
    req.type = BYTE;
    req.is_len_fix = len_not_fix;
    req.max_len = 32;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                              АКЦИЗ
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "АКЦИЗ";
    req.tag = TAG_EXCISE_DUTY;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                          КОД СТРАНЫ
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "КОД СТРАНЫ";
    req.tag = TAG_ORIGIN_COUNTRY_CODE;
    req.type = ASCII;
    req.is_len_fix = len_fix;
    req.max_len = 3;
    req.type_doc_mask = print;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                          ДЕКЛАРАЦИЯ
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ДЕКЛАРАЦИЯ";
    req.tag = TAG_CUSTOM_ENTRY_NUM;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 32;
    req.type_doc_mask = print;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                          ИНН покупателя
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ИНН ПОКУПАТЕЛЯ";
    req.tag = TAG_BUYER_INN;
    req.type = ASCII;
    req.is_len_fix = len_fix;
    req.max_len = 12;
    req.type_doc_mask = print;
    requisites_list.push_back(req);
    //-------------------------------------------------------------------------------------------------------
    //                                          Покупатель
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПОКУПАТЕЛЬ";
    req.tag = TAG_BUYER_NAME;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.type_doc_mask = print;
    requisites_list.push_back(req);
    //-------------------------------------------------------------------------------------------------------
    //                                дополнительный реквизит пользователя
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ДОП. РЕКВИЗИТ ПОЛЬЗ.";
    req.tag = TAG_ADD_USER_REQ;
    req.type = STLV;
    req.is_len_fix = len_not_fix;
    req.max_len = 320;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                наименование дополнительного реквизита пользователя
    //-------------------------------------------------------------------------------------------------------
    req_clear(req);
    req.name = "НАИМЕНОВАНИЕ ДОП. РЕК. ПОЛЬЗ.";
    req.tag = TAG_ADD_USER_NAME;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 64;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back(req);
    //-------------------------------------------------------------------------------------------------------
    //                                значение дополнительного реквизита пользователя
    //-------------------------------------------------------------------------------------------------------
    req_clear(req);
    req.name = "ЗНАЧЕНИЕ ДОП. РЕК. ПОЛЬЗ.";
    req.tag = TAG_ADD_USER_VALUE;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back(req);
    //-------------------------------------------------------------------------------------------------------
    //                                дополнительный реквизит предмета расчёта
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ДОП. РЕКВИЗИТ ПР. РАСЧ.";
    req.tag = TAG_ADD_POS_REQ;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 64;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                дополнительный реквизит чека (БСО)
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ДОП. РЕКВИЗИТ ЧЕКА (БСО)";
    req.tag = TAG_ADD_REC_REQ;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 16;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      //ИЗМ. СВЕД. О ККТ
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ИЗМ. СВЕД. О ККТ";
    req.tag = 1205;
    req.type = INT_32LE;
    req.is_len_fix = len_fix;
    req.cur_len = 4;
    req.max_len = 4;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                 Наименование пользователя
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "НАИМЕНОВАНИЕ ПОЛЬЗОВАТЕЛЯ";
    req.tag = 1048;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                    Адрес отправителя
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ЭЛ. АДР. ОТПРАВИТЕЛЯ";
    req.tag = 1117;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 64;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                 ПЕРВЫЙ НЕПЕРЕДАННЫЙ ФД
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПЕРВЫЙ НЕПЕРЕДАННЫЙ ФД";
    req.tag = 1116;
    req.type = INT_32LE;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Инн пользователя
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ИНН";
    req.tag = 1018;
    req.type = ASCII;
    req.is_len_fix = len_fix;
    req.max_len = 12;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Наименование основания для коррекции
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ОПИСАНИЕ КОРРЕКЦИИ";
    req.tag = 1177;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Номер документа основания для коррекции
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "НОМЕР ДОКУМЕНТА ОСНОВАНИЯ ДЛЯ КОРРЕКЦИИ";
    req.tag = 1179;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 12;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      //системы налогооблажения
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СНО";     //"СНО"
    req.tag = 1062;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Дата, время
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ДАТА, ВРЕМЯ";
    req.tag = 1012;
    req.type = UNIX_TIME;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Дата документа основания для коррекции
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ДАТА ДОКУМЕНТА ОСНОВАНИЯ ДЛЯ КОРРЕКЦИИ";
    req.tag = 1178;
    req.type = UNIX_TIME;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Регистрационный номер ККТ
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "РН ККТ";
    req.tag = 1037;
    req.type = ASCII;
    req.is_len_fix = len_fix;
    req.max_len = 20;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Заводской номер ККТ
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ЗН ККТ";
    req.tag = 1013;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 20;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак автономного режима
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "АВТОНОМН. РЕЖИМ";
    req.tag = 1002;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак установки принтера в автомате
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРИНТЕР В АВТОМАТЕ";
    req.tag = 1221;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак установки принтера в автомате
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "АВТОМАТ";
    req.tag = 1036;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 20;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                              ИНН поставщика
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ИНН поставщика";
    req.tag = 1226;
    req.type = ASCII;
    req.is_len_fix = len_fix;
    req.max_len = 12;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак АС БСО
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "АС БСО";
    req.tag = 1110;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак шифрования
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ШФД";
    req.tag = 1056;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Признак агента по предмету расчёта
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРИЗНАК АГЕНТА";
    req.tag = 1022;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак автоматического режима
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "АВТОМАТ. РЕЖИМ";
    req.tag = 1001;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак ККТ для расчётов только в Интернет
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ККТ ДЛЯ ИНТЕРНЕТ";
    req.tag = 1108;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      номер автомата
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "АВТОМАТ";
    req.tag = 1036;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 21;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      продажа подакцизного товара
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПОДАКЦИЗНЫЕ ТОВАРЫ";
    req.tag = 1207;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак расчетов за услуги **************************************
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ККТ ДЛЯ УСЛУГ";
    req.tag = 1109;
    req.type = BYTE;
    req.is_len_fix = len_not_fix/*len_fix*/;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак проеведения азартных игр
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРОВЕДЕНИЕ АЗАРТНОЙ ИГРЫ";
    req.tag = 1193;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак проведения лотереи
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРОВЕДЕНИЕ ЛОТЕРЕИ";
    req.tag = 1126;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      признак платёжного агента
    //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "ПРИЗНАК АГЕНТА";
     req.tag = 1057;
     req.type = BYTE;
     req.is_len_fix = len_fix;
     req.max_len = 1;
     req.cur_len = req.max_len;
     req.type_doc_mask = print_electron;
     requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                               признак платёжного агента на позицию
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРИЗНАК АГЕНТА";
    req.tag = 1222;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
     //-------------------------------------------------------------------------------------------------------
     //                                      телефон платёжного агента
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "ТЛФ. ПЛ. АГЕНТА";
     req.tag = 1073;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 19;
     req.type_doc_mask = print_electron;
     requisites_list.push_back( req );
     //-------------------------------------------------------------------------------------------------------
     //                                      телефон оператора по приёму платежей
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "ТЛФ. ОП. ПР. ПЛАТЕЖА";
     req.tag = 1074;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 19;
     req.type_doc_mask = print_electron;
     requisites_list.push_back( req );
     //-------------------------------------------------------------------------------------------------------
     //                                      телефон поставщика
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "ТЛФ. ПОСТ.";
     req.tag = 1171;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 19;
     req.type_doc_mask = print_electron;
     requisites_list.push_back( req );
     //-------------------------------------------------------------------------------------------------------
     //                                   Наименование поставщика
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "НАИМЕНОВАНИЕ ПОСТАВЩИКА";
     req.tag = 1225;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 256;
     req.type_doc_mask = print_electron;
     requisites_list.push_back( req );
     //-------------------------------------------------------------------------------------------------------
     //                                      телефон оператора перевода
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "ТЛФ. ОП. ПЕРЕВОДА";
     req.tag = 1075;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 19;
     req.type_doc_mask = print_electron;
     requisites_list.push_back( req );
     //-------------------------------------------------------------------------------------------------------
     //                                      операция платёжного агента
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "ОП. АГЕНТА";
     req.tag = 1044;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 24;
     req.type_doc_mask = print_electron;
     requisites_list.push_back( req );
     //-------------------------------------------------------------------------------------------------------
     //                                      наименование опертор перевода
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "ОПЕРАТОР ПЕРЕВОДА";
     req.tag = 1026;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 64;
     req.type_doc_mask = print_electron;
     requisites_list.push_back( req );
     //-------------------------------------------------------------------------------------------------------
     //                                      адрес оператора перевода
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "АДР. ОП. ПЕРЕВОДА";
     req.tag = 1005;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 256;
     req.type_doc_mask = print_electron;
     requisites_list.push_back( req );
     //-------------------------------------------------------------------------------------------------------
     //                                      ИНН оператора перевода
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "ИНН ОП. ПЕРЕВОДА";
     req.tag = 1016;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 12;
     req.type_doc_mask = print_electron;
     requisites_list.push_back( req );
    // УСТАРЕЛ-----------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------------
    //                                      признак банковского платёжного агента
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРИЗНАК";
    req.tag = 1058;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-----------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------------
    //                                      Кассир
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "КАССИР";
    req.tag = 1021;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 64;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      ИНН кассира
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ИНН КАССИРА";
    req.tag = 1203;
    req.type = ASCII;
    req.is_len_fix = len_fix;
    req.max_len = 12;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Адрес расчётов
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "АДРЕС РАСЧЁТОВ";
    req.tag = 1009;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      место расчётов
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "МЕСТО РАСЧЁТОВ";
    req.tag = 1187;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      ИНН ОФД
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ИНН ОФД";
    req.tag = 1017;
    req.type = ASCII;
    req.is_len_fix = len_fix;
    req.max_len = 12;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Наименование ОФД
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ОФД";
    req.tag = 1046;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Адрес сайта ФНС
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "САЙТ ФНС";
    req.tag = 1060;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Эл. адрес отправителя чека
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ЭЛ. АДР. ОТПРАВИТЕЛЯ";
    req.tag = 1117;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 64;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      версия ККТ
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ВЕР. ККТ";     //вер.ККТ
    req.tag = 1188;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 8;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      версия ФФД ККТ
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ФФД ККТ";
    req.tag = 1189;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Тип коррекции
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ТИП КОРРЕКЦИИ";
    req.tag = 1173;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      версия ФФД ФН
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ФФД ФН";
    req.tag = 1190;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                     ресурс ключей ФП
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "РЕСУРС КЛЮЧЕЙ";
    req.tag = 1213;
    req.type = INT_16LE;
    req.is_len_fix = len_fix;
    req.max_len = 2;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      номер ФД
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ФД";
    req.tag = 1040;
    req.type = INT_32LE;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      номер ФН
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ФН";
    req.tag = 1041;
    req.type = ASCII;
    req.is_len_fix = len_fix;
    req.max_len = 16;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      ФПД
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ФП";
    req.tag = 1077;
    req.type = BYTE; //BYTE[1l]
    req.is_len_fix = len_fix;
    req.max_len = 6;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
     /*
     //-------------------------------------------------------------------------------------------------------
     //                                      ФПС (4)
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "Адрес (место) расчётов";
     req.tag = -;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 256;
     req.type_doc_mask = electron;
     requisites_list.push_back( req );
     */
     /*
     //-------------------------------------------------------------------------------------------------------
     //                                      ФПА (5)
     //-------------------------------------------------------------------------------------------------------
     req_clear( req );
     req.name = "Адрес (место) расчётов";
     req.tag = -;
     req.type = ASCII;
     req.is_len_fix = len_not_fix;
     req.max_len = 256;
     req.type_doc_mask = electron;
     requisites_list.push_back( req );
     */
//=======================================================================================================
//=======================================================================================================
//=======================================================================================================
//-------------------------------------------------------------------------------------------------------
//                                      Номер смены
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СМЕНА";
    req.tag = 1038;
    req.type = INT_32LE;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                 Порядковый номер фискального документа
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ФД";
    req.tag = 1040;
    req.type = INT_32LE;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                Фискальный признак документа
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ФП";
    req.tag = 1077;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 6;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                Данные агента
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ДАННЫЕ АГЕНТА";
    req.tag = 1223;
    req.type = STLV;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                Данные поставщика
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ДАННЫЕ ПОСТАВЩИКА";
    req.tag = 1224;
    req.type = STLV;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                Сообщение оператора
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СООБЩЕНИЕ ОПЕРАТОРА";
    req.tag = 1206;
    req.type = STLV;
    req.is_len_fix = len_fix;
    req.max_len = 328;
    req.cur_len = req.max_len;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                Дополнительный реквизит
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ДОПОЛНИТЕЛЬНЫЙ РЕКВИЗИТ ПОЛЬЗОВАТЕЛЯ";
    req.tag = 1084;
    req.type = STLV;
    req.is_len_fix = len_not_fix;
    req.max_len = 328;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                Основание для коррекции
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ОСН. ДЛЯ КОРР.";
    req.tag = 1174;
    req.type = STLV;
    req.is_len_fix = len_not_fix;
    req.max_len = 292;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                               Отчёт об открытии смены
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "Отчёт об открытии смены";
    req.tag = 2;
    req.type = INT_16LE;
    req.is_len_fix = len_fix;
    req.max_len = 2;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                      Номер смены
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СМЕНА";
    req.tag = 1038;
    req.type = INT_32LE;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                           Количество кассовых чеков (БСО) за смену
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ЧЕКОВ ЗА СМЕНУ";
    req.tag = 1118;
    req.type = INT_32LE;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                           Общее количество ФД за смену
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ФД ЗА СМЕНУ";
    req.tag = 1111;
    req.type = INT_32LE;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                           Количество непереданных ФД
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "НЕПЕРЕДАННЫХ ФД";
    req.tag = 1097;
    req.type = INT_32LE;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                     Дата и время первого из непереданных ФД
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ФД НЕ ПЕРЕДАНЫ С";
    req.tag = 1098;
    req.type = UNIX_TIME;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                             Номер чека за смену
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ЧЕК";
    req.tag = 1042;
    req.type = INT_32LE;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                               Признак расчёта
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРИЗНАК РАСЧЁТА";
    req.tag = 1054;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                               Применяемая система налогооблажения
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СНО";
    req.tag = 1055;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                      Предмет расчёта
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРЕДМЕТ РАСЧЁТА";
    req.tag = 1059;
    req.type = STLV;
    req.is_len_fix = len_not_fix;
    req.max_len = 1024;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                 Наименование предмета расчёта
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "НАИМЕНОВАНИЕ ПРЕДМЕТА РАСЧЁТА";
    req.tag = 1030;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 128;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Цена за единицу предмета расчёта
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ЦЕНА ЗА ЕДИНИЦУ ПРЕДМЕТА РАСЧЁТА";
    req.tag = 1079;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                 Признак способа расчёта
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРИЗНАК СПОСОБА РАСЧЁТА";
    req.tag = 1214;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Сумма по чеку предоплатой
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРЕДВАРИТЕЛЬНАЯ ОПЛАТА (АВАНС)";
    req.tag = 1215;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Сумма по чеку постоплатой
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПОСЛЕДУЮЩАЯ ОПЛАТА (КРЕДИТ)";
    req.tag = 1216;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Признак исчерпания ресурса ФН
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "РЕСУРС ФН МЕНЕЕ 30 ДНЕЙ";
    req.tag = 1050;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Признак исчерпания ресурса ФН
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "РЕСУРС ФН МЕНЕЕ 3 ДНЕЙ";
    req.tag = 1051;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Признак переполнения памяти ФН
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПАМЯТЬ ФН ЗАПОЛНЕНА";
    req.tag = 1052;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Признак превышения времени ожидания ответа ОФД
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ОФД НЕ ОТВЕЧАЕТ";
    req.tag = 1053;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Сумма НДС 10/110
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СУММА НДС 10/110";
    req.tag = 1107;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Сумма НДС 18/118
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СУММА НДС 18/118";
    req.tag = 1106;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Сумма без НДС
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СУММА без НДС";
    req.tag = 1105;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Сумма с НДС 0%
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СУММА С НДС 0%";
    req.tag = 1104;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Сумма НДС 10%
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СУММА НДС 10%";
    req.tag = 1103;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Сумма НДС 18%
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СУММА НДС 18%";
    req.tag = 1102;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Количество предмета расчёта
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "КОЛИЧЕСТВО ПРЕДМЕТА РАСЧЁТА";
    req.tag = 1023;
    req.type = FVLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 8;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                              Стоимость предмета расчёта
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "СТОИМОСТЬ ПРЕДМЕТА РАСЧЁТА";
    req.tag = 1043;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                      Сумма расчёта
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ИТОГ";
    req.tag = 1020;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                      Сумма наличными
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "НАЛИЧНЫМИ";
    req.tag = 1031;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                      Сумма электронными
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "БЕЗНАЛИЧНЫМИ";
    req.tag = 1081;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
    //-------------------------------------------------------------------------------------------------------
    //                                      Сумма встречным представлением
    //-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ИНАЯ ФОРМА ОПЛАТЫ";
    req.tag = 1217;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                             Адрес электронной почты отправителя чека
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ЭЛ.АДР.ОТПРАВИТЕЛЯ";
    req.tag = 1195;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 64;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                      Адрес сайта ФНС
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "САЙТ ФНС";
    req.tag = 1060;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 256;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                        QR - код
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "QR - код";
    req.tag = 1196;
    req.type = UNIX_TIME;
    req.is_len_fix = len_fix;
    req.max_len = 4;
    req.cur_len = req.max_len;
    req.type_doc_mask = electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                        НДС 0%
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ИТОГО с НДС 0";
    req.tag = 1104;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                       Cумма НДС за предмет расчёта
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "НДС ПР";
    req.tag = 1200;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                  Размер НДС за единицу предмета расчета
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "НДС ЗА. ЕД. ПР";
    req.tag = 1198;
    req.type = VLN;
    req.is_len_fix = len_not_fix;
    req.max_len = 6;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                       Ставка НДС за предмет расчёта
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "НДС";
    req.tag = 1199;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                                       Признак предмета расчёта
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ПРИЗНАК ПРЕДМЕТА РАСЧЁТА";
    req.tag = 1212;
    req.type = BYTE;
    req.is_len_fix = len_fix;
    req.max_len = 1;
    req.type_doc_mask = print_electron;
    requisites_list.push_back( req );
//-------------------------------------------------------------------------------------------------------
//                              Телефон или электронный адрес покупателя
//-------------------------------------------------------------------------------------------------------
    req_clear( req );
    req.name = "ТЕЛ. ИЛИ ЭЛ. АДРЕС ПОКУПАТЕЛЯ";
    req.tag = 1008;
    req.type = ASCII;
    req.is_len_fix = len_not_fix;
    req.max_len = 64;
    req.type_doc_mask = print;
    requisites_list.push_back( req );
//=======================================================================================================
//=======================================================================================================
    logINFO( "Size req_list = %d", requisites_list.size() );
    return 0;
}

bool Requisites::isReqSTLV( uint16_t tag )
{
    logDBG( "REQUISITE %d STLV TEST ... ", tag );

    for ( auto it = requisites_list.begin(); it != requisites_list.end(); ++it )
    {
        if (tag == (*it).tag)
        {
            if ((*it).type == STLV)
            {
                logDBG( "REQUISITE %d IS STLV", tag );
                return true;
            }
        }
    }

    return false;
}

int Requisites::req_search( unsigned short tag, Requisite *req )
{
    logDBG( "REQUISITE SEARCHING IN TABLE ... " );

    int count = 0;

    for ( auto it = requisites_list.begin(); it != requisites_list.end(); ++it )
    {
        count++;
        if (tag == (*it).tag)
        {
            *req = (*it);
            logDBG( "REQUISITE %d FOUND", tag );
            return EXIT_SUCCESS;
        }
    }

    // Реквизит не найден
    logDBG( "REQUISITE %d NOT FOUND!", tag );

    return EXIT_FAILURE;
}

int Requisites::tlv_to_list_add_uch( unsigned short tag, unsigned char *bytes, unsigned short len, vector <Requisite> &new_req_list )
{
    //logINFO( "=============================================================" );
    //logINFO( "Requisites::tlv_to_list_add" );
    Requisite tmp_req;
    // Поиск нужного реквизита в общем листе реквизитов
    if (req_search( tag, &tmp_req ) == EXIT_FAILURE)
    {
        return -1;
    }
    //logINFO( "Requisites::req initialization" );
    // Инициализация реквизита (value -> requisites)
    if ((tmp_req.type == BYTE) || (tmp_req.type == STLV))
    {
        // Тип BYTE или STLV
        //В случае фиксированной длины задаём максимально допустимой
        if (tmp_req.is_len_fix)
        {
            //logINFO( "Requisites::len fix!" );
            tmp_req.cur_len = tmp_req.max_len;
        }
        else
        {
            //logINFO( "Requisites::len not fix!" );
            // В случае нефиксированной длины задаём длину
            if (len > tmp_req.max_len)
            {
                //Размер превышает макимально допустимый
                tmp_req.cur_len = tmp_req.max_len;
                len = tmp_req.max_len;
            }
            else
            {
                tmp_req.cur_len = len;
            }
        }
        // Выделяем память для реквизита
        tmp_req.value_byte = new unsigned char[tmp_req.cur_len];
        memset( tmp_req.value_byte, 0, tmp_req.cur_len);
        memcpy( tmp_req.value_byte, bytes, tmp_req.cur_len);
        // Добавление реквизита в лист реквизитов
        new_req_list.push_back( tmp_req );
        return 0;
    }
    logINFO( "Requisites::wrong type of value!" );
    // Неверный тип value
    return -1;
}

int Requisites::tlv_to_list_add_str( unsigned short tag, const char *text,  vector <Requisite> &new_req_list )
{
    //logINFO( "=============================================================" );
    //logINFO( "Requisites::tlv_to_list_add" );
    Requisite tmp_req;
    uint32_t len = strlen(text);
    // Поиск нужного реквизита в общем листе реквизитов
    if (req_search( tag, &tmp_req ) == EXIT_FAILURE)
    {
        return -1;
    }
    //logINFO( "Requisites::req initialization" );
    // Инициализация реквизита (value -> requisites)
    if (tmp_req.type == ASCII)
    {
        // Тип ASCII
        //В случае фиксированной длины задаём максимально допустимой
        if (tmp_req.is_len_fix)
        {
            tmp_req.cur_len = tmp_req.max_len;
        }
        else
        {
            // В случае нефиксированной длины задаём длину
            if (len > tmp_req.max_len)
            {
                //Размер превышает макимально допустимый
                tmp_req.cur_len = tmp_req.max_len;
                len = tmp_req.max_len;
            }
            else
            {
                tmp_req.cur_len = len;
            }
        }
        // Выделяем память для реквизита
        tmp_req.value_str = new unsigned char[tmp_req.cur_len];
        memset( tmp_req.value_str, 0, tmp_req.cur_len );
        if (tmp_req.cur_len > strlen( text ))
        {
            strncpy( reinterpret_cast<char*>(tmp_req.value_str), text, strlen(text) );
        }
        else
        {
            strncpy( reinterpret_cast<char*>(tmp_req.value_str), text, tmp_req.cur_len );
        }
        // Добавление реквизита в лист реквизитов
        new_req_list.push_back( tmp_req );
        return 0;
    }
    // Неверный тип value
    logINFO( "Requisites::wrong type of value" );
    return -1;
}

int Requisites::tlv_to_list_add_int32( unsigned short tag, int32_t value, vector <Requisite> &new_req_list )
{
    //logINFO( "=============================================================" );
    //logINFO( "Requisites::tlv_to_list_add" );
    Requisite tmp_req;
    // Поиск нужного реквизита в общем листе реквизитов
    if (req_search( tag, &tmp_req ) == EXIT_FAILURE)
    {
        return -1;
    }
    //logINFO( "Requisites::req initialization" );
    if ((tmp_req.type == INT_32LE) || (tmp_req.type == INT_16LE) || (tmp_req.type == UNIX_TIME))
    {   // Инициализация реквизита (value -> requisites)
        // В случае нефиксированной длины задаём длину
        if (tmp_req.is_len_fix)
        {
            tmp_req.cur_len = tmp_req.max_len;
        }
        else
        {
            tmp_req.cur_len = sizeof(value);
        }
        tmp_req.value_int = value;
        // Добавление реквизита в лист реквизитов
        new_req_list.push_back( tmp_req );
        return 0;
    }
    // Неверный тип value
    logINFO( "Requisites::wrong type of value" );
    return -1;
}

int Requisites::vln_to_list_add( unsigned short tag, unsigned char *bytes, unsigned short count_bytes, vector <Requisite> &new_req_list )
{
    logINFO( "=============================================================" );
    logINFO( "Requisites::vln_to_list_add" );
    Requisite tmp_req;
    // Поиск нужного реквизита в общем листе реквизитов
    if (req_search( tag, &tmp_req ) == EXIT_FAILURE)
    {
        return -1;
    }
    logINFO( "Requisites::req initialization" );
    if ((tmp_req.type == VLN) || (tmp_req.type == FVLN))
    {   // Инициализация реквизита (value -> requisites)
        tmp_req.cur_len = count_bytes;
        // Выделяем память для реквизита
        tmp_req.value_byte = new unsigned char[tmp_req.cur_len];
        memset( tmp_req.value_byte, 0, tmp_req.cur_len );
        memcpy( tmp_req.value_byte, bytes, count_bytes );
        // Добавление реквизита в лист реквизитов
        new_req_list.push_back( tmp_req );
        return 0;
    }
    // Неверный тип value
    logINFO( "Requisites::wrong type of value" );
    return -1;
}

int Requisites::stlv_create (unsigned short tag,
                             vector<Requisite> &new_req_list,
                             unsigned char *stlv_package)
{
    logDBG( "=====================STLV PACK CREATE=====================" );
    logDBG( "REQUISITES QUANTITY = %d", new_req_list.size() );

    Requisite       tmp_req;
    unsigned short stlv_len,
                    tmp_len,
                   byte_num;
    unsigned char  *tmp_buf;

    byte_num = 0;

    if (new_req_list.empty())
    {
        // Пустой лист
        logERR( "REQUISITES LIST IS EMPTY!" );
        return -1;
    }

    stlv_len                 =   stlv_len_clc( &new_req_list );
    stlv_package[byte_num++] =   tag           & 0x00ff;         // Тэг документа
    stlv_package[byte_num++] = ( tag           & 0xff00) >> 8;
    stlv_package[byte_num++] = ( stlv_len - 4) & 0x00ff;         // Длина STLV
    stlv_package[byte_num++] = ((stlv_len - 4) & 0xff00) >> 8;

    for ( auto it = new_req_list.begin(); it != new_req_list.end(); ++it )
    {
        // Выгружаем очередной реквизит из листа
        tmp_req = (*it);
        tmp_buf = new unsigned char[tmp_req.cur_len];

        if (tmp_req.type != STLV)
        {
            stlv_package[byte_num++] =  tmp_req.tag     & 0x00ff;       // Тэг
            stlv_package[byte_num++] = (tmp_req.tag     & 0xff00) >> 8;
            stlv_package[byte_num++] =  tmp_req.cur_len & 0x00ff;       // Длина
            stlv_package[byte_num++] = (tmp_req.cur_len & 0xff00) >> 8;
        }
        if (   (tmp_req.type == INT_16LE )
            || (tmp_req.type == INT_32LE )
            || (tmp_req.type == UNIX_TIME))
        {
            tmp_len = uint_to_uchar_convert(  tmp_req.value_int,
                                              tmp_req.type,
                                              tmp_buf);
            memcpy( &stlv_package[byte_num], tmp_buf, tmp_len );
            byte_num += tmp_len;
        }
        if (tmp_req.type == ASCII)
        {
            memcpy( &stlv_package[byte_num], tmp_req.value_str, tmp_req.cur_len );
            byte_num += tmp_req.cur_len;
        }
        if (   (tmp_req.type == STLV)
            || (tmp_req.type == BYTE)
            || (tmp_req.type == VLN )
            || (tmp_req.type == FVLN))
        {
            memcpy( &stlv_package[byte_num], tmp_req.value_byte, tmp_req.cur_len );
            byte_num += tmp_req.cur_len;
        }
        if (tmp_buf != NULL)
        {
            delete[] tmp_buf;
        }
    }
    logDBG( "STLV PACKAGE: " );

    if(getLogDBLevel() >= LOG_INFO)
    {
        for (int i = 0; i < stlv_len; i++ )
        {
            logDBG( "0x%02x ", stlv_package[i]);
            if ((i + 1) % 20 == 0)
            {
                logDBG("\n");
            }
        }
        logDBG("\n");
    }

    req_list_erase( &new_req_list );

    return 0;
}

int Requisites::uint_to_uchar_convert (u_int64_t      uint,
                                       unsigned char  int_type,
                                       unsigned char *uchar_buf)
{
    //logINFO( "=============================================================" );
    //logINFO( "Requisites::uint_to_uchar_convert " );
    uint64_t init_mask = 0xff;
    uint64_t mask;
    unsigned len;
    const unsigned char size_of_byte = 8;

    if ((int_type == INT_32LE) || (int_type == UNIX_TIME))
        len = sizeof(int32_t);

    if (int_type == INT_16LE)
        len = sizeof(int16_t);

    if (int_type == INT40LE)
        len = 5;

    if (len == 0)
        return -1;

    for ( unsigned int i = 0; i < len; i++ )
    {
        mask = init_mask << ((len - i - 1) * size_of_byte);
        uchar_buf[(len - i - 1)] = (uint & mask) >> ((len - i - 1) * size_of_byte);
    }

    return len;
}

int Requisites::stlv_len_clc( vector <Requisite> *req_list )
{
    unsigned short stlv_len = 0;

    for ( auto it = req_list->begin(); it != req_list->end(); ++it )
    {
        // Длина данных реквизита
        stlv_len += (*it).cur_len;
        if ((*it).type != STLV)
        {
            // Шапка TLV реквизита
            stlv_len += 4;
        }
    }
    stlv_len += 4;

    return stlv_len;
}

int Requisites::req_list_erase(  vector <Requisite> *req_list )
{
    //logINFO( "=============================================================" );
    logINFO( "REQ LIST ERASE" );
    Requisite tmp_req;

    if (req_list == NULL)
    {
        logERR( "NULL POINTER!" );
        return -1;
    }

    if (req_list->empty())
    {
        // Пустой лист
        return -1;
    }
    //Высвобождение памяти, занимаемой new_req_list
    for ( auto it = req_list->begin(); it != req_list->end(); ++it )
    {
        tmp_req = (*it);
        if (tmp_req.value_byte != NULL)
        {
            delete[] tmp_req.value_byte;
        }
        tmp_req.value_byte = NULL;
        if (tmp_req.value_str != NULL)
        {
            delete[] tmp_req.value_str;
        }
        tmp_req.value_str = NULL;
    }
    req_list->clear();

    return 0;
}

int Requisites::req_list_display()
{
    logINFO( "Requisites::req_list_display " );
    //Вывод листа на экран
    for ( auto it = requisites_list.begin(); it != requisites_list.end(); ++it )
    {
        logINFO( "Requisites::tag: %d", (*it).tag );
        logINFO( "Requisites::name:  %s", (*it).name.c_str() );
        logINFO( "Requisites::name size: %d", (*it).name.size() );
        logINFO( "Requisites::len fix y/n: %d", (*it).is_len_fix );
        logINFO( "Requisites::max len: %d", (*it).max_len );
        logINFO( "Requisites::cur len: %d", (*it).cur_len );
    }
    return 0;
}

bool Requisites::IsTagExist( vector<uint16_t> *tags,
                             uint16_t           tag   )
{
    for ( auto it = tags->begin(); it != tags->end(); ++it )
    {
        if ((*it) == tag)
        {
            logDBG( "REQUISITE %u DUBLICATE!", tag );
            return true;
        }
    }
    return false;
}

int Requisites::test_stlv_create()
{
   //logINFO( "=============================================================" );
    logINFO( "Requisites::test_stlv_create" );
    vector < Requisites::Requisite > new_req_list;
    unsigned char* stlv_pack;
    unsigned short err_count = 0;
    if (tlv_to_list_add_str( 1000, "АБВГД", new_req_list ) == -1)
    {
        err_count++;
    }
    unsigned char* bytes;
    bytes = new unsigned char[7];
    bytes[0] = 1;
    bytes[1] = 2;
    bytes[2] = 3;
    bytes[3] = 4;
    bytes[4] = 5;
    bytes[5] = 6;
    bytes[6] = 7;
    if (tlv_to_list_add_uch( 1077, bytes, 7, new_req_list ) == -1)
    {
        err_count++;
    }
    delete[] bytes;

    unsigned char* stlv;
    stlv = new unsigned char[5];
    stlv[0] = 7;
    stlv[1] = 8;
    stlv[2] = 9;
    stlv[3] = 10;
    stlv[4] = 11;
    if (tlv_to_list_add_uch( 1206, stlv, 5, new_req_list ) == -1)
    {
        err_count++;
    }
    delete[] stlv;
    if (tlv_to_list_add_int32( 2, 500, new_req_list ) == -1)
    {
        err_count++;
    }
    if (tlv_to_list_add_int32( 1040, 500000, new_req_list ) == -1)
    {
        err_count++;
    }
    stlv_pack = new unsigned char[stlv_len_clc( &new_req_list )];
    if (stlv_create( 5, new_req_list, stlv_pack ) == -1)
    {
        err_count++;
    }
    delete[] stlv_pack;
    if (err_count != 0)
    {
        logINFO( "Requisites::test finished with errors!" );
        return -1;
    }
    logINFO( "Requisites::test finished without errors!" );
    return 0;
}

string Requisites::doc_name_search( unsigned short tag )
{
    string doc_name = "";
    logINFO("Requisites::doc_name_search");
    // Поиск наименования документа по коду (тэгу) документа
    switch (tag)
    {
        case 1:
        {
            doc_name = "ОТЧЁТ О РЕГИСТРАЦИИ";
            break;
        }
        case 2:
        {
            doc_name = "ОТЧЁТ ОБ ОТКРЫТИИ СМЕНЫ";
            break;
        }
        case 3:
        {
            doc_name = "КАССОВЫЙ ЧЕК";
            break;
        }
        case 4:
        {
            doc_name = "БЛАНК СТРОГОЙ ОТЧЁТНОСТИ";
            break;
        }
        case 5:
        {
            doc_name = "ОТЧЁТ О ЗАКРЫТИИ СМЕНЫ";
            break;
        }
        case 6:
        {
            doc_name = "ОТЧЁТ О ЗКРЫТИИ ФИСКАЛЬНОГО НАКОПИТЕЛЯ";
            break;
        }
        case 7:
        {
            doc_name = "ПОДТВЕРЖДЕНИЕ ОПЕРАТОРА";
            break;
        }
        case 11:
        {
            doc_name = "ОТЧЁТ ОБ ИЗМЕНЕНИИ ПАРАМЕТРОВ РЕГИСТРАЦИИ";
            break;
        }
        case 21:
        {
            doc_name = "ОТЧЁТ О ТЕКУЩЕМ СОСТОЯНИИ РАСЧЁТОВ";
            break;
        }
        case 31:
        {
            doc_name = "КАССОВЫЙ ЧЕК КОРРЕКЦИИ";
            break;
        }
        case 41:
        {
            doc_name = "БЛАНК СТРОГОЙ ОТЧЁТНОСТИ КОРРЕКЦИИ";
            break;
        }
        default:
        {
            logINFO( "Requisites::doc name not found!" );
            break;
        }
    }
    return doc_name;
}

//=======================================================================================================
//                                 Конструктор и методы таблицы имён реквизитов
//=======================================================================================================
void Requisites::req_names_table_init()
{
    logINFO( "REQ NAMES TABLE INIT" );

    req_name_table.clear();

    for ( auto it = requisites_list.begin(); it != requisites_list.end(); ++it )
    {
        req_name_table.push_back( REQ_NAME(*it) );
    }
}

void Requisites::req_names_table_show()
{
    logDBG( "REQ NAMES TABLE SHOW" );
    logDBG( "Names table exist %d requisites", req_name_table.size() );
    for ( auto it = req_name_table.begin(); it != req_name_table.end(); ++it )
    {
        logDBG( "TAG %d REQ NAME IS %s", (*it).tag, (*it).req_name_str.c_str() );
    }
}

string Requisites::req_name_search( unsigned short tag )
{

    for (unsigned i = 0; i < req_name_table.size(); i++)
    {
        if (req_name_table.at( i ).tag == tag)
        {
            logDBG( "REQUISITE %d NAME IS %s", tag, req_name_table.at( i ).req_name_str.c_str() );
            return req_name_table.at( i ).req_name_str;
        }
    }

    logERR( "REQUISITE %d NAME NOT FOUND!", tag );

    return null_str;
}
