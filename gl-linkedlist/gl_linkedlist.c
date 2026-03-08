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

void append(GLLinkedList *list, int data) {
  // exit early if the list is null
  if (list == NULL) {
    return;
  }

  // acquire the lock on the list first
  pthread_mutex_lock(&list->lock);

  // immediately append the node if the head is null
  if (list->head == NULL) {
    list->head = createNode(data);
    pthread_mutex_unlock(&list->lock);
    return;
  }

  // traverse the list starting at the head
  GLNode *curr = list->head;

  while (curr->next != NULL) {
    curr = curr->next;
  }

  // append the new node to the end of the list
  curr->next = createNode(data);
  pthread_mutex_unlock(&list->lock);
}

void insertNode(GLLinkedList *list, int index, int data) {
  // exit early if the list is null
  if (list == NULL) {
    return;
  }

  // acquire the lock on the list first
  pthread_mutex_lock(&list->lock);

  if (index == 0) {
    // insert the node as the new head of the list
    GLNode *next = list->head;
    list->head = createNode(data);
    list->head->next = next;
    pthread_mutex_unlock(&list->lock);
    return;
  }

  // check if the head is null before traversing the list; index out of bounds
  if (list->head == NULL) {
    pthread_mutex_unlock(&list->lock);
    return;
  }

  // decrement the index; the neighbor of the current is the target index
  index--;
  GLNode *curr = list->head;
  while (curr->next != NULL && index != 0) {
    curr = curr->next;
    index--;
  }

  if (index != 0) {
    // relinquish the lock on the list; index out of bounds
    pthread_mutex_unlock(&list->lock);
    return;
  }

  // insert the new node now that we've found where to insert at
  GLNode *next = curr->next;
  curr->next = createNode(data);
  curr->next->next = next;
  pthread_mutex_unlock(&list->lock);
}

void insertHead(GLLinkedList *list, int data) { insertNode(list, 0, data); }

void insertTail(GLLinkedList *list, int data) { append(list, data); }

int size(GLLinkedList *list) {
  if (list == NULL) {
    return 0;
  }

  // acquire the lock on the list first
  pthread_mutex_lock(&list->lock);

  // check if the head is null before counting the size
  if (list->head == NULL) {
    pthread_mutex_unlock(&list->lock);
    return 0;
  }

  // size must be greater than or equal to 1 now
  int count = 1;

  // traverse the list starting at the head
  GLNode *curr = list->head;
  while (curr->next != NULL) {
    count++;
    curr = curr->next;
  }

  // relinquish the lock on the list
  pthread_mutex_unlock(&list->lock);
  return count;
}

int get(GLLinkedList *list, int index) {
  // TODO implement error handling
  if (list == NULL) {
    return 0;
  }

  // acquire the lock on the list first
  pthread_mutex_lock(&list->lock);

  GLNode *curr = list->head;
  while (curr != NULL && index > 0) {
    curr = curr->next;
    index--;
  }

  // TODO implement error handling
  if (curr == NULL) {
    pthread_mutex_unlock(&list->lock);
    return 0;
  }

  // retrieve the current node's data
  int value = curr->data;
  pthread_mutex_unlock(&list->lock);
  return value;
}

int front(GLLinkedList *list) { return get(list, 0); }

int back(GLLinkedList *list) {
  // TODO implement error handling
  if (list == NULL) {
    return 0;
  }

  // acquire the lock on the list first
  pthread_mutex_lock(&list->lock);

  // TODO implement error handling
  // check if the head is null before traversing the list
  if (list->head == NULL) {
    pthread_mutex_unlock(&list->lock);
    return 0;
  }

  // traverse the list starting at the head
  GLNode *curr = list->head;

  while (curr->next != NULL) {
    curr = curr->next;
  }

  // retrieve the tail's data
  int value = curr->data;
  pthread_mutex_unlock(&list->lock);
  return value;
}

void printList(GLLinkedList *list) {
  // exit early if the list is null
  if (list == NULL) {
    return;
  }

  // acquire the lock on the list first
  pthread_mutex_lock(&list->lock);

  // traverse the list starting at the head
  GLNode *curr = list->head;

  while (curr != NULL) {
    printf("%d -> ", curr->data);
    curr = curr->next;
  }

  // print the end of the list
  printf("(null)\n");

  // relinquish the lock on the list
  pthread_mutex_unlock(&list->lock);
}

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

void removeNode(GLLinkedList *list, int index) {
  // exit early if the list is null
  if (list == NULL) {
    return;
  }

  // acquire the lock on the list first
  pthread_mutex_lock(&list->lock);

  // check if the head is null before removing any nodes
  if (list->head == NULL) {
    pthread_mutex_unlock(&list->lock);
    return;
  }

  if (index == 0) {
    // remove the head of the list
    GLNode *newHead = list->head->next;
    free(list->head);
    list->head = newHead;
    pthread_mutex_unlock(&list->lock);
    return;
  }

  // decrement the index; the neighbor of the current is the target index
  index--;
  GLNode *curr = list->head;
  while (curr->next != NULL && index != 0) {
    curr = curr->next;
    index--;
  }

  if (curr->next == NULL || index != 0) {
    // relinquish the lock on the list; index out of bounds
    pthread_mutex_unlock(&list->lock);
    return;
  }

  // free the node now that we've found its index
  GLNode *next = curr->next->next;
  free(curr->next);
  curr->next = next;
  pthread_mutex_unlock(&list->lock);
}

void removeHead(GLLinkedList *list) { removeNode(list, 0); }

void removeTail(GLLinkedList *list) {
  // exit early if the list is null
  if (list == NULL) {
    return;
  }

  // acquire the lock on the list first
  pthread_mutex_lock(&list->lock);

  // check if the head is null before removing any nodes
  if (list->head == NULL) {
    pthread_mutex_unlock(&list->lock);
    return;
  }

  // check if the head is also the tail
  if (list->head->next == NULL) {
    free(list->head);
    list->head = NULL;
    pthread_mutex_unlock(&list->lock);
    return;
  }

  // traverse the list starting at the head
  GLNode *curr = list->head;

  // the current node's neighbor should never be null
  while (curr->next->next != NULL) {
    curr = curr->next;
  }

  // free the current node's neighbor as it's the last in the list
  free(curr->next);
  curr->next = NULL;
  pthread_mutex_unlock(&list->lock);
}
