#ifndef FORMS_SSL_ERRORS_H_
#define FORMS_SSL_ERRORS_H_

#include <QDialog>
#include <QSslCertificate>

class QSslError;
template <typename T> class QList;

namespace Ui
{
	class SslErrors;
}

namespace forms
{

class SslErrors : public QDialog
{
Q_OBJECT

public:
	explicit SslErrors(QWidget * parent = 0);
	~SslErrors();

	void setMessages(QList<QSslError> const & pErrors);
	void setCertificate(QSslCertificate const & pCertificate);

private:
	Ui::SslErrors * ui;
	QSslCertificate mCertificate;
};

} // namespace forms

#endif // FORMS_SSL_ERRORS_H_
