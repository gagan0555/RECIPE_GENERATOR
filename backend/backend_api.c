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

void handle_add_recipe(int argc, char* argv[]) {
    if (argc < 5) {
        printf("{\"error\": \"Missing arguments\", \"success\": false}\n");
        return;
    }

    char* name = argv[2];
    char* ingredients_csv = argv[3];
    char* instructions = argv[4];

    char ingredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int count = 0;
    char temp[1000];
    strcpy(temp, ingredients_csv);
    char* token = strtok(temp, ",");

    while (token && count < MAX_INGREDIENTS) {
        char trimmed[MAX_ING_LENGTH];
        trim_string(token, trimmed);
        if (strlen(trimmed) > 0) {
            strcpy(ingredients[count], trimmed);
            count++;
        }
        token = strtok(NULL, ",");
    }

    if (count == 0) {
        printf("{\"error\": \"No ingredients provided\", \"success\": false}\n");
        return;
    }

    if (addRecipe(&global_manager, name, ingredients, count, instructions)) {
        saveRecipesToFile(&global_manager, "recipes.bin");
        printf("{\"success\": true, \"message\": \"Recipe added successfully\", \"id\": %d}\n",
               global_manager.recipes[global_manager.recipeCount - 1].id);
    } else {
        printf("{\"error\": \"Failed to add recipe\", \"success\": false}\n");
    }
}

void handle_search_recipes(int argc, char* argv[]) {
    if (argc < 3) {
        printf("{\"success\": true, \"recipes\": []}\n");
        return;
    }

    char* ingredients_csv = argv[2];
    char ingredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int count = 0;
    char temp[1000];
    strcpy(temp, ingredients_csv);
    char* token = strtok(temp, ",");

    while (token && count < MAX_INGREDIENTS) {
        char trimmed[MAX_ING_LENGTH];
        trim_string(token, trimmed);
        if (strlen(trimmed) > 0) {
            char lowercase[MAX_ING_LENGTH];
            strcpy(lowercase, trimmed);
            for (int i = 0; lowercase[i]; i++) lowercase[i] = tolower(lowercase[i]);
            strcpy(ingredients[count], lowercase);
            count++;
        }
        token = strtok(NULL, ",");
    }

    int foundRecipes[MAX_RECIPES] = {0};
    int recipeScores[MAX_RECIPES] = {0};

    for (int i = 0; i < count; i++) {
        int resultIds[MAX_RECIPES];
        int result_count;
        findRecipesByIngredient(&global_manager.ingredientMap, ingredients[i], resultIds, &result_count);

        for (int j = 0; j < result_count; j++) {
            int recipeId = resultIds[j];
            foundRecipes[recipeId] = 1;
            recipeScores[recipeId]++;
        }
    }

    printf("{\"success\": true, \"recipes\": [");
    int first = 1;

    for (int score = count; score > 0; score--) {
        for (int i = 1; i <= global_manager.recipeCount; i++) {
            if (foundRecipes[i] && recipeScores[i] == score) {
                if (!first) printf(",");
                Recipe* recipe = &global_manager.recipes[i - 1];
                char escaped_name[256], escaped_instructions[1024];
                escape_json_string(recipe->name, escaped_name);
                escape_json_string(recipe->instructions, escaped_instructions);

                printf("{\"id\": %d, \"name\": \"%s\", \"score\": %.1f, \"ingredients\": [",
                       recipe->id, escaped_name, (float)score / count * 100);

                for (int j = 0; j < recipe->ingredientCount; j++) {
                    char escaped_ing[256];
                    escape_json_string(recipe->ingredients[j], escaped_ing);
                    if (j > 0) printf(", ");
                    printf("\"%s\"", escaped_ing);
                }

                printf("], \"instructions\": \"%s\"}", escaped_instructions);
                first = 0;
            }
        }
    }

    printf("]}\n");
}

void handle_search_ranked(int argc, char* argv[]) {
    if (argc < 3) {
        printf("{\"success\": true, \"recipes\": []}\n");
        return;
    }

    char* ingredients_csv = argv[2];
    char ingredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int count = 0;
    char temp[1000];
    strcpy(temp, ingredients_csv);
    char* token = strtok(temp, ",");

    while (token && count < MAX_INGREDIENTS) {
        char trimmed[MAX_ING_LENGTH];
        trim_string(token, trimmed);
        if (strlen(trimmed) > 0) {
            char lowercase[MAX_ING_LENGTH];
            strcpy(lowercase, trimmed);
            for (int i = 0; lowercase[i]; i++) lowercase[i] = tolower(lowercase[i]);
            strcpy(ingredients[count], lowercase);
            count++;
        }
        token = strtok(NULL, ",");
    }

    RecipeHeap heap;
    initRecipeHeap(&heap);

    int foundRecipes[MAX_RECIPES] = {0};
    int recipeScores[MAX_RECIPES] = {0};

    for (int i = 0; i < count; i++) {
        int resultIds[MAX_RECIPES];
        int result_count;
        findRecipesByIngredient(&global_manager.ingredientMap, ingredients[i], resultIds, &result_count);

        for (int j = 0; j < result_count; j++) {
            int recipeId = resultIds[j];
            foundRecipes[recipeId] = 1;
            recipeScores[recipeId]++;
        }
    }

    for (int i = 1; i <= global_manager.recipeCount; i++) {
        if (foundRecipes[i]) {
            Recipe* recipe = &global_manager.recipes[i - 1];
            insertRankedRecipe(&heap, i, recipeScores[i], recipe->ingredientCount, count);
        }
    }

    printf("{\"success\": true, \"recipes\": [");
    int first = 1;
    while (heap.size > 0) {
        RankedRecipe ranked = extractMax(&heap);
        Recipe* recipe = &global_manager.recipes[ranked.recipeId - 1];

        if (!first) printf(",");

        char escaped_name[256], escaped_instructions[1024];
        escape_json_string(recipe->name, escaped_name);
        escape_json_string(recipe->instructions, escaped_instructions);

        printf("{\"id\": %d, \"name\": \"%s\", \"score\": %.1f, \"ingredients\": [",
               recipe->id, escaped_name, ranked.score);

        for (int j = 0; j < recipe->ingredientCount; j++) {
            char escaped_ing[256];
            escape_json_string(recipe->ingredients[j], escaped_ing);
            if (j > 0) printf(", ");
            printf("\"%s\"", escaped_ing);
        }
        printf("], \"instructions\": \"%s\"}", escaped_instructions);
        first = 0;
    }
    printf("]}\n");
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
        printf("{\"success\": true, \"recipes\": []}\n");
        return;
    }

    char* ingredients_csv = argv[2];
    char ingredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int count = 0;
    char temp[1000];
    strcpy(temp, ingredients_csv);
    char* token = strtok(temp, ",");

    while (token && count < MAX_INGREDIENTS) {
        char trimmed[MAX_ING_LENGTH];
        trim_string(token, trimmed);
        if (strlen(trimmed) > 0) {
            char lowercase[MAX_ING_LENGTH];
            strcpy(lowercase, trimmed);
            for (int i = 0; lowercase[i]; i++) lowercase[i] = tolower(lowercase[i]);
            strcpy(ingredients[count], lowercase);
            count++;
        }
        token = strtok(NULL, ",");
    }

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

    for (int i = 0; i < expanded_count; i++) {
        int resultIds[MAX_RECIPES];
        int result_count;
        findRecipesByIngredient(&global_manager.ingredientMap, expanded_ingredients[i], resultIds, &result_count);

        for (int j = 0; j < result_count; j++) {
            int recipeId = resultIds[j];
            foundRecipes[recipeId] = 1;
            recipeScores[recipeId]++;
        }
    }

    printf("{\"success\": true, \"recipes\": [");
    int first = 1;
    for (int score = expanded_count; score > 0; score--) {
        for (int i = 1; i <= global_manager.recipeCount; i++) {
            if (foundRecipes[i] && recipeScores[i] == score) {
                if (!first) printf(",");
                Recipe* recipe = &global_manager.recipes[i - 1];

                char escaped_name[256], escaped_instructions[1024];
                escape_json_string(recipe->name, escaped_name);
                escape_json_string(recipe->instructions, escaped_instructions);

                printf("{\"id\": %d, \"name\": \"%s\", \"score\": %.1f, \"ingredients\": [",
                       recipe->id, escaped_name, (float)score / expanded_count * 100);

                for (int j = 0; j < recipe->ingredientCount; j++) {
                    char escaped_ing[256];
                    escape_json_string(recipe->ingredients[j], escaped_ing);
                    if (j > 0) printf(", ");
                    printf("\"%s\"", escaped_ing);
                }
                printf("], \"instructions\": \"%s\"}", escaped_instructions);
                first = 0;
            }
        }
    }
    printf("]}\n");
}

void handle_list_recipes() {
    printf("{\"success\": true, \"recipes\": [");
    for (int i = 0; i < global_manager.recipeCount; i++) {
        if (i > 0) printf(",");

        Recipe* recipe = &global_manager.recipes[i];
        char escaped_name[256], escaped_instructions[1024];
        escape_json_string(recipe->name, escaped_name);
        escape_json_string(recipe->instructions, escaped_instructions);

        printf("{\"id\": %d, \"name\": \"%s\", \"ingredients\": [", recipe->id, escaped_name);

        for (int j = 0; j < recipe->ingredientCount; j++) {
            char escaped_ing[256];
            escape_json_string(recipe->ingredients[j], escaped_ing);
            if (j > 0) printf(", ");
            printf("\"%s\"", escaped_ing);
        }

        printf("], \"instructions\": \"%s\"}", escaped_instructions);
    }
    printf("]}\n");
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
        printf("{\"error\": \"Missing command\", \"success\": false}\n");
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
        printf("{\"error\": \"Unknown command\", \"success\": false}\n");
        cleanup_system();
        return 1;
    }

    cleanup_system();
    return 0;
}
