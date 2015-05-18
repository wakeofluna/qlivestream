#include "config.h"
#include "logger.h"

Logger * Logger::mInstance = nullptr;

Logger::Logger()
{
	Q_ASSERT_X(mInstance == nullptr, "Logger::Logger()", "Logger singleton already initialised");
	mInstance = this;
}

Logger::~Logger()
{
	if (mInstance == this)
		mInstance = nullptr;
}

Logger * Logger::get()
{
	return mInstance;
}

QWidget * Logger::networkCaptureWindow() const
{
	return nullptr;
}

void Logger::logNetworkMessage(QString pTag, QVariant const & pMessage)
{
}

void Logger::logNetworkError(QString pTag, QString const & pMessage)
{
}
