//
// Created on 02.08.17.
//

#ifndef MENUTREE_H
#define MENUTREE_H

#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <stdio.h>

#include"license.h"
#include "../loggerdaemon.h"

using namespace std;

struct MenuItemData
{
    MenuItemData(const std::string &_name,
                 const std::string &_subscript,
                 const bool _active) : name(_name),
                                       subscript(_subscript),
                                       active(_active) {}
    std::string name;        // �᭮���� ⥪��
    std::string subscript;  // �������
    bool active {true};     // 䫠� ��⨢����
};

typedef std::vector<MenuItemData> MENU_ITEM_TYPE;


enum MENU_ID
{
    SALES_ID = 188,
    KASSA_ID = 1,
        SHIFT_ID,
            SHIFT_ID_OPEN,
            SHIFT_ID_CLOSE,
            SHIFT_ID_X_REPORT,
            SHIFT_ID_RESERVE,
            SHIFT_ID_INSERTION,
            SHIFT_ID_COPY_FISCAL_DOC,
            SHIFT_ID_COPY_FISCAL_DOC_LAST,
            SHIFT_ID_COPY_FISCAL_DOC_NUMBER,
            SHIFT_ID_CANCEL_DOC,
            SHIFT_ID_PRINT_CTRL_TAPE,
            SHIFT_ID_PRINT_MEM_UNIT,
            SHIFT_ID_KL_DOC_COPY,
            SHIFT_ID_CARD_BALANCE,
        DOCS_ID,
            DOCS_ID_CUR_REP,
            DOCS_ID_DOC_FN,
            DOCS_ID_DOC_FN_ONE,
            DOCS_ID_DOC_FN_ALL,
            DOCS_ID_DOC_FN_ALL_TO_FLASH,
            DOCS_ID_DOC_FN_TO_CRPT,
            DOCS_ID_DOC_FISCAL_RESULT,
            DOCS_ID_DOC_FISCAL_RESULT_ONE,
            DOCS_ID_DOC_FISCAL_RESULT_ALL,
            DOCS_ID_RECEIPT_COR,
        ADD_OPERATIONS_ID,
            CONSUMPTION_ID,
            REVERT_ID_REV,
                REVERT_ID_REV_COM,
                REVERT_ID_REV_CON,
        SYSTEM_ID,
            SYSTEM_ID_CH_USER,
            SYSTEM_ID_POWEROFF,
            SYSTEM_ID_REBOOT,
            SYSTEM_ID_LOG,
                SYSTEM_ID_LOG_LEVEL,
                    SYSTEM_ID_LOG_LEVEL_ERROR,
                    SYSTEM_ID_LOG_LEVEL_WARN,
                    SYSTEM_ID_LOG_LEVEL_INFO,
                    SYSTEM_ID_LOG_LEVEL_DEBUG,
                SYSTEM_ID_SAVE_LOG,
                SYSTEM_ID_SEND_LOG,
            SYSTEM_ID_VERSION,
            SYSTEM_ID_VERSION_UPDATE,
            SYSTEM_ID_VERSION_REVERT,
            SYSTEM_ID_SAVE_LIB,
            SYSTEM_ID_RESET,
            SYSTEM_ID_USERS,
                SYSTEM_ID_USERS_INSERT,
                SYSTEM_ID_USERS_EDIT,
                SYSTEM_ID_USERS_DELETE,
            SYSTEM_ID_COUNTERS_EDIT,
                SYSTEM_ID_CASH_IN_FINAL,
                SYSTEM_ID_ADVENT_TOTAL_ABS,
                SYSTEM_ID_CONSUMPTION_TOTAL_ABS,
                SYSTEM_ID_ADVENT_RETURN_TOTAL_ABS,
                SYSTEM_ID_CONSUMPTION_RETURN_TOTAL_ABS,
            SYSTEM_ID_DELETE_GOODS,
            SYSTEM_ID_HOTKEYS,
                SYSTEM_ID_HOTKEYS_DELETE,
            SYSTEM_ID_DIAGNOSTIC,
        SETTINGS_ID,
            SETTINGS_ID_OFD,
                SETTINGS_ID_OFD_REG_KKT,
                SETTINGS_ID_OFD_EDIT_DATA,
                SETTINGS_ID_OFD_CH_OFD,
                SETTINGS_ID_OFD_TUNE_OFD,
                SETTINGS_ID_OFD_REPLACE_FS,
                SETTINGS_ID_OFD_FS_DAMAGED,
                SETTINGS_ID_OFD_DEREG_KKT,
                SETTINGS_ID_OFD_OFFLINE,
                SETTINGS_ID_OFD_CANCEL,
                SETTINGS_ID_OFD_KKT_MODE,
                SETTINGS_ID_OFD_REG_KKT_KPP,
            SETTINGS_ID_NET,
                SETTINGS_ID_NET_ETHERNET,
                    SETTINGS_ID_NET_ETHERNET_STATE,
                    SETTINGS_ID_NET_ETHERNET_SET,
                SETTINGS_ID_NET_WIFI,
                    SETTINGS_ID_NET_WIFI_STATE,
                    SETTINGS_ID_NET_WIFI_SET,
                SETTINGS_ID_NET_2G,
                    SETTINGS_ID_NET_2G_ON,
                    SETTINGS_ID_NET_2G_OFF,
            SETTINGS_ID_DATE_ZONE,
                SETTINGS_ID_DATE_TIME,
                SETTINGS_ID_TIME_ZONE,
            SETTINGS_EQUIPMENT,
                SETTINGS_EQUIPMENT_ID_SCALES,
                    SETTINGS_EQUIPMENT_ID_SCALES_USE_TARE,
                    SETTINGS_EQUIPMENT_ID_SCALES_MY_TARE,
                        SETTINGS_EQUIPMENT_ID_SCALES_MY_TARE_ADD,
                        SETTINGS_EQUIPMENT_ID_SCALES_MY_TARE_ALL_TARES,
                        SETTINGS_EQUIPMENT_ID_SCALES_MY_TARE_DEFAULT_TARE,
                    SETTINGS_EQUIPMENT_ID_SCALES_INFO,
                SETTINGS_EQUIPMENT_ID_BANK_TERM,
                    SETTINGS_EQUIPMENT_ID_BANK_TERM_STATE,
                    SETTINGS_EQUIPMENT_ID_BANK_TERM_TYPE,
                    SETTINGS_EQUIPMENT_ID_BANK_TERM_OPS,
                        SETTINGS_EQUIPMENT_ID_BANK_TERM_OPS_KEYS,
                        SETTINGS_EQUIPMENT_ID_BANK_TERM_OPS_KEYS_DEF,
                        SETTINGS_EQUIPMENT_ID_BANK_TERM_OPS_KEYS_MASTER,
                        SETTINGS_EQUIPMENT_ID_BANK_TERM_OPS_KEYS_WORK,
                        SETTINGS_EQUIPMENT_ID_BANK_TERM_OPS_PARAM,
                        SETTINGS_EQUIPMENT_ID_BANK_TERM_OPS_CONN,
                        SETTINGS_EQUIPMENT_ID_BANK_TERM_OPS_INFO,
                        SETTINGS_EQUIPMENT_ID_BANK_TERM_OPS_EMV,
                        SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_CASHIER,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_REVERSAL_LAST,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_RECON,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_TEST_CON,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_SHORT_REPORT,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_FULL_REPORT,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_TC_INFO,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_TC_VALIDATE,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_REBOOT,
                        SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_SET,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SET_DATE_TIME,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_SERVICE_MENU,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_TMS,
                            SETTINGS_EQUIPMENT_ID_BANK_TERM_SKYPOS_ACTIVATION,
                SETTINGS_EQUIPMENT_ID_BLUETOOTH,
                    SETTINGS_EQUIPMENT_ID_BLUETOOTH_ON,
                    SETTINGS_EQUIPMENT_ID_BLUETOOTH_OFF,
                    SETTINGS_EQUIPMENT_ID_BLUETOOTH_PAIR,
            SETTINGS_ID_DISCONT,
            SETTINGS_ID_TECH_REP,
            SETTINGS_ID_TAXS,
            SETTINGS_ID_CABINET,
            SETTINGS_ID_AGENTS,
            SETTINGS_ID_TRADE_RULES,
                SETTINGS_ID_TRADE_RULES_DISCOUNTS,
                    SETTINGS_ID_TRADE_RULES_DISCOUNT_ADD,
                        SETTINGS_ID_TRADE_RULES_DISCOUNT_ADD_RUB,
                        SETTINGS_ID_TRADE_RULES_DISCOUNT_ADD_PERC,
                    SETTINGS_ID_TRADE_RULES_DISCOUNT_DEL,
                SETTINGS_ID_TRADE_RULES_FREE_PRICE,
                SETTINGS_ID_TRADE_RULES_TOBACCO_MRC,
                        SETTINGS_ID_TRADE_RULES_PAY_FUNCS,
                            SETTINGS_ID_TRADE_RULES_PAY_FUNCS_ACTIVE,
                            SETTINGS_ID_TRADE_RULES_PAY_FUNCS_EXPIRED,
                        SETTINGS_ID_TRADE_RULES_FREE_PRICE_NDS,
                        SETTINGS_ID_TRADE_RULES_FREE_PRICE_NDS_EDIT,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_NDS_RB_1,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_NDS_RB_2,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_NDS_RB_3,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_NDS_RB_4,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_NDS_RB_5,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_NDS_RB_6,
                        SETTINGS_ID_TRADE_RULES_FREE_PRICE_NDS_CHOOSE,
                    SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP,
                        SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_EDIT,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_1,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_2,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_3,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_4,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_5,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_6,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_7,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_8,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_9,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_10,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_11,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_12,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_13,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_14,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_15,
                            SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_16,
                        SETTINGS_ID_TRADE_RULES_FREE_PRICE_DEP_CHOOSE,
                        SETTINGS_ID_TRADE_RULES_EGAIS,
                            SETTINGS_ID_TRADE_RULES_EGAIS_SALE_ALCOHOL,
                            SETTINGS_ID_TRADE_RULES_EGAIS_SET_UTM,
                                SETTINGS_ID_TRADE_RULES_EGAIS_SET_UTM_IP,
                                SETTINGS_ID_TRADE_RULES_EGAIS_SET_UTM_PORT,
                                SETTINGS_ID_TRADE_RULES_EGAIS_SET_UTM_TIMEOUT,
                                SETTINGS_ID_TRADE_RULES_EGAIS_SET_UTM_CHECK_CONNECTION,
                                SETTINGS_ID_TRADE_RULES_EGAIS_SET_PRINT,
                            SETTINGS_ID_TRADE_RULES_EGAIS_SALE_CONTROL,
                                SETTINGS_ID_TRADE_RULES_EGAIS_SALE_CONTROL_SET_CHECK_TIME,
                                SETTINGS_ID_TRADE_RULES_EGAIS_SALE_CONTROL_SET_CHECK_TIME_HOURS,
                            SETTINGS_ID_TRADE_RULES_EGAIS_REQUEST_MRP_TABLES,
                SETTINGS_ID_TRADE_RULES_AUTO_RESERVE,
                SETTINGS_ID_TRADE_RULES_ENVD,
                SETTINGS_ID_TRADE_RULES_INC_POS,
                SETTINGS_ID_TRADE_RULES_FREE_PRICE_SALE,
                SETTINGS_ID_TRADE_RULES_FREE_PRICE_SALE_MAX_SUM,
                SETTINGS_ID_TRADE_RULES_CLOUD_LOAD,
                SETTINGS_ID_SOUND_BUTTON,
                SETTINGS_ID_DISCOUNTS,
                    SETTINGS_ID_DISCOUNTS_ADD,
                    SETTINGS_ID_DISCOUNTS_REMOVE,
    SETTINGS_ID_TRADE_RULES_ENTER_WEIGHT,
    SETTINGS_ID_TRADE_RULES_ENTER_WEIGHT_ON,
    SETTINGS_ID_TRADE_RULES_ENTER_WEIGHT_OFF,
    SETTINGS_ID_TRADE_RULES_PAY_TYPES,
        SETTINGS_ID_TRADE_RULES_PAY_TYPES_CREATE,
        SETTINGS_ID_TRADE_RULES_PAY_TYPES_EDIT,
        SETTINGS_ID_TRADE_RULES_PAY_TYPES_DELETE,
    SETTINGS_ID_REG_BSO,
            SETTINGS_ID_INTERNET_SHOP,
            SETTINGS_ID_INTERNET_AUTO_RECIEPT,
                SETTINGS_AUTO_OPEN_SHIFT_EN,
                SETTINGS_AUTO_OPEN_SHIFT_DIS,
    SETTINGS_ID_ROUTE_DATA,
        SETTINGS_ID_ROUTE_DATA_INSPECTOR_ID,
        SETTINGS_ID_ROUTE_DATA_DRIVER_ID,
        SETTINGS_ID_ROUTE_DATA_RUN_ID,
        SETTINGS_ID_ROUTE_DATA_TAIL_NUMBER,
        SETTINGS_ID_ROUTE_DATA_ROUTE_ID,
        SETTINGS_ID_ROUTE_DATA_START_ZONE,
        SETTINGS_ID_ROUTE_DATA_PLATE_NUMBER,
        SETTINGS_ID_ROUTE_DATA_VEHICLE_ID,
        SETTINGS_ID_ROUTE_DATA_VEHICLE_TYPE,
        SETTINGS_ID_ROUTE_DATA_OPERATOR_ID,
    SPEC_CLC_METHOD,
        SPEC_CLC_METHOD_ON_POS,
            DISCOUNT_POSITION,
            FULL_PREPAYMENT,
            PREPAYMENT,
            IMPREST,
            FULL_CREDIT,
            PREPAYMENT_CREDIT,
            CREDIT_PAY,
        SPEC_CLC_METHOD_ON_REC,
            ADVANCE,
            AGENTS_APPLY,
            SPEC_CLC_METHOD_ENTITY,
            CHECK_SEND,
    CABINET_CONNECT,
    CABINET_DISCONNECT,
    CABINET_RELOAD_GOODS,
    BUYERS_DATA,
    RECEIPT_SEND,
        RECEIPT_TO_SMS,
        EMAIL,
    ROOT_ID,
    INVALID_ID,
    PIRIT_TEST,
    //--
    //���樠��� ��⮤� ������ RB
    DISCOUNT_CHECK,
    DISCOUNT_POS,
    MARGIN_CHECK,
    MARGIN_POS,
    DEPARTMENT_SALE,

    DISCOUNT_POS_RET,
    MARGIN_CHECK_RET,
    MARGIN_POS_RET,
    DEPARTMENT_SALE_RET,
    //--
    // ����� ������⪨
    TRANSPORT_ROUTES,
    // COURIER
    COURIER_SALE,
    TRANSPORT_CASHLESS,
    //--
    UNKNOWN_MENU_ID
};


enum ITEM_TYPE
{
    NODE_ = 0,
    LEAF_
};


//============================================================================
// ����� �-� ��ॢ� ����.
// �࠭�� ���ଠ�� � ���孥� � ������ �஢���.
// ����� � ���뭬 �१ ����䥩�� �����
// � �������� ४��ᨢ�� ��頥� �� �������� ���������
//============================================================================


class MenuItem
{
public:
    /**
     * �������� ��࠭�祭��, ��������� �� �㭪� ����
     */
    enum MENU_RULES
    {
        RULE_NO = 0x00,
        RULE_ONLY_DK_KEY = 0x01,
        RULE_LOCAL_OPTION_ALCOHOL   // ��⨢�� ⮫쪮 �� ����稨 �����쭮� ��樨 "�த��� �������쭮� �த�樨"
    };

private:

    // ��������
    MenuItem *parent;                 // ���孨� �஢��� ����
    std::vector<MenuItem*> childVect; // ����� ����⮢ �������
    // ����ঠ���
    MENU_ID id;         // 㭨����� ID �㭪� ����
    ITEM_TYPE type;          // ��� �⥬� ���� (��⪠/����)
    std::string name;        // ��� �㭪� ���� (��� �����)
    std::string _subscript;   // ������� ����
    uint64_t rule;           // ��᪠ ����㯠
    bool visible;            // 䫠� �������� � ����

    std::string GetStrIndex(); // ����祭�� ����� � ��ॢ� �ଠ� "1.". �᫨ ����� 10 �-⮢, � "1. "

public:
    MenuItem (MenuItem *newParent,
              string newName,
              ITEM_TYPE it,
              MENU_ID id,
              uint64_t rule,
              const std::string &subscript = "");
    MenuItem (const MenuItem &item) = delete;
    ~MenuItem();
    //----------------------
    // �������
    //----------------------
    MenuItem& operator = (MenuItem &rItem);
    //----------------------
    // ����䥩�� � ����
    //----------------------
    // 1 parent
    inline void SetParent(MenuItem *newParent)   {parent = newParent;}          // ������ த�⥫� ������
    inline MenuItem* GetParent()                 {return parent;}               // ������� த�⥫�

    // 2 childVect
    bool AddChild    (MenuItem* newChild);                                      // �������� ���� �-� �������
    inline vector<MenuItem*> & GetChildVect()   {return childVect;}             // ������� ����� �-⮢ �������

    // 3 id
    inline MENU_ID GetID()                 {return id;}                    // ������� 㭨����� ID �-� ����
    inline void SetID(MENU_ID newID)       {id = newID;}                   // ������� 㭨����� ID �-� ����

    // 4 type
    inline void SetType(const ITEM_TYPE newType){type = newType;}               //������ ⨯ �-�
    inline ITEM_TYPE GetType()                  {return type;}                  //������� ⨯ �-�

    // 5 name
    inline void SetName(const string newName)   {name = newName;}                       //������ ��� �-�
    string GetNameWithNo()                      {return GetStrIndex() + " " + name;}    //������� ��� �-� � �ਪ����� ����஬
    inline string GetName()                     {return name;}                          //������� ��� �-� ��� �����
    /**
     * @brief getSubscript �⥭�� ������ ����� ����
     * @return ������� ����� ����
     */
    std::string getSubscript() const {return _subscript;}
    /**
     * @brief setSubscript ������ ������ ����� ����
     * @param _subscript ������� ����� ����
     */
    void setSubscript(const std::string &subscript) {_subscript = subscript;}
    // 6 rule
    inline void SetRule(const uint64_t newRule){rule = newRule;}              // ������ �ࠢ� �-�
    inline uint64_t GetRule()      const            {return rule;}                 // ������� �ࠢ� �-�
    bool    HasRule(MenuItem::MENU_RULES r)  {return (rule & static_cast<uint64_t>(r)) == r ;} // ������� �ࠢ� �-�

    // 7 visible
    inline void SetVisible(const bool newVis)    {visible = newVis;}            // ������ ��������� �-�
    inline bool GetVisible()                     {return visible;}              // ������� ��������� �-�
           void SetVisibleRecursiveUp();                                        // ������� ����� ������ ������ �� ����
           void SetVisibleRecursiveDown();                                      // ������� ������ ᥡ� � ��� ��⮬���
           void SetInvisibleRecursiveDown();                                    // ������� �������� ᥡ� � ��� ��⮬���

    //----------------------
    // ��⠫�� ��⮤�
    //----------------------
            void OutInfo();                             // �뢮� � cout ����� �������
             int GetLvlOfItm();                         // ����祭�� �஢�� ��㡨�� ����� � ��ॢ�, 1...
    unsigned int GetIndexOfItm();                       // ����祭�� ���浪����� ����� �।� ������� ���쥢, 1...
       MenuItem* FindItmById(MENU_ID searchingID); // �����ᨢ�� ���� �⥬� � �����ॢ� �� ID, ��������� ����������!
};


//======================================
// ����� ��ॢ� ����.
// ���⮨� �� �⥬�� ���. �᧤�� ��� ������樨. �࠭�� ��ॢ� �⥬�� � �����⢥���� �������
//======================================
class MenuTree
{
private:
    MenuItem *root_item;            // ��७� ��ॢ�
    MenuItem *cur_item;             // �����⥫� �� ⥪�騩 ����� ��ॢ�
    shared_ptr<License> _license;   // �����⥫� �� ����� ��業���

public:
    // ���������, ���樠������ ��ॢ� ��୥�� ����⮬
    MenuTree()      {root_item = new MenuItem(NULL, "root", NODE_, ROOT_ID, 0);
                    cur_item = root_item;}
    // ��������, 㤠����� root_item, �� �ਢ���� � ४��ᨢ��� ���⪥ ��ॢ�
    ~MenuTree()     {delete root_item;}

    //----------------------------------------
    // ��⮤� ��� ࠡ��� � ���������� ����⮢ ��ॢ�
    //----------------------------------------
    // ������� ������ �� ��ॢ�
    void TreeSetVisibleAll      ()
    {
        root_item->SetVisibleRecursiveDown();
    }
    // ������� �����묨 �-�� � ����� ID
    bool TreeSetVisibleItems    ( vector<MENU_ID> vID);
    // ������� �������묨 �-�� � ����� ID
    bool TreeSetInvisibleItems  ( vector<MENU_ID> vID);
    /**
     * @brief updateSubscriptById ���������� ������ ����� ����
     * @param id �����䨪��� ����� ����
     * @param subscript �������
     */
    void updateSubscriptById(const MENU_ID id, const std::string &subscript) const;
    //----------------------------------------
    // ��⮤� ������樨 true - ���室 ᮢ��襭 �ᯥ譮, false - ���室 ��㤠祭(�ࠩ��� �����)
    //----------------------------------------
    void GoRoot()   {cur_item = root_item;}             // ��३� � ��७�
    bool GoDown(MENU_ID ID);             		    // ��३� � ������� �� ID
    bool GoDown(string fullName);        		        // ��३� � ������� �� ������� �����(� ����஬)
    bool GoUp();                                        // ��������� ����� �� ��ॢ� ����
    bool GoRight();                                     // ��३� � ���� �ࠢ�
    bool GoLeft();                                      // ��३� � ���� ᫥��
    // �஢�ઠ �� ����稥 ������� ��⮬��� � ⥪. �-�
    bool IsEnd();
    // �஢�ઠ �� ��宦����� � ��୥
    bool IsRoot             ()                   { return (cur_item == root_item) ? true : false; }
    // �஢�ઠ �� ��������� ����� �� ���室� �� �������
    bool IsSubMnuItmVisbl   (unsigned int index);
    /**
     * @brief isSubMenuItemActive ��� �� ����㯭���� ����� ����
     * @param index ������ �����
     * @return १���� �믮������
     */
    bool isSubMenuItemActive(const unsigned int index);
    // ������⢮ ������� ��⮬��� � ⥪. �-�
    unsigned int NumOfChilds();
    /**
     * setLicense - ������ ��業��� � �����
     * @param lic - 㪠��⥫� �� ��業���
     */
    inline void setLicense(const shared_ptr<License> &lic) { _license = lic; };
    //----------------------------------------
    // ��⮤� ��� ࠡ���� � ᯨ᪮� �������
    //----------------------------------------
    bool AddSubMenu(MenuItem*  newItem);          // �ਢ易�� ���� ����� ������� � ��ॢ�
    bool CreateSubMenu(string  newName,           // ������� ���� ����� �������
                       ITEM_TYPE    nt,
                       MENU_ID id,
                       uint64_t rule = MenuItem::RULE_NO,
                       const std::string &subscript = "");

    bool CreateSubMenu(const MENU_ID id,
                       const ITEM_TYPE type,
                       const uint64_t rule = MenuItem::RULE_NO,
                       const std::string &subscript = "") {return CreateSubMenu("Hardcode name", type, id, rule, subscript);}
    // ������� ᯨ᮪ ����(� ����ࠬ�) ��� �������, �� ⮫쪮 �������!
    std::vector<std::string> GetSubMenuNames();
    /**
     * @brief getActive ��� �� ����㯭���� �㭪� ����
     * @param menuItem �㭪� ����
     * @return १���� �믮������
     */
    bool getActive(MenuItem *menuItem);
    // ������� ᯨ᮪ ����(� ����ࠬ�) ��� �������, �� ⮫쪮 �������, � ⮬ �᫥ � "����"
    MENU_ITEM_TYPE GetSubMenuNamesWithIncative();
    std::vector<MENU_ID> GetSubMenuIdList();
    // ������� ID ������� �� ������� ������� ����⮢, �� ��⮤� GetSubMenuNames()
    MENU_ID GetSubMenuIDByIndex( int index);

    //--------------------
    // ����祭�� ���ଠ樨 � ��୥ ��ॢ�
    //--------------------
    MENU_ID getRootID ()    // ������� ID ��୥���� �����, ��� �����䨪�樨 ��ॢ�
    {
        return root_item->GetID();
    }

    //--------------------
    // ��⮤� ��� ����祭�� ���祭�� ⥪ �⥬�
    //--------------------
    inline MENU_ID GetCurID()             {return cur_item->GetID();}             // ������� 㭨����� ID �-� ����
    inline ITEM_TYPE    GetCurType()           {return cur_item->GetType();}           // ������� ⨯ �-�
    inline string       GetCurName()           {return cur_item->GetName();}           // ������� ��� �-� ��� �����
    inline std::string getCurSubscript() const {return cur_item->getSubscript();}      // �⥭�� ������ ⥪�饣� ����� ����
    inline string       GetCurNameWithNo()     {return cur_item->GetNameWithNo();}     // ������� ��� �-� � �ਪ����� ����஬
    inline uint64_t     GetCurRule()     const {return cur_item->GetRule();}           // ������� �ࠢ� �-�
    inline int          GetCurLevel()          {return cur_item->GetLvlOfItm();}       // ������� ⥪�騩 �஢��� ����������
           unsigned int GetCurIndex()          {return cur_item->GetIndexOfItm();}     // ����祭�� ���浪����� ����� ⥪ ������� ���-� � ���� 1...N

           MenuItem* CurrentItem() {return cur_item;}    // �����⥫� �� ⥪ Item � ��ॢ� ����(����� ��ॢ�) ������������ ������������, ��� ������

};


#endif //MENUTREE_H
