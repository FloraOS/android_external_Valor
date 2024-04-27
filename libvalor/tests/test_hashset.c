#include <stdio.h>
#include <valor/hashset.h>

int main() {
    int errors = 0;
    hashset_t *hs = create_hashset(10);
    if (hs == NULL) errors++;

    // Test adding a value
    hashset_add(hs, 123);
    if (!hashset_check(hs, 123)) errors++;

    // Test that non-added value is not found
    if (hashset_check(hs, 456)) errors++;

    destroy_hashset(hs);

    if (errors == 0) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed!\n");
        return 1;
    }
}
