#pragma once
#ifndef MList_h 
#define MList_h

#include "containerBase.h"

namespace MContainer
{
	template<typename T>
	class List :public ContainerBase
	{
	public:
		explicit List()noexcept;

		List(const List& t)noexcept;
		List(List&& t)noexcept;

		List& operator= (const List& t)noexcept;
		List& operator= (List&& t)noexcept;

		~List();

		List& append(const T& t)noexcept;
		List& removeAt(uint pos)noexcept;
		List& removeAt(uint pos, uint len)noexcept;
		List& insert(uint pos, const T& t)noexcept;
		const T& at(uint pos)const;
		void clear()noexcept;
		List& removeOne(const T& t, uint pos = 0, bool onlyOne = true)noexcept;
		uint size()const noexcept { return m_size; };
		void swap(uint first, uint second)noexcept;
	private:
		template<typename T>
		struct Node
		{
			T data;
			Node<T>* previous = nullptr;
			Node<T>* next = nullptr;
			Node<T>(T* t, Node<T>* p, Node<T>* n) { data = t; previous = p; next = n; };
			Node<T>(const Node<T>& t) { data = t.data; previous = t.previous; next = t.next; };

		};

		Node<T>* m_head;
		Node<T>* m_tail;
		uint m_size;
	};

	template<typename T>
	MContainer::List<T>::List() noexcept
	{
		m_size = 0;
		m_head = new Node;
		m_tail = nullptr;
	}

	template<typename T>
	MContainer::List<T>::List(const List& t) noexcept
	{
		m_size = t.m_size;

		Node<T>* srcHead = t.m_head;
		Node<T>* desHead = m_head;
		Node<T>* previous = nullptr;
		while (srcHead->next != nullptr)
		{
			Node<T>* newNode = new Node<T>(srcHead->next);
			desHead->next = newNode;
			if (previous != nullptr)
				desHead->previous = previous;
			
		}
		m_tail = desHead;
		
	}

	template<typename T>
	MContainer::List<T>::List(List&& t) noexcept
	{

	}

	template<typename T>
	List<T>& MContainer::List<T>::operator=(const List<T>& t) noexcept
	{

	}

	template<typename T>
	List<T>& MContainer::List<T>::operator=(List<T>&& t) noexcept
	{

	}

	template<typename T>
	MContainer::List<T>::~List()
	{

	}

	template<typename T>
	List<T>& MContainer::List<T>::append(const T& t) noexcept
	{

	}

	template<typename T>
	List<T>& MContainer::List<T>::removeAt(uint pos) noexcept
	{

	}

	template<typename T>
	List<T>& MContainer::List<T>::removeAt(uint pos, uint len) noexcept
	{

	}

	template<typename T>
	List<T>& MContainer::List<T>::insert(uint pos, const T& t) noexcept
	{

	}

	template<typename T>
	const T& MContainer::List<T>::at(uint pos) const
	{

	}

	template<typename T>
	void MContainer::List<T>::clear() noexcept
	{

	}

	template<typename T>
	List<T>& MContainer::List<T>::removeOne(const T& t, uint pos /*= 0*/, bool onlyOne /*= true*/) noexcept
	{

	}

	template<typename T>
	void MContainer::List<T>::swap(uint first, uint second) noexcept
	{

	}


}





#endif