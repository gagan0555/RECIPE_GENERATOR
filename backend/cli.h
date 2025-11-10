#ifndef CLI_H
#define CLI_H

#include "recipe_manager.h"


void showMainMenu(void);
void addRecipeInteractive(RecipeManager* manager);
void searchRecipesInteractive(RecipeManager* manager);
void displayAllRecipes(RecipeManager* manager);
void clearInputBuffer(void);

#endif
