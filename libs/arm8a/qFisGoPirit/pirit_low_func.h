#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <vector>
#include <sstream>
#include <typeinfo>
#include <typeindex>

#include "pirit_structs.h"

enum DeviceMode { COM_MODE = 0, TCP_MODE };
enum PacketType { SEND = 0, RECIEVE = 1 };          // ��� ����� �� ��ࠢ��/�� ���
enum Country    { RF = 0, RB = 1 };                 // ��࠭�, � ���ன �㤥� ࠡ���� ����
enum DEBUG_LEVEL { DEBUG_OFF = 0, DEBUG_ON = 1 };   // ���� �⫠��筮�� �뢮��

/**
 * @brief The PACKET_MODE enum - �������� ⨯� �ନ஢���� ������
 */
enum PACKET_MODE
{
    PACKET_OFF = 0, // ������ ०�� OFF
    PACKET_ON  = 1  // ������ ०�� ON
};

const unsigned char STX = 0x02;                             // ��砫� �����
const unsigned char ETX = 0x03;                             // ����� �����
const unsigned char password_RF[4] = { 'P', 'I', 'R', 'I' };   // ���प�থ�� ��஫� � ����� ��� ������
const unsigned char password_RB[4] = { 'D', 'R', 'R', 'B' };   // ���प�থ�� ��஫� � ����� ��� ���������

const unsigned char START_ID = 0x20;                        // ���⮢� ID � �����


class CheckBuffer
{
private:
    bool accumulate = false;
    string commands;
public:
    inline bool getAccumulate()           { return accumulate;}
    inline void clear()                   { commands.clear();accumulate = false;}
    inline void setAccumulate(bool isAcc) { accumulate = isAcc;}
    void addCommand(string& command)      { commands+=command;}
    bool empty()                          {return commands.empty();}
    string getAllCommands()
    {
        string retStr = commands;
        commands.clear();
        setAccumulate(false);
        return retStr;
    }
};


class PIRIT_IOCLASS
{
private:

    // �����
    int err;
    int sd_out;
    struct sockaddr_in dest_out;
    std::string ip;
    int       port;

    // �����
    DataToSend       dataToSend;      // ����� �� ��ࠢ�� (STX	��஫� �裡	ID �����	��� �������	�����	ETX	    CRC)
    DataToSend       dataToRead;      // �ਭ��� �����    (STX	ID �����	    ��� �������	��� �訡��	�����	ETX	    CRC)
    DataToSend           answer;
    int     AmountDecimalPlaces;
    int   QuantityDecimalPlaces;
    int PercentageDecimalPlaces;
    unsigned char      packetID;
    unsigned char    packetMode;

    /**
     * @brief dataToReadBinary - ���⥩��� ��� �࠭���� �⢥��� ������ � ����୮� ����
     */
    vector<uint8_t> dataToReadBinary;

    void printPacket(int n, PacketType type);  // �뢮� ��ࠢ�塞�� � ����砥��� ������ � ���᮫� � ०��� �⫠���

    void clearData(PacketType type);
    long pow10(long power);
    void setPackedID(unsigned char newID);
    unsigned char incPackedID();
    unsigned char getPackedID();
    int readAck();

    template <typename TYPE> bool strToVal(TYPE &val, char *tmpBuf);     // ��ॢ�� ��ப� � ��६����� �� ⨯�


public:
    PIRIT_IOCLASS();

    CheckBuffer checkBuffer;

    DEBUG_LEVEL DEBUGLEVEL;     // 0 - �� �뢮��� �����, 1 - �뢮��� � ���᮫�

    // ����⨯�� ��砫� � ����� ����� �� ��ࠢ��
    int makeFirstPartPacketToSend(unsigned cmd);
    int makeEndPartPacket();

    // ����� � ����� �� ��ࠢ��
    void addChar(char* str, size_t size = 0);
    /**
     * @brief addString �������� ��ப� � �����
     * @param text ⥪��
     * @param limit �।�� ������⢠ ᨬ�����
     */
    void addString(std::string text, const size_t &limit = 0);
    void addByte(unsigned char byte);
    void addEmptyByte();
    void addInt(int twoByte);
    void addLong(long fourByte);
    void addDouble(double data);
    void addDoubleFromLong(long data, int numFormat);
    void addDoubleFromInt64(int64_t data, int numFormat);
    void addConstChar(const char* str);

    // �६� � ����� �� ��ࠢ��
    void addDate(PLDate piritDate);
    void addTime(PLTime piritTime);
    void addDateTime(int day, int month, int year, int hour,int min, int sec);
    void addDateTime(PLDateTime dt);

    // ��ନ஢���� ������ ���� �� �室��� ��ࠬ��஢
    int commandVoid                       (int command);
    MData commandVoidMData                (int command);
    MData commandByteEx                   (int command);
    MData commandByte(int command, unsigned char dByte);
    int commandVoidInt         (int command, int *data);

    // ���ᨭ� �⢥�
    int parseAnswer();                   // ࠧ��� ����� �� ��� �訡�� � ����

    /**
     * @brief getBinryData - ������� �⢥� ������� ������
     * @return ���⥩��� � �����묨 ����묨
     */
    const vector<uint8_t>& getBinryData();
    //------------------
    // ���䮢� ���� �����
    template <typename TYPE> int parseAnswerN(uint32_t n, TYPE &val, int *length = NULL); // ��楦����� �� ��ப� answer ���祭�� int, double, char*, int32 ��� int64,   ���浪��� ����� N � �����, ������ �� 0
    int parseAnswerChN_constCount  ( int number, char **val, int &valLength );       // ��楦����� �� ��ப� answer ���ᨢ char val[valLength], ���浪��� ����� N � �����, ������ �� 0
    //------------------
    // ����� ���� �����
    int parseAnswer1Int    (int  *data); // ��楦����� �� ��ப� answer ���祭�� INT � data, ����� ��� ASCII   (��ࢮ� ���祭��)
    int parseAnswer1Char   (char *data); // ��楦����� �� ��ப� answer ���祭�� char � data, ����� ��� ASCII  (��ࢮ� ���祭��)     ������ �������� ������� !!!
    int parseAnswer1CharEx (char *data); // ��楦����� �� ��ப� answer ���祭�� char* � data, ����� ��� ASCII (��ࢮ� ���祭��)     ������ �������� �������
    int parseAnswer2Char   (char *data); // ��楦����� �� ��ப� answer ���祭�� char* � data, ����� ��� ASCII (���� 2 ���祭��)   ������ �������� �������
    int parseAnswerChar(int count, int num, char *data); // ���� ��ப� �஢, ��६����� ����� "num"[0..n-1] �� "count" ��६�����
    int parseAnswerIntXd(int intData[], int maxElement, int type);
    int parseAnswerIntXdEx(int intData[], int maxElement, int type);
    int parseAnswer1Int2Int3Int(int *i1, int *i2, int *i3);
    int parseAnswerCommand0x00(int *fatalStatus, int *currentFlagsStatus, int *documentStatus);        // ����� 䫠��� ����� ��� (0x00)
    int parseAnswerCommand0x00_4args(int *fatalStatus, int *currentFlagsStatus, int *documentStatus, int *sknoStatus);  // ����� 䫠��� ����� ��� (0x00) (4 ��㬥��, �� �� ���筮)
    int parseAnswerCommand0x01_12(int intData[], int maxElement, char *str);

    //------------------
    // ����ன��
    void setIP_io(std::string   newIP)  { ip = newIP;    if(DEBUGLEVEL == DEBUG_ON) printf("\n\n\tPIRIT_IO:: SetIP:: ip = '%s'\n", ip.c_str() ); }
    void setPort_io(int  newPort)  { port = newPort; }
    void setDebugLevel(DEBUG_LEVEL level);
    void setPacketMode(unsigned char newPacketMode);
    void setPasswd(Country newCountry);
    unsigned char getPacketMode();

    //------------------
    // ����
    int connectSock();
    int disconnectSock();
    int sendData();
    int readData();
    int readBinaryData();
    void getMData(MData *mData);
    int scrollPaper();
    int loadDataToPirit(int size, unsigned char* data);
    int loadPictureToPirit(int width, int height, int sm, unsigned char* data);

    void setAmountDecimalPlaces(int decimalPlaces);
    void setQuantityDecimalPlaces(int decimalPlaces);
    void setPercentageDecimalPlaces(int decimalPlaces);
    int getAmountDecimalPlaces();
    int getQuantityDecimalPlaces();
    int getPercentageDecimalPlaces();
};




//======================================================================================
//
template <typename TYPE> bool PIRIT_IOCLASS::strToVal(TYPE &val, char *tmpBuf)
{
   if(typeid(TYPE) == typeid(int))
    {
        val = atoi(tmpBuf);
    }
    else if( typeid(TYPE) == typeid(uint32_t) || typeid(TYPE) == typeid(unsigned int) )
    {
        val = strtoul(tmpBuf, NULL, 10);
    }
    else if( typeid(TYPE) == typeid(uint64_t)  || typeid(TYPE) == typeid(long long unsigned int) )
    {
        val = strtoull(tmpBuf, NULL, 10);
    }
    else if( typeid(TYPE) == typeid(double) )
    {
        val =  strtod(tmpBuf, NULL);
    }
    else if( typeid(TYPE) == typeid(float) )
    {
        val = strtof(tmpBuf, NULL);
    }
    else if( typeid(TYPE) == typeid(char) )
    {
        strcpy((char*)&val, tmpBuf);
    }
    else
    {
        printf("<typename TYPE> strToVal:: Unknown type!");
        return false;
    }
    return true;
}


//======================================================================================
// ��竨����� �� ����� ���� ���浪��� ����� N, ������ �� 0,  ⨯� ���, �32, �64, ����, 䫮�, ��*
template <typename TYPE> int PIRIT_IOCLASS::parseAnswerN( uint32_t n, TYPE &val,int *length)
{
    int i, len;
    uint32_t counter = 0;

    const char BUF_LEN = 128;
    char tmpBuf[BUF_LEN] = {0};

    len = 0;
    for(i = 0; i < answer.length; i++)
    {
        if(answer.data[i] == 0x1C)
        {
            if(counter == n)
            {
                if( !strToVal<TYPE>(val, tmpBuf) )
                {

                    return 1989;
                }
                if (length != NULL )  // ���࠭塞 �����, �᫨ ���� 㪠��⥫�
                {
                    *length = len;
                }
            }
            memset(tmpBuf, 0, sizeof(tmpBuf));
            counter ++;
            len = 0;
        }
        else
        {
            tmpBuf[len] = answer.data[i];
            len++;
        }
    }
    // �᫨ ��᫥���� ���� �� ����� � ��稭� �� ����� �ਧ���� ����砭�� 1�, � ��� ⮦� ��襬
    if ( len > 0 )
    {
        if(counter == n)
        {
            if( !strToVal<TYPE>(val, tmpBuf) )
            {
                return 1989;
            }
            if (length != NULL )  // ���࠭塞 �����, �᫨ ���� 㪠��⥫�
            {
                *length = len;
            }
        }
        memset(tmpBuf, 0, sizeof(tmpBuf));
        counter ++;
        len = 0;
    }
    if(counter >= (n+1) )
    {
        return 0;
    }
    printf("%s:: Error! Num of fields in packet = %d, try get PIRIT_PARAM_%d \n",__func__, counter, n);
    return 2222;
}
