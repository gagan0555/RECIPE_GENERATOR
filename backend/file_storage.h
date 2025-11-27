#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include "recipe_manager.h"

#define RECIPES_FILE "recipes.dat"
#define BACKUP_FILE "recipes_backup.dat"

// Function declarations
int saveRecipesToFile(RecipeManager* manager, const char* filename);
int loadRecipesFromFile(RecipeManager* manager, const char* filename);
int exportRecipesToText(RecipeManager* manager, const char* filename);
int importRecipesFromText(RecipeManager* manager, const char* filename);
int createBackup(RecipeManager* manager);
int restoreFromBackup(RecipeManager* manager);

#endif
