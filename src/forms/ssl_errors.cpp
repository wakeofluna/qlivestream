#include "config.h"
#include "ssl_errors.h"
#include "ui_ssl_errors.h"

#include <QList>
#include <QSslError>
#include <QString>
#include <QStyle>
#include <QTextStream>

namespace forms
{

SslErrors::SslErrors(QWidget * parent) : QDialog(parent, Qt::Dialog)
{
	ui = new Ui::SslErrors();
	ui->setupUi(this);

	setWindowIcon(style()->standardIcon(QStyle::SP_MessageBoxCritical));
}

SslErrors::~SslErrors()
{
	delete ui;
}

void SslErrors::setMessages(QList<QSslError> const & pErrors)
{
	QString lMessage;
	QTextStream lStream(&lMessage);

	lStream << "<ul>";
	for (QSslError const & lError : pErrors)
	{
		if (mCertificate.isNull())
			setCertificate(lError.certificate());

		lStream << "<li>" << lError.errorString() << "</li>";
	}
	lStream << "</ul>";

	ui->lblMessage->setText(lMessage);
}

void SslErrors::setCertificate(QSslCertificate const & pCertificate)
{
	mCertificate = pCertificate;
}

} // namespace forms
