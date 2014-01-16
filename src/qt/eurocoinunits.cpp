#include "ceurounits.h"

#include <QStringList>

CEuroUnits::CEuroUnits(QObject *parent):
        QAbstractListModel(parent),
        unitlist(availableUnits())
{
}

QList<CEuroUnits::Unit> CEuroUnits::availableUnits()
{
    QList<CEuroUnits::Unit> unitlist;
    unitlist.append(CEU);
    unitlist.append(mCEU);
    unitlist.append(uCEU);
    return unitlist;
}

bool CEuroUnits::valid(int unit)
{
    switch(unit)
    {
    case CEU:
    case mCEU:
    case uCEU:
        return true;
    default:
        return false;
    }
}

QString CEuroUnits::name(int unit)
{
    switch(unit)
    {
    case CEU: return QString("CEU");
    case mCEU: return QString("mCEU");
    case uCEU: return QString::fromUtf8("μCEU");
    default: return QString("???");
    }
}

QString CEuroUnits::description(int unit)
{
    switch(unit)
    {
    case CEU: return QString("CryptoEURO");
    case mCEU: return QString("Milli-CryptoEURO (1 / 1,000)");
    case uCEU: return QString("Micro-CryptoEURO (1 / 1,000,000)");
    default: return QString("???");
    }
}

mpq CEuroUnits::factor(int unit)
{
    switch(unit)
    {
    case uCEU: return mpq("100/1");
    case mCEU: return mpq("100000/1");
    default:
    case CEU:  return mpq("100000000/1");
    }
}

int CEuroUnits::amountDigits(int unit)
{
    switch(unit)
    {
    case CEU: return 8; // <100,000,000 (# digits, without commas)
    case mCEU: return 11; // <100,000,000,000
    case uCEU: return 14; // <100,000,000,000,000
    default: return 0;
    }
}

int CEuroUnits::decimals(int unit)
{
    switch(unit)
    {
    case CEU: return 8;
    case mCEU: return 5;
    case uCEU: return 2;
    default: return 0;
    }
}

QString CEuroUnits::format(int unit, const mpq& n, bool fPlus)
{
    // Note: not using straight sprintf here because we do NOT want
    // localized number formatting.
    if(!valid(unit))
        return QString(); // Refuse to format invalid unit
    mpq q = n * COIN / factor(unit);
    std::string str = FormatMoney(q, fPlus);
    int diff = 8 - decimals(unit);
    if(diff > 0)
        str.erase(str.length() - diff, diff);
    return QString::fromStdString(str);
}

QString CEuroUnits::formatWithUnit(int unit, const mpq& amount, bool plussign)
{
    return format(unit, amount, plussign) + QString(" ") + name(unit);
}

bool CEuroUnits::parse(int unit, const QString &value, mpq *val_out)
{
    mpq ret_value;
    if (!ParseMoney(value.toStdString(), ret_value))
        return false;
    if(val_out)
    {
        *val_out = ret_value * factor(unit) / COIN;
    }
    return true;
}

int CEuroUnits::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return unitlist.size();
}

QVariant CEuroUnits::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row >= 0 && row < unitlist.size())
    {
        Unit unit = unitlist.at(row);
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return QVariant(name(unit));
        case Qt::ToolTipRole:
            return QVariant(description(unit));
        case UnitRole:
            return QVariant(static_cast<int>(unit));
        }
    }
    return QVariant();
}
