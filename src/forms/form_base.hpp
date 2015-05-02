#ifndef FORMS_FORM_BASE_HPP_
#define FORMS_FORM_BASE_HPP_

#include "form_base.h"
#include <QWidget>
#include <QSettings>

namespace forms
{

template <typename T>
FormBase<T>::FormBase(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	ui = new T();
	ui->setupUi(this);

	settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
	settings->beginGroup(objectName());
}

template <typename T>
FormBase<T>::~FormBase()
{
	delete settings;
	delete ui;
}

} // namespace forms

#endif // FORMS_FORM_BASE_HPP_
