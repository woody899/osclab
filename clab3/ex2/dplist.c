/**
 * \author Jeroen Van Aken, Bert Lagaisse, Ludo Bruynseels
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "dplist.h"


/*
 * The real definition of struct list / struct node
 */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element;
};

struct dplist {
    dplist_node_t *head;
    // more fields will be added later
};

dplist_t *dpl_create() {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
    return list;
}


// current has an element which points to the head of a list
// which is a node, after freeing the current node
// we need to have the pointer of head, in current, change positions
// so that the while loop can go through the whole list

void dpl_free(dplist_t **list) {
    // Honestly for this part
    // at some point I had to resort to the internet for help
    // This took me way more hours than I care to admit
    //TODO: add your code here
    dplist_node_t *currentNode = (*list)->head;
    while (currentNode != NULL) {
        dplist_node_t *after = currentNode->next;
        free(currentNode);
        currentNode = after;
    }

    free(*list);
    *list = NULL;
}
/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/


dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));

    list_node->element = element;
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index) {

    //TODO: add your code here
    int i;
    dplist_node_t *temp = list->head;

    if (index <= 0) {
        list->head = temp->next;
        free(temp);
        return list;
    } else if (index > dpl_size(list)) {

        while(temp != NULL){
            dplist_node_t* after = list->head->next;
            if(after != NULL){
                temp = after;
            } else{
                free(temp);
            }
        }
        return list;

    } else if(list->head == NULL){
        return NULL;
    }
    else {

        for (i = 0; i < index - 2; i++) {
            temp = temp->next;
            dplist_node_t *temp2 = temp->next;
            temp = temp2->next;
            free(temp2);

        }
        return list;
    }
}

int dpl_size(dplist_t *list) {

    //TODO: add your code here
    int i = 0;
    dplist_node_t* current = list -> head;

    while(current != NULL){
//        dplist_node_t* after = list->head->next;
//       current->head=after; //this is wrong, it turns the original list null
        current = current->next;
        i++;

    }
    return i;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    int count = 0 ;
    dplist_node_t *dummy = list->head;

    //TODO: add your code here
    if(dpl_size(list) <= 0){
        // we return first line node
        return list->head;
    } else if(index > dpl_size(list)){
        while(dummy != NULL){
            dplist_node_t* after = dummy->next;
            if(after != NULL){
                dummy = after;
            } else{
                return dummy;
            }
        }
    } else if(dpl_size(list) == 0){
        return NULL;
    } else if(list == NULL){
        return NULL;
    }
    else{

        for (count = 0; count < index - 2; count++) {
            dummy = dummy->next;
            if(count == index - 2){
                return dummy;
            }

        }
    }

    return dummy;
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {

    //TODO: add your code here
    return '\e';
}

int dpl_get_index_of_element(dplist_t *list, element_t element) {

    //TODO: add your code here
    return -1;
}



