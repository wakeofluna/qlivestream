#ifndef CORE_CLASS_BITSET_H_
#define CORE_CLASS_BITSET_H_

#include <bitset>

template<typename T>
class ClassBitset
{
public:
	typedef typename T::enum_type enum_type;

	inline ClassBitset() : c()
	{
		c.reset();
	}

	inline ClassBitset(T pos) : c()
	{
		c.reset();
		c.set(value(pos));
	}

	inline ~ClassBitset()
	{
	}

	inline bool test(T pos) const
	{
		return c.test(value(pos));
	}

	inline ClassBitset<T> & set()
	{
		c.set();
		return *this;
	}

	inline ClassBitset<T> & set(T pos, bool val = true)
	{
		c.set(value(pos), val);
		return *this;
	}

	inline ClassBitset<T> & reset()
	{
		c.reset();
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

	inline ClassBitset<T> operator~ () const
	{
		return ClassBitset<T>(~c);
	}

	inline ClassBitset<T> & operator|= (ClassBitset<T> const & pOther) const
	{
		c |= pOther.c;
		return *this;
	}

	inline ClassBitset<T> & operator&= (ClassBitset<T> const & pOther) const
	{
		c &= pOther.c;
		return *this;
	}

	inline ClassBitset<T> & operator^= (ClassBitset<T> const & pOther) const
	{
		c ^= pOther.c;
		return *this;
	}

protected:
	std::bitset<T::max> c;

	inline ClassBitset(std::bitset<T::max> const & b) : c(b)
	{
	}

	inline size_t value(T pos) const
	{
		return static_cast<size_t>(enum_type(pos));
	}
};


#define ENUMSTRUCT(_name, _enum, _max) \
public: \
typedef _enum enum_type; \
static const enum_type max = _max; \
inline _name() : m##_enum(_max) {} \
inline _name(_enum pValue) : m##_enum(pValue) {} \
inline operator _enum() const { return m##_enum; } \
inline bool isValid() const { return m##_enum != _max; } \
private: \
_enum m##_enum


#endif // CORE_CLASS_BITSET_H_
