#define _CRT_SECURE_NO_WARNING 1
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
	Node* GetRootNode()
	{
		return _root;
	}
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
			Node* parent = new Node((left->_weight) + (right->_weight));
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
	FileCompress()
	{
		for (int i = 0; i <= 255; i++)
		{
			_info[i]._count = 0;
			_info[i]._ch = i;
		}
	}
	void Compress(const char* filename)
	{
		FILE *fout = fopen(filename, "rb");
		assert(fout);
		//ͳ���ļ����ַ����ֵĴ���
		char ch = fgetc(fout);
		while (!feof(fout))
		{
			_info[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}
		//������������
		CharInfo invalid(0);
		HuffmanTree<CharInfo> tree(_info, 256, invalid);
		//���ɹ���������
		string code;
		GenerateHuffmanCode(tree.GetRootNode(), code);
		//�ļ�ѹ��
		string CompressFilename = filename;
		CompressFilename += ".compress";
		FILE* fin = fopen(CompressFilename.c_str(), "wb"); //������д�ļ�
		fseek(fout, 0, SEEK_SET);    //���嵽�ļ���ʼλ��
		ch = fgetc(fout);
		char value = 0;
		int size = 0;
		while (!feof(fout))
		{
			string &code = _info[(unsigned char)ch]._code;
			for (size_t i = 0; i < code.size(); ++i)
			{
				value <<= 1;
				if (code[i] == '1')
				{
					value |= 1;
				}

				if (++size == 8)   //��8λ������������д���Ӧ���ļ�
				{
					fputc(value, fin);
					value = 0;
					size = 0;
				}
			}
			ch = fgetc(fout);//��ȡ��һ���ַ��Ĺ���������
		}
			if (size!= 0)    //��ʾ����λ����0
			{
				value <<= (8 - size);
				fputc(value, fin);
			}
			//д�����ļ�,����ѹ����ʱ���ؽ���������
			string configFile = filename;
			configFile += ".config";
			FILE* finconfig = fopen(configFile.c_str(), "wb");
			assert(finconfig);
			char buffer[128];
			string line;
			for (int i = 0; i < 256; ++i)
			{
				if (_info[i]._count>0)
				{
					line += _info[i]._ch;
					line += ",";
					_itoa(_info[i]._count, buffer, 10);
					line += buffer;
					line += '\n';
				}
				fputs(line.c_str(), finconfig);
				line.clear();
			}
			fclose(fout);
			fclose(fin);
			fclose(finconfig);
	}

	void UnCompress(const char* filename)
	{
		string configfile = filename;
		configfile += ".config";
		FILE* foutconfig = fopen(configfile.c_str(), "rb");
		string str;
		LongType CharCount = 0;
		while (Readline(foutconfig, str))
		{
				_info[(unsigned char)str[0]]._count = atoi(str.substr(2).c_str());
				CharCount += _info[(unsigned char)str[0]]._count;
				str.clear();
		}
		CharInfo invalid(0);
		HuffmanTree<CharInfo>tree(_info, 256, invalid);
		CharCount = tree.GetRootNode()->_weight._count;
		string Compressfilename = filename;
		Compressfilename += ".compress";
		FILE* fout = fopen(Compressfilename.c_str(), "rb");
		assert(fout);
		string UnCompressfilename = filename;
		UnCompressfilename += ".uncompress";
		FILE* fin = fopen(UnCompressfilename.c_str(), "wb");
		assert(fin);
		char ch = fgetc(fout);
		HuffmanTreeNode<CharInfo>* root = tree.GetRootNode();
		HuffmanTreeNode<CharInfo>* cur = root;
		int pos = 7;
		while (!feof(fout))
		{
			if (ch&(1 << pos))
				cur = cur->_right;
			else
				cur = cur->_left;
			if (pos-- == 0)
			{
				pos = 7;
				ch = fgetc(fout);
			}
			if (cur->_left == NULL&&cur->_right == NULL)
			{
				fputc(cur->_weight._ch, fin);
				cur = root;
				if (--CharCount == 0)
				{
					break;
				}
				
			}
		}
		fclose(foutconfig);
		fclose(fout);
		fclose(fin);
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
		GenerateHuffmanCode(root->_right, code + '1');
	}

	bool Readline(FILE* &fout, string& str)
	{
		char ch = fgetc(fout);
		if (feof(fout))
			return false;
		while (!feof(fout))
		{
			str += ch;
			ch = fgetc(fout);
			if (ch == '\n')
			{
				break;
			}
		}
		return true;
	}
protected:
	CharInfo _info[256];
};

void TestFile()
{
	FileCompress<int> fc;
	fc.Compress("Input.BIG");
	//Input.BIG
}
void TestUnFileCompress()
{
	FileCompress<int> fc;
	fc.UnCompress("Input.BIG");
}