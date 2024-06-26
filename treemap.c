#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap *createTreeMap(int (*lt)(void *key1, void *key2)) {
    TreeMap *map = (TreeMap *)malloc(sizeof(TreeMap));
    if (map == NULL) {
        return NULL;
    }

    map->root = NULL;
    map->current = NULL;
    map->lower_than = lt;

    return map;
}


void insertTreeMap(TreeMap* tree, void* key, void* value) {
    if (searchTreeMap(tree, key) != NULL)
        return; 
    TreeNode* new_node = createTreeNode(key, value); 

    if (new_node == NULL)
        return; 

    if (tree->root == NULL) {
        tree->root = new_node;
        tree->current = new_node;
        return;
    }

    TreeNode* current = tree->root;
    TreeNode* parent = NULL;

    while (current != NULL) {
        parent = current;
        if (tree->lower_than(new_node->pair->key, current->pair->key))
            current = current->left;
        else
            current = current->right;
    }

    new_node->parent = parent;
    if (tree->lower_than(new_node->pair->key, parent->pair->key))
        parent->left = new_node;
    else
        parent->right = new_node;

    tree->current = new_node;
}

TreeNode * minimum(TreeNode * x) {
    if (x == NULL)
        return NULL;

    while (x->left != NULL)
        x = x->left;

    return x;
}


void removeNode(TreeMap * tree, TreeNode* node) {
    if (node == NULL)
        return;

    if (node->left == NULL && node->right == NULL) {
        if (node->parent != NULL) {
            if (node->parent->left == node)
                node->parent->left = NULL;
            else
                node->parent->right = NULL;
        }
        free(node);
        return;
    }

    if (node->left == NULL || node->right == NULL) {
        TreeNode* child = (node->left != NULL) ? node->left : node->right;
        if (node->parent != NULL) {
            if (node->parent->left == node)
                node->parent->left = child;
            else
                node->parent->right = child;
        }
        child->parent = node->parent;
        free(node);
        return;
    }
    TreeNode* successor = minimum(node->right);
    node->pair->key = successor->pair->key;
    node->pair->value = successor->pair->value;
    removeNode(tree, successor);
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}




Pair* searchTreeMap(TreeMap* tree, void* key) {
    TreeNode* current = tree->root;

    while (current != NULL) {
        if (tree->lower_than(key, current->pair->key))
            current = current->left;
        else if (tree->lower_than(current->pair->key, key))
            current = current->right;
        else {
            tree->current = current; 
            return current->pair;
        }
    }

    return NULL; 
}


Pair* upperBound(TreeMap* tree, void* key) {
    if (tree == NULL || tree->root == NULL)
        return NULL;

    TreeNode* current = tree->root;
    TreeNode* ub_node = NULL;
    while (current != NULL) {
        int comparison = tree->lower_than(current->pair->key, key);
        if (comparison == 0) {
            return current->pair; 
        } else if (comparison < 0) {
            current = current->right;
        } else { 
            ub_node = current;
            current = current->left;
        }
    }

    if (ub_node != NULL)
        return ub_node->pair;
    else
        return NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->root == NULL)
        return NULL;

    TreeNode* minimumNode = minimum(tree->root);
    tree->current = minimumNode;

    return minimumNode->pair; 
}

Pair * nextTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->current == NULL)
        return NULL;

    if (tree->current->right != NULL) {
        tree->current = minimum(tree->current->right);
        return tree->current->pair;
    }

   
    TreeNode* parent = tree->current->parent;
    while (parent != NULL && tree->current == parent->right) {
        tree->current = parent;
        parent = parent->parent;
    }
    tree->current = parent;

    if (parent != NULL)
        return parent->pair;
    else
        return NULL;
}
