/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#ifndef ASSIGNMENT_LISTNODE_H
#define ASSIGNMENT_LISTNODE_H

#include <stdlib.h>
#include "../../deps/zf_log/zf_log.h"

/**
 * This type represents a node in a linked list
 * As a result, all funtions here are implemented to work on it
 * recursively
 *
 * It was attempted to make all methods tail recursive, to avoid
 * stack overflows
 */
typedef struct ListNode {
    int newsockfd;
    struct ListNode * next;
} ListNode;

/**
 * This type represents a tuple <int x ListNode>
 */
typedef struct Pair {
    int index;
    ListNode * node;
} Pair;

typedef int (*Predicate)(ListNode *);

/**
 * Adds an element at the end of the ListNode List given
 * @param node  the list to add it to
 * @param sockfd the int value (newsockfd)
 */
void ListNode_add(ListNode * node, int sockfd);

/**
 * Gets the ith element from the linked list given
 * @param node The linked list to get the ith element from
 * @param i the element number
 * @return the int value at the ith element
 */
int ListNode_get(ListNode * node, int i);

/**
 * Gets the index to the first element in the linked list that has the sockfd given
 * @param node the list to check
 * @param sockfd the sockfd to check
 * @return the index to the first element, negative if not found
 */
int ListNode_getNodeIndexFromSockfd(ListNode *node, int sockfd, int n);

/**
 * Removes the ith element from the linked list moving all elements to the left
 * @param node the list to remove it from
 * @param i the element to remove
 */
void ListNode_remove(ListNode * node, int i);

/**
 * Clears the linked list by freeing all memory
 * @param node The list to clear
 */
void ListNode_clear(ListNode * node);

/**
 * Returns the size of the Linked list
 * @return the size of the linked list given
 */
int ListNode_size(ListNode * node);

#endif //ASSIGNMENT_LISTNODE_H
