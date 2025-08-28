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
	using NodeMap = std::unordered_map<Key, Value>;
	struct LRUNode
	{
		Key key_;
		Value value_;
		LRUNode* preNode_ = nullptr;
		LRUNode* nextNode_ = nullptr;
		LRUNode(Key&& key, Value&& value) :key_(std::move(key)), value_(std::move(value)) {};
		LRUNode(const Key& key, const Value& value) :key_(key), value_(value) {};
	};
	LRUContainers();
	~LRUContainers();
	bool get(const Key& key, Value& value)const;
	void put(Key&& key, Value&& value);
	uint size()const;
	void clear();
private:
	NodeMap m_nodeMap;
	LRUNode* m_sentryNode = nullptr;
	uint m_size = 0;
};

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
	if (m_nodeMap.find(key) == m_nodeMap.end())
	{
		LRUNode* node = new LRUNode(std::forward<Key>(key), std::forward<Value>(value));

		node->nextNode_ = m_sentryNode->nextNode_;
		node->preNode_ = m_sentryNode;

		node->nextNode_->preNode_ = node;
		node->preNode_->nextNode_ = node;
	}
	else
	{
		LRUNode* node = m_sentryNode->nextNode_;
		while (node != m_sentryNode)
		{
			if (node->key_ == key)
			{
				node->value_ = value;
				break;
			}
			node = node->nextNode_;
		}

		node->preNode_->nextNode_ = node->nextNode_;
		node->nextNode_->preNode_ = node->preNode_;

		node->nextNode_ = m_sentryNode->nextNode_;
		node->preNode_ = m_sentryNode;

		m_sentryNode->nextNode_->preNode_ = node;
		m_sentryNode->nextNode_ = node;

	}

	m_nodeMap[key] = value;
	m_size++;
}

template <typename Key, typename Value>
bool LRUContainers<Key, Value>::get(const Key& key, Value& value) const
{
	auto itor = m_nodeMap.find(key);
	if (itor == m_nodeMap.end())
	{
		return false;
	}

	value = itor->second;

	LRUNode* node = m_sentryNode->nextNode_;
	while (node->nextNode_ != m_sentryNode)
	{
		if (node->key_ == key)
		{
			break;
		}
		node = node->nextNode_;
	}

	if (node == m_sentryNode)
	{
		return false;
	}

	if (node->preNode_ == m_sentryNode)
	{
		return true;
	}

	node->preNode_->nextNode_ = node->nextNode_;
	node->nextNode_->preNode_ = node->preNode_;
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

