#ifndef INGREDIENT_MAP_H
#define INGREDIENT_MAP_H

#include "recipe.h"

#define MAX_INGREDIENT_RECIPES 50

typedef struct {
    char ingredient[MAX_ING_LENGTH];
    int recipeIds[MAX_INGREDIENT_RECIPES];
    int recipeCount;
} IngredientNode;

typedef struct {
    IngredientNode nodes[MAX_INGREDIENTS];
    int nodeCount;
} IngredientMap;

void initIngredientMap(IngredientMap* map);
void addIngredientRecipe(IngredientMap* map, const char* ingredient, int recipeId);
void findRecipesByIngredient(IngredientMap* map, const char* ingredient, int resultIds[], int* resultCount);
void cleanupIngredientMap(IngredientMap* map);
void addIngredientsToMap(IngredientMap* map, int recipeId, char ingredients[][MAX_ING_LENGTH], int ingredientCount);

#endif
