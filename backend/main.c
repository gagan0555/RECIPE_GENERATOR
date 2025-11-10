/*#include "cli.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    RecipeManager manager;
    int choice;
    
    printf("Initializing Recipe Generator System...\n");
    
    
    initRecipeManager(&manager);
    
   
    printf("Loading sample recipes...\n");
    loadSampleRecipes(&manager);
    
    printf("\nWelcome to the Recipe Generator System!\n");
    
    do {
        showMainMenu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                addRecipeInteractive(&manager);
                break;
            case 2:
                searchRecipesInteractive(&manager);
                break;
            case 3:
                displayAllRecipes(&manager);
                break;
            case 4:
                printf("\nThank you for using Recipe Generator System!\n");
                break;
            default:
                printf("\nInvalid choice. Please select 1-4.\n");
                break;
        }
        
        if (choice != 4) {
            printf("\nPress Enter to continue...");
            getchar();
            getchar(); 
        }
        
    } while (choice != 4);
    
   
    cleanupRecipeManager(&manager);
    
    return 0;
}*/
#include "enhanced_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include "cli.h"

int main() {
    RecipeManager manager;
    IngredientGraph substitutionGraph;
    int choice;
    
    printf("Initializing Recipe Generator System (Enhanced)...\n");
    
    // Initialize all systems
    initRecipeManager(&manager);
    initIngredientGraph(&substitutionGraph);
    
    // Try to load existing recipes
    printf("Loading recipes from file...\n");
    if (!loadRecipesFromFile(&manager, RECIPES_FILE)) {
        printf("Loading sample recipes instead...\n");
        loadSampleRecipes(&manager);
    }
    
    // Load default substitutions
    printf("Loading ingredient substitutions...\n");
    loadDefaultSubstitutions(&substitutionGraph);
    
    printf("\n===========================================\n");
    printf("Welcome to the Recipe Generator System!\n");
    printf("Enhanced with Ranking & Substitutions\n");
    printf("===========================================\n");
    
    do {
        showEnhancedMenu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                addRecipeInteractive(&manager);
                break;
                
            case 2:
                searchRecipesInteractive(&manager);
                break;
                
            case 3:
                searchWithRanking(&manager);
                break;
                
            case 4:
                searchWithSubstitution(&manager, &substitutionGraph);
                break;
                
            case 5:
                displayAllRecipes(&manager);
                break;
                
            case 6:
                manageSubstitutions(&substitutionGraph);
                break;
                
            case 7:
                fileManagementMenu(&manager);
                break;
                
            case 8:
                displayStatistics(&manager);
                break;
                
            case 9:
                // Save before exit
                printf("\nSaving recipes before exit...\n");
                saveRecipesToFile(&manager, RECIPES_FILE);
                printf("Thank you for using Recipe Generator System!\n");
                break;
                
            default:
                printf("\nInvalid choice. Please select 1-9.\n");
                break;
        }
        
        if (choice != 9) {
            printf("\nPress Enter to continue...");
            getchar();
            getchar();
        }
        
    } while (choice != 9);
    
    // Cleanup
    cleanupRecipeManager(&manager);
    cleanupIngredientGraph(&substitutionGraph);
    
    return 0;
}

