#ifndef ENHANCED_UI_H
#define ENHANCED_UI_H

#include "recipe_manager.h"
#include "ingredient_substitution.h"
#include "recipe_ranking.h"
#include "file_storage.h"
#include "recipe.h"
// Function declarations for enhanced UI
void showEnhancedMenu(void);
void searchWithSubstitution(RecipeManager* manager, IngredientGraph* graph);
void searchWithRanking(RecipeManager* manager);
void manageSubstitutions(IngredientGraph* graph);
void fileManagementMenu(RecipeManager* manager);
void displayStatistics(RecipeManager* manager);
void advancedSearchMenu(RecipeManager* manager, IngredientGraph* graph);

#endif
