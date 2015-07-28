#include "exception.h"

Exception::Exception(QString pTitle, QString pDescription) : mTitle(pTitle), mDescription(pDescription)
{
}

Exception::~Exception()
{
}

void Exception::raise() const
{
	throw *this;
}

QException * Exception::clone() const
{
	return new Exception(mTitle, mDescription);
}
