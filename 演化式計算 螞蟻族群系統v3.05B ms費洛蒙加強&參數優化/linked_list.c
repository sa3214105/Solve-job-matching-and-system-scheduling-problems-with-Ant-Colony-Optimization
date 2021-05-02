#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"
int appendE(Node* list,int value){
	Node* buf=malloc(sizeof(Node));
	buf->value=value;
	buf->next=NULL;
	if(list==NULL){
		list = buf;
		return 0;
	}else{
		Node* current;	
		current=list;
		while(current->next != NULL) {
			current = current->next;
		}
		current->next = buf;
		return 0;
	}
	return 0;
}
int appendS(Node* list,int value){
	Node* buf=malloc(sizeof(Node));
	buf->value=value;
	list->next=buf;
	list=buf;
	return 0;
}
int removeNodeE(Node* list){
	Node* current=list;
	while(current->next->next != NULL) {
		current = current->next;
	}
	free(current->next);
	current->next=NULL;
	return 0;
}
int removeNodeS(Node* list){
	Node* current=list->next;
	free(list);
	list=current;
	return 0;
}
int freeList(Node* list){
	while(list==NULL){
		Node* current=list->next;
		free(list);
		list=current;
	}
	return 0;
}
