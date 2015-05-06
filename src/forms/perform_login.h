#ifndef FORMS_PERFORM_LOGIN_H_
#define FORMS_PERFORM_LOGIN_H_

#include "core/network_access.h"
#include <QDialog>

class Profile;

namespace Ui
{
	class PerformLogin;
}

namespace forms
{

class PerformLogin : public QDialog, public NetworkAccess
{
Q_OBJECT

public:
	explicit PerformLogin(Profile & pProfile, QWidget * parent = 0);
	~PerformLogin();

private slots:
	void on_stkFeedback_currentChanged(int pIndex);
	void on_btnAcquire_clicked();
	void on_txtToken_textChanged(QString const & pValue);
	void on_btnToken_clicked();

	void checkTokenFinished(QObject * pReply);

private:
	Ui::PerformLogin * ui;

	Profile & mProfile;
	int mStep;
};

} // namespace forms

#endif // FORMS_PERFORM_LOGIN_H_
