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
//热节点会在链表表头
//超过容量时会移除链表尾部的节点
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
		LRUNode(const Key& key,const Value&& value) :key_(key ), value_(value) {};
	};
	LRUContainers(uint size);
	~LRUContainers();
private:
	NodeMap m_nodeMap;
	LRUNode* m_sentryNode = nullptr;
	uint m_size = 0;
};

template <typename Key, typename Value>
LRUContainers<Key, Value>::LRUContainers(uint size)
	:m_size(size)
{
	m_sentryNode = new LRUNode({}, {});
	m_sentryNode->preNode_ = m_sentryNode;
	m_sentryNode->nextNode_ = m_sentryNode;
}

template <typename Key, typename Value>
LRUContainers<Key, Value>::~LRUContainers()
{

}

