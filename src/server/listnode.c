/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#include "listnode.h"

//Declared here to keep it "private"...
int ListNode_rec_size(ListNode * node, int n);

void ListNode_add(ListNode *node, int sockfd) {
    if (node->next == NULL) {
        node->next = malloc(sizeof(ListNode));
        node->next->newsockfd = sockfd;
        node->next->next = NULL;
    } else {
        ListNode_add(node->next, sockfd);
    }
}

int ListNode_get(ListNode *node, int i) {
    if (i < 0) {
        ZF_LOGF_STR("Tried to access a node that was out of range");
        return -1;
    } else if (i == 0) return node->newsockfd;
    else return ListNode_get(node->next, i - 1);
}

void ListNode_remove(ListNode *node, int i) {
    if (i < 0)
        ZF_LOGF_STR("Tried to access a node that was out of range");
    else if (i == 0) {
        if (node->next == NULL)
            free(node);
        else {
            // this implementation removes the next node after
            // moving it to the left, to facilitate removal of the
            // first node also. Hence the check if next is null.

            ListNode * next = node->next;
            node->newsockfd = next->newsockfd;
            node->next = next->next;

            free(next);
        }
    } else ListNode_remove(node->next, i-1);
}

void ListNode_clear(ListNode *node) {
    ListNode * next = node->next;
    free(node);

    if(next != NULL)
        ListNode_clear(next);
}

int ListNode_size(ListNode *node){
    return ListNode_rec_size(node, 0);
}

int ListNode_rec_size(ListNode * node, int n){
    if(node == NULL) return n;
    else return ListNode_rec_size(node->next, n+1);
}
