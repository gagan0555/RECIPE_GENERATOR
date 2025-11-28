#include "recipe_manager.h"
#include "ingredient_map.h"
#include "file_storage.h"
#include "recipe_ranking.h"
#include "ingredient_substitution.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

RecipeManager global_manager;
IngredientGraph global_graph;

// ============ HELPER FUNCTIONS ============

void escape_json_string(const char* input, char* output) {
    int j = 0;
    for (int i = 0; input[i]; i++) {
        switch (input[i]) {
            case '"': output[j++] = '\\'; output[j++] = '"'; break;
            case '\\': output[j++] = '\\'; output[j++] = '\\'; break;
            case '\n': output[j++] = '\\'; output[j++] = 'n'; break;
            case '\r': output[j++] = '\\'; output[j++] = 'r'; break;
            case '\t': output[j++] = '\\'; output[j++] = 't'; break;
            default: output[j++] = input[i]; break;
        }
    }
    output[j] = '\0';
}

void trim_string(char* str, char* result) {
    int start = 0, end = strlen(str) - 1;
    while (start <= end && isspace(str[start])) start++;
    while (end >= start && isspace(str[end])) end--;
    if (end < start) {
        result[0] = '\0';
        return;
    }
    strncpy(result, str + start, end - start + 1);
    result[end - start + 1] = '\0';
}

// ============ NEW EXTRACTION FUNCTIONS ============

// Parse CSV string into ingredient array
// Returns: number of ingredients parsed
// lowercase: if 1, converts to lowercase; if 0, keeps original case
int parse_ingredients_csv(const char* csv, char ingredients[][MAX_ING_LENGTH], int lowercase) {
    int count = 0;
    char temp[1000];
    strcpy(temp, csv);
    char* token = strtok(temp, ",");
    
    while (token && count < MAX_INGREDIENTS) {
        char trimmed[MAX_ING_LENGTH];
        trim_string(token, trimmed);
        if (strlen(trimmed) > 0) {
            if (lowercase) {
                for (int i = 0; trimmed[i]; i++) trimmed[i] = tolower(trimmed[i]);
            }
            strcpy(ingredients[count++], trimmed);
        }
        token = strtok(NULL, ",");
    }
    return count;
}

// Print JSON error message
void print_json_error(const char* message) {
    printf("{\"error\": \"%s\", \"success\": false}\n", message);
}

// Print start of JSON recipe array
void print_json_start() {
    printf("{\"success\": true, \"recipes\": [");
}

// Print end of JSON recipe array
void print_json_end() {
    printf("]}\n");
}

// Print a single recipe as JSON (with optional score)
void print_recipe_json(Recipe* recipe, float score, int is_first) {
    if (!is_first) printf(",");
    
    char escaped_name[256], escaped_instructions[1024];
    escape_json_string(recipe->name, escaped_name);
    escape_json_string(recipe->instructions, escaped_instructions);
    
    printf("{\"id\": %d, \"name\": \"%s\", \"score\": %.1f, \"ingredients\": [",
           recipe->id, escaped_name, score);
    
    for (int j = 0; j < recipe->ingredientCount; j++) {
        char escaped_ing[256];
        escape_json_string(recipe->ingredients[j], escaped_ing);
        if (j > 0) printf(", ");
        printf("\"%s\"", escaped_ing);
    }
    
    printf("], \"instructions\": \"%s\"}", escaped_instructions);
}

// Find matching recipes for given ingredients
void find_matching_recipes(char ingredients[][MAX_ING_LENGTH], int count,
                          int foundRecipes[], int recipeScores[]) {
    for (int i = 0; i < count; i++) {
        int resultIds[MAX_RECIPES];
        int result_count;
        findRecipesByIngredient(&global_manager.ingredientMap, ingredients[i], 
                               resultIds, &result_count);
        
        for (int j = 0; j < result_count; j++) {
            int recipeId = resultIds[j];
            foundRecipes[recipeId] = 1;
            recipeScores[recipeId]++;
        }
    }
}

void initialize_system() {
    initRecipeManager(&global_manager);
    initIngredientGraph(&global_graph);
    loadDefaultSubstitutions(&global_graph);
    if (!loadRecipesFromFile(&global_manager, "recipes.bin")) {
        loadSampleRecipes(&global_manager);
    }
}

void cleanup_system() {
    saveRecipesToFile(&global_manager, "recipes.bin");
    cleanupRecipeManager(&global_manager);
    cleanupIngredientGraph(&global_graph);
}

// ============ COMMAND HANDLERS ============

void handle_add_recipe(int argc, char* argv[]) {
    if (argc < 5) {
        print_json_error("Missing arguments");
        return;
    }

    char* name = argv[2];
    char ingredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int count = parse_ingredients_csv(argv[3], ingredients, 0);

    if (count == 0) {
        print_json_error("No ingredients provided");
        return;
    }

    if (addRecipe(&global_manager, name, ingredients, count, argv[4])) {
        saveRecipesToFile(&global_manager, "recipes.bin");
        printf("{\"success\": true, \"message\": \"Recipe added successfully\", \"id\": %d}\n",
               global_manager.recipes[global_manager.recipeCount - 1].id);
    } else {
        print_json_error("Failed to add recipe");
    }
}

void handle_search_recipes(int argc, char* argv[]) {
    if (argc < 3) {
        print_json_start();
        print_json_end();
        return;
    }

    char ingredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int count = parse_ingredients_csv(argv[2], ingredients, 1);

    int foundRecipes[MAX_RECIPES] = {0};
    int recipeScores[MAX_RECIPES] = {0};
    find_matching_recipes(ingredients, count, foundRecipes, recipeScores);

    print_json_start();
    int first = 1;

    for (int score = count; score > 0; score--) {
        for (int i = 1; i <= global_manager.recipeCount; i++) {
            if (foundRecipes[i] && recipeScores[i] == score) {
                print_recipe_json(&global_manager.recipes[i - 1], 
                                (float)score / count * 100, first);
                first = 0;
            }
        }
    }

    print_json_end();
}

void handle_search_ranked(int argc, char* argv[]) {
    if (argc < 3) {
        print_json_start();
        print_json_end();
        return;
    }

    char ingredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int count = parse_ingredients_csv(argv[2], ingredients, 1);

    RecipeHeap heap;
    initRecipeHeap(&heap);

    int foundRecipes[MAX_RECIPES] = {0};
    int recipeScores[MAX_RECIPES] = {0};
    find_matching_recipes(ingredients, count, foundRecipes, recipeScores);

    for (int i = 1; i <= global_manager.recipeCount; i++) {
        if (foundRecipes[i]) {
            Recipe* recipe = &global_manager.recipes[i - 1];
            insertRankedRecipe(&heap, i, recipeScores[i], recipe->ingredientCount, count);
        }
    }

    print_json_start();
    int first = 1;
    while (heap.size > 0) {
        RankedRecipe ranked = extractMax(&heap);
        print_recipe_json(&global_manager.recipes[ranked.recipeId - 1], ranked.score, first);
        first = 0;
    }

    print_json_end();
}

void handle_get_substitutes(int argc, char* argv[]) {
    if (argc < 3) {
        printf("{\"success\": true, \"ingredient\": \"\", \"substitutes\": []}\n");
        return;
    }

    char* ingredient = argv[2];
    char substitutes[MAX_SUBSTITUTIONS][MAX_ING_LENGTH];
    int count = 0;

    findSubstitutes(&global_graph, ingredient, substitutes, &count);

    printf("{\"success\": true, \"ingredient\": \"%s\", \"substitutes\": [", ingredient);
    for (int i = 0; i < count; i++) {
        if (i > 0) printf(", ");
        printf("\"%s\"", substitutes[i]);
    }
    printf("]}\n");
}

void handle_search_with_substitutes(int argc, char* argv[]) {
    if (argc < 3) {
        print_json_start();
        print_json_end();
        return;
    }

    char ingredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int count = parse_ingredients_csv(argv[2], ingredients, 1);

    char expanded_ingredients[MAX_INGREDIENTS * MAX_SUBSTITUTIONS][MAX_ING_LENGTH];
    int expanded_count = 0;

    for (int i = 0; i < count; i++) {
        strcpy(expanded_ingredients[expanded_count++], ingredients[i]);

        char substitutes[MAX_SUBSTITUTIONS][MAX_ING_LENGTH];
        int sub_count = 0;
        findSubstitutes(&global_graph, ingredients[i], substitutes, &sub_count);

        for (int j = 0; j < sub_count && expanded_count < MAX_INGREDIENTS * MAX_SUBSTITUTIONS; j++) {
            strcpy(expanded_ingredients[expanded_count++], substitutes[j]);
        }
    }

    int foundRecipes[MAX_RECIPES] = {0};
    int recipeScores[MAX_RECIPES] = {0};
    find_matching_recipes(expanded_ingredients, expanded_count, foundRecipes, recipeScores);

    print_json_start();
    int first = 1;
    for (int score = expanded_count; score > 0; score--) {
        for (int i = 1; i <= global_manager.recipeCount; i++) {
            if (foundRecipes[i] && recipeScores[i] == score) {
                print_recipe_json(&global_manager.recipes[i - 1],
                                (float)score / expanded_count * 100, first);
                first = 0;
            }
        }
    }

    print_json_end();
}

void handle_list_recipes() {
    print_json_start();
    for (int i = 0; i < global_manager.recipeCount; i++) {
        print_recipe_json(&global_manager.recipes[i], 0, i == 0);
    }
    print_json_end();
}

void handle_statistics() {
    int total_ingredients = 0;
    for (int i = 0; i < global_manager.recipeCount; i++) {
        total_ingredients += global_manager.recipes[i].ingredientCount;
    }

    float avg_ingredients = global_manager.recipeCount > 0 ?
        (float)total_ingredients / global_manager.recipeCount : 0;

    printf("{\"success\": true, \"total_recipes\": %d, \"avg_ingredients\": %.1f}\n",
           global_manager.recipeCount, avg_ingredients);
}

int main(int argc, char* argv[]) {
    initialize_system();

    if (argc < 2) {
        print_json_error("Missing command");
        cleanup_system();
        return 1;
    }

    char* command = argv[1];

    if (strcmp(command, "add") == 0) {
        handle_add_recipe(argc, argv);
    } else if (strcmp(command, "search") == 0) {
        handle_search_recipes(argc, argv);
    } else if (strcmp(command, "search-ranked") == 0) {
        handle_search_ranked(argc, argv);
    } else if (strcmp(command, "get-substitutes") == 0) {
        handle_get_substitutes(argc, argv);
    } else if (strcmp(command, "search-substitutes") == 0) {
        handle_search_with_substitutes(argc, argv);
    } else if (strcmp(command, "list") == 0) {
        handle_list_recipes();
    } else if (strcmp(command, "stats") == 0) {
        handle_statistics();
    } else {
        saveRecipesToFile(&manager, RECIPES_FILE);
        print_json_error("Unknown command");
        cleanup_system();
        return 1;
    }

    cleanup_system();
    return 0;
}
