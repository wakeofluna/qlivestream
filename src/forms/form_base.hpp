#ifndef FORMS_FORM_BASE_HPP_
#define FORMS_FORM_BASE_HPP_

#include "form_base.h"

namespace forms
{

template <typename T>
FormBase<T>::FormBase(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	ui = new T();
	ui->setupUi(this);
}

template <typename T>
FormBase<T>::~FormBase()
{
	delete ui;
}

} // namespace forms

#endif // FORMS_FORM_BASE_HPP_
