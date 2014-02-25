#ifndef REQUEST_H
#define REQUEST_H

#include "message.h"

#include <QIODevice>
#include <QDataStream>

#include <exception>

namespace NetworkProtocol
{
namespace Requests
{

class NETWORKPROTOCOLSHARED_EXPORT InvalidRequest : public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "Invalid data in request.";
    }
};


class NETWORKPROTOCOLSHARED_EXPORT Request : public Message
{
protected:
    Request(QDataStream& in);
    Request();
    void validateRequest(QDataStream& in);
public:
    static DTO::Types::Message::MessageType getType(QDataStream &in);
    static DTO::Types::Message::MessageLengthType getMessageLength(QIODevice* data);


    virtual ~Request() {}
};
}
}
#endif // REQUEST_H
