#pragma once
#include<stdlib.h>

typedef int QUEUE_ITEM;
typedef char String[256];

typedef struct node_type {
	QUEUE_ITEM info;
	int priority;
	String code;
	struct node_type* next;
}Node;
typedef struct {
	Node* head, * rear;
}PriorityQueue;

void Queue_Init(PriorityQueue* q);
void Insert(PriorityQueue* q, QUEUE_ITEM x, int p);
int Queue_Empty(PriorityQueue q);
int Queue_Full(PriorityQueue q);
QUEUE_ITEM Remove(PriorityQueue* q);
QUEUE_ITEM Queue_Top(PriorityQueue q);