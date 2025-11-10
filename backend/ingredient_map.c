#include "ingredient_map.h"
#include <string.h>
#include <ctype.h>

// Convert string to lowercase - modifies in place
static void to_lowercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void initIngredientMap(IngredientMap* map) {
    map->nodeCount = 0;
    for (int i = 0; i < MAX_INGREDIENTS; i++) {
        map->nodes[i].recipeCount = 0;
    }
}

void addIngredientRecipe(IngredientMap* map, const char* ingredient, int recipeId) {
    char lowerIngredient[MAX_ING_LENGTH];
    strcpy(lowerIngredient, ingredient);
    to_lowercase(lowerIngredient);

    for (int i = 0; i < map->nodeCount; i++) {
        if (strcmp(map->nodes[i].ingredient, lowerIngredient) == 0) {
            if (map->nodes[i].recipeCount < MAX_INGREDIENT_RECIPES) {
                map->nodes[i].recipeIds[map->nodes[i].recipeCount++] = recipeId;
            }
            return;
        }
    }

    if (map->nodeCount < MAX_INGREDIENTS) {
        strcpy(map->nodes[map->nodeCount].ingredient, lowerIngredient);
        map->nodes[map->nodeCount].recipeIds[0] = recipeId;
        map->nodes[map->nodeCount].recipeCount = 1;
        map->nodeCount++;
    }
}

void findRecipesByIngredient(IngredientMap* map, const char* ingredient, int resultIds[], int* resultCount) {
    char lowerIngredient[MAX_ING_LENGTH];
    strcpy(lowerIngredient, ingredient);
    to_lowercase(lowerIngredient);

    *resultCount = 0;
    for (int i = 0; i < map->nodeCount; i++) {
        if (strcmp(map->nodes[i].ingredient, lowerIngredient) == 0) {
            for (int j = 0; j < map->nodes[i].recipeCount; j++) {
                resultIds[*resultCount] = map->nodes[i].recipeIds[j];
                (*resultCount)++;
            }
            return;
        }
    }
}

void cleanupIngredientMap(IngredientMap* map) {
    map->nodeCount = 0;
}

void addIngredientsToMap(IngredientMap* map, int recipeId, char ingredients[][MAX_ING_LENGTH], int ingredientCount) {
    for (int i = 0; i < ingredientCount; i++) {
        addIngredientRecipe(map, ingredients[i], recipeId);
    }
}
