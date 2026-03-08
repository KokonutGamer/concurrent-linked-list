#include "hoh_linkedlist.h"

#include <stdio.h>

/**
 * Internal function used for assertions.
 */
static void assertEquals(int actual, int expected, const char *name) {
  if (actual == expected) {
    printf("%s passed\n", name);
  } else {
    printf("%s failed (%d expected but contained %d)\n", name, expected,
           actual);
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
}

int main() { emptyListTests(); }