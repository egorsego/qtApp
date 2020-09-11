//
// Created by krazh on 16.08.17.
//

#ifndef FISCAT_DYNAMIC_MASK_H
#define FISCAT_DYNAMIC_MASK_H

#include <string.h>

#define MASK_MAX_LEN 120

#define USLEEP_WHILE_1  1000     // ��㧠 ����� �����뢭묨 �஢�ઠ�� ��ண���� ��ଥ����

// ����ﭨ� ��� �祡��, ����, �᪠���(�� ���ﭨ� �᪠��� ��� �����᪠���)
enum KKT_STAGE
{
    STUDY  = 0,
    ENVD   = 1,
    FISCAL = 2,
};
// ����ﭨ� ��� �祡��, ����, �᪠���(�� ���ﭨ� �᪠��� ��� �����᪠���)
enum MODE
{
    MODE_OFF  = 0,
    MODE_ON   = 1
};
struct DYNAMIC_MASK
{
    unsigned short size;
    bool *mask;
    //--------------------------------------
    DYNAMIC_MASK (int newSize = 1 , int index = 0 )
    {
        mask = new bool [newSize];
        size = newSize;
        index == 1111 ?  memset( mask, true, newSize ) : memset( mask, false, newSize );
        ((index < 0) || (index >= newSize))  ? 1==1 : mask[index] = true;
    }
    //--------------------------------------
    DYNAMIC_MASK (const DYNAMIC_MASK &maskRight )
    {
        mask = new bool[maskRight.size];
        size = maskRight.size;

        for (int i = 0; i < size; i++)
        {
            mask[i] = maskRight.mask[i];
        }
    }
    //--------------------------------------
    ~DYNAMIC_MASK ()
    {
        delete [] mask;
    }
    //--------------------------------------
    DYNAMIC_MASK&       operator  = (const DYNAMIC_MASK &maskRight);
    bool                operator && (const DYNAMIC_MASK &maskRight);
    bool                operator == (const DYNAMIC_MASK &maskRight);
    const DYNAMIC_MASK  operator  & (const DYNAMIC_MASK &maskRight);
    const DYNAMIC_MASK  operator  | (const DYNAMIC_MASK &maskRight);
    const DYNAMIC_MASK  operator  ~ ();
    //--------------------------------------
};
// ����⠭�� ��᮪ �������� �ਧ�����
static DYNAMIC_MASK
        CONFIG_VER              (MASK_MAX_LEN, 0),
        KKT_MODE                (MASK_MAX_LEN, 1),
        TERMINAL_MODE           (MASK_MAX_LEN, 2),
        FS_REPLACE_MODE         (MASK_MAX_LEN, 3),
        SHIFT_TIMER             (MASK_MAX_LEN, 4),
        FISGO_VERSION           (MASK_MAX_LEN, 5),
        KKT_REG_VERSION         (MASK_MAX_LEN, 6),
        ARTICLE                 (MASK_MAX_LEN, 7),
        UUID                    (MASK_MAX_LEN, 8),
        ETH_UP                  (MASK_MAX_LEN, 9),
        IP_ADDRESS              (MASK_MAX_LEN, 10),
        MASK                    (MASK_MAX_LEN, 11),
        ROUTER                  (MASK_MAX_LEN, 12),
        WIFI_UP                 (MASK_MAX_LEN, 13),
        NET_WIFI_NAME           (MASK_MAX_LEN, 14),
        NET_WIFI_KEY            (MASK_MAX_LEN, 15),
        NET_WIFI_SIGNAL         (MASK_MAX_LEN, 16),
        NET_PASS                (MASK_MAX_LEN, 17),
        FS_NUMBER               (MASK_MAX_LEN, 18),
        FS_NUMBER_COUNT         (MASK_MAX_LEN, 19),
        FS_NUMBERS_TABLE        (MASK_MAX_LEN, 20),
        CASHIER_INN             (MASK_MAX_LEN, 21),
        ORGANIZATION_NAME       (MASK_MAX_LEN, 22),
        CALCULATION_ADDRESS     (MASK_MAX_LEN, 23),
        CALCULATION_PLACE       (MASK_MAX_LEN, 24),
        ORGANIZATION_INN        (MASK_MAX_LEN, 25),
        ORGANIZATION_KPP        (MASK_MAX_LEN, 26),
        KKT_REG_NUM             (MASK_MAX_LEN, 27),
        KKT_PLANT_NUM           (MASK_MAX_LEN, 28),
        TAX_SYSTEMS             (MASK_MAX_LEN, 29),
        CUR_TAX_SYSTEM          (MASK_MAX_LEN, 30),
        ENCRYPTION_SIGN         (MASK_MAX_LEN, 31),
        EXCISABLE_SIGN          (MASK_MAX_LEN, 32),
        CLC_SERVICE_SIGN        (MASK_MAX_LEN, 33),
        GAMBLING_SIGN           (MASK_MAX_LEN, 34),
        LOTTERY_SIGN            (MASK_MAX_LEN, 35),
        PAYING_AGENT_SIGN       (MASK_MAX_LEN, 36),
        OFD_CHOOSE              (MASK_MAX_LEN, 37),
        OFD_INN                 (MASK_MAX_LEN, 38),
        OFD_SERVER_ADDRESS      (MASK_MAX_LEN, 39),
        OFD_NAME                (MASK_MAX_LEN, 40),
        OFD_SERVER_PORT         (MASK_MAX_LEN, 41),
        CHECK_RECEIPT_ADDRESS   (MASK_MAX_LEN, 42),
        OFD_SERVER_IP           (MASK_MAX_LEN, 43),
        OPEN_SHIFT_DATE         (MASK_MAX_LEN, 44),
        AGENT_MASK              (MASK_MAX_LEN, 45),
        CURRENT_AGENT           (MASK_MAX_LEN, 46),
        LOG_TYPE                (MASK_MAX_LEN, 47),
        IS_CABINET_ENABLE       (MASK_MAX_LEN, 48),
        START_TIME              (MASK_MAX_LEN, 49),
        LAST_MSG_OFD            (MASK_MAX_LEN, 50),
        NDS_FREE_PRICE          (MASK_MAX_LEN, 51),
        SCALE_TYPE              (MASK_MAX_LEN, 52),
        SCALE_SPEED             (MASK_MAX_LEN, 53),
        SCALE_PORT_NAME         (MASK_MAX_LEN, 54),
        SCALE_OPTIONS           (MASK_MAX_LEN, 55),
        SCANNER_TYPE            (MASK_MAX_LEN, 56),
        SCANNER_SPEED           (MASK_MAX_LEN, 57),
        SCANNER_PORT_NAME       (MASK_MAX_LEN, 58),
        SCANNER_OPTIONS         (MASK_MAX_LEN, 59),
        DNS_MAIN                (MASK_MAX_LEN, 60),
        DNS_ADD                 (MASK_MAX_LEN, 61),
        ETH_MANUAL              (MASK_MAX_LEN, 62),
        AUTO_CASH_OUT           (MASK_MAX_LEN, 63),
        FFD_KKT_VER             (MASK_MAX_LEN, 64),
        BSO                     (MASK_MAX_LEN, 65),
        KKT_SIGNS               (MASK_MAX_LEN, 66),
        ADD_KKT_SIGNS           (MASK_MAX_LEN, 67),
        AUTO_PRINTER_NUM        (MASK_MAX_LEN, 68),
        AUTO_OPEN_SHIFT         (MASK_MAX_LEN, 69),
        STAGE                   (MASK_MAX_LEN, 70),
        INTERNET_RECIEPT        (MASK_MAX_LEN, 71),
        CONNECT_TO              (MASK_MAX_LEN, 72),
        CLOUD_REG_STATUS        (MASK_MAX_LEN, 73),
        CLOUD_TEL               (MASK_MAX_LEN, 74),
        CLOUD_PIN               (MASK_MAX_LEN, 75),
        CLOUD_ORG_FNS_NUM       (MASK_MAX_LEN, 76),
        CLOUD_KKT_FNS_NUM       (MASK_MAX_LEN, 77),
        CABINET_REG_EMAIL       (MASK_MAX_LEN, 78),
        CUR_DEPARTMENT          (MASK_MAX_LEN, 79),
        REC_SENDER_EMAIL        (MASK_MAX_LEN, 80),
        INC_POS                 (MASK_MAX_LEN, 81),
        SOUND_BUTTON            (MASK_MAX_LEN, 82),
        CURRENT_ROUTE           (MASK_MAX_LEN, 83),
        CLOUD_GOODS             (MASK_MAX_LEN, 84),
        IS_2G_UP                (MASK_MAX_LEN, 85),
        IMAGE_SHIFT             (MASK_MAX_LEN, 86),
        LAST_FD                 (MASK_MAX_LEN, 87),
        TARE_DEFAULT            (MASK_MAX_LEN, 88),
        IS_TARE_USE             (MASK_MAX_LEN, 89),
        FREE_PRICE_SALE         (MASK_MAX_LEN, 90),
        FREE_PRICE_SALE_MAX_SUM (MASK_MAX_LEN, 91),
        BT_STATE                (MASK_MAX_LEN, 92),
        BT_PAIR                 (MASK_MAX_LEN, 93),
        REVERT_VER              (MASK_MAX_LEN, 94),
        SALE_ALCOHOL            (MASK_MAX_LEN, 95),
        UTM_ADDRESS             (MASK_MAX_LEN, 96),
        UTM_PORT                (MASK_MAX_LEN, 97),
        UTM_TIMEOUT             (MASK_MAX_LEN, 98),
        PRINT_EGAIS             (MASK_MAX_LEN, 99),
        CHECK_ALC_SALE_TIME     (MASK_MAX_LEN, 100),
        ALC_SALE_HR_START       (MASK_MAX_LEN, 101),
        ALC_SALE_HR_FINISH      (MASK_MAX_LEN, 102),
        ENTER_WEIGHT            (MASK_MAX_LEN, 103),
        TOBACCO_COST_FROM       (MASK_MAX_LEN, 104),

        ALL_FIELDS              (MASK_MAX_LEN, 1111);       // 1111 - ���⠢��� �� ���� = 1
        //================================================================
        // ��᪨ ��� ����㯠 � ����稪��
        //================================================================
 static DYNAMIC_MASK
        UNLOAD                       (MASK_MAX_LEN, 0),
        DOWNLOAD                     (MASK_MAX_LEN, 1),
        // ������
        // �� ��砫� ᬥ��
        CASH_ON_TOP                  (MASK_MAX_LEN, 0),
        // ��室
        ADVENT                       (MASK_MAX_LEN, 1),
        ADVENT_CNT                   (MASK_MAX_LEN, 2),
        // ���室
        CONSUMPTION                  (MASK_MAX_LEN, 3),
        CONSUMPTION_CNT              (MASK_MAX_LEN, 4),
        // ������ ��室�
        ADVENT_RETURN                (MASK_MAX_LEN, 5),
        ADVENT_RETURN_CNT            (MASK_MAX_LEN, 6),
        // ������ ��室�
        CONSUMPTION_RETURN           (MASK_MAX_LEN, 7),
        CONSUMPTION_RETURN_CNT       (MASK_MAX_LEN, 8),
        // ���ᥭ��
        INSERTION                    (MASK_MAX_LEN, 9),
        // ����⨥
        RESERVE                      (MASK_MAX_LEN, 10),
        // �� ����� ᬥ��(�㬬� � �������� �騪�)
        CASH_IN_FINAL                (MASK_MAX_LEN, 11),
        // ������᪠� ����
        // ��室
        ADVENT_CARD                  (MASK_MAX_LEN, 12),
        ADVENT_CARD_CNT              (MASK_MAX_LEN, 13),
        // ������ ��室�
        ADVENT_RETURN_CARD           (MASK_MAX_LEN, 14),
        ADVENT_RETURN_CARD_CNT       (MASK_MAX_LEN, 15),
        // �⮣� ��室�
        ADVENT_TOTAL                 (MASK_MAX_LEN, 16),
        // �⮣� ��室�
        CONSUMPTION_TOTAL            (MASK_MAX_LEN, 17),
        // �⮣� ������ ��室�
        ADVENT_RETURN_TOTAL          (MASK_MAX_LEN, 18),
        // �⮣� ������ ��室�
        CONSUMPTION_RETURN_TOTAL     (MASK_MAX_LEN, 19),
        // �⮣� ॠ������
        REALIZATION_TOTAL            (MASK_MAX_LEN, 20),
        // ���浪��� ����� �����
        REPORT_CNT                   (MASK_MAX_LEN, 21),
        // �����묨
        CASH                         (MASK_MAX_LEN, 22),
        // �����஭�묨
        CARD                         (MASK_MAX_LEN, 23),
        // ��᫮ ����権 �����묨
        CASH_CNT                     (MASK_MAX_LEN, 24),
        // ��᫮ ����権 ���஭�묨
        CARD_CNT                     (MASK_MAX_LEN, 25),
        // ������⢮ ���ᥭ��
        INSERTION_CNT                (MASK_MAX_LEN, 26),
        // ������⢮ ����⨩
        RESERVE_CNT                  (MASK_MAX_LEN, 27),
        // ���室 ���⮩
        CONSUMPTION_CARD             (MASK_MAX_LEN, 28),
        CONSUMPTION_CARD_CNT         (MASK_MAX_LEN, 29),
        // ���室 ���⮩
        RET_CONSUMPTION_CARD         (MASK_MAX_LEN, 30),
        RET_CONSUMPTION_CARD_CNT     (MASK_MAX_LEN, 31),
        //------------------------------------------
        // ������塞� (��᮫���) ����稪�
        //------------------------------------------
        // �⮣� ��室� (���.)
        ADVENT_TOTAL_ABS             (MASK_MAX_LEN, 32),
        // �⮣� ��室� (���.)
        CONSUMPTION_TOTAL_ABS        (MASK_MAX_LEN, 33),
        // �⮣� ������ ��室� (���.)
        ADVENT_RETURN_TOTAL_ABS      (MASK_MAX_LEN, 34),
        // �⮣� ������ ��室� (���.)
        CONSUMPTION_RETURN_TOTAL_ABS (MASK_MAX_LEN, 35),
        // �⮣� ॠ������ (���.)
        REALIZATION_TOTAL_ABS        (MASK_MAX_LEN, 36),
        //------------------------------------------
        // ����
        //------------------------------------------
        CURR_RECEIPT_NUM             (MASK_MAX_LEN, 37),
        CURR_SHIFT_NUM               (MASK_MAX_LEN, 38),
        ALL                          (MASK_MAX_LEN, 1111);

//=============================================================================
// ������⭮� ��ᢠ������, �᫨ a[5] = b[10], � a.size = 10
 inline DYNAMIC_MASK& DYNAMIC_MASK::operator=(const DYNAMIC_MASK &maskRight)
 {
     if ( this != &maskRight )
     {
         // �᫨ ࠧ��� �ࠢ��� ����� ࠧ��� ������, � 㢥��稢��� ����� �� ࠧ��� �ࠢ���
         if(size < maskRight.size)
         {
             delete[] mask;
             mask = new bool[maskRight.size];
             size = maskRight.size;
         }

         for (int i = 0; i < size; i++)
         {
             mask[i] = maskRight.mask[i];
         }
     }

    return *this;
}

//=============================================================================
// �᫨ ��� ���� �� ���ᨢ�� ��������� ���� ���������⢮� ��ண�, � true. 1001 && 0001 = true; 1001 && 0011 = false
inline bool DYNAMIC_MASK::operator && (const DYNAMIC_MASK &maskRight)
{
    //cout << "=============================    DYNAMIC_MASK::operator && " << endl;
    bool    isEqual1 = true,    // 䫠� mask1 = mask1 && mask2
            isEqual2 = true;    // 䫠� mask2 = mask1 && mask2
    short size = this->size > maskRight.size ? maskRight.size : this->size;
    for (int i = 0; i < size; i++)
    {
        //cout << "i=" << i << " this->mask[i]" << this->mask[i] << " maskRight.mask[i]=" << maskRight.mask[i] << endl;
        if ( (this->mask[i] && maskRight.mask[i]) != this->mask[i])     {isEqual1 = false;}
        if ( (this->mask[i] && maskRight.mask[i]) != maskRight.mask[i]) {isEqual2 = false;}
        //cout << "  i=" << i << " this->mask[i]" << this->mask[i] << " maskRight.mask[i]=" << maskRight.mask[i] << endl;
        // �᫨ ��� ���ᨢ� �⫨����� �� १������饣�, � ��� ��᫠ �த������
        if ( !isEqual1  && !isEqual2   )
        {return false;}
    }
    return true;
}

//=============================================================================
// ������ ᮢ������ ��᮪ ������ ࠧ��� = true
inline bool DYNAMIC_MASK::operator == (const DYNAMIC_MASK &maskRight)
{
    //cout << "=============================    DYNAMIC_MASK::operator == " << endl;
    if (this->size != maskRight.size)
    {
        return false;
    }
    for (int i = 0; i < this->size; i++)
    {
        if (this->mask[i] != maskRight.mask[i]) {return false;}
    }
    return true;
}

//=============================================================================
// ������⭮� �
inline const DYNAMIC_MASK DYNAMIC_MASK::operator & (const DYNAMIC_MASK &maskRight)
{
    //cout << "=============================    DYNAMIC_MASK::operator & 1" << endl;
    short maxSize = this->size > maskRight.size ? maskRight.size : this->size;
    DYNAMIC_MASK result(maxSize, 999);
    for (int i = 0; i < maxSize; i++)
    {
        result.mask[i] = this->mask[i] && maskRight.mask[i];
    }
    return result;
}

//=============================================================================
// ������⭮� �
inline const DYNAMIC_MASK operator & (const DYNAMIC_MASK &maskLeft, const DYNAMIC_MASK &maskRight)
{
    //cout << "=============================    DYNAMIC_MASK::operator & 2 " << endl;
    short maxSize = maskLeft.size > maskRight.size ? maskRight.size : maskLeft.size;
    DYNAMIC_MASK result(maxSize, 999);
    for (int i = 0; i < maxSize; i++)
    {
        result.mask[i] = maskLeft.mask[i] && maskRight.mask[i];
    }
    return result;
}

//=============================================================================
// ������⭮� ���
inline const DYNAMIC_MASK DYNAMIC_MASK::operator | (const DYNAMIC_MASK &maskRight)
{
    //cout << "=============================    DYNAMIC_MASK::operator | 1 " << endl;
    //std::cout << "1 this->size =  " << this->size << std::endl;
    //std::cout << "1 maskRight.size =  " << maskRight.size << std::endl;
    short maxSize = this->size > maskRight.size ? maskRight.size : this->size;
    DYNAMIC_MASK result(maxSize, 999);
    for (int i = 0; i < maxSize; i++)
    {
        result.mask[i] = this->mask[i] || maskRight.mask[i];
    }
    return result;
}

//=============================================================================
// ������⭮� ���
inline const DYNAMIC_MASK operator | (const DYNAMIC_MASK &maskLeft, const DYNAMIC_MASK &maskRight)
{
    //std::cout << "=============================    DYNAMIC_MASK::operator | 2 " << std::endl;
    short maxSize = maskLeft.size > maskRight.size ? maskRight.size : maskLeft.size;
    DYNAMIC_MASK result(maxSize, 999);
    for (int i = 0; i < maxSize; i++)
    {
        result.mask[i] = maskLeft.mask[i] || maskRight.mask[i];
    }
    return result;
}

//=============================================================================
// ������⭠� �������
inline const DYNAMIC_MASK DYNAMIC_MASK::operator ~ ()
{
    //cout << "=============================    DYNAMIC_MASK::operator ~ 1 " << endl;
    DYNAMIC_MASK result(this->size, 999);
    for (int i = 0; i < this->size; i++)
    {
        result.mask[i] = !(this->mask[i]);
    }
    return result;
}

//=============================================================================
// ������⭠� �������
inline DYNAMIC_MASK operator ~ (const DYNAMIC_MASK &maskLeft)
{
    //cout << "=============================    DYNAMIC_MASK::operator ~ 2 " << endl;
    DYNAMIC_MASK result(maskLeft.size, 999);
    for (int i = 0; i < maskLeft.size; i++)
    {
        result.mask[i] = !(maskLeft.mask[i]);
    }
    return result;
}

#endif //FISCAT_DYNAMIC_MASK_H
