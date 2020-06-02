#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Structs.h"


#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))
const char NEW_LINE[] = "\n";

int stringCompare(const void *a, const void *b)
{
    const char *first = (char*)a;
    const char *second = (char*)b;
    return strcmp(first, second);
}


int concatenate(const void *word, void *pConcatenated)
{
    char *base = (char *)pConcatenated;
    strcat(base, word);
    strcat(base, NEW_LINE);
    if (base == NULL)
    {
        return 0;
    }
    return 1;
}


void freeString(void *s)
{
    free((char*)s);
    s = NULL;
}


int vectorCompare1By1(const void *a, const void *b)
{
    Vector *vec1 = (Vector *)a;
    Vector *vec2 = (Vector *)b;
    int minLen = MIN(vec1->len, vec2->len);;
    for (int i = 0; i < minLen; i++)
    {
        if (vec1->vector[i] < vec2->vector[i])
        {
            return -1;
        }
        else if (vec2->vector[i] < vec1->vector[i])
        {
            return 1;
        }
    }
    int longer = (vec1->len < vec2->len) ? -1 : 1;
    longer = (vec1->len == vec2->len) ? 0 : longer;
    return longer;
}


void freeVector(void *pVector)
{
    free(((Vector*)pVector)->vector);
    ((Vector*)pVector)->vector = NULL;
    free(pVector);
}


double getNorm(Vector *vec)
{
    double norm = 0;
    for (int i = 0; i < vec->len ; i++)
    {
        norm += vec->vector[i]*vec->vector[i];
    }
    return norm;
}



int copyIfNormIsLarger(const void *pVector, void *pMaxVector)
{
    if (pVector == NULL || pMaxVector == NULL)
    {
        return 0;
    }
    Vector *vec = (Vector *)pVector;
    if (vec->len == 0)
    {
        return 0;
    }
    Vector *max = (Vector *)pMaxVector;
    if (max->vector == NULL || (getNorm(vec) > getNorm(max)))
    {
        max->len = vec->len;
        max->vector = (double *) realloc(max->vector, vec->len*sizeof(double));
        if (max->vector == NULL)
        {
            return 0;
        }
        memcpy(max->vector, vec->vector, vec->len*sizeof(double));
        return 1;
    }

    return 1;
}


Vector *findMaxNormVectorInTree(RBTree *tree)
{
    if (tree == NULL)
    {
        return NULL;
    }
    Vector *max;
    max = (Vector *) malloc(sizeof(Vector));
    max->len = 0;
    max->vector = NULL;
    forEachRBTree(tree, copyIfNormIsLarger, max);
    return max;
}