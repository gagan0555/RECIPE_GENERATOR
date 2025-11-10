#ifndef RECIPE_MANAGER_H
#define RECIPE_MANAGER_H

#include "recipe.h"
#include "ingredient_map.h"

#define MAX_RECIPES 100

typedef struct {
    Recipe recipes[MAX_RECIPES];
    int recipeCount;
    IngredientMap ingredientMap;
} RecipeManager;


void initRecipeManager(RecipeManager* manager);
void loadSampleRecipes(RecipeManager* manager);
int addRecipe(RecipeManager* manager, const char* name, char ingredients[][MAX_ING_LENGTH], 
              int ingredientCount, const char* instructions);
void displayRecipe(const Recipe* recipe);
void searchRecipesByIngredients(RecipeManager* manager, char ingredients[][MAX_ING_LENGTH], 
                               int ingredientCount);
void cleanupRecipeManager(RecipeManager* manager);

#endif
