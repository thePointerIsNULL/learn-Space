/**
* @file			LRU.hpp
* @brief		LRU缓存算法，包含LRU容器定义
* @author       caoliguo
* @date			2025/08/26
* @copyright
*/
#pragma once
#include <unordered_map>
using uint = unsigned int;

template <typename Key, typename Value>
class LRUContainers
{
public:
	using NodeMap = std::unordered_map<Key, Value>;
	struct LRUNode
	{
		Key key_;
		value value_;
		LRUNode* preNode_ = nullptr;
		LRUNode* nextNode_ = nullptr;
	};
	LRUContainers() = default;
	~LRUContainers();
private:
	NodeMap m_nodeMap;
	LRUNode* m_sentryNode = nullptr;
	uint m_size = 0;
};

template <typename Key, typename Value>
LRUContainers<Key, Value>::~LRUContainers()
{

}

