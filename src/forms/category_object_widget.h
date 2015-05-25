#ifndef FORMS_CATEGORY_OBJECT_WIDGET_H_
#define FORMS_CATEGORY_OBJECT_WIDGET_H_

#include <QWidget>
class CategoryObject;
class QByteArray;

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

	inline CategoryObject * object() const { return mCategory; }

	void setLogo(QByteArray const & pData);

public slots:
	void updateFromObject();

signals:
	void clicked(CategoryObject * pCategory = nullptr);

private slots:
	void on_btnCategory_clicked();

private:
	Ui::CategoryObjectWidget * ui;
	CategoryObject * mCategory;
};

} // namespace forms

#endif // FORMS_CATEGORY_OBJECT_WIDGET_H_
