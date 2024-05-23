#pragma once
#ifndef MQueue_h 
#define MQueue_h

#include "containerBase.h"

namespace MContainer
{
	template<typename T>
	class Queue : public ContainerBase
	{
	public:
		explicit Queue() noexcept;
		Queue(const Queue&t)noexcept;
		Queue(Queue&&t)noexcept;

		Queue& operator= (const Queue& t)noexcept;
		Queue& operator= (Queue&& t)noexcept;

		~Queue();

		bool isEmpty()const noexcept { return m_size == 0; };
		uint size()const noexcept { return m_size; };
		Queue& pushBack(const T& t)noexcept;
		Queue& pushBack(T&& t)noexcept;
		Queue& pushFront(const T& t)noexcept;
		Queue& pushFront(T&& t)noexcept;

		T popBack();
		T popFront();

		T& head();
		T& tail();
		const T& at(int pos);
		void clear()noexcept;
	protected:

		template<typename T>
		struct Node
		{
			T data;
			Node* next = nullptr;
			Node * previous = nullptr;
		};
		using NodeT = Node<T>;

		NodeT * m_head;
		NodeT * m_tail;
		uint m_size;

		void _throw(bool v)const
		{
			if (v)
				throw std::out_of_range("Queue request out of range");
		}
	};

	template<typename T>
	MContainer::Queue<T>::Queue() noexcept
	{
		m_head = new NodeT;
		m_tail = nullptr;
		m_size = 0;

	}

	template<typename T>
	MContainer::Queue<T>::Queue(const Queue&t) noexcept
	{
		m_size = t.m_size;
		NodeT* srcPtr = t.m_head->next;
		NodeT* desPtr = m_head;
		while (srcPtr != nullptr)
		{
			NodeT * node = new NodeT;
			node->data = srcPtr->data;
			desPtr->next = node;
			node->previous = desPtr;

			desPtr = node;
			srcPtr = srcPtr->next;
		}
		m_tail = desPtr;

	}

	template<typename T>
	MContainer::Queue<T>::Queue(Queue&&t) noexcept
	{
		if (this == &t)
			return;
		m_head->next = t.m_head->next;
		m_tail = t.m_tail;
		m_size = t.m_size;

		m_head->next = nullptr;
		t.m_size = 0;
	}

	template<typename T>
	Queue<T>& MContainer::Queue<T>::operator=(const Queue& t) noexcept
	{
		if (this == &t)
			return *this;
		m_size = t.m_size;
		NodeT* srcPtr = t.m_head->next;
		NodeT* desPtr = m_head;
		while (srcPtr != nullptr)
		{
			NodeT * node = new NodeT;
			node->data = srcPtr->data;
			desPtr->next = node;
			node->previous = desPtr;

			desPtr = node;
			srcPtr = srcPtr->next;
		}
		m_tail = desPtr;
	}

	template<typename T>
	Queue<T>& MContainer::Queue<T>::operator=(Queue&& t) noexcept
	{
		if (this == &t)
			return *this;
		m_head->next = t.m_head->next;
		m_tail = t.m_tail;
		m_size = t.m_size;

		m_head->next = nullptr;
		t.m_size = 0;
	}

	template<typename T>
	MContainer::Queue<T>::~Queue()
	{
		clear();
		delete m_head;
	}

	template<typename T>
	Queue<T>& MContainer::Queue<T>::pushBack(const T& t) noexcept
	{
		assert(m_size + 1 <= m_maxSize);
		NodeT * node = new NodeT;
		node->data = t;
		if (m_tail == nullptr)
		{
			node->previous = m_head;
			m_head->next = node;
		}
		else
		{
			node->previous = m_tail;
			m_tail->next = node;
		}

		m_tail = node;
		m_size++;
		return *this;
	}

	template<typename T>
	Queue<T>& MContainer::Queue<T>::pushBack(T&& t) noexcept
	{
		assert(m_size + 1 <= m_maxSize);
		NodeT * node = new NodeT;
		node->data = std::move(t);
		if (m_tail == nullptr)
		{
			node->previous = m_head;
			m_head->next = node;
		}
		else
		{
			node->previous = m_tail;
			m_tail->next = node;
		}

		m_tail = node;
		m_size++;
		return *this;
	}

	template<typename T>
	Queue<T>& MContainer::Queue<T>::pushFront(const T& t) noexcept
	{
		assert(m_size + 1 <= m_maxSize);
		NodeT * node = new NodeT;
		node->data = t;
		if (m_tail == nullptr)
		{
			m_head->next = node;
			node->previous = m_head;
			m_tail = node;
		}
		else
		{
			node->next = m_head->next;
			m_head->next = m_node;
			node->next->previous = node;
			node->previous = m_head;
		}
		m_size++;
		return *this;
	}

	template<typename T>
	Queue<T>& MContainer::Queue<T>::pushFront(T&& t) noexcept
	{
		assert(m_size + 1 <= m_maxSize);
		NodeT * node = new NodeT;
		node->data = std::move(t);
		if (m_tail == nullptr)
		{
			m_head->next = node;
			node->previous = m_head;
			m_tail = node;
		}
		else
		{
			node->next = m_head->next;
			m_head->next = node;
			node->next->previous = node;
			node->previous = m_head;
		}
		m_size++;
		return *this;
	}

	template<typename T>
	T MContainer::Queue<T>::popBack()
	{
		_throw(m_size == 0);

		NodeT *ptr = m_tail;
		T t = ptr->data;

		if (ptr->previous == m_head)
		{
			m_tail = nullptr;
			m_head->next = nullptr;
		}
		else
		{
			m_tail = m_tail->previous;
			m_tail->next = nullptr;
		}
		delete ptr;
		m_size--;
		return t;
	}

	template<typename T>
	T MContainer::Queue<T>::popFront()
	{
		_throw(m_size == 0);

		NodeT * ptr = m_head->next;
		T t = ptr->data;

		if (ptr == m_tail)
		{
			m_head->next = nullptr;
			m_tail = nullptr;
		}
		else
		{
			m_head->next = ptr->next;
			ptr->next->previous = m_head;
		}

		delete ptr;
		m_size--;
		return t;
	}

	template<typename T>
	T& MContainer::Queue<T>::head()
	{
		_throw(m_size == 0);
		return m_head->next->data;
	}

	template<typename T>
	T& MContainer::Queue<T>::tail()
	{
		_throw(m_size == 0);
		return m_tail->data;
	}

	template<typename T>
	const T& MContainer::Queue<T>::at(int pos)
	{
		_throw(pos >= m_size);
		NodeT * ptr = m_head->next;

		while (pos != 0)
		{
			ptr = ptr->next;
			pos--;
		}

		return ptr->data;
	}

	template<typename T>
	void MContainer::Queue<T>::clear() noexcept
	{
		NodeT *ptr = m_head->next;
		while (ptr != nullptr)
		{
			NodeT * node = ptr;
			ptr = ptr->next;
			delete node;
		}
		m_tail = nullptr;
		m_size = 0;
	}

	template<typename T>
	class SQueue : public Queue<T>
	{
	public:
		SQueue() = default;
		~SQueue() = default;
		SQueue& enqueue(const T& t)noexcept;
		T dequeue();
		Queue& pushBack(const T& t) = delete;
		Queue& pushBack(T&& t) = delete;
		Queue& pushFront(const T& t) = delete;
		Queue& pushFront(T&& t) = delete;;

		T popBack() = delete;;
		T popFront() = delete;;
	};

	template<typename T>
	T MContainer::SQueue<T>::dequeue()
	{
		return Queue<T>::popFront();
	}

	template<typename T>
	SQueue<T>& MContainer::SQueue<T>::enqueue(const T& t) noexcept
	{
		Queue<T>::pushBack(t);
		return *this;
	}

}


#endif