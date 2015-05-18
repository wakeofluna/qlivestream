#ifndef CORE_LOGGER_H_
#define CORE_LOGGER_H_

#include <QString>
#include <QVariant>
class QWidget;

class Logger
{
public:
	static Logger * get();

	virtual QWidget * networkCaptureWindow() const;
	virtual void logNetworkMessage(QString pTag, QVariant const & pMessage);
	virtual void logNetworkError(QString pTag, QString const & pMessage);

protected:
	Logger();
	virtual ~Logger();

	static Logger * mInstance;
};


#endif // CORE_LOGGER_H_
