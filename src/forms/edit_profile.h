#ifndef FORMS_EDIT_PROFILE_H_
#define FORMS_EDIT_PROFILE_H_

#include <QDialog>

class Profile;
class QSettings;

namespace Ui
{
	class EditProfile;
}

namespace forms
{

class EditProfile : public QDialog
{
Q_OBJECT

public:
	explicit EditProfile(Profile & pProfile, QWidget * parent = 0);
	~EditProfile();

public slots:
	void on_btnBox_accepted();

private:
	Ui::EditProfile * ui;
	Profile & mProfile;
};

} // namespace forms

#endif // FORMS_EDIT_PROFILE_H_
