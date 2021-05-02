struct node{
	int value;
	struct node *next;
};
typedef struct node Node;
int appendE(Node* list,int value);
int appendS(Node* list,int value);
int removeNodeE(Node* list);
int removeNodeS(Node* list);
int freeList(Node* list);
