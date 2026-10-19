#include "../test_suites.h"
#include "hps/data_structure/define.h"
#include "hps/data_structure/define.h"
#include <assert.h>
#include <stdint.h>

/**
 * @file tests/data_structure/test_data_structure.c
 * @brief Typed container initialization, borrowing and argument evaluation checks.
 */

typedef struct hps_test_item_s {
    uint16_t key;
    uint8_t flags;
} hps_test_item_t;
typedef uint16_t *hps_test_word_pointer_t;

HPS_VECTOR_DEFINE(uint16_t, hps_test_vector)
HPS_MUTABLE_SPAN_DEFINE(uint16_t, hps_test_mutable_span)
HPS_CSPAN_DEFINE(uint16_t, hps_test_const_span)
HPS_ROBUFFER_DEFINE(uint16_t, hps_test_robuffer)
HPS_SVECTOR_DEFINE(hps_test_item_t, hps_test_svector, (2U + 1U))
HPS_SRING_DEFINE(hps_test_item_t, hps_test_sring, (2U * 2U))
HPS_RING_DEFINE(uint16_t, hps_test_ring)
HPS_STACK_DEFINE(uint16_t, hps_test_stack)
HPS_SLIST_DEFINE(uint16_t, hps_test_slist)
HPS_DLIST_DEFINE(uint16_t, hps_test_dlist)
HPS_BST_DEFINE(uint16_t, hps_test_bst)
HPS_CSPAN_DEFINE(hps_test_word_pointer_t, hps_test_pointer_span)
HPS_BST_DEFINE(hps_test_item_t, hps_test_item_tree)

static uint16_t mutable_words[] = {11U, 22U, 33U, 44U};
static const uint16_t readonly_words[] = {55U, 66U, 77U, 88U};
static const hps_test_vector_t static_vector =
    HPS_VECTOR_STATIC_INIT(mutable_words, 2U, 4U);
static const hps_test_mutable_span_t static_mutable_span =
    HPS_MUTABLE_SPAN_STATIC_INIT(mutable_words, 2U);
static const hps_test_const_span_t static_const_span =
    HPS_CSPAN_STATIC_INIT(readonly_words, 2U);
static const hps_test_robuffer_t static_robuffer =
    HPS_ROBUFFER_STATIC_INIT(readonly_words, 2U, 4U);
static const hps_test_svector_t static_svector = HPS_SVECTOR_STATIC_INIT;
static const hps_test_sring_t static_sring = HPS_SRING_STATIC_INIT;
static const hps_test_ring_t static_ring =
    HPS_RING_STATIC_INIT(mutable_words, 3U, 1U, 2U, 4U);
static const hps_test_stack_t static_stack =
    HPS_STACK_STATIC_INIT(mutable_words, 2U, 4U);
static hps_test_slist_node_t static_slist_node =
    HPS_SLIST_NODE_STATIC_INIT(11U, NULL);
static const hps_test_slist_t static_slist =
    HPS_SLIST_STATIC_INIT(&static_slist_node, 1U);
static hps_test_dlist_node_t static_dlist_node =
    HPS_DLIST_NODE_STATIC_INIT(22U, NULL, NULL);
static const hps_test_dlist_t static_dlist =
    HPS_DLIST_STATIC_INIT(&static_dlist_node, &static_dlist_node, 1U);
static hps_test_bst_node_t static_bst_node =
    HPS_BST_NODE_STATIC_INIT(33U, NULL, NULL);
static const hps_test_bst_t static_bst =
    HPS_BST_STATIC_INIT(&static_bst_node, 1U);

static uint16_t *counted_data(uint8_t *calls)
{
    *calls += 1U;
    return mutable_words;
}

static const uint16_t *counted_const_data(uint8_t *calls)
{
    *calls += 1U;
    return readonly_words;
}

static size_t counted_size(uint8_t *calls, size_t value)
{
    *calls += 1U;
    return value;
}

static uint16_t counted_value(uint8_t *calls, uint16_t value)
{
    *calls += 1U;
    return value;
}

static hps_test_slist_node_t *counted_slist_node(uint8_t *calls,
    hps_test_slist_node_t *node)
{
    *calls += 1U;
    return node;
}

static hps_test_dlist_node_t *counted_dlist_node(uint8_t *calls,
    hps_test_dlist_node_t *node)
{
    *calls += 1U;
    return node;
}

static hps_test_bst_node_t *counted_bst_node(uint8_t *calls,
    hps_test_bst_node_t *node)
{
    *calls += 1U;
    return node;
}

static void check_calls(const uint8_t *calls, size_t length)
{
    size_t index;
    for (index = 0U; index < length; index += 1U) {
        assert(calls[index] == 1U);
    }
}

static void test_borrowed_arrays(void)
{
    uint8_t vector_calls[3] = {0U};
    uint8_t mutable_calls[2] = {0U};
    uint8_t const_calls[2] = {0U};
    uint8_t robuffer_calls[3] = {0U};
    uint8_t stack_calls[3] = {0U};
    hps_test_vector_t vector = HPS_VECTOR_MAKE(hps_test_vector,
        counted_data(&vector_calls[0]), counted_size(&vector_calls[1], 2U),
        counted_size(&vector_calls[2], 4U));
    hps_test_mutable_span_t mutable_span =
        HPS_MUTABLE_SPAN_MAKE(hps_test_mutable_span,
            counted_data(&mutable_calls[0]), counted_size(&mutable_calls[1], 2U));
    hps_test_const_span_t const_span = HPS_CSPAN_MAKE(hps_test_const_span,
        counted_const_data(&const_calls[0]), counted_size(&const_calls[1], 2U));
    hps_test_robuffer_t robuffer = HPS_ROBUFFER_MAKE(hps_test_robuffer,
        counted_const_data(&robuffer_calls[0]), counted_size(&robuffer_calls[1], 2U),
        counted_size(&robuffer_calls[2], 4U));
    hps_test_stack_t stack = HPS_STACK_MAKE(hps_test_stack,
        counted_data(&stack_calls[0]), counted_size(&stack_calls[1], 2U),
        counted_size(&stack_calls[2], 4U));
    hps_test_const_span_t borrowed_readonly =
        hps_test_const_span_make(mutable_words, 2U);

    check_calls(vector_calls, 3U);
    check_calls(mutable_calls, 2U);
    check_calls(const_calls, 2U);
    check_calls(robuffer_calls, 3U);
    check_calls(stack_calls, 3U);
    assert(vector.data == mutable_words && vector.length == 2U && vector.capacity == 4U);
    assert(mutable_span.data == mutable_words && mutable_span.length == 2U);
    assert(const_span.data == readonly_words && const_span.length == 2U);
    assert(robuffer.data == readonly_words && robuffer.length == 2U && robuffer.capacity == 4U);
    assert(stack.data == mutable_words && stack.length == 2U && stack.capacity == 4U);

    /* Borrowed metadata copies keep the same live storage; payload is not copied. */
    mutable_span.data[1] = 99U;
    assert(vector.data[1] == 99U && stack.data[1] == 99U);
    assert(borrowed_readonly.data[1] == 99U);
    assert(readonly_words[1] == 66U);
    mutable_span.data[1] = 22U;

    assert(static_vector.data == mutable_words && static_vector.length == 2U);
    assert(static_vector.capacity == 4U);
    assert(static_mutable_span.data == mutable_words && static_mutable_span.length == 2U);
    assert(static_const_span.data == readonly_words && static_const_span.length == 2U);
    assert(static_robuffer.data == readonly_words && static_robuffer.length == 2U);
    assert(static_robuffer.capacity == 4U);
    assert(static_stack.data == mutable_words && static_stack.length == 2U);
    assert(static_stack.capacity == 4U);

    vector = hps_test_vector_make(NULL, 0U, 0U);
    mutable_span = hps_test_mutable_span_make(NULL, 0U);
    const_span = hps_test_const_span_make(NULL, 0U);
    robuffer = hps_test_robuffer_make(NULL, 0U, 0U);
    stack = hps_test_stack_make(NULL, 0U, 0U);
    assert(vector.data == NULL && vector.length == 0U && vector.capacity == 0U);
    assert(mutable_span.data == NULL && mutable_span.length == 0U);
    assert(const_span.data == NULL && const_span.length == 0U);
    assert(robuffer.data == NULL && robuffer.length == 0U && robuffer.capacity == 0U);
    assert(stack.data == NULL && stack.length == 0U && stack.capacity == 0U);
}

static void test_fixed_storage_and_rings(void)
{
    size_t index;
    uint8_t ring_calls[5] = {0U};
    hps_test_svector_t vector = HPS_SVECTOR_MAKE(hps_test_svector);
    hps_test_sring_t fixed_ring = HPS_SRING_MAKE(hps_test_sring);
    hps_test_ring_t ring = HPS_RING_MAKE(hps_test_ring,
        counted_data(&ring_calls[0]), counted_size(&ring_calls[1], 3U),
        counted_size(&ring_calls[2], 1U), counted_size(&ring_calls[3], 2U),
        counted_size(&ring_calls[4], 4U));
    hps_test_svector_t vector_copy;
    hps_test_sring_t ring_copy;

    check_calls(ring_calls, 5U);
    assert(sizeof(vector.data) / sizeof(vector.data[0]) == 3U);
    assert(sizeof(fixed_ring.data) / sizeof(fixed_ring.data[0]) == 4U);
    assert(vector.length == 0U && static_svector.length == 0U);
    for (index = 0U; index < 3U; index += 1U) {
        assert(vector.data[index].key == 0U && vector.data[index].flags == 0U);
        assert(static_svector.data[index].key == 0U && static_svector.data[index].flags == 0U);
    }
    assert(fixed_ring.head == 0U && fixed_ring.tail == 0U && fixed_ring.length == 0U);
    assert(static_sring.head == 0U && static_sring.tail == 0U && static_sring.length == 0U);
    for (index = 0U; index < 4U; index += 1U) {
        assert(fixed_ring.data[index].key == 0U && fixed_ring.data[index].flags == 0U);
        assert(static_sring.data[index].key == 0U && static_sring.data[index].flags == 0U);
    }

    /* Fixed-storage copies own distinct payload arrays, including struct elements. */
    vector.data[0].key = 123U;
    vector.data[0].flags = 1U;
    vector.length = 1U;
    vector_copy = vector;
    vector_copy.data[0].key = 456U;
    assert(vector.data[0].key == 123U && vector_copy.data[0].key == 456U);
    assert(vector_copy.data[0].flags == 1U && vector_copy.length == 1U);
    fixed_ring.data[3].key = 789U;
    ring_copy = fixed_ring;
    ring_copy.data[3].key = 321U;
    assert(fixed_ring.data[3].key == 789U && ring_copy.data[3].key == 321U);
    vector = hps_test_svector_make();
    fixed_ring = hps_test_sring_make();
    assert(vector.data[0].key == 0U && vector.length == 0U);
    assert(fixed_ring.data[3].key == 0U && fixed_ring.length == 0U);

    assert(ring.data == mutable_words && ring.head == 3U && ring.tail == 1U);
    assert(ring.length == 2U && ring.capacity == 4U);
    assert(static_ring.data == mutable_words && static_ring.head == 3U);
    assert(static_ring.tail == 1U && static_ring.length == 2U && static_ring.capacity == 4U);
    ring = hps_test_ring_make(NULL, 0U, 0U, 0U, 0U);
    assert(ring.data == NULL && ring.head == 0U && ring.tail == 0U);
    assert(ring.length == 0U && ring.capacity == 0U);
}

static void test_linked_storage(void)
{
    uint8_t slist_node_calls[2] = {0U};
    uint8_t slist_calls[2] = {0U};
    uint8_t dlist_node_calls[3] = {0U};
    uint8_t dlist_calls[3] = {0U};
    uint8_t bst_node_calls[3] = {0U};
    uint8_t bst_calls[2] = {0U};
    hps_test_slist_node_t slist_tail = hps_test_slist_node_make(22U, NULL);
    hps_test_slist_node_t slist_head = HPS_SLIST_NODE_MAKE(hps_test_slist,
        counted_value(&slist_node_calls[0], 11U),
        counted_slist_node(&slist_node_calls[1], &slist_tail));
    hps_test_slist_t slist = HPS_SLIST_MAKE(hps_test_slist,
        counted_slist_node(&slist_calls[0], &slist_head), counted_size(&slist_calls[1], 2U));
    hps_test_dlist_node_t dlist_head = hps_test_dlist_node_make(33U, NULL, NULL);
    hps_test_dlist_node_t dlist_tail = HPS_DLIST_NODE_MAKE(hps_test_dlist,
        counted_value(&dlist_node_calls[0], 44U),
        counted_dlist_node(&dlist_node_calls[1], &dlist_head),
        counted_dlist_node(&dlist_node_calls[2], NULL));
    hps_test_dlist_t dlist = HPS_DLIST_MAKE(hps_test_dlist,
        counted_dlist_node(&dlist_calls[0], &dlist_head),
        counted_dlist_node(&dlist_calls[1], &dlist_tail), counted_size(&dlist_calls[2], 2U));
    hps_test_bst_node_t left = hps_test_bst_node_make(11U, NULL, NULL);
    hps_test_bst_node_t right = hps_test_bst_node_make(33U, NULL, NULL);
    hps_test_bst_node_t root = HPS_BST_NODE_MAKE(hps_test_bst,
        counted_value(&bst_node_calls[0], 22U),
        counted_bst_node(&bst_node_calls[1], &left),
        counted_bst_node(&bst_node_calls[2], &right));
    hps_test_bst_t bst = HPS_BST_MAKE(hps_test_bst,
        counted_bst_node(&bst_calls[0], &root), counted_size(&bst_calls[1], 3U));

    check_calls(slist_node_calls, 2U);
    check_calls(slist_calls, 2U);
    check_calls(dlist_node_calls, 3U);
    check_calls(dlist_calls, 3U);
    check_calls(bst_node_calls, 3U);
    check_calls(bst_calls, 2U);
    assert(slist.length == 2U && slist.head == &slist_head);
    assert(slist.head->value == 11U && slist.head->next == &slist_tail);
    assert(slist.head->next->value == 22U && slist_tail.next == NULL);
    dlist_head.next = &dlist_tail;
    assert(dlist.length == 2U && dlist.head == &dlist_head && dlist.tail == &dlist_tail);
    assert(dlist.head->prev == NULL && dlist.head->next == dlist.tail);
    assert(dlist.tail->prev == dlist.head && dlist.tail->next == NULL);
    assert(dlist.head->value == 33U && dlist.tail->value == 44U);
    assert(bst.root == &root && bst.length == 3U && bst.root->value == 22U);
    assert(bst.root->left == &left && bst.root->right == &right);
    assert(left.value == 11U && right.value == 33U);

    assert(static_slist.head == &static_slist_node && static_slist.length == 1U);
    assert(static_slist_node.value == 11U && static_slist_node.next == NULL);
    assert(static_dlist.head == &static_dlist_node && static_dlist.tail == &static_dlist_node);
    assert(static_dlist.length == 1U && static_dlist_node.value == 22U);
    assert(static_dlist_node.prev == NULL && static_dlist_node.next == NULL);
    assert(static_bst.root == &static_bst_node && static_bst.length == 1U);
    assert(static_bst_node.value == 33U && static_bst_node.left == NULL && static_bst_node.right == NULL);

    slist = hps_test_slist_make(NULL, 0U);
    dlist = hps_test_dlist_make(NULL, NULL, 0U);
    bst = hps_test_bst_make(NULL, 0U);
    assert(slist.head == NULL && slist.length == 0U);
    assert(dlist.head == NULL && dlist.tail == NULL && dlist.length == 0U);
    assert(bst.root == NULL && bst.length == 0U);
}

static void test_object_elements(void)
{
    hps_test_word_pointer_t pointers[2] = {&mutable_words[0], &mutable_words[1]};
    hps_test_pointer_span_t span = HPS_CSPAN_MAKE(hps_test_pointer_span, pointers, 2U);
    hps_test_item_t item = {123U, 1U};
    hps_test_item_tree_node_t left = hps_test_item_tree_node_make(item, NULL, NULL);
    hps_test_item_tree_node_t root =
        HPS_BST_NODE_MAKE(hps_test_item_tree, item, &left, NULL);
    hps_test_item_tree_t tree = HPS_BST_MAKE(hps_test_item_tree, &root, 2U);

    /* A readonly span of pointers protects its pointer elements, not the pointees. */
    assert(span.data[0] == &mutable_words[0] && span.length == 2U);
    *span.data[0] = 99U;
    assert(mutable_words[0] == 99U);
    *span.data[0] = 11U;
    span = hps_test_pointer_span_make(pointers, 1U);
    assert(span.data == pointers && span.length == 1U);

    /* Node payloads accept an object typedef and copy its value. */
    item.key = 456U;
    assert(tree.root->value.key == 123U && tree.root->value.flags == 1U);
    assert(tree.root->left == &left && tree.root->left->value.key == 123U);
    assert(tree.root->right == NULL && tree.length == 2U);
    assert(item.key == 456U);
    tree = hps_test_item_tree_make(&left, 1U);
    assert(tree.root == &left && tree.length == 1U);
    assert(tree.root->value.key == 123U && tree.root->value.flags == 1U);
}

int32_t test_data_structure(void)
{
    test_borrowed_arrays();
    test_fixed_storage_and_rings();
    test_linked_storage();
    test_object_elements();
    return INT32_C(0);
}
