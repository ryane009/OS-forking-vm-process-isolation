#include "common.h"

#include <stdlib.h>

// TODO: define global variables needed for your snake! (part 1A)
int g_snake_cell;
int g_length = 1;
enum curr_direction g_curr_direction = RIGHT;
// Definition of global variables for game status.
int g_game_over = 0;
int g_score = 0;

/** Sets the seed for random number generation.
 * Arguments:
 *  - `seed`: the seed.
 */
void set_seed(unsigned seed) {
    /* DO NOT MODIFY THIS FUNCTION */
    srand(seed);
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Returns a random index in [0, size)
 * Arguments:
 *  - `size`: the upper bound for the generated value (exclusive).
 */
unsigned generate_index(unsigned size) {
    /* DO NOT MODIFY THIS FUNCTION */
    return rand() % size;
    /* DO NOT MODIFY THIS FUNCTION */
}
