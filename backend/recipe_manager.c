#include "recipe_manager.h"
#include "ingredient_map.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

void initRecipeManager(RecipeManager* manager) {
    manager->recipeCount = 0;
    initIngredientMap(&manager->ingredientMap);
}

void cleanupRecipeManager(RecipeManager* manager) {
    (void)manager; // Unused parameter suppressed, no cleanup needed currently
}

void loadSampleRecipes(RecipeManager* manager) {
    // No printf to preserve API integrity

    char pasta_ingredients[3][MAX_ING_LENGTH] = {"pasta", "tomato", "cheese"};
    addRecipe(manager, "Simple Pasta", pasta_ingredients, 3,
              "1. Boil pasta 2. Add tomato sauce 3. Sprinkle cheese 4. Serve hot");

    char sandwich_ingredients[4][MAX_ING_LENGTH] = {"bread", "cheese", "tomato", "lettuce"};
    addRecipe(manager, "Cheese Sandwich", sandwich_ingredients, 4,
              "1. Toast bread 2. Add cheese 3. Add tomato and lettuce 4. Close sandwich");

    char salad_ingredients[4][MAX_ING_LENGTH] = {"lettuce", "tomato", "cucumber", "olive oil"};
    addRecipe(manager, "Fresh Salad", salad_ingredients, 4,
              "1. Wash vegetables 2. Chop lettuce, tomato, cucumber 3. Mix with olive oil 4. Serve fresh");

    char eggs_ingredients[3][MAX_ING_LENGTH] = {"eggs", "butter", "milk"};
    addRecipe(manager, "Scrambled Eggs", eggs_ingredients, 3,
              "1. Beat eggs 2. Melt butter 3. Cook eggs 4. Serve hot");

    char grilled_ingredients[3][MAX_ING_LENGTH] = {"bread", "cheese", "butter"};
    addRecipe(manager, "Grilled Cheese", grilled_ingredients, 3,
              "1. Butter bread 2. Add cheese 3. Grill sandwich 4. Serve hot");
}

int addRecipe(RecipeManager* manager, const char* name, char ingredients[][MAX_ING_LENGTH],
              int ingredientCount, const char* instructions) {
    if (manager->recipeCount >= MAX_RECIPES) {
        // storage full
        return 0;
    }

    Recipe* newRecipe = &manager->recipes[manager->recipeCount];
    newRecipe->id = manager->recipeCount + 1;

    strncpy(newRecipe->name, name, MAX_NAME_LENGTH);
    newRecipe->name[MAX_NAME_LENGTH - 1] = '\0';

    strncpy(newRecipe->instructions, instructions, MAX_INSTRUCTIONS);
    newRecipe->instructions[MAX_INSTRUCTIONS - 1] = '\0';

    newRecipe->ingredientCount = ingredientCount;

    for (int i = 0; i < ingredientCount; i++) {
        strncpy(newRecipe->ingredients[i], ingredients[i], MAX_ING_LENGTH);
        newRecipe->ingredients[i][MAX_ING_LENGTH - 1] = '\0';

        // This call updates ingredient map
        addIngredientRecipe(&manager->ingredientMap, ingredients[i], newRecipe->id);
    }

    manager->recipeCount++;
    return 1;
}

void searchRecipesByIngredients(RecipeManager* manager, char ingredients[][MAX_ING_LENGTH],
                                int ingredientCount) {
    int foundRecipes[MAX_RECIPES] = {0};
    int recipeScores[MAX_RECIPES] = {0};

    for (int i = 0; i < ingredientCount; i++) {
        int resultIds[MAX_RECIPES];
        int count;

        findRecipesByIngredient(&manager->ingredientMap, ingredients[i], resultIds, &count);

        for (int j = 0; j < count; j++) {
            int recipeId = resultIds[j];
            foundRecipes[recipeId] = 1;
            recipeScores[recipeId]++;
        }
    }

    // Output matched recipes for demonstration:
    printf("{\"success\": true, \"recipes\": [");
    int first = 1;
    for (int i = 1; i <= manager->recipeCount; i++) {
        if (foundRecipes[i]) {
            if (!first) printf(",");
            Recipe* recipe = &manager->recipes[i - 1];
            printf("{\"id\": %d, \"name\": \"%s\", \"score\": %d, \"ingredientCount\": %d}",
                   recipe->id, recipe->name, recipeScores[i], recipe->ingredientCount);
            first = 0;
        }
    }
    printf("]}\n");
}

void displayRecipe(const Recipe* recipe) {
    if (!recipe) return;

    printf("\nRecipe: %s\n", recipe->name);

    printf("Ingredients:\n");
    for (int i = 0; i < recipe->ingredientCount; i++) {
        printf(" - %s\n", recipe->ingredients[i]);
    }

    printf("Instructions: %s\n", recipe->instructions);
}
