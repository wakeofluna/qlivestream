#ifndef FORMS_PERFORM_LOGIN_H_
#define FORMS_PERFORM_LOGIN_H_

#include <QDialog>

class Profile;

namespace Ui
{
	class PerformLogin;
}

namespace forms
{

class PerformLogin : public QDialog
{
Q_OBJECT

public:
	explicit PerformLogin(Profile & pProfile, QWidget * parent = 0);
	~PerformLogin();

private slots:
	void on_btnAcquire_clicked();
	void on_txtToken_textChanged(QString const & pValue);
	void on_btnToken_clicked();

private:
	Ui::PerformLogin * ui;

	Profile & mProfile;
};

} // namespace forms

#endif // FORMS_PERFORM_LOGIN_H_
