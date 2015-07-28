#ifndef CORE_FLOWING_LAYOUT_H_
#define CORE_FLOWING_LAYOUT_H_

#include <functional>
#include <QLayout>
#include <QStyle>
class QAnimationGroup;

class FlowingLayout : public QLayout
{
Q_OBJECT

public:
	typedef std::function<bool (QWidget const * lhs, QWidget const * rhs)> Sorter;

public:
	explicit FlowingLayout(QWidget * parent);
	explicit FlowingLayout(QWidget * parent, Sorter && pSorter);
	~FlowingLayout();

	// Overrides QLayout
	virtual void addItem(QLayoutItem * pItem) Q_DECL_OVERRIDE;
    virtual QLayoutItem *itemAt(int index) const Q_DECL_OVERRIDE;
    virtual QLayoutItem *takeAt(int index) Q_DECL_OVERRIDE;
    virtual int count() const Q_DECL_OVERRIDE;

    // Overrides QLayoutItem
    virtual Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
    virtual bool hasHeightForWidth() const Q_DECL_OVERRIDE;
    virtual int heightForWidth(int w) const Q_DECL_OVERRIDE;
    virtual QSize minimumSize() const Q_DECL_OVERRIDE;
    virtual void setGeometry(const QRect & r) Q_DECL_OVERRIDE;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    int horizontalSpacing() const;
    void setHorizontalSpacing(int pSpacing = -1);

    int verticalSpacing() const;
    void setVerticalSpacing(int pSpacing = -1);

    int currentRows() const;
    int currentColumns() const;

    bool isAnimated() const;
    void setAnimated(bool pAnimated);

	void clear(bool pDeleteWidgets = false);

	void setSorter(Sorter && pSorter);
	void sort();
	void sort(Sorter const & pSorter);

protected:
	int calcSpacing(QStyle::PixelMetric pMetric) const;
	QSize doLayout(QRect const & pRect, bool pApply = true);

	QList<QLayoutItem *> mItems;
	int mHorizontalSpacing;
	int mVerticalSpacing;

	int mCurrentRows;
	int mCurrentColumns;
	int mLayedoutItems;

	QAnimationGroup * mAnimationGroup;
	bool mAnimated;

	Sorter mSorter;
};

#endif // CORE_FLOWING_LAYOUT_H_
