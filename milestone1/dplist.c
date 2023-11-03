

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"




/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {
    //TODO: add your code here

    dplist_t* current = *list;
    if(current == NULL){
        return;
    }

    dplist_node_t* currNode = current->head;
    while(currNode != NULL){
        dplist_node_t* after = currNode->next; //this is temporary
        if(free_element){
            current->element_free(&(currNode->element));
        }

        free(currNode);
        currNode = after;
    }
    free(current);
    *list = NULL;


}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {

    //TODO: add your code here
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) {return NULL;}

    list_node = malloc(sizeof(dplist_node_t));

    if(insert_copy){
        list_node->element = list->element_copy(element);
    }

    else{
        list_node->element = element;
    }
    if (list->head == NULL) {
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) {
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) {
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else {
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;



}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {

    //TODO: add your code here
    if(list == NULL){return NULL;}

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
    if(list == NULL){return -1;}

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

void *dpl_get_element_at_index(dplist_t *list, int index) {

    if(list == NULL){return NULL;}
    //TODO: add your code here
    dplist_node_t* current = list->head;
    int i = 0;
    while(i <= index){
        if(index == i){
            return current;
        }
        current = current->next;
        i++;

    }

}

int dpl_get_index_of_element(dplist_t *list, void *element) {

    //TODO: add your code here
    if(list == NULL){return -1;}

    dplist_node_t* current = list->head;
    int i = 0;
    while(current != NULL){
        if(current->element == element){
            return i;
        }
        current = current->next;
        i++;
    }
    return i;

}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    int count = 0 ;
    dplist_node_t *current = list->head;
    //TODO: add your code here
    if(dpl_size(list) <= 0){
        // we return first line node
        return list->head;
    } else if(index > dpl_size(list)){
        while(current != NULL){
            dplist_node_t* after = current->next;
            if(after != NULL){
                current = after;
            } else{
                return current;
            }
        }
    } else if(dpl_size(list) == 0){
        return NULL;
    } else if(list == NULL){
        return NULL;
    }
    else{

        for (count = 0; count < index - 2; count++) {
            current = current->next;
            if(count == index - 2){
                return current;
            }

        }
    }

    return current;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {

    //TODO: add your code here
    dplist_node_t* current = list->head;
    while(current != NULL){
        if(current == reference){
            return current->element;
        }
        current = current->next;

    }
    return NULL;
}



