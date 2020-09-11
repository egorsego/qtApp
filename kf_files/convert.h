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
     * @brief The LIMITS enum ��࠭�祭�� ��� regexp
     */
    enum LIMITS
    {
        LIMIT_MAX_BUYER_ADDR = 64
    };

    virtual bool isMacValid( string );
    /**
     * @brief isEmailValid ��� email �� ����������
     * @param email ���� �����
     * @return �������?
     */
    virtual bool isEmailValid(const string &email);
    /**
     * @brief isPlateNumberValid ��� ��� �� �� ����������
     * @param plateNumber ��� ��
     * @return �������?
     */
    static bool isPlateNumberValid(const std::string &plateNumber);
    /**
     * @brief isTelNumberValid ��� ⥫�䮭���� ����� �� ����������
     * @param telNumber ⥫�䮭�� �����
     * @return �������?
     */
    virtual bool isTelNumberValid(const string &telNumber);
	static const int LEFT  = 1,
	                 RIGHT = 2;
    static string zero_erase( string source, int where );

    static const uint16_t SEC_IN_HOUR = 3600; /*!< ᥪ㭤 � ��*/
    static const BYTE BISSEXTILE_YEAR = 4; /*!< ��᮪���� ���*/

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
     * @brief removeDel 㤠����� ࠧ����⥫� �� ��ப�
     * @param src ��ப�
     * @param del ࠧ����⥫�
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
     * @brief dateISO8601toTimeT ��������� �६��� �� DATE_ISO_8601 � time_t
     * @return �६� �� 誠�� time_t
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
     * @brief cop2rub cop2rub �������� �㬬� � �������� � �㬬� � �㡫��
     * @param cop �㬬� � ��������
     * @return �㬬� � �㡫��
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
     * @brief strLeftComplite - ��������� ��ப� ᫥��
     * @param dest - ��ப�
     * @param sub - �����ப�
     * @param len - ������
     */
    static void strLeftComplite(string &dest, string sub, const size_t &len);
    /**
     * @brief splitStringLength ࠧ������ ��ப� �� �����
     * @param str ��室��� ��ப�
     * @param length ���ᨬ��쭠� �����
     * @return १������騥 ��ப�
     */
    static std::vector<string> splitStringLength(const std::string &str, const uint32_t &length);
    /**
     * @brief getStringCurTime - ������� ⥪�騥 ���� � �६� � ��ப�
     * �ଠ� ddMMYYYYHHmmss
     * @return ⥪�騥 ��� � �६�
     */
    virtual string getStringCurTime();
    /**
     * @brief getDate_YYYY_MM_DD �⥭�� ⥪�饩 ���� � �ଠ� ����-��-��
     * @return ��� � �ଠ� ����-��-��
     */
    static std::string getDate_YYYY_MM_DD();
    /**
     * @brief stringDateTimeToTM - �८�ࠧ������ �� ��ப� � �������� tm
     * ��ଠ� �室��� ������:
     * ddMMYY, ddMMYYYY, ddMMYYHHmm, ddMMYYYYHHmm, ddMMYYYYHHmmss
     * @param dateTime - ��ப� � ��⮩ � �६����
     * @param calendarDateTime - ����������� ������� tm
     * @return true - � ��砥 �ᯥ�, ���� false
     */
    virtual bool stringDateTimeToTM(const string &dateTime,tm &calendarDateTime);

    /**
     * @brief strDateTimeToTimet - �८�ࠧ������ �� ��ப� � time_t
     * ��ଠ� �室��� ������:
     * ddMMYYHHmmss
     * @param[in] dateTime - ��ப� � ��⮩ � �६����
     * @param[out] unixTime - �६� �� tm �� 誠�� unix time
     * @return true - � ��砥 �ᯥ�, ���� false
     */
    virtual bool strDateTimeToTimeT(const string &dateTime, time_t &unixTime);

    /**
     * @brief tmToTimet - �८�ࠧ������ �������� tm � time_t
     * @param calendarDateTime - ������� tm
     * @param unixTime - �६� �� tm �� 誠�� unix time
     * @return true - � ��砥 �ᯥ�, ���� false
     */
    virtual bool tmToTimet(tm &calendarDateTime, time_t &unixTime);

    /**
     * @brief dateLessSelected - �஢�ઠ ࠧ���� ���� ���
     * ���� � �ଠ� ddMMYY, ddMMYYYY, ddMMYYHHmm, ddMMYYYYHHmm, ddMMYYYYHHmmss
     * @param startDatePoint - ��砫쭠� ��� (�� ��� ������� ࠧ���)
     * @param displacedDatePoint - ����筠� ���
     * @param diff - ���祭�� ࠧ���� ��� � ᥪ㭤��
     * @param realDiff - ॠ�쭮� ���祭�� ࠧ���� ��� � ᥪ㭤��
     * @return true - �᫨ ��砫쭠� ��� ���� ࠭�� ����筮� � ࠧ��� ����� diff, ���� false
     */
    virtual bool dateLessSelected(const string &startDatePoint, const string &displacedDatePoint, const int64_t &diff, int64_t &realDiff);
    /**
     * @brief doubleToUint64ArithmeticRound - ��ॢ�� �� double � uint64_t � ��䬥��᪨� ���㣫�����
     * @param value - �᫮ double
     * @return ���㣫����� �᫮ uint64_t
     */
    static uint64_t doubleToUint64ArithmeticRound(const double &value);

    /**
     * @brief getStringExpireDays - ������� ��ப� � ��⠢訬�� ������⢮� ���� ��
     * @param expireSec - ᥪ㭤� �� ���祭�� ��
     * @return ��ப� ���� "��⠫��� XX �."
     */
    string getStringExpireDays(int64_t &expireSec);
    /**
     * @brief hex2str - hex ���� � ��ப� BIOS
     * @param hex - ����
     * @return ��ப� ᡠ�� � hex
     */
    static std::string hex2str(const BYTE &hex);
    /**
     * @brief hexStrGet - ��ॢ��� �᫮ � hex ��ப� big-endian
     * @param source - ���筨�
     * @param size - ࠧ��� १������饩 ��ப� � ᨬ�����
     * @return hex ��ப� big-endian
     */
    static string hexStrGet(const string &source, const uint64_t &size = 0);
    /**
     * @brief fpdValid - �������� �� 10 ᨬ�����
     * @return �������� ��ப�
     */
    string fpdValid(string fpd);

    /**
     * @brief getStrDate - ��������� timeinfo � std::string
     * @param[in] timeinfo - �६�
     * @return ��ப�
     */
    static string getStrDate(const tm& timeinfo);

    /**
     * @brief checkDayMonth - �஢�ઠ ��������� ��� � �����
     * @param[in] timeinfo - �६�
     * @return true - �᫨ ⠪�� ���� �������� ��� false �᫨ ���
     */
    static bool checkDayMonth(const tm& timeinfo);
    /**
     * @brief dateIsoToTimet - ��������� dateIso � time_t
     * @param[in] dateIso - �६�
     * @return �६�
     */
    static time_t dateIsoToTimet(const DATE_ISO_8601& dateIso);

    /**
     * @brief tmToStr - �८�ࠧ������ �������� tm � ��ப� ����������
     * @param timeinfo
     * @return ��ப� ����������
     */
    static string tmToStr(const struct tm& timeinfo);

    /**
     * @brief Convert::tmToISODate - �८�ࠧ������ �������� tm � DATE_ISO_8601
     * @param dateTime - �६� � tm
     * @return �६� � DATE_ISO_8601
     */
    DATE_ISO_8601 tmToISODate(const tm &dateTime);

    /**
     * @brief getISODate - ������� ⥪���� ISO ����
     * @return ISO "YYYYMMDDTHH:MM:SS.UUUUUU+ZZ:00
     */
    string getISODate();
    /**
     * @brief hex2ASCII ASCII-����� � ��ப�
     * @param src ASCII-�����
     * @param len ������ ������
     * @return १�������� ��ப�
     */
    string hex2ASCII(const char *src, const size_t &len);

    /**
     * @param source ��室��� �᫮ � ���� ��ப� ascii ᨬ�����
     * @param dest - ����� �㤠 ᪫��뢠�� ����� �᫠
     * @param minSize - �������쭠� ����� ������塞�� ���, ��⠫쭮� �������� ��ﬨ
     *  @return �믮�����/�� �믮�����
     */
    static bool appendToVectNum(const string &source, std::vector<BYTE> &dest, const uint64_t &minSize = 0);

    /**
     * @param source - ��室��� ��ப�
     * @param dest - ����� �㤠 ������塞 �����
     */
    static void appendToVectorStr(const std::string &source, std::vector<BYTE> &dest);

    /**
     *  @param inStr �室��� ��ப� ��� �८�ࠧ������ � 奪� ��ப�
     *  @return १�������� ��ப� ��� ����� ᨬ��� �� �� ��ப� �।�⠢��� � 奪� �����
     */
    static std::string strToHexStr(const std::string &inStr);
    /**
     * @brief DateTime_Get ������� ���� � �६� � �ଠ� NVR
     * @return ��� � �६� � �ଠ� NVR
     */
    static CDate DateTime_Get();
//    /**
//     * @brief parseISO8601_2004 ����� ���� �ଠ� YYYYMMDDThhmm
//     * @param date ��� �ଠ� YYYYMMDDThhmm
//     * @return ������� ���� DATE_ISO_8601
//     */
//    virtual DATE_ISO_8601 parseISO8601_2004(const std::string &date);
    /**
     * @brief getFmtPhone ������� �ଠ�஢���� ��䮭�� �����
     * @param phone ��室�� ⥫�䮭�� �����
     * @return �ଠ�஢���� ��䮭�� ����� +7-xxx-***-**-xx (rus)/ x-xxx-***-**-xx (international)
     */
    static std::string getFmtPhone(const std::string &phone);
    /**
     *
     * @param in - ��室�� ����� ����/��ப�/���� ���⥩��� char/BYTE ��� ����஢����. ������ ����� push_back � size
     * @return   - ������஢����� ��ப� � base64
     */
    template<typename T> static std::string toBase64(const T &in);
    /**
     *
     * @param in - �室��� base64 ��ப�
     * @return   - �᪮��஢���� ����� ����/��ப�/���� ���⥩���. ������ ����� push_back
     */
    template<typename T> static T fromBase64(const std::string &in);
    /**
     * @brief isCntrl ��� �� �ࠢ���騩 ᨬ���
     * @param ch ᨬ���
     * @return १���� �믮������
     */
    template<typename T> static inline const bool isCntrl(const T& ch) {return std::iscntrl(static_cast<unsigned char>(ch));}
    /**
     * @brief stripCntrl 㤠����� �ࠢ����� ᨬ�����
     * @param src STL Containers
     * @return STL Containers ��� �ࠢ����� ᨬ�����
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
     * @brief alignCenter ��ࠢ������� ⥪�� �� 業���
     * @param src ��室�� ⥪��
     * @param width �ਭ� ����
     * @return ��ࠢ����� �� 業��� ⥪��
     */
    static std::string alignCenter(const std::string& src, const size_t widthArea);
    /**
     * @brief getRand ������� ��砩��� �ᥫ
     * @return ��砩��� �᫮
     */
    static int getRand();
    /**
     * @brief toUpperCase �८�ࠧ������ � ���孥�஢���� ॣ����
     * @param src ��室�� ⥪�� [��᫥ �८�ࠧ������ ⥪�� � ���孥�஢����� ॣ����]
     * no throw
     */
    static void toUpperCase(std::string& src);
    /**
     * @brief toLowerCase �८�ࠧ������ � ������஢���� ॣ����
     * @param src ��室�� ⥪�� [��᫥ �८�ࠧ������ ⥪�� � ������஢���� ॣ����]
     * no throw
     */
    static void toLowerCase(std::string& src);
    /**
     * @brief complite ���������� ��ப� �� �㦭�� ������
     * @param src ��室�� ⥪��
     * @param sym ᨢ�� ����������
     * @param len �ॡ㥬�� ������
     * @return १�������� ��ப�
     */
    static std::string complite(const std::string &src, const char sym, const size_t len);
    /**
     * @brief hex2char �������� ��ப� ���� � ���ᨢ ���⮢
     * @param hexstr ��ப� ����
     * @param len ������ ���ᨢ� ����
     * @return ���ᨢ ���⮢
     */
    static unsigned char* hex2char(const char* hexstr, uint64_t &len);
    /**
     * @brief insertSymbol 横���᪠� ��⠢�� ᨬ���� � ��ப�
     * @param src ��室��� ��ப�
     * @param symbol ᨬ��� ��� ��⠢��
     * @param interval ���ࢠ� ��⠢��
     * @return १�������� ��ப�
     */
    static std::string insertSymbol(std::string src, const char symbol, const size_t interval);
    /**
     * @brief exec ��⥬�� �맮�
     * @param cmd �������
     * @return १���� �믮������
     * [no throw]
     */
    static std::string exec(const char* cmd);
    /**
     * @brief systemExec ��⥬�� �맮� � ����� ������
     * @param cmd ��⥬��� �������
     * @return ��� ������
     */
    static int systemExec(const std::string &cmd);
    /**
     * @brief double2str �������� double � std::string
     * @param value ���祭�� � �窮�
     * @param prec �筮���
     * @return ��ப���� �।�⠢�����
     */
    static std::string double2str(const long double value, const std::streamsize prec = 2);
    /**
     * @brief isIp �������� ip-����
     * @param src ip-����
     * @return १���� �믮������
     */
    static bool isIp(const std::string &src);
    /**
     * @brief isKpp  �������� ���
     * @param src ���
     * @return १���� �믮������
     */
    static bool isKpp(const std::string &src);
    /**
     * @brief getDateYearsAgo ����� ���� years ��� �����
     * @param years ������⢮ ���
     * @return १���� �믮������
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

