#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "linked_list.h"
#include "mbstrings.h"

void updateDirection(enum input_key input){
    switch (input)
    {
        case INPUT_UP:
            if(g_length != 1){
                if(g_curr_direction != DOWN){
                    g_curr_direction = UP;
                }
            }
            else{
                g_curr_direction = UP;
            }
            break;
        case INPUT_DOWN:
            if(g_length != 1){
                if(g_curr_direction != UP){
                    g_curr_direction = DOWN;
                }
            }
            else{
                g_curr_direction = DOWN;
            }
            break;
        case INPUT_RIGHT:
            if(g_length != 1){
                if(g_curr_direction != LEFT){
                    g_curr_direction = RIGHT;
                }
            }
            else{
                g_curr_direction = RIGHT;
            }
            break;
        case INPUT_LEFT:
            if(g_length != 1){
                if(g_curr_direction != RIGHT){
                    g_curr_direction = LEFT;
                }
            }
            else{
                g_curr_direction = LEFT;
            }
            break;          
        default:
            break;
    }
}
/** Updates the game by a single step, and modifies the game information
 * accordingly. Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: width of the board.
 *  - height: height of the board.
 *  - snake_p: pointer to your snake struct (not used until part 2!)
 *  - input: the next input.
 *  - growing: 0 if the snake does not grow on eating, 1 if it does.
 */
void update(int* cells, size_t width, size_t height, snake_t* snake_p,
            enum input_key input, int growing) {
    // `update` should update the board, your snake's data, and global
    // variables representing game information to reflect new state. If in the
    // updated position, the snake runs into a wall or itself, it will not move
    // and global variable g_game_over will be 1. Otherwise, it will be moved
    // to the new position. If the snake eats food, the game score (`g_score`)
    // increases by 1. This function assumes that the board is surrounded by
    // walls, so it does not handle the case where a snake runs off the board.

    // TODO: implement!
    
    updateDirection(input);

    if(cells[g_snake_cell + 1] == FLAG_WALL || cells[g_snake_cell - 1] == FLAG_WALL || cells[g_snake_cell + 20] == FLAG_WALL
    || cells[g_snake_cell - 20] == FLAG_WALL){
        g_game_over = 1;
    } 
    else{
        if(cells[g_snake_cell] == FLAG_FOOD){
            g_score ++;
            cells[g_snake_cell] = FLAG_PLAIN_CELL;
            place_food(cells, width, height);
        }

        if(g_curr_direction == RIGHT){
            cells[g_snake_cell] = FLAG_PLAIN_CELL;
            g_snake_cell += 1;
            cells[g_snake_cell] = FLAG_SNAKE;
        }
        else if(g_curr_direction == LEFT){
            cells[g_snake_cell] = FLAG_PLAIN_CELL;
            g_snake_cell -= 1;
            cells[g_snake_cell] = FLAG_SNAKE;
        }
        else if(g_curr_direction == UP){
            cells[g_snake_cell] = FLAG_PLAIN_CELL;
            g_snake_cell -= 20;
            cells[g_snake_cell] = FLAG_SNAKE;
        }
        else if(g_curr_direction == DOWN){
            cells[g_snake_cell] = FLAG_PLAIN_CELL;
            g_snake_cell += 20;
            cells[g_snake_cell] = FLAG_SNAKE;
        }
    } 
}

/** Sets a random space on the given board to food.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: the width of the board
 *  - height: the height of the board
 */
void place_food(int* cells, size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */
    unsigned food_index = generate_index(width * height);
    if (*(cells + food_index) == FLAG_PLAIN_CELL) {
        *(cells + food_index) = FLAG_FOOD;
    } else {
        place_food(cells, width, height);
    }
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Prompts the user for their name and saves it in the given buffer.
 * Arguments:
 *  - `write_into`: a pointer to the buffer to be written into.
 */
void read_name(char* write_into) {
    // TODO: implement! (remove the call to strcpy once you begin your
    // implementation)
    strcpy(write_into, "placeholder");
}

/** Cleans up on game over — should free any allocated memory so that the
 * LeakSanitizer doesn't complain.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - snake_p: a pointer to your snake struct. (not needed until part 2)
 */
void teardown(int* cells, snake_t* snake_p) {
    free(cells);
    // TODO: implement!
}
