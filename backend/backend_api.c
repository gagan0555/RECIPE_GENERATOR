#include "recipe_manager.h"
#include "ingredient_map.h"
#include "file_storage.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

RecipeManager global_manager;

// Escape special characters for JSON output
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
    if (!loadRecipesFromFile(&global_manager, "recipes.bin")) {
        loadSampleRecipes(&global_manager);
    }
}

void cleanup_system() {
    saveRecipesToFile(&global_manager, "recipes.bin");
    cleanupRecipeManager(&global_manager);
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
        printf("{\"error\": \"Missing ingredients\", \"success\": false, \"recipes\": []}\n");
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
            for (int i = 0; lowercase[i]; i++) {
                lowercase[i] = tolower(lowercase[i]);
            }
            strcpy(ingredients[count], lowercase);
            count++;
        }
        token = strtok(NULL, ",");
    }

    if (count == 0) {
        printf("{\"error\": \"No ingredients provided\", \"success\": false, \"recipes\": []}\n");
        return;
    }

    int foundRecipes[MAX_RECIPES] = {0};
    int recipeScores[MAX_RECIPES] = {0};

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

    printf("{\"success\": true, \"recipes\": [");
    int first = 1;

    for (int score = count; score > 0; score--) {
        for (int i = 1; i <= global_manager.recipeCount; i++) {
            if (foundRecipes[i] && recipeScores[i] == score) {
                if (!first) printf(",");

                Recipe* recipe = &global_manager.recipes[i - 1];
                char escaped_name[256];
                char escaped_instructions[1024];
                escape_json_string(recipe->name, escaped_name);
                escape_json_string(recipe->instructions, escaped_instructions);

                printf("{\"id\": %d, \"name\": \"%s\", \"score\": %.1f, \"matches\": %d, \"total_ingredients\": %d, \"ingredients\": [",
                       recipe->id, escaped_name, (float)score / count * 100, score, recipe->ingredientCount);

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

void handle_list_recipes(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    printf("{\"success\": true, \"recipes\": [");
    for (int i = 0; i < global_manager.recipeCount; i++) {
        if (i > 0) printf(",");

        Recipe* recipe = &global_manager.recipes[i];
        char escaped_name[256];
        char escaped_instructions[1024];
        escape_json_string(recipe->name, escaped_name);
        escape_json_string(recipe->instructions, escaped_instructions);

        printf("{\"id\": %d, \"name\": \"%s\", \"ingredients\": [",
               recipe->id, escaped_name);

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

void handle_statistics(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

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
    } else if (strcmp(command, "list") == 0) {
        handle_list_recipes(argc, argv);
    } else if (strcmp(command, "stats") == 0) {
        handle_statistics(argc, argv);
    } else {
        printf("{\"error\": \"Unknown command: %s\", \"success\": false}\n", command);
        cleanup_system();
        return 1;
    }

    cleanup_system();
    return 0;
}
