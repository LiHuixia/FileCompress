#pragma once
#include"Heap.h"
#include<iostream>
#include<assert.h>
#include<string>
using namespace std;

template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode * _left;
	HuffmanTreeNode * _right;
	T _weight; //Ȩֵ
	HuffmanTreeNode(const T& x)
		:_weight(x)
		, _left(NULL)
		, _right(NULL)
	{}
};

template<class T>
class HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;
public:
	HuffmanTree()
	{}
	HuffmanTree(const T* a, size_t n,const T& invalid) 
	{
		//ѡ����С��������
		struct NodeCompare
		{
			bool operator()(const Node *r, const Node *l)
			{
				return (l->_weight < r->_weight);
			}
		};
		Heap<Node*, NodeCompare> minHeap;
		for (size_t i = 0; i < n; ++i)
		{
			if (a[i]!=invalid)
			minHeap.Push(new Node(a[i]));
		}
		while (minHeap.Size()>1) //���ʣһ����������
		{
			Node *left = minHeap.Top();
			minHeap.Pop();
			Node* right = minHeap.Top();
			minHeap.Pop();
			Node* parent = new Node(left->_weight + right->_weight);
			parent->_left = left;
			parent->_right = right;
			minHeap.Push(parent);
		}
		_root = minHeap.Top();
	}
	~HuffmanTree()
	{
		Destory(_root);
		_root = NULL;
	}
protected:
	void Destory(Node * root)
	{
		if (root == NULL)
		{
			return;
		}
		Destory(root->_left);
		Destory(root->_right);
		delete root;
	}
protected:
	Node* _root;
};
typedef unsigned long long LongType;
struct CharInfo
{
	unsigned char _ch;
	LongType _count; //�ַ����ֵĴ���
	string _code;//��Ӧ�Ĺ���������
	CharInfo(LongType count = 0)
		:_count(count)
		, _ch(0)
	{}
	bool operator!=(const CharInfo& info)const
	{
		return _count != info._count;
	}
	bool operator<(const CharInfo& info)const
	{
		return _count < info._count;
	}
	CharInfo operator+(const CharInfo& info)const
	{
		return CharInfo(_count + info._count);
	}
		
};

template<class T>
class FileCompress
{
	typedef HuffmanTreeNode<T> Node;
public:
	void Compress(const char* filename)
	{
		FILE *fout = fopen(filename, "r");
		assert(fout);
		//ͳ���ļ����ַ����ֵĴ���
		char ch = fgetc(fout);
		while (ch != EOF)
		{
			_info[(unsigned char)ch]._count++;
		}
	}
protected:
	CharInfo _info[256];
};
void Huffmantest()
{
	int a[] = { 1, 3, 5, 2, 4, 6 };
	HuffmanTree<int> hu(a,sizeof(a)/sizeof(a[0]),'#');
}