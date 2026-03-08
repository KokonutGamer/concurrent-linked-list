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
 *
 */
static int runTest(void (*testFunction)(HOHLinkedList *), HOHLinkedList *list) {
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
 * Internal function used for segfault assertions.
 */
static void assertNoSegFault(void (*testFunction)(HOHLinkedList *),
                             HOHLinkedList *list, const char *name) {
  if (runTest(testFunction, list)) {
    printf("[PASSED] %s\n", name);
  } else {
    printf("[FAILED] %s, segmentation fault detected\n", name);
  }
}

/**
 * ==================== SINGLE-THREADED EDGE CASES ====================
 */
void emptyListTests() {
  // arrange
  HOHLinkedList *list;

  // act
  initList(&list);

  // assert
  assertEquals(size(list), 0, "Empty list size is zero");
  assertEquals(front(list), 0, "Empty list front is zero");
  assertEquals(back(list), 0, "Empty list back is zero");
  assertEquals(get(list, 0), 0, "Empty list at index 0 is zero");

  assertNoSegFault(removeHead, list,
                   "Empty list head removal results in no segmentation fault");
  assertNoSegFault(removeTail, list,
                   "Empty list tail removal results in no segmentation fault");
  assertNoSegFault(clearList, list,
                   "Clearing an empty list results in no segmentation fault");
  assertNoSegFault(printList, list,
                   "Printing an empty list results in no segmentation fault");
  //   removeHead(list);
  //   removeTail(list);
  //   clearList(list);
  //   printList(list);
}

int main() { emptyListTests(); }