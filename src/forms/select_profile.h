#ifndef FORMS_SELECT_PROFILE_H_
#define FORMS_SELECT_PROFILE_H_

#include "form_base.h"
#include <QWidget>

namespace Ui
{
	class SelectProfile;
}

namespace forms
{

class SelectProfile : public FormBase<Ui::SelectProfile>
{
Q_OBJECT

public:
	explicit SelectProfile(QWidget * parent = 0, Qt::WindowFlags f = 0);
	~SelectProfile();

public slots:
	void on_btnBox_accepted();
};

} // namespace forms

#endif // FORMS_SELECT_PROFILE_H_
