#ifndef _TREE
#define _TREE

typedef struct
{
	char keys[20];
	int  repetition;
} CharFrequency;

typedef CharFrequency Tree_Info;

struct node
{
	Tree_Info info;
	struct node* left, * right;
};

typedef struct node BinTree, * BinTreePtr;


// Prototypes

void InitTree(BinTreePtr* t);
BinTreePtr BuildTree(BinTreePtr L, BinTreePtr R, Tree_Info x);
void SetRight(BinTreePtr* t, Tree_Info x);
void SetLeft(BinTreePtr* t, Tree_Info x);
BinTreePtr TreeRsub(BinTreePtr t);
BinTreePtr TreeLsub(BinTreePtr t);
void ChangeLsub(BinTreePtr* t, BinTreePtr new_tree);
void ChangeRsub(BinTreePtr* t, BinTreePtr new_tree);
Tree_Info GetRoot(BinTreePtr t);
void SetRoot(BinTreePtr* t, Tree_Info x);
int IsEmptyTree(BinTreePtr t);


#endif
