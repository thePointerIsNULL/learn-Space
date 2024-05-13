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
		bool isEmpty()const noexcept { return m_size; }
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

		Node<T>* m_head /*头结点不存储数据*/;
		Node<T>* m_tail /*数据节点*/;
		uint m_size;
	};

	template<typename T>
	MContainer::List<T>::List() noexcept
	{
		m_size = 0;
		m_head = new Node;
		m_tail = = nullptr;
	}

	template<typename T>
	MContainer::List<T>::List(const List& t) noexcept
	{
		m_size = t.m_size;

		Node<T>* srcPtr = t.m_head;
		Node<T>* desPtr = m_head;
		while (srcPtr->next != nullptr)
		{
			Node<T>* newNode = new Node<T>(srcHead->next);
			desPtr->next = newNode;
			newNode->previous = desPtr;
			desPtr = newNode;
			srcHead = srcHead->next;
		}
		m_tail = desHead;
	}

	template<typename T>
	MContainer::List<T>::List(List&& t) noexcept
	{
		m_size = t.m_size;
		m_head = t.m_head;
		m_tail = t.m_tail;

		t.size = 0;
		t.m_head = new Node<T>;
		t.m_tail = nullptr;
	}

	template<typename T>
	List<T>& MContainer::List<T>::operator=(const List<T>& t) noexcept
	{
		m_size = t.m_size;

		Node<T>* srcPtr = t.m_head;
		Node<T>* desPtr = m_head;
		while (srcPtr->next != nullptr)
		{
			Node<T>* newNode = new Node<T>(srcHead->next);
			desPtr->next = newNode;
			newNode->previous = desPtr;
			desPtr = newNode;
			srcHead = srcHead->next;
		}
		m_tail = desHead;
	}

	template<typename T>
	List<T>& MContainer::List<T>::operator=(List<T>&& t) noexcept
	{
		m_size = t.m_size;
		m_head = t.m_head;
		m_tail = t.m_tail;

		t.size = 0;
		t.m_head = new Node<T>;
		t.m_tail = nullptr;
	}

	template<typename T>
	MContainer::List<T>::~List()
	{
		clear();
	}

	template<typename T>
	List<T>& MContainer::List<T>::append(const T& t) noexcept
	{
		if (m_size + 1 >= m_maxSize)
			return *this;
		Node<T> * node = new Node<T>;
		node->data = t;
		m_tail->next = node;
		node->previous = m_tail;
		m_tail = node;
		m_size++;
	}

	template<typename T>
	List<T>& MContainer::List<T>::removeAt(uint pos) noexcept
	{
		if (pos >= m_size)
			return *this;
		uint startPos = 0;
		char step = 1;
		Node<T>* ptr = nullptr;
		if (pos > m_size / 2)
		{
			startPos = m_szie - 1;
			step = -1;
			ptr = m_tail;
		}
		else
		{
			startPos =0;
			step = 1;
			ptr = m_head->next;
		}
		
		for (; startPos != 0; startPos += step)
		{
			if (startPos == pos)
			{
				Node<T>* delNode = prt;
				Node<T>* nextNode = ptr->next;
				Node<T>* preNode = ptr->previous;
				if (nextNode != nullptr)
				{
					nextNode->previous = preNode;
				}
				preNode->next = nextNode;
				delete delNode;
				break;
			}
			ptr = ptr->next;
		}
		return *this;
	}

	template<typename T>
	List<T>& MContainer::List<T>::removeAt(uint pos, uint len) noexcept
	{
		if (pos >= m_size)
			return *this;
		uint startPos = 0;
		char step = 1;
		Node<T>* ptr = nullptr;
		if (pos > m_size / 2)
		{
			startPos = m_szie - 1;
			step = -1;
			ptr = m_tail;
		}
		else
		{
			startPos =0;
			step = 1;
			ptr = m_head->next;
		}
		for (; startPos != 0; startPos += step)
		{
			if (startPos == pos)
			{
				
			}
		}
		return *this;
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