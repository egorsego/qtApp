//
// Created on 02.08.17.
//

#include "MenuTree.h"


//========================================================================================
//========================================================================================
//============                      ����� MenuItem               =========================
//========================================================================================
//========================================================================================
// 1 ���������
MenuItem::MenuItem(MenuItem *newParent, string newName, ITEM_TYPE it, MENU_ID id, uint64_t rule, const std::string &subscript)
{
    this->name = newName;
    _subscript = subscript;
    this->parent = newParent;
    if(newParent != NULL)
    {
        newParent->AddChild(this);
    }
    this->SetType(it);
    this->SetID(id);
    this->SetRule(rule);
    SetVisible(true);
}

//========================================================================================
// ��������, ४��ᨢ��� 㤠����� �ᥩ ��⪨ ����, � ��᫥���騬 㤠������ 㪠��⥫� �� ᥡ� �� ᯨ᪠ ��⥩ த�⥫�
MenuItem::~MenuItem()
{
    while( !childVect.empty() )
    {
        delete  childVect.back();
    }
    // �������� �� ����� parent ᥡ�, �᫨ ��� �-� �� ��७�
    if(GetParent() != NULL)
    {
        vector<MenuItem*>::iterator it=find(GetParent()->GetChildVect().begin(), GetParent()->GetChildVect().end(), this);
        GetParent()->GetChildVect().erase(it);
    }
}


//========================================================================================
// �����ᨢ�� ���� �⥬� � �����ॢ� �� ID
MenuItem* MenuItem::FindItmById(MENU_ID searchingID)
    {
    // �᫨ ��� ����� � ���� �᪮��, � �����頥� 㪠��⥫� �� ����
    if (this->GetID() == searchingID)
    {
        return this;
    }
    // �᫨ �� �� �� �����, � �� �����, � �����頥� NULL(�㯨�)
    if(this->GetChildVect().empty())
    {
        return NULL;
    }
    // �᫨ � �⮣� ����� ���� ��⮬��, � ४��ᨢ�� �饬 � ���
    for(unsigned int i=0; i<GetChildVect().size(); i++)
    {
        MenuItem* iterItm = GetChildVect().at(i)->FindItmById(searchingID);
        // �᫨ ������� ��襫��, � ����頥� 㪠��⥫�
        if( iterItm != NULL)
        {
            return iterItm;
        }
    }
    // �᫨ ������� �� ��襫��, � ����頥� NULL
    return NULL;
    }
//========================================================================================
// �������� ���� �-� �������    �������� �� ID????????
bool MenuItem::AddChild(MenuItem *newChild)
{
    if( newChild->GetName().empty() )
        {return false;}
    childVect.push_back(newChild);
    newChild->SetParent(this);
    return true;
}


//========================================================================================
// ����祭�� ����� � ��ॢ� �ଠ� "1 ". �᫨ ����� 10 �-⮢, � "1  " (��� ���쭥�襣� ��������� � �����)
string MenuItem::GetStrIndex()
{
    if (this->parent == NULL)
        {return string("R.");}
    for (int index = 0; index < int(GetParent()->GetChildVect().size()); index++)
    {
        if( this == GetParent()->GetChildVect().at(index) )
        {
            string strIndex;
            strIndex = to_string( index +1 ) + string(".");
            return string(strIndex);
        } // if
    } // for
    logDBG( "MenuItem:: GetStrIndex() %s:: Stop. Error, Element didn't found! How!?" , GetName().c_str() );
    return string("? ");
}


//========================================================================================
// ����祭�� �஢�� ��㡨�� ����� � ��ॢ�
int MenuItem::GetLvlOfItm()
{
    MenuItem *curItem;
    int lvl=0;
    curItem = this;
    while (curItem->GetParent() != NULL || lvl < 99) // 99 - १�ࢭ� ��室
    {
        lvl++;
        curItem = curItem->GetParent();
    }//while
    return lvl;
}


//========================================================================================
// ����祭�� ���浪����� ����� �।� ������� ���쥢, 1...
unsigned int MenuItem::GetIndexOfItm()
{
    // �᫨ �� �� ���設� ��ॢ�, � false
    if (this->GetParent() == NULL)
    {
        return -1;
    }
    vector <MenuItem*> clds = GetParent()->GetChildVect();
    int index = 0;
    for(unsigned int i=0; i < clds.size(); i++)
    {
        if (clds.at(i)->GetVisible() == true)   {index++;}
        if (this == clds.at(i))                 {break;}
    }

    return index;
}


//========================================================================================
// ������� ����� ������ ������ �� ����
void MenuItem::SetVisibleRecursiveUp()
{
    // ������ ������ ⥪. �����
    this->SetVisible(true);
    // �᫨ �� ��७�, � ४��ᨢ�� ������ ������ � த�⥫�
    if( this->GetParent() != NULL )
    { this->GetParent()->SetVisibleRecursiveUp(); }
}


//========================================================================================
// ������� ������ ᥡ� � ��� ��⮬���. �㦭� ��� ��⨬���樨 ��室� ��ॢ�
void MenuItem::SetVisibleRecursiveDown()
{
    // ������ �������� ⥪. �����
    this->visible = true;
    // �᫨ ���� ��⮬��, � �� ⮦� ������ ४��ᨢ�� �������묨
    if( this-> GetChildVect().empty() == false )
    {
        for (vector<MenuItem *>::iterator it = this->GetChildVect().begin();
             it != this->GetChildVect().end();
             it++)
        {
            (*it)->SetVisibleRecursiveDown();
        }
    }
}


//========================================================================================
// ������� �������� ᥡ� � ��� ��⮬���
void MenuItem::SetInvisibleRecursiveDown()
{
    // ������ �������� ⥪. �����
    this->visible = false;
    // �᫨ ���� ��⮬��, � �� ⮦� ������ ४��ᨢ�� �������묨
    if( this-> GetChildVect().empty() == false )
    {
        for (vector<MenuItem *>::iterator it = this->GetChildVect().begin();
             it != this->GetChildVect().end();
             it++) {
            (*it)->SetInvisibleRecursiveDown();
        }
    }
}



//========================================================================================
// ����஢���� ��� ��ࠬ��஢ �� �᪫�祭��� ᯨ᪠ ��⥩, �⮡� �� ���몠�� ��ॢ�
MenuItem& MenuItem::operator=(MenuItem &rItem)
{
    if ( this != &rItem )
    {
        this->SetID(rItem.GetID());
        this->SetParent(rItem.GetParent());
        this->SetType(rItem.GetType());
        this->SetName(rItem.GetName());
        _subscript = rItem.getSubscript();
        this->SetRule(rItem.GetRule());
        this->SetVisible(rItem.GetVisible());
    }

    return *this;
}


//========================================================================================
// �뢮� � cout ����� ������� (��� �⫠���)
void MenuItem::OutInfo()
{
    logDBG( "-----------");
    logDBG( "MenuItem:: OutInfo() %s:: ID = %llu", GetName().c_str(), GetID() );
    logDBG( "MenuItem:: OutInfo() %s:: Type = %d", GetName().c_str(), GetType() );
    logDBG( "MenuItem:: OutInfo() %s:: Full name(with Number) = %s", GetName().c_str(), GetNameWithNo().c_str() );
    logDBG( "MenuItem:: OutInfo() %s:: Visible = %B", GetName().c_str(), GetVisible() );
    logDBG( "MenuItem:: OutInfo() %s:: Num Of Childs = %d", GetName().c_str(), GetChildVect().size() );
    logDBG( "-----------");
}



//========================================================================================
//========================================================================================
//============                      ����� MenuTree               =========================
//========================================================================================
//========================================================================================
// ��������� ����� �� ��ॢ� ����
bool MenuTree::GoUp()
{
    if(cur_item->GetParent() == NULL)
    {
        return false;
    }
    cur_item = cur_item->GetParent();
    return true;
}


//========================================================================================
// ��३� � ������� �� ID
bool MenuTree::GoDown(MENU_ID subMenuID)
{
    // �᫨ �� � ����筮� ����� ��ॢ�, � false
    if (cur_item->GetChildVect().empty())
    {
        return false;
    }
    // �饬 ᮢ�������
    for(vector<MenuItem*>::iterator it = cur_item->GetChildVect().begin();
                                    it != cur_item->GetChildVect().end();
                                    it++)
    {
        if ( (*it)->GetID() == subMenuID )
        {
            cur_item = *it;
            return true;
        }
    }
    // �᫨ ᮢ������� �� �������
    return false;
    }


//========================================================================================
// ��३� � ������� �� ������� ����� (��ப� � ��ᯫ��, � � ����஬)
bool MenuTree::GoDown(string fullName)
{
    // �᫨ �� � ����筮� ����� ��ॢ�, � false
    if (cur_item->GetChildVect().empty())
    {
        return false;
    }
    // �饬 ᮢ������� �� ��ࠬ����
    for(vector<MenuItem*>::iterator it = cur_item->GetChildVect().begin();
                                    it != cur_item->GetChildVect().end();
                                    it++)
    {
        if ( (*it)->GetNameWithNo() == fullName )
        {
            cur_item = *it;
            return true;
        }
    }
    // �᫨ ᮢ������� �� �������
    return false;
}


//========================================================================================
// ��३� � ������ �ࠢ�(�� �� �஢���)
bool MenuTree::GoRight()
{
    MenuItem *newItem;
    int itmIndex=-1;    // ������ ��⥭樠�쭮�� ���� ��� ���室�
    itmIndex = cur_item->GetIndexOfItm();
    if (itmIndex < 0)
        {return false;}
    itmIndex--; // � ����� �� 0 � �� �� 1
    if (itmIndex >= int(cur_item->GetParent()->GetChildVect().size()))
    {
        return false;
    }
    newItem = cur_item->GetParent()->GetChildVect().at(itmIndex + 1);
    cur_item = newItem;
    return true;
}


//========================================================================================
// ��३� � ������ �ࠢ�(�� �� �஢���)
bool MenuTree::GoLeft()
{
    MenuItem *newItem;
    int itmIndex=-1;    // ������ ��⥭樠�쭮�� ���� ��� ���室�
    itmIndex = cur_item->GetIndexOfItm();
    if (itmIndex < 0)
        {return false;}
    itmIndex--; // � ����� �� 0 � �� �� 1
    if (itmIndex <= 0)
    {
        return false;
    }
    newItem = cur_item->GetParent()->GetChildVect().at(itmIndex - 1);
    cur_item = newItem;
    return true;
}


//========================================================================================
// ������� ID ������� �� ��������� �������, � �� ����� �� ���������
MENU_ID MenuTree::GetSubMenuIDByIndex( int index)
{
    if (cur_item->GetChildVect().empty() || cur_item->GetChildVect().size() <= (unsigned int)index)
    {
        return INVALID_ID;
    }
    return ( cur_item->GetChildVect().at(index)->GetID()    ) ;

}

//========================================================================================
// ������� ᯨ᮪ ����(� ����ࠬ�) ��� �������, �� ⮫쪮 �������!
vector<std::string> MenuTree::GetSubMenuNames()
{
    vector<string> vectorNames;
    vectorNames.clear();
    // �᫨ ��� ��⥩(�������)
    if (cur_item->GetChildVect().empty())
    {
        vectorNames.push_back("NO ITEMS");
        return vectorNames;
    }
    for(vector<MenuItem*>::iterator it = cur_item->GetChildVect().begin();
        it != cur_item->GetChildVect().end();
        it++)
    {
        // �᫨ ����� ������, � ����頥� ��� � ᯨ᮪
        if((*it)->GetVisible())
        {
            vectorNames.push_back( (*it)->GetNameWithNo() );
        }
    }
    if (vectorNames.empty())
    {
        vectorNames.push_back("NO ITEMS");
        return vectorNames;
    }
    return vectorNames;
}

bool MenuTree::getActive(MenuItem *menuItem)
{
    bool active {false};

    if (menuItem == nullptr)
    {
        logERR("%s menuItem == nullptr!", __FUNCTION__);
        return active;
    }

    switch (menuItem->GetRule())
    {
        case MenuItem::MENU_RULES::RULE_ONLY_DK_KEY:
        {
            // active if has key or no RULE_ONLY_DK_KEY
            bool isDKSubscribeExist {_license && _license->isTagExistAndValid(License::LICENSE_TAG_SUBSCRIPTION)};
            active =  !menuItem->HasRule(MenuItem::MENU_RULES::RULE_ONLY_DK_KEY) || isDKSubscribeExist;
            break;
        }
        case MenuItem::MENU_RULES::RULE_LOCAL_OPTION_ALCOHOL:
        {
            bool isSellAlcoholExist {_license && _license->isLocalLicExist(License::LOCAL_LICENSE_TAG::SELL_ALCOHOL)};
            active =  !menuItem->HasRule(MenuItem::MENU_RULES::RULE_LOCAL_OPTION_ALCOHOL) || isSellAlcoholExist;
            break;
        }
        default: active = true; break;
    }

    return active;
}

//========================================================================================
MENU_ITEM_TYPE MenuTree::GetSubMenuNamesWithIncative()
{
    MENU_ITEM_TYPE vNameAndActive;
    // �᫨ ��� ��⥩(�������)
    if (cur_item->GetChildVect().empty())
    {
        vNameAndActive.push_back({"NO ITEMS", "", true});
        return vNameAndActive;
    }

    for (const auto &item : cur_item->GetChildVect())
    {
        // �᫨ ����� ������, � ����頥� ��� � ᯨ᮪
        if (item->GetVisible())
        {
            vNameAndActive.push_back({item->GetNameWithNo(), item->getSubscript(), getActive(item)});
        }
    }

    if (vNameAndActive.empty())
    {
        vNameAndActive.push_back({"NO ITEMS", "", true});
    }
    return vNameAndActive;
}

//========================================================================================
// ������� ᯨ᮪ id ��� �������, �� ⮫쪮 �������!
vector<MENU_ID> MenuTree::GetSubMenuIdList()
{
    vector<MENU_ID> vectorId;
    vectorId.clear();

    // �᫨ ��� ��⥩(�������)
    if (cur_item->GetChildVect().empty())
    {
        return vectorId;
    }

    for(const auto &item : cur_item->GetChildVect())
    {
        // �᫨ ����� ������, � ����頥� ��� � ᯨ᮪
        if(item->GetVisible())
        {
            vectorId.push_back( item->GetID() );
        }
    }
    return vectorId;
}

//========================================================================================
// �������� ���� ����� �������
bool MenuTree::AddSubMenu(MenuItem* newItem)
{
    return cur_item->AddChild(newItem);
}

//========================================================================================
// ������� ���� ����� �������
bool MenuTree::CreateSubMenu(string newName,
                             ITEM_TYPE it,
                             MENU_ID id,
                             uint64_t rule,
                             const std::string &subscript)
{
    MenuItem *newItem;
    newItem = new MenuItem(cur_item, newName, it, id, rule, subscript);
    newItem->GetChildVect().clear();
    return true;
}
//========================================================================================
// ������� �����묨 �-�� � ����묨 ID, return false - �� �� ID ᤥ���� �����묨 (�� �� �������)
bool MenuTree::TreeSetVisibleItems( vector<MENU_ID> vID)
{
    MenuItem *tmpItem;
    bool res = true;
    // ���� �� ᯨ�� vID
    for (vector<MENU_ID>::iterator it = vID.begin();
         it != vID.end();
         it++)
    {
        tmpItem = root_item->FindItmById(*it);
        // �᫨ ����� � ⠪�� ������ �� ������, � ���室�� � ᫥� �� ᯨ᪠ vID
        if(tmpItem == NULL)
            {
                res = false;
                logDBG("MenuTree::TreeSetVisibleItems: Can't find ID = %d", (*it));
                continue;
            }
        // ���� ������ ����� ����� ������ �� ����
        tmpItem->SetVisibleRecursiveUp();
    }
    return res;
}

/*
//========================================================================================
// ����祭�� ���浪����� ����� ⥪ ���-� � ���� 1...N
int MenuTree::GetCurIndexVisbl()
{
    int counter = 0;
    for(vector<MenuItem*>::iterator it = cur_item->GetChildVect().begin();
        it != cur_item->GetChildVect().end();
        it++)
    {
        // �᫨ ����� ������, � ����頥� ��� � ᯨ᮪
        if((*it)->GetVisible())
        {
            counter++;
            cout << "MenuTree::GetCurIndexVisbl() counter ++ " << endl;
        }
        if( ( (*it)->GetVisible() )
            &&
            ( this->GetCurNameWithNo() == (*it)->GetNameWithNo() )     )
        {
            cout << "MenuTree::GetCurIndexVisbl() counter = " << counter << endl;
          return counter;
        }
    }
    return 0;

}
*/

//========================================================================================
// ������� �����묨 �-�� � ����묨 ID, return false - �� �� ID ᤥ���� �����묨 (�� �� �������)
bool MenuTree::TreeSetInvisibleItems( vector<MENU_ID> vID)
{
    MenuItem *tmpItem;
    bool res = true;
    // ���� �� ᯨ�� vID
    for (vector<MENU_ID>::iterator it = vID.begin();
         it != vID.end();
         it++)
    {
        tmpItem = root_item->FindItmById(*it);
        // �᫨ ����� � ⠪�� ������ �� ������, � ���室�� � ᫥� �� ᯨ᪠ vID
        if(tmpItem == NULL)
        {
            res = false;
            logDBG("MenuTree::TreeSetVisibleItems: Can't find ID = %d", (*it));
            continue;
        }
        // ���� ������ ����� ����� � �� �� ���� ��������
        tmpItem->SetInvisibleRecursiveDown();
    }
    return res;
}

void MenuTree::updateSubscriptById(const MENU_ID id, const std::string &subscript) const
{
    if (root_item != nullptr)
    {
        MenuItem *_item = root_item->FindItmById(id);
        if (_item != nullptr)
        {
            _item->setSubscript(subscript);
        }
    }
}

//========================================================================================
// �஢�ઠ �� ����稥 ������� ��⮬��� � ⥪. �-�
bool MenuTree::IsEnd()
{
    for(unsigned int i=0; i < cur_item->GetChildVect().size(); i++)
    {
        if(cur_item->GetChildVect().at(i)->GetVisible() == true)
            {return false;}
    }
    return true;
}


//========================================================================================
// �஢�ઠ �� ��������� ����� �� ���室� �� ������� [0 .. N-1]
bool MenuTree::IsSubMnuItmVisbl(unsigned int index)
{
    if(cur_item->GetChildVect().size() <= index  )
    {
        return false;
    }
    return cur_item->GetChildVect().at(index)->GetVisible();
}

bool MenuTree::isSubMenuItemActive(const unsigned int index)
{
    if ((cur_item == nullptr) || (cur_item->GetChildVect().size() <= index))
    {
        return false;
    }

    MenuItem *menuItem = cur_item->GetChildVect().at(index);
    if (menuItem == nullptr)
    {
        logERR("%s menuItem == nullptr", __FUNCTION__);
        return false;
    }

    return getActive(menuItem);
}

//========================================================================================
// ������⢮ ������� ��⮬��� � ⥪. �-�
unsigned int MenuTree::NumOfChilds()
{
    unsigned int num = 0;
    vector <MenuItem*> clds = cur_item->GetChildVect();
    for(unsigned int i=0; i< clds.size(); i++)
    {
        if(clds.at(i)->GetVisible() == true) {num++;}
    }
    return num;
}
