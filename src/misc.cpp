#include "misc.h"

template <>
bool updateIfChanged<QString>(QString & pTarget, QString const & pReplacement)
{
	if (pTarget != pReplacement && !pReplacement.isEmpty())
	{
		pTarget = pReplacement;
		return true;
	}
	else
	{
		return false;
	}
}

