#include "config.h"
#include "flowing_layout.h"

#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QWidget>

FlowingLayout::FlowingLayout(QWidget * parent) : QLayout(parent)
{
	mHorizontalSpacing = -1;
	mVerticalSpacing = -1;
	mCurrentRows = 0;
	mCurrentColumns = 0;
	mLayedoutItems = 0;
	mAnimationGroup = nullptr;
	mAnimated = true; // XXX default from settings

	setSizeConstraint(SetMinimumSize);
}

FlowingLayout::FlowingLayout(QWidget * parent, Sorter && pSorter) : FlowingLayout(parent)
{
	mSorter = std::move(pSorter);
}

FlowingLayout::~FlowingLayout()
{
	clear(false);
}

void FlowingLayout::addItem(QLayoutItem * pItem)
{
	mItems.append(pItem);
	invalidate();
}

QLayoutItem * FlowingLayout::itemAt(int index) const
{
	return mItems.value(index);
}

QLayoutItem * FlowingLayout::takeAt(int index)
{
	if (index < 0 || index >= mItems.size())
		return nullptr;

	QLayoutItem * lItem = mItems.takeAt(index);
	invalidate();

	return lItem;
}

int FlowingLayout::count() const
{
	return mItems.size();
}

void FlowingLayout::invalidate()
{
	mCurrentRows = 0;
	mCurrentColumns = 0;
	mLayedoutItems = 0;

	QLayout::invalidate();
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
	QSize lItemSize(0, 0);
	for (QLayoutItem * lItem : mItems)
		lItemSize = lItemSize.expandedTo(lItem->sizeHint());

	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);

	return lItemSize + QSize(left + right, top + bottom);
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
	invalidate();
}

int FlowingLayout::verticalSpacing() const
{
	if (mVerticalSpacing >= 0)
		return mVerticalSpacing;

	return calcSpacing(QStyle::PM_LayoutVerticalSpacing);
}

void FlowingLayout::setVerticalSpacing(int pSpacing)
{
	mVerticalSpacing = pSpacing;
	invalidate();
}

int FlowingLayout::currentRows() const
{
	return mCurrentColumns;
}

int FlowingLayout::currentColumns() const
{
	return mCurrentColumns;
}

bool FlowingLayout::isAnimated() const
{
	return mAnimated;
}

void FlowingLayout::setAnimated(bool pAnimated)
{
	mAnimated = pAnimated;
}

void FlowingLayout::clear(bool pDeleteWidgets)
{
	delete mAnimationGroup;
	mAnimationGroup = nullptr;

	while (!mItems.isEmpty())
	{
		QLayoutItem * lItem = mItems.last();
		mItems.removeLast();

		if (pDeleteWidgets)
			delete lItem->widget();
		delete lItem;
	}

	invalidate();
}

void FlowingLayout::setSorter(Sorter && pSorter)
{
	mSorter = std::move(pSorter);
}

void FlowingLayout::sort()
{
	sort(mSorter);
}

void FlowingLayout::sort(Sorter const & pSorter)
{
	if (pSorter && mItems.count() > 1)
	{
		std::sort(mItems.begin(), mItems.end(), [&pSorter] (QLayoutItem * lhs, QLayoutItem * rhs) -> bool
		{
			return pSorter(lhs->widget(), rhs->widget());
		});

		invalidate();
	}
}

int FlowingLayout::calcSpacing(QStyle::PixelMetric pMetric) const
{
    QObject * lParent = this->parent();
    if (lParent != nullptr)
    {
		if (lParent->isWidgetType())
		{
			QWidget * lWidget = static_cast<QWidget *>(lParent);
			return lWidget->style()->pixelMetric(pMetric, 0, lWidget);
		}
		else
		{
			QVariant lSpacing;

			if (pMetric == QStyle::PM_LayoutHorizontalSpacing)
				lSpacing = lParent->property("horizontalSpacing");
			else if (pMetric == QStyle::PM_LayoutVerticalSpacing)
				lSpacing = lParent->property("verticalSpacing");

			if (!lSpacing.isValid())
				lSpacing = lParent->property("spacing");

			bool lOk;
			int lValue = lSpacing.toInt(&lOk);
			if (lOk && lValue >= 0)
				return lValue;
		}
    }

    return QApplication::style()->pixelMetric(pMetric, 0, 0);
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
		delete mAnimationGroup;
		mAnimationGroup = nullptr;

		for (int i = 0; i < mItems.size(); ++i)
		{
			int y = i / lColumns;
			int x = i % lColumns;

			QPoint lTopLeft;
			lTopLeft.setX(lRect.left() + x * lItemSize.width() + (x-1) * lSpaceX);
			lTopLeft.setY(lRect.top() + y * lItemSize.height() + (y-1) * lSpaceY);

			QWidget * lWidget = mItems[i]->widget();

			if (mAnimated)
			{
				if (mAnimationGroup == nullptr)
					mAnimationGroup = new QParallelAnimationGroup();

				QPropertyAnimation * lAnim = new QPropertyAnimation();
				lAnim->setDuration(500);
				mAnimationGroup->addAnimation(lAnim);

				if (lWidget->geometry().top() < top)
				{
					// Fade in
					QGraphicsOpacityEffect * lEffect = new QGraphicsOpacityEffect(lAnim);
					lEffect->setOpacity(0.0);
					lWidget->setGraphicsEffect(lEffect);
					lWidget->setGeometry(QRect(lTopLeft, lItemSize));
					lAnim->setTargetObject(lEffect);
					lAnim->setEasingCurve(QEasingCurve::InQuad);
					lAnim->setPropertyName("opacity");
					lAnim->setEndValue(1.0);
					connect(lAnim, &QAbstractAnimation::finished, [lEffect] () { lEffect->deleteLater(); });
				}
				else
				{
					// Move
					QEasingCurve lCurve(QEasingCurve::OutBack);
					lCurve.setOvershoot(1);

					lAnim->setTargetObject(lWidget);
					lAnim->setPropertyName("geometry");
					lAnim->setEasingCurve(lCurve);
					lAnim->setEndValue(QRect(lTopLeft, lItemSize));
				}
			}
			else
			{
				lWidget->setGeometry(QRect(lTopLeft, lItemSize));
			}
		}

		mCurrentColumns = lColumns;
		mCurrentRows = lRows;
		mLayedoutItems = mItems.size();

		if (mAnimationGroup != nullptr)
			mAnimationGroup->start();
	}

	lSelfSize += QSize(left + right, top + bottom);
	return lSelfSize;
}
