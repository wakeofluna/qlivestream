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
