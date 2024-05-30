#pragma once
#ifndef MTree_h 
#define MTree_h

#include "containerBase.h"
#include "Queue.hpp"
#include "Stack.hpp"
namespace MContainer
{
	template<typename T>
	class Tree
	{
	protected:
		Tree() = default;
		virtual ~Tree() = default;

		virtual const Tree& root()const = 0;
		virtual Tree& root() = 0;
		virtual const Tree& left()const = 0;
		virtual const Tree& right()const = 0;
		virtual Tree& left() = 0;
		virtual Tree& right() = 0;
		virtual const T& data()const = 0;
		virtual T& data() = 0;
		virtual bool valid()const { return false };
	};


	template<typename T, typename Fun>
	void preorderTraversal(const Tree<T>& node, Fun fun)
	{
		if (!node.valid())
			return;
		if (fun(node.data()))
			return;
		preorderTraversal(node.left(), fun);
		preorderTraversal(node.right(), fun);

	}

	template<typename T, typename Fun>
	void inorderTraversal(const Tree<T>& node, Fun fun)
	{
		if (!node.valid())
			return;

		inorderTraversal(node.left(), fun);
		if (fun(node.data()))
			return;
		inorderTraversal(node.right(), fun);

	}

	template<typename T, typename Fun>
	void postorderTraversal(const Tree<T>& node, Fun fun)
	{
		if (!node.valid())
			return;
		postorderTraversal(node.left(), fun);
		postorderTraversal(node.right(), fun);
		if (fun(node.data()))
			return;
	}

	template<typename T, typename Fun>
	void levelTraversal(const Tree<T>& root, Fun fun)
	{
		SQueue<Tree<T>*> queue;
		queue.enqueue(&root);

		while (!queue.isEmpty())
		{
			Tree<T>* node = queue.dequeue();
			if (!node->valid())
				continue;
			if (fun(node->data()))
				return;
			if (node->left().valid())
				queue.enqueue(&node->left());

			if (node->right().valid())
				queue.enqueue(&node->right());
		}

	}


	template<typename T, typename Fun>
	void preorderTraversalV2(const Tree<T>& node, Fun fun)
	{
		if (!node.valid())
			return;
		Stack<Tree<T>*> stack;
		stack.push(&node);

		while (!stack.isEmpty())
		{
			Tree<T>* treeNode = stack.pop();
			if (fun(treeNode->data))
				return;

			if (treeNode->right().valid())
				stack.push(&treeNode->right());

			if (treeNode->left().valid())
				stack.push(&treeNode->left());
		}

	}

	template<typename T, typename Fun>
	void inorderTraversalV2(const Tree<T>& node, Fun fun)
	{
		if (!node.valid())
			return;
		Stack<Tree<T>*> stack;
		Tree<T>* currentNode = &node;

		while (currentNode->valid() || !stack.isEmpty())
		{
			while (currentNode->valid())
			{
				stack.push(currentNode);
				currentNode = &currentNode->left();
			}

			currentNode = &stack.pop();
			if (currentNode->data())
				return;

			currentNode = &currentNode->right();
		}


	}

	template<typename T, typename Fun>
	void postorderTraversalV2(const Tree<T>& node, Fun fun)
	{
		if (!node.valid())
			return;
		Stack<Tree<T>*> stack1, stack2;
		stack1.push(&node);

		while (!stack1.isEmpty())
		{
			Tree<T> * currentNode = stack1.pop();
			stack2.push(currentNode);

			if(currentNode->left().valid)
				stack1.push(&currentNode->left());
			if (currentNode->right().valid)
				stack1.push(&currentNode->right());
		}

		while (!stack2.isEmpty())
		{
			Tree<T> * currentNode = stack2.pop();
			if (fun(currentNode->data()))
				return;
		}

	}

}


#endif