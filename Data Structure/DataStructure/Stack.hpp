#pragma once
#ifndef MStack_h 
#define MStack_h

#include "Vector.hpp"

namespace MContainer
{
	template<typename T>
	class Stack :protected Vector<T>
	{
	public:
		explicit Stack()noexcept;
		Stack(const Stack&t)noexcept;
		Stack(Stack&&t)noexcept;

		Stack& operator= (const Stack& t)noexcept;
		Stack& operator= (Stack&& t)noexcept;

		~Stack();
		
		bool isEmpty()const noexcept;
		uint size()const noexcept;
		const T& top()const;
		T& top();
		Stack& push(const T& t)noexcept;
		T pop();
		void clear()noexcept;

		T& operator[](uint pos);
	private:
		int m_top;
	};

	template<typename T>
	MContainer::Stack<T>::Stack() noexcept
		:Vector<T>(10)
	{
		m_top = m_npos;
	}

	template<typename T>
	MContainer::Stack<T>::Stack(const Stack&t) noexcept
		:Vector<T>(t)
	{
		m_top = t.m_top;

	}

	template<typename T>
	MContainer::Stack<T>::Stack(Stack&&t) noexcept
		:Vector<T>(t)
	{
		m_top = t.m_top;

	}

	template<typename T>
	Stack<T>& MContainer::Stack<T>::operator=(const Stack& t) noexcept
	{
		if (this == &t)
			return *this;
		Vector<T>operator=(t);
		m_top = t.m_top;

	}

	template<typename T>
	Stack<T>& MContainer::Stack<T>::operator=(Stack&& t) noexcept
	{
		if (this == &t)
			return *this;
		Vector<T>operator=(std::move(t));
		m_top = t.m_top;
	}

	template<typename T>
	MContainer::Stack<T>::~Stack()
	{
		
	}

	template<typename T>
	inline bool Stack<T>::isEmpty() const noexcept
	{
		return m_top == m_npos;
	}

	template<typename T>
	uint MContainer::Stack<T>::size() const noexcept
	{
		return m_top;
	}

	template<typename T>
	const T& MContainer::Stack<T>::top() const
	{
		return last();
	}

	template<typename T>
	T& MContainer::Stack<T>::top()
	{
		return last();
	}

	template<typename T>
	Stack<T>& MContainer::Stack<T>::push(const T& t) noexcept
	{
		append(t);
		m_top++;
		return *this;
	}

	template<typename T>
	T MContainer::Stack<T>::pop()
	{
		T t = last();
		removeAt(m_top);
		m_top--;
		return t;
	}

	template<typename T>
	inline void Stack<T>::clear() noexcept
	{
		Vector<T>::clear();
		m_top = m_npos;
	}

	template<typename T>
	T& MContainer::Stack<T>::operator[](uint pos)
	{
		return Vector<T>::operator[](pos);
	}

}



#endif