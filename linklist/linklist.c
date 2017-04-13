/* vim: ts=4 sw=4 expandtab softtabstop=4
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct Node {
    uint32_t index;
    struct Node *next;
    struct Node *previous;
};

struct Node *head_node = NULL, *last_node = NULL;

void add_first(uint32_t index)
{
    struct Node * newNode;

    newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->index = index;
    newNode->previous = NULL;
    
    if (head_node == NULL) {
        newNode->next = NULL;
        head_node = newNode;
        last_node = newNode;
    } else {
        newNode->next = head_node;
        head_node = newNode;
    }
}

void add_last(uint32_t index)
{
    struct Node * newNode;

    newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->index = index;
    newNode->next = NULL;
    
    if (last_node == NULL) {
        newNode->previous = NULL;
        head_node = last_node = newNode;
    } else {
        newNode->previous = last_node;
        last_node = newNode;
    }
}

void insert_after(uint32_t index, uint32_t location)
{
    struct Node * newNode;

    newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->next = NULL;

    if (head_node == NULL) {
        newNode->previous = newNode->next = NULL;
        head_node = last_node = newNode;
    } else {
        struct Node *temp1 = head_node, *temp2;
        while (temp1->index != location) {
            if (temp1->next == NULL) {
                printf("Given node is not found in the list!!!\n\r");
                goto EndFunction;
            } else {
                temp1 = temp1->next;
            }
        }
        temp2 = temp1->next;
        temp1->next = newNode;
        newNode->previous = temp1;
        newNode->next = temp2;
        temp2->previous = newNode;
    }
EndFunction: ;
}

void insert_before(uint32_t index, uint32_t location)
{
    struct Node * newNode;

    newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->next = NULL;

    if (head_node == NULL) {
        newNode->previous = newNode->next = NULL;
        head_node = last_node = newNode;
    } else {
        struct Node *temp1 = head_node, *temp2;
        while (temp1->index != location) {
            if (temp1->next == NULL) {
                printf("Given node is not found in the list!!!\n\r");
                goto EndFunction;
            } else {
                temp1 = temp1->next;
            }
        }
        temp2 = temp1->previous;
        temp1->previous= newNode;
        newNode->next = temp1;
        newNode->previous = temp2;
        temp2->next= newNode;
    }
EndFunction: ;
}

void remove_first()
{
    if (head_node == NULL) {
        printf("List is Empty!!!\n\r");
    } else {
        struct Node *temp = head_node;
        if (temp->previous == temp->next) {
            /*only one data, previous and next are null*/
            head_node = last_node = NULL;
            free(temp);
        } else {
            head_node = temp->next;
            head_node->previous = NULL;
            free(temp);
        }
    }
}

void remove_last()
{
    if (last_node == NULL) {
        printf("List is Empty!!!\n\r");
    } else {
        struct Node *temp = last_node;
        if (temp->previous == temp->next) {
            /*only one data, previous and next are null*/
            head_node = last_node = NULL;
            free(temp);
        } else {
            last_node = temp->previous;
            last_node->next = NULL;
            free(temp);
        }
    }
}

void remove_node(uint32_t index)
{
    if (head_node == NULL) {
        printf("List is Empty!!!\n\r");
    } else {
        struct Node *temp = head_node;

        while (temp->index != index) {
            if (temp->next == NULL) {
                printf("Give index is not found in the list!!!\n\r");
                goto EndFunction;
            } else {
                temp = temp->next;
            }
        }
        if (temp == head_node) {
            head_node = last_node = NULL;
            free(temp);
        } else {
            temp->previous->next = temp->next;
            free(temp);
        }
    }
EndFunction: ;
}

struct Node *find_node(struct Node *head, uint32_t index)
{
    struct Node *ptr;

    ptr = head_node;
    while (ptr != NULL) {
        if (ptr->index == index)
            return ptr;
        ptr = ptr->next;
    }
    return ptr;
}

