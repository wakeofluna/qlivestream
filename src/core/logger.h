#ifndef CORE_LOGGER_H_
#define CORE_LOGGER_H_

#include "config.h"
class QStatusBar;
class QString;
class QVariant;
class QWidget;

class COREDLL Logger
{
public:
	struct StatusMessage
	{
		inline StatusMessage(QString pMessage) { Logger::get()->logStatusUpdate(mIdent, pMessage); }
		inline StatusMessage(StatusMessage const & pOther) { mIdent = pOther.mIdent; pOther.mIdent = -1; }
		inline ~StatusMessage() { Logger::get()->logStatusClear(mIdent); }
		mutable int mIdent;
	};

public:
	static Logger * get();

	virtual QWidget * networkCaptureWindow() const = 0;
	virtual void logNetworkMessage(QString pTag, QVariant const & pMessage) = 0;
	virtual void logNetworkError(QString pTag, QString const & pError, QVariant const & pMessage) = 0;

	virtual void pushStatusBar(QStatusBar * pStatusBar) = 0;
	virtual void popStatusBar(QStatusBar * pStatusBar) = 0;
	virtual void logStatusUpdate(int & pIdent, QString pMessage) = 0;
	virtual void logStatusClear(int pIdent) = 0;

protected:
	Logger();
	virtual ~Logger();

	static Logger * mInstance;
};


#endif // CORE_LOGGER_H_
