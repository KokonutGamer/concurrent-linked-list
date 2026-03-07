#ifndef HOH_LINKEDLIST_H
#define HOH_LINKEDLIST_H

#include <pthread.h>

typedef struct HOHNode {
  pthread_mutex_t lock;
  int data;
  struct HOHNode *next;
} HOHNode;

typedef struct HOHLinkedList {
  HOHNode *head;
  pthread_mutex_t printLock;
} HOHLinkedList;

/**
 * Factory method for creating a hand-over-hand linked list node.
 */
HOHNode *createNode(int data);

// ==================== LINKED LIST FUNCTIONS ====================

// Create lists

/**
 * Initializes an empty linked list.
 */
void initList(HOHLinkedList **list);

// Delete lists

/**
 * Destroys a linked list.
 */
void destroyList(HOHLinkedList *list);

// Create nodes

/**
 * Appends a new node to the specified linked list.
 */
void append(HOHLinkedList *list, int data);

/**
 * Inserts a node at the specified index.
 */
void insertNode(HOHLinkedList *list, int index, int data);

/**
 * Inserts a node at the head of the linked list.
 */
void insertHead(HOHLinkedList *list, int data);

/**
 * Inserts a node at the tail of the linked list. Note that this method is the
 * same as the append method.
 */
void insertTail(HOHLinkedList *list, int data);

// Read nodes

/**
 * Retrieves the size of the linked list.
 */
int size(HOHLinkedList *list);

/**
 * Retrieves the value of the node at the specified index.
 */
int get(HOHLinkedList *list, int index);

/**
 * Retrieves the value of the node at the head of the linked list.
 */
int front(HOHLinkedList *list);

/**
 * Retrieves the value of the node at the tail of the linked list.
 */
int back(HOHLinkedList *list);

/**
 * Prints the linked list's contents.
 */
void printList(HOHLinkedList *list);

// Delete nodes

/**
 * Frees all data in the entire list.
 */
void clearList(HOHLinkedList *list);

/**
 * Deletes the node at the specified index.
 */
void removeNode(HOHLinkedList *list, int index);

/**
 * Deletes the node at the head of the linked list.
 */
void removeHead(HOHLinkedList *list);

/**
 * Deletes the node at the tail of the linked list.
 */
void removeTail(HOHLinkedList *list);

#endif // HOH_LINKEDLIST_H
