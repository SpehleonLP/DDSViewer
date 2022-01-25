#ifndef COUNTED_PTR_H
#define COUNTED_PTR_H
#include <algorithm>

template<typename T>
class counted_ptr
{
public:
typedef T value_type;
	static counted_ptr Make()
	{
		return counted_ptr<T>(new T());
	}

	static counted_ptr asWrap(T * it)
	{
		return counted_ptr<T>(it);
	}

	static counted_ptr Wrap(T * it)
	{
		if(it) it->AddRef();
		return counted_ptr<T>(it);
	}

	template<typename...Args>
	static counted_ptr Make(Args&&...args)
	{
		return counted_ptr<T>(new T(std::forward<Args...>(args...)));
	}

	counted_ptr(const std::nullptr_t &) : counted_ptr() {}
	counted_ptr() = default;
	counted_ptr(const counted_ptr & it) :
		m_counted(it.m_counted)
	{
		if(m_counted) m_counted->AddRef();
	}
	counted_ptr(counted_ptr && it) :
		m_counted(it.m_counted)
	{
		it.m_counted = nullptr;
	}

	~counted_ptr()
	{
		if(m_counted) m_counted->Release();
		m_counted = nullptr;
	}

	bool empty() const { return m_counted == nullptr; }

	counted_ptr & operator=(const counted_ptr & it)
	{
	//avoids flushing a cache due to an atomic operation
		if(m_counted != it.m_counted)
		{
			T * temp = m_counted;
			m_counted = it.m_counted;
			if(m_counted) m_counted->AddRef();
			if(temp)      temp->Release();
		}

		return *this;
	}

	counted_ptr & operator=(const std::nullptr_t &)
	{
	//avoids flushing a cache due to an atomic operation
		T * temp = m_counted;
		m_counted = nullptr;
		if(temp)      temp->Release();

		return *this;
	}

	counted_ptr & operator=(counted_ptr && it)
	{
		T * temp = m_counted;
		m_counted = it.m_counted;
		it.m_counted = nullptr;
		if(temp)      temp->Release();

		return *this;
	}

	T * get() const { return m_counted; }

	T * asGet() const
	{
		if(m_counted) m_counted->AddRef();
		return m_counted;
	}

	T * asTake()
	{
		T * temp = m_counted;
		m_counted = nullptr;
		return temp;
	}

	void CountedSet(T * it)
	{
		if(m_counted != it)
		{
			it->AddRef();
			T * temp = m_counted;
			m_counted = it;
			if(temp) temp->Release();
		}
	}

	void asSet(T * it)
	{
		if(m_counted == it)
			m_counted->Release();
		else
		{
			T * temp = m_counted;
			m_counted = it;
			if(temp) temp->Release();
		}
	}

	void reset()
	{
		T * temp = m_counted;
		m_counted = nullptr;
		if(temp) temp->Release();
	}

	explicit operator bool() const { return m_counted != nullptr; }

	T * operator->() { return  m_counted; }
	T & operator* () { return *m_counted; }

	const T * operator->() const { return  m_counted; }
	const T & operator* () const { return *m_counted; }

	bool operator==(const void * it) const { return m_counted == it; }
	bool operator!=(const void * it) const { return m_counted != it; }

	bool operator==(const std::nullptr_t & it) const { return m_counted == it; }
	bool operator!=(const std::nullptr_t & it) const { return m_counted != it; }

	bool operator==(const counted_ptr & it) const { return m_counted == it.m_counted; }
	bool operator!=(const counted_ptr & it) const { return m_counted != it.m_counted; }
	bool operator<=(const counted_ptr & it) const { return m_counted <= it.m_counted; }
	bool operator< (const counted_ptr & it) const { return m_counted < it.m_counted; }
	bool operator>=(const counted_ptr & it) const { return m_counted >= it.m_counted; }
	bool operator> (const counted_ptr & it) const { return m_counted > it.m_counted; }

	void swap(counted_ptr & it)
	{
		auto eax = m_counted;
		m_counted = it.m_counted;
		it.m_counted = eax;
	}

	friend void swap(counted_ptr<T> & a, counted_ptr<T> & b)
	{
		std::swap(a.m_counted, b.m_counted);
	}

//destructive
	template<class U>
	counted_ptr<U> do_cast()
	{
		auto r = counted_ptr<U>::asWrap(static_cast<U*>(m_counted));
		m_counted = nullptr;
		return r;
	}

protected:
	counted_ptr(T * object) : m_counted(object) {}

private:
	T * m_counted{};
};

template<class U, class V>
inline counted_ptr<U> counted_cast(counted_ptr<V> && in)
{
	return in.do_cast();
}

template<class T>
inline counted_ptr<T> CountedWrap(T * it)
{
	return counted_ptr<T>::Wrap(it);
}

template<class T>
inline counted_ptr<T> UncountedWrap(T * it)
{
	return counted_ptr<T>::asWrap(it);
}

template<class U, class V>
inline counted_ptr<U> CountedCast(V * it)
{
	return counted_ptr<U>::Wrap(static_cast<U*>(it));
}

template<class U, class V>
inline counted_ptr<U> UncountedCast(V * it)
{
	return counted_ptr<U>::asWrap(static_cast<U*>(it));
}


#endif // COUNTED_PTR_H
