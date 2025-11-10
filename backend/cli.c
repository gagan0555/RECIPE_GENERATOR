#include "cli.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void showMainMenu(void) {
    printf("\n========================================\n");
    printf("       RECIPE GENERATOR SYSTEM         \n");
    printf("========================================\n");
    printf("1. Add New Recipe\n");
    printf("2. Search Recipes by Ingredients\n");
    printf("3. Display All Recipes\n");
    printf("4. Exit\n");
    printf("========================================\n");
    printf("Enter your choice (1-4): ");
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void addRecipeInteractive(RecipeManager* manager) {
    char name[MAX_NAME_LENGTH];
    char ingredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    char instructions[MAX_INSTRUCTIONS];
    int ingredientCount;
    
    printf("\n=== ADD NEW RECIPE ===\n");
    
    printf("Enter recipe name: ");
    clearInputBuffer();
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = 0; 
    
   
    printf("Enter number of ingredients (max %d): ", MAX_INGREDIENTS);
    scanf("%d", &ingredientCount);
    
    if (ingredientCount < 1 || ingredientCount > MAX_INGREDIENTS) {
        printf("Invalid ingredient count. Recipe not added.\n");
        return;
    }
    
   
    printf("Enter ingredients:\n");
    clearInputBuffer();
    for (int i = 0; i < ingredientCount; i++) {
        printf("Ingredient %d: ", i + 1);
        fgets(ingredients[i], MAX_ING_LENGTH, stdin);
        ingredients[i][strcspn(ingredients[i], "\n")] = 0; 
    }
    
    
    printf("Enter cooking instructions: ");
    fgets(instructions, MAX_INSTRUCTIONS, stdin);
    instructions[strcspn(instructions, "\n")] = 0; 
    
    
    if (addRecipe(manager, name, ingredients, ingredientCount, instructions)) {
        printf("\nRecipe '%s' added successfully!\n", name);
    } else {
        printf("\nFailed to add recipe. Storage might be full.\n");
    }
}

void searchRecipesInteractive(RecipeManager* manager) {
    char searchIngredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int searchCount;
    
    printf("\n=== SEARCH RECIPES BY INGREDIENTS ===\n");
    
    printf("Enter number of ingredients to search with (max %d): ", MAX_INGREDIENTS);
    scanf("%d", &searchCount);
    
    if (searchCount < 1 || searchCount > MAX_INGREDIENTS) {
        printf("Invalid ingredient count.\n");
        return;
    }
    
    printf("Enter ingredients you have:\n");
    clearInputBuffer();
    for (int i = 0; i < searchCount; i++) {
        printf("Ingredient %d: ", i + 1);
        fgets(searchIngredients[i], MAX_ING_LENGTH, stdin);
        searchIngredients[i][strcspn(searchIngredients[i], "\n")] = 0; // Remove newline
    }
    
    searchRecipesByIngredients(manager, searchIngredients, searchCount);
}

void displayAllRecipes(RecipeManager* manager) {
    printf("\n=== ALL RECIPES ===\n");
    
    if (manager->recipeCount == 0) {
        printf("No recipes available.\n");
        return;
    }
    
    printf("Total recipes: %d\n", manager->recipeCount);
    
    for (int i = 0; i < manager->recipeCount; i++) {
        displayRecipe(&manager->recipes[i]);
    }
}
