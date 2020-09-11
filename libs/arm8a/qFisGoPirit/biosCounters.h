#ifndef COUNTERS_H_
#define COUNTERS_H_
#include <vector>
#include <stdio.h>

#include "pirit_structs.h"
#include "pirit_low_func.h"

using namespace std;
// ��ꥪ� ��� ��饭�� �� ��⮪��� ����, ��⮤� ���� ��� ��
extern PIRIT_IOCLASS pirit_io;
//========================================================================================
// ����稪�
//========================================================================================
enum CNTRS_TYPE
{
    CNTRS_NONE                  =  0,
    CNTRS_TYPE_CUR_SHIFT        =  1,
    CNTRS_TYPE_NEXT_REC         =  2,
    CNTRS_TYPE_SUM_BY_PAY       =  3,
    CNTRS_TYPE_CNT_BY_PAY       =  4,
    CNTRS_TYPE_SUM_BY_PAY_RET   =  5,
    CNTRS_TYPE_CNT_BY_PAY_RET   =  6,
    CNTRS_TYPE_REC_CNT_BY_TYPE  =  7,
    CNTRS_TYPE_REC_SUM_BY_TYPE  =  8,
    CNTRS_TYPE_DISC_SUMS        =  9,
    CNTRS_TYPE_TAX_SALE_SUMS    = 10,
    CNTRS_TYPE_TAX_RET_SUMS     = 11,
    CNTRS_TYPE_X_Z_DATA         = 12,
    CNTRS_TYPE_DEP_SALE_SUM     = 13,
    CNTRS_TYPE_DEP_SALE_RET_SUM = 14,
    CNTRS_TYPE_REC_COUNT        = 15,
    CNTRS_TYPE_SALE_SUM_BY_TYPE = 16,
    CNTRS_TYPE_RET_SUM_BY_TYPE  = 17,
    CNTRS_TYPE_COR_DATA         = 18
};
//========================================================================================
// ��砫� ����� �� ����� ���� ����稪��
//========================================================================================
inline int countersHeader(CNTRS_TYPE field)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(PIRIT_COUNTERS_AND_REGISTERS);
    pirit_io.addInt( (int)field );
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock();
    if( err != 0 )
    {
        printf("countersHeader():: Error while connecting with Punix!");
        return err;
    }
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    else
    {
        printf("countersHeader()::sendData FAILED!!!");
    }

    pirit_io.disconnectSock();
    return 0;
}
//========================================================================================
/**
 *  ������ ����稪�
 *  �㬬�      -   double
 *  ������⢮ - uint64_t
 */
//========================================================================================
template<class T>
class BiosCounters
{
    public:
        BiosCounters()
        {
            reset();
        }
        ~BiosCounters()
        {
            reset();
        }
    private:
        CNTRS_TYPE countersType = CNTRS_NONE;
        vector<T> countersTable;
        /**
         * �������� ����稪
         * @brief addCountersValue
         * @param value - ���祭�� ����稪�
         */
        inline void addCountersValue(T value){ countersTable.push_back(value); }
    public:
        /**
         * ���㧨�� ⨯ ����稪��
         * @brief getCountersType
         * @return
         */
        inline CNTRS_TYPE getCountersType(){ return countersType; }
        /**
         * @brief getCounter - ���㧨�� ���祭�� ����稪�
         * @param counterNum [0; countersTable.size() - 1]
         * @return
         */
        T getCounter( uint8_t counterNum )
        {
            if(counterNum >= countersTable.size())
            {
                return T(0);
            }
            return countersTable.at(counterNum);
        }
        /**
         * @brief setCountersType - ��⠭����� ⨯ ����稪�
         * @return
         */
        void setCountersType(CNTRS_TYPE type)
        {
            // ���� �� ���������� ⨯� ����稪��
            if((type < CNTRS_NONE) || (type > CNTRS_TYPE_COR_DATA))
            {
                type = CNTRS_NONE;
            }
            countersType = type;
            countersTable.clear();
        }
        /**
         * @brief getCountersTable - ���㧨�� ⠡���� ����稪��
         * @return
         */
        inline vector<T> getCountersTable(){ return countersTable; }
        /**
         * @brief reset - ��� ����稪��
         */
        void reset()
        {
            countersType = CNTRS_NONE;
            countersTable.clear();
        }
        /**
         * @brief piritParamsParse - ���ᨭ� ����稪��
         * @param paramQuantity - ���ᨬ���� ����� ��ࠬ��� ��� ����
         * @return
         */
        int piritParamsParse( PIRIT_PARAM_NUMBER maxNumber )
        {
            int res = -1;
            for(unsigned int valueNumber = PIRIT_PARAM_1; valueNumber <= maxNumber; valueNumber++)
            {
                T value;
                res = pirit_io.parseAnswerN<T>( static_cast<PIRIT_PARAM_NUMBER>(valueNumber), value);
                if(res != 0)
                {
                    return res;
                }
                if(pirit_io.DEBUGLEVEL == DEBUG_ON)
                {
                    string fmt = "Counter value #%u : " + formatGet(typeid(value).hash_code() ) + "\n";
                    printf( fmt.c_str(), valueNumber, value);
                }
                addCountersValue(value);
            }
            return res;
        }
        /**
         * @brief read - ���뢠��� ����稪�� �� bios
         * @return
         */
        int read()
        {
            if(pirit_io.DEBUGLEVEL == DEBUG_ON)
            {
                printf("\t%s\n", __PRETTY_FUNCTION__);
            }
            countersTable.clear();
            int res = countersHeader(getCountersType());
            if( res != 0 )
            {
                return res;
            }
            switch(getCountersType())
            {
                case CNTRS_TYPE_SUM_BY_PAY:
                case CNTRS_TYPE_CNT_BY_PAY:
                case CNTRS_TYPE_SUM_BY_PAY_RET:
                case CNTRS_TYPE_CNT_BY_PAY_RET:
                {
                    res = piritParamsParse( PIRIT_PARAM_16 );
                    break;
                }
                case CNTRS_TYPE_REC_CNT_BY_TYPE:
                {
                    res = piritParamsParse( PIRIT_PARAM_6 );
                    break;
                }
                case CNTRS_TYPE_REC_SUM_BY_TYPE:
                case CNTRS_TYPE_DISC_SUMS:
                {
                    res = piritParamsParse( PIRIT_PARAM_4 );
                    break;
                }
                default:
                {
                    res = -1;
                    break;
                }

            }
            return res;
        }
        /**
         * @brief formatGet - RTTI ��।������
         *        �ଠ� �뢮�� ���祭��
         *        ����稪�
         * @param typeHashCode - hash ���
         *        ⨯� ������ ����稪�
         * @return
         */
        string formatGet(size_t typeHashCode)
        {
            // RTTI
            if(typeHashCode == typeid(double).hash_code())
            {
                return "%f";
            }
            else if(typeHashCode== typeid(uint64_t).hash_code())
            {
                return "%llu";
            }
            else
            {
                return "";
            }
        }
};

//------------------------------------------------------------------
//                        �㬬� ����権
//------------------------------------------------------------------
template< class T >
class OPERATION
{
    public:
    OPERATION()
    {
        cash     = 0;
        card     = 0;
        advance  = 0;
        credit   = 0;
        exchange = 0;
    };

    T         cash, // �����묨
              card, // ���஭�묨
           advance, // �����
            credit, // �।��
          exchange; // ����筮� �।�⠢�����
};
//------------------------------------------------------------------
//                 ������⢮ ��ଫ����� 祪��
//------------------------------------------------------------------
struct REC_CNTS
{
    REC_CNTS()
    {
        sale      = 0;
        ret       = 0;
        canceled  = 0;
        deferred  = 0;
        insert    = 0;
        reserve   = 0;

    };
    uint64_t     sale, // ������⢮ 祪�� �த���
                  ret, // ������⢮ 祪�� ������
             canceled, // ������⢮ ���㫨஢����� 祪��
             deferred, // ������⢮ �⫮������ 祪��
               insert, // ������⢮ 祪�� ���ᥭ��
              reserve; // ������⢮ 祪�� ������
};
//------------------------------------------------------------------
//                 �㬬� �� ��ଫ���� 祪��
//------------------------------------------------------------------
struct REC_SUMS
{
    REC_SUMS()
    {
        canceled  = 0;
        deferred  = 0;
        insert    = 0;
        reserve   = 0;

    };
    double  canceled, // �㬬� �� ���㫨஢���� 祪��
            deferred, // �㬬� �� �⫮����� 祪��
              insert, // �㬬� �� 祪�� ���ᥭ��
             reserve; // �㬬� �� 祪�� ������
};
//------------------------------------------------------------------
//                    �㬬� �� ᪨����
//------------------------------------------------------------------
struct DISCOUNT_SUMS
{
    DISCOUNT_SUMS()
    {
        sale  = 0;
        ret  = 0;
    };
    double     sale, // �㬬� ᪨��� �� �த����
                ret; // �㬬� ᪨��� �� �����⠬
};
//------------------------------------------------------------------
//                     �㬬� �������
//------------------------------------------------------------------
struct TAX_SUMS
{
    TAX_SUMS()
    {
        nds_18       = 0;
        nds_10       = 0;
        nds_0        = 0;
        without_nds  = 0;
        nds_18_118   = 0;
        nds_10_110   = 0;
    };
    double      nds_18,
                nds_10,
                 nds_0,
           without_nds,
            nds_18_118,
            nds_10_110;
};

//------------------------------------------------------------------
// ����� �� ��᫥����� X ����� ��� ����� � �����⨨ ᬥ��
//------------------------------------------------------------------
struct X_Z_DATA
{
    X_Z_DATA()
    {
        operation_cnt.clear();
        doc_num        = 0;
        cash_in_drawer = 0;
        sale_cnt       = 0;
        sale_sum       = 0;
        ret_cnt        = 0;
        ret_sum        = 0;
        canceled_cnt   = 0;
        canceled_sum   = 0;
        insert_cnt     = 0;
        insert_sum     = 0;
        reserve_cnt    = 0;
        reserve_sum    = 0;
    };

    string    operation_cnt; // ⥪�騩 ����樮��� ���稪
    uint64_t        doc_num; // ����� ���㬥��
    double   cash_in_drawer; // �㬬� � ����
    uint64_t       sale_cnt; // ���-�� �த�� (���-�� ��室��)
    double         sale_sum; // �㬬� �த�� (�㬬� ��室��)
    uint64_t        ret_cnt; // ���-�� �����⮢ (�����⮢ ��室�)
    double          ret_sum; // �㬬� �����⮢ (�����⮢ ��室�)
    uint64_t   canceled_cnt; // ������⢮ ���㫨஢�����
    double     canceled_sum; // �㬬� ���㫨஢�����
    uint64_t     insert_cnt; // ������⢮ ���ᥭ��
    double       insert_sum; // �㬬� ���ᥭ��
    uint64_t    reserve_cnt; // �����⢮ ����⨩
    double      reserve_sum; // �㬬� ����⨩
};
//------------------------------------------------------------------
// ������ ����� �� ���४��
//------------------------------------------------------------------
struct COR_DATA
{
    COR_DATA()
    {
        cnt  = 0;
        cash = 0;
        card = 0;
    }
    uint64_t  cnt; // ������⢮ ���४権
    double   cash; // �㬬� ���४権  �����묨
    double   card; // �㬬� ���४権  ��������묨
};
//------------------------------------------------------------------
struct COUNTERS_RF
{
    uint64_t                     shift_num; // ����� ⥪�饩 ᬥ��
    uint64_t                  next_rec_num; // ����� ᫥���饣� 祪�
    OPERATION <double>           sale_sums; // �㬬� �த�� �� ⨯�� ���⥦�
    OPERATION <uint64_t>         sale_cnts; // ������⢮ ����� �� �த����
    OPERATION <double>       ret_sale_sums; // �㬬� �����⮢ ��室� �� ⨯�� ���⥦�
    OPERATION <uint64_t>     ret_sale_cnts; // ������⢮ ����� �� �����⠬ ��室�
    REC_CNTS                      rec_cnts; // ������⢮ ��ଫ����� 祪�� �� ⨯�� ����権
    REC_SUMS                      rec_sums; // �㬬� �� ��ଫ���� 祪��
    DISCOUNT_SUMS            discount_sums; // �㬬� �� ᪨����
    TAX_SUMS                     sale_taxs; // �㬬� ������� �� �த����
    TAX_SUMS                      ret_taxs; // �㬬� ������� �� �����⠬
    X_Z_DATA                      x_z_data; // ����� �� ��᫥����� X ����� ��� ����� � �����⨨ ᬥ��
    double   dep_sale_sum[NUM_OF_SECTOINS]; // �㬬� �� ᥪ��/�⤥��� �� �த����
    double    dep_ret_sum[NUM_OF_SECTOINS]; // �㬬� �� ᥪ��/�⤥��� �� �����⠬
    uint64_t                  purchase_cnt; // ���-�� 祪�� ��室��          0x15
    uint64_t              ret_purchase_cnt; // ���-�� 祪�� �����⮢ ��室� 0x15
    OPERATION <double>       purchase_sums; // �㬬� ��室�� �� ⨯�� ���⥦� (�� ���ᨬ��쭮 ���������� �������� ⨯�� ���⥦��)
    OPERATION <double> ret_purchase_sums; // �㬬� �����⮢ ��室�  �� ⨯�� ���⥦� (�� ���ᨬ��쭮 ���������� �������� ⨯�� ���⥦��)
    COR_DATA                      cor_data; // ����� �� ���४��
};

int libGetCounters_CurShiftNum     ( uint64_t               &shiftNum );  // 0x01 ����� ⥪ ᬥ��
int libGetCounters_NextRecNum      ( uint64_t             &nextRecNum );  // 0x02 ������ ����� ᫥���饣� 祪�
int libGetCounters_SaleSumByPay    ( OPERATION<double>          &ssbp );  // 0x03 ������ �㬬� �த�� �� ⨯�� ���⥦�
int libGetCounters_SaleCntByPay    ( OPERATION<uint64_t>        &scbp );  // 0x04 ������ ������⢮ ����� �� �த����
int libGetCounters_RetSaleSumByPay ( OPERATION<double>         &rssbp );  // 0x05 ������ �㬬� �����⮢ �� ⨯�� ���⥦�
int libGetCounters_RetSaleCntByPay ( OPERATION<uint64_t>       &rscbp );  // 0x06 ������ ������⢮ ����� �� �����⠬
int libGetCounters_RecCntsByType   ( REC_CNTS                &recCnts );  // 0x07 ������⢮ ��ଫ����� 祪��
int libGetCounters_RecSumsByType   ( REC_SUMS                &recSums );  // 0x08 �㬬� �� ��ଫ���� 祪��
int libGetCounters_DiscountSums    ( DISCOUNT_SUMS          &discSums );  // 0x09 �㬬� �� ᪨����
int libGetCounters_TaxSumsSale     ( TAX_SUMS                &taxSums );  // 0x10 �㬬� �������  �� �த����
int libGetCounters_TaxSumsRets     ( TAX_SUMS                &taxSums );  // 0x11 �㬬� �������  �� �����⠬
int libGetCounters_XZ              ( X_Z_DATA                 &xzData );  // 0x12 ����� �� ��᫥����� X ����� ��� ����� � �����⨨ ᬥ��
int libGetCounters_depSaleSum      ( double         (&depSaleSum)[NUM_OF_SECTOINS] );  // 0x13 ������ �㬬� �� ᥪ��/�⤥��� �� �த����
int libGetCounters_depRetSum       ( double          (&depRetSum)[NUM_OF_SECTOINS] );  // 0x14 ������ �㬬� �� ᥪ��/�⤥��� �� �����⠬
int libGetCounters_RecCount        ( uint64_t            &purchaseCnt ,   // 0x15 ���-�� 祪�� ��室��
                                     uint64_t         &retPurchaseCnt );  // 0x15 ���-�� 祪�� �����⮢ ��室�
int libGetCounters_SaleSumByType   ( OPERATION<double>     &purchSums );  // 0x16 ������ �㬬� ���㯮� �� ⨯�� ���⥦�
int libGetCounters_RetSumByType    ( OPERATION<double>  &purchSumsRet );  // 0x17 ������ �㬬� �����⮢ ���㯮� �� ⨯�� ���⥦�
int libGetCounters_CorData         ( COR_DATA                &corData );  // 0x18 ������ ����� �� ���४��

#endif
