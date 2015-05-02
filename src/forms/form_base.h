#ifndef FORMS_FORM_BASE_H_
#define FORMS_FORM_BASE_H_

#include <QWidget>

namespace forms
{

template <typename T>
class FormBase : public QWidget
{
public:
	explicit FormBase(QWidget * parent = 0, Qt::WindowFlags f = 0);
	virtual ~FormBase();

protected:
	typedef T ui_type;
	T * ui;
};

} // namespace forms

#endif // FORMS_FORM_BASE_H_
