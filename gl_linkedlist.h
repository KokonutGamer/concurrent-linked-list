#ifndef GL_LINKEDLIST_H
#define GL_LINKEDLIST_H

#include <pthread.h>

typedef struct GLNode {
  int data;
  struct GLNode *next;
} GLNode;

typedef struct GLLinkedList {
  pthread_mutex_t lock;
  GLNode *head;
} GLLinkedList;

/**
 * Factory method for creating a global-locked linked list node.
 */
GLNode *createNode(int data);

// ==================== LINKED LIST FUNCTIONS ====================

// Create

/**
 * Appends a new node to the specified linked list.
 */
void append(GLLinkedList *list, int data);

/**
 * Inserts a node at the specified index.
 */
void insert(GLLinkedList *list, int index, int data);

/**
 * Inserts a node at the head of the linked list.
 */
void insertHead(GLLinkedList *list, int data);

/**
 * Inserts a node at the tail of the linked list. Note that this method is the
 * same as the append method.
 */
void insertTail(GLLinkedList *list, int data);

// Read

/**
 * Retrieves the size of the linked list.
 */
int size(const GLLinkedList *list);

/**
 * Retrieves the value of the node at the specified index.
 */
int get(const GLLinkedList *list, int index);

/**
 * Retrieves the value of the node at the head of the linked list.
 */
int front(const GLLinkedList *list);

/**
 * Retrieves the value of the node at the tail of the linked list.
 */
int back(const GLLinkedList *list);

/**
 * Prints the linked list's contents.
 */
void printList(const GLLinkedList *list);

// Delete

/**
 * Frees all data in the entire list.
 */
void clear(GLLinkedList *list);

/**
 * Deletes the node at the specified index.
 */
void remove(GLLinkedList *list, int index);

/**
 * Deletes the node at the head of the linked list.
 */
void removeHead(GLLinkedList *list);

/**
 * Deletes the node at the tail of the linked list.
 */
void removeTail(GLLinkedList *list);

#endif // GL_LINKEDLIST_H
