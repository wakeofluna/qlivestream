#ifndef MISC_H_
#define MISC_H_

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

#endif // MISC_H_
