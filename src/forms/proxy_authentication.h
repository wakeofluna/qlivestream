#ifndef FORMS_PROXY_AUTHENTICATION_H_
#define FORMS_PROXY_AUTHENTICATION_H_

#include <QDialog>

namespace Ui
{
	class ProxyAuthentication;
}

namespace forms
{

class ProxyAuthentication : public QDialog
{
Q_OBJECT

public:
	explicit ProxyAuthentication(QWidget * parent = 0);
	~ProxyAuthentication();

	QString username() const;
	QString password() const;

private:
	Ui::ProxyAuthentication * ui;
};

} // namespace forms

#endif // FORMS_PROXY_AUTHENTICATION_H_
