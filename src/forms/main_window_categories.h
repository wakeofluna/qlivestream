#ifndef FORMS_MAIN_WINDOW_CATEGORIES_H_
#define FORMS_MAIN_WINDOW_CATEGORIES_H_

#include <QWidget>

class ICategory;
class IProfile;
template <typename T> class QList;

namespace Ui
{
	class MainWindowCategories;
}

namespace forms
{

class CategoryObjectWidget;

class MainWindowCategories : public QWidget
{
Q_OBJECT

public:
	explicit MainWindowCategories(IProfile & pProfile, QWidget * parent);
	~MainWindowCategories();

public slots:
	void refresh();
	void rollup();

signals:
	void selected(ICategory * pCategory);

private slots:
	void checkRollup(int pSliderValue);
	void onCategoriesUpdated();

private:
	Ui::MainWindowCategories * ui;

	IProfile & mProfile;
	QList<CategoryObjectWidget*> mFavourite;
	QList<CategoryObjectWidget*> mTop;
};

} // namespace forms

#endif // FORMS_MAIN_WINDOW_CATEGORIES_H_
