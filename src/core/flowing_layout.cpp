#include "config.h"
#include "flowing_layout.h"

#include <QWidget>

FlowingLayout::FlowingLayout(QWidget * parent) : QLayout(parent)
{
	mHorizontalSpacing = -1;
	mVerticalSpacing = -1;
	mCurrentRows = 0;
	mCurrentColumns = 0;
	mLayedoutItems = 0;

	setSizeConstraint(SetMinimumSize);
}

FlowingLayout::~FlowingLayout()
{
	clear(false);
}

void FlowingLayout::addItem(QLayoutItem * pItem)
{
	mItems.append(pItem);
}

QLayoutItem * FlowingLayout::itemAt(int index) const
{
	return mItems.value(index);
}

QLayoutItem * FlowingLayout::takeAt(int index)
{
	if (index < 0 || index >= mItems.size())
		return nullptr;

	return mItems.takeAt(index);
}

int FlowingLayout::count() const
{
	return mItems.size();
}

Qt::Orientations FlowingLayout::expandingDirections() const
{
	return 0;
}

bool FlowingLayout::hasHeightForWidth() const
{
	return true;
}

int FlowingLayout::heightForWidth(int w) const
{
	QSize lSize = const_cast<FlowingLayout*>(this)->doLayout(QRect(0, 0, w, 0), false);
	return lSize.height();
}

QSize FlowingLayout::minimumSize() const
{
	return sizeHint();
}

void FlowingLayout::setGeometry(const QRect & r)
{
	QLayout::setGeometry(r);
	doLayout(r);
}

QSize FlowingLayout::sizeHint() const
{
	QSize lItemSize;
	for (QLayoutItem * lItem : mItems)
		lItemSize = lItemSize.expandedTo(lItem->sizeHint());

	return lItemSize;
}

int FlowingLayout::horizontalSpacing() const
{
	if (mHorizontalSpacing >= 0)
		return mHorizontalSpacing;

	return calcSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

void FlowingLayout::setHorizontalSpacing(int pSpacing)
{
	mHorizontalSpacing = pSpacing;
}

int FlowingLayout::verticalSpacing() const
{
	if (mHorizontalSpacing >= 0)
		return mHorizontalSpacing;

	return calcSpacing(QStyle::PM_LayoutVerticalSpacing);
}

void FlowingLayout::setVerticalSpacing(int pSpacing)
{
	mVerticalSpacing = pSpacing;
}

int FlowingLayout::currentRows() const
{
	return mCurrentColumns;
}

int FlowingLayout::currentColumns() const
{
	return mCurrentColumns;
}

void FlowingLayout::clear(bool pDeleteWidgets)
{
	while (count() > 0)
	{
		QLayoutItem * lItem = takeAt(count() - 1);
		if (pDeleteWidgets)
			delete lItem->widget();
		delete lItem;
	}

	mCurrentRows = 0;
	mCurrentColumns = 0;
}

int FlowingLayout::calcSpacing(QStyle::PixelMetric pMetric) const
{
    QObject * lParent = this->parent();
    if (!lParent)
        return -1;

    if (lParent->isWidgetType())
    {
        QWidget * lWidget = static_cast<QWidget *>(lParent);
        return lWidget->style()->pixelMetric(pMetric, 0, lWidget);
    }
    else
    {
        return static_cast<QLayout *>(lParent)->spacing();
    }
}

QSize FlowingLayout::doLayout(QRect const & pRect, bool pApply)
{
	if (mItems.size() == 0)
		return QSize(0, 0);

	int lSpaceX = qMax(0, horizontalSpacing());
	int lSpaceY = qMax(0, verticalSpacing());

	QSize lItemSize;
	for (QLayoutItem * lItem : mItems)
		lItemSize = lItemSize.expandedTo(lItem->sizeHint());

	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);
	QRect lRect = pRect.adjusted(left, top, -right, -bottom);

	int lColumns = qMax(1, (lRect.width() + lSpaceX) / (lItemSize.width() + lSpaceX));
	int lRows = (mItems.size() + lColumns - 1) / lColumns;

	QSize lSelfSize;
	lSelfSize.setWidth(lColumns * lItemSize.width() + (lColumns - 1) * lSpaceX);
	lSelfSize.setHeight(lRows * lItemSize.height() + (lRows - 1) * lSpaceY);

	if (pApply && (mCurrentColumns != lColumns || mCurrentRows != lRows || mLayedoutItems != mItems.size()))
	{
		for (int i = 0; i < mItems.size(); ++i)
		{
			int y = i / lColumns;
			int x = i % lColumns;

			QPoint lTopLeft;
			lTopLeft.setX(lRect.left() + x * lItemSize.width() + (x-1) * lSpaceX);
			lTopLeft.setY(lRect.top() + y * lItemSize.height() + (y-1) * lSpaceY);

			QWidget * lWidget = mItems[i]->widget();
			lWidget->setGeometry(QRect(lTopLeft, lItemSize));
		}

		mCurrentColumns = lColumns;
		mCurrentRows = lRows;
		mLayedoutItems = mItems.size();
	}

	lSelfSize += QSize(left + right, top + bottom);
	return lSelfSize;
}
