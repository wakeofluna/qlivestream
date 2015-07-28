#ifndef FORMS_CATEGORY_OBJECT_WIDGET_H_
#define FORMS_CATEGORY_OBJECT_WIDGET_H_

#include "core/storage_access.h"
#include <QWidget>
class ICategory;
class QByteArray;

namespace Ui
{
	class CategoryObjectWidget;
}

namespace forms
{

class CategoryObjectWidget : public QWidget, public StorageAccess
{
Q_OBJECT

public:
	CategoryObjectWidget(ICategory & pObject, QWidget * parent);
	~CategoryObjectWidget();

	inline ICategory & object() const { return mCategory; }
	inline ICategory & category() const { return mCategory; }

	void setLogo(QByteArray const & pData);
	bool operator< (CategoryObjectWidget const & pOther) const;

public slots:
	void updateFromObject();

signals:
	void clicked(ICategory * pCategory = nullptr);

private slots:
	void on_btnCategory_clicked();

private:
	Ui::CategoryObjectWidget * ui;
	ICategory & mCategory;
};

} // namespace forms

#endif // FORMS_CATEGORY_OBJECT_WIDGET_H_
