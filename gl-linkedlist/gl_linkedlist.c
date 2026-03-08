#include "gl_linkedlist.h"

#include <stdio.h>
#include <stdlib.h>

GLNode *createNode(int data) {
  GLNode *node = (GLNode *)malloc(sizeof(GLNode));
  node->data = data;
  node->next = NULL;
  return node;
}

void initList(GLLinkedList **list) {
  *list = (GLLinkedList *)malloc(sizeof(GLLinkedList));

  // initialize the list with a null head and the global lock
  (*list)->head = NULL;
  pthread_mutex_init(&(*list)->lock, NULL);
}

void destroyList(GLLinkedList *list) {
  // exit early if the list is null
  if (list == NULL) {
    return;
  }
  clearList(list);
  pthread_mutex_destroy(&list->lock);
  free(list);
}

void append(GLLinkedList *list, int data);

void insertNode(GLLinkedList *list, int index, int data);

void insertHead(GLLinkedList *list, int data);

void insertTail(GLLinkedList *list, int data);

int size(GLLinkedList *list);

int get(GLLinkedList *list, int index);

int front(GLLinkedList *list);

int back(GLLinkedList *list);

void printList(GLLinkedList *list);

void clearList(GLLinkedList *list) {
  // exit early if the list is null
  if (list == NULL) {
    return;
  }

  // acquire the lock on the list first
  pthread_mutex_lock(&list->lock);

  if (list->head == NULL) {
    // relinquish the lock on the list
    pthread_mutex_unlock(&list->lock);
    return;
  }

  // traverse the list starting at the head
  GLNode *curr = list->head;
  list->head = NULL;

  // iterate over the list and free the current node
  while (curr != NULL) {
    GLNode *next = curr->next;
    free(curr);
    curr = next;
  }

  // relinquish the lock on the list
  pthread_mutex_unlock(&list->lock);
}

void removeNode(GLLinkedList *list, int index);

void removeHead(GLLinkedList *list);

void removeTail(GLLinkedList *list);
