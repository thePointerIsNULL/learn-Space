#pragma once
#ifndef MVector_h 
#define MVector_h

#include "containerBase.h"


namespace MContainer
{
	
	template<typename T >
	class IteratorVector;

	template<typename T >
	class IteratorVectorConst;


	template<typename T >
	class Vector final :public ContainerBase
	{
	public:
		explicit Vector()noexcept;
		explicit Vector(uint size)noexcept;

		Vector(const Vector& t)noexcept;
		Vector(Vector&& t)noexcept;

		Vector& operator= (const Vector& t)noexcept;
		Vector& operator= (Vector&& t)noexcept;

		~Vector();

		Vector& append(const T& t)noexcept;
		Vector& removeAt(uint pos)noexcept;
		Vector& removeAt(uint pos, uint len)noexcept;
		Vector& insert(uint pos, const T& t)noexcept;
		const T& at(uint pos)const;
		void clear()noexcept;
		Vector& removeOne(const T& t, uint pos = 0, bool onlyOne = true)noexcept;
		uint size()const noexcept { return m_size; };
		uint maxSize()const noexcept { return m_maxSize; };
		void setStep(uint step)noexcept { m_step = step; };
		uint step()const noexcept { return m_step; };
		bool isEmpty()const noexcept { return m_size; };
		void resize(uint size)noexcept;
		void resize(uint size, const T& t)noexcept;
		void swap(uint first, uint second)noexcept;
		void removeFirst()noexcept;
		void removeLast()noexcept;
		const T& first()const;
		const T& last()const;
		T& first();
		T& last();
		bool contains(const T& t, uint pos = 0)const noexcept;
		size_t find(const T& t, uint pos = 0) const noexcept;
		template<typename Funtion>
		size_t find(const T& t, const Funtion& f, uint pos = 0) const noexcept;
		uint capacity() const noexcept { return m_capacity; };
		void reverse()noexcept;
		T& operator[](uint pos)noexcept;

		IteratorVector<T> begin()noexcept;
		IteratorVector<T> end()noexcept;

		IteratorVectorConst<T> begin()const noexcept;
		IteratorVectorConst<T> end()const noexcept;

	private:
		T* m_element;
		uint m_size;
		uint m_step;
		uint m_capacity;

		static constexpr uint m_stepUint = 10;

		void _throw(bool v)const
		{
			if (v)
				throw std::out_of_range("Vector request out of range");
		}

		void freeMemory()noexcept
		{
			if (m_element != nullptr)
			{
				delete[] m_element;
				m_element = nullptr;
			}
		}
	};


#define OverstepMaxAssert assert(m_capacity <= m_maxSize);
#define ElementSize sizeof(T) * m_size
	template<typename T >
	MContainer::Vector<T>::Vector() noexcept
		:Vector(0)
	{

	}

	template<typename T >
	MContainer::Vector<T>::Vector(uint size) noexcept
		:m_size(0)
		, m_step(m_stepUint)
		, m_capacity(size)
	{
		OverstepMaxAssert
			m_element = new T[m_capacity];
	}



	template<typename T >
	MContainer::Vector<T>::Vector(const Vector& t) noexcept
	{
		if (&t == this)
			return;

		this->m_element = new T[t.m_capacity];
		this->m_size = t.m_size;
		this->m_step = t.m_step;
		this->m_capacity = t.m_capacity;
		std::memcpy(this->m_element, t.m_element, ElementSize);


	}

	template<typename T >
	MContainer::Vector<T>::Vector(Vector&& t) noexcept
	{
		if (&t == this)
			return;


		this->m_element = t.m_element;
		this->m_size = t.m_size;
		this->m_step = t.m_step;
		this->m_capacity = t.m_capacity;

		t.m_element = nullptr;
		t.m_size = 0;
		t.m_capacity = 0;
	}

	template<typename T>
	inline Vector<T>& Vector<T>::operator=(const Vector& t) noexcept
	{
		if (&t == this)
			return *this;


		this->m_element = new T[t.m_capacity];
		this->m_size = t.m_size;
		this->m_step = t.m_step;
		this->m_capacity = t.m_capacity;
		std::memcpy(this->m_element, t.m_element, ElementSize);

		return *this;
	}

	template<typename T>
	inline Vector<T>& Vector<T>::operator=(Vector&& t) noexcept
	{
		if (&t == this)
			return *this;
		this->clear();
		this->m_element = t.m_element;
		this->m_size = t.m_size;
		this->m_step = t.m_step;
		this->m_capacity = t.m_capacity;

		t.m_element = nullptr;
		t.m_size = 0;
		t.m_capacity = 0;


		return *this;
	}

	template<typename T>
	inline Vector<T>::~Vector()
	{
		clear();
	}


	template<typename T>
	inline Vector<T>& Vector<T>::append(const T& t) noexcept
	{
		if (m_size + 1 > m_capacity)
		{
			assert(m_capacity + 1 <= m_maxSize);

			m_capacity = m_capacity + m_step > m_maxSize ? m_maxSize : m_capacity + m_step;

			T* newArray = new T[m_capacity];

			std::memcpy(newArray, m_element, ElementSize);

			freeMemory();
			m_element = newArray;
		}

		m_element[m_size++] = t;

		return *this;
	}

	template<typename T >
	Vector<T>& MContainer::Vector<T>::removeAt(uint pos, uint len) noexcept
	{
		if (pos < 0 || pos > m_size)
			return *this;

		if (len + pos > m_size)
			len = m_size - pos;

		uint range = pos + len;

		uint size = m_size - range;
		std::memmove(m_element + pos, m_element + range, sizeof(T) * size);
		m_size -= len;
		return *this;
	}


	template<typename T>
	inline Vector<T>& Vector<T>::removeAt(uint pos) noexcept
	{
		return removeAt(pos, 1);
	}

	template<typename T>
	inline Vector<T>& Vector<T>::insert(uint pos, const T& t)noexcept
	{
		uint size = m_size + 1;
		if (pos > size)
			return *this;

		if (size > m_capacity)
		{
			assert(m_capacity + 1 <= m_maxSize);

			m_capacity = m_capacity + m_step > m_maxSize ? m_maxSize : m_capacity + m_step;

			T* newArray = new T[m_capacity];

			std::memcpy(newArray, m_element, ElementSize);
			freeMemory();
			m_element = newArray;

		}
		uint count = m_size - pos;
		uint movePos = m_size;
		while (count)
		{
			m_element[movePos] = m_element[movePos - 1];
			movePos -= 1;
			count--;
		}
		m_element[pos] = t;
		m_size++;
		return *this;
	}

	template<typename T>
	inline const T& Vector<T>::at(uint pos) const
	{
		_throw(pos >= m_size);

		return m_element[pos];
	}

	template<typename T>
	inline void Vector<T>::clear()noexcept
	{
		freeMemory();
		m_size = 0;
		m_capacity = 0;
		m_step = m_stepUint;
	}

	template<typename T>
	inline Vector<T>& Vector<T>::removeOne(const T& t, uint pos, bool onlyOne)noexcept
	{
		if (pos >= m_size)
			return *this;

		for (size_t i = 0; i < m_size; i++)
		{
			if (m_element[i] == t)
			{
				removeAt(i);
				if (onlyOne)
					return *this;
			}
		}


		return *this;
	}

	template<typename T>
	inline T& Vector<T>::operator[](uint pos) noexcept
	{
		_throw(pos >= m_size);
		return m_element[pos];
	}

	template<typename T >
	T& MContainer::Vector<T>::last()
	{
		_throw(!m_size);
		return m_element[m_size - 1];
	}

	template<typename T >
	T& MContainer::Vector<T>::first()
	{
		_throw(!m_size);
		return m_element[0];
	}

	template<typename T >
	bool MContainer::Vector<T>::contains(const T& t, uint pos /*= 0*/)const noexcept
	{
		for (; pos < m_size; pos++)
		{
			if (t == m_element[pos])
				return true;
		}
		return false;
	}

	template<typename T >
	const T& MContainer::Vector<T>::last()const
	{
		_throw(!m_size);
		return m_element[m_size - 1];
	}

	template<typename T >
	const T& MContainer::Vector<T>::first()const
	{
		_throw(!m_size);
		return m_element[0];
	}

	template<typename T >
	void MContainer::Vector<T>::removeLast()noexcept
	{
		removeAt(m_size - 1);
	}

	template<typename T >
	void MContainer::Vector<T>::removeFirst()noexcept
	{
		removeAt(0);
	}

	template<typename T >
	void MContainer::Vector<T>::swap(uint first, uint second)noexcept
	{
		if (first == second
			|| first >= m_size
			|| second >= m_size)
			return;
		T tmp = m_element[first];
		m_element[first] = m_element[second];
		m_element[second] = tmp;
	}

	template<typename T >
	void MContainer::Vector<T>::resize(uint size, const T& t)noexcept
	{
		clear();
		m_capacity = size;
		m_size = size;
		m_element = new T[m_capacity]{ t };

	}

	template<typename T >
	void MContainer::Vector<T>::resize(uint size)noexcept
	{
		resize(size, {});
	}


	template<typename T >
	size_t MContainer::Vector<T>::find(const T& t, uint pos)const noexcept
	{
		for (; pos < m_size; pos++)
		{
			if (m_element[pos] == t)
				return pos;
		}
		return m_nopos;
	}


	template<typename T >
	template<typename Funtion>
	size_t MContainer::Vector<T>::find(const T& t, const Funtion& f, uint pos /*= 0*/)const noexcept
	{
		for (; pos < m_size; pos++)
		{
			if (f(this->at(pos), t))
				return pos;
		}
		return m_nopos;
	}


	template<typename T >
	void MContainer::Vector<T>::reverse() noexcept
	{
		if (isEmpty())
			return;
		T tmp;
		for (size_t i = 0; i < m_size / 2; i++)
		{
			tmp = m_element[i];
			m_element[i] = m_element[m_size - 1 - i];
			m_element[m_size - 1 - i] = tmp;
		}
	}


	template<typename T >
	IteratorVectorConst<T> MContainer::Vector<T>::end() const noexcept
	{
		return IteratorVectorConst<T>(m_element + m_size);
	}

	template<typename T >
	IteratorVectorConst<T> MContainer::Vector<T>::begin() const noexcept
	{
		return IteratorVectorConst<T>(m_element);
	}

	template<typename T >
	IteratorVector<T> MContainer::Vector<T>::end()noexcept
	{
		return IteratorVector<T>(m_element + m_size);
	}

	template<typename T >
	IteratorVector<T> MContainer::Vector<T>::begin()noexcept
	{
		return IteratorVector<T>(m_element);
	}









	template<typename T >
	class IteratorVector :public  std::iterator<std::random_access_iterator_tag, T>
	{
	private:
		friend  Vector<T>;
		T* m_ptr;
		IteratorVector<T>(T* t)
			:m_ptr(t)
		{
		
		}
	public:
		IteratorVector<T> operator++()
		{
			return ++m_ptr;
		}
		IteratorVector<T> operator++(int)
		{
			T * tmp = m_ptr;
			m_ptr++;
			return tmp;
		}
		IteratorVector<T> operator--()
		{
			return --m_ptr;
		}
		IteratorVector<T> operator--(int)
		{
			T * tmp = m_ptr;
			m_ptr--;
			return tmp;
		}
		T*operator->()
		{
			return m_ptr;
		}
		T& operator*()
		{
			return* m_ptr;
		}
		bool operator!=(const IteratorVector<T>& t)const
		{
			return m_ptr != t.m_ptr;
		}
		bool operator==(const IteratorVector<T>& t)const
		{
			return m_ptr == t.m_ptr;
		}
		bool operator<(const IteratorVector<T>& t)const
		{
			return m_ptr < t.m_ptr;
		}
		bool operator>(const IteratorVector<T>& t)const
		{
			return m_ptr > t.m_ptr;
		}
		IteratorVector<T> operator+=(difference_type size)const
		{
			T* tmp = m_ptr + size;
			return tmp;
		}
		IteratorVector<T> operator-=(difference_type size)const
		{
			T* tmp = m_ptr - size;
			return tmp;
		}
	};

	template<typename T >
	class IteratorVectorConst :public  std::iterator<std::random_access_iterator_tag, T>
	{
	private:
		friend  Vector<T>;
		T* m_ptr;
		IteratorVectorConst<T>(T* t)
			: m_ptr(t)
		{

		}
	public:
		IteratorVectorConst<T> operator++()
		{
			return ++m_ptr;
		}
		IteratorVectorConst<T> operator++(int)
		{
			T * tmp = m_ptr;
			m_ptr++;
			return tmp;
		}
		IteratorVectorConst<T> operator--()
		{
			-return -m_ptr;
		}
		IteratorVectorConst<T> operator--(int)
		{
			T * tmp = m_ptr;
			m_ptr--;
			return tmp;
		}
		const T *operator->()
		{
			return m_ptr;
		}
		const T& operator*()
		{
			return*m_ptr;
		}
		bool operator!=(const IteratorVectorConst<T>& t)const
		{
			return m_ptr != t.m_ptr;
		}
		bool operator==(const IteratorVectorConst<T>& t)const
		{
			return m_ptr == t.m_ptr;
		}
		bool operator<(const IteratorVectorConst<T>& t)const
		{
			return m_ptr < t.m_ptr;
		}
		bool operator>(const IteratorVectorConst<T>& t)const
		{
			return m_ptr > t.m_ptr;
		}
		IteratorVectorConst<T> operator+=(difference_type size)const
		{
			T* tmp = m_ptr + size;
			return tmp;
		}
		IteratorVectorConst<T> operator-=(difference_type size)const
		{
			T* tmp = m_ptr - size;
			return tmp;
		}
	};


}

#endif // !MVectorContainer 

