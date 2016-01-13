#ifndef MISC_H_
#define MISC_H_

#include <QObject>
#include <QString>
#include "core/class_bitset.h"

template <typename T1, typename T2>
struct qobject_less
{
	bool operator() (T1 const * lhs, T1 const * rhs)
	{
		if (lhs == rhs)
			return false;

		T2 const * _lhs = qobject_cast<T2 const *>(lhs);
		if (_lhs == nullptr)
			return false;

		T2 const * _rhs = qobject_cast<T2 const *>(rhs);
		if (_rhs == nullptr)
			return true;

		return *_lhs < *_rhs;
	}

	inline bool operator() (T1 const & lhs, T1 const & rhs)
	{
		return operator() (&lhs, &rhs);
	}
};

template <typename T>
inline bool updateIfChanged(T & pTarget, T const & pReplacement)
{
	if (pTarget != pReplacement)
	{
		pTarget = pReplacement;
		return true;
	}
	else
	{
		return false;
	}
}

template <>
inline bool updateIfChanged<QString>(QString & pTarget, QString const & pReplacement)
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

template <typename T>
inline bool updateIfChanged(T & pTarget, T const & pReplacement, bool pPredicate)
{
	if (pTarget != pReplacement && pPredicate)
	{
		pTarget = pReplacement;
		return true;
	}
	else
	{
		return false;
	}
}

template <typename B>
inline bool updateIfChanged(ClassBitset<B> & pBitset, typename B::enum_type pFlag, bool pSet)
{
	if (pBitset.test(pFlag) != pSet)
	{
		pBitset.set(pFlag, pSet);
		return true;
	}
	else
	{
		return false;
	}
}

#endif // MISC_H_
