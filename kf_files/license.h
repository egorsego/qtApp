#ifndef LICENCE_H_
#define LICENCE_H_

#include <string>
#include <vector>

#include "loggerdaemon.h"
#include "convert.h"

using namespace std;

/**
 * @brief The License class - класс для обработки платных услуг
 */
class License
{
public:
    const int64_t DATE_OVERDUE_SERVICES = 30 * Convert::TIME_HOUR_IN_DAY * Convert::TIME_MIN_IN_HOUR * Convert::TIME_SEC_IN_MIN;

    /**
     * @brief The LOCAL_LICENSE_TAG enum тэги локальных фич(если получены то остаются)
     */
    enum class LOCAL_LICENSE_TAG
    {
        FFD_1_1   = 0x01,
        SELL_SIZ,
        SELL_ALCOHOL,    // продажа алкогольной продукции
        TOBACCO_MRC      // продажа табака по МРЦ
    };

    enum LICENSE_TAG
    {
        LICENSE_TAG_NDS_20_120   = 0x01,
        LICENSE_TAG_FFD_1_05     = 0x03,
        LICENSE_TAG_TOBACCO      = 0x04,
        LICENSE_TAG_SUBSCRIPTION = 0x08     // подписка Дримкас ключ
    };
    static const int16_t ACTIVE = 0x03;
    /**
     * @brief The LicenseTag struct - структура для одного тэга
     */
    struct LicenseTag
    {
        LicenseTag() = default;

        LicenseTag(int16_t    newTagNum,
                   int16_t      newMask,
                   string       newName,
                   string newExpireDate)
        {
            tagNum     =     newTagNum;
            mask       =       newMask;
            name       =       newName;
            expireDate = newExpireDate;
        }

        int16_t    tagNum = -1;
        int16_t      mask = -1;
        string       name;
        string expireDate;

        int64_t expireDateSec = 0;
    };

    License()
    {
        _convert.reset(new Convert);
    }

    License(const License &obj)
    {
        _keyActivation = obj._keyActivation;
        _licenseList   = obj._licenseList;
    }

    /**
     * @brief clear - очистка списка услуг
     */
    inline void clear() { _licenseList.clear(); }

    /**
     * @brief size - получить размер списка услуг
     * @return количество услуг
     */
    inline size_t size() const { return _licenseList.size(); }

    /**
     * @brief empty - проверка вектора на пустоту
     * @return
     */
    inline bool empty() const { return _licenseList.empty(); }

    /**
     * @brief isTagExistAndValid - проверка наличия тэга в векторе
     * @param tagNumber - индекс тэга
     * @return true если тэг найден, иначе false
     */
    virtual bool isTagExistAndValid( const int16_t &tagNumber)
    {
        for(auto &lic : _licenseList)
        {
            if(lic.tagNum == tagNumber) // Такой тэг есть в активационном ключе
            {
                if(lic.mask != ACTIVE )
                {
                    logDBG("%s:: Данная фича есть в ключе, но не активна! mask = %d", __PRETTY_FUNCTION__, lic.mask);
                    return false;
                }
                return true;
            }
        }
        return false;
    }

    /**
     * @brief showLicense - вывод всех услуг в лог и консоль
     */
    void showLicense()
    {
        logWARN("=========== Лицензия ===========");
        for(const auto& it : _licenseList)
        {
            string date = it.expireDate.empty() ? "Бессрочно" : it.expireDate;
            logWARN( "%u)\t mask = %u,\t expire date = '%s',\t name = '%s'",
                    it.tagNum,
                    it.mask,
                    date   .c_str(),
                    it.name.c_str() );
        }
        logWARN("======== Активные опции ========");
        if( isLocalLicExist(License::LOCAL_LICENSE_TAG::FFD_1_1) )
        {
            logWARN("ФФД 1.1");
        }
        if( isLocalLicExist(License::LOCAL_LICENSE_TAG::SELL_SIZ) )
        {
            logWARN("КТ для СИЗ");
        }
        if( isLocalLicExist(License::LOCAL_LICENSE_TAG::SELL_ALCOHOL) )
        {
            logWARN("Продажа алкогольной продукции");
        }
        if( isLocalLicExist(License::LOCAL_LICENSE_TAG::TOBACCO_MRC) )
        {
            logWARN("Продажа табака по МРЦ");
        }
        logWARN("================================");
    }

    /**
     * @brief addLocalTag - добавить локальный тэг(остается при окончании ключа)
     * @param llt - новый активный локальный тэг(опция)
     */
    void addLocalTag( const License::LOCAL_LICENSE_TAG llt )
    {
        // if FFD1.1 ON in mask and not added yet
        if( !isLocalLicExist(llt) )
        {
            _localLicenseList.push_back(llt);
        }
    }

    /**
     * @brief addTag - добавить тэг
     * @param newLic значение нового тэга
     */
    void addTag(LicenseTag newLic)
    {
        for(const auto& it : _licenseList)
        {
            if(it.tagNum == newLic.tagNum)
            {
                return;
            }
        }
        _licenseList.push_back(newLic);
    }

    /**
     * @brief getTag - получить тэг по индексу
     * @param tagNumber - индекс тэга
     * @param licenseTag - ссылка на структуру для сохранения тэга
     * @return true если тэг есть, иначе false
     */
    bool getTag(const int16_t &tagNumber, LicenseTag &licenseTag)
    {
        for(LicenseTag &license : _licenseList)
        {
            if(license.tagNum == tagNumber)
            {
                licenseTag = license;
                return true;
            }
        }
        logERR("%s:: Такого тега в структуре лицензии нет: №%d", __PRETTY_FUNCTION__, tagNumber);
        return false;
    }

    /**
     * @brief getLicList - получить копию списка тэгов
     * @return список тэгов
     */
    inline vector<LicenseTag> getLicList() const { return _licenseList; }

    /**
     * @brief Статусы лицензионных тегов
     */
    enum LICENSE_TAG_STATUS
    {
        LICENSE_TAG_STATUS_ERROR   = -1,
        LICENSE_TAG_STATUS_DISABLE =  0,
        LICENSE_TAG_STATUS_ENABLE  =  3,
        LICENSE_TAG_STATUS_EXPIRED =  1
    };


    /**
     * @brief Получение тегов c определенным статусом
     * @param[in] status статус
     * @param[out] licenseTag вектор с результатом
     * @return true если вектор не пустой
     */
    bool getTagsByStatus(vector<LicenseTag> &licenseTag, int16_t status)
    {
        licenseTag.clear();

        for(const LicenseTag &license : _licenseList)
        {
            if (license.mask == status)
            {
                licenseTag.push_back(license);
            }
        }

        return !(licenseTag.empty());
    }

    virtual string daysLeft( LicenseTag &licenseTag )
    {
        if(!_convert)
        {
            logERR("%s Null ptr!", __PRETTY_FUNCTION__);
            return "";
        }
        string strExpireDays = licenseTag.expireDate;
        strExpireDays.erase(remove(strExpireDays.begin(), strExpireDays.end(), '.'), strExpireDays.end());
        strExpireDays += "0000";

        _convert->dateLessSelected(_convert->getStringCurTime(), strExpireDays, DATE_OVERDUE_SERVICES, licenseTag.expireDateSec);
        logERR("%s Expire date seconds : %lld", __PRETTY_FUNCTION__, licenseTag.expireDateSec);
        if(licenseTag.expireDateSec >= DATE_OVERDUE_SERVICES)
        {
            return "до " + licenseTag.expireDate;
        }

        return _convert->getStringExpireDays(licenseTag.expireDateSec);
    }

    /**
     * @brief isLocalLicExist - есть ли локальная лицензия(локальный тэг) на ККТ
     * @param licenseTag - finding Tag
     * @return  - exist/not exist
     */
    virtual bool isLocalLicExist( const LOCAL_LICENSE_TAG findTag )
    {
        std::vector <LOCAL_LICENSE_TAG>::iterator it;
        it = find (_localLicenseList.begin(), _localLicenseList.end(), findTag);
        return (it != _localLicenseList.end());
    }

private:
    string                         _keyActivation;
    vector <LicenseTag>              _licenseList;
    vector <LOCAL_LICENSE_TAG>  _localLicenseList;
    unique_ptr<Convert>                  _convert;
};

#endif
