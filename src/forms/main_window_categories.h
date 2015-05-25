#ifndef FORMS_MAIN_WINDOW_CATEGORIES_H_
#define FORMS_MAIN_WINDOW_CATEGORIES_H_

#include "core/storage_access.h"
#include <QWidget>

class CategoryObject;
class Profile;
template <typename T> class QList;

namespace Ui
{
	class MainWindowCategories;
}

namespace forms
{

class CategoryObjectWidget;

class MainWindowCategories : public QWidget, public StorageAccess
{
Q_OBJECT

public:
	explicit MainWindowCategories(Profile & pProfile, QWidget * parent);
	~MainWindowCategories();

public slots:
	void refresh();
	void rollup();

signals:
	void selected(CategoryObject * pCategory);

private slots:
	void checkRollup(int pSliderValue);

private:
	void clear();
	void addData(QList<CategoryObject*> && pCategories);

private:
	Ui::MainWindowCategories * ui;

	Profile & mProfile;
	QList<CategoryObject*> mCategories;
	QList<CategoryObjectWidget*> mFavourite;
	QList<CategoryObjectWidget*> mTop;
	bool mCanRollup;
};

} // namespace forms

#endif // FORMS_MAIN_WINDOW_CATEGORIES_H_
