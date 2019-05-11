#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "skiplist.h"

#define max(a,b) (a)>(b)?(a):(b)


SkipList::SkipList(int maxLevel)
{
	mMaxLevel = maxLevel;
	mSize = 0;
	mTopLevel = -1;
	mSeed = INT_MAX;
	mNodeHeaders = new Node*[maxLevel];

	Node *down = NULL;
	for (int i = 0; i < maxLevel; i++)
	{
		mNodeHeaders[i] = new Node(0);
		mNodeHeaders[i]->Dlink = down;
		down = mNodeHeaders[i];
	}
}

SkipList::~SkipList()
{
	for (int i = 0; i < mMaxLevel; i++)
	{
		if (mNodeHeaders[i])
		{
			Node *p = mNodeHeaders[i];
			while (p)
			{
				Node *t = p->Flink;
				delete p;
				p = t;
			}
		}
	}
	delete[] mNodeHeaders;
}

void SkipList::Insert(int data) /* O(n) + O(log n) */
{
	int level = 0;
	Node *down = NULL, *current = NULL;

	current = Insert(mNodeHeaders[level++], data); /* O(n) */
	current->Dlink = NULL;
	down = current;

	while ((Random() % 2 == 1) && (level < mMaxLevel)) /* O(log n) */
	{
		current = Insert(mNodeHeaders[level++], data);
		current->Dlink = down;
		down = current;
	}
	mTopLevel = max(mTopLevel, level - 1);
	mSize++;
}

Node *SkipList::Find(int data) /* O(log n) */
{
	if (mSize == 0) return NULL;

	int cmpTimes = 0; /* Ԫ�رȽϴ��� */

	Node *p = mNodeHeaders[mTopLevel]->Flink;
	while (p)
	{
		while (p && p->Flink && p->Data < data)
		{
			p = p->Flink;
			cmpTimes++;
		}

		if (p == NULL) return NULL;
		
		if (p->Data == data)
		{
			cmpTimes++;
			//printf("%d(cmp times: %d)\n", data, cmpTimes);
			return p;
		}
		
		if (p->Data < data)
		{
			p = p->Dlink; /* ֱ�ӽ�����һ�� */
		}
		else
		{
			p = p->Blink->Dlink; /* ����һ��λ�ú��ٽ�����һ�� */
		}

		if (p && p->Blink == NULL)
		{
			p = p->Flink; /* ���pָ��ͷ�ڵ�, �����䲻��������, ����Ҫ��p�ƶ�������ĵ�һ�����������ݵĽڵ� */
		}
	}

	return NULL;
}

bool SkipList::Delete(int data) /* O(log n) */
{
	Node *p = Find(data);
	if (p == NULL) return false;

	/* �ӵ�ǰ��ֱ���0��·���ϵ�ֵΪdata�Ľڵ㶼Ҫɾ�� */
	while (p)
	{
		Node *prev = p->Blink;
		Node *next = p->Flink;
		Node *down = p->Dlink;
		prev->Flink = next;
		if (next)
		{
			next->Blink = prev;
		}
		delete p;

		if (prev->Blink == NULL && prev->Flink == NULL) /* �ò�ֻʣͷ�ڵ�, ����ڵ��ѱ�ɾ�� */
		{
			mTopLevel--;
		}
		p = down;
	}
	mSize--;
	return true;
}

int SkipList::Size()
{
	return mSize;
}

void SkipList::Display()
{
	printf("+--------------------------------------------+\n");

	for (int i = mTopLevel; i >= 0; i--)
	{
		if (mNodeHeaders[i] && mNodeHeaders[i]->Flink)
		{
			printf("L%d (%d): ", i, mNodeHeaders[i]->Data);
			PrintNodes(mNodeHeaders[i]);
		}
	}

	printf("+--------------------------------------------+\n");
}

int SkipList::Random()
{
	unsigned int c = (unsigned int)clock();
	unsigned int t = (unsigned int)time(NULL);
	mSeed += c * t;
	srand(mSeed);
	return rand();
}

Node *SkipList::Insert(Node *head, int data)
{
	if (head == NULL)
	{
		head = new Node(0);
	}

	Node *p = head;
	while (p->Flink && p->Flink->Data < data)
	{
		p = p->Flink;
	}

	Node *node = new Node(data);
	node->Flink = p->Flink;
	node->Blink = p;
	p->Flink = node;
	if (node->Flink)
	{
		node->Flink->Blink = node;
	}

	head->Data++;

	return node;
}

void SkipList::PrintNodes(Node *head)
{
	if (head == NULL) return;

	Node *p = head->Flink;
	while (p && p->Flink)
	{
		printf("%d -> ", p->Data);
		p = p->Flink;
		if (p->Dlink)
		{
			assert(p->Dlink->Data == p->Data);
		}
	}
	if (p)
	{
		printf("%d\n", p->Data);
		if (p->Dlink)
		{
			assert(p->Dlink->Data == p->Data);
		}
	}
}
