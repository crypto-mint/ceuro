#include "ceuroamountfield.h"
#include "qvaluecombobox.h"
#include "ceurounits.h"

#include "guiconstants.h"
#include "main.h"
#include "util.h"

#include <QLabel>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QApplication>
#include <qmath.h>

CEuroAmountField::CEuroAmountField(QWidget *parent):
        QWidget(parent), amount(0), currentUnit(-1)
{
    amount = new QDoubleSpinBox(this);
    amount->setLocale(QLocale::c());
    amount->setDecimals(8);
    amount->installEventFilter(this);
    amount->setMaximumWidth(170);
    amount->setSingleStep(0.001);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(amount);
    unit = new QValueComboBox(this);
    unit->setModel(new CEuroUnits(this));
    layout->addWidget(unit);
    layout->addStretch(1);
    layout->setContentsMargins(0,0,0,0);

    setLayout(layout);

    setFocusPolicy(Qt::TabFocus);
    setFocusProxy(amount);

    // If one if the widgets changes, the combined content changes as well
    connect(amount, SIGNAL(valueChanged(QString)), this, SIGNAL(textChanged()));
    connect(unit, SIGNAL(currentIndexChanged(int)), this, SLOT(unitChanged(int)));

    // Set default based on configuration
    unitChanged(unit->currentIndex());
}

void CEuroAmountField::setText(const QString &text)
{
    if (text.isEmpty())
        amount->clear();
    else
        amount->setValue(text.toDouble());
}

void CEuroAmountField::clear()
{
    amount->clear();
    unit->setCurrentIndex(0);
}

bool CEuroAmountField::validate()
{
    mpq value = 0; bool valid = false;
    if (CEuroUnits::parse(currentUnit, text(), &value) && MoneyRange(value))
        valid = true;

    setValid(valid);

    return valid;
}

void CEuroAmountField::setValid(bool valid)
{
    if (valid)
        amount->setStyleSheet("");
    else
        amount->setStyleSheet(STYLE_INVALID);
}

QString CEuroAmountField::text() const
{
    if (amount->text().isEmpty())
        return QString();
    else
        return amount->text();
}

bool CEuroAmountField::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        // Clear invalid flag on focus
        setValid(true);
    }
    else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Comma)
        {
            // Translate a comma into a period
            QKeyEvent periodKeyEvent(event->type(), Qt::Key_Period, keyEvent->modifiers(), ".", keyEvent->isAutoRepeat(), keyEvent->count());
            qApp->sendEvent(object, &periodKeyEvent);
            return true;
        }
    }
    return QWidget::eventFilter(object, event);
}

QWidget *CEuroAmountField::setupTabChain(QWidget *prev)
{
    QWidget::setTabOrder(prev, amount);
    return amount;
}

qint64 CEuroAmountField::value(bool *valid_out) const
{
    mpq qValue = valueAsMpq(valid_out);
    mpz zValue = qValue.get_num() / qValue.get_den();
    return mpz_to_i64(zValue);
}

void CEuroAmountField::setValue(qint64 value)
{
    setValue(i64_to_mpq(value));
}

mpq CEuroAmountField::valueAsMpq(bool *valid_out) const
{
    mpq val_out = 0;
    bool valid = CEuroUnits::parse(currentUnit, text(), &val_out);
    if(valid_out)
    {
        *valid_out = valid;
    }
    return val_out;
}

void CEuroAmountField::setValue(const mpq& value)
{
    setText(CEuroUnits::format(currentUnit, RoundAbsolute(value, ROUND_TOWARDS_ZERO)));
}

void CEuroAmountField::unitChanged(int idx)
{
    // Use description tooltip for current unit for the combobox
    unit->setToolTip(unit->itemData(idx, Qt::ToolTipRole).toString());

    // Determine new unit ID
    int newUnit = unit->itemData(idx, CEuroUnits::UnitRole).toInt();

    // Parse current value and convert to new unit
    bool valid = false;
    mpq currentValue = valueAsMpq(&valid);

    currentUnit = newUnit;

    // Set max length after retrieving the value, to prevent truncation
    amount->setDecimals(CEuroUnits::decimals(currentUnit));
    amount->setMaximum(qPow(10, CEuroUnits::amountDigits(currentUnit)) - qPow(10, -amount->decimals()));

    if(CEuroUnits::decimals(currentUnit)<3)
        amount->setSingleStep(0.01);
    else
        amount->setSingleStep(0.001);

    if(valid)
    {
        // If value was valid, re-place it in the widget with the new unit
        setValue(currentValue);
    }
    else
    {
        // If current value is invalid, just clear field
        setText("");
    }
    setValid(true);
}

void CEuroAmountField::setDisplayUnit(int newUnit)
{
    unit->setValue(newUnit);
}
