/*
 * requisites.h
 *
 *  Created on: 18 окт. 2016 г.
 *      Author: user
 */

#ifndef SRC_APPL_REQUISITES_H_
#define SRC_APPL_REQUISITES_H_

#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <list>
#include <vector>
#include <typeinfo>
//#include "goods_data.h"
//#include "log.h"
//#include "positions.h"
#include "unistd.h"

#include "loggerdaemon.h"

using namespace std;
//=======================================================================================================
//                                           КЛАСС ДОКУМЕНТОВ
//=======================================================================================================
class Requisites
{
    public:
        Requisites();
        ~Requisites();

        enum TAG
        {
            TAG_NOMENCLATURE_CODE = 1162,
            TAG_BUYER_NAME = 1227,
            TAG_BUYER_INN,
            TAG_EXCISE_DUTY,
            TAG_ORIGIN_COUNTRY_CODE,
            TAG_CUSTOM_ENTRY_NUM,
            TAG_ADD_USER_REQ = 1084,            // дополнительный реквизит пользователя
            TAG_ADD_USER_NAME = 1085,           // наименование дополнительного реквизита пользователя
            TAG_ADD_USER_VALUE = 1086,          // значение дополнительного реквизита пользователя
            TAG_ADD_POS_REQ = 1191,             // дополнительный реквизит предмета расчёта
            TAG_ADD_REC_REQ = 1192              // дополнительный реквизит чека (БСО)
        };

        // Реквизит
        class Requisite
        {
            public:
                Requisite();
                ~Requisite();

                string                                       name,
                                                    print_tempale;
                bool                             is_print_tempale;
                string                           electron_tempale;
                bool                          is_electron_tempale;
                unsigned short                                tag;
                bool                                   is_len_fix;
                unsigned short                            cur_len,
                                                          max_len;
                unsigned char                       type_doc_mask,
                                                             type;
                int64_t                                 value_int;
                unsigned char                          *value_str,
                                                      *value_byte;

                std::vector<Requisite> requisites;

                Requisite& operator = (const Requisite &requisite);
        };

        class REQ_NAME
        {
            public:
                REQ_NAME( Requisite &requisite );
                ~REQ_NAME();
                uint16_t          tag;
                string   req_name_str;
            REQ_NAME operator = (const REQ_NAME &req_name);
        };
        vector <REQ_NAME> req_name_table;
        string null_str;
        // Маски типов value
        static const unsigned short INT_16LE =   1,
                                    INT_32LE =   2,
                                   UNIX_TIME =   4,
                                       ASCII =   8,
                                        BYTE =  16,
                                        STLV =  32,
                                         VLN =  64,
                                        FVLN = 128,
                                     INT40LE = 255;
        // Маски фиксированной длины
        const bool len_fix = true;
        const bool len_not_fix = false;
        // Маски типов документа
        const unsigned char print = 1;
        const unsigned char electron = 2;
        const unsigned char print_electron = 4;
        // Лист реквизитов
        vector <Requisite> requisites_list;
        // Методы формирования документа
        // Конструктор таблицы реквизитов
        int req_clear( Requisite& req );
        int req_table_init();

        void req_names_table_init();
        void req_names_table_show();
        string req_name_search( unsigned short tag );

        // Метод поиска реквизита по коду реквизита
        int req_search( unsigned short tag, Requisite *req );
        bool isReqSTLV( uint16_t tag );
        string doc_name_search( unsigned short tag );
        // Методы добавления реквизита в лист
        int tlv_to_list_add_uch( unsigned short tag, unsigned char *bytes, unsigned short len,  vector <Requisite> &new_req_list );
        int tlv_to_list_add_str( unsigned short tag, const char *text,  vector <Requisite> &new_req_list );
        int tlv_to_list_add_int32( unsigned short tag, int32_t value, vector <Requisite> &new_req_list );
        int vln_to_list_add( unsigned short tag, unsigned char *bytes, unsigned short count_bytes, vector <Requisite> &new_req_list );
        // Метод формирования STLV пакета
        int stlv_create( unsigned short tag, vector <Requisite> &new_req_list, unsigned char* stlv_package );
        // Расчёт длины финального stlv пакета
        int stlv_len_clc( vector <Requisite> *new_req_list );
        // Конверторы
        int uint_to_uchar_convert(  u_int64_t uint, unsigned char int_type, unsigned char *uchar_buf );
        // Удаление листа и высвобождение памяти
        int req_list_erase(  vector <Requisite> *req_list );
        int req_list_display( );
        // Методы тестов
        int test_stlv_create();
        static bool IsTagExist( vector<uint16_t> *tags,
                                uint16_t          tag  );



};


#endif /* SRC_APPL_REQUISITES_H_ */
