#include "recipe_ranking.h"
#include <stdio.h>
#include <math.h>

// Initialize the heap
void initRecipeHeap(RecipeHeap* heap) {
    heap->size = 0;
}

// Calculate recipe score based on match percentage and completeness
float calculateScore(int matchCount, int totalIngredients, int userIngredientCount) {
    if (totalIngredients == 0) return 0.0;
    
    // Match percentage (0-100)
    float matchPercentage = (float)matchCount / totalIngredients * 100.0;
    
    // Bonus for using more user ingredients
    float usageBonus = (float)matchCount / userIngredientCount * 10.0;
    
    // Penalty for missing ingredients
    int missingIngredients = totalIngredients - matchCount;
    float penalty = missingIngredients * 5.0;
    
    // Final score
    float score = matchPercentage + usageBonus - penalty;
    return (score > 0) ? score : 0.0;
}

// Swap two ranked recipes
void swap(RankedRecipe* a, RankedRecipe* b) {
    RankedRecipe temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify down for max heap
void heapifyDown(RecipeHeap* heap, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    
    if (left < heap->size && heap->heap[left].score > heap->heap[largest].score) {
        largest = left;
    }
    
    if (right < heap->size && heap->heap[right].score > heap->heap[largest].score) {
        largest = right;
    }
    
    if (largest != index) {
        swap(&heap->heap[index], &heap->heap[largest]);
        heapifyDown(heap, largest);
    }
}

// Heapify up for max heap
void heapifyUp(RecipeHeap* heap, int index) {
    int parent = (index - 1) / 2;
    
    if (index > 0 && heap->heap[index].score > heap->heap[parent].score) {
        swap(&heap->heap[index], &heap->heap[parent]);
        heapifyUp(heap, parent);
    }
}

// Insert a ranked recipe into the heap
void insertRankedRecipe(RecipeHeap* heap, int recipeId, int matchCount, 
                        int totalIngredients, int userIngredientCount) {
    if (heap->size >= MAX_HEAP_SIZE) {
        printf("Heap is full!\n");
        return;
    }
    
    RankedRecipe recipe;
    recipe.recipeId = recipeId;
    recipe.matchCount = matchCount;
    recipe.totalIngredients = totalIngredients;
    recipe.score = calculateScore(matchCount, totalIngredients, userIngredientCount);
    
    heap->heap[heap->size] = recipe;
    heapifyUp(heap, heap->size);
    heap->size++;
}

// Extract the recipe with maximum score
RankedRecipe extractMax(RecipeHeap* heap) {
    RankedRecipe max = heap->heap[0];
    heap->heap[0] = heap->heap[heap->size - 1];
    heap->size--;
    heapifyDown(heap, 0);
    return max;
}

// Rank and display recipes from heap
void rankAndDisplayRecipes(RecipeManager* manager, RecipeHeap* heap) {
    printf("\n=== RANKED RECIPE RECOMMENDATIONS ===\n");
    
    if (heap->size == 0) {
        printf("No recipes to display.\n");
        return;
    }
    
    int rank = 1;
    RecipeHeap tempHeap = *heap; // Create a copy to preserve original
    
    while (tempHeap.size > 0) {
        RankedRecipe ranked = extractMax(&tempHeap);
        
        printf("\n--- RANK #%d (Score: %.2f) ---\n", rank++, ranked.score);
        printf("Match: %d/%d ingredients (%.1f%% complete)\n", 
               ranked.matchCount, ranked.totalIngredients,
               (float)ranked.matchCount / ranked.totalIngredients * 100);
        
        // Display the actual recipe
        for (int i = 0; i < manager->recipeCount; i++) {
            if (manager->recipes[i].id == ranked.recipeId) {
                displayRecipe(&manager->recipes[i]);
                break;
            }
        }
    }
    
    printf("=====================================\n");
}
