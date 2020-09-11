/*
 * convert.h
 */

#ifndef SRC_APPL_CONVERT_H_
#define SRC_APPL_CONVERT_H_

#include <sys/time.h>
#include <time.h>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <list>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <limits.h>

#include "../loggerdaemon.h"
#include "libs/armeabi-v7a/qFisGoPirit/PiritLib.h"

using namespace std;

class Convert
{
private:
    static const std::string bs64Values;
    typedef uint8_t BYTE;
public:
    Convert(){}
    virtual ~Convert(){}
    enum TIME
    {
        TIME_SEC_IN_MIN  = 60,
        TIME_MIN_IN_HOUR = 60,
        TIME_HOUR_IN_DAY = 24,
        TIME_SEC_IN_HOUR = 3600
    };
    /**
     * @brief The LIMITS enum ограничения для regexp
     */
    enum LIMITS
    {
        LIMIT_MAX_BUYER_ADDR = 64
    };

    virtual bool isMacValid( string );
    /**
     * @brief isEmailValid тест email на валидность
     * @param email адрес почты
     * @return валидный?
     */
    virtual bool isEmailValid(const string &email);
    /**
     * @brief isPlateNumberValid тест ГТС ТС на валидность
     * @param plateNumber ГТС ТС
     * @return валидный?
     */
    static bool isPlateNumberValid(const std::string &plateNumber);
    /**
     * @brief isTelNumberValid тест телефонного номера на валидность
     * @param telNumber телефонный номер
     * @return валидный?
     */
    virtual bool isTelNumberValid(const string &telNumber);
	static const int LEFT  = 1,
	                 RIGHT = 2;
    static string zero_erase( string source, int where );

    static const uint16_t SEC_IN_HOUR = 3600; /*!< секунд в часе*/
    static const BYTE BISSEXTILE_YEAR = 4; /*!< високостный год*/

    struct Date //LIB
    {
        int day;
        int month;
        int year;
        int hours;
        int minutes;
    };

    virtual time_t dateToUnix(string date);
    virtual bool isFnNumberValid(string fnNumber);
    virtual void getDateTime(tm &timeinfo);
    virtual DateTime getDateTime(const time_t *time);
    virtual int32_t getTimeZone();
	static void getStrDate(char *tmp, tm timeinfo);
	static void getStrTime(char *tmp, tm timeinfo);
	static string getStrTime(tm timeinfo);
	static string getStrDateTime(tm ti);
	static string getStrFullDateTime(tm ti);
	static string getStrShortDateTime(time_t time);
	static string getStrFullDateTime(time_t time);
    static string getStrFullDateTimeUtc(time_t time);
	static string charToString( const char *source);
	static string charToString( const char *source, uint32_t max_size);
	static string intToString ( unsigned int *source );
    virtual string sumToString(uint64_t val);
    inline std::string sumToStringShort(uint64_t val) {return (val % Convert::DIVISIOR_RUBL == 0) ? std::to_string(val / Convert::DIVISIOR_RUBL) : sumToString(val);}
    static string sumToString(int64_t val);
    static time_t convertToUnixTime(tm timeinfo);
    static char* str_to_char( string );
	static void stringToChar( string source, char *dest, uint32_t max_size);
	static string time_str_create( unsigned char *date );
    virtual string time_str_get( time_t time );
    virtual time_t cur_unixtime_get();
    virtual time_t cur_localtime_get();
	static int cur_unixtime_usec_get( timeval * );
    virtual string dateTimeToStr(time_t time);
	static string char_to_string_convert( const char *text, unsigned short len );
	static string end_symbols_delete( string text, unsigned char symbol_code );
	static string weight_to_str( uint64_t weight );
    static bool convertASCIImassToHex(char (&data)[1000], int &size);
	static string quantityToString( uint64_t quantity, bool isWeightGoods );
	static string escapingOfQuotes(string srcStr);
    static string escapingOfSqlite(string srcStr, string find, string replace);
	static string escapingOfSlash(string srcStr);
	static bool isDoUpdateCompareVersions(string curVersion, string updVersion);
	static int charToInt(const char *src, int maxLen);
	static string replaceSymbols(const string val);
	static void dateTimeToDDate(time_t time, int *year, int *month, int *day, int *hour, int *min, int *sec);
    virtual string intDateTimeToStr(int year, int month, int day, int hour, int min, int sec, int time_zone);
    /**
     * @brief removeDel удаление разделителя из строки
     * @param src строка
     * @param del разделитель
     */
    static void removeDel(std::string &src, const std::string &del);
	struct DATE_ISO_8601
	{
        int       day = 0,
                month = 0,
                 year = 0,
                hours = 0,
              minutes = 0,
              seconds = 0,
            time_zone = 0;
	};

    enum MONTHS
    {
        MONTHS_JANUARY = 0,
        MONTHS_FEBRUARY,
        MONTHS_MARCH,
        MONTHS_APRIL,
        MONTHS_MAY,
        MONTHS_JUNE,
        MONTHS_JULY,
        MONTHS_AUGUST,
        MONTHS_SEPTEMBER,
        MONTHS_OCTOBER,
        MONTHS_NOVEMBER,
        MONTHS_DECEMBER
    };

    enum DAYS_IN_MONTH
    {
        DAYS_IN_FEBRUARY            = 28,
        DAYS_IN_FEBRUARY_BISSEXTILE = 29,
        DAYS_IN_SHORT_MONTH         = 30,
        DAYS_IN_LONG_MONTH          = 31
    };

    enum PREC
    {
        PREC_MS = 0,
        PREC_S
    };

	enum DIVISIORS
	{
        DIVISIOR_TEN               =     10,
        DIVISIOR_RUBL              =    100,
        DIVISIOR_THOUSAND          =   1000
	};

    static const BYTE BIOS_BYTE_LEN = 3;
    virtual  string DateISO8601Create( DATE_ISO_8601 *, PREC prec = PREC_MS );
    virtual  bool DateISO8601Create( DATE_ISO_8601&, string, bool useTime = false );

    static uint32_t uchar_to_uint32_convert( unsigned char *data );
    /**
     *
     * @param buf - buf for calc crc32
     * @param len - len of buf
     * @return  -crc32 result
     */
    static uint32_t crc32(const BYTE* buf, const uint32_t& len);
    /**
     *
     * @param digit - digit to convertation to LE vect
     * @return - digit, transformed to vector in Little Endian
     */
    static const std::vector<BYTE>& getCharsLE(const uint32_t &digit);
    static const std::vector<BYTE>& getCharsLE(const uint16_t &digit);

    /**
     * @brief dateISO8601toTimeT конвертация времени из DATE_ISO_8601 в time_t
     * @return время по шкале time_t
     */
    virtual time_t dateISO8601toTimeT( DATE_ISO_8601 &dateIso );

   // virtual time_t makeTimeTZ(struct tm *tm);

    static bool date_convert(string date_str, Date &date);
	static  int  day_in_month_clc( int year, int month );
	static  bool date_difference( string date_cur_str, string date_prev_str, int minutes );
	static  uint64_t weight_str_to_uint64( string str );
    static  int crc_16_ciit_clc( const char *data, unsigned short len );
    static  double str_to_double  (string cop);
    /**
     * @brief cop2rub cop2rub конвертер суммы в копейках в сумму в рублях
     * @param cop сумма в копейках
     * @return сумма в рублях
     */
    template<typename T>
    double cop2rub(const T &cop) {return cop/100.;}

    static  uint64_t double_to_uint64(double rub);
    virtual string int64_to_string_prec(int64_t, BYTE);
    std::string uint64toStrPrec(uint64_t, BYTE);
    static  int pld_date_create( string date_str, unsigned char &DD, unsigned char &MM, int &YYYY );

    static vector<string> splitString(string divisibleStr, char delimiter);
    static string str_len_div( string &, unsigned );
    static string str_complite(string, char, unsigned  );

    static string getFractionalPart(const string &value, char decimal_point = ',');
    static string getTriadNumber(const string &value);
    static string getTriadNumWithFraction(const string &value, bool zero_fraction_need = true, char decimal_point = ',');
    /**
     * @brief strLeftComplite - дополнить строку слева
     * @param dest - строка
     * @param sub - подстрока
     * @param len - длинна
     */
    static void strLeftComplite(string &dest, string sub, const size_t &len);
    /**
     * @brief splitStringLength разделить строку по длине
     * @param str исходная строка
     * @param length максимальная длина
     * @return результирующие строки
     */
    static std::vector<string> splitStringLength(const std::string &str, const uint32_t &length);
    /**
     * @brief getStringCurTime - получить текущие дату и время в строке
     * формат ddMMYYYYHHmmss
     * @return текущие дата и время
     */
    virtual string getStringCurTime();
    /**
     * @brief getDate_YYYY_MM_DD чтение текущей даты в формате ГГГГ-ММ-ДД
     * @return дата в формате ГГГГ-ММ-ДД
     */
    static std::string getDate_YYYY_MM_DD();
    /**
     * @brief stringDateTimeToTM - преобразование из строки в структуру tm
     * Формат входных данных:
     * ddMMYY, ddMMYYYY, ddMMYYHHmm, ddMMYYYYHHmm, ddMMYYYYHHmmss
     * @param dateTime - строка с датой и временем
     * @param calendarDateTime - заполненная структура tm
     * @return true - в случае успеха, иначе false
     */
    virtual bool stringDateTimeToTM(const string &dateTime,tm &calendarDateTime);

    /**
     * @brief strDateTimeToTimet - преобразование из строки в time_t
     * Формат входных данных:
     * ddMMYYHHmmss
     * @param[in] dateTime - строка с датой и временем
     * @param[out] unixTime - время из tm по шкале unix time
     * @return true - в случае успеха, иначе false
     */
    virtual bool strDateTimeToTimeT(const string &dateTime, time_t &unixTime);

    /**
     * @brief tmToTimet - преобразование структуры tm в time_t
     * @param calendarDateTime - структура tm
     * @param unixTime - время из tm по шкале unix time
     * @return true - в случае успеха, иначе false
     */
    virtual bool tmToTimet(tm &calendarDateTime, time_t &unixTime);

    /**
     * @brief dateLessSelected - проверка разницы двух дат
     * Даты в формате ddMMYY, ddMMYYYY, ddMMYYHHmm, ddMMYYYYHHmm, ddMMYYYYHHmmss
     * @param startDatePoint - начальная дата (от нее ведется разница)
     * @param displacedDatePoint - конечная дата
     * @param diff - значение разницы дат в секундах
     * @param realDiff - реальное значение разницы дат в секундах
     * @return true - если начальная дата идет раньше конечной и разница меньше diff, иначе false
     */
    virtual bool dateLessSelected(const string &startDatePoint, const string &displacedDatePoint, const int64_t &diff, int64_t &realDiff);
    /**
     * @brief doubleToUint64ArithmeticRound - перевод из double в uint64_t с арифметическим округлением
     * @param value - число double
     * @return округленное число uint64_t
     */
    static uint64_t doubleToUint64ArithmeticRound(const double &value);

    /**
     * @brief getStringExpireDays - получить строку с оставшимся количеством дней фичи
     * @param expireSec - секунды до истечения фичи
     * @return строка вида "осталось XX д."
     */
    string getStringExpireDays(int64_t &expireSec);
    /**
     * @brief hex2str - hex байт в строку BIOS
     * @param hex - байт
     * @return строка сбайта в hex
     */
    static std::string hex2str(const BYTE &hex);
    /**
     * @brief hexStrGet - перевести число в hex строку big-endian
     * @param source - источник
     * @param size - размер результирующей строки в символах
     * @return hex строка big-endian
     */
    static string hexStrGet(const string &source, const uint64_t &size = 0);
    /**
     * @brief fpdValid - валидация до 10 символов
     * @return валидная строка
     */
    string fpdValid(string fpd);

    /**
     * @brief getStrDate - конвертация timeinfo в std::string
     * @param[in] timeinfo - время
     * @return строка
     */
    static string getStrDate(const tm& timeinfo);

    /**
     * @brief checkDayMonth - проверка валидности дня в месяце
     * @param[in] timeinfo - время
     * @return true - если такой день возможен или false если нет
     */
    static bool checkDayMonth(const tm& timeinfo);
    /**
     * @brief dateIsoToTimet - конвертация dateIso в time_t
     * @param[in] dateIso - время
     * @return время
     */
    static time_t dateIsoToTimet(const DATE_ISO_8601& dateIso);

    /**
     * @brief tmToStr - преобразование структуры tm в строку ДДММГГЧЧММ
     * @param timeinfo
     * @return строка ДДММГГЧЧММ
     */
    static string tmToStr(const struct tm& timeinfo);

    /**
     * @brief Convert::tmToISODate - преобразование структуры tm в DATE_ISO_8601
     * @param dateTime - время в tm
     * @return время в DATE_ISO_8601
     */
    DATE_ISO_8601 tmToISODate(const tm &dateTime);

    /**
     * @brief getISODate - получить текущую ISO дату
     * @return ISO "YYYYMMDDTHH:MM:SS.UUUUUU+ZZ:00
     */
    string getISODate();
    /**
     * @brief hex2ASCII ASCII-байты в строку
     * @param src ASCII-байты
     * @param len длинна данных
     * @return результирующая строка
     */
    string hex2ASCII(const char *src, const size_t &len);

    /**
     * @param source исходное число в виде строки ascii символов
     * @param dest - вектор куда складываем байты числа
     * @param minSize - минимальная длина добавляемой части, остальное забиваем нулями
     *  @return выполнено/не выполнено
     */
    static bool appendToVectNum(const string &source, std::vector<BYTE> &dest, const uint64_t &minSize = 0);

    /**
     * @param source - исходная строка
     * @param dest - вектор куда добавляем байты
     */
    static void appendToVectorStr(const std::string &source, std::vector<BYTE> &dest);

    /**
     *  @param inStr входная строка для преобразования в хекс строку
     *  @return результирующая строка где каждый символ из вх строки представлен в хекс записи
     */
    static std::string strToHexStr(const std::string &inStr);
    /**
     * @brief DateTime_Get получить дату и время в формате NVR
     * @return дата и время в формате NVR
     */
    static CDate DateTime_Get();
//    /**
//     * @brief parseISO8601_2004 парсер даты формата YYYYMMDDThhmm
//     * @param date дата формата YYYYMMDDThhmm
//     * @return структура даты DATE_ISO_8601
//     */
//    virtual DATE_ISO_8601 parseISO8601_2004(const std::string &date);
    /**
     * @brief getFmtPhone получить форматированный тетефонный номер
     * @param phone исходный телефонный номер
     * @return форматированный тетефонный номер +7-xxx-***-**-xx (rus)/ x-xxx-***-**-xx (international)
     */
    static std::string getFmtPhone(const std::string &phone);
    /**
     *
     * @param in - исходный вектор байт/строка/иной контейнер char/BYTE для кодирования. должен иметь push_back и size
     * @return   - закодированная строка в base64
     */
    template<typename T> static std::string toBase64(const T &in);
    /**
     *
     * @param in - входная base64 строка
     * @return   - раскодированный вектор байт/строка/иной контейнер. должен иметь push_back
     */
    template<typename T> static T fromBase64(const std::string &in);
    /**
     * @brief isCntrl тест на управляющий символ
     * @param ch символ
     * @return результат выполнения
     */
    template<typename T> static inline const bool isCntrl(const T& ch) {return std::iscntrl(static_cast<unsigned char>(ch));}
    /**
     * @brief stripCntrl удаление управляющих символов
     * @param src STL Containers
     * @return STL Containers без управляющих символов
     */
    template<typename T> static const T stripCntrl(T src)
    {
        src.erase(std::remove_if(begin(src), end(src), [](const unsigned char& ch){return isCntrl(ch);}), end(src));
        return src;
    }
    template<typename T> static const T onlyPrintable(T src)
    {
        src.erase(std::remove_if(begin(src), end(src), [](const unsigned char& ch){return (ch < 0x20 || ch >0x7E);}), end(src));
        return src;
    }
    /**
     * @brief alignCenter выравнивание текста по центру
     * @param src исходный текст
     * @param width ширина поля
     * @return выравненный по центру текст
     */
    static std::string alignCenter(const std::string& src, const size_t widthArea);
    /**
     * @brief getRand генератор случайных чисел
     * @return случайное число
     */
    static int getRand();
    /**
     * @brief toUpperCase преобразование в верхнеуровневый регистр
     * @param src исходный текст [после преобразования текст в верхнеуровневом регистре]
     * no throw
     */
    static void toUpperCase(std::string& src);
    /**
     * @brief toLowerCase преобразование в нижнеуровневый регистр
     * @param src исходный текст [после преобразования текст в нижнеуровневый регистр]
     * no throw
     */
    static void toLowerCase(std::string& src);
    /**
     * @brief complite дополнение строки до нужной длинны
     * @param src исходный текст
     * @param sym сивол дополнения
     * @param len требуемая длинна
     * @return результирующая строка
     */
    static std::string complite(const std::string &src, const char sym, const size_t len);
    /**
     * @brief hex2char конвертер строки байт в массив байтов
     * @param hexstr строка байт
     * @param len длинна массива байт
     * @return массив байтов
     */
    static unsigned char* hex2char(const char* hexstr, uint64_t &len);
    /**
     * @brief insertSymbol циклическая вставка символа в строку
     * @param src исходная строка
     * @param symbol символ для вставки
     * @param interval интервал вставки
     * @return результрующая строка
     */
    static std::string insertSymbol(std::string src, const char symbol, const size_t interval);
    /**
     * @brief exec системный вызов
     * @param cmd команда
     * @return результат выполнения
     * [no throw]
     */
    static std::string exec(const char* cmd);
    /**
     * @brief systemExec системный вызов с кодом возврата
     * @param cmd системная команда
     * @return код возврата
     */
    static int systemExec(const std::string &cmd);
    /**
     * @brief double2str конвертер double в std::string
     * @param value значение с точкой
     * @param prec точность
     * @return строковое представление
     */
    static std::string double2str(const long double value, const std::streamsize prec = 2);
    /**
     * @brief isIp валидация ip-адреса
     * @param src ip-адрес
     * @return результат выполнения
     */
    static bool isIp(const std::string &src);
    /**
     * @brief isKpp  валидация КПП
     * @param src КПП
     * @return результат выполнения
     */
    static bool isKpp(const std::string &src);
    /**
     * @brief getDateYearsAgo расчёт даты years лет назад
     * @param years количество лет
     * @return результат выполнения
     */
    static std::string getDateYearsAgo(const int years);
};

// to bs64
template<typename T> std::string Convert::toBase64(const T &in)
{
    std::string out;
    int val = 0;
    int valb=-6;
    BYTE shift6{6};
    BYTE shift8{8};

    BYTE mask6bit(0x3F);
    for (const BYTE c : in)
    {
        val = (val<<shift8) + c;
        valb += shift8;
        while (valb>=0)
        {
            out.push_back(bs64Values[(val>>valb)&mask6bit]);
            valb-=shift6;
        }
    }
    if (valb>-shift6)
    {
        out.push_back(bs64Values[((val<<shift8)>>(valb+shift8))&mask6bit]);

    }

    BYTE sectorSize{4};
    while (out.size()%sectorSize != 0)
    {
        out.push_back('=');
    }
    return out;
}

// from bs64
template<typename T> T Convert::fromBase64(const std::string &in)
{
    T out;
    std::vector<int> V(256,-1);
    BYTE charIndex;
    BYTE shift6{6};
    BYTE shift8{8};

    for (int16_t i=0; i<64; ++i)
    {
        charIndex =  static_cast<BYTE>(bs64Values[i]);
        V[charIndex] = i;
    }

    int val=0, valb=-shift8;
    for (auto c : in)
    {
        if (V[c] == -1) break;
        val = (val<<shift6) + V[c];
        valb += shift6;
        if (valb>=0)
        {
            out.push_back(static_cast<char>((val>>valb)&0xFF));
            valb-=shift8;
        }
    }
    return out;
}

#endif /* SRC_APPL_CONVERT_H_ */

