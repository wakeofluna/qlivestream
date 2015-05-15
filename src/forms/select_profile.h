#ifndef FORMS_SELECT_PROFILE_H_
#define FORMS_SELECT_PROFILE_H_

#include "core/config_profile.h"
#include <QWidget>

class QSettings;
class Profile;

namespace Ui
{
	class SelectProfile;
}

namespace forms
{

class SelectProfile : public QWidget
{
Q_OBJECT

public:
	explicit SelectProfile(QWidget * parent = 0);
	~SelectProfile();

	bool setSelected(ConfigProfile const & pProfile);

public slots:
	void updateProfiles();

private slots:
	void on_btnEdit_clicked();
	void on_btnBox_accepted();

private:
	Ui::SelectProfile * ui;
	ConfigProfile::List mProfiles;
};

} // namespace forms

#endif // FORMS_SELECT_PROFILE_H_
