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
    if(g_game_over == 1){
        return;
    }
    else{
        if(cells[new_cell] == FLAG_WALL || (cells[new_cell] == FLAG_SNAKE && new_cell != *((int*)get_last(snake->snake_cells)))){
            g_game_over = 1;
            return;
        }
        else{
            // node_t* nodes = snake->snake_cells;
            if(cells[new_cell] == FLAG_FOOD){
                cells[new_cell] = FLAG_PLAIN_CELL;
                g_score ++;
                // for(int i = 0; i < length_list(snake->snake_cells); i++){
                //     if(cells[*((int*)get(snake->snake_cells, i))] == FLAG_FOOD){
                //         cells[*((int*)get(snake->snake_cells, i))] = FLAG_SNAKE;
                //         place_food(cells, width, height);
                //     }
                // }
                snake->length++;
                if(growing == 0){
                    void* data = get_last(snake->snake_cells);
                    cells[*((int*)data)] = FLAG_PLAIN_CELL;
                    free(remove_last(&snake->snake_cells));
                }
                
                int* ptr = &new_cell;
                insert_first(&snake->snake_cells, ptr, sizeof(int));
                cells[new_cell] = FLAG_SNAKE;
                place_food(cells, width, height);
            }
            else{
                void* data = get_last(snake->snake_cells);
                cells[*((int*)data)] = FLAG_PLAIN_CELL;
                free(remove_last(&snake->snake_cells));

                int* ptr = &new_cell;
                insert_first(&snake->snake_cells, ptr, sizeof(int));
                cells[new_cell] = FLAG_SNAKE;
            }
        }
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

    if(g_game_over == 1){
        return;
    }
    
    updateDirection(input, snake_p);


    int* ptr = (int*)get_first(snake_p->snake_cells);
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
    int status;
    int val = 0;
    while(val == 0){
        printf("Name > ");
        fflush(stdout);
        status = read(0, write_into, 1000);
        write_into[status - 1] = '\0';
        if(strlen(write_into) != 0){
            val = 1;
            return;
        }
        else{
            printf("Name Invalid: must be longer than 0 characters\n");
        }
    }
    //strcpy(write_into, "placeholder");
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
    // free(snake_p);
    // TODO: implement!
    while(snake_p->snake_cells){
        free(remove_first(&snake_p->snake_cells));
    }
    
}
