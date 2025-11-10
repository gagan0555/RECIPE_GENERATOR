#include "file_storage.h"
#include <stdio.h>
#include <string.h>

int saveRecipesToFile(RecipeManager* manager, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for writing.\n", filename);
        return 0;
    }

    fwrite(&manager->recipeCount, sizeof(int), 1, file);
    for (int i = 0; i < manager->recipeCount; i++) {
        fwrite(&manager->recipes[i], sizeof(Recipe), 1, file);
    }

    fclose(file);
    fprintf(stderr, "Successfully saved %d recipes to '%s'.\n", manager->recipeCount, filename);
    return 1;
}

int loadRecipesFromFile(RecipeManager* manager, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Info: No existing recipe file found. Starting fresh.\n");
        return 0;
    }

    manager->recipeCount = 0;
    cleanupIngredientMap(&manager->ingredientMap);
    initIngredientMap(&manager->ingredientMap);

    int count = 0;
    fread(&count, sizeof(int), 1, file);

    for (int i = 0; i < count; i++) {
        Recipe recipe;
        fread(&recipe, sizeof(Recipe), 1, file);
        manager->recipes[manager->recipeCount] = recipe;

        for (int j = 0; j < recipe.ingredientCount; j++) {
            addIngredientRecipe(&manager->ingredientMap, recipe.ingredients[j], recipe.id);
        }
        manager->recipeCount++;
    }

    fclose(file);
    fprintf(stderr, "Successfully loaded %d recipes from '%s'.\n", count, filename);
    return 1;
}

int exportRecipesToText(RecipeManager* manager, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not create file '%s'.\n", filename);
        return 0;
    }

    fprintf(file, "RECIPE GENERATOR - EXPORTED RECIPES\n");
    fprintf(file, "====================================\n");
    fprintf(file, "Total Recipes: %d\n\n", manager->recipeCount);

    for (int i = 0; i < manager->recipeCount; i++) {
        Recipe* recipe = &manager->recipes[i];
        fprintf(file, "Recipe #%d: %s\n", recipe->id, recipe->name);
        fprintf(file, "Ingredients (%d):\n", recipe->ingredientCount);
        for (int j = 0; j < recipe->ingredientCount; j++) {
            fprintf(file, " - %s\n", recipe->ingredients[j]);
        }
        fprintf(file, "\nInstructions:\n%s\n", recipe->instructions);
        fprintf(file, "\n-----------------------------------\n\n");
    }

    fclose(file);
    fprintf(stderr, "Successfully exported %d recipes to '%s'.\n", manager->recipeCount, filename);
    return 1;
}

int importRecipesFromText(RecipeManager* manager, const char* filename) {
    (void)manager;

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for reading.\n", filename);
        return 0;
    }

    fprintf(stderr, "Text import feature is simplified. Please use binary format for full import.\n");
    fprintf(stderr, "You can manually add recipes from the text file using the Add Recipe menu.\n");

    fclose(file);
    return 1;
}

int createBackup(RecipeManager* manager) {
    return saveRecipesToFile(manager, BACKUP_FILE);
}

int restoreFromBackup(RecipeManager* manager) {
    fprintf(stderr, "Restoring from backup...\n");
    return loadRecipesFromFile(manager, BACKUP_FILE);
}
