

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"
#include "/home/dony/osc/plab1/config.h"




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
    if (list == NULL) {
        return NULL;
    }
    if (list->head == NULL) {
        return list;
    }
    if(index <= 0){
        dplist_node_t *temp = list->head;
        if(free_element){
            list->element_free(&(list->head->element));
        }
        list->head = temp->next;
        free(temp);

    } else{
        int i = 0;
        dplist_node_t* temp = list->head;
        while(i < index-1 && temp->next!= NULL ){
            temp = temp->next;
            i++;
        }
        if(temp == NULL){
            dplist_node_t* temp2 = temp->prev;
            dplist_node_t* temp3 = temp2->prev;
            if(free_element){
                list->element_free(&(temp2->element));
            }
            free(temp2);
            temp3->next = temp;
        }else{
            dplist_node_t* temp2 = temp->next;
            dplist_node_t* temp3 = temp2->next;
            if(free_element){
                list->element_free(&(temp2->element));
            }
            free(temp2);
            temp->next = temp3;

        }




    }
    return list;
}



int dpl_size(dplist_t *list) {

    //TODO: add your code here
    if(list == NULL){return -1;}

    int i = 1;
    dplist_node_t* current = list -> head;
    if(list->head == NULL){
        return 0;
    }
    while(current != NULL){
    if(current->next == NULL){return i;}
    else{
        current = current->next;
        i++;
    }
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
    return NULL;

}

int dpl_get_index_of_element(dplist_t *list, void *element) {

    //TODO: add your code here
    if(list == NULL){return -1;}

    dplist_node_t* current = list->head;
    int i;
    int count = 0;
    while(current != NULL){
        i = list->element_compare(current,element);

        if(i == 0){
            return count;
        }
        current = current->next;
       count++;
   }
    return -1;

}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {

    //TODO: add your code here
    if (list == NULL) {return NULL;}
    if (list->head == NULL || index <= 0) {
        return list->head;
    }
    dplist_node_t *current = list->head;
    int count = 0;
    while (count < index && current->next != NULL) {
        current = current->next;
        count++;
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



