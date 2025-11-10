#ifndef RECIPE_H
#define RECIPE_H

#define MAX_NAME_LENGTH 100
#define MAX_INGREDIENTS 20
#ifndef MAX_ING_LENGTH
#define MAX_ING_LENGTH 50  // This is the missing macro causing error
#endif
#define MAX_INSTRUCTIONS 500

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    char ingredients[MAX_INGREDIENTS][MAX_ING_LENGTH];
    int ingredientCount;
    char instructions[MAX_INSTRUCTIONS];
} Recipe;

#endif
