#pragma once
#ifndef MList_h 
#define MList_h

#include "containerBase.h"

namespace MContainer
{
	class IteratorList;

	class IteratorListConst;

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
			Node<T>() = default;
			Node<T>(T* t, Node<T>* p, Node<T>* n) { data = t; previous = p; next = n; };
			Node<T>(const Node<T>& t) { data = t.data; previous = t.previous; next = t.next; };

		};
		using NodeT = Node<T>;
		NodeT* m_head /*头结点不存储数据*/;
		NodeT* m_tail /*数据节点*/;
		uint m_size;


		NodeT* findNode(uint pos)const noexcept;
		void _throw(bool v)const
		{
			if (v)
				throw std::out_of_range("List request out of range");
		}

	private:

		class IteratorListConst :public  std::iterator<std::bidirectional_iterator_tag, NodeT>
		{
		private:
			friend List<T>;
			NodeT* m_ptr;
			IteratorListConst(NodeT* t)
				: m_ptr(t)
			{

			}
		public:
			IteratorListConst& operator++()
			{
				m_ptr = m_ptr->next;
				return *this;
			}
			IteratorListConst operator++(int)
			{
				IteratorList temp = *this;
				++(*this);
				return temp;
			}
			IteratorListConst& operator--()
			{
				m_ptr = m_ptr->previous;
				return *this;
			}
			IteratorListConst operator--(int)
			{
				IteratorList temp = *this;
				--(*this);
				return temp;
			}
			bool operator!=(const IteratorListConst& t)const
			{
				return m_ptr->data != t.m_ptr->data;
			}
			bool operator==(const IteratorListConst& t)const
			{
				return m_ptr->data == t.m_ptr->data;
			}
			const T & operator*()
			{
				return m_ptr->data;
			}
			const T * operator->()
			{
				return &m_ptr->data;
			}
		};

		class IteratorList :public  std::iterator<std::bidirectional_iterator_tag, NodeT>
		{
		private:
			friend List<T>;
			NodeT* m_ptr;
			IteratorList(NodeT* t)
				: m_ptr(t)
			{

			}
		public:
			IteratorList& operator++()
			{
				m_ptr = m_ptr->next;
				return *this;
			}
			IteratorList operator++(int)
			{
				IteratorList temp = *this;
				++(*this);
				return temp;
			}
			IteratorList& operator--()
			{
				m_ptr = m_ptr->previous;
				return *this;
			}
			IteratorList operator--(int)
			{
				IteratorList temp = *this;
				--(*this);
				return temp;
			}
			bool operator!=(const IteratorList& t)const
			{
				return m_ptr->data != t.m_ptr->data;
			}
			bool operator==(const IteratorList& t)const
			{
				return m_ptr->data == t.m_ptr->data;
			}
			T& operator*()
			{
				return m_ptr->data;
			}
			T* operator->()
			{
				return &m_ptr->data;
			}
		};

	public:
		IteratorList begin() noexcept { return IteratorList(m_head->next); };
		IteratorList end()noexcept
		{
			if (m_tail != nullptr)
				return IteratorList(m_tail);
			else
				return IteratorList(m_head->next);
		}
		IteratorListConst begin()const noexcept { IteratorListConst(m_head->next); };
		IteratorListConst end() const noexcept
		{
			if (m_tail != nullptr)
				return IteratorListConst(m_tail);
			else
				return IteratorListConst(m_head->next);
		}
	};




	template<typename T>
	MContainer::List<T>::List() noexcept
	{
		m_size = 0;
		m_head = new NodeT;
		m_tail = nullptr;
	}

	template<typename T>
	MContainer::List<T>::List(const List& t) noexcept
	{
		m_size = t.m_size;

		NodeT* srcPtr = t.m_head;
		NodeT* desPtr = m_head;
		while (srcPtr->next != nullptr)
		{
			NodeT* newNode = new NodeT(srcHead->next);
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
		t.m_head = new NodeT;
		t.m_tail = nullptr;
	}

	template<typename T>
	List<T>& MContainer::List<T>::operator=(const List<T>& t) noexcept
	{
		m_size = t.m_size;

		NodeT* srcPtr = t.m_head;
		NodeT* desPtr = m_head;
		while (srcPtr->next != nullptr)
		{
			NodeT* newNode = new NodeT(srcHead->next);
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
		t.m_head = new NodeT;
		t.m_tail = nullptr;
	}

	template<typename T>
	MContainer::List<T>::~List()
	{
		clear();
		delete m_head;
	}

	template<typename T>
	List<T>& MContainer::List<T>::append(const T& t) noexcept
	{
		if (m_size + 1 >= m_maxSize)
			return *this;
		NodeT * node = new NodeT;
		node->data = t;
		if (m_size == 0)
		{
			m_head->next = node;
			node->previous = m_head;
			m_tail = node;
			m_size++;
			return *this;
		}

		m_tail->next = node;
		node->previous = m_tail;
		m_tail = node;
		m_size++;
		return *this;
	}

	template<typename T>
	List<T>& MContainer::List<T>::removeAt(uint pos) noexcept
	{
		if (pos >= m_size)
			return *this;

		NodeT* ptr = findNode(pos);
		if (ptr == m_tail)
		{
			if (ptr->previous != m_head)
				m_tail = ptr->previous;
			else
				m_tail = nullptr;
		}

		ptr->previous->next = ptr->next;
		if (ptr->next != nullptr)
			ptr->next->previous = ptr->previous;

		delete ptr;
		m_size--;
		return *this;
	}

	template<typename T>
	List<T>& MContainer::List<T>::removeAt(uint pos, uint len) noexcept
	{
		if (pos >= m_size)
			return *this;
		if (pos + len >= m_size)
			len = m_size - pos;

		NodeT* ptr = findNode(pos);

		NodeT * node = ptr;
		NodeT *tail = nullptr;
		if (pos + len == m_size)
		{
			tail = m_tail;
		}
		else
		{
			uint moveLen = len;
			while (moveLen--)
			{
				ptr = node->next;
			}
			tail = ptr;
		}

		if (tail == m_tail)
		{
			if (node->previous == m_head)
				m_tail = nullptr;
			else
				m_tail = node->previous;
		}

		node->previous->next = tail->next;
		if (tail->next != nullptr)
			tail->previous = node->previous;

		while (node != tail)
		{
			NodeT * ptr = node->next;
			delete node;
			node = ptr;
		}
		delete tail;
		m_size -= len;
		return *this;
	}

	template<typename T>
	List<T>& MContainer::List<T>::insert(uint pos, const T& t) noexcept
	{
		if (pos >= m_size + 1)
			return *this;
		if (m_size + 1 >= m_maxSize)
			return *this;
		NodeT* node = new NodeT;
		node->data = t;
		if (pos == 0)
		{
			NodeT* nextPtr = m_head->next;
			if (nextPtr == nullptr)
			{
				m_head->next = node;
				m_tail = node;
			}
			else
			{
				node->next = nextPtr;
				nextPtr->previous = node;
			}
			node->previous = m_head;
		}
		else if (pos == m_size)
		{
			m_tail->next = node;
			node->previous = m_tail;
			m_tail = node;
		}
		else
		{
			NodeT* posNode = findNode(pos);
			posNode->previous->next = node;
			node->previous = posNode->previous;
			posNode->previous = node;
			node->next = posNode;
		}
		m_size++;
		return *this;
	}

	template<typename T>
	const T& MContainer::List<T>::at(uint pos) const
	{
		_throw(pos >= m_size);

		return findNode(pos)->data;
	}

	template<typename T>
	void MContainer::List<T>::clear() noexcept
	{
		NodeT * ptr = m_head->next;
		while (ptr != nullptr)
		{
			NodeT * node = ptr->next;
			delete ptr;
			ptr = node;
		}
		m_size = 0;
		m_tail = nullptr;
		m_head->next = nullptr;
	}

	template<typename T>
	List<T>& MContainer::List<T>::removeOne(const T& t, uint pos /*= 0*/, bool onlyOne /*= true*/) noexcept
	{
		if (pos >= m_size)
			return *this;
		NodeT* startFindNode = findNode(pos);
		while (startFindNode != nullptr)
		{
			NodeT* nextNode = startFindNode->next;
			if (startFindNode->data == t)
			{
				startFindNode->previous->next = startFindNode->next;
				if (startFindNode->next != nullptr)
					startFindNode->next->previous = startFindNode->previous;
				delete startFindNode;
				m_size--;
				if (onlyOne)
					break;
			}

			startFindNode = nextNode;
		}
		return *this;
	}

	template<typename T>
	void MContainer::List<T>::swap(uint first, uint second) noexcept
	{
		_throw(first >= m_size || second >= m_size);
		NodeT * firstNode = findNode(first);
		NodeT * secondNode = findNode(second);

		T value = firstNode->data;

		firstNode->data = secondNode->data;
		secondNode->data = value;

	}

	template<typename T>
	typename List<T>::Node<T>* MContainer::List<T>::findNode(uint pos)const noexcept
	{
		if (pos >= m_size)
			return{};
		NodeT* ptr = m_tail;
		bool nextMove = false;
		uint nodePos = m_size - 1;
		if (pos < m_size / 2)
		{
			nextMove = true;
			nodePos = 0;
			ptr = m_head->next;
		}

		while (ptr != nullptr)
		{
			if (nodePos == pos)
			{
				return ptr;
			}
			if (nextMove)
			{
				ptr = ptr->next;
				nodePos++;
			}
			else
			{
				ptr = ptr->previous;
				nodePos--;
			}
		}

		return{};
	}


	template<typename T>
	class StaticList :public ContainerBase
	{
	public:
		explicit StaticList()noexcept;

		StaticList(const StaticList& t)noexcept;
		StaticList(StaticList&& t)noexcept;

		StaticList& operator= (const StaticList& t)noexcept;
		StaticList& operator= (StaticList&& t)noexcept;

		~StaticList();

		StaticList& append(const T& t)noexcept;
		StaticList& removeAt(uint pos)noexcept;
		StaticList& removeAt(uint pos, uint len)noexcept;
		StaticList& insert(uint pos, const T& t)noexcept;
		const T& at(uint pos)const;
		void clear()noexcept;
		StaticList& removeOne(const T& t, uint pos = 0, bool onlyOne = true)noexcept;
		uint size()const noexcept { return m_size; };
		void swap(uint first, uint second)noexcept;
		bool isEmpty()const noexcept { return m_size; }
	private:
		uint m_maxSize = 0xFFFF;

		uint m_size;
		uint m_step;
		uint m_capacity;

		static constexpr unsigned short m_stepUint = 10;

		template<typename T>
		struct Node
		{
			T data;
			int next = m_npos;
			int previous = m_npos;
		};
		using NodeT = Node<T>;

		NodeT * m_head; //头结点的previous为尾部数据节点下标，尾节点的next为下一个可用的下标

		void _throw(bool v)const
		{
			if (v)
				throw std::out_of_range("StaticList request out of range");
		}
		void creatMemory()
		{
			NodeT * node = new NodeT[m_capacity];
			std::memcpy(node, m_head, NodeSize);
			delete[]m_head;
			m_head = node;
		}
	};
#define NodeSize sizeof(NodeT) * m_size
	template<typename T>
	MContainer::StaticList<T>::StaticList() noexcept
	{
		m_size = 0;
		m_step = m_stepUint;
		m_capacity = m_step;

		m_head = new NodeT[m_capacity];
	}

	template<typename T>
	MContainer::StaticList<T>::StaticList(const StaticList& t) noexcept
	{
		m_size = t.m_size;
		m_step = t.m_step;
		m_capacity = t.m_capacity;

		m_head = new NodeT[m_capacity];

		std::memcpy(m_head, t.m_head, NodeSize);

	}

	template<typename T>
	MContainer::StaticList<T>::StaticList(StaticList&& t) noexcept
	{
		m_size = t.m_size;
		m_step = t.m_step;
		m_capacity = t.m_capacity;
		m_head = t.m_head;

		t.m_head = nullptr;
		t.m_size = 0;
		t.m_capacity = 0;
		t.m_step = m_stepUint;
	}

	template<typename T>
	StaticList<T>& MContainer::StaticList<T>::operator=(const StaticList& t) noexcept
	{
		m_size = t.m_size;
		m_step = t.m_step;
		m_capacity = t.m_capacity;

		m_head = new NodeT[m_capacity];

		std::memcpy(m_head, t.m_head, NodeSize);
	}

	template<typename T>
	StaticList<T>& MContainer::StaticList<T>::operator=(StaticList&& t) noexcept
	{
		m_size = t.m_size;
		m_step = t.m_step;
		m_capacity = t.m_capacity;
		m_head = t.m_head;

		t.m_head = nullptr;
		t.m_size = 0;
		t.m_capacity = 0;
		t.m_step = m_stepUint;
	}

	template<typename T>
	MContainer::StaticList<T>::~StaticList()
	{
		delete[]m_head;
	}

	template<typename T>
	StaticList<T>& MContainer::StaticList<T>::append(const T& t) noexcept
	{
		assert(m_capacity <= m_maxSize);
		NodeT* newNode = nullptr;
		if (m_size == 0)
		{
			newNode = m_head[1];
			newNode->data = t;
			newNode->next = 2;
			newNode->previous = 1;
			m_head->next = 1;
			m_head->previous = 1;
		}
		else
		{
			NodeT* tailNode = m_head[m_head->previous];
			newNode = m_head[tailNode->next];
			newNode->data = t;
			newNode->previous = m_head->previous;
			m_head->previous = tailNode->next;
		}
		m_size++;
		if (m_size >= m_capacity)
		{
			if (m_capacity + m_step >= m_maxSize)
				m_capacity = m_maxSize;
			else
				m_capacity += m_step;
			creatMemory();
		}

		for (uint i = 1; i < m_capacity; i++)
		{
			if (m_head[i]->previous == m_npos)
			{
				newNode->next = i;
				break;
			}
		}
		return*this;
	}

	template<typename T>
	StaticList<T>& MContainer::StaticList<T>::removeAt(uint pos) noexcept
	{
		if (pos >= m_size)
			return *this;

		//if (pos == 0)
		//{
		//	NodeT * node = m_head[m_head->next];
		//	if (node->next != m_nPos)
		//	{
		//		m_head->next = node->next;
		//		m_head[node->next]->previous = 0;
		//	}
		//	node->next = m_nPos;
		//	node->previous = m_nPos;
		//}
		if (pos == m_size - 1)
		{
			NodeT* tail = m_head[m_head->previous];
			m_head[tail->previous]->next = m_nPos;
			m_head->previous = tail->previous;
			tail->previous = m_nPos;
		}
		else
		{

		}
		m_size--;
		return *this;
	}

	template<typename T>
	StaticList<T>& MContainer::StaticList<T>::removeAt(uint pos, uint len) noexcept
	{

	}

	template<typename T>
	StaticList<T>& MContainer::StaticList<T>::insert(uint pos, const T& t) noexcept
	{

	}

	template<typename T>
	const T& MContainer::StaticList<T>::at(uint pos) const
	{
		_throw(pos >= m_size);
		int index = m_head->next;
		for (size_t i = 0; i <= pos; i++)
		{
			index = m_head[index]->next;
		}

		return m_head[index]->data;
	}

	template<typename T>
	void MContainer::StaticList<T>::clear() noexcept
	{
		m_size = 0;
		m_step = m_stepUint;
		m_capacity = m_step;
		delete[]m_head;
		m_head = new NodeT[m_capacity];
	}

	template<typename T>
	StaticList<T>& MContainer::StaticList<T>::removeOne(const T& t, uint pos /*= 0*/, bool onlyOne /*= true*/) noexcept
	{

	}

	template<typename T>
	void MContainer::StaticList<T>::swap(uint first, uint second) noexcept
	{
		_throw(first >= m_size);
		_throw(second >= m_size);

		if (first == second)
			return;

		int  lNode, rNode;
		int index = m_head->next;
		for (uint i = 0; i <= first || i <= second; i++)
		{
			if (i == first)
				lNode = i;
			if (i == second)
				rNode = i;
			index = m_head[index]->next;
		}

		T t = m_head[lNode]->data;
		m_head[lNode]->data = m_head[rNode]->data;
		m_head[rNode]->data = t;
	}

}





#endif