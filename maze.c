#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "maze.h"
#define LINE_BUFFER_LEN 512
#define NUMBER_BUFFER_LEN 8
#define MIN_PORTAL_INDEX 0
#define MIN_NUM_OF_PORTALS 2
#define MAX_NUM_OF_PORTALS 9
#define NUM_OF_TARGETS 3

#define puts_no_newline(s) fputs(s, stdout)

// VERY IMPORTANT!!!
// IN ORDER TO MAKE MAZE2 LOOK AS IT SHOULD: CLOSE THE LEFT SIDE TOOLBAR AND RAISE THE TERMINAL
// ALL THE WAY UP AND THEN RUM THE MAZE.

const char *msg_unknown[] = {"Do or do not. There is no try",
                             "You must unlearn what you have learned",
                             "Fear is the path to the dark side",
                             "The greatest teacher, failure is",
                             "These aren't the droids your'e looking for",
                             "press a wrong key again, I dare you, I double dare you madafaka"};

const char *msg_quit[] = {"I'LL BE BACK",
                          "May the force be with you",
                          "We will always have Paris",
                          "Don't go breaking my heart"};

const char *msg_wall[] = {"YOU SHALL NOT PASS",
                          "Boop",
                          "Inconceivable!",
                          "Come at me bro",
                          "Highly illogical",
                          "Go ahead, make my day",
                          "The dude abides",
                          "Yeah... no... you can't",
                          "I'M WALKING HERE!",
                          "One does not simply walk into Mordor"};

const char *msg_win[] = {"Halleluja!",
                         "Praise the lord!",
                         "We are the champions, my friends",
                         "No time for losers",
                         "Elemantery, my dear Watson"};

const char *msg_portal[] = {"To infinity, AND BEYOND!",
                            "I have a feeling we're not in Kansas anymore",
                            "Where we're going we don't need roads!",
                            "Heeeeeeeeeeeere's Johnney!",
                            "Chewie, we're home"};



void delete_maze(maze_t *p_maze)
{
    if (NULL != p_maze) {
        if (NULL != p_maze->maze_tiles) {
            free(p_maze->maze_tiles);
            p_maze->maze_tiles = NULL;
        }

        // portals are circular linked list of 2-9
        if (NULL != p_maze->portals) {
            portal_t *head = p_maze->portals;
            portal_t *next;
            portal_t *curr = head;

            while (NULL != curr) {
                next = curr->next;
                free(curr);
                curr = next;

                if (next == head) {
                    break;
                }
            }
            p_maze->portals = NULL;
        }
        free(p_maze);
        p_maze = NULL;
    }
}

portal_t *create_portal(int x_pos, int y_pos, int portal_id)
{
    portal_t *new_portal;

    if (NULL == (new_portal = (portal_t *)malloc(sizeof(portal_t)))) {
        printf("failed to allocate memory for portal %d in %d x %d", portal_id, x_pos, y_pos);
        return NULL;
    }

    new_portal->id = portal_id;
    new_portal->portal_location.x = x_pos;
    new_portal->portal_location.y = y_pos;
    new_portal->next = NULL;

    return new_portal;

}

char first_line_from_text_file[LINE_BUFFER_LEN];

maze_t *read_maze(const char *input_file)
{
    FILE *in_f = NULL;

    char line_buffer[LINE_BUFFER_LEN];

    int number_of_portals = 0;
    portal_t *array_of_portals[MAX_NUM_OF_PORTALS - MIN_PORTAL_INDEX + 1];

    if (NULL == (in_f = fopen(input_file, "r"))) {
        printf("Failed to read the input file '%s'\n", input_file);
        return NULL;
    }

    // read first line
    fgets(first_line_from_text_file, LINE_BUFFER_LEN, in_f);

    int num_of_rows;
    int num_of_cols;

    // read second line
    fgets(line_buffer, LINE_BUFFER_LEN, in_f);
    sscanf(line_buffer, "%d %d", &num_of_rows, &num_of_cols);


    // create maze struct
    maze_t *p_maze;

    if (NULL == (p_maze = (maze_t *)malloc(sizeof(maze_t)))) {
        printf ("failed to create maze\n");
        return NULL;
    }

    p_maze->maze_cols = num_of_cols;
    p_maze->maze_rows = num_of_rows;
    p_maze->portals = NULL;
    p_maze->MSG = first_line_from_text_file;

    if (NULL == (p_maze->maze_tiles = malloc(num_of_rows * num_of_cols * sizeof(char)))) {
        printf ("failed to allocate memory for %d x %d tiles\n", num_of_rows, num_of_cols);
        delete_maze(p_maze);
        return NULL;
    }

    // read file line by line
    int curr_line = 0;
    int num_of_players = 0;
    int num_of_pushables = 0; 

    while(fgets(line_buffer, LINE_BUFFER_LEN, in_f)) {

        int temp_len_of_line = strlen(line_buffer);

        // trim newline chars from line_buffer
        for (int i=0; i < temp_len_of_line; i++) {
            if (line_buffer[i] == '\r' || line_buffer[i] == '\n' ) {
                line_buffer[i] = '\0';
            }
        }

        int len_of_line = strlen(line_buffer);

        if (len_of_line > num_of_cols) {
            printf("illegal maze, line %d too long\n", curr_line);
            delete_maze(p_maze);
            return NULL;
        }


        for (int i=0; i < len_of_line ; i++) {
            if (line_buffer[i] != '@'  &&
                line_buffer[i] != '#'  &&
                line_buffer[i] != '*'  &&     // Invisible wall
                line_buffer[i] != '!'  &&
                line_buffer[i] != '?'  &&
                line_buffer[i] != '<'  &&
                line_buffer[i] != '>'  &&
                line_buffer[i] != '^'  &&
                line_buffer[i] != 'v'  &&
                line_buffer[i] != '-'  &&
                line_buffer[i] != '|'  &&
                line_buffer[i] != '1'  &&      // Portals
                line_buffer[i] != '2'  &&
                line_buffer[i] != '3'  &&
                line_buffer[i] != '4'  &&
                line_buffer[i] != '5'  &&
                line_buffer[i] != '6'  &&
                line_buffer[i] != '7'  &&
                line_buffer[i] != '8'  &&
                line_buffer[i] != '9'  &&
                line_buffer[i] != '0'  &&
                line_buffer[i] != 'a'  &&      // Multi targets
                line_buffer[i] != 'b'  &&
                line_buffer[i] != 'c'  &&
                line_buffer[i] != '('  &&
                line_buffer[i] != ' '  ) {

                    printf("illegal char - %c found in maze\n", line_buffer[i]);
                    delete_maze(p_maze);
                    return NULL;
            }

            // The player is read
            if ('@' == line_buffer[i]) {
                p_maze->player_location.x = i;
                p_maze->player_location.y = curr_line;
                num_of_players ++;

            // Pushable is read
            } if ('(' == line_buffer[i]) {
                p_maze->pushable_location.x = i;
                p_maze->pushable_location.y = curr_line;
                num_of_pushables++;

            // A portal is read
            } else if ('0' <= line_buffer[i] && '9' >= line_buffer[i]) {
                portal_t *portal = create_portal(i, curr_line, line_buffer[i] - '0');
                if (NULL == portal) {
                    printf("Failed to allocate portal %c\n", line_buffer[i]);
                    delete_maze(p_maze);
                    return NULL;
                }
                array_of_portals[line_buffer[i] - '0'] = portal;
                number_of_portals++;
                p_maze->maze_tiles[curr_line*num_of_cols + i] = line_buffer[i];

            } else {
                p_maze->maze_tiles[curr_line*num_of_cols + i] = line_buffer[i];
            }
        }

        // handle case of too short input line
        if (len_of_line < num_of_cols) {
            for(int i = len_of_line; i < num_of_cols; i++) {
                p_maze->maze_tiles[curr_line*num_of_cols + i] = ' ';
            }
        }

        curr_line++;
    }

    if (1 != num_of_players) {
        printf("One player is required\n");
        delete_maze(p_maze);
        return NULL;
    }

    if (1 != num_of_pushables) {
        printf("One pushable wall is required\n");
        delete_maze(p_maze);
        return NULL;
    }

    //create a cyclic linked list to be the portals
    if (number_of_portals > 0) {
        for (int i = 0; i < number_of_portals - 1; i++) {
            array_of_portals[i]->next = array_of_portals[i+1];
        }
        array_of_portals[number_of_portals-1]->next = array_of_portals[0];
        p_maze->portals = array_of_portals[0];
    }

    return p_maze;
}

void print_boundries(int size_of_boundries, const char *color_of_boundries)
{
    for (int j = 0; j < size_of_boundries + 2; j++)
        puts_no_newline(color_of_boundries);
    putchar('\n');
}

void print_maze(maze_t *p_maze, int visited_targets[], const char *color_of_boundries)
{
    unsigned playeri = p_maze->player_location.y;
    unsigned playerj = p_maze->player_location.x;
    unsigned pushablei = p_maze->pushable_location.y;
    unsigned pushablej = p_maze->pushable_location.x;
    const char *message = p_maze->MSG;
    static unsigned frame_num; // this variable retains its value between calls

    // clear terminal
    puts_no_newline(CLEAR_SCREEN);

    // print frame number and message
    printf("%-*.*s%4d\n", p_maze->maze_cols-2, p_maze->maze_cols-2, message, frame_num++);

    // print header row
    print_boundries(p_maze->maze_cols, color_of_boundries);

    // print maze rows (including player)
    for (int i = 0; i < p_maze->maze_rows; i++) {
        puts_no_newline(color_of_boundries);
        for (int j = 0; j < p_maze->maze_cols; j++) {
            puts_no_newline( i == playeri && j == playerj &&
                            '?' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_BLUE_SECRET  :
                             i == playeri && j == playerj &&
                            '<' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_BLUE_LEFT    :
                             i == playeri && j == playerj &&
                            '>' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_BLUE_RIGHT   :
                             i == playeri && j == playerj &&
                            '^' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_BLUE_UP      :
                             i == playeri && j == playerj &&
                            'v' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_BLUE_DOWN    :
                             i == playeri && j == playerj &&
                            '-' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_BLUE_HWALL   :
                             i == playeri && j == playerj &&
                            '|' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_BLUE_VWALL   :

                            
                             i == playeri && j == playerj ? S_PLAYER                      :
                             i == pushablei && j == pushablej ? S_WALL                    : 
                            '#' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_WALL   :
                            '!' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_WALL   :
                            '?' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_WALL   :
                            '<' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_LEFT   :
                            '>' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_RIGHT  :
                            '^' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_UP     :
                            'v' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_DOWN   :
                            '-' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_HWALL  :
                            '|' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_VWALL  :
                            '*' == p_maze->maze_tiles[i*p_maze->maze_cols + j] ? S_FLOOR  :     // Invisible wall


                            'a' == p_maze->maze_tiles[i*p_maze->maze_cols + j] &&
                                   visited_targets[0] == 0 ? S_TARGET  :
                            'a' == p_maze->maze_tiles[i*p_maze->maze_cols + j] &&
                                   visited_targets[0] == 1 ? S_VISITED_TARGET  :
                            'b' == p_maze->maze_tiles[i*p_maze->maze_cols + j] &&
                                   visited_targets[1] == 0 ? S_TARGET  :
                            'b' == p_maze->maze_tiles[i*p_maze->maze_cols + j] &&
                                   visited_targets[1] == 1 ? S_VISITED_TARGET  :
                            'c' == p_maze->maze_tiles[i*p_maze->maze_cols + j] &&
                                   visited_targets[2] == 0 ? S_TARGET  :
                            'c' == p_maze->maze_tiles[i*p_maze->maze_cols + j] &&
                                   visited_targets[2] == 1 ? S_VISITED_TARGET  :

                            '0' <= p_maze->maze_tiles[i*p_maze->maze_cols + j] &&      // Portals
                            '9' >= p_maze->maze_tiles[i*p_maze->maze_cols + j]? S_GREEN_PORTAL  :
                            S_FLOOR);
        }

        puts_no_newline(color_of_boundries);
        putchar('\n');
    }

    // print footer row
    print_boundries(p_maze->maze_cols, color_of_boundries);
}

bool handle_input(maze_t *p_maze, bool var, int visited_targets[])
{
    var = false;
    int counter_of_visited_targets = 0;
    do {

        // get user input
        int key = getchar();

        switch(key) {

            case EOF:
            case 'q':
            case 'Q':

                // exit game
                var = true;
                p_maze->MSG = MSG_QUIT;
                break;

            case 'a':
            case 'A':

                // go left
                p_maze->player_location.x--;

                // WALLS AND ONE SIDED FLOORS
                if (-1 == p_maze->player_location.x) {
                   p_maze->MSG = MSG_WALL;
                   p_maze->player_location.x++;

                // Special case: Pushable wall   
                } else if (p_maze->player_location.x == p_maze->pushable_location.x &&
                           p_maze->player_location.y == p_maze->pushable_location.y ) {

                    // Wall is pushable only if he is not touched by another tile
                    if (   '#' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           '!' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           '?' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           '|' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           '-' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           '>' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           '<' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           '^' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           'v' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           '*' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           ('0' <= p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] && 
                           '9' >= p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols 
                            + p_maze->player_location.x - 1]) ||
                           'a' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           'b' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           'c' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                            0 == p_maze->pushable_location.x) {
                            
                            p_maze->MSG = MSG_WALL;
                            p_maze->player_location.x++;  
                        
                    } else {

                    p_maze->pushable_location.x--;
                    }

                } else if ('#' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '|' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '>' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '^' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           'v' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '*' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x]) {

                    p_maze->MSG = MSG_WALL;
                    p_maze->player_location.x++;

                // TRYING TO MOVE FROM SPECIAL WALL
                } else if ('|' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           '^' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           'v' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1]) {

                    p_maze->MSG = MSG_WALL;
                    p_maze->player_location.x++;

                } else {
                    p_maze->MSG = MSG_NORMAL;
                }

                break;

            case 'd':
            case 'D':

                // go right
                p_maze->player_location.x++;

                // WALLS AND ONE SIDED FLOORS
                if (p_maze->maze_cols == p_maze->player_location.x) {
                   p_maze->MSG = MSG_WALL;
                   p_maze->player_location.x--;
                
                // Special case: Pushable wall   
                } else if (p_maze->player_location.x == p_maze->pushable_location.x &&
                           p_maze->player_location.y == p_maze->pushable_location.y ) {

                    // Wall is pushable only if he is not touched by another tile
                    if (   '#' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           '!' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           '?' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           '|' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           '-' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           '>' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           '<' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           '^' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           'v' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           '*' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           ('0' <= p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] && 
                           '9' >= p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols 
                            + p_maze->player_location.x + 1]) ||
                           'a' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           'b' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                           'c' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x + 1] ||
                            (p_maze->maze_cols - 1) == p_maze->pushable_location.x) {
                            
                            p_maze->MSG = MSG_WALL;
                            p_maze->player_location.x--;  
                        
                    } else {

                        p_maze->pushable_location.x++;
                    }

                } else if ('#' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '|' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '<' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '^' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           'v' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '*' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x]) {

                    p_maze->MSG = MSG_WALL;
                    p_maze->player_location.x--;

                // TRYING TO MOVE FROM SPECIAL WALL
                } else if ('|' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           '^' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1] ||
                           'v' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x - 1]) {

                    p_maze->MSG = MSG_WALL;
                    p_maze->player_location.x--;

                } else {
                    p_maze->MSG = MSG_NORMAL;
                }
                break;

            case 'w':
            case 'W':

                // go up
                p_maze->player_location.y--;

                // WALLS AND ONE SIDED FLOORS
                if (-1 == p_maze->player_location.y) {
                   p_maze->MSG = MSG_WALL;
                   p_maze->player_location.y++;

                // Special case: Pushable wall   
                } else if (p_maze->player_location.x == p_maze->pushable_location.x &&
                           p_maze->player_location.y == p_maze->pushable_location.y ) {

                    // Wall is pushable only if he is not touched by another tile
                    if (    0 == p_maze->pushable_location.y ||
                           '#' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '!' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '?' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '|' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '-' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '>' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '<' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '^' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           'v' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '*' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           ('0' <= p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] && 
                           '9' >= p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols 
                            + p_maze->player_location.x]) ||
                           'a' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           'b' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           'c' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x]) {
                            
                            p_maze->MSG = MSG_WALL;
                            p_maze->player_location.y++;
                    
                    } else {
                        p_maze->pushable_location.y--;

                    }

                } else if ('#' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '-' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '<' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '>' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           'v' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '*' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x]) {

                    p_maze->MSG = MSG_WALL;
                    p_maze->player_location.y++;

                // TRYING TO MOVE FROM SPECIAL WALL
                } else if ('-' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '>' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '<' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x]) {

                    p_maze->MSG = MSG_WALL;
                    p_maze->player_location.y++;

                } else {
                    p_maze->MSG = MSG_NORMAL;
                }
                break;

            case 's':
            case 'S':

                // go down
                p_maze->player_location.y++;

                // WALLS AND ONE SIDED FLOORS
                if (p_maze->maze_rows == p_maze->player_location.y) {
                   p_maze->MSG = MSG_WALL;
                   p_maze->player_location.y--;
                
                // Special case: Pushable wall   
                } else if (p_maze->player_location.x == p_maze->pushable_location.x &&
                           p_maze->player_location.y == p_maze->pushable_location.y ) {

                    // Wall is pushable only if he is not touched by another tile
                    if (   '#' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '!' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '?' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '|' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '-' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '>' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '<' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '^' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           'v' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '*' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           ('0' <= p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] && 
                           '9' >= p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols 
                            + p_maze->player_location.x]) ||
                           'a' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           'b' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           'c' == p_maze->maze_tiles[(p_maze->player_location.y + 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                            (p_maze->maze_rows - 1) == p_maze->player_location.y) {
                            
                            p_maze->MSG = MSG_WALL;
                            p_maze->player_location.y--;  
                        
                    } else {
                    p_maze->pushable_location.y++;

                    }

                } else if ('#' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '-' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '<' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '^' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '>' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '*' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
                            + p_maze->player_location.x]) {

                    p_maze->MSG = MSG_WALL;
                    p_maze->player_location.y--;

                // TRYING TO MOVE FROM SPECIAL WALL
                } else if ('-' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '>' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x] ||
                           '<' == p_maze->maze_tiles[(p_maze->player_location.y - 1) * p_maze->maze_cols
                            + p_maze->player_location.x]) {

                    p_maze->MSG = MSG_WALL;
                    p_maze->player_location.y--;

                } else {
                    p_maze->MSG = MSG_NORMAL;
                }
                break;

            default:
                p_maze->MSG = MSG_UNKNOWN;
        }

        // PORTALS
        int player_tile_index = p_maze->player_location.y * p_maze->maze_cols + p_maze->player_location.x;

        if ('0' <= p_maze->maze_tiles[player_tile_index] && '9' >= p_maze->maze_tiles[player_tile_index]) {
            int portal_index = p_maze->maze_tiles[player_tile_index] - '0';
            portal_t *curr_portal = p_maze->portals;

            while (curr_portal->id != portal_index) {
                curr_portal = curr_portal->next;
            }

            p_maze->player_location.x = curr_portal->next->portal_location.x;
            p_maze->player_location.y = curr_portal->next->portal_location.y;

            p_maze->MSG = MSG_PORTAL;
        }


        // INSTANCES THAT END THE GAME

        // TARGETS
        if ('a' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
            + p_maze->player_location.x] && 0 == visited_targets[0]) {
                visited_targets[0] = 1;
                counter_of_visited_targets++;
        }

        if ('b' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
            + p_maze->player_location.x] && 0 == visited_targets[1]) {
                visited_targets[1] = 1;
                counter_of_visited_targets++;
        }

        if ('c' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
            + p_maze->player_location.x] && 0 == visited_targets[2]) {
                visited_targets[2] = 1;
                counter_of_visited_targets++;
        }

        if (3 == counter_of_visited_targets) {
            p_maze->MSG = MSG_WIN;
            print_maze(p_maze, visited_targets, S_WIN_WALL);
            var = true;
            break;
        }


        // TRAP
        if ('!' == p_maze->maze_tiles[p_maze->player_location.y * p_maze->maze_cols
            + p_maze->player_location.x]) {

            p_maze->MSG = MSG_TRAP;
            print_maze(p_maze, visited_targets, S_LOSE_WALL);
            var = true;
            break;
        }

        // print maze
        print_maze(p_maze, visited_targets, S_REG_WALL);

    } while (!var);

    return var;
}

int main(int argc, char *argv[])
{
    if (1 == argc) {
        printf("maze: missing file name after '%s'\n", argv[0]);
        exit(1);
    }

    if (2 < argc) {
        printf("maze: extra operand '%s' after file name\n", argv[2]);
        exit(1);
    }

    srand(time(0));
    set_terminal(true);

    maze_t *maze;

    if(NULL == (maze = read_maze(argv[1]))) {
        exit(1);
    }

    int visited_targets [NUM_OF_TARGETS] = {0};
    print_maze(maze, visited_targets, S_REG_WALL);

    bool game_over;

    game_over = handle_input(maze, false, visited_targets);
    if (game_over) {
        set_terminal(false);
    }

    delete_maze(maze);

    return 0;
}
