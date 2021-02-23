#include <stdbool.h>

#define MSG_NORMAL  "There is only one lord of the ring"
#define MSG_START   "Welcome! " MSG_NORMAL
#define MSG_UNKNOWN msg_unknown[rand() % sizeof(msg_unknown) / sizeof(msg_unknown[0])]
#define MSG_QUIT msg_quit[rand() % sizeof(msg_quit) / sizeof(msg_quit[0])]
#define MSG_WALL msg_wall[rand() % sizeof(msg_wall) / sizeof(msg_wall[0])]
#define MSG_WIN msg_win[rand() % sizeof(msg_win) / sizeof(msg_win[0])]
#define MSG_PORTAL msg_portal[rand() % sizeof(msg_portal) / sizeof(msg_portal[0])]
#define MSG_TRAP "SURPRISE MADAFAKA!"

typedef struct {
    unsigned x;
    unsigned y;
} location_t;

typedef struct portal {
    int id;
    location_t portal_location;
    struct portal *next;
} portal_t;

typedef struct {
    portal_t *portals;
    char *maze_tiles;
    int maze_cols;
    int maze_rows;
    location_t player_location;
    location_t pushable_location;
    const char *MSG;
} maze_t;

void set_terminal(bool start);
void print_maze(maze_t *maze, int visited_targets[], const char *color_of_boundries);

/************************************
 * TERMINAL CODES ("MAGIC STRINGS") *
 ************************************/

#define COLOR_OFF       "\e[0m"
#define COLOR_BLACK     "\e[0;30m"
#define COLOR_GRAY      "\e[1;30m"
#define COLOR_RED       "\e[0;31m"
#define COLOR_LT_RED    "\e[1;31m"
#define COLOR_GREEN     "\e[0;32m"
#define COLOR_LT_GREEN  "\e[1;32m"
#define COLOR_BROWN     "\e[0;33m"
#define COLOR_YELLOW    "\e[1;33m"
#define COLOR_BLUE      "\e[0;34m"
#define COLOR_LT_BLUE   "\e[1;34m"
#define COLOR_PURPLE    "\e[0;35m"
#define COLOR_LT_PURPLE "\e[1;35m"
#define COLOR_CYAN      "\e[0;36m"
#define COLOR_LT_CYAN   "\e[1;36m"
#define COLOR_LT_GRAY   "\e[0;37m"
#define COLOR_WHITE     "\e[1;37m"

#define CLEAR_SCREEN    "\e[2J\e[1;1H"

/*****************************
 * SYMBOLS FOR GAME ELEMENTS *
 *****************************/

#define S_FLOOR     " "
#define S_INVISIBLE "*"
#define S_PLAYER    COLOR_BLUE   "@"   COLOR_OFF
#define S_WALL      COLOR_GRAY   "#"   COLOR_OFF
#define S_TARGET    COLOR_YELLOW "$"   COLOR_OFF
#define S_TRAP      COLOR_GRAY   "!"   COLOR_OFF
#define S_SECRET    COLOR_GRAY   "?"   COLOR_OFF
#define S_LEFT      COLOR_GRAY   "<"   COLOR_OFF
#define S_RIGHT     COLOR_GRAY   ">"   COLOR_OFF
#define S_UP        COLOR_GRAY   "^"   COLOR_OFF
#define S_DOWN      COLOR_GRAY   "v"   COLOR_OFF
#define S_HWALL     COLOR_GRAY   "-"   COLOR_OFF
#define S_VWALL     COLOR_GRAY   "|"   COLOR_OFF
#define S_LOSE_WALL COLOR_RED    "#"   COLOR_OFF
#define S_REG_WALL  COLOR_CYAN   "#"   COLOR_OFF
#define S_WIN_WALL  COLOR_GREEN  "#"   COLOR_OFF
// #define S_PUSHABLE  COLOR_BLUE   "("   COLOR_OFF


#define S_BLUE_SECRET    COLOR_BLUE    "#"   COLOR_OFF
#define S_BLUE_LEFT      COLOR_BLUE    "<"   COLOR_OFF
#define S_BLUE_RIGHT     COLOR_BLUE    ">"   COLOR_OFF
#define S_BLUE_UP        COLOR_BLUE    "^"   COLOR_OFF
#define S_BLUE_DOWN      COLOR_BLUE    "v"   COLOR_OFF
#define S_BLUE_HWALL     COLOR_BLUE    "-"   COLOR_OFF
#define S_BLUE_VWALL     COLOR_BLUE    "|"   COLOR_OFF
#define S_VISITED_TARGET COLOR_LT_BLUE "$"   COLOR_OFF

#define S_GREEN_PORTAL   COLOR_LT_GREEN   "O"  COLOR_OFF
