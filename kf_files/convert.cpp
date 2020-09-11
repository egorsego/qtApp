/*
 * convert.cpp
 */

#include "convert.h"
#include <string.h>
#include <stdexcept>

#include "config.h"

using namespace std;

const string Convert::bs64Values = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

bool Convert::isMacValid( string mac )
{
//    boost::regex macTemplate("^([[:xdigit:]]{2}[:.-]){5}[[:xdigit:]]{2}$");
//    boost::smatch xResults;
//    return boost::regex_match( mac, xResults, macTemplate );
    return true;
}

bool Convert::isEmailValid(const string &email)
{
//    bool rc(false);
//    if (email.length() <= LIMIT_MAX_BUYER_ADDR)
//    {
//        boost::regex emailTemplate("^[^@]+@{1}[^@]+$");
//        boost::smatch xResults;
//        rc = boost::regex_match(email, xResults, emailTemplate);
//    }
//    return rc;
    return true;
}

bool Convert::isPlateNumberValid(const std::string &plateNumber)
{
//    bool rc(false);

//    try
//    {
//        boost::regex plateNumberTemplate("^[а-яА-Я]\\d{3}[а-яА-Я]{2}\\d{2,3}$");
//        boost::smatch xResults;
//        rc = boost::regex_match(plateNumber, xResults, plateNumberTemplate);
//    }
//    catch (boost::exception &e)
//    {
//        logERR("%s\t%s", __FUNCTION__, boost::diagnostic_information(e).c_str());
//    }

//    return rc;
    return true;
}

bool Convert::isTelNumberValid(const string &telNumber)
{
//    bool rc(false);
//    if (telNumber.length() <= LIMIT_MAX_BUYER_ADDR)
//    {
//        boost::regex telNumberTemplate("^\\+[[:digit:]]{11,}$");
//        boost::smatch xResults;
//        rc = boost::regex_match(telNumber, xResults, telNumberTemplate);
//    }
//    return rc;
    return true;
}

bool Convert::isFnNumberValid(string fnNumber)
{
//    boost::regex fnNumberTemplate("^[[:digit:]]{16}$");
//    boost::smatch xResults;
//    return boost::regex_match(fnNumber, xResults, fnNumberTemplate);
    return true;
}

DateTime Convert::getDateTime(const time_t *time)
{
    DateTime dateTime;
    if(time == nullptr)
    {
        logERR("%s Null ptr!", __PRETTY_FUNCTION__);
        return dateTime;
    }
    tm tmi = *gmtime(time);

    dateTime.day     = tmi.tm_mday;
    dateTime.month   = tmi.tm_mon  + 1;
    dateTime.year    = tmi.tm_year + 1900;
    dateTime.hours   = tmi.tm_hour;
    dateTime.minutes = tmi.tm_min;
    dateTime.seconds = tmi.tm_sec;

    logDBG("%s Date\t:\t%02d.%02d.%04d %02d:%02d:%02d", __PRETTY_FUNCTION__,
           dateTime.day,
           dateTime.month,
           dateTime.year,
           dateTime.hours,
           dateTime.minutes,
           dateTime.seconds);

    return dateTime;
}

void Convert::getDateTime(tm& timeinfo)
{
    tm* tmi;
    time_t rawtime;
    time( &rawtime );
    tmi = localtime( &rawtime );
    timeinfo = *tmi;
}

int32_t Convert::getTimeZone()
{
    time_t time = cur_unixtime_get();

    struct tm *ti = localtime( &time );
    return (ti->tm_gmtoff / Convert::SEC_IN_HOUR);
}

void Convert::getStrDate(char *tmp, tm timeinfo)
{
    string str;
    sprintf(tmp,"%02d.%02d.%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
}

void Convert::getStrTime(char *tmp, tm timeinfo)
{
    string str;
    sprintf(tmp,"%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
}

string Convert::getStrTime(tm timeinfo)
{
    char tmp[256];
    string str;
    sprintf(tmp,"%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    str = charToString(tmp);
    return str;
}

string Convert::getStrDateTime(tm ti)
{
    char tmp[256];
    string str;
    sprintf(tmp,"%02d.%02d.%04d  %02d:%02d", ti.tm_mday, ti.tm_mon + 1, ti.tm_year + 1900, ti.tm_hour, ti.tm_min);
    str = charToString(tmp);
    return str;
}
string Convert::getStrFullDateTime(tm ti)
{
    char tmp[256];
    string str;
    sprintf(tmp,"%02d.%02d.%04d  %02d:%02d:%02d", ti.tm_mday, ti.tm_mon + 1, ti.tm_year + 1900, ti.tm_hour, ti.tm_min, ti.tm_sec);
    str = charToString(tmp);
    return str;
}

string Convert::getStrFullDateTime(time_t time)
{
    char tmp[256];
    string str;
    struct tm *ti;
    ti = localtime( &time );
    sprintf(tmp,"%02d.%02d.%04d  %02d:%02d:%02d", ti->tm_mday, ti->tm_mon + 1, ti->tm_year + 1900, ti->tm_hour, ti->tm_min, ti->tm_sec);
    str = charToString(tmp);
    return str;
}

string Convert::getStrFullDateTimeUtc(time_t time)
{
    char tmp[256] = {0};
    struct tm *ti;
    ti = gmtime( &time );
    sprintf(tmp,"%02d.%02d.%04d  %02d:%02d:%02d", ti->tm_mday, ti->tm_mon + 1, ti->tm_year + 1900, ti->tm_hour, ti->tm_min, ti->tm_sec);
    return charToString(tmp);
}

string Convert::getStrShortDateTime(time_t time)
{
    char tmp[256];
    string str;
    struct tm *ti;
    ti = localtime( &time );
    sprintf(tmp,"%02d.%02d.%04d %02d:%02d", ti->tm_mday, ti->tm_mon + 1, ti->tm_year + 1900, ti->tm_hour, ti->tm_min);
    str = charToString(tmp);
    return str;
}

string Convert::sumToString(uint64_t val)
{
    string str="";
    char data[256];
    sprintf(data, "%llu.%02llu", val/100, val%100);
    str = charToString(data);
    return str;
}

bool Convert::date_difference( string date_cur_str, string date_prev_str, int minutes )
{
    int days       = 0;
    Date date_cur, date_prev, date_res;
    bool success = date_convert(date_cur_str, date_cur);
    success = success && date_convert(date_prev_str, date_prev);

    if(!success)
    {
        logERR("%s Failed!", __PRETTY_FUNCTION__);
        return false;
    }

    date_res.year  = date_cur.year - date_prev.year;
    date_res.month = date_res.year * 12 + date_cur.month - date_prev.month;

    if (date_res.month > 1)
    {
        logINFO( "MONTH > 1" );
        return true;
    }
    else if (date_res.month == 1)
    {
        days = day_in_month_clc( date_prev.year, date_prev.month );
    }

    date_res.day     = days + date_cur.day - date_prev.day;
    date_res.hours   = date_res.day * 24 + date_cur.hours - date_prev.hours;
    date_res.minutes = date_res.hours * 60 + date_cur.minutes
                     - date_prev.minutes;

    if (date_res.minutes >= minutes)
    {
        logINFO( "MINUTES > %d", minutes );
        return true;
    }

    return false;
}

string Convert::sumToString(int64_t val)
{
    string  str ="";
    char    data[256];
    int64_t tmp = 0;

    tmp = val%100;
    if (tmp < 0)
        tmp = tmp * (-1);

    sprintf(data, "%lld.%02llu", val/100, tmp%100);
    str = charToString(data);
    return str;
}

/* Дата вводится в нормальном формате, например 23 февраля 2017 выглядит как 23.02.2017 */
time_t Convert::convertToUnixTime(tm timeinfo)
{
    time_t rawtime;
    timeinfo.tm_year -= 1900;
    timeinfo.tm_mon -= 1;
    rawtime = mktime(&timeinfo);
    return rawtime;
}

string Convert::charToString(const char *source)
{
    if (source != nullptr)
    {
        int size = strlen(source);
        return string(source, size);
    }
    return string();
}

string Convert::charToString( const char *source, uint32_t max_size)
{
    string strDest;
    uint32_t max = strlen(source);
    if (max > max_size)
    {
        max = max_size;
    }
    strDest.assign(source, max);
    return strDest;
}

string Convert::intToString ( unsigned int *source )
{
    string result;

    for (unsigned i = 0; i < sizeof(unsigned int); i++)
        result += to_string(source[i]);

    return result;
}

char* Convert::str_to_char( string str )
{
    char* result = new char[str.length() + 1];
    std::copy(str.begin(), str.end(), result);

    return result;
}

void Convert::stringToChar( string source, char *dest, uint32_t max_size)
{
    uint32_t max = source.length();
    if ((max) > max_size)
    {
        max = max_size;
    }
    memset( dest, 0x00, max_size);
    strncpy( dest, source.c_str(), max);
}

int Convert::crc_16_ciit_clc( const char *data, unsigned short len )
{
    char x;
    unsigned short crc = 0xFFFF;

    while (len--)
    {
        x = crc >> 8 ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((unsigned short) (x << 12))
                ^ ((unsigned short) (x << 5)) ^ ((unsigned short) x);
    }
    return crc;
}

double Convert::str_to_double( string cop )
{
    double rub = 0.0;

    if(cop.empty())
    {
        logERR( "SUMM IS EMPTY!!!" );
        return rub;
    }

    rub = QString( cop.c_str()).toULongLong()/100.;

    return rub;
}

uint64_t Convert::double_to_uint64(double rub)
{
    uint64_t cop = rub * 100.0;

    return cop;
}

uint64_t Convert::doubleToUint64ArithmeticRound(const double &value)
{
    return static_cast<uint64_t>(value * 100.0 + 0.50000001);
}

// Преобразование значени int64_t в string(double) с заданной точность знаков после запятой
string Convert::int64_to_string_prec(int64_t val, BYTE precision)
{
    stringstream stream;
    stream << fixed << setprecision(precision) << cop2rub(val);

    return stream.str();
}

string Convert::uint64toStrPrec(uint64_t val, BYTE precision)
{
    stringstream stream;
    stream << fixed << setprecision(precision) << cop2rub(val);

    return stream.str();
}

int Convert::pld_date_create ( string date_str, unsigned char &DD, unsigned char &MM, int &YYYY ) // DD.MM.YY
{
    if( (std::count( date_str.begin(), date_str.end(), '.' ) != 2) || (date_str.length() != 8) )
    {
        logERR( "WRONG DATE!!!" );
        return EXIT_FAILURE;
    }

//    DD   = (unsigned char)QString(date_str.substr( 0, 2 ).c_str());
//    logDBG( "0x%02x\n", DD );
//    MM = (unsigned char)QString(date_str.substr( 3, 2 ).c_str());
//    logDBG( "0x%02x\n", MM );
//    YYYY  =         QString(date_str.substr( 6, 2 ).c_str());
//    logDBG( "%d\n", YYYY );

    return EXIT_SUCCESS;
}

string Convert::time_str_create( unsigned char *date )
{
    // ДДММГГЧЧММ
    string      date_str = "";
    uint16_t        year = 0,
                   month = 1,
                     day = 2,
                   hours = 3,
                 minutes = 4;

    if ((date[day]) < 10)
    {
        date_str += "0";
    }
    date_str += to_string( date[day] );

    if ((date[month]) < 10)
    {
        date_str += "0";
    }
    date_str += to_string( date[month]);

    if ((date[year]) < 10)
    {
        date_str += "0";
    }
    date_str += to_string( date[year] );

    if ((date[hours]) < 10)
    {
        date_str += "0";
    }
    date_str += to_string( date[hours] );

    if ((date[minutes]) < 10)
    {
        date_str += "0";
    }
    date_str += to_string( date[minutes] );

    return date_str;
}

string Convert::time_str_get( time_t time )
{
    // ДДММГГЧЧММ
    struct tm *timeinfo;
    timeinfo = localtime( &time );

    return tmToStr(*timeinfo);
}

string Convert::tmToStr( const tm &timeinfo)
{
    string dateStr;

    if ((timeinfo.tm_mday) < 10)
    {
        dateStr += "0";
    }
    dateStr += to_string( timeinfo.tm_mday );

    if ((timeinfo.tm_mon + 1) < 10)
    {
        dateStr += "0";
    }
    dateStr += to_string( timeinfo.tm_mon + 1 );

    if ((timeinfo.tm_year - 100) < 10)
    {
        dateStr += "0";
    }
    dateStr += to_string( timeinfo.tm_year - 100 );

    if ((timeinfo.tm_hour) < 10)
    {
        dateStr += "0";
    }
    dateStr += to_string( timeinfo.tm_hour );

    if ((timeinfo.tm_min) < 10)
    {
        dateStr += "0";
    }
    dateStr += to_string( timeinfo.tm_min );

    return dateStr;
}

string Convert::getISODate()
{
    logDBG("%s", __PRETTY_FUNCTION__);

    const BYTE TIMEZONE_LEN = 5;
    string date;

    char timezone[TIMEZONE_LEN] = {};
    sprintf(timezone, "+%02d:00", Convert::getTimeZone());

//    try
//    {
//        boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
//        date = boost::posix_time::to_iso_string(time) + timezone;
//    }
//    catch (boost::exception &ex)
//    {
//        logERR("%s %s", __PRETTY_FUNCTION__, boost::diagnostic_information(ex).c_str());
//    }

    logDBG("%s Date\t:\t%s", __PRETTY_FUNCTION__, date.c_str());
    return date;
}

Convert::DATE_ISO_8601 Convert::tmToISODate(const tm &dateTime)
{
    DATE_ISO_8601 result;
    result.year  = dateTime.tm_year + YEAR_CONSTANT;
    result.month = dateTime.tm_mon + 1;
    result.day   = dateTime.tm_mday;
    result.hours = dateTime.tm_hour;
    result.minutes = dateTime.tm_min;
    result.seconds = dateTime.tm_sec;
    result.time_zone = getTimeZone();
    return result;
}

time_t Convert::cur_unixtime_get()
{
    time_t rawtime;
    time( &rawtime );

    return rawtime;
}

time_t Convert::cur_localtime_get()
{
    return time(nullptr) + getTimeZone() * TIME_SEC_IN_MIN * TIME_MIN_IN_HOUR;
}

int Convert::cur_unixtime_usec_get( timeval *time )
{
    if (time == NULL)
    {
        logERR( "NULL POINTER!!!" );
        return EXIT_FAILURE;
    }
    if (gettimeofday( time, NULL ) != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

string Convert::dateTimeToStr(time_t time)
{
    //"2017-04-24T12:00:01.018+03:00
    struct tm *ti;
    ti = localtime( &time );
    int zone = ti->tm_gmtoff / Convert::SEC_IN_HOUR;

    char tmpC[256];
    sprintf(tmpC, "%04d-%02d-%02dT%02d:%02d:%02d.%03d+%02d:00",ti->tm_year+1900,ti->tm_mon+1,ti->tm_mday,ti->tm_hour,ti->tm_min,ti->tm_sec, 0, zone );
    charToString(tmpC);
    return charToString(tmpC);
}
string Convert::char_to_string_convert( const char *text, unsigned short len )
{
    string result = "";

    for (unsigned short i = 0; i < len; i++)
    {
        result += text[i];
    }
    return end_symbols_delete( result, 0x00 );
}

string Convert::end_symbols_delete( string text, unsigned char symbol_code )
{
    string result = text;
    unsigned int i = result.length();
    while (i != 0)
    {
        if (result[i - 1] == symbol_code)
        {
            result.erase( i - 1, 1 );
            i--;
        }
        else
            break;

    }
    return result;
}

string Convert::weight_to_str( uint64_t weight )
{
    string result =   "";
    int    gramms = 1000;

    result = to_string(float(weight / gramms));
    result = result.substr(0, result.find('.'));
    if((weight % gramms) != 0)
    {
        result += ",";
        if((weight % gramms) < 10)
            result += "00";
        else
            if((weight % gramms) < 100)
                result += "0";
        result += to_string( weight % gramms );
        while(result[result.size()-1] == '0')
            result.erase( result.end() );
    }
    return result;
}

//==============================================================
// Конверт массива из аски, где каждые 2 символа - это 1 хекс символ
bool Convert::convertASCIImassToHex(char (&data)[1000], int &size)
{
    logDBG("ДО ПРЕОБРАЗОВАНИЯ:\t Длина документа = %d", size);
    if( size % 2  != 0 )
    {
        logERR(" ---- ОШИБКА! НЕВЕРНАЯ ДЛИНА выгруженного документа! ---- ");
        return false;
    }

    string str = "";
    char hexStr[] = {'0','x',0x00,0x00,'\0'};
    for ( int j = 0; j < size; j+=2 )
    {
        hexStr[2] = data[j];
        hexStr[3] = data[j+1];
        str=str + hexStr + " ";
        data[j/2] = (char)QString( hexStr).toUInt();
    }
    logINFO(str.c_str());

    // Уменьшаем длину вдвое, тк конвертанули в форе
    size /= 2;
    logDBG("ПОСЛЕ ПРЕОБРАЗОВАНИЯ:\t Длина документа = %d", size );
    for ( int j = 0; j < size; j++ )
    {
        printf("0x%02x ", data[j]);
    }
    printf("\n");
    return true;
}


string Convert::quantityToString( uint64_t quantity, bool isWeightGoods )
{
    string result;

    if(isWeightGoods)
    {
        result = to_string(quantity / 1000);
        if((quantity % 1000) != 0)
        {
            result += ".";
            if((quantity % 1000) < 10)
                result += "00";
            else
                if((quantity % 1000) < 100)
                    result += "0";
            result += to_string( quantity % 1000 );
            while(result[result.size()-1] == '0')
                result.erase( result.end() );
        }
    }
    else
    {
        result = to_string(quantity);
    }
    return result;
}


string Convert::escapingOfQuotes(string srcStr)
{
    const string h1 = "\"";
    const string h2 = "\\\"";
    int r = 0;
    for(int i=0; i < (int)srcStr.length(); i++)
    {
        r = srcStr.find_first_of(h1.c_str(), r, 1);
        if(r >= 0)
        {
            srcStr.replace(r, 1, h2);
        }
        else
        {
            i = srcStr.length();
            break;
        }
        r += 2;

        if(r > (int)srcStr.length())
        {
            break;
        }
    }
    return srcStr;
}

// Экранирование символов для SQLITE (простое дублирование)
string Convert::escapingOfSqlite(string srcStr, string find, string replace)
{
    size_t pos = 0;
    const size_t rlen = replace.length();

    for(size_t i=0; i < srcStr.length(); i++)
    {
        pos = srcStr.find_first_of(find.c_str(), pos, 1);

        if(pos != string::npos)
        {
            srcStr.replace(pos, 1, replace);
            pos += rlen;
        }
        else
        {
            break;
        }

        if(pos > srcStr.length())
        {
            break;
        }
    }
    return srcStr;
}

string Convert::escapingOfSlash(string srcStr)
{
    const string h1 = "\\";
    const string h2 = "\\\\";
    int r = 0;
    for(int i=0; i < (int)srcStr.length(); i++)
    {
        r = srcStr.find_first_of(h1.c_str(), r, 1);
        if(r >= 0)
        {
            srcStr.replace(r, 1, h2);
        }
        else
        {
            i = srcStr.length();
            break;
        }
        r += 2;

        if(r > (int)srcStr.length())
        {
            break;
        }
    }
    return srcStr;
}

bool Convert::isDoUpdateCompareVersions(string curVersion, string updVersion)
{
    bool res = false;
    int cv[3] = {0, 0, 0};
    int uv[3] = {0, 0, 0};
    int r1 = 0, r2 = 0;

    if((curVersion.length() >= 5) && (updVersion.length() >= 5))
    {
        r1 = curVersion.find_first_of(".", 0, 1);
        r2 = curVersion.find_last_of(".", curVersion.length(), 1);
        cv[0] = QString( curVersion.substr(0, r1).c_str()).toInt();
        cv[1] = QString( curVersion.substr(r1+1,(r2-r1-1)).c_str()).toInt();//QString(curVersion.substr(r1+1,(r2-r1-1)).c_str());
        cv[2] = QString( curVersion.substr(r2+1,(curVersion.length()-r2)).c_str()).toInt();// QString(curVersion.substr(r2+1,(curVersion.length()-r2)).c_str());

        r1 = updVersion.find_first_of(".", 0, 1);
        r2 = updVersion.find_last_of(".", updVersion.length(), 1);
        uv[0] = QString( updVersion.substr(0, r1).c_str()).toInt();
        uv[1] = QString( updVersion.substr(r1+1,(r2-r1-1)).c_str()).toInt();
        uv[2] = QString( updVersion.substr(r2+1,(updVersion.length()-r2)).c_str()).toInt();

        if(uv[0] > cv[0]) res = true;
        else if(uv[0] == cv[0])
        {
            if(uv[1] > cv[1]) res = true;
            else if(uv[1] == cv[1])
            {
                if(uv[2] > cv[2]) res = true;
            }
        }
    }
    return res;
}

int Convert::charToInt(const char *src, int maxLen)
{
    int result = 0;
    string tmpStr = charToString(src, maxLen);
    result = QString(tmpStr.c_str()).toInt();
    return result;
}

//Метод удаления русских кавычек из UTF8
string Convert::replaceSymbols(const string val)
{
    string sResult;
    unsigned char tmpStrC[1024];
    stringToChar(val, reinterpret_cast<char*>(tmpStrC), val.length());

    for(unsigned int i = 1; i < val.length(); i++)
    {
        if((tmpStrC[i] == 171) || (tmpStrC[i] == 187))
        {
            if(tmpStrC[i-1] == 194)
            {
                tmpStrC[i-1] = '\"';
                memcpy(&tmpStrC[i], &tmpStrC[i+1], strlen(reinterpret_cast<char*>(tmpStrC)-i));
            }
        }
    }
    sResult = charToString(reinterpret_cast<char*>(tmpStrC));
    return sResult;
}

void Convert::dateTimeToDDate(time_t time, int *year, int *month, int *day, int *hour, int *min, int *sec)
{
    struct tm *ti;
    ti = localtime( &time );
    if(year  != NULL) *year  = ti->tm_year+1900;
    if(month != NULL) *month = ti->tm_mon+1;
    if(day   != NULL) *day   = ti->tm_mday;
    if(hour  != NULL) *hour  = ti->tm_hour;
    if(min   != NULL) *min   = ti->tm_min;
    if(sec   != NULL) *sec   = ti->tm_sec;
}

bool Convert::DateISO8601Create(DATE_ISO_8601 &date, string date_str , bool useTime)
{
    if( (useTime == false) && (date_str.length() != 6) )
    {
        logERR( "WRONG DATE STR FORMAT!!!" );
        return false;
    }
    else if ( (useTime == true) && (date_str.length() != 12) )
    {
        logERR( "WRONG DATE TIME STR FORMAT!!!" );
        return false;
    }

    date.day       =        QString( date_str.substr( 0, 2 ).c_str() ).toInt();
    date.month     =        QString( date_str.substr( 2, 2 ).c_str() ).toInt();
    date.year      = 2000 + QString( date_str.substr( 4, 2 ).c_str() ).toInt();

    if ( useTime == false )
    {
        date.hours     = 0;
        date.minutes   = 0;
        date.seconds   = 0;
    }
    else
    {
        date.hours     = QString( date_str.substr(  6, 2 ).c_str() ).toInt();
        date.minutes   = QString( date_str.substr(  8, 2 ).c_str() ).toInt();
        date.seconds   = QString( date_str.substr( 10, 2 ).c_str() ).toInt();
    }

    date.time_zone = Convert::getTimeZone();

    return true;
}

time_t Convert::dateISO8601toTimeT(Convert::DATE_ISO_8601 &dateIso)
{
    tm calendarDate;
    calendarDate.tm_sec  = dateIso.seconds;
    calendarDate.tm_min  = dateIso.minutes;
    calendarDate.tm_hour = dateIso.hours;
    calendarDate.tm_mday = dateIso.day;
    calendarDate.tm_mon  = dateIso.month - 1; // mon [0;11]
    calendarDate.tm_year = dateIso.year - YEAR_CONSTANT;
    return mktime(&calendarDate);
}

time_t Convert::dateIsoToTimet(const Convert::DATE_ISO_8601 &dateIso)
{
    tm timeinfo;
    timeinfo.tm_year = dateIso.year;
    timeinfo.tm_mon  = dateIso.month;
    timeinfo.tm_mday = dateIso.day;
    timeinfo.tm_hour = dateIso.hours;
    timeinfo.tm_min  = dateIso.minutes;
    timeinfo.tm_sec  = dateIso.seconds;

    return convertToUnixTime(timeinfo);
}

//time_t Convert::makeTimeTZ(tm *tm)
//{
//    time_t ret = -1;

//    if ( tm == nullptr )
//    {
//        return ret;
//    }

//    int32_t zone = getTimeZone();

//    char *tz;

//    tz = getenv("TZ");
//    setenv("TZ", "",  zone);
//    tzset();

//    ret = mktime(tm);

//    if (tz)
//    {
//        setenv("TZ", tz,  zone);
//    }
//    else
//    {
//        unsetenv("TZ");
//    }

//    tzset();

//    return ret;
//}

string Convert::DateISO8601Create( DATE_ISO_8601 *date_iso_8601, PREC prec )
{
    //"2017-04-24T12:00:01.018+03:00
    char tmpC[256];
    memset( tmpC, 0x00, sizeof( tmpC ) );

    if (date_iso_8601 == NULL)
    {
        logERR( "NULL POINTER!" );
        return charToString(tmpC);
    }

    switch(prec)
    {
        case PREC_MS:
        {
            sprintf(tmpC, "%02d-%02d-%02dT%02d:%02d:%02d.%03d+%02d:00", date_iso_8601->year,
                                                                        date_iso_8601->month,
                                                                        date_iso_8601->day,
                                                                        date_iso_8601->hours,
                                                                        date_iso_8601->minutes,
                                                                        date_iso_8601->seconds,
                                                                        0,
                                                                        date_iso_8601->time_zone);
            break;
        }
        case PREC_S:
        {
            sprintf(tmpC, "%02d-%02d-%02dT%02d:%02d:%02d+%02d:00",      date_iso_8601->year,
                                                                        date_iso_8601->month,
                                                                        date_iso_8601->day,
                                                                        date_iso_8601->hours,
                                                                        date_iso_8601->minutes,
                                                                        date_iso_8601->seconds,
                                                                        date_iso_8601->time_zone);
            break;
        }
        default:
        {
            logERR( "UNKNOWN PRECISION!!!" );
            return "";
        }
    }

    charToString(tmpC);
    return charToString(tmpC);
}

uint32_t Convert::uchar_to_uint32_convert( unsigned char *data )
{
    uint32_t result = 0;
    const unsigned char size_of_byte = 8;
    for (int i = sizeof(uint32_t) - 1; i >= 0; i--)
    {
        result = (result | data[i]);
        if (i != 0)
        {
            result = result  << size_of_byte;
        }
    }
    return result;
}

/**
 *
 * @param digit - digit to convertation to LE vect
 * @return - digit, transformed to vector in Little Endian
 */
const std::vector<Convert::BYTE>& Convert::getCharsLE(const uint32_t &digit)
{
    static const BYTE BYTE_SIZE = 8,
            INDEX_LEN = 4;
    static vector<BYTE> vectLE(INDEX_LEN, 0x00);
    static uint32_t digitLE{0};

    if(digit != digitLE) // if new digit
    {
        // insert digit in LE like a byte
        for(BYTE i=0; i < INDEX_LEN; ++i)
        {
            vectLE[i] = static_cast<BYTE>(digit >> (BYTE_SIZE *i));
        }
        digitLE = digit;
    }
    return vectLE;
}
/**
 *
 * @param digit - digit to convertation to LE vect
 * @return - digit, transformed to vector in Little Endian
 */
const std::vector<Convert::BYTE>& Convert::getCharsLE(const uint16_t &digit)
{
    static const BYTE BYTE_SIZE = 8,
            INDEX_LEN = 2;
    static vector<BYTE> vectLE(INDEX_LEN, 0x00);
    static uint32_t digitLE{0};

    if(digit != digitLE) // if new digit
    {
        // insert digit in LE like a byte
        for(BYTE i=0; i < INDEX_LEN; ++i)
        {
            vectLE[i] = static_cast<BYTE>(digit >> (BYTE_SIZE *i));
        }
        digitLE = digit;
    }
    return vectLE;
}
uint32_t Convert::crc32(const BYTE* buf, const uint32_t& len)
{
    logERR("%s:: data len = %u", __func__, len);
    uint32_t crc_table[256];
    uint32_t crc;
    constexpr uint32_t FULL_MASK = 0xFFFFFFFF;
    constexpr BYTE FULL_BYTE = 0xFF;
    constexpr uint32_t POLINOME = 0xEDB88320UL;

    for (uint16_t i = 0; i < 256; ++i)
    {
        crc = i;
        for (uint16_t j = 0; j < 8; j++)
        {
            crc = crc & 1 ? (crc >> 1) ^ POLINOME : crc >> 1;
        }
        crc_table[i] = crc;
    }
    crc = FULL_MASK;
    uint32_t curLen = len;
    while (curLen--)
    {
        crc = crc_table[(crc ^ *buf++) & FULL_BYTE] ^ (crc >> 8);
    }

    return crc ^ FULL_MASK;
}

string Convert::intDateTimeToStr(int year, int month, int day, int hour, int min, int sec, int time_zone)
{
    //"2017-04-24T12:00:01.018+03:00
    char tmpC[256];
    if(year > 2000)
    {
        sprintf(tmpC, "%04d-%02d-%02dT%02d:%02d:%02d.%03d+%02d:00", year, month, day, hour, min, sec, 0, time_zone);
    }
    else
    {
        sprintf(tmpC, "20%02d-%02d-%02dT%02d:%02d:%02d.%03d+%02d:00", year, month, day, hour, min, sec, 0, time_zone);
    }
    charToString(tmpC);
    return charToString(tmpC);
}

void Convert::removeDel(std::string &src, const std::string &del)
{
    size_t pos {src.find(del)};
    while (pos != std::string::npos)
    {
        src.erase(pos, del.size());
        pos = src.find(del);
    }
}

string Convert::fpdValid( string fpd )
{
    static const BYTE FPD_LEN = 10;

    return (fpd.length() < FPD_LEN) ? string(FPD_LEN - fpd.length(),'0') + fpd : fpd;
}

bool Convert::date_convert(string date_str, Date &date)
{
    BYTE offset = 0;

    if(date_str.length() < 8)
    {
        logERR("%s Wrong date len\t:\t%d", __PRETTY_FUNCTION__, date_str.length());
        return false;
    }

    date.day  = QString( date_str.substr( offset, 2 ).c_str() ).toInt();
    offset   += 2;

    date.month = QString( date_str.substr( offset, 2 ).c_str() ).toInt();
    offset    += 2;

    if ( date_str.length() == 12 )
    {
        date.year = QString( date_str.substr( offset, 4 ).c_str() ).toInt() - 2000;
        offset   += 4;
    }
    else
    {
        date.year = QString( date_str.substr( offset, 2 ).c_str() ).toInt();
        offset   += 2;
    }

    date.hours = QString( date_str.substr( offset, 2 ).c_str() ).toInt();
    offset    += 2;

    date.minutes = QString( date_str.substr( offset, 2 ).c_str() ).toInt();

    return true;
}

int Convert::day_in_month_clc( int year, int month )
{

    int days = 0;
    if ((month == 4) || (month == 6) || (month == 9) || (month == 11))
    {
        days = 30;
    }
    else if (month == 2)
    {
        if ( year % 4 == 0)
        {
            days = 29;
        }
        else
        {
            days = 28;
        }
    }
    else
    {
        days = 31;
    }
    return days;
}

uint64_t Convert::weight_str_to_uint64( string weight )
{
    // weight = xxx,xxx

    logDBG( "STRING %s TO UINT64_T CONVERT", weight.c_str() );

    if (weight.length() == 0)
    {
        logERR("STRING IS EMPTY");
        return 0;
    }

    int         comma_pos;
    string    comma = ",";
    uint64_t result =   0;

    comma_pos  = weight.find( comma );
    if (comma_pos > 0)
        weight = weight.erase( comma_pos, 1 );

    while(weight.at( 0 ) == '0')
    {
        if(weight.length() == 0)
            break;
        weight = weight.erase( 0, 1 );
    }

    result = QString(weight.c_str()).toULongLong();

    logDBG( "STRING %s -> %llu UINT64_T", weight.c_str(), result );

    return result;
}

string Convert::zero_erase( string source, int where )
{
    logDBG( "ZERO ERASE FROM %s", source.c_str() );

    if (source.length() == 0)
    {
        logERR("STRING IS EMPTY");
        return source;
    }

    switch(where)
    {
        case LEFT:
            while(source.at(0) == '0')
            {
                if(source.length() == 0)
                    break;
                source = source.erase( 0, 1 );
            }
            break;
        case RIGHT:
            while(source.back() == '0')
            {
                if(source.length() == 0)
                    break;
                source = source.erase( source.length() - 1, 1 );
            }
            break;
        default:
            logERR( "WRONG WHERE!" );
            return source;
    }

    logDBG( "RESULT %s" , source.c_str() );

    return source;
}

time_t Convert::dateToUnix(string date)
{
    // NEED FORMAT YYYY-MM-DD
    struct tm tmVar;
    memset(&tmVar, 0, sizeof(tmVar));

    time_t timeVar = 0;
    if ( sscanf(date.c_str(), "%d-%d-%d", &tmVar.tm_year, &tmVar.tm_mon, &tmVar.tm_mday) == 3 )
    {
        tmVar.tm_year -= 1900;
        tmVar.tm_mon  -= 1;

        timeVar = mktime(&tmVar);
        logDBG("DATE TO UNIX   : %u", timeVar);
    }
    else
    {
        logERR("Bad format!!! Nead YYYY-MM-DD");
    }

    return timeVar;
}

vector<string> Convert::splitString(string divisibleStr, char delimiter)
{
    vector<string> rv;

    if ( divisibleStr.empty() )
        return rv;

    string tmpString;
    istringstream streamStr(divisibleStr);

    while ( getline(streamStr, tmpString, delimiter) )
    {
       rv.push_back( tmpString );
    }

    return rv;
}
//=============================================================
// Добавление строки str до длинны len символом sym
//=============================================================
string Convert::str_complite( string str, char sym, unsigned len )
{
    string result = str;

    while(result.length() < len)
    {
        result.push_back( sym );
    }

    return result;
}

void Convert::strLeftComplite(string &dest, string sub, const size_t &len)
{
    string newStr;

    if(dest.length() >= len)
    {
        logERR("%s Texi is already full!", __PRETTY_FUNCTION__);
        return;
    }

    while(newStr.length() < (len - dest.length()))
    {
        newStr += sub;
    }
    dest.insert(0, newStr);

    if(dest.length() > len)
    {
        dest = dest.substr(dest.length() - len, len);
    }
}

//=============================================================
// Разбиение строки на div_len символов и остаток rest
//=============================================================
string Convert::str_len_div( string &str, unsigned div_len )
{
    string rest;

    if(str.length() < div_len)
    {
        logERR( "STR IS TOO SMALL!!!" );
        return "";
    }
    rest = str.substr( div_len, str.length() - div_len );
    str  = str.substr( 0, div_len );

    return rest;
}


//=============================================================
// Получение дробной части (по албански - по точке или запятой)
// можно выбрать десятичный разделитель по вкусу
//=============================================================
string Convert::getFractionalPart(const string &value, char decimal_point)
{
    std::string fraction;
    fraction.clear();

    size_t n = value.find(".");
    if (n != std::string::npos) {
        fraction = value.substr(n);
        fraction.at(0) = decimal_point;
    } else {
        n = value.find(",");
        if (n != std::string::npos) {
            fraction = value.substr(n);
            fraction.at(0) = decimal_point;
        }
    }

    if (fraction.empty()) {
        fraction = string(decimal_point + "00");
    }

    return fraction;
}


//=============================================================
// Получение числа с триадами
//=============================================================
string Convert::getTriadNumber(const string &value)
{
    uint64_t num;

    try
    {
        num = std::stoull(value);
    }
    catch (const std::invalid_argument& ia)
    {
        return "";
    }

    uint64_t rest = num;
    std::vector<std::string> triads;
    triads.clear();

    std::ostringstream ss;
    while(rest > 0)
    {
        ss.seekp(0);
        ss << std::setw(3) << std::setfill('0') << static_cast<uint16_t>(rest%1000);

        triads.push_back(ss.str());
        rest = rest / 1000;
    }

    std::reverse(triads.begin(), triads.end());

    if (triads.empty())
    {
        return "0";
    }

    triads.at(0) = std::to_string(std::stoull(triads.at(0)));
    std::string result;
    result.clear();
    for ( auto it = triads.begin(); it != triads.end(); ++it )
    {
        result += *it;
        result += " ";
    }

    if (!result.empty())
    {
        result.pop_back();
    }

    return result;
}

string Convert::getTriadNumWithFraction(const string &value, bool zero_fraction_need, char decimal_point)
{
    if(value.empty())
    {
        return std::string{(zero_fraction_need ? "0.00" : "0")};
    }
    string fraction = getFractionalPart(value, decimal_point);

    string dec_frac = "";
    dec_frac += decimal_point;
    dec_frac += "00";

    if ( ((fraction == dec_frac) && !zero_fraction_need) )
    {
        fraction.clear();
    }

    return string(getTriadNumber(value) + fraction);
}



vector<string> Convert::splitStringLength(const string &str, const uint32_t &length)
{
    vector<string> resultList;

    size_t index = 0;
    const size_t lenStr = str.length();

    while ( index < lenStr )
    {
        resultList.push_back( str.substr( index, length ) );
        index += length;
    }

    return resultList;
}


string Convert::getStringCurTime()
{
    string dateTime;

    time_t time = cur_unixtime_get();
    struct tm *unixTime = localtime( &time );

    char tmp[64];
    sprintf(tmp,"%02d%02d%04d%02d%02d%02d", unixTime->tm_mday, unixTime->tm_mon + 1, unixTime->tm_year + YEAR_CONSTANT, unixTime->tm_hour, unixTime->tm_min, unixTime->tm_sec);
    dateTime = charToString(tmp);

    return dateTime;
}

std::string Convert::getDate_YYYY_MM_DD()
{
    time_t rawtime;
    time(&rawtime);
    struct tm *unixTime = gmtime(&rawtime);
    char tmp[10];
    sprintf(tmp,"%04d-%02d-%02d", (unixTime->tm_year + YEAR_CONSTANT), (unixTime->tm_mon + 1), unixTime->tm_mday);

    return charToString(tmp);
}

bool Convert::stringDateTimeToTM(const string &dateTime, tm &calendarDateTime)
{
    const BYTE SHORT_DATE_WITHOUT_TIME = 6;
    const BYTE LONG_DATE_WITHOUT_TIME  = 8;
    const BYTE SHORT_DATE_SHORT_TIME   = 10;
    const BYTE LONG_DATE_SHORT_TIME    = 12;
    const BYTE LONG_DATE_LONG_TIME     = 14;

    const BYTE OFFSET_TWO  = 2;
    const BYTE OFFSET_FOUR = 4;
    const uint16_t HUNDRED = 100;
    const size_t length = dateTime.length();

    BYTE offset = 0;

    calendarDateTime.tm_mday = 0;
    calendarDateTime.tm_mon  = 0;
    calendarDateTime.tm_year = 0;
    calendarDateTime.tm_hour = 0;
    calendarDateTime.tm_min  = 0;
    calendarDateTime.tm_sec  = 0;

    try
    {
        calendarDateTime.tm_mday = stoi(dateTime.substr(offset, OFFSET_TWO).c_str());
        offset += OFFSET_TWO;

        calendarDateTime.tm_mon = stoi(dateTime.substr(offset, OFFSET_TWO).c_str()) - 1;
        offset += OFFSET_TWO;

        switch (length)
        {
            case SHORT_DATE_WITHOUT_TIME:
            case SHORT_DATE_SHORT_TIME:
            {
                calendarDateTime.tm_year = HUNDRED + stoi(dateTime.substr(offset, OFFSET_TWO).c_str());
                offset += OFFSET_TWO;

                break;
            }
            case LONG_DATE_WITHOUT_TIME:
            case LONG_DATE_SHORT_TIME:
            case LONG_DATE_LONG_TIME:
            {
                calendarDateTime.tm_year = stoi(dateTime.substr(offset, OFFSET_FOUR).c_str()) - YEAR_CONSTANT;
                offset += OFFSET_FOUR;

                break;
            }
            default:
            {
                logERR("%s Ошибка! Неверный формат строки для преобразования в структуру tm!", __PRETTY_FUNCTION__);
                return false;
            }
        }

        if ( (length != SHORT_DATE_WITHOUT_TIME) && (length != LONG_DATE_WITHOUT_TIME) )
        {
            calendarDateTime.tm_hour = stoi(dateTime.substr(offset, OFFSET_TWO).c_str());
            offset += OFFSET_TWO;

            calendarDateTime.tm_min = stoi(dateTime.substr(offset, OFFSET_TWO).c_str());
            offset += OFFSET_TWO;

            if ( length == LONG_DATE_LONG_TIME )
            {
                calendarDateTime.tm_sec = stoi(dateTime.substr(offset, OFFSET_TWO).c_str());
                offset += OFFSET_TWO;
            }
        }

        return true;
    }
    catch (exception &_exception)
    {
        logERR("%s EXCEPTION : %s", __PRETTY_FUNCTION__, _exception.what());
    }

    return false;
}

bool Convert::strDateTimeToTimeT(const string &dateTime, time_t &unixTime)
{
    const BYTE DATE_TIME_LEN = 12;

    if (dateTime.length() != DATE_TIME_LEN)
    {
        logERR("%s, Ошибка! Неверный формат строки для преобразования!", __PRETTY_FUNCTION__);
        return false;
    }

    const BYTE OFFSET_TWO  = 2;
    const uint16_t HUNDRED = 100;

    BYTE offset = 0;

    tm calendarDateTime;

    calendarDateTime.tm_mday = 0;
    calendarDateTime.tm_mon  = 0;
    calendarDateTime.tm_year = 0;
    calendarDateTime.tm_hour = 0;
    calendarDateTime.tm_min  = 0;
    calendarDateTime.tm_sec  = 0;

    try
    {
        calendarDateTime.tm_mday = stoi(dateTime.substr(offset, OFFSET_TWO).c_str());
        offset += OFFSET_TWO;

        calendarDateTime.tm_mon = stoi(dateTime.substr(offset, OFFSET_TWO).c_str()) - 1;
        offset += OFFSET_TWO;

        calendarDateTime.tm_year = HUNDRED + stoi(dateTime.substr(offset, OFFSET_TWO).c_str());
        offset += OFFSET_TWO;

        calendarDateTime.tm_hour = stoi(dateTime.substr(offset, OFFSET_TWO).c_str());
        offset += OFFSET_TWO;

        calendarDateTime.tm_min = stoi(dateTime.substr(offset, OFFSET_TWO).c_str());
        offset += OFFSET_TWO;

        calendarDateTime.tm_sec = stoi(dateTime.substr(offset, OFFSET_TWO).c_str());
    }
    catch (exception &_exception)
    {
        logERR("%s EXCEPTION : %s", __PRETTY_FUNCTION__, _exception.what());
        return false;
    }

    return tmToTimet(calendarDateTime, unixTime);
}

bool Convert::tmToTimet(tm &calendarDateTime, time_t &unixTime)
{
    errno = 0;
    unixTime = mktime(&calendarDateTime);
    if ( unixTime == -1 )
    {
        logERR("%s Ошибка преобразования из структуры tm в time_t : %s", __PRETTY_FUNCTION__, strerror(errno));
        return false;
    }

    return true;
}

bool Convert::dateLessSelected(const string &startDatePoint, const string &displacedDatePoint, const int64_t &diff, int64_t &realDiff)
{
    struct tm startDateTM;
    if ( !stringDateTimeToTM(startDatePoint, startDateTM) )
    {
        logERR("%s Ошибка конвертации из строки(начальная дата) в структуру tm!", __PRETTY_FUNCTION__);
        return false;
    }

    struct tm displaceDateTM;
    if ( !stringDateTimeToTM(displacedDatePoint, displaceDateTM) )
    {
        logERR("%s Ошибка конвертации из строки(смещенная дата) в структуру tm!", __PRETTY_FUNCTION__);
        return false;
    }

    time_t unixStartDate = 0;
    if ( !tmToTimet(startDateTM, unixStartDate) )
    {
        logERR("%s Ошибка конвертации из tm(начальная дата) в time_t!", __PRETTY_FUNCTION__);
        return false;
    }

    time_t unixDisplDate = 0;
    if ( !tmToTimet(displaceDateTM, unixDisplDate) )
    {
        logERR("%s Ошибка конвертации из tm(смещенная дата) в time_t!", __PRETTY_FUNCTION__);
        return false;
    }

    realDiff = -(static_cast<int64_t>(difftime(unixStartDate, unixDisplDate)));

    logDBG("%s Начальная дата : %u  Смещенная дата : %u", __PRETTY_FUNCTION__, unixStartDate, unixDisplDate);

    if ( realDiff < 0 )
    {
        return false;
    }

    if ( realDiff <= diff )
    {
        return true;
    }

    return false;
}


string Convert::getStringExpireDays(int64_t &expireSec)
{
    string expireDaysSrt;
    int64_t expireDays = expireSec / (Convert::TIME_HOUR_IN_DAY * Convert::TIME_MIN_IN_HOUR * Convert::TIME_SEC_IN_MIN) + 1;

    int modDay    = expireDays % 10;
    bool oneDay   = (modDay == 1) && (expireDays != 11);

    if(oneDay)
    {
        expireDaysSrt = "остался " + to_string(expireDays);
    }
    else
    {
        expireDaysSrt = "осталось " + to_string(expireDays);
    }

    expireDaysSrt += " д.";

    if(expireSec <= 0)
    {
        expireSec = 0;
        expireDaysSrt.clear();
    }

    return expireDaysSrt;
}

std::string Convert::hex2str(const BYTE &hex)
{
    const BYTE TEMPLATE_LEN = 3;
    char dest[TEMPLATE_LEN];
    sprintf(dest, "%02x", hex);
    return string(dest);
}

// Преобразуем массив src размера len в хекс строку размера len*2 (но с нуль терминантом!)
string Convert::hex2ASCII(const char *src, const size_t &len)
{
    string result;
    for (size_t j = 0; j < len; ++j )
    {
        char tmp[3] = {0x00};
        snprintf(tmp, sizeof(tmp), "%02x", *(src + j));
        result += string(tmp);
    }
    logDBG("%s result ASCII STRING(len = %u) = %s", __FUNCTION__, result.length(), result.c_str());
    return result;
}

void Convert::appendToVectorStr(const std::string &source, std::vector<BYTE> &dest)
{
    for ( const char a : source )
    {
        dest.push_back( static_cast<BYTE>(a) );
    }
}

std::string Convert::strToHexStr(const std::string &inStr)
{
    stringstream ss;
    for(const char ch : inStr)
    {
        ss << std::hex << std::setfill ('0') << std::setw (2) << static_cast<uint16_t>(ch);
    }
    return ss.str();
}


bool Convert::appendToVectNum(const std::string &source, std::vector<BYTE> &dest, const uint64_t &minSize)
{
    int64_t value = QString(source.c_str()).toLongLong();
    if (errno == ERANGE)
    {
        logERR( "%s:: Ошибка диапазона, выход! строка = %s", __func__, source.c_str());
        errno = 0;
        return false;
    }
    constexpr BYTE byteMask = 0xff;
    BYTE len {0};
    size_t  startSize{dest.size()};
    while(value > 0)
    {
        dest.insert(dest.begin()+startSize, (value & byteMask) );
        value = value >> CHAR_BIT;
        ++len;
    }
    while(len < minSize)
    {
        dest.push_back(0);
        ++len;
    }
    return true;
}

std::string Convert::hexStrGet(const std::string &source, const uint64_t &size)
{
    std::string result;
    int64_t value = QString(source.c_str()).toLongLong();
    if (errno == ERANGE)
    {
        logERR( "%s:: Ошибка диапазона, выход! строка = %s", __func__, source.c_str());
        errno = 0;
        return result;
    }
    constexpr BYTE byteMask = 0xff;

    while(value > 0)
    {
        result.insert(0, '$' + hex2str(value & byteMask));
        value = value >> CHAR_BIT;
    }

    if(size != 0)
    {
        strLeftComplite(result, "$00", size * BIOS_BYTE_LEN);
    }

    return result;
}

string Convert::getStrDate(const tm& timeinfo)
{
    char tmp[11] = {0};
    sprintf(tmp,"%02d.%02d.%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
    return {tmp};
}

bool Convert::checkDayMonth(const tm &timeinfo)
{
    if ((timeinfo.tm_mon == MONTHS_APRIL) || (timeinfo.tm_mon == MONTHS_JUNE) || (timeinfo.tm_mon == MONTHS_SEPTEMBER) || (timeinfo.tm_mon == MONTHS_NOVEMBER))
    {
        if (timeinfo.tm_mday > DAYS_IN_SHORT_MONTH)
        {
            return false;
        }
    }
    else if (timeinfo.tm_mon == MONTHS_FEBRUARY)
    {
        if (((timeinfo.tm_year % BISSEXTILE_YEAR == 0) && (timeinfo.tm_mday > DAYS_IN_FEBRUARY_BISSEXTILE)) ||
            ((timeinfo.tm_year % BISSEXTILE_YEAR != 0) && (timeinfo.tm_mday > DAYS_IN_FEBRUARY)))
        {
            return false;
        }
    }
    else
    {
        if (timeinfo.tm_mday > DAYS_IN_LONG_MONTH)
        {
            return false;
        }
    }

    return true;
}

CDate Convert::DateTime_Get()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    CDate cdate;
    cdate.dostime.hour = tm.tm_hour;
    cdate.dostime.minute = tm.tm_min;
    cdate.dostime.second = tm.tm_sec;
    cdate.dosdate.month = tm.tm_mon + 1;
    cdate.dosdate.day = tm.tm_mday;
    cdate.dosdate.year = tm.tm_year + 1900;

    return cdate;
}

//Convert::DATE_ISO_8601 Convert::parseISO8601_2004(const std::string &date)
//{
//    logINFO("%s", __FUNCTION__);
//    // Паттерн регулярного выражения для даты формата ISO8601:2004 YYYYMMDDThhmm
//    boost::regex dateISO8601Template("^(?<year>\\d{4})(?<month>\\d{2})(?<day>\\d{2})T(?<hours>\\d{2})(?<minutes>\\d{2})$");
//    // Контейнер с результатом парса
//    boost::smatch match;
//    // Структура даты
//    DATE_ISO_8601 dateISO8601;
//    // Парс даты
//    try
//    {
//        // Тест результата парса
//        if(boost::regex_match(date, match, dateISO8601Template))
//        {
//            // Парс прошёл успешно
//            logINFO("%s parse success", __FUNCTION__);
//            dateISO8601.year = QString(match.str("year").c_str());
//            dateISO8601.month = QString(match.str("month").c_str());
//            dateISO8601.day = QString(match.str("day").c_str());
//            dateISO8601.hours = QString(match.str("hours").c_str());
//            dateISO8601.minutes = QString(match.str("minutes").c_str());
//            constexpr int tzMoscow = 3;
//            dateISO8601.time_zone = tzMoscow;
//        }
//        else
//        {
//            // Парс не завершён
//            logINFO("%s expression : %s parse failed!", __FUNCTION__, date.c_str());
//        }
//    }
//    catch (boost::exception &ex)
//    {
//        // Сообщение в лог при возникновении исключения
//        logERR("%s expression : %s %s", __FUNCTION__, date.c_str(), boost::diagnostic_information(ex).c_str());
//    }

//    return dateISO8601;
//}

//std::string Convert::getFmtPhone(const std::string &phone)
//{
//    if (phone.empty())
//    {
//        return std::string();
//    }
//    logINFO("%s", __FUNCTION__);
//    // Паттерн регулярного выражения для телефонного номера формата +7xxxxxxxxxx (rus)/xxxxxxxxxxx (international)
//    boost::regex phoneTemplate("^((?<interCode>\\d{1})|(?<rusCode>\\+7))(?<defCode>\\d{3})\\d{5}(?<endPart>\\d{2})$");
//    // Контейнер с результатом парса
//    boost::smatch match;
//    // Парс номера
//    try
//    {
//        // Тест результата парса
//        if(boost::regex_match(phone, match, phoneTemplate))
//        {
//            // Парс прошёл успешно
//            logINFO("%s parse success", __FUNCTION__);
//            // Формируем форматированный телефонный номер +7-xxx-***-**-xx (rus)/ x-xxx-***-**-xx (international)
//            std::stringstream fmtPhone;
//#ifndef SPUTNIK_F
//            fmtPhone << boost::format("%s-%s-***-**-%s") % (match.str("rusCode") + match.str("interCode")) %  match.str("defCode") % match.str("endPart");
//#else
//            fmtPhone << boost::format("%s%s*****%s") % (match.str("rusCode") + match.str("interCode")) %  match.str("defCode") % match.str("endPart");
//#endif
//            logINFO("%s result formated phone : %s", __FUNCTION__, fmtPhone.str().c_str());
//            return fmtPhone.str();
//        }
//        else
//        {
//            // Парс не завершён
//            logINFO("%s expression : %s parse failed!", __FUNCTION__, phone.c_str());
//        }
//    }
//    catch (const boost::io::format_error &ex)
//    {
//        // Сообщение в лог при возникновении исключения
//        logERR("%s Ops!\t%s", __FUNCTION__, boost::diagnostic_information(ex).c_str());
//    }
//    catch (boost::exception &ex)
//    {
//        // Сообщение в лог при возникновении исключения
//        logERR("%s expression : %s %s", __FUNCTION__, phone.c_str(), boost::diagnostic_information(ex).c_str());
//    }

//    return std::string();
//}

std::string Convert::alignCenter(const std::string &src, const size_t widthArea)
{
    const size_t len {src.length()};
    const size_t indent {(len < widthArea) ? ((widthArea - len) / 2) : 0};
    return string(indent, ' ') + src + '\n';
}

//int Convert::getRand()
//{
//    std::srand(std::time(nullptr));
//    return std::rand();
//}

void Convert::toUpperCase(std::string &src)
{
    src = QString(src.c_str()).toUpper().toStdString();
//    try
//    {
//        boost::to_upper(src);
//    }
//    catch (const boost::bad_any_cast& e)
//    {
//        logERR("%s Ops! %s", __FUNCTION__, e.what());
//    }
}

void Convert::toLowerCase(std::string &src)
{
    src = QString(src.c_str()).toLower().toStdString();
//    try
//    {
//        boost::to_lower(src);
//    }
//    catch (const boost::bad_any_cast& e)
//    {
//        logERR("%s Ops! %s", __FUNCTION__, e.what());
//    }
}

std::string Convert::complite(const std::string& src, const char sym, const size_t len)
{
    std::string dst {src};
    const size_t srcLen {dst.length()};
    const size_t compliteLen {(len > srcLen) ? (len - srcLen) : 0};
    dst.insert(srcLen, std::string(compliteLen, sym));
    return dst;
}

unsigned char* Convert::hex2char(const char *hexStr, uint64_t &len)
{
    logDBG("%s", __PRETTY_FUNCTION__);

    const size_t  STR_LEN     = strlen(hexStr);
                  len         = STR_LEN / 2;
    const uint8_t BYTE        = 8;
    const uint8_t OFFSET      = 2;
    const uint8_t MAGIC_VALUE = 25;

    unsigned char *chrs       = nullptr;

    if ((STR_LEN % OFFSET) != 0)
    {
        logERR("%s Wrong hexStr len!", __PRETTY_FUNCTION__);
        return chrs;
    }

    chrs = new unsigned char[len];
    memset(chrs, 0x00, static_cast<size_t>(len));

    for (size_t i = 0, j = 0; j < len; i += OFFSET, ++j)
    {
        chrs[j] = (hexStr[i] % (4 * BYTE) + (BYTE + 1)) % MAGIC_VALUE * (2 * BYTE) + (hexStr[i + 1] % (4 * BYTE) + (BYTE + 1)) % MAGIC_VALUE;
        logDBG("0x%02x", chrs[j]);
    }

    return chrs;
}

std::string Convert::insertSymbol(std::string src, const char symbol, const size_t interval)
{
    size_t offset {0};
    std::stringstream ss;
    try
    {
        while (offset < src.length())
        {
            ss << "$" << src.substr(offset, interval);
            offset += 2;
        }
    }
    catch (const std::out_of_range&) {}

    return ss.str();
}

std::string Convert::exec(const char* cmd)
{           
    char buffer[128] = {0x00};
    std::string result;
    FILE *pipe = popen(cmd, "r");

    if (pipe)
    {
        try
        {
            while (fgets(buffer, sizeof buffer, pipe) != NULL)
            {
                result += buffer;
            }
        }
        catch (...)
        {
            pclose(pipe);
            throw;
        }
        pclose(pipe);
    }

    return Convert::onlyPrintable(result);
}

int Convert::systemExec(const std::string &cmd)
{
//    int rv {std::system(cmd.c_str())};
//    rv = (WIFEXITED(rv) != 0) ? WEXITSTATUS(rv) : -1;

    return 0; //rv;
}

std::string Convert::double2str(const long double value, const streamsize prec)
{
    std::stringstream ss;
    ss.precision(prec);
    ss << std::fixed << value;
    return ss.str();
}

//bool Convert::isIp(const std::string &src)
//{
//    bool rc {false};
//    boost::regex ipTmpl("^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$");
//    boost::smatch match;

//    try
//    {
//        // Тест результата парса
//        rc = boost::regex_match(src, match, ipTmpl);
//    }
//    catch (boost::exception &ex)
//    {
//        // Сообщение в лог при возникновении исключения
//        logERR("%s expression : %s %s", __FUNCTION__, src.c_str(), boost::diagnostic_information(ex).c_str());
//    }
//    return rc;
//}

//bool Convert::isKpp(const std::string &src)
//{
////    bool rc {false};
////    boost::regex kppTmpl("^\\d{9}$");
////    boost::smatch match;

////    try
////    {
////        // Тест результата парса
////        rc = boost::regex_match(src, match, kppTmpl);
////    }
////    catch (boost::exception &ex)
////    {
////        // Сообщение в лог при возникновении исключения
////        logERR("%s expression : %s %s", __FUNCTION__, src.c_str(), boost::diagnostic_information(ex).c_str());
////    }
////    return rc;
//    return true;
//}

std::string Convert::getDateYearsAgo(const int years)
{
    time_t unixtime = time(nullptr);
    struct tm date = *localtime(&unixtime);
    if (date.tm_year >= years)
    {
        date.tm_year -= years;
    }
    char dateStr[10] = {0x00};
    Convert::getStrDate(dateStr, date);
    return std::string(dateStr);
}
