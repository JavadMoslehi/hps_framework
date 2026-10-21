#include "../test_suites.h"
#include "hps/data_structure/data_structure.h"
#include "hps/data_structure/data_structure.h"
#include "hps/memory/hps_refcount.h"
#include "hps/memory/hps_refcount.h"
#include <assert.h>
#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"

/**
 * @file tests/data_structure/test_collection_operations.c
 * @brief Bounded typed mutation, traversal and reference-count failure checks.
 */

typedef uint16_t *hps_test_collection_pointer;

HPS_VECTOR_DEFINE(uint32_t, hps_test_collection_vector)
HPS_VECTOR_OPERATIONS_DEFINE(uint32_t, hps_test_collection_vector)
HPS_VECTOR_DEFINE(hps_test_collection_pointer, hps_test_collection_pointers)
HPS_VECTOR_OPERATIONS_DEFINE(hps_test_collection_pointer,
                             hps_test_collection_pointers)
HPS_DLIST_DEFINE(uint32_t, hps_test_collection_list)
HPS_DLIST_OPERATIONS_DEFINE(uint32_t, hps_test_collection_list)

static void test_checked_reference_count(void)
{
    hps_refcount counter;
    uint8_t final_release = UINT8_C(77);

    hps_refcount_init(NULL);
    hps_refcount_init(&counter);
    assert(hps_refcount_get(&counter) == UINT32_C(1));
    assert(hps_refcount_get(NULL) == UINT32_C(0));
    assert(hps_refcount_retain(NULL) == HPS_REFCOUNT_INVALID_ARGUMENT);
    assert(hps_refcount_release(NULL, &final_release) ==
           HPS_REFCOUNT_INVALID_ARGUMENT);
    assert(final_release == UINT8_C(77));
    assert(hps_refcount_release(&counter, NULL) ==
           HPS_REFCOUNT_INVALID_ARGUMENT);
    assert(hps_refcount_get(&counter) == UINT32_C(1));
    assert(hps_refcount_retain(&counter) == HPS_REFCOUNT_OK);
    assert(hps_refcount_get(&counter) == UINT32_C(2));
    assert(hps_refcount_release(&counter, &final_release) == HPS_REFCOUNT_OK);
    assert(final_release == UINT8_C(0));
    assert(hps_refcount_get(&counter) == UINT32_C(1));
    assert(hps_refcount_release(&counter, &final_release) == HPS_REFCOUNT_OK);
    assert(final_release == UINT8_C(1));
    assert(hps_refcount_get(&counter) == UINT32_C(0));
    final_release = UINT8_C(77);
    assert(hps_refcount_release(&counter, &final_release) ==
           HPS_REFCOUNT_INACTIVE);
    assert(hps_refcount_retain(&counter) == HPS_REFCOUNT_INACTIVE);
    assert(hps_refcount_get(&counter) == UINT32_C(0));
    assert(final_release == UINT8_C(77));

    /* Inject the representational boundary without billions of retain calls. */
    counter.count = UINT32_MAX;
    assert(hps_refcount_retain(&counter) == HPS_REFCOUNT_OVERFLOW);
    assert(hps_refcount_get(&counter) == UINT32_MAX);
    assert(hps_refcount_release(&counter, &final_release) == HPS_REFCOUNT_OK);
    assert(hps_refcount_get(&counter) == UINT32_MAX - UINT32_C(1));
    assert(final_release == UINT8_C(0));
}

static void test_vector_capacity_and_failure_preservation(void)
{
    uint32_t storage[4] = {UINT32_C(91), UINT32_C(92),
                           UINT32_C(93), UINT32_C(94)};
    hps_test_collection_vector_t vector =
        hps_test_collection_vector_make(&storage[1], 0U, 2U);
    uint32_t value = UINT32_C(77);

    assert(hps_test_collection_vector_check(NULL) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_vector_push(NULL, UINT32_C(11)) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_vector_push(&vector, UINT32_C(11)) ==
           HPS_COLLECTION_OK);
    assert(hps_test_collection_vector_push(&vector, UINT32_C(22)) ==
           HPS_COLLECTION_OK);
    assert(vector.length == 2U);
    assert(storage[0] == UINT32_C(91));
    assert(storage[1] == UINT32_C(11));
    assert(storage[2] == UINT32_C(22));
    assert(storage[3] == UINT32_C(94));
    assert(hps_test_collection_vector_push(&vector, UINT32_C(33)) ==
           HPS_COLLECTION_CAPACITY);
    assert(vector.length == 2U);
    assert(storage[2] == UINT32_C(22));
    assert(storage[3] == UINT32_C(94));
    assert(hps_test_collection_vector_get(&vector, 1U, &value) ==
           HPS_COLLECTION_OK);
    assert(value == UINT32_C(22));
    assert(hps_test_collection_vector_get(&vector, 2U, &value) ==
           HPS_COLLECTION_OUT_OF_RANGE);
    assert(hps_test_collection_vector_get(&vector, SIZE_MAX, &value) ==
           HPS_COLLECTION_OUT_OF_RANGE);
    assert(value == UINT32_C(22));
    assert(hps_test_collection_vector_get(NULL, 0U, &value) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_vector_get(&vector, 0U, NULL) ==
           HPS_COLLECTION_INVALID_ARGUMENT);

    vector.length = 3U;
    assert(hps_test_collection_vector_push(&vector, UINT32_C(33)) ==
           HPS_COLLECTION_INVALID_STATE);
    assert(hps_test_collection_vector_get(&vector, 0U, &value) ==
           HPS_COLLECTION_INVALID_STATE);
    assert(vector.length == 3U);
    assert(value == UINT32_C(22));
    vector = hps_test_collection_vector_make(NULL, 0U, 1U);
    assert(hps_test_collection_vector_check(&vector) ==
           HPS_COLLECTION_INVALID_STATE);
    vector = hps_test_collection_vector_make(storage, 0U,
                                             SIZE_MAX / sizeof(uint32_t) + 1U);
    assert(hps_test_collection_vector_push(&vector, UINT32_C(33)) ==
           HPS_COLLECTION_SIZE_OVERFLOW);
    assert(vector.length == 0U);
    assert(storage[0] == UINT32_C(91));
    vector = hps_test_collection_vector_make(NULL, 0U, 0U);
    assert(hps_test_collection_vector_check(&vector) == HPS_COLLECTION_OK);
    assert(hps_test_collection_vector_push(&vector, UINT32_C(33)) ==
           HPS_COLLECTION_CAPACITY);
    assert(hps_test_collection_vector_get(&vector, 0U, &value) ==
           HPS_COLLECTION_OUT_OF_RANGE);
    assert(value == UINT32_C(22));
}

static void test_vector_iteration(void)
{
    uint32_t storage[2] = {UINT32_C(11), UINT32_C(22)};
    hps_test_collection_vector_t vector =
        hps_test_collection_vector_make(storage, 2U, 2U);
    hps_test_collection_vector_iterator_t iterator = {NULL, 0U, 0U};
    uint32_t value = UINT32_C(77);
    uint8_t found = UINT8_C(77);

    assert(hps_test_collection_vector_iterator_begin(NULL, &iterator) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(iterator.data == NULL && iterator.length == 0U);
    assert(hps_test_collection_vector_iterator_begin(&vector, NULL) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_vector_iterator_begin(&vector, &iterator) ==
           HPS_COLLECTION_OK);
    assert(hps_test_collection_vector_iterator_next(&iterator, NULL, &found) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_vector_iterator_next(&iterator, &value, NULL) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_vector_iterator_next(NULL, &value, &found) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(iterator.index == 0U);
    assert(value == UINT32_C(77) && found == UINT8_C(77));
    assert(hps_test_collection_vector_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(value == UINT32_C(11) && found == UINT8_C(1));
    assert(hps_test_collection_vector_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(value == UINT32_C(22) && found == UINT8_C(1));
    assert(hps_test_collection_vector_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(value == UINT32_C(22) && found == UINT8_C(0));
    assert(hps_test_collection_vector_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(iterator.index == 2U && found == UINT8_C(0));

    iterator.index = 3U;
    found = UINT8_C(77);
    assert(hps_test_collection_vector_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_INVALID_STATE);
    assert(iterator.index == 3U);
    assert(value == UINT32_C(22) && found == UINT8_C(77));
    iterator.index = 0U;
    iterator.length = SIZE_MAX;
    assert(hps_test_collection_vector_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_SIZE_OVERFLOW);
    assert(iterator.index == 0U && iterator.length == SIZE_MAX);
    iterator.data = NULL;
    iterator.length = 1U;
    assert(hps_test_collection_vector_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_INVALID_STATE);
    vector = hps_test_collection_vector_make(NULL, 0U, 0U);
    assert(hps_test_collection_vector_iterator_begin(&vector, &iterator) ==
           HPS_COLLECTION_OK);
    assert(hps_test_collection_vector_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(value == UINT32_C(22) && found == UINT8_C(0));
}

static void test_nullable_pointer_elements(void)
{
    uint16_t target = UINT16_C(42);
    hps_test_collection_pointer storage[1] = {&target};
    hps_test_collection_pointers_t vector =
        hps_test_collection_pointers_make(storage, 0U, 1U);
    hps_test_collection_pointers_iterator_t iterator;
    hps_test_collection_pointer value = &target;
    uint8_t found = UINT8_C(77);

    assert(hps_test_collection_pointers_push(&vector, NULL) == HPS_COLLECTION_OK);
    assert(hps_test_collection_pointers_get(&vector, 0U, &value) ==
           HPS_COLLECTION_OK);
    assert(value == NULL);
    assert(hps_test_collection_pointers_iterator_begin(&vector, &iterator) ==
           HPS_COLLECTION_OK);
    value = &target;
    assert(hps_test_collection_pointers_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(value == NULL && found == UINT8_C(1));
    value = &target;
    assert(hps_test_collection_pointers_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(value == &target && found == UINT8_C(0));
}

static void test_detached_list_append(void)
{
    hps_test_collection_list_node_t first =
        hps_test_collection_list_node_make(UINT32_C(11), NULL, NULL);
    hps_test_collection_list_node_t second =
        hps_test_collection_list_node_make(UINT32_C(22), NULL, NULL);
    hps_test_collection_list_node_t third =
        hps_test_collection_list_node_make(UINT32_C(33), NULL, NULL);
    hps_test_collection_list_t list =
        hps_test_collection_list_make(NULL, NULL, 0U);

    assert(hps_test_collection_list_check(NULL) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_list_push_back(NULL, &first) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_list_push_back(&list, NULL) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_list_push_back(&list, &first) ==
           HPS_COLLECTION_OK);
    assert(list.length == 1U && list.head == &first && list.tail == &first);
    /* A detached-looking singleton already belongs to the list. */
    assert(hps_test_collection_list_push_back(&list, &first) ==
           HPS_COLLECTION_INVALID_STATE);
    assert(first.prev == NULL && first.next == NULL && list.length == 1U);
    assert(hps_test_collection_list_push_back(&list, &second) ==
           HPS_COLLECTION_OK);
    assert(list.length == 2U && list.head == &first && list.tail == &second);
    assert(first.next == &second && second.prev == &first);
    third.prev = &first;
    assert(hps_test_collection_list_push_back(&list, &third) ==
           HPS_COLLECTION_INVALID_STATE);
    assert(list.length == 2U && list.tail == &second);
    assert(second.next == NULL && third.prev == &first && third.next == NULL);
    third.prev = NULL;
    first.prev = &third;
    assert(hps_test_collection_list_push_back(&list, &third) ==
           HPS_COLLECTION_INVALID_STATE);
    assert(list.length == 2U && third.prev == NULL && third.next == NULL);
    first.prev = NULL;
    /* Inject a count boundary; no traversal of the inconsistent count occurs. */
    list.length = SIZE_MAX;
    assert(hps_test_collection_list_push_back(&list, &third) ==
           HPS_COLLECTION_SIZE_OVERFLOW);
    assert(list.length == SIZE_MAX && list.tail == &second);
    assert(second.next == NULL && third.prev == NULL && third.next == NULL);
    list.length = 2U;
    assert(hps_test_collection_list_push_back(&list, &third) ==
           HPS_COLLECTION_OK);
    assert(list.length == 3U && list.tail == &third);
    assert(second.next == &third && third.prev == &second && third.next == NULL);
    assert(first.value == UINT32_C(11) && second.value == UINT32_C(22));
    assert(third.value == UINT32_C(33));

    list = hps_test_collection_list_make(&first, NULL, 0U);
    assert(hps_test_collection_list_check(&list) == HPS_COLLECTION_INVALID_STATE);
    list = hps_test_collection_list_make(NULL, NULL, 1U);
    assert(hps_test_collection_list_check(&list) == HPS_COLLECTION_INVALID_STATE);
    list = hps_test_collection_list_make(&first, &third, 1U);
    assert(hps_test_collection_list_check(&list) == HPS_COLLECTION_INVALID_STATE);
}

static void test_list_iteration_and_traversal_budget(void)
{
    hps_test_collection_list_node_t first =
        hps_test_collection_list_node_make(UINT32_C(11), NULL, NULL);
    hps_test_collection_list_node_t second =
        hps_test_collection_list_node_make(UINT32_C(22), &first, NULL);
    hps_test_collection_list_t list =
        hps_test_collection_list_make(&first, &second, 2U);
    hps_test_collection_list_iterator_t iterator = {NULL, NULL, 0U};
    uint32_t value = UINT32_C(77);
    uint8_t found = UINT8_C(77);

    first.next = &second;
    assert(hps_test_collection_list_iterator_begin(NULL, 2U, &iterator) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_list_iterator_begin(&list, 2U, NULL) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_list_iterator_begin(&list, 1U, &iterator) ==
           HPS_COLLECTION_CAPACITY);
    assert(iterator.node == NULL && iterator.remaining == 0U);
    assert(hps_test_collection_list_iterator_begin(&list, 2U, &iterator) ==
           HPS_COLLECTION_OK);
    assert(hps_test_collection_list_iterator_next(&iterator, NULL, &found) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_list_iterator_next(&iterator, &value, NULL) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(hps_test_collection_list_iterator_next(NULL, &value, &found) ==
           HPS_COLLECTION_INVALID_ARGUMENT);
    assert(iterator.node == &first && iterator.remaining == 2U);
    assert(value == UINT32_C(77) && found == UINT8_C(77));
    assert(hps_test_collection_list_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(value == UINT32_C(11) && found == UINT8_C(1));
    assert(iterator.node == &second && iterator.remaining == 1U);
    assert(hps_test_collection_list_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(value == UINT32_C(22) && found == UINT8_C(1));
    assert(hps_test_collection_list_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(value == UINT32_C(22) && found == UINT8_C(0));
    assert(hps_test_collection_list_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(iterator.node == NULL && iterator.remaining == 0U);

    /* Synthetic live-node corruption: the cursor cannot loop past its bound. */
    iterator.node = &first;
    iterator.previous = NULL;
    iterator.remaining = 1U;
    found = UINT8_C(77);
    assert(hps_test_collection_list_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_INVALID_STATE);
    assert(iterator.node == &first && iterator.remaining == 1U);
    assert(value == UINT32_C(22) && found == UINT8_C(77));
    iterator.node = &second;
    iterator.previous = NULL;
    assert(hps_test_collection_list_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_INVALID_STATE);
    assert(iterator.node == &second && iterator.remaining == 1U);
    iterator.node = NULL;
    assert(hps_test_collection_list_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_INVALID_STATE);
    iterator.node = &second;
    iterator.remaining = 0U;
    assert(hps_test_collection_list_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_INVALID_STATE);
    assert(value == UINT32_C(22) && found == UINT8_C(77));

    list = hps_test_collection_list_make(NULL, NULL, 0U);
    assert(hps_test_collection_list_iterator_begin(&list, 0U, &iterator) ==
           HPS_COLLECTION_OK);
    assert(hps_test_collection_list_iterator_next(&iterator, &value, &found) ==
           HPS_COLLECTION_OK);
    assert(value == UINT32_C(22) && found == UINT8_C(0));
}

int32_t test_collection_operations(void)
{
    test_checked_reference_count();
    test_vector_capacity_and_failure_preservation();
    test_vector_iteration();
    test_nullable_pointer_elements();
    test_detached_list_append();
    test_list_iteration_and_traversal_budget();
    return INT32_C(0);
}
