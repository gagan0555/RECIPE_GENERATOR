#include "enhanced_ui.h"
#include <stdio.h>
#include <string.h>
#include "cli.h"

// Display enhanced main menu
void showEnhancedMenu(void) {
    printf("\n========================================\n");
    printf("   RECIPE GENERATOR SYSTEM - ENHANCED   \n");
    printf("========================================\n");
    printf("1. Add New Recipe\n");
    printf("2. Basic Search (Ingredients)\n");
    printf("3. Advanced Search with Ranking\n");
    printf("4. Search with Substitutions\n");
    printf("5. Display All Recipes\n");
    printf("6. Manage Ingredient Substitutions\n");
    printf("7. File Management (Save/Load)\n");
    printf("8. View Statistics\n");
    printf("9. Exit\n");
    printf("========================================\n");
    printf("Enter your choice (1-9): ");
}

// Search recipes with ingredient substitution
void searchWithSubstitution(RecipeManager* manager, IngredientGraph* graph) {
    char searchIngredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int searchCount;
    
    printf("\n=== SEARCH WITH INGREDIENT SUBSTITUTION ===\n");
    printf("Enter number of ingredients (max %d): ", MAX_INGREDIENTS);
    scanf("%d", &searchCount);
    clearInputBuffer(); 
    if (searchCount < 1 || searchCount > MAX_INGREDIENTS) {
        printf("Invalid ingredient count.\n");
        return;
    }
    
    printf("Enter ingredients you have:\n");
    void clearInputBuffer();
    for (int i = 0; i < searchCount; i++) {
        printf("Ingredient %d: ", i + 1);
        fgets(searchIngredients[i], MAX_ING_LENGTH, stdin);
        searchIngredients[i][strcspn(searchIngredients[i], "\n")] = 0;
    }
    
    // Expand search with substitutes
    char expandedIngredients[MAX_INGREDIENTS * 3][MAX_ING_LENGTH];
    int expandedCount = 0;
    
    for (int i = 0; i < searchCount; i++) {
        strcpy(expandedIngredients[expandedCount++], searchIngredients[i]);
        
        char substitutes[MAX_SUBSTITUTIONS][MAX_ING_LENGTH];
        int subCount;
        findSubstitutes(graph, searchIngredients[i], substitutes, &subCount);
        
        if (subCount > 0) {
            printf("\nFound %d substitutes for '%s': ", subCount, searchIngredients[i]);
            for (int j = 0; j < subCount; j++) {
                printf("%s", substitutes[j]);
                if (j < subCount - 1) printf(", ");
                strcpy(expandedIngredients[expandedCount++], substitutes[j]);
            }
            printf("\n");
        }
    }
    
    printf("\nSearching with %d ingredients (including substitutes)...\n", expandedCount);
    searchRecipesByIngredients(manager, expandedIngredients, expandedCount);
}

// Search with ranking system
void searchWithRanking(RecipeManager* manager) {
    char searchIngredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int searchCount;
    
    printf("\n=== ADVANCED SEARCH WITH RANKING ===\n");
    printf("Enter number of ingredients (max %d): ", MAX_INGREDIENTS);
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
        searchIngredients[i][strcspn(searchIngredients[i], "\n")] = 0;
    }
    
    // Find matching recipes and rank them
    RecipeHeap heap;
    initRecipeHeap(&heap);
    
    int foundRecipes[MAX_RECIPES] = {0};
    int recipeScores[MAX_RECIPES] = {0};
    
    // Calculate match scores
    for (int i = 0; i < searchCount; i++) {
        int resultIds[MAX_RECIPES];
        int count;
        findRecipesByIngredient(&manager->ingredientMap, searchIngredients[i], 
                               resultIds, &count);
        
        for (int j = 0; j < count; j++) {
            int recipeId = resultIds[j];
            foundRecipes[recipeId] = 1;
            recipeScores[recipeId]++;
        }
    }
    
    // Add to heap for ranking
    for (int i = 1; i <= manager->recipeCount; i++) {
        if (foundRecipes[i]) {
            Recipe* recipe = &manager->recipes[i-1];
            insertRankedRecipe(&heap, recipe->id, recipeScores[i], 
                             recipe->ingredientCount, searchCount);
        }
    }
    
    // Display ranked results
    rankAndDisplayRecipes(manager, &heap);
}

// Manage ingredient substitutions
void manageSubstitutions(IngredientGraph* graph) {
    int choice;
    
    printf("\n=== INGREDIENT SUBSTITUTION MANAGEMENT ===\n");
    printf("1. View All Substitutions\n");
    printf("2. Add New Substitution\n");
    printf("3. Find Substitutes for Ingredient\n");
    printf("4. Back to Main Menu\n");
    printf("Enter choice: ");
    scanf("%d", &choice);
    
    switch (choice) {
        case 1:
            displayAllSubstitutions(graph);
            break;
            
        case 2: {
            char ingredient[MAX_ING_LENGTH], substitute[MAX_ING_LENGTH];
            printf("Enter ingredient: ");
            clearInputBuffer();
            fgets(ingredient, MAX_ING_LENGTH, stdin);
            ingredient[strcspn(ingredient, "\n")] = 0;
            
            printf("Enter substitute: ");
            fgets(substitute, MAX_ING_LENGTH, stdin);
            substitute[strcspn(substitute, "\n")] = 0;
            
            addSubstitution(graph, ingredient, substitute);
            printf("Substitution added successfully!\n");
            break;
        }
        
        case 3: {
            char ingredient[MAX_ING_LENGTH];
            char substitutes[MAX_SUBSTITUTIONS][MAX_ING_LENGTH];
            int count;
            
            printf("Enter ingredient: ");
            clearInputBuffer();
            fgets(ingredient, MAX_ING_LENGTH, stdin);
            ingredient[strcspn(ingredient, "\n")] = 0;
            
            findSubstitutes(graph, ingredient, substitutes, &count);
            
            if (count == 0) {
                printf("No substitutes found for '%s'.\n", ingredient);
            } else {
                printf("\nSubstitutes for '%s':\n", ingredient);
                for (int i = 0; i < count; i++) {
                    printf("  %d. %s\n", i+1, substitutes[i]);
                }
            }
            break;
        }
    }
}

// File management menu
void fileManagementMenu(RecipeManager* manager) {
    int choice;
    
    printf("\n=== FILE MANAGEMENT ===\n");
    printf("1. Save Recipes to File\n");
    printf("2. Load Recipes from File\n");
    printf("3. Export to Text File\n");
    printf("4. Create Backup\n");
    printf("5. Restore from Backup\n");
    printf("6. Back to Main Menu\n");
    printf("Enter choice: ");
    scanf("%d", &choice);
    
    switch (choice) {
        case 1:
            saveRecipesToFile(manager, RECIPES_FILE);
            break;
        case 2:
            loadRecipesFromFile(manager, RECIPES_FILE);
            break;
        case 3: {
            char filename[100];
            printf("Enter filename for export: ");
            clearInputBuffer();
            fgets(filename, 100, stdin);
            filename[strcspn(filename, "\n")] = 0;
            exportRecipesToText(manager, filename);
            break;
        }
        case 4:
            createBackup(manager);
            break;
        case 5:
            restoreFromBackup(manager);
            break;
    }
}

// Display statistics about recipes
void displayStatistics(RecipeManager* manager) {
    printf("\n=== RECIPE DATABASE STATISTICS ===\n");
    printf("Total Recipes: %d\n", manager->recipeCount);
    printf("Storage Capacity: %d\n", MAX_RECIPES);
    printf("Usage: %.1f%%\n", (float)manager->recipeCount / MAX_RECIPES * 100);
    
    // Calculate average ingredients per recipe
    int totalIngredients = 0;
    for (int i = 0; i < manager->recipeCount; i++) {
        totalIngredients += manager->recipes[i].ingredientCount;
    }
    
    if (manager->recipeCount > 0) {
        float avgIngredients = (float)totalIngredients / manager->recipeCount;
        printf("Average Ingredients per Recipe: %.1f\n", avgIngredients);
    }
    
    printf("==================================\n");
}
