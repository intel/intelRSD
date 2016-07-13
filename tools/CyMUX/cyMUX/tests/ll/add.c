#include "../../ll.c"
#include "tap/basic.h"
#include "mock_pthread/mock_pthread.h"


/// TAP control variables
void* block_allocated = NULL;
size_t block_size = 0;
bool mock_calloc_failure = false;


static void *sys_calloc (size_t __nmemb, size_t __size)
{
    block_size = __nmemb * __size;
    if (mock_calloc_failure) {
        block_allocated = NULL;
    }
    else {
        block_allocated = calloc(__nmemb, __size);
    }
    return block_allocated;
}


int
main(void)
{
    Lnode_t*        pNode = NULL;
    Lnode_t         nodes[3];
    Lnode_t         zeroed;
    Message_t       msg;
    Message_t       msg_test;

    bzero(nodes, sizeof(nodes));
    bzero(&zeroed, sizeof(zeroed));

    memset(&msg_test, 1, sizeof(msg_test));
    msg = msg_test;

    plan(41);

    diag("Test for expected error with NULL node pointer passed");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    mock_calloc_failure = false;
    ok(-1 == ll_add(NULL, &msg), "Error returned with NULL Node Pointer");
    ok(EINVAL == errno, "errno set to EINVAL with NULL Node Pointer");
    ok(NULL == block_allocated, "calloc not called with NULL Node Pointer");

    diag("Test for expected error with NULL message pointer passed");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    mock_calloc_failure = false;
    ok(-1 == ll_add(&pNode, NULL), "Error returned with NULL Message Pointer");
    ok(EINVAL == errno, "errno set to EINVAL with NULL Message Pointer");
    ok(NULL == block_allocated, "calloc not called with NULL Message Pointer");

    diag("Test for expected error with NULL pointers passed");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    mock_calloc_failure = false;
    ok(-1 == ll_add(NULL, NULL), "Error returned with NULL pointers passed");
    ok(EINVAL == errno, "errno set to EINVAL with NULL pointers passed");
    ok(NULL == block_allocated, "calloc not called with NULL pointers passed");

    diag("node pointer == NULL, test on simulated memory allocation error");
    errno = 0;
    block_size = 0;
    pNode = block_allocated = NULL;
    mock_calloc_failure = true;
    ok(-1 == ll_add(&pNode, &msg), "Error returned on expected memory allocation error with node pointer == NULL");
    ok(ENOMEM == errno, "errno set to ENOMEM on expected memory allocation error with node pointer == NULL");
    ok(NULL == pNode, "Node pointer not changed on expected memory allocation error with node pointer == NULL");

    diag("node pointer == nodes[0], test on simulated memory allocation error");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    pNode = &nodes[0];
    mock_calloc_failure = true;
    ok(-1 == ll_add(&pNode, &msg), "Error returned on expected memory allocation error with node pointer == NULL");
    ok(ENOMEM == errno, "errno set to ENOMEM on expected memory allocation error with node pointer == NULL");
    ok(&nodes[0] == pNode, "Node pointer not changed on expected memory allocation error with node pointer == NULL");
    ok(0 == memcmp(&nodes[0], &zeroed, sizeof(zeroed)), "Node hasn't changed");

    diag("node pointer == NULL, test on successful memory allocation");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    pNode = NULL;
    mock_calloc_failure = false;
    ok(0 == ll_add(&pNode, &msg), "Success with node pointer == NULL");
    ok(0 == errno, "errno unchanged on success with node pointer == NULL");
    ok(NULL != pNode, "node pointer changed on success when node pointer was NULL");
    ok(block_allocated == pNode, "node pointer is assigned system allocated memory");
    ok(sizeof(Lnode_t) == block_size, "Allocated sizeof(Lnode_t)");
    ok(0 == memcmp(&pNode->request, &msg_test, sizeof(msg_test)), "Message was assigned to node");
    ok(NULL == pNode->prev && NULL == pNode->next, "Pointers initialized");
    free(pNode);

    diag("node pointer == nodes[0], test on successful memory allocation");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    pNode = &nodes[0];
    mock_calloc_failure = false;
    bzero(&nodes[0], sizeof(nodes[0]));
    ok(0 == ll_add(&pNode, &msg), "Success with node pointer == nodes[0]");
    ok(0 == errno, "errno unchanged on success with node pointer == nodes[0]");
    ok(NULL != pNode, "node pointer changed on success when node pointer was nodes[0]");
    ok(&nodes[0] == pNode, "node pointer hasn't changed on success");
    ok(NULL != block_allocated && nodes[0].next == block_allocated, "New node added");
    ok(sizeof(Lnode_t) == block_size, "Allocated sizeof(Lnode_t)");
    ok(0 == memcmp(&pNode->next->request, &msg_test, sizeof(msg_test)), "Message was assigned to node");
    ok(&nodes[0] == pNode->next->prev, "Pointers initialized");
    free(pNode->next);

    diag("chain is properly followed in addition");
    errno = 0;
    block_size = 0;
    block_allocated = NULL;
    pNode = &nodes[0];
    mock_calloc_failure = false;
    bzero(&nodes[0], sizeof(nodes[0]));
    nodes[0].next = &nodes[1];
    nodes[1].next = &nodes[2];
    nodes[1].prev = &nodes[0];
    nodes[2].prev = &nodes[1];
    memset(&msg_test, 0xaa, sizeof(msg_test));
    memset(&msg, 0x55, sizeof(msg));
    nodes[0].request = msg_test;
    nodes[1].request = msg_test;
    nodes[2].request = msg_test;
    ok(0 == ll_add(&pNode, &msg), "Success with node pointer == nodes[0]");
    ok(0 == errno, "errno unchanged on success with node pointer == nodes[0]");
    ok(NULL != pNode, "node pointer changed on success when node pointer was nodes[0]");
    ok(&nodes[0] == pNode, "node pointer hasn't changed on success");
    ok(NULL != block_allocated && nodes[2].next == block_allocated, "New node added");
    ok(sizeof(Lnode_t) == block_size, "Allocated sizeof(Lnode_t)");
    memset(&msg_test, 0x55, sizeof(msg_test));
    ok(0 == memcmp(&nodes[2].next->request, &msg_test, sizeof(msg_test)), "Message was assigned to node");
    ok(&nodes[2] == nodes[2].next->prev, "Pointers initialized");
    ok(nodes[0].next == &nodes[1] && nodes[1].next == &nodes[2] &&
       nodes[2].prev == &nodes[1] && nodes[1].prev == &nodes[0] &&
       NULL == nodes[0].prev, "Node chain pointers untouched");
    memset(&msg_test, 0xaa, sizeof(msg_test));
    ok(0 == memcmp(&nodes[0].request, &msg_test, sizeof(msg_test)) &&
       0 == memcmp(&nodes[1].request, &msg_test, sizeof(msg_test)) &&
       0 == memcmp(&nodes[2].request, &msg_test, sizeof(msg_test)),
       "Node chain messages untouched");
    free(nodes[2].next);
}


/// Stub routines
bool valid_request_msg(Message_t const __attribute((__unused__))* const m)
{
    return true;
}


static void sys_free (void __attribute((__unused__))* __ptr)
{
}
