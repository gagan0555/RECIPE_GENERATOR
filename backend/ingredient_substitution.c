#include "ingredient_substitution.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Helper function to convert string to lowercase
void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Initialize the ingredient graph
void initIngredientGraph(IngredientGraph* graph) {
    graph->nodeCount = 0;
    for (int i = 0; i < MAX_GRAPH_NODES; i++) {
        graph->nodes[i] = NULL;
    }
}

// Find a node in the graph by ingredient name
SubstitutionNode* findNode(IngredientGraph* graph, const char* ingredient) {
    char lowerIngredient[MAX_ING_LENGTH];
    strcpy(lowerIngredient, ingredient);
    toLowerCase(lowerIngredient);
    
    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i] != NULL && 
            strcmp(graph->nodes[i]->ingredient, lowerIngredient) == 0) {
            return graph->nodes[i];
        }
    }
    return NULL;
}

// Add a substitution relationship between two ingredients
void addSubstitution(IngredientGraph* graph, const char* ingredient, const char* substitute) {
    char lowerIngredient[MAX_ING_LENGTH];
    char lowerSubstitute[MAX_ING_LENGTH];
    
    strcpy(lowerIngredient, ingredient);
    strcpy(lowerSubstitute, substitute);
    toLowerCase(lowerIngredient);
    toLowerCase(lowerSubstitute);
    
    // Find or create node for the ingredient
    SubstitutionNode* node = findNode(graph, lowerIngredient);
    if (node == NULL) {
        if (graph->nodeCount >= MAX_GRAPH_NODES) {
            printf("Graph is full. Cannot add more ingredients.\n");
            return;
        }
        
        node = (SubstitutionNode*)malloc(sizeof(SubstitutionNode));
        strcpy(node->ingredient, lowerIngredient);
        node->substituteCount = 0;
        node->next = NULL;
        graph->nodes[graph->nodeCount++] = node;
    }
    
    // Add substitute if not already present
    for (int i = 0; i < node->substituteCount; i++) {
        if (strcmp(node->substitutes[i], lowerSubstitute) == 0) {
            return; // Already exists
        }
    }
    
    if (node->substituteCount < MAX_SUBSTITUTIONS) {
        strcpy(node->substitutes[node->substituteCount++], lowerSubstitute);
    } else {
        printf("Maximum substitutions reached for %s\n", ingredient);
    }
}

// Find all substitutes for a given ingredient
void findSubstitutes(IngredientGraph* graph, const char* ingredient, 
                     char substitutes[][MAX_ING_LENGTH], int* count) {
    *count = 0;
    SubstitutionNode* node = findNode(graph, ingredient);
    
    if (node == NULL) {
        return;
    }
    
    for (int i = 0; i < node->substituteCount; i++) {
        strcpy(substitutes[*count], node->substitutes[i]);
        (*count)++;
    }
}

// Display all substitution relationships
void displayAllSubstitutions(IngredientGraph* graph) {
    printf("\n=== INGREDIENT SUBSTITUTION GRAPH ===\n");
    if (graph->nodeCount == 0) {
        printf("No substitutions available.\n");
        return;
    }
    
    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i] != NULL) {
            printf("\n%s can be substituted with:\n", graph->nodes[i]->ingredient);
            for (int j = 0; j < graph->nodes[i]->substituteCount; j++) {
                printf("  - %s\n", graph->nodes[i]->substitutes[j]);
            }
        }
    }
    printf("=====================================\n");
}

// Load default substitution relationships
void loadDefaultSubstitutions(IngredientGraph* graph) {
    // Dairy substitutions
    addSubstitution(graph, "milk", "almond milk");
    addSubstitution(graph, "milk", "soy milk");
    addSubstitution(graph, "milk", "oat milk");
    addSubstitution(graph, "butter", "margarine");
    addSubstitution(graph, "butter", "olive oil");
    addSubstitution(graph, "cheese", "vegan cheese");
    
    // Protein substitutions
    addSubstitution(graph, "eggs", "tofu");
    addSubstitution(graph, "eggs", "flax eggs");
    addSubstitution(graph, "chicken", "tofu");
    addSubstitution(graph, "chicken", "paneer");
    
    // Vegetable substitutions
    addSubstitution(graph, "tomato", "tomato paste");
    addSubstitution(graph, "lettuce", "spinach");
    addSubstitution(graph, "lettuce", "cabbage");
    addSubstitution(graph, "cucumber", "zucchini");
    
    // Carb substitutions
    addSubstitution(graph, "pasta", "rice noodles");
    addSubstitution(graph, "pasta", "zucchini noodles");
    addSubstitution(graph, "bread", "tortilla");
    addSubstitution(graph, "bread", "pita bread");
    
    // Oil substitutions
    addSubstitution(graph, "olive oil", "vegetable oil");
    addSubstitution(graph, "olive oil", "coconut oil");
    
    printf("Loaded %d ingredient substitution relationships.\n", graph->nodeCount);
}

// Cleanup the graph
void cleanupIngredientGraph(IngredientGraph* graph) {
    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i] != NULL) {
            free(graph->nodes[i]);
            graph->nodes[i] = NULL;
        }
    }
    graph->nodeCount = 0;
}
