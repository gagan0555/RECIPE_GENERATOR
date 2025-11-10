#ifndef RECIPE_RANKING_H
#define RECIPE_RANKING_H

#include "recipe_manager.h"

#define MAX_HEAP_SIZE 100

// Structure for ranked recipe
typedef struct {
    int recipeId;
    float score;
    int matchCount;
    int totalIngredients;
} RankedRecipe;

// Max heap structure for ranking
typedef struct {
    RankedRecipe heap[MAX_HEAP_SIZE];
    int size;
} RecipeHeap;

// Function declarations
void initRecipeHeap(RecipeHeap* heap);
void insertRankedRecipe(RecipeHeap* heap, int recipeId, int matchCount, 
                        int totalIngredients, int userIngredientCount);
RankedRecipe extractMax(RecipeHeap* heap);
void heapifyDown(RecipeHeap* heap, int index);
void heapifyUp(RecipeHeap* heap, int index);
void rankAndDisplayRecipes(RecipeManager* manager, RecipeHeap* heap);
float calculateScore(int matchCount, int totalIngredients, int userIngredientCount);

#endif
