#include "hoh_linkedlist.h"

#include <stdio.h>
#include <stdlib.h>

HOHNode *createNode(int data) {
  HOHNode *node = (HOHNode *)malloc(sizeof(HOHNode));
  pthread_mutex_init(&node->lock, NULL);
  node->data = data;
  node->next = NULL;
  return node;
}

void initList(HOHLinkedList **list) {
  *list = (HOHLinkedList *)malloc(sizeof(HOHLinkedList));

  // initialize the list with a dummy head
  (*list)->head = createNode(0);
}

void destroyList(HOHLinkedList *list) {
  // exit early if the list is null
  if (list == NULL || list->head == NULL) {
    return;
  }
  clearList(list);
  pthread_mutex_destroy(&list->head->lock);
  free(list->head); // dummy node
  free(list);       // list data structure
}

void append(HOHLinkedList *list, int data) {
  // exit early if the list is null
  if (list == NULL || list->head == NULL) {
    return;
  }

  // traverse the list starting at the dummy node
  HOHNode *curr = list->head;

  // acquire the lock on the dummy node first
  pthread_mutex_lock(&curr->lock);

  while (curr->next != NULL) {
    // acquire the lock on the current node's neighbor
    HOHNode *next = curr->next;
    if (next != NULL) {
      // we can safely assume that this memory is still allocated for the
      // neighbor since deletion must occur on the second node of the pair,
      // NEVER on the first node
      pthread_mutex_lock(&next->lock);
      pthread_mutex_unlock(&curr->lock);

      // continue traversing the list
      curr = next;
    }
  }

  // append the new node to the end of the list
  curr->next = createNode(data);
  pthread_mutex_unlock(&curr->lock);
}

void insertNode(HOHLinkedList *list, int index, int data) {
  // exit early if the list is null
  if (list == NULL || list->head == NULL) {
    return;
  }

  // traverse the list starting at the dummy node
  HOHNode *curr = list->head;

  // acquire the lock on the dummy node first
  pthread_mutex_lock(&curr->lock);

  while (curr->next != NULL) {
    // acquire the lock on the current node's neighbor
    HOHNode *next = curr->next;
    if (next != NULL) {
      // we can safely assume that this memory is still allocated for the
      // neighbor since deletion must occur on the second node of the pair,
      // NEVER on the first node
      pthread_mutex_lock(&next->lock);

      if (index == 0) {
        // insert the new node now that we've found where to insert at
        curr->next = createNode(data);
        curr->next->next = next;

        // relinquish both (current and next) locks
        pthread_mutex_unlock(&curr->lock);
        pthread_mutex_unlock(&next->lock);
        return;
      }

      // relinquish the lock of the current node
      pthread_mutex_unlock(&curr->lock);

      // continue traversing the list
      curr = next;
      index--;
    }
  }

  // the current node's neighbor is null
  if (index == 0) {
    curr->next = createNode(data);
  }
  pthread_mutex_unlock(&curr->lock);
}

void insertHead(HOHLinkedList *list, int data) { insertNode(list, 0, data); }

void insertTail(HOHLinkedList *list, int data) { append(list, data); }

int size(HOHLinkedList *list);

int get(HOHLinkedList *list, int index);

int front(HOHLinkedList *list) {
  // TODO implement error handling
  if (list == NULL || list->head == NULL) {
    return 0;
  }

  // acquire the locks on the dummy and first nodes
  pthread_mutex_lock(&list->head->lock);

  // TODO implement error handling
  if (list->head->next == NULL) {
    pthread_mutex_unlock(&list->head->lock);
    return 0;
  }

  pthread_mutex_lock(&list->head->next->lock);

  // retrieve the value of the first node
  int value = list->head->next->data;

  // relinquish the locks on the dummy and first nodes
  pthread_mutex_unlock(&list->head->lock);
  pthread_mutex_unlock(&list->head->next->lock);

  return value;
}

int back(HOHLinkedList *list);

void printList(HOHLinkedList *list);

void clearList(HOHLinkedList *list) {
  // exit early if the list is null
  if (list == NULL || list->head == NULL) {
    return;
  }

  // acquire the lock on the dummy node first
  pthread_mutex_lock(&list->head->lock);

  // traverse the list starting at the dummy node's neighbor
  HOHNode *curr = list->head->next;
  list->head->next = NULL;

  if (curr == NULL) {
    // relinquish the dummy node's lock
    pthread_mutex_unlock(&list->head->lock);
    return;
  }

  // acquire the lock on the current node
  pthread_mutex_lock(&curr->lock);
  while (curr != NULL) {
    HOHNode *next = curr->next;
    if (next != NULL) {
      // acquire the lock on the current node's neighbor; we can safely assume
      // that this memory is still allocated for the neighbor since deletion
      // must occur on the second node of the pair, NEVER on the first node
      pthread_mutex_lock(&next->lock);
    }

    // free all memory related to the current node
    pthread_mutex_unlock(&curr->lock); // safe because of dummy node's lock
    pthread_mutex_destroy(&curr->lock);
    free(curr);
    curr = next;
  }

  // relinquish the dummy node's lock
  pthread_mutex_unlock(&list->head->lock);
}

void removeNode(HOHLinkedList *list, int index) {
  // exit early if the list is null
  if (list == NULL || list->head == NULL) {
    return;
  }

  // traverse the list starting at the dummy node
  HOHNode *curr = list->head;

  // acquire the lock on the dummy node first
  pthread_mutex_lock(&curr->lock);

  while (curr->next != NULL) {
    // acquire the lock on the current node's neighbor
    HOHNode *next = curr->next;
    if (next != NULL) {
      // we can safely assume that this memory is still allocated for the
      // neighbor since deletion must occur on the second node of the pair,
      // NEVER on the first node
      pthread_mutex_lock(&next->lock);

      if (index == 0) {
        // free the node now that we've found its index
        curr->next = next->next;
        pthread_mutex_unlock(&next->lock);
        pthread_mutex_destroy(&next->lock);
        free(next);

        // relinquish the lock on the current node
        pthread_mutex_unlock(&curr->lock);
        return;
      }

      // relinquish the lock of the current node
      pthread_mutex_unlock(&curr->lock);

      // continue traversing the list
      curr = next;
      index--;
    }
  }

  // the current node's neighbor is null
  pthread_mutex_unlock(&curr->lock);
}

void removeHead(HOHLinkedList *list) { removeNode(list, 0); }

void removeTail(HOHLinkedList *list) {
  // exit early if the list is null
  if (list == NULL || list->head == NULL) {
    return;
  }

  // traverse the list starting at the dummy node
  HOHNode *curr = list->head;

  // acquire the lock on the dummy node first
  pthread_mutex_lock(&curr->lock);

  while (curr->next != NULL) {
    // acquire the lock on the current node's neighbor
    HOHNode *next = curr->next;
    if (next != NULL) {
      // we can safely assume that this memory is still allocated for the
      // neighbor since deletion must occur on the second node of the pair,
      // NEVER on the first node
      pthread_mutex_lock(&next->lock);

      if (next->next == NULL) {
        // free the current node's neighbor as it's the last in the list
        curr->next = NULL;
        pthread_mutex_unlock(&next->lock);
        pthread_mutex_destroy(&next->lock);
        free(next);
        pthread_mutex_unlock(&curr->lock);
        return;
      }

      pthread_mutex_unlock(&curr->lock);

      // continue traversing the list
      curr = next;
    }
  }

  // relinquish the lock on the dummy node; this code should only execute if
  // no nodes exist after the dummy node
  pthread_mutex_unlock(&curr->lock);
}
