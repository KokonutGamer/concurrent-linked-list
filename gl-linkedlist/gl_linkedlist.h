#ifndef GL_LINKEDLIST_H
#define GL_LINKEDLIST_H

#include <pthread.h>

typedef struct GLNode {
  int data;
  struct GLNode *next;
} GLNode;

typedef struct GLLinkedList {
  GLNode *head;
  pthread_mutex_t lock;
} GLLinkedList;

/**
 * Factory method for creating a global-locked linked list node.
 */
GLNode *createNode(int data);

// ==================== LINKED LIST FUNCTIONS ====================

// Create lists

/**
 * Initializes an empty linked list.
 */
void initList(GLLinkedList *list);

// Delete lists

/**
 * Destroys a linked list.
 */
void destroyList(GLLinkedList *list);

// Create nodes

/**
 * Appends a new node to the specified linked list.
 */
void append(GLLinkedList *list, int data);

/**
 * Inserts a node at the specified index.
 */
void insertNode(GLLinkedList *list, int index, int data);

/**
 * Inserts a node at the head of the linked list.
 */
void insertHead(GLLinkedList *list, int data);

/**
 * Inserts a node at the tail of the linked list. Note that this method is the
 * same as the append method.
 */
void insertTail(GLLinkedList *list, int data);

// Read nodes

/**
 * Retrieves the size of the linked list.
 */
int size(GLLinkedList *list);

/**
 * Retrieves the value of the node at the specified index.
 */
int get(GLLinkedList *list, int index);

/**
 * Retrieves the value of the node at the head of the linked list.
 */
int front(GLLinkedList *list);

/**
 * Retrieves the value of the node at the tail of the linked list.
 */
int back(GLLinkedList *list);

/**
 * Prints the linked list's contents.
 */
void printList(GLLinkedList *list);

// Delete nodes

/**
 * Frees all data in the entire list.
 */
void clearList(GLLinkedList *list);

/**
 * Deletes the node at the specified index.
 */
void removeNode(GLLinkedList *list, int index);

/**
 * Deletes the node at the head of the linked list.
 */
void removeHead(GLLinkedList *list);

/**
 * Deletes the node at the tail of the linked list.
 */
void removeTail(GLLinkedList *list);

#endif // GL_LINKEDLIST_H
