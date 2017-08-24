#ifndef FORMS_PERFORM_LOGIN_H_
#define FORMS_PERFORM_LOGIN_H_

#include "core/network_access.h"
#include <QDialog>

class IProfile;

namespace Ui
{
	class PerformLogin;
}

namespace forms
{

class PerformLogin : public QDialog, protected NetworkAccess
{
Q_OBJECT

public:
	explicit PerformLogin(IProfile & pProfile, QWidget * parent = 0);
	~PerformLogin();

protected:
	void showEvent(QShowEvent * event);

private:
	void proceed();
	void restart();
	void runStep();
	void runStepImpl();

private:
	Ui::PerformLogin * ui;

	IProfile & mProfile;
	int mStep;
	QWidget * mSubPanel;
};

} // namespace forms

#endif // FORMS_PERFORM_LOGIN_H_
