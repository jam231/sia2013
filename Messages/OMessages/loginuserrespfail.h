#ifndef LOGINUSERRESPFAIL_H
#define LOGINUSERRESPFAIL_H


#include <QString>

#include "omessage.h"

class LoginUserRespFail : public OMessage
{
    QString m_reason;

    qint32 length() const;
public:
    LoginUserRespFail(QString);

    MessageType type() const;
    void send(QIODevice* connection);
};

#endif // LOGINUSERRESPFAIL_H