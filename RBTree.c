#include "RBTree.h"
#include "Structs.h"
#include <stdlib.h>


#define INITIAL_SIZE 0
#define LEFT 1
#define FOR_EACH_TRUE 1


RBTree *newRBTree(CompareFunc compFunc, FreeFunc freeFunc)
{
    if(compFunc == NULL || freeFunc == NULL)
    {
        exit(EXIT_FAILURE);
    }
    RBTree *t = (RBTree*)malloc(sizeof(RBTree));
    t->compFunc = compFunc;
    t->freeFunc = freeFunc;
    t->root = NULL;
    t->size = INITIAL_SIZE;
    return t;
}



Node *newNode(void *data, Node *parent)
{
    Node *new;
    new = (Node*)malloc(sizeof(Node));
    new->parent = parent;
    new->left = NULL; //null ptr?
    new->right = NULL;
    new->color = RED;
    new->data = data;
    return new;
}



void rotate(Node *p, int orientation, RBTree *tree)
{
    Node *g = p->parent;
    if (orientation == LEFT)
    {
        Node *n = p->right;
        p->right = n->left;
        if (p->right != NULL)
        {
            p->right->parent = p;
        }
        n->parent = g;
        n->left = p;
        p->parent = n;
        if (g != NULL)
        {
            g->right = (g->right == p) ? n : g->right;
            g->left = (g->left == p) ? n : g->left;
        }
        else
        {
            tree->root = p->parent;
        }
    }
    else
    {
        Node *n = p->left;
        p->left = n->right;
        if (p->left != NULL)
        {
            (p->left)->parent = p;
        }
        n->parent = g;
        n->right = p;
        p->parent = n;
        if (g != NULL)
        {
            g->right = (g->right == p) ? n : g->right;
            g->left = (g->left == p) ? n : g->left;
        }
        else
        {
            tree->root = p->parent;
        }
    }
}



void fixInsert(Node *n, RBTree *tree)
{
    CompareFunc compFunc = tree->compFunc;
    Node *p = n->parent;
    if (p == NULL)
    {
        n->color = BLACK;
        return;
    }
    if (p->color == BLACK)
    {
        return;
    }
    Node *g = p->parent;
    Node *u = NULL;
    u = (compFunc(p->data , g->data) < 0) ? g->right : g->left;
    if (u != NULL && u->color == RED)
    {
        u->color = BLACK;
        p->color = BLACK;
        g->color = RED;
        fixInsert(g, tree);
        return;
    }
    else
    {
        if ((compFunc(n->data, p->data) < 0 && compFunc(g->data, p->data) < 0) ||
        (compFunc(n->data, p->data) > 0 && compFunc(g->data, p->data) > 0))
        {
            rotate(p, compFunc(n->data, p->data) > 0, tree);
            fixInsert(p, tree);
            return;
        }
        else
        {
            rotate(g, compFunc(n->data, p->data) > 0, tree);
            p->color = BLACK;
            g->color = RED;
        }
    }
}



int insertToRBTree(RBTree *tree, void *data)
{
    if (tree->root == NULL)
    {
        Node *new = newNode(data, NULL);
        fixInsert(new, tree);
        tree->root = new;
        tree->size++;
        return 2;
    }
    else
    {
        Node *cur = tree->root;
        Node *parent = NULL;
        int isLeft = 0;
        while(cur != NULL)
        {
            if (tree->compFunc(cur->data, data) == 0)
            {
                return 0;
            }
            else if (tree->compFunc(cur->data, data) < 0)
            {
                parent = (cur->right == NULL) ? cur : parent;
                cur = cur->right;
                isLeft = 0;
            }
            else if (tree->compFunc(cur->data, data) > 0)
            {
                parent = (cur->left == NULL) ? cur : parent;
                cur = cur->left;
                isLeft = 1;
            }
        }
        Node *new = newNode(data, parent);
        parent->right = isLeft ? parent->right : new;
        parent->left = isLeft ? new : parent->left;
        fixInsert(new, tree);
        tree->size++;
        return 2;
    }
}


int RBTreeContains(const RBTree *tree, const void *data)
{
    CompareFunc f = tree->compFunc;
    Node *cur = tree->root;
    if (cur == NULL)
    {
        return 0;
    }
    while (cur != NULL)
    {
        if (f(cur->data, data) > 0)
        {
            cur = cur->left;
        }
        else if (f(data, cur->data) > 0)
        {
            cur = cur->right;
        }
        else
        {
            return 1;
        }
    }
    return 0;
}


Node *findSuccesor(Node *m)
{
    Node *suc = m->right;
    while (suc->left != NULL)
    {
        suc = suc->left;
    }
    return suc;
}


void doubleBlack(Node *c, Node *p, RBTree *tree)
{
    if (tree->root == c)
    {
        return;
    }
    Node *s = (p->left == c) ? p->right : p->left; //can be null?
    Node *sCloser = (p->left == c) ? s->left : s->right;
    Node *sFurther = (p->left == c) ? s->right : s->left;
    unsigned int farColor = (sFurther == NULL) ? BLACK : sFurther->color;
    unsigned int closeColor = (sCloser == NULL) ? BLACK : sCloser->color;
    if (s->color == BLACK && farColor == BLACK && closeColor == BLACK)
    {
        if (p->color == RED)
        {
            p->color = BLACK;
            s->color = RED;
            return;
        }
        if (p->color == BLACK)
        {
            s->color = RED;
            doubleBlack(p, p->parent, tree);
            return;
        }
    }
    if (s->color == RED)
    {
        s->color = BLACK;
        p->color = RED;
        rotate(p, p->left == c, tree);
        doubleBlack(c, p, tree);
        return;
    }
    if (s->color == BLACK && farColor == BLACK && closeColor == RED)
    {
        sCloser->color = BLACK;
        s->color = RED;
        rotate(s, p->left == s, tree);
        doubleBlack(c, p, tree);
        return;
    }
    if (s->color == BLACK && sFurther->color == RED)
    {
        unsigned int temp = p->color;
        p->color = s->color;
        s->color = temp;
        rotate(p, p->left == c, tree);
        sFurther->color = BLACK;
        return;
    }
}



void connectSon(Node *p, Node *c, Node *m, RBTree *tree)
{
    if (c != NULL)
    {
        c->color = BLACK;
        c->parent = p;
    }
    if (p == NULL)
    {
        tree->root = c;
        return;
    }
    p->left = (p->left == m) ? c : p->left;
    p->right = (p->right == m) ? c : p->right;
}


void deleteNode(Node *m, RBTree *tree)
{
    FreeFunc ff = tree->freeFunc;
    Node *p = m->parent;
    Node *c = (m->left == NULL) ? m->right : m->left;
    unsigned int cColor = (c != NULL) ? c->color : BLACK;
    if (m->color == RED || (cColor == RED))
    {
        connectSon(p, c, m, tree);
        ff(m->data);
        tree->size--;
        return;
    }
    else
    {
        connectSon(p, c, m, tree);
        ff(m->data);
        tree->size--;
        doubleBlack(c, p, tree);
        return;
    }
}



int deleteFromRBTree(RBTree *tree, void *data)
{
    if (RBTreeContains(tree, data) == 0)
    {
        return 0;
    }
    CompareFunc cf = tree->compFunc;
    Node *cur = tree->root;
    while (cf(cur->data, data) != 0)
    {
        if (cf(cur->data, data) > 0)
        {
            cur = cur->left;
        }
        else if (cf(data, cur->data) > 0)
        {
            cur = cur->right;
        }
    }
    Node *m = NULL;
    if (cur->left != NULL && cur->right != NULL)
    {
        void *temp = cur->data;
        Node *suc = findSuccesor(cur);
        cur->data = suc->data;;
        suc->data = temp;
        m = suc;
    }
    else
    {
        m = cur;
    }
    deleteNode(m, tree);
    free(m);
    return 1;
}


int nodesInOrder(Node *root, forEachFunc func, void *args)
{
    if (root == NULL)
    {
        return FOR_EACH_TRUE;
    }
    int left = nodesInOrder(root->left, func, args);
    int self = func(root->data, args);
    int right = nodesInOrder(root->right, func, args);
    return left && self && right;
}


int forEachRBTree(const RBTree *tree, forEachFunc func, void *args)
{
    if (tree == NULL || func == NULL)
    {
        return 0;
    }
    return nodesInOrder(tree->root, func, args);
}



void freeLeftRight(Node **root, FreeFunc func)
{
    if (*root == NULL)
    {
        return;
    }
    freeLeftRight(&(*root)->left, func);
    freeLeftRight(&(*root)->right, func);
    func((*root)->data);
    free(*root);
}


void freeRBTree(RBTree **tree)
{
    freeLeftRight(&((*tree)->root), (*tree)->freeFunc);
    free(*tree);
}

int cmpr(const void* a, const void* b)
{
    int* i = (int*)a;
    int* j = (int*)b;

    if (*i > *j) return 1;
    if (*i < *j) return -1;
    if (*i == *j) return 0;
    return 1;
}

void freeintr(void) {}



//int main() {
//    Vector v = {.len = 4, .vector = (double*)calloc(v.len, sizeof(double))};
//    v.vector[0] = 4.5;
//    v.vector[1] = 3.5;
//    v.vector[2] = 2.5;
//    v.vector[3] = -2.5;
//    Vector u = {.len = 4, .vector = NULL};
//
//    int i = copyIfNormIsLarger(&v, &u);
//    printf("%d\n", i);
//    freeVector(&v);
//    printf("%d\n", u.len);
//    printf("%f %f %f %f", u.vector[0], u.vector[1], u.vector[2], u.vector[3]);
//    RBTree *r = newRBTree(stringCompare, freeString);
//    int n = 15;
//    int *input = (int*)calloc(n,sizeof(int));
//    for(int i=0;i<n;i++)
//    {
//        input[i] = i;
//    }
//    for(int i=0;i<n;i++)
//    {
//        insertToRBTree(r, &input[i]);
//    }
//    printRBTree(r->root);
//    int j = deleteFromRBTree(r, &input[4]);
//    printRBTree(r->root);
//    printf("\n%d", j);

//    int *t = &input[2];
//
//    int data = 10;
//    int i = insertToRBTree(r, &data);
//    int data1 = 6;
//    int i1 = insertToRBTree(r, &data1);
//    int data2 = 3;
//    int i2 = insertToRBTree(r, &data2);
//    int data3 = 20;
//    int i3 = insertToRBTree(r, &data3);
//    int data4 = 7;
//    int i4 = insertToRBTree(r, &data4);
//    int data5 = 8;
//    int i5 = insertToRBTree(r, &data5);
//    int data6 = 9;
//    int i6 = insertToRBTree(r, &data6);
//    int data7 = 10;
//    int i7 = insertToRBTree(r, &data7);

//}