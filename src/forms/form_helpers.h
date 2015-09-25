#ifndef FORMS_FORM_HELPERS_H_
#define FORMS_FORM_HELPERS_H_

#include <algorithm>
#include <QList>
#include <QObject>
#include "forms/flowing_layout.h"
#include "misc.h"

template <typename I, typename W, typename WIN>
void addToFlowLayout(QList<I*> pItems, QList<W*> & pTarget, QLayout * pLayout, WIN * pWindow, void (WIN::*pCallback)(I*))
{
	addToFlowLayout<I,W,WIN>(pItems, pTarget, pLayout, pWindow,
			[pWindow,pCallback] (W * pNewWidget)
			{
				QObject::connect(pNewWidget, &W::clicked, pWindow, pCallback);
			});
}

template <typename I, typename W, typename WIN>
void addToFlowLayout(QList<I*> pItems, QList<W*> & pTarget, QLayout * pLayout, WIN * pWindow, std::function<void (W*)> const& pCallback)
{
	QList<W*> lWidgets;
	lWidgets.swap(pTarget);

	FlowingLayout * lLayout = qobject_cast<FlowingLayout*>(pLayout);

	for (I * lItem : pItems)
	{
		auto i = std::find_if(lWidgets.begin(), lWidgets.end(), [lItem] (W * w) { return &w->object() == lItem; });
		if (i != lWidgets.end())
		{
			pTarget.append(*i);
			lWidgets.erase(i);
		}
		else
		{
			W * lWidget = new W(*lItem, pWindow);
			pTarget.append(lWidget);

			pCallback(lWidget);

			lLayout->addWidget(lWidget);
		}
	}

	qDeleteAll(lWidgets);
	lLayout->sort();
}


#endif // FORMS_FORM_HELPERS_H_
