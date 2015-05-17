#ifndef FORMS_CATEGORY_OBJECT_WIDGET_H_
#define FORMS_CATEGORY_OBJECT_WIDGET_H_

#include "core/category_object.h"
#include <QWidget>

namespace Ui
{
	class CategoryObjectWidget;
}

namespace forms
{

class CategoryObjectWidget : public QWidget
{
Q_OBJECT

public:
	CategoryObjectWidget(CategoryObject * pObject, QWidget * parent);
	~CategoryObjectWidget();

	inline CategoryObject const * object() const { return mCategory; }

	void setLogo(QByteArray const & pData);

public slots:
	void updateFromObject();

private:
	Ui::CategoryObjectWidget * ui;
	CategoryObject * mCategory;
};

} // namespace forms

#endif // FORMS_CATEGORY_OBJECT_WIDGET_H_
