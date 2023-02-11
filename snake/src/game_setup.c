#include "game_setup.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Some handy dandy macros for decompression
#define E_CAP_HEX 0x45
#define E_LOW_HEX 0x65
#define S_CAP_HEX 0x53
#define S_LOW_HEX 0x73
#define W_CAP_HEX 0x57
#define W_LOW_HEX 0x77
#define DIGIT_START 0x30
#define DIGIT_END 0x39

int g_col;
int g_rows;
int g_snakes;

/** Initializes the board with walls around the edge of the board.
 *
 * Modifies values pointed to by cells_p, width_p, and height_p and initializes
 * cells array to reflect this default board.
 *
 * Returns INIT_SUCCESS to indicate that it was successful.
 *
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 */
enum board_init_status initialize_default_board(int** cells_p, size_t* width_p,
                                                size_t* height_p) {
    *width_p = 20;
    *height_p = 10;
    int* cells = malloc(20 * 10 * sizeof(int));
    *cells_p = cells;
    for (int i = 0; i < 20 * 10; i++) {
        cells[i] = FLAG_PLAIN_CELL;
    }

    // Set edge cells!
    // Top and bottom edges:
    for (int i = 0; i < 20; ++i) {
        cells[i] = FLAG_WALL;
        cells[i + (20 * (10 - 1))] = FLAG_WALL;
    }
    // Left and right edges:
    for (int i = 0; i < 10; ++i) {
        cells[i * 20] = FLAG_WALL;
        cells[i * 20 + 20 - 1] = FLAG_WALL;
    }

    // Add snake
    cells[20 * 2 + 2] = FLAG_SNAKE;
    g_snake_cell = 20 * 2 + 2;

    place_food(cells, *width_p, *height_p);

    return INIT_SUCCESS;
}

/** Initialize variables relevant to the game board.
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 *  - snake_p: a pointer to your snake struct (not used until part 2!)
 *  - board_rep: a string representing the initial board. May be NULL for
 * default board.
 */
enum board_init_status initialize_game(int** cells_p, size_t* width_p,
                                       size_t* height_p, snake_t* snake_p,
                                       char* board_rep) {
    enum board_init_status value;
    if(board_rep == NULL){
        value = initialize_default_board(cells_p, width_p, height_p);
    }
    else{
        value = decompress_board_str(cells_p, width_p, height_p, snake_p, board_rep);
    }
    
    g_game_over = 0;  // 1 if game is over, 0 otherwise
    g_score = 0;      // game score: 1 point for every food eaten
    g_length = 1;
    g_curr_direction = RIGHT;

    return value;
}

enum board_init_status addToBoard(int** cells_p, int num, char* token, size_t* width_p){
    char* numStr = &token[0];
    token = strtok(NULL, numStr);
    while(*token >= DIGIT_START && *token <= DIGIT_END){
        numStr = strcat(numStr, token);
        token = strtok(NULL, numStr);
    }
    int value = atoi(numStr);
    g_col = value;
    if(g_col > *((long*)width_p)){
        return INIT_ERR_INCORRECT_DIMENSIONS;
    }

    for(int i = 0; i < value; i++){
        cells_p[i] = &num;
    }
    return INIT_SUCCESS;
}

enum board_init_status initializeRow(int** cells_p, size_t* width_p, char* row, int rowNum){
    enum board_init_status value;
    while(row != NULL){
        char first = row[0];
        row = strtok(NULL, &first);
        if(first == E_CAP_HEX){
            value = addToBoard(cells_p, FLAG_PLAIN_CELL, row, width_p);
            if(value == INIT_ERR_INCORRECT_DIMENSIONS){
                return value;
            }
        }
        else if(first == W_CAP_HEX){
            value = addToBoard(cells_p, FLAG_WALL, row, width_p);
            if(value == INIT_ERR_INCORRECT_DIMENSIONS){
                return value;
            }
        }
        else if(first == S_CAP_HEX){
            value = addToBoard(cells_p, FLAG_SNAKE, row, width_p);
            g_snakes ++;
            if(value == INIT_ERR_INCORRECT_DIMENSIONS){
                return value;
            }
        }
        else{
            return INIT_ERR_BAD_CHAR;
        }
    }
    return INIT_SUCCESS;
}

/** Takes in a string `compressed` and initializes values pointed to by
 * cells_p, width_p, and height_p accordingly. Arguments:
 *      - cells_p: a pointer to the pointer representing the cells array
 *                 that we would like to initialize.
 *      - width_p: a pointer to the width variable we'd like to initialize.
 *      - height_p: a pointer to the height variable we'd like to initialize.
 *      - snake_p: a pointer to your snake struct (not used until part 2!)
 *      - compressed: a string that contains the representation of the board.
 * Note: We assume that the string will be of the following form:
 * B24x80|E5W2E73|E5W2S1E72... To read it, we scan the string row-by-row
 * (delineated by the `|` character), and read out a letter (E, S or W) a number
 * of times dictated by the number that follows the letter.
 */
enum board_init_status decompress_board_str(int** cells_p, size_t* width_p,
                                            size_t* height_p, snake_t* snake_p,
                                            char* compressed) {
    // TODO: implement!
    enum board_init_status value;
    char* del = "|"; 
    char* token = compressed;
    token = strtok(token, "x");
    *height_p = atoi(&token[1]);
    token = strtok(NULL, "x");

    token = strtok(token, del);
    *width_p = atoi(&token[1]);
    int* cells = malloc(*height_p * *width_p * sizeof(int));
    *cells_p = cells;

    token = strtok(NULL, del);

    g_rows = 0;
    while(token != NULL){
        value = initializeRow(cells_p, width_p, token, g_rows);
        g_rows++;
        if(value == INIT_ERR_INCORRECT_DIMENSIONS){
                return value;
        }
        if(value == INIT_ERR_BAD_CHAR){
                return value;
        }
        token = strtok(NULL, del);
    }

    if(g_snakes != 1){
        return INIT_ERR_WRONG_SNAKE_NUM;
    }
    if(g_rows > *((long*)height_p)){
        return INIT_ERR_INCORRECT_DIMENSIONS;
    }

    return INIT_SUCCESS;
}
