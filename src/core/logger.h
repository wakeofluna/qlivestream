#ifndef CORE_LOGGER_H_
#define CORE_LOGGER_H_

#include "../config.h"

class QStatusBar;
class QString;
class QVariant;
class QWidget;

class COREDLL Logger
{
public:
	class StatusMessage
	{
	public:
		StatusMessage() = delete;

		inline explicit StatusMessage(QString pMessage) : mIdent(-1)
		{
			Logger::get()->logStatusUpdate(mIdent, pMessage);
		}

		inline StatusMessage(StatusMessage const & pOther) : mIdent(-1)
		{
			Q_ASSERT(false && "Invalid use of copy constructor");
		}

		inline StatusMessage(StatusMessage && pOther) noexcept : mIdent(pOther.mIdent)
		{
			pOther.mIdent = -1;
		}

		inline ~StatusMessage()
		{
			if (mIdent != -1)
				Logger::get()->logStatusClear(mIdent);
		}

		inline StatusMessage& operator= (StatusMessage const & pOther)
		{
			Q_ASSERT(false && "Invalid use of copy assignment operator");
			mIdent = -1;
			return *this;
		}

		inline StatusMessage& operator= (StatusMessage && pOther) noexcept
		{
			mIdent = pOther.mIdent;
			pOther.mIdent = -1;
			return *this;
		}

	private:
		int mIdent;
	};

public:
	static Logger * get();

	virtual QWidget * networkCaptureWindow() const = 0;
	virtual void logNetworkMessage(QString const & pTag, QVariant const & pMessage) = 0;
	virtual void logNetworkError(QString const & pTag, QString const & pError, QVariant const & pMessage) = 0;

	virtual void pushStatusBar(QStatusBar * pStatusBar) = 0;
	virtual void popStatusBar(QStatusBar * pStatusBar) = 0;
	virtual void logStatusUpdate(int & pIdent, QString const & pMessage) = 0;
	virtual void logStatusClear(int pIdent) = 0;

protected:
	Logger();
	virtual ~Logger();

	static Logger * mInstance;
};


#endif // CORE_LOGGER_H_
