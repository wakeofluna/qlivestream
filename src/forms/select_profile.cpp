#include "config.h"
#include "select_profile.h"
#include "ui_select_profile.h"
#include "form_base.hpp"

namespace forms
{

template class FormBase<Ui::SelectProfile>;

SelectProfile::SelectProfile(QWidget * parent, Qt::WindowFlags f) : FormBase(parent, f)
{
}

SelectProfile::~SelectProfile()
{
}

void SelectProfile::on_btnBox_accepted()
{
}

} // namespace forms
