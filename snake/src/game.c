#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "linked_list.h"
#include "mbstrings.h"

void updateDirection(enum input_key input, snake_t* snake){
    switch (input)
    {
        case INPUT_UP:
            if((*snake).length != 1){
                if((*snake).curr_direction != DOWN){
                    (*snake).curr_direction = UP;
                }
            }
            else{
                (*snake).curr_direction = UP;
            }
            break;
        case INPUT_DOWN:
            if((*snake).length != 1){
                if((*snake).curr_direction != UP){
                    (*snake).curr_direction = DOWN;
                }
            }
            else{
                (*snake).curr_direction = DOWN;
            }
            break;
        case INPUT_RIGHT:
            if((*snake).length != 1){
                if((*snake).curr_direction != LEFT){
                    (*snake).curr_direction = RIGHT;
                }
            }
            else{
                (*snake).curr_direction = RIGHT;
            }
            break;
        case INPUT_LEFT:
            if((*snake).length != 1){
                if((*snake).curr_direction != RIGHT){
                    (*snake).curr_direction = LEFT;
                }
            }
            else{
                (*snake).curr_direction = LEFT;
            }
            break;          
        default:
            break;
    }
}

void updateBoard(int* cells, size_t width, size_t height, int new_cell, snake_t* snake, int growing){
    if(cells[new_cell] == FLAG_WALL){
        g_game_over = 1;
    }
    else{
        node_t* nodes = snake->snake_cells;
        if(new_cell == FLAG_FOOD){
            cells[(int) new_cell] = FLAG_PLAIN_CELL;
            g_score ++;
            place_food(cells, width, height);
            while(nodes){
                if(cells[*((int*)nodes->data)] == FLAG_FOOD){
                    place_food(cells, width, height);
                }
                if(nodes->next){
                    nodes = nodes->next;
                }
            }
        }
        int* ptr = &new_cell;
        insert_first(&snake->snake_cells, (void*)ptr, sizeof(int));
        cells[new_cell] = FLAG_SNAKE;
        
        if(growing == 0){
            int remove_cell = *(int*)remove_last(&nodes);
            cells[remove_cell] = FLAG_PLAIN_CELL;
        }
    }

}

// void updateBody(int*cells, node_t* body){
//     int* front_ptr = (int*)body->prev->data;
//     int* curr_ptr = (int*)body->data;
//     int new_cell = *front_ptr;
//     int curr_cell = *curr_ptr;
//     cells[curr_cell] = FLAG_PLAIN_CELL;
//     cells[new_cell] = FLAG_SNAKE;
//     body->data = (void*)&new_cell;
// }
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
    
    updateDirection(input, snake_p);


    int* ptr = (int*)snake_p->snake_cells->data;
    int first_cell = *ptr;
    //The index of the cell at the beginning of the list


    if((*snake_p).curr_direction == RIGHT){
        first_cell++;
        updateBoard(cells, width, height, first_cell, snake_p, growing);
    }
    else if((*snake_p).curr_direction == LEFT){
        first_cell--;
        updateBoard(cells, width, height, first_cell, snake_p, growing);
    }
    else if((*snake_p).curr_direction == UP){
        first_cell -= (int)*(&width);
        updateBoard(cells, width, height, first_cell, snake_p, growing);
    }
    else if((*snake_p).curr_direction == DOWN){
        first_cell += (int)*(&width);
        updateBoard(cells, width, height, first_cell, snake_p, growing);
    }

    // if(first->next){
    //     node_t* body = first->next;
    //     while(body){
    //         updateBody(cells, body);
    //         if(body->next){
    //             body = body->next;
    //         }
    //     }
    // }
    
    snake_p->snake_cells->data = (void*)&first_cell;
    
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
    free(snake_p);
    // TODO: implement!
}
