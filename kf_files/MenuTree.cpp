//
// Created on 02.08.17.
//

#include "MenuTree.h"


//========================================================================================
//========================================================================================
//============                      КЛАСС MenuItem               =========================
//========================================================================================
//========================================================================================
// 1 Конструктор
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
// Деструктор, рекурсивное удаление всей ветки ниже, с последующим удалением указателя на себя из списка детей родителя
MenuItem::~MenuItem()
{
    while( !childVect.empty() )
    {
        delete  childVect.back();
    }
    // Удаление из вектора parent себя, если этот эл-т не корень
    if(GetParent() != NULL)
    {
        vector<MenuItem*>::iterator it=find(GetParent()->GetChildVect().begin(), GetParent()->GetChildVect().end(), this);
        GetParent()->GetChildVect().erase(it);
    }
}


//========================================================================================
// Рекурсивный поиск итема в поддереве по ID
MenuItem* MenuItem::FindItmById(MENU_ID searchingID)
    {
    // Если этот элемент и есть искомый, то возвращаем указатель на него
    if (this->GetID() == searchingID)
    {
        return this;
    }
    // Если это не тот элемент, и это конец, то возвращаем NULL(тупик)
    if(this->GetChildVect().empty())
    {
        return NULL;
    }
    // Если у этого элемента есть потомки, то рекурсивно ищем в них
    for(unsigned int i=0; i<GetChildVect().size(); i++)
    {
        MenuItem* iterItm = GetChildVect().at(i)->FindItmById(searchingID);
        // Если Элемент нашелся, то возращаем указатель
        if( iterItm != NULL)
        {
            return iterItm;
        }
    }
    // Если Элемент НЕ нашелся, то возращаем NULL
    return NULL;
    }
//========================================================================================
// Добавить один эл-т подменю    ПРОВЕРКА ПО ID????????
bool MenuItem::AddChild(MenuItem *newChild)
{
    if( newChild->GetName().empty() )
        {return false;}
    childVect.push_back(newChild);
    newChild->SetParent(this);
    return true;
}


//========================================================================================
// Получение номера в дереве формата "1 ". Если более 10 эл-тов, то "1  " (для дальнейшего доавления к имени)
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
// Получение уровня глубины элемента в дереве
int MenuItem::GetLvlOfItm()
{
    MenuItem *curItem;
    int lvl=0;
    curItem = this;
    while (curItem->GetParent() != NULL || lvl < 99) // 99 - резервный выход
    {
        lvl++;
        curItem = curItem->GetParent();
    }//while
    return lvl;
}


//========================================================================================
// Получение порядкового номера среди Видимых братьев, 1...
unsigned int MenuItem::GetIndexOfItm()
{
    // Если мы на вершине дерева, то false
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
// Сделать элемент видимым вплоть до корня
void MenuItem::SetVisibleRecursiveUp()
{
    // Делаем видимым тек. элемент
    this->SetVisible(true);
    // Если не корень, то рекурсивно делаем видимым и родителя
    if( this->GetParent() != NULL )
    { this->GetParent()->SetVisibleRecursiveUp(); }
}


//========================================================================================
// Сделать видимым себя и всех потомков. Нужна для оптимизации обхода дерева
void MenuItem::SetVisibleRecursiveDown()
{
    // Делаем невидимым тек. элемент
    this->visible = true;
    // Если есть потомки, то их тоже делаем рекурсивно невидимыми
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
// Сделать НЕвидимым себя и всех потомков
void MenuItem::SetInvisibleRecursiveDown()
{
    // Делаем невидимым тек. элемент
    this->visible = false;
    // Если есть потомки, то их тоже делаем рекурсивно невидимыми
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
// Копирование всех параметров за исключением списка детей, чтобы не замыкать дерево
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
// Вывод в cout полей экземпляра (Для отладки)
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
//============                      КЛАСС MenuTree               =========================
//========================================================================================
//========================================================================================
// Подняться вверх по дереву меню
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
// Перейти в подменю по ID
bool MenuTree::GoDown(MENU_ID subMenuID)
{
    // Если мы в конечном элементе дерева, то false
    if (cur_item->GetChildVect().empty())
    {
        return false;
    }
    // Ищем совпадение
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
    // Если совпадения не найдено
    return false;
    }


//========================================================================================
// Перейти в подменю по полному имени (строка с дисплея, те с номером)
bool MenuTree::GoDown(string fullName)
{
    // Если мы в конечном элементе дерева, то false
    if (cur_item->GetChildVect().empty())
    {
        return false;
    }
    // Ищем совпадение по параметру
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
    // Если совпадения не найдено
    return false;
}


//========================================================================================
// Перейти к элементу справа(тот же уровень)
bool MenuTree::GoRight()
{
    MenuItem *newItem;
    int itmIndex=-1;    // Индекс потенциального места для перехода
    itmIndex = cur_item->GetIndexOfItm();
    if (itmIndex < 0)
        {return false;}
    itmIndex--; // тк отсчет от 0 а не от 1
    if (itmIndex >= int(cur_item->GetParent()->GetChildVect().size()))
    {
        return false;
    }
    newItem = cur_item->GetParent()->GetChildVect().at(itmIndex + 1);
    cur_item = newItem;
    return true;
}


//========================================================================================
// Перейти к элементу справа(тот же уровень)
bool MenuTree::GoLeft()
{
    MenuItem *newItem;
    int itmIndex=-1;    // Индекс потенциального места для перехода
    itmIndex = cur_item->GetIndexOfItm();
    if (itmIndex < 0)
        {return false;}
    itmIndex--; // тк отсчет от 0 а не от 1
    if (itmIndex <= 0)
    {
        return false;
    }
    newItem = cur_item->GetParent()->GetChildVect().at(itmIndex - 1);
    cur_item = newItem;
    return true;
}


//========================================================================================
// Получить ID подменю по РЕАЛЬНОМУ индексу, те не взирая на видимость
MENU_ID MenuTree::GetSubMenuIDByIndex( int index)
{
    if (cur_item->GetChildVect().empty() || cur_item->GetChildVect().size() <= (unsigned int)index)
    {
        return INVALID_ID;
    }
    return ( cur_item->GetChildVect().at(index)->GetID()    ) ;

}

//========================================================================================
// Получить список имен(с номерами) всех подменю, но только Видимых!
vector<std::string> MenuTree::GetSubMenuNames()
{
    vector<string> vectorNames;
    vectorNames.clear();
    // Если нет детей(подменю)
    if (cur_item->GetChildVect().empty())
    {
        vectorNames.push_back("NO ITEMS");
        return vectorNames;
    }
    for(vector<MenuItem*>::iterator it = cur_item->GetChildVect().begin();
        it != cur_item->GetChildVect().end();
        it++)
    {
        // Если элемент Видимый, то помещаем его в список
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
    // Если нет детей(подменю)
    if (cur_item->GetChildVect().empty())
    {
        vNameAndActive.push_back({"NO ITEMS", "", true});
        return vNameAndActive;
    }

    for (const auto &item : cur_item->GetChildVect())
    {
        // Если элемент Видимый, то помещаем его в список
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
// Получить список id всех подменю, но только Видимых!
vector<MENU_ID> MenuTree::GetSubMenuIdList()
{
    vector<MENU_ID> vectorId;
    vectorId.clear();

    // Если нет детей(подменю)
    if (cur_item->GetChildVect().empty())
    {
        return vectorId;
    }

    for(const auto &item : cur_item->GetChildVect())
    {
        // Если элемент Видимый, то помещаем его в список
        if(item->GetVisible())
        {
            vectorId.push_back( item->GetID() );
        }
    }
    return vectorId;
}

//========================================================================================
// Добавить новый элемент подменю
bool MenuTree::AddSubMenu(MenuItem* newItem)
{
    return cur_item->AddChild(newItem);
}

//========================================================================================
// Создать новый элемент подменю
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
// Сделать Видимыми эл-ты с данными ID, return false - не все ID сделаны видимыми (не все найдены)
bool MenuTree::TreeSetVisibleItems( vector<MENU_ID> vID)
{
    MenuItem *tmpItem;
    bool res = true;
    // Идем по списку vID
    for (vector<MENU_ID>::iterator it = vID.begin();
         it != vID.end();
         it++)
    {
        tmpItem = root_item->FindItmById(*it);
        // Если элемент с таким именем не найден, то переходим к след из списка vID
        if(tmpItem == NULL)
            {
                res = false;
                logDBG("MenuTree::TreeSetVisibleItems: Can't find ID = %d", (*it));
                continue;
            }
        // Иначе делаем данный элемент видимым до корня
        tmpItem->SetVisibleRecursiveUp();
    }
    return res;
}

/*
//========================================================================================
// Получение порядкового номера тек поз-и в меню 1...N
int MenuTree::GetCurIndexVisbl()
{
    int counter = 0;
    for(vector<MenuItem*>::iterator it = cur_item->GetChildVect().begin();
        it != cur_item->GetChildVect().end();
        it++)
    {
        // Если элемент Видимый, то помещаем его в список
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
// Сделать Видимыми эл-ты с данными ID, return false - не все ID сделаны видимыми (не все найдены)
bool MenuTree::TreeSetInvisibleItems( vector<MENU_ID> vID)
{
    MenuItem *tmpItem;
    bool res = true;
    // Идем по списку vID
    for (vector<MENU_ID>::iterator it = vID.begin();
         it != vID.end();
         it++)
    {
        tmpItem = root_item->FindItmById(*it);
        // Если элемент с таким именем не найден, то переходим к след из списка vID
        if(tmpItem == NULL)
        {
            res = false;
            logDBG("MenuTree::TreeSetVisibleItems: Can't find ID = %d", (*it));
            continue;
        }
        // Иначе делаем данный элемент и все что ниже невидимым
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
// Проверка на наличие ВИДИМЫХ потомков у тек. эл-та
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
// Проверка на видимость элемента при переходе по индексу [0 .. N-1]
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
// Количество видимых потомков у тек. эл-та
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
