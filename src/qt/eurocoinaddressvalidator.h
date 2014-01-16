#ifndef EUROCOINADDRESSVALIDATOR_H
#define EUROCOINADDRESSVALIDATOR_H

#include <QRegExpValidator>

/** Base48 entry widget validator.
   Corrects near-miss characters and refuses characters that are no part of base48.
 */
class CEuroAddressValidator : public QValidator
{
    Q_OBJECT
public:
    explicit CEuroAddressValidator(QObject *parent = 0);

    State validate(QString &input, int &pos) const;

    static const int MaxAddressLength = 35;
signals:

public slots:

};

#endif // EUROCOINADDRESSVALIDATOR_H
