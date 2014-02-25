#ifndef COMPANYSTATUSMSG_H
#define COMPANYSTATUSMSG_H

#include "request.h"

namespace NetworkProtocol {
namespace Requests
{
class NETWORKPROTOCOLSHARED_EXPORT CompanyStatus : public Request
{
    DTO::Types::CompanyIdType m_companyId;

public:
    CompanyStatus(QDataStream& in);

    DTO::Types::Message::MessageLengthType length() const;
    DTO::Types::Message::MessageType type() const;
    DTO::Types::CompanyIdType getCompanyId() const;

    static const QString getMessageName();
};
}
}
#endif // COMPANYSTATUSMSG_H