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

			QObject::connect(lWidget, &W::clicked, pWindow, pCallback);
			lLayout->addWidget(lWidget);
		}
	}

	qDeleteAll(lWidgets);
	lLayout->sort();
}


#endif // FORMS_FORM_HELPERS_H_
