#ifndef CORE_FLOWING_LAYOUT_H_
#define CORE_FLOWING_LAYOUT_H_

#include <QLayout>
#include <QStyle>
class QAnimationGroup;

class FlowingLayout : public QLayout
{
public:
	explicit FlowingLayout(QWidget * parent);
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

	void clear(bool pDeleteWidgets = false);

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
};

#endif // CORE_FLOWING_LAYOUT_H_
