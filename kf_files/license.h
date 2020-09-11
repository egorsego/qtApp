#ifndef LICENCE_H_
#define LICENCE_H_

#include <string>
#include <vector>

#include "loggerdaemon.h"
#include "convert.h"

using namespace std;

/**
 * @brief The License class - ����� ��� ��ࠡ�⪨ ������ ���
 */
class License
{
public:
    const int64_t DATE_OVERDUE_SERVICES = 30 * Convert::TIME_HOUR_IN_DAY * Convert::TIME_MIN_IN_HOUR * Convert::TIME_SEC_IN_MIN;

    /**
     * @brief The LOCAL_LICENSE_TAG enum �� �������� ��(�᫨ ����祭� � �������)
     */
    enum class LOCAL_LICENSE_TAG
    {
        FFD_1_1   = 0x01,
        SELL_SIZ,
        SELL_ALCOHOL,    // �த��� �������쭮� �த�樨
        TOBACCO_MRC      // �த��� ⠡��� �� ���
    };

    enum LICENSE_TAG
    {
        LICENSE_TAG_NDS_20_120   = 0x01,
        LICENSE_TAG_FFD_1_05     = 0x03,
        LICENSE_TAG_TOBACCO      = 0x04,
        LICENSE_TAG_SUBSCRIPTION = 0x08     // �����᪠ �ਬ��� ����
    };
    static const int16_t ACTIVE = 0x03;
    /**
     * @brief The LicenseTag struct - ������� ��� ������ ��
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
     * @brief clear - ���⪠ ᯨ᪠ ���
     */
    inline void clear() { _licenseList.clear(); }

    /**
     * @brief size - ������� ࠧ��� ᯨ᪠ ���
     * @return ������⢮ ���
     */
    inline size_t size() const { return _licenseList.size(); }

    /**
     * @brief empty - �஢�ઠ ����� �� ������
     * @return
     */
    inline bool empty() const { return _licenseList.empty(); }

    /**
     * @brief isTagExistAndValid - �஢�ઠ ������ �� � �����
     * @param tagNumber - ������ ��
     * @return true �᫨ �� ������, ���� false
     */
    virtual bool isTagExistAndValid( const int16_t &tagNumber)
    {
        for(auto &lic : _licenseList)
        {
            if(lic.tagNum == tagNumber) // ����� �� ���� � ��⨢�樮���� ����
            {
                if(lic.mask != ACTIVE )
                {
                    logDBG("%s:: ������ �� ���� � ����, �� �� ��⨢��! mask = %d", __PRETTY_FUNCTION__, lic.mask);
                    return false;
                }
                return true;
            }
        }
        return false;
    }

    /**
     * @brief showLicense - �뢮� ��� ��� � ��� � ���᮫�
     */
    void showLicense()
    {
        logWARN("=========== ��業��� ===========");
        for(const auto& it : _licenseList)
        {
            string date = it.expireDate.empty() ? "�����筮" : it.expireDate;
            logWARN( "%u)\t mask = %u,\t expire date = '%s',\t name = '%s'",
                    it.tagNum,
                    it.mask,
                    date   .c_str(),
                    it.name.c_str() );
        }
        logWARN("======== ��⨢�� ��樨 ========");
        if( isLocalLicExist(License::LOCAL_LICENSE_TAG::FFD_1_1) )
        {
            logWARN("��� 1.1");
        }
        if( isLocalLicExist(License::LOCAL_LICENSE_TAG::SELL_SIZ) )
        {
            logWARN("�� ��� ���");
        }
        if( isLocalLicExist(License::LOCAL_LICENSE_TAG::SELL_ALCOHOL) )
        {
            logWARN("�த��� �������쭮� �த�樨");
        }
        if( isLocalLicExist(License::LOCAL_LICENSE_TAG::TOBACCO_MRC) )
        {
            logWARN("�த��� ⠡��� �� ���");
        }
        logWARN("================================");
    }

    /**
     * @brief addLocalTag - �������� ������� ��(��⠥��� �� ����砭�� ����)
     * @param llt - ���� ��⨢�� ������� ��(����)
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
     * @brief addTag - �������� ��
     * @param newLic ���祭�� ������ ��
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
     * @brief getTag - ������� �� �� �������
     * @param tagNumber - ������ ��
     * @param licenseTag - ��뫪� �� �������� ��� ��࠭���� ��
     * @return true �᫨ �� ����, ���� false
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
        logERR("%s:: ������ ⥣� � ������� ��業��� ���: �%d", __PRETTY_FUNCTION__, tagNumber);
        return false;
    }

    /**
     * @brief getLicList - ������� ����� ᯨ᪠ ���
     * @return ᯨ᮪ ���
     */
    inline vector<LicenseTag> getLicList() const { return _licenseList; }

    /**
     * @brief ������ ��業������� ⥣��
     */
    enum LICENSE_TAG_STATUS
    {
        LICENSE_TAG_STATUS_ERROR   = -1,
        LICENSE_TAG_STATUS_DISABLE =  0,
        LICENSE_TAG_STATUS_ENABLE  =  3,
        LICENSE_TAG_STATUS_EXPIRED =  1
    };


    /**
     * @brief ����祭�� ⥣�� c ��।������ ����ᮬ
     * @param[in] status �����
     * @param[out] licenseTag ����� � १���⮬
     * @return true �᫨ ����� �� ���⮩
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
            return "�� " + licenseTag.expireDate;
        }

        return _convert->getStringExpireDays(licenseTag.expireDateSec);
    }

    /**
     * @brief isLocalLicExist - ���� �� �����쭠� ��業���(������� ��) �� ���
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
