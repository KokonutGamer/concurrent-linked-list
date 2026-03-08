#include "hoh_linkedlist.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Internal function used for comparison assertions.
 */
static void assertEquals(int actual, int expected, const char *name) {
  if (actual == expected) {
    printf("[PASSED] %s\n", name);
  } else {
    printf("[FAILED] %s, %d expected but contained %d\n", name, expected,
           actual);
  }
}

/**
 * Internal function used for negative comparison assertions.
 */
static void assertNotEquals(int actual, int notExpected, const char *name) {
  if (actual != notExpected) {
    printf("[PASSED] %s\n", name);
  } else {
    printf("[FAILED] %s, %d was not expected\n", name, notExpected);
  }
}

/**
 * Internal function used for testing functions with a supplied list returning
 * void.
 */
static int runWithList(void (*testFunction)(HOHLinkedList *),
                       HOHLinkedList *list) {
  // fork the process at this execution point
  pid_t pid = fork();

  if (pid == -1) {
    perror("Fork failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // within child process
    testFunction(list);
    exit(EXIT_SUCCESS);
  } else {
    // within parent process; check on the status of the child
    int status;
    waitpid(pid, &status, 0);

    if (WIFSIGNALED(status)) {
      // detect segmentation faults
      if (WTERMSIG(status) == SIGSEGV) {
        return 0;
      }
    }
    return 1; // no segmentation fault occurred
  }
}

/**
 * Internal function used for testing functions with a supplied list and value
 * returning void.
 */
static int runWithListValue(void (*testFunction)(HOHLinkedList *, int),
                            HOHLinkedList *list, int value) {
  // fork the process at this execution point
  pid_t pid = fork();

  if (pid == -1) {
    perror("Fork failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // within child process
    testFunction(list, value);
    exit(EXIT_SUCCESS);
  } else {
    // within parent process; check on the status of the child
    int status;
    waitpid(pid, &status, 0);

    if (WIFSIGNALED(status)) {
      // detect segmentation faults
      if (WTERMSIG(status) == SIGSEGV) {
        return 0;
      }
    }
    return 1; // no segmentation fault occurred
  }
}

/**
 * Internal function used for segfault assertions on functions with a supplied
 * list returning void.
 */
static void assertNoSegFaultList(void (*testFunction)(HOHLinkedList *),
                                 HOHLinkedList *list, const char *name) {
  if (runWithList(testFunction, list)) {
    printf("[PASSED] %s\n", name);
  } else {
    printf("[FAILED] %s, segmentation fault detected\n", name);
  }
}

/**
 * Internal function used for segfault assertions on functions with a supplied
 * list and value returning void.
 */
static void
assertNoSegFaultListValue(void (*testFunction)(HOHLinkedList *, int value),
                          HOHLinkedList *list, int value, const char *name) {
  if (runWithListValue(testFunction, list, value)) {
    printf("[PASSED] %s\n", name);
  } else {
    printf("[FAILED] %s, segmentation fault detected\n", name);
  }
}

/**
 * ==================== SINGLE-THREADED TESTS ====================
 */
void emptyListTests() {
  // arrange
  HOHLinkedList *list;

  // act
  initList(&list);

  // assert
  assertEquals(size(list), 0, "Empty list size is zero");
  assertEquals(front(list), 0, "Empty list head is zero");
  assertEquals(back(list), 0, "Empty list tail is zero");
  assertEquals(get(list, 0), 0, "Empty list at index 0 is zero");

  assertNoSegFaultList(
      removeHead, list,
      "Empty list head removal results in no segmentation fault");
  assertNoSegFaultList(
      removeTail, list,
      "Empty list tail removal results in no segmentation fault");
  assertNoSegFaultList(
      clearList, list,
      "Clearing an empty list results in no segmentation fault");
  assertNoSegFaultList(
      printList, list,
      "Printing an empty list results in no segmentation fault");

  destroyList(list);
}

void singleElementListTests() {
  // arrange
  HOHLinkedList *list;
  initList(&list);
  int value = 24;

  // act
  append(list, value);

  // assert
  assertEquals(front(list), value, "Single-element list head is a constant");
  assertEquals(back(list), value, "Single-element list tail is a constant");
  assertNoSegFaultList(
      printList, list,
      "Printing a single-element list results in no segmentation fault");

  // act
  removeTail(list);

  // assert
  assertEquals(
      front(list), 0,
      "Single-element list tail removal results in head equal to zero");
  assertEquals(
      back(list), 0,
      "Single-element list tail removal results in tail equal to zero");
  assertNoSegFaultList(
      printList, list,
      "Printing an empty list results in no segmentation fault");

  destroyList(list);
}

void outOfBoundsListTests() {
  // arrange
  HOHLinkedList *list;
  initList(&list);
  int values[] = {23, 59, 47};

  // act
  for (int i = 0; i < 3; i++) {
    append(list, values[i]);
  }

  // assert
  assertEquals(get(list, 999), 0,
               "Attempting to retrieve an out-of-bounds index returns 0");
  assertNoSegFaultListValue(removeNode, list, 999,
                            "Attempting to remove an out-of-bounds index "
                            "results in no segmentation fault");

  // act
  insertNode(list, 999, 5);

  // assert
  assertNotEquals(back(list), 5,
                  "Attempting to insert an out-of-bounds index results in no "
                  "new insertion");

  destroyList(list);
}

/**
 * ==================== MULTI-THREADED TESTS ====================
 */

void *appendRoutine(void *args) {
  HOHLinkedList *list = (HOHLinkedList *)args;

  for (int i = 0; i < 1000; i++) {
    append(list, i);
  }

  return NULL;
}

void massAppendTests() {
  // arrange
  int numberOfThreads = 10;
  pthread_t threads[numberOfThreads];

  HOHLinkedList *list;
  initList(&list);

  // act
  for (int i = 0; i < numberOfThreads; i++) {
    pthread_create(&threads[i], NULL, appendRoutine, (void *)list);
  }
  for (int i = 0; i < numberOfThreads; i++) {
    pthread_join(threads[i], NULL);
  }

  // assert
  assertEquals(size(list), numberOfThreads * 1000,
               "Concurrent-append list size is a constant");
}

void *producerRoutine(void *args) {
  HOHLinkedList *list = (HOHLinkedList *)args;

  for (int i = 0; i < 50; i++) {
    append(list, i);
  }

  return NULL;
}

void *consumerRoutine(void *args) {
  HOHLinkedList *list = (HOHLinkedList *)args;

  for (int i = 0; i < 50; i++) {
    removeHead(list);
  }

  return NULL;
}

void producerConsumerTests() {
  // arrange
  int numberOfProducers = 5;
  int numberOfConsumers = 5;
  pthread_t producers[numberOfProducers];
  pthread_t consumers[numberOfConsumers];

  HOHLinkedList *list;
  initList(&list);

  // act
  for (int i = 0; i < numberOfProducers && i < numberOfConsumers; i++) {
    if (i < numberOfProducers) {
      pthread_create(&producers[i], NULL, producerRoutine, (void *)list);
    }
    if (i < numberOfConsumers) {
      pthread_create(&consumers[i], NULL, consumerRoutine, (void *)list);
    }
  }

  for (int i = 0; i < numberOfProducers && i < numberOfConsumers; i++) {
    if (i < numberOfProducers) {
      pthread_join(producers[i], NULL);
    }
    if (i < numberOfConsumers) {
      pthread_join(consumers[i], NULL);
    }
  }

  // assert
  printList(list);

  destroyList(list);
}

int main() {
  emptyListTests();
  singleElementListTests();
  outOfBoundsListTests();
  massAppendTests();
  producerConsumerTests();
}
