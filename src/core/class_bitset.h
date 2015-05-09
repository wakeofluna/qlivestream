#ifndef CORE_CLASS_BITSET_H_
#define CORE_CLASS_BITSET_H_

#include <bitset>
#include <type_traits>

template<typename T>
class ClassBitset
{
public:
	typedef typename T::enum_type enum_type;

	inline ClassBitset() : c()
	{
	}

	inline ~ClassBitset()
	{
	}

	inline bool test(T pos) const
	{
		return c.test(value(pos));
	}

	inline ClassBitset<T> & set(T pos)
	{
		c.set(value(pos));
		return *this;
	}

	inline ClassBitset<T> & reset(T pos)
	{
		c.reset(value(pos));
		return *this;
	}

	inline ClassBitset<T> & operator= (ClassBitset<T> const & pOther)
	{
		c = pOther.c;
		return *this;
	}

	inline bool operator== (ClassBitset<T> const & pOther) const
	{
		return c == pOther.c;
	}

	inline bool operator!= (ClassBitset<T> const & pOther) const
	{
		return c != pOther.c;
	}

protected:
	std::bitset<T::max> c;

	inline size_t value(T pos) const
	{
		return static_cast<size_t>(enum_type(pos));
	}
};


#endif // CORE_CLASS_BITSET_H_
