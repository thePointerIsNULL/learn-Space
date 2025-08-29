/**
* @file			LRU.hpp
* @brief		LRU缓存算法，包含LRU容器定义
* @author       caoliguo
* @date			2025/08/26
* @copyright
*/
#pragma once
#include <unordered_map>
#include <memory>
using uint = unsigned int;

//循环双链表+hashMap
template <typename Key, typename Value>
class LRUContainers
{
public:
	struct LRUNode
	{
		Key key_;
		Value value_;
		LRUNode* preNode_ = nullptr;
		LRUNode* nextNode_ = nullptr;
		LRUNode(Key&& key, Value&& value) :key_(std::move(key)), value_(std::move(value)) {};
		LRUNode(const Key& key, const Value& value) :key_(key), value_(value) {};
		LRUNode() {};
	};
	using NodeMap = std::unordered_map<Key, LRUNode*>;
	LRUContainers();
	~LRUContainers();
	bool get(const Key& key, Value& value);
	void put(Key&& key, Value&& value);
	void put(const Key& key, const Value& value);
	uint size()const;
	void clear();
	void removeLast();
private:
	void moveNodeToHead(LRUNode* node);
	void insertToHead(LRUNode* node);
private:
	NodeMap m_nodeMap;
	LRUNode* m_sentryNode = nullptr;
	uint m_size = 0;
};

template <typename Key, typename Value>
void LRUContainers<Key, Value>::removeLast()
{
	if (m_size == 0)
	{
		return;
	}
	LRUNode* lastNode = m_sentryNode->preNode_;

	lastNode->preNode_->nextNode_ = m_sentryNode;
	m_sentryNode->preNode_ = lastNode->preNode_;

	m_nodeMap.erase(lastNode->key_);
	m_size--;

	delete lastNode;
}

template <typename Key, typename Value>
void LRUContainers<Key, Value>::insertToHead(LRUNode* node)
{
	node->nextNode_ = m_sentryNode->nextNode_;
	node->preNode_ = m_sentryNode;

	m_sentryNode->nextNode_ = node;
	node->nextNode_->preNode_ = node;
}

template <typename Key, typename Value>
void LRUContainers<Key, Value>::moveNodeToHead(LRUNode* node)
{
	if (node->preNode_ == m_sentryNode)
	{
		return;
	}

	node->preNode_->nextNode_ = node->nextNode_;
	node->nextNode_->preNode_ = node->preNode_;

	insertToHead(node);
}

template <typename Key, typename Value>
void LRUContainers<Key, Value>::clear()
{
	LRUNode* node = m_sentryNode->nextNode_;
	while (node != m_sentryNode)
	{
		node = node->nextNode_;
		delete node->preNode_;
	}
	m_size = 0;
	m_nodeMap.clear();
}

template <typename Key, typename Value>
uint LRUContainers<Key, Value>::size() const
{
	return m_size;
}

template <typename Key, typename Value>
void LRUContainers<Key, Value>::put(Key&& key, Value&& value)
{
	LRUNode* node = nullptr;
	if (m_nodeMap.find(key) == m_nodeMap.end())
	{
		node = new LRUNode(std::move(key), std::move(value));
		insertToHead(node);
		m_size++;
	}
	else
	{
		LRUNode* node = m_nodeMap[key];
		moveNodeToHead(node);
	}
	m_nodeMap[key] = node;
}

template <typename Key, typename Value>
void LRUContainers<Key, Value>::put(const Key& key, const Value& value)
{
	LRUNode* node = nullptr;
	if (m_nodeMap.find(key) == m_nodeMap.end())
	{
		node = new LRUNode(key, value);
		insertToHead(node);
		m_size++;
	}
	else
	{
		LRUNode* node = m_nodeMap[key];
		moveNodeToHead(node);
	}
	m_nodeMap[key] = node;
}

template <typename Key, typename Value>
bool LRUContainers<Key, Value>::get(const Key& key, Value& value)
{
	auto itor = m_nodeMap.find(key);
	if (itor == m_nodeMap.end())
	{
		return false;
	}

	LRUNode* node = itor->second;
	value = node->value_;

	moveNodeToHead(node);
	return true;
}

template <typename Key, typename Value>
LRUContainers<Key, Value>::LRUContainers()
{
	m_sentryNode = new LRUNode({}, {});
	m_sentryNode->preNode_ = m_sentryNode;
	m_sentryNode->nextNode_ = m_sentryNode;
}

template <typename Key, typename Value>
LRUContainers<Key, Value>::~LRUContainers()
{
	clear();
	delete m_sentryNode;
	m_sentryNode = nullptr;
}

template <typename Key, typename Value>
class LRUCache
{
public:
	LRUCache(uint capacity);
	~LRUCache();
	bool get(const Key& key, Value& value);
	void put(Key&& key, Value&& value);
	void put(const Key& key, const Value& value);
private:
	LRUContainers<Key, Value> m_containers;
	uint m_capacity;
};

template <typename Key, typename Value>
void LRUCache<Key, Value>::put(const Key& key, const Value& value)
{
	m_containers.put(key, value);
	if (m_containers.size() > m_capacity)
	{
		m_containers.removeLast();
	}
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::put(Key&& key, Value&& value)
{
	m_containers.put(std::move(key), std::move(value));
	if (m_containers.size() > m_capacity)
	{
		m_containers.removeLast();
	}
}

template <typename Key, typename Value>
bool LRUCache<Key, Value>::get(const Key& key, Value& value)
{
	return m_containers.get(key, value);
}

template <typename Key, typename Value>
LRUCache<Key, Value>::~LRUCache()
{

}

template <typename Key, typename Value>
LRUCache<Key, Value>::LRUCache(uint capacity)
	:m_capacity(capacity)
{

}
