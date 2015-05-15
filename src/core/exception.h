#ifndef CORE_EXCEPTION_H_
#define CORE_EXCEPTION_H_

#include <QException>
#include <QString>

class Exception : public QException
{
public:
	Exception(QString pTitle, QString pDescription = QString());
	virtual ~Exception();

	virtual void raise() const override;
	virtual QException * clone() const override;

	inline QString title() const { return mTitle; }
	inline QString description() const { return mDescription; }

protected:
	QString mTitle;
	QString mDescription;
};

#endif // CORE_EXCEPTION_H_
