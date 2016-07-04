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
	T _weight; //权值
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
		//选出最小两个出来
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
		while (minHeap.Size()>1) //结点剩一个，构建好
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
	LongType _count; //字符出现的次数
	string _code;//对应的哈夫曼编码
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
		//统计文件中字符出现的次数
		char ch = fgetc(fout);
		while (ch != EOF)
		{
			_info[(unsigned char)ch]._count++;
		}
		//构建哈夫曼树
		CharInfo invalid(0);
		HuffmanTree<CharInfo> tree(_info, 256, invalid);
		//生成哈夫曼编码
		string code;
		GenerateHuffmanCode(tree.GetRootNode(), code);
		//文件压缩
		string CompressFilename = filename;
		CompressFilename += ".compress";
		FILE* fin = fopen(CompressFilename.c_str(), "wb"); //二进制写文件
		fseek(fout, 0, SEEK_SET);    //定义到文件起始位置
		ch = fgetc(fout);
		char value = 0;
		int size = 0;
		while (ch != EOF)
		{
			string &code = _info[(unsigned char)ch]._code;
			for (rsize_t i = 0; i < code.size(); ++i)
			{
				if (code[i] = '1')
				{
					value |= 1;
				}
				value <<= 1;
				++size;
				if (size == 8)   //满8位将哈夫曼编码写入对应的文件
				{
					fputc(value, fin);
					value = 0;
					size = 0;
				}
			}
			ch = fgetc(fout);//读取下一个字符的哈夫曼编码
			if (size > 0)    //表示还有位，补0
			{
				value << (8 - size);
				fputc(value, fin);
			}
			//写配置文件,方便压缩的时候重建哈夫曼树
			string configFile = filename;
			configFile += ".config";
			FILE* finconfig = fopen(configFile.c_str, "wb");
			assert(finconfig);
			char buffer[128];
			string line;
			for (int i = 0; i < 256; ++i)
			{
				if (_info[i]._count>0)
				{
					line += _info[i]._ch;
					line += ",";
					itoa(_info[i]._count, buffer, 10);
					line += buffer;
					line += '\n';
					fput(line.c_str(), finconfig);
					line.clear();
				}
			}
			fclose(fout);
			fclose(fin);
			fclose(finconfig);
		}
	}
protected:
	void GenerateHuffmanCode(HuffmanTreeNode<CharInfo>* root, string code)
	{
		if (root == NULL)
		{
			return;
		}
		if (root->_left == NULL&&root->_right == NULL)
		{
			_info[root->_weight._ch]._code = code;
		}
		GenerateHuffmanCode(root->_left, code + '0');
		GenerateHuffmanCode(root->_left, code + '1');
	}
protected:
	CharInfo _info[256];
};

void Huffmantest()
{
	int a[] = { 1, 3, 5, 2, 4, 6 };
	HuffmanTree<int> hu(a,sizeof(a)/sizeof(a[0]),'#');
}