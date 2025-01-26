#include<stdio.h>
#include<stdlib.h>
#include "PriorityQueue.h"

void Queue_Init(PriorityQueue* q)
{
	q->head = q->rear = NULL;
}

void Insert(PriorityQueue* q, QUEUE_ITEM x, int p)
{
	Node* a = (Node*)malloc(sizeof(Node));

	PriorityQueue* temp = q;
	a->info = x;
	a->priority = p;
	a->next = NULL;
	if (Queue_Empty(*q))
		q->head = q->rear = a;

	else
	{
		if (a->priority < q->head->priority)
		{
			a->next = q->head;
			q->head = a;
		}
		else
		{
			while (temp->head->next && a->priority > temp->head->next->priority)
			{
				temp->head = temp->head->next;
			}
			if (temp->head->next)
			{
				a->next = temp->head->next;
				temp->head->next = a;
			}
			else
				temp->head->next = a;
		}
	}
}

int Queue_Empty(PriorityQueue q)
{
	return (q.head == NULL);
}


int Queue_Full(PriorityQueue q)
{
	return 0;
}

QUEUE_ITEM Remove(PriorityQueue* q)
{
	QUEUE_ITEM x = NULL;
	Node* ptr = q->head;
	if (!Queue_Empty(*q))
	{
		x = q->head->info;
		q->head = q->head->next;
		if (q->head == NULL)
			q->rear = NULL;
		free(ptr);
		return x;
	}
	return x;
}

QUEUE_ITEM Queue_Top(PriorityQueue q)
{
	if (!Queue_Empty(q))
		return q.head->info;
}