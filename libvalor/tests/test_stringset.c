#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <valor/hashset.h>

int main() {
    int errors = 0;
    stringset_t *ss = create_stringset(10);
    if (ss == NULL) {
        printf("Failed to create string set\n");
        errors++;
    }

    // Test adding a value
    stringset_add(ss, "hello");
    if (!stringset_check(ss, "hello")) {
        printf("Failed to find 'hello'\n");
        errors++;
    }

    // Test that non-added value is not found
    if (stringset_check(ss, "world")) {
        printf("Found non-existent 'world'\n");
        errors++;
    }

    // Cleanup
    destroy_stringset(ss);

    if (errors == 0) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Total failures: %d\n", errors);
        return 1;
    }
}
