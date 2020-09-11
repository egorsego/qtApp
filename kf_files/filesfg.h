/*
 * filesfg.h
 */

#ifndef SRC_APPL_INCLUDE_FILESFG_H_
#define SRC_APPL_INCLUDE_FILESFG_H_
#include <string>
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fstream>

#include "config.h"
#include "loggerdaemon.h"

#include <sys/stat.h>

#define EX_REG_REP_FILE   "/FisGo/ex_reg_rep"
#define STAT_DIR          "/FisGo/stat"
#define FAST_KEY_CNT_FILE "/FisGo/stat/FastKeyCnt"
#define DIAGNOSTICS       "/FisGo/diagnostics"
#define ETH_MAC_ADDR      "/sys/class/net/eth0/address"
#define WLAN_MAC_ADDR     "/sys/class/net/wlan0/address"

using namespace std;
class FilesFG
{
public:
    FilesFG(){};
    virtual ~FilesFG(){};

    typedef std::string PATH;
    typedef std::vector<PATH> PATHS;

    static int save_in_file( string filename, char *data, size_t size, bool sync = true );
    virtual int save_in_file( string filename, const uint8_t *data, size_t size, bool sync );
    virtual int save_in_file( string filename, string strSource );
	static int save_array_in_file( string filename, uint32_t *mass, uint32_t size );

	static void print_array_of_char( char *mass, unsigned int size );
	static int cfg_data_copy ( Config        *dest,
	                           Config      *source,
							   DYNAMIC_MASK &mask );
    enum FILE_STATUS
    {
        FILE_IS_ALREADY_EXIST,
        FILE_CREATE_SUCCESS,
        FILE_CREATE_ERR,
        FILE_OPEN_ERR,
        FILE_WRITE_ERR,
        FILE_CLOSE_ERR,
        FILE_READ_SUCCESS,
        FILE_WRITE_SUCCESS,
        FILE_IS_EXIST,
        FILE_IS_NOT_EXIST,
        FILE_IS_CLOSE,
        SUCCESS,
        FAILED
    };

    virtual                FILE_STATUS    isFileExist          ( string                         );
    virtual                FILE_STATUS    fileCreate           ( string                         );
    virtual                FILE_STATUS    fileCreateIfNotExist ( string                         );
    virtual                FILE_STATUS    str_to_file          ( string,                string  );
    virtual                FILE_STATUS    strFromFile          ( string,                string& );
    //-----------------------------------------
    // Запись в файл
    //-----------------------------------------
    template <typename T> FILE_STATUS write( string filename, T value, ios_base::openmode mode = ios_base::in | ios_base::trunc )
    {
        logDBG( "WRITE ANY IN FILE : %s", filename.c_str() );
        if(isFileExist( filename ) == FILE_IS_NOT_EXIST)
        {
            if(fileCreate( filename ) == FILE_CREATE_ERR)
            {
                return FILE_CREATE_ERR;
            }
        }

        logDBG( "FILE : %s CREATE", filename.c_str() );
        ofstream fout( filename.c_str(), mode );
        if(fout.is_open())
        {
            logDBG( "SAVE IN FILE" );
            fout << value;
            fout.flush();
            fout.close();
            sync();
            logDBG( "SAVE IN FILE SUCCESS" );
            return FILE_WRITE_SUCCESS;
        }
        else
        {
            logERR( "FILE : %s OPEN ERROR!!!", filename.c_str() );
            return FILE_OPEN_ERR;
        }

        return SUCCESS;
    }
    //-----------------------------------------
    // Чтение из файла
    //-----------------------------------------
    template <typename T> FILE_STATUS read( string filename, T &value )
    {
        logDBG( "READ ANY FROM FILE : %s", filename.c_str() );

        if( isFileExist( filename ) == FILE_IS_NOT_EXIST )
        {
            return FILE_IS_NOT_EXIST;
        }
        //-----------------------------------------
        ifstream fin( filename.c_str() );
        //-----------------------------------------
        if(fin.is_open())
        {
            logDBG( "READ FROM FILE" );

            while ( !fin.eof() )
            {
                fin >> value;
            }
            fin.close();
            logDBG( "READ FROM FILE SUCCESS" );
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
    // Работа с директориями
    //-----------------------------------------------------
    enum DIR_STATUS
    {
        DIR_IS_EXIST,
        DIR_IS_NOT_EXIST,
        MKDIR_SUCCESS,
        MKDIR_ERR
    };
    //-----------------------------------------------------
    // Тест на наличие директории
    //-----------------------------------------------------
    static DIR_STATUS           isDirExist( string path );
    //-----------------------------------------------------
    // Создание директории
    //-----------------------------------------------------
    static DIR_STATUS            dirCreate( string path );
    //-----------------------------------------------------
    // Создание директорииСоздание директории
    // в случае отсутствия
    //----------------------------------------------------
    static DIR_STATUS  dirCreateIfNotExist( string path );

    //-----------------------------------------------------
    // Метод чтения файлов для отправки через CURL
    //-----------------------------------------------------
    static size_t readCurlCallback(void *ptr, size_t size, size_t nmemb, FILE *stream);
    /**
     * @brief searchFiles поиск файлов в директории
     * @param dir директория
     * @param filenames часть имени файлов
     * @param files файлы
     */
    static void searchFiles(const PATH& dir, const PATH& filenames, PATHS& files);
};

#endif /* SRC_APPL_INCLUDE_FILESFG_H_ */
