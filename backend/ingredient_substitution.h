#ifndef INGREDIENT_SUBSTITUTION_H
#define INGREDIENT_SUBSTITUTION_H
#include "recipe.h"
#define MAX_SUBSTITUTIONS 10
#define MAX_GRAPH_NODES 100

// Structure to represent ingredient substitution relationships
typedef struct SubstitutionNode {
    char ingredient[MAX_ING_LENGTH];
    char substitutes[MAX_SUBSTITUTIONS][MAX_ING_LENGTH];
    int substituteCount;
    struct SubstitutionNode* next;
} SubstitutionNode;

// Graph structure for ingredient substitutions
typedef struct {
    SubstitutionNode* nodes[MAX_GRAPH_NODES];
    int nodeCount;
} IngredientGraph;

// Function declarations
void initIngredientGraph(IngredientGraph* graph);
void addSubstitution(IngredientGraph* graph, const char* ingredient, const char* substitute);
void findSubstitutes(IngredientGraph* graph, const char* ingredient, 
                     char substitutes[][MAX_ING_LENGTH], int* count);
void displayAllSubstitutions(IngredientGraph* graph);
void loadDefaultSubstitutions(IngredientGraph* graph);
void cleanupIngredientGraph(IngredientGraph* graph);

#endif
