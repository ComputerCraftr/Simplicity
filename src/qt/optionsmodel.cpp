// Copyright (c) 2011-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "optionsmodel.h"

#include "bitcoinunits.h"
#include "guiutil.h"

#include "init.h"
#include "main.h"
#include "net.h"
#ifdef ENABLE_WALLET
#include "wallet.h"
#include "walletdb.h"
#endif

#include <QNetworkProxy>
#include <QSettings>
#include <QStringList>

#ifdef USE_NATIVE_I2P
#include "i2p.h"
#include <sstream>

#define I2P_OPTIONS_SECTION_NAME    "I2P"

class ScopeGroupHelper
{
public:
    ScopeGroupHelper(QSettings& settings, const QString& groupName)
        : settings_(settings)
    {
        settings_.beginGroup(groupName);
    }
    ~ScopeGroupHelper()
    {
        settings_.endGroup();
    }

private:
    QSettings& settings_;
};

#endif

bool fUseBlackTheme;

OptionsModel::OptionsModel(QObject *parent) :
    QAbstractListModel(parent)
{
    Init();
}

void OptionsModel::addOverriddenOption(const std::string &option)
{
    strOverriddenByCommandLine += QString::fromStdString(option) + "=" + QString::fromStdString(mapArgs[option]) + " ";
}

#ifdef USE_NATIVE_I2P
std::string& FormatI2POptionsString(
        std::string& options,
        const std::string& name,
        const std::pair<bool, std::string>& value)
{
    if (value.first)
    {
        if (!options.empty())
            options += " ";
        options += name + "=" + value.second;
    }
    return options;
}

std::string& FormatI2POptionsString(
        std::string& options,
        const std::string& name,
        const std::pair<bool, bool>& value)
{
    if (value.first)
    {
        if (!options.empty())
            options += " ";
        options += name + "=" + (value.second ? "true" : "false");
    }
    return options;
}

std::string& FormatI2POptionsString(
        std::string& options,
        const std::string& name,
        const std::pair<bool, int>& value)
{
    if (value.first)
    {
        if (!options.empty())
            options += " ";
        std::ostringstream oss;
        oss << value.second;
        options += name + "=" + oss.str();
    }
    return options;
}
#endif

// Writes all missing QSettings with their default values
void OptionsModel::Init()
{
    QSettings settings;

    // Ensure restart flag is unset on client startup
    setRestartRequired(false);

    // These are Qt-only settings:

    // Window
    if (!settings.contains("fMinimizeToTray"))
        settings.setValue("fMinimizeToTray", false);
    fMinimizeToTray = settings.value("fMinimizeToTray").toBool();

    if (!settings.contains("fMinimizeOnClose"))
        settings.setValue("fMinimizeOnClose", false);
    fMinimizeOnClose = settings.value("fMinimizeOnClose").toBool();


    // Display
    if (!settings.contains("nDisplayUnit"))
        settings.setValue("nDisplayUnit", BitcoinUnits::BTC);
    nDisplayUnit = settings.value("nDisplayUnit").toInt();
    
    fUseBlackTheme = settings.value("fUseBlackTheme", false).toBool();
    
    if (!settings.contains("fCoinControlFeatures"))
        settings.setValue("fCoinControlFeatures", false);
    fCoinControlFeatures = settings.value("fCoinControlFeatures", false).toBool();

    // Dark Send
    if (!settings.contains("nDarksendRounds"))
        settings.setValue("nDarksendRounds", 2);
    nDarksendRounds = settings.value("nDarksendRounds").toLongLong();
    if (!settings.contains("nAnonymizeSimplicityAmount"))
        settings.setValue("nAnonymizeSimplicityAmount", 1000);
    nAnonymizeSimplicityAmount = settings.value("nAnonymizeSimplicityAmount").toLongLong();
    if (settings.contains("nDarksendRounds"))
        SoftSetArg("-darksendrounds", settings.value("nDarksendRounds").toString().toStdString());
    if (settings.contains("nAnonymizeSimplicityAmount"))
        SoftSetArg("-anonymizetraveamount", settings.value("nAnonymizeSimplicityAmount").toString().toStdString());

#ifdef USE_NATIVE_I2P
    ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);

    if (settings.value("useI2POnly", false).toBool())
    {
        mapArgs["-onlynet"] = NATIVE_I2P_NET_STRING;
        std::vector<std::string>& onlyNets = mapMultiArgs["-onlynet"];
        if (std::find(onlyNets.begin(), onlyNets.end(), NATIVE_I2P_NET_STRING) == onlyNets.end())
            onlyNets.push_back(NATIVE_I2P_NET_STRING);
    }

    if (settings.contains("samhost"))
        SoftSetArg(I2P_SAM_HOST_PARAM, settings.value("samhost").toString().toStdString());

    if (settings.contains("samport"))
        SoftSetArg(I2P_SAM_PORT_PARAM, settings.value("samport").toString().toStdString());

    if (settings.contains("sessionName"))
        SoftSetArg(I2P_SESSION_NAME_PARAM, settings.value("sessionName").toString().toStdString());

    i2pInboundQuantity        = settings.value(SAM_NAME_INBOUND_QUANTITY       , SAM_DEFAULT_INBOUND_QUANTITY       ).toInt();
    i2pInboundLength          = settings.value(SAM_NAME_INBOUND_LENGTH         , SAM_DEFAULT_INBOUND_LENGTH         ).toInt();
    i2pInboundLengthVariance  = settings.value(SAM_NAME_INBOUND_LENGTHVARIANCE , SAM_DEFAULT_INBOUND_LENGTHVARIANCE ).toInt();
    i2pInboundBackupQuantity  = settings.value(SAM_NAME_INBOUND_BACKUPQUANTITY , SAM_DEFAULT_INBOUND_BACKUPQUANTITY ).toInt();
    i2pInboundAllowZeroHop    = settings.value(SAM_NAME_INBOUND_ALLOWZEROHOP   , SAM_DEFAULT_INBOUND_ALLOWZEROHOP   ).toBool();
    i2pInboundIPRestriction   = settings.value(SAM_NAME_INBOUND_IPRESTRICTION  , SAM_DEFAULT_INBOUND_IPRESTRICTION  ).toInt();
    i2pOutboundQuantity       = settings.value(SAM_NAME_OUTBOUND_QUANTITY      , SAM_DEFAULT_OUTBOUND_QUANTITY      ).toInt();
    i2pOutboundLength         = settings.value(SAM_NAME_OUTBOUND_LENGTH        , SAM_DEFAULT_OUTBOUND_LENGTH        ).toInt();
    i2pOutboundLengthVariance = settings.value(SAM_NAME_OUTBOUND_LENGTHVARIANCE, SAM_DEFAULT_OUTBOUND_LENGTHVARIANCE).toInt();
    i2pOutboundBackupQuantity = settings.value(SAM_NAME_OUTBOUND_BACKUPQUANTITY, SAM_DEFAULT_OUTBOUND_BACKUPQUANTITY).toInt();
    i2pOutboundAllowZeroHop   = settings.value(SAM_NAME_OUTBOUND_ALLOWZEROHOP  , SAM_DEFAULT_OUTBOUND_ALLOWZEROHOP  ).toBool();
    i2pOutboundIPRestriction  = settings.value(SAM_NAME_OUTBOUND_IPRESTRICTION , SAM_DEFAULT_OUTBOUND_IPRESTRICTION ).toInt();
    i2pOutboundPriority       = settings.value(SAM_NAME_OUTBOUND_PRIORITY      , SAM_DEFAULT_OUTBOUND_PRIORITY      ).toInt();

    std::string i2pOptionsTemp;
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_INBOUND_QUANTITY       , std::make_pair(settings.contains(SAM_NAME_INBOUND_QUANTITY       ), i2pInboundQuantity));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_INBOUND_LENGTH         , std::make_pair(settings.contains(SAM_NAME_INBOUND_LENGTH         ), i2pInboundLength));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_INBOUND_LENGTHVARIANCE , std::make_pair(settings.contains(SAM_NAME_INBOUND_LENGTHVARIANCE ), i2pInboundLengthVariance));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_INBOUND_BACKUPQUANTITY , std::make_pair(settings.contains(SAM_NAME_INBOUND_BACKUPQUANTITY ), i2pInboundBackupQuantity));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_INBOUND_ALLOWZEROHOP   , std::make_pair(settings.contains(SAM_NAME_INBOUND_ALLOWZEROHOP   ), i2pInboundAllowZeroHop));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_INBOUND_IPRESTRICTION  , std::make_pair(settings.contains(SAM_NAME_INBOUND_IPRESTRICTION  ), i2pInboundIPRestriction));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_OUTBOUND_QUANTITY      , std::make_pair(settings.contains(SAM_NAME_OUTBOUND_QUANTITY      ), i2pOutboundQuantity));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_OUTBOUND_LENGTH        , std::make_pair(settings.contains(SAM_NAME_OUTBOUND_LENGTH        ), i2pOutboundLength));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_OUTBOUND_LENGTHVARIANCE, std::make_pair(settings.contains(SAM_NAME_OUTBOUND_LENGTHVARIANCE), i2pOutboundLengthVariance));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_OUTBOUND_BACKUPQUANTITY, std::make_pair(settings.contains(SAM_NAME_OUTBOUND_BACKUPQUANTITY), i2pOutboundBackupQuantity));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_OUTBOUND_ALLOWZEROHOP  , std::make_pair(settings.contains(SAM_NAME_OUTBOUND_ALLOWZEROHOP  ), i2pOutboundAllowZeroHop));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_OUTBOUND_IPRESTRICTION , std::make_pair(settings.contains(SAM_NAME_OUTBOUND_IPRESTRICTION ), i2pOutboundIPRestriction));
    FormatI2POptionsString(i2pOptionsTemp, SAM_NAME_OUTBOUND_PRIORITY      , std::make_pair(settings.contains(SAM_NAME_OUTBOUND_PRIORITY      ), i2pOutboundPriority));

    if (!i2pOptionsTemp.empty())
        SoftSetArg(I2P_SAM_I2P_OPTIONS_PARAM, i2pOptionsTemp);

    i2pOptions = QString::fromStdString(i2pOptionsTemp);
#endif

    // These are shared with the core or have a command-line parameter
    // and we want command-line parameters to overwrite the GUI settings.
    //
    // If setting doesn't exist create it with defaults.
    //
    // If SoftSetArg() or SoftSetBoolArg() return false we were overridden
    // by command-line and show this in the UI.
    // Wallet
#ifdef ENABLE_WALLET
    if (!settings.contains("nTransactionFee"))
        settings.setValue("nTransactionFee", (qint64)MIN_TX_FEE);
    nTransactionFee = settings.value("nTransactionFee").toLongLong(); // if -paytxfee is set, this will be overridden later in init.cpp
    if (mapArgs.count("-paytxfee"))
        addOverriddenOption("-paytxfee");
    nReserveBalance = settings.value("nReserveBalance").toLongLong();
#endif


    // Network
    if (!settings.contains("fUseUPnP"))
#ifdef USE_UPNP
        settings.setValue("fUseUPnP", true);
#else
        settings.setValue("fUseUPnP", false);
#endif
    if (!SoftSetBoolArg("-upnp", settings.value("fUseUPnP").toBool()))
        addOverriddenOption("-upnp");

    if (!settings.contains("fUseProxy"))
        settings.setValue("fUseProxy", false);
    if (!settings.contains("addrProxy"))
        settings.setValue("addrProxy", "127.0.0.1:9050");
    // Only try to set -proxy, if user has enabled fUseProxy
    if (settings.value("fUseProxy").toBool() && !SoftSetArg("-proxy", settings.value("addrProxy").toString().toStdString()))
        addOverriddenOption("-proxy");
    if (!settings.contains("nSocksVersion"))
        settings.setValue("nSocksVersion", 5);
    // Only try to set -socks, if user has enabled fUseProxy
    if (settings.value("fUseProxy").toBool() && !SoftSetArg("-socks", settings.value("nSocksVersion").toString().toStdString()))
        addOverriddenOption("-socks");

    // Display
    if (!settings.contains("language"))
        settings.setValue("language", "");
    if (!SoftSetArg("-lang", settings.value("language").toString().toStdString()))
        addOverriddenOption("-lang");




    language = settings.value("language").toString();
}

void OptionsModel::Reset()
{
    QSettings settings;

    // Remove all entries from our QSettings object
    settings.clear();

    // default setting for OptionsModel::StartAtStartup - disabled
    if (GUIUtil::GetStartOnSystemStartup())
        GUIUtil::SetStartOnSystemStartup(false);
}
int OptionsModel::rowCount(const QModelIndex & parent) const
{
    return OptionIDRowCount;
}

// read QSettings values and return them
QVariant OptionsModel::data(const QModelIndex & index, int role) const
{
    if(role == Qt::EditRole)
    {
        QSettings settings;
        switch(index.row())
        {
        case StartAtStartup:
            return GUIUtil::GetStartOnSystemStartup();
        case MinimizeToTray:
            return fMinimizeToTray;
        case MapPortUPnP:
#ifdef USE_UPNP
            return settings.value("fUseUPnP");
#else
            return false;
#endif
        case MinimizeOnClose:
            return fMinimizeOnClose;

        // default proxy
        case ProxyUse:
            return settings.value("fUseProxy", false);
        case ProxyIP: {
            // contains IP at index 0 and port at index 1
            QStringList strlIpPort = settings.value("addrProxy").toString().split(":", QString::SkipEmptyParts);
            return strlIpPort.at(0);
        }
        case ProxyPort: {
            // contains IP at index 0 and port at index 1
            QStringList strlIpPort = settings.value("addrProxy").toString().split(":", QString::SkipEmptyParts);
            return strlIpPort.at(1);
        }
        case ProxySocksVersion:
            return settings.value("nSocksVersion", 5);

#ifdef ENABLE_WALLET
        case Fee:
            // Attention: Init() is called before nTransactionFee is set in AppInit2()!
            // To ensure we can change the fee on-the-fly update our QSetting when
            // opening OptionsDialog, which queries Fee via the mapper.
            if (nTransactionFee != settings.value("nTransactionFee").toLongLong())
                settings.setValue("nTransactionFee", (qint64)nTransactionFee);
            // Todo: Consider to revert back to use just nTransactionFee here, if we don't want
            // -paytxfee to update our QSettings!
            return settings.value("nTransactionFee");            
        case ReserveBalance:
            return QVariant((qint64) nReserveBalance);
#endif
        case DisplayUnit:
            return nDisplayUnit;
        case Language:
            return settings.value("language");
        case CoinControlFeatures:
            return fCoinControlFeatures;
        case DarksendRounds:
            return QVariant(nDarksendRounds);
        case AnonymizeSimplicityAmount:
            return QVariant(nAnonymizeSimplicityAmount);
        case UseBlackTheme:
            return QVariant(fUseBlackTheme);
#ifdef USE_NATIVE_I2P
        case I2PUseI2POnly:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            bool useI2POnly = false;
            if (mapArgs.count("-onlynet"))
            {
                const std::vector<std::string>& onlyNets = mapMultiArgs["-onlynet"];
                if (std::find(onlyNets.begin(), onlyNets.end(), NATIVE_I2P_NET_STRING) != onlyNets.end())
                    useI2POnly = true;
            }
            return settings.value("useI2POnly", useI2POnly);
        }
        case I2PSAMHost:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            return settings.value("samhost", QString::fromStdString(GetArg(I2P_SAM_HOST_PARAM, I2P_SAM_HOST_DEFAULT)));
        }
        case I2PSAMPort:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            return settings.value("samport", QString::number((qint64)GetArg(I2P_SAM_PORT_PARAM, I2P_SAM_PORT_DEFAULT)));
        }
        case I2PSessionName:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            return settings.value("sessionName", QString::fromStdString(GetArg(I2P_SESSION_NAME_PARAM, I2P_SESSION_NAME_DEFAULT)));
        }
        case I2PInboundQuantity:
            return QVariant(i2pInboundQuantity);
        case I2PInboundLength:
            return QVariant(i2pInboundLength);
        case I2PInboundLengthVariance:
            return QVariant(i2pInboundLengthVariance);
        case I2PInboundBackupQuantity:
            return QVariant(i2pInboundBackupQuantity);
        case I2PInboundAllowZeroHop:
            return QVariant(i2pInboundAllowZeroHop);
        case I2PInboundIPRestriction:
            return QVariant(i2pInboundIPRestriction);
        case I2POutboundQuantity:
            return QVariant(i2pOutboundQuantity);
        case I2POutboundLength:
            return QVariant(i2pOutboundLength);
        case I2POutboundLengthVariance:
            return QVariant(i2pOutboundLengthVariance);
        case I2POutboundBackupQuantity:
            return QVariant(i2pOutboundBackupQuantity);
        case I2POutboundAllowZeroHop:
            return QVariant(i2pOutboundAllowZeroHop);
        case I2POutboundIPRestriction:
            return QVariant(i2pOutboundIPRestriction);
        case I2POutboundPriority:
            return QVariant(i2pOutboundPriority);
#endif
        default:
            return QVariant();
        }
    }
    return QVariant();
}

// write QSettings values
bool OptionsModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    bool successful = true; /* set to false on parse error */
    if(role == Qt::EditRole)
    {
        QSettings settings;
        switch(index.row())
        {
        case StartAtStartup:
            successful = GUIUtil::SetStartOnSystemStartup(value.toBool());
            break;
        case MinimizeToTray:
            fMinimizeToTray = value.toBool();
            settings.setValue("fMinimizeToTray", fMinimizeToTray);
            break;
        case MapPortUPnP: // core option - can be changed on-the-fly
            settings.setValue("fUseUPnP", value.toBool());
            MapPort(value.toBool());
            break;
        case MinimizeOnClose:
            fMinimizeOnClose = value.toBool();
            settings.setValue("fMinimizeOnClose", fMinimizeOnClose);
            break;

        // default proxy
        case ProxyUse:
            if (settings.value("fUseProxy") != value) {
                settings.setValue("fUseProxy", value.toBool());
                setRestartRequired(true);
            }
            break;
        case ProxyIP: {
            // contains current IP at index 0 and current port at index 1
            QStringList strlIpPort = settings.value("addrProxy").toString().split(":", QString::SkipEmptyParts);
            // if that key doesn't exist or has a changed IP
            if (!settings.contains("addrProxy") || strlIpPort.at(0) != value.toString()) {
                // construct new value from new IP and current port
                QString strNewValue = value.toString() + ":" + strlIpPort.at(1);
                settings.setValue("addrProxy", strNewValue);
                setRestartRequired(true);
            }
        }
        break;
        case ProxyPort: {
            // contains current IP at index 0 and current port at index 1
            QStringList strlIpPort = settings.value("addrProxy").toString().split(":", QString::SkipEmptyParts);
            // if that key doesn't exist or has a changed port
            if (!settings.contains("addrProxy") || strlIpPort.at(1) != value.toString()) {
                // construct new value from current IP and new port
                QString strNewValue = strlIpPort.at(0) + ":" + value.toString();
                settings.setValue("addrProxy", strNewValue);
                setRestartRequired(true);
            }
        }
        break;
        case ProxySocksVersion: {
            if (settings.value("nSocksVersion") != value) {
                settings.setValue("nSocksVersion", value.toInt());
                setRestartRequired(true);
            }
        }
        break;
#ifdef ENABLE_WALLET
        case Fee: // core option - can be changed on-the-fly
            // Todo: Add is valid check  and warn via message, if not
            nTransactionFee = value.toLongLong();
            settings.setValue("nTransactionFee", (qint64) nTransactionFee);
            emit transactionFeeChanged(nTransactionFee);
            break;
        case ReserveBalance:
            nReserveBalance = value.toLongLong();
            settings.setValue("nReserveBalance", (qint64) nReserveBalance);
            emit reserveBalanceChanged(nReserveBalance);
            break;
#endif
        case DisplayUnit:
            nDisplayUnit = value.toInt();
            settings.setValue("nDisplayUnit", nDisplayUnit);
            emit displayUnitChanged(nDisplayUnit);
            break;
        case Language:
            if (settings.value("language") != value) {
                settings.setValue("language", value);
                setRestartRequired(true);
            }
            break;
        case CoinControlFeatures:
            fCoinControlFeatures = value.toBool();
            settings.setValue("fCoinControlFeatures", fCoinControlFeatures);
            emit coinControlFeaturesChanged(fCoinControlFeatures);
            break;
        case UseBlackTheme:
            fUseBlackTheme = value.toBool();
            settings.setValue("fUseBlackTheme", fUseBlackTheme);
            break;
        case DarksendRounds:
            nDarksendRounds = value.toInt();
            settings.setValue("nDarksendRounds", nDarksendRounds);
            emit darksendRoundsChanged(nDarksendRounds);
            break;
        case AnonymizeSimplicityAmount:
            nAnonymizeSimplicityAmount = value.toInt();
            settings.setValue("nAnonymizeSimplicityAmount", nAnonymizeSimplicityAmount);
            emit AnonymizeSimplicityAmountChanged(nAnonymizeSimplicityAmount);
            break;
#ifdef USE_NATIVE_I2P
        case I2PUseI2POnly:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            settings.setValue("useI2POnly", value.toBool());
            break;
        }
        case I2PSAMHost:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            settings.setValue("samhost", value.toString());
            break;
        }
        case I2PSAMPort:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            settings.setValue("samport", value.toString());
            break;
        }
        case I2PSessionName:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            settings.setValue("sessionName", value.toString());
            break;
        }
        case I2PInboundQuantity:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pInboundQuantity = value.toInt();
            settings.setValue(SAM_NAME_INBOUND_QUANTITY, i2pInboundQuantity);
            break;
        }
        case I2PInboundLength:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pInboundLength = value.toInt();
            settings.setValue(SAM_NAME_INBOUND_LENGTH, i2pInboundLength);
            break;
        }
        case I2PInboundLengthVariance:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pInboundLengthVariance = value.toInt();
            settings.setValue(SAM_NAME_INBOUND_LENGTHVARIANCE, i2pInboundLengthVariance);
            break;
        }
        case I2PInboundBackupQuantity:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pInboundBackupQuantity = value.toInt();
            settings.setValue(SAM_NAME_INBOUND_BACKUPQUANTITY, i2pInboundBackupQuantity);
            break;
        }
        case I2PInboundAllowZeroHop:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pInboundAllowZeroHop = value.toBool();
            settings.setValue(SAM_NAME_INBOUND_ALLOWZEROHOP, i2pInboundAllowZeroHop);
            break;
        }
        case I2PInboundIPRestriction:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pInboundIPRestriction = value.toInt();
            settings.setValue(SAM_NAME_INBOUND_IPRESTRICTION, i2pInboundIPRestriction);
            break;
        }
        case I2POutboundQuantity:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pOutboundQuantity = value.toInt();
            settings.setValue(SAM_NAME_OUTBOUND_QUANTITY, i2pOutboundQuantity);
            break;
        }
        case I2POutboundLength:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pOutboundLength = value.toInt();
            settings.setValue(SAM_NAME_OUTBOUND_LENGTH, i2pOutboundLength);
            break;
        }
        case I2POutboundLengthVariance:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pOutboundLengthVariance = value.toInt();
            settings.setValue(SAM_NAME_OUTBOUND_LENGTHVARIANCE, i2pOutboundLengthVariance);
            break;
        }
        case I2POutboundBackupQuantity:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pOutboundBackupQuantity = value.toInt();
            settings.setValue(SAM_NAME_OUTBOUND_BACKUPQUANTITY, i2pOutboundBackupQuantity);
            break;
        }
        case I2POutboundAllowZeroHop:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pOutboundAllowZeroHop = value.toBool();
            settings.setValue(SAM_NAME_OUTBOUND_ALLOWZEROHOP, i2pOutboundAllowZeroHop);
            break;
        }
        case I2POutboundIPRestriction:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pOutboundIPRestriction = value.toInt();
            settings.setValue(SAM_NAME_OUTBOUND_IPRESTRICTION, i2pOutboundIPRestriction);
            break;
        }
        case I2POutboundPriority:
        {
            ScopeGroupHelper s(settings, I2P_OPTIONS_SECTION_NAME);
            i2pOutboundPriority = value.toInt();
            settings.setValue(SAM_NAME_OUTBOUND_PRIORITY, i2pOutboundPriority);
            break;
        }

#endif
        default:
            break;
        }
    }
    emit dataChanged(index, index);

    return successful;
}

bool OptionsModel::getProxySettings(QNetworkProxy& proxy) const
{
    // Directly query current base proxy, because
    // GUI settings can be overridden with -proxy.
    proxyType curProxy;
    if (GetProxy(NET_IPV4, curProxy)) {
        if (curProxy.second == 5) {
            proxy.setType(QNetworkProxy::Socks5Proxy);
            proxy.setHostName(QString::fromStdString(curProxy.first.ToStringIP()));
            proxy.setPort(curProxy.first.GetPort());

            return true;
        }
        else
            return false;
    }
    else
        proxy.setType(QNetworkProxy::NoProxy);






    return true;
}

void OptionsModel::setRestartRequired(bool fRequired)
{
    QSettings settings;
    return settings.setValue("fRestartRequired", fRequired);
}

bool OptionsModel::isRestartRequired()
{
    QSettings settings;
    return settings.value("fRestartRequired", false).toBool();
}


qint64 OptionsModel::getReserveBalance()
{
    return nReserveBalance;
}
