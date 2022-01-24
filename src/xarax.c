/*
================================================================================

    Xarax - a simple adventure game
    written by Sebastian Steinhauer <s.steinhauer@yahoo.de>


    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <https://unlicense.org>


================================================================================
*/
/*
================================================================================
================================================================================
*/
/*----------------------------------------------------------------------------*/
#include "SDL.h"


/*
================================================================================

        DEFINES

================================================================================
*/
/*----------------------------------------------------------------------------*/
#define SCREEN_FPS          10.0
#define SCREEN_FPS_TICKS    (1000.0 / SCREEN_FPS)

#define SCREEN_COLS         32
#define SCREEN_ROWS         18
#define SCREEN_SIZE         32      /* must be power of two */


/*----------------------------------------------------------------------------*/
enum {
    GAME_STATE_QUIT,
    GAME_STATE_PLAY,
    GAME_STATE_REST,
    GAME_STATE_REST2,
    GAME_STATE_SAIL,
    GAME_STATE_TAVERN,
    GAME_STATE_HEALER,
    GAME_STATE_SMITH,
    GAME_STATE_STORY,
    GAME_STATE_QUESTION
};


/*----------------------------------------------------------------------------*/
enum {
    BUTTON_UP               = 1,
    BUTTON_DOWN             = 2,
    BUTTON_LEFT             = 4,
    BUTTON_RIGHT            = 8,
    BUTTON_A                = 16,
    BUTTON_B                = 32
};


/*----------------------------------------------------------------------------*/
#define TILE_UI_BORDER_BOX  0x01
#define TILE_UI_BORDER_H    0x02
#define TILE_UI_BORDER_V    0x03
#define TILE_UI_ARROW_0     0x04
#define TILE_BUTTON_A       0x06
#define TILE_BUTTON_B       0x07
#define TILE_CLOCK_START    0x08

#define TILE_HEART          0x10
#define TILE_MONEY          0x11
#define TILE_KEY            0x12
#define TILE_TORCH          0x13
#define TILE_SWORD_0        0x14
#define TILE_ARMOR_0        0x18
#define TILE_POTION_A       0x1c
#define TILE_POTION_B       0x1e

#define TILE_HEART          0x10
#define TILE_MONEY          0x11
#define TILE_KEY            0x12
#define TILE_TORCH          0x13
#define TILE_SWORD_0        0x14
#define TILE_ARMOR_0        0x18

#define TILE_FLOOR_FIRST    0x80
#define TILE_FLOOR_LAST     0x8f

#define TILE_ANIMATED_FIRST 0xa0
#define TILE_ANIMATED_LAST  0xaf

#define TILE_MONSTER_FIRST  0xd0
#define TILE_MONSTER_LAST   0xdf

#define TILE_FIRE_PLACE     0xa0
#define TILE_DOOR_CLOSED    0xb0
#define TILE_DOOR_LOCKED    0xb1
#define TILE_DOOR_MAGIC     0xb2
#define TILE_DOOR_OPEN      0x8f
#define TILE_SIGN_POST      0xb3
#define TILE_STAIRS_DOWN    0xb4
#define TILE_STAIRS_UP      0xb5
#define TILE_CHEST_CLOSED   0xb6
#define TILE_CHEST_OPEN     0xb7
#define TILE_DOCK           0xb8
#define TILE_SHIP           0xb9
#define TILE_FLAG_OFF       0xba
#define TILE_FLAG_ON        0xbb

#define TILE_AVATAR_0       0xc0
#define TILE_AVATAR_1       0xc1
#define TILE_TAVERN_0       0xc2
#define TILE_TAVERN_1       0xc3
#define TILE_HEALER_0       0xc4
#define TILE_HEALER_1       0xc5
#define TILE_SMITH_0        0xc6
#define TILE_SMITH_1        0xc7
#define TILE_STORY_0        0xc8
#define TILE_STORY_1        0xc9
#define TILE_STORY_2        0xca
#define TILE_STORY_3        0xcb

#define TILE_SIGNAL_OFF     0xf0
#define TILE_SIGNAL_ON      0xf1
#define TILE_SIGNAL_Z       0xf2
#define TILE_SIGNAL_AND     0xf3
#define TILE_SIGNAL_OR      0xf4
#define TILE_SIGNAL_TILE    0xf5

#define TILE_HURT           0xef


/*----------------------------------------------------------------------------*/
#define NUM_OBJECTS         4096

typedef struct object_t {
    Uint16                  id;
    Uint8                   picture;
    Uint8                   x, y, z;
    Uint8                   spawn_x, spawn_y, spawn_z;
    Uint8                   life;
} object_t;

typedef struct avatar_t {
    object_t                *obj;
    Uint8                   money, keys, torch, time;
    Uint8                   sword, sword_life;
    Uint8                   armor, armor_life;
    Uint8                   potions[2];
    Sint8                   sail_x, sail_y;
    Uint16                  seed;
} avatar_t;


/*----------------------------------------------------------------------------*/
#define NUM_STRINGS         4096

typedef struct text_info_t {
    Uint8                   x, y, z;
    Uint16                  offset;
} text_info_t;


/*----------------------------------------------------------------------------*/
#define TORCH_LIGHT_RADIUS  6


/*
================================================================================

        STATIC DATA

================================================================================
*/
/*----------------------------------------------------------------------------*/
static const int            light_radius[256] = {
     3,  3,  4,  5,  6,  6,  7,  8,  9, 10, 10, 11, 12, 13, 13, 14,
    15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25,
    25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 30, 31, 31, 31, 32,
    32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
    34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 33, 33, 33, 33, 33, 32,
    32, 32, 31, 31, 31, 30, 30, 29, 29, 29, 28, 28, 27, 27, 26, 25,
    25, 24, 24, 23, 23, 22, 21, 21, 20, 19, 19, 18, 17, 17, 16, 15,
    14, 14, 13, 12, 11, 11, 10,  9,  8,  8,  7,  6,  5,  4,  4,  3,
     2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,
};


/*
================================================================================

        GLOBAL VARIABLES

================================================================================
*/
/*----------------------------------------------------------------------------*/
static int                  game_state = GAME_STATE_PLAY;
static int                  btn, btnp;


/*----------------------------------------------------------------------------*/
static SDL_Window           *window = NULL;
static SDL_Renderer         *renderer = NULL;
static SDL_Texture          *texture = NULL;
static Uint8                screen[SCREEN_SIZE][SCREEN_SIZE];
static int                  frame_animation = 0;


/*----------------------------------------------------------------------------*/
static Uint8                tilemap[2][256][256];
static Uint8                codemap[2][256][256];
static Uint16               objmap[2][256][256];
static object_t             objects[NUM_OBJECTS];
static avatar_t             avatar;
static Uint8                hurt_states[NUM_OBJECTS];


/*----------------------------------------------------------------------------*/
static char                 text_data[1 << 16];
static text_info_t          text_info[NUM_STRINGS];


/*----------------------------------------------------------------------------*/
static int                  question_states[2];
static char                 question_text[256];
static int                  question_lines;


/*----------------------------------------------------------------------------*/
static int                  story_x, story_y, story_z;
static int                  story_lines, story_page;
static const char           *story_text;


/*
================================================================================

        GENERAL FUNCTIONS

================================================================================
*/
/*----------------------------------------------------------------------------*/
static void panic(const char *fmt, ...) {
    va_list                 va;
    char                    message[1024];

    va_start(va, fmt);
    SDL_vsnprintf(message, sizeof(message), fmt, va);
    va_end(va);

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Panic!", message, window);
    exit(1);
}


/*----------------------------------------------------------------------------*/
static void clear_input() {
    btn = btnp = 0;
}


/*----------------------------------------------------------------------------*/
static void enter_state(int state) {
    clear_input();
    game_state = state;
}


/*----------------------------------------------------------------------------*/
static Uint16 rand16() {
    if (avatar.seed == 0) avatar.seed = 1;
    avatar.seed ^= avatar.seed << 7;
    avatar.seed ^= avatar.seed >> 9;
    avatar.seed ^= avatar.seed << 8;
    return avatar.seed;
}


/*----------------------------------------------------------------------------*/
static Uint8 dice6() {
    return 1 + (rand16() % 6);
}


/*----------------------------------------------------------------------------*/
static int count_lines(const char *str) {
    int                     count;
    for (count = 1; *str; ++str)
        if (*str == '\n') ++count;
    return count;
}


/*----------------------------------------------------------------------------*/
static void ask_question(int yes_state, int no_state, const char *fmt, ...) {
    va_list                 va;

    va_start(va, fmt);
    SDL_vsnprintf(question_text, sizeof(question_text), fmt, va);
    va_end(va);

    question_lines = count_lines(question_text);
    question_states[0] = yes_state;
    question_states[1] = no_state;

    enter_state(GAME_STATE_QUESTION);
}


/*----------------------------------------------------------------------------*/
static const char *find_text(const int x, const int y, const int z, int skip) {
    int                     i;
    for (i = 0; i < NUM_STRINGS; ++i) {
        if ((text_info[i].x == x) && (text_info[i].y == y) && (text_info[i].z == z)) {
            if (skip == 0)
                return &text_data[text_info[i].offset];
            else
                --skip;
        }
    }
    return NULL;
}


/*
================================================================================

        VIDEO FUNCTIONS

================================================================================
*/
/*----------------------------------------------------------------------------*/
static void render_screen() {
    unsigned int            x, y, id;
    SDL_Rect                src, dst;

    if (SDL_RenderClear(renderer))
        panic("SDL_RenderClear() failed: %s", SDL_GetError());
    
    src.w = src.h = dst.w = dst.h = 8;
    for (y = 0; y < SCREEN_ROWS; ++y) {
        dst.y = y * 8;
        for (x = 0; x < SCREEN_COLS; ++x) {
            dst.x = x * 8;
            id = screen[y][x];
            src.x = (id % 16) * 8; src.y = (id / 16) * 8;
            if (SDL_RenderCopy(renderer, texture, &src, &dst))
                panic("SDL_RenderCopy() failed: %s", SDL_GetError());
        }
    }
    
    SDL_RenderPresent(renderer);
}


/*----------------------------------------------------------------------------*/
static void clear_screen() {
    SDL_zero(screen);
}


/*----------------------------------------------------------------------------*/
static void draw_tile(unsigned int x, unsigned int y, unsigned int id) {
    screen[y % SCREEN_SIZE][x % SCREEN_SIZE] = (Uint8)id;
}


/*----------------------------------------------------------------------------*/
static void draw_text(int x, int y, const char *text) {
    int                     ox;

    for (ox = x - 1; *text; ++text, ++x) {
        if (*text == '\n')  { x = ox; ++y; }
        else                draw_tile(x, y, (Uint8)*text);
    }
}


/*----------------------------------------------------------------------------*/
static void draw_textf(int x, int y, const char *fmt, ...) {
    va_list                 va;
    char                    text[1024];

    va_start(va, fmt);
    SDL_vsnprintf(text, sizeof(text), fmt, va);
    va_end(va);
    draw_text(x, y, text);
}


/*----------------------------------------------------------------------------*/
static void draw_box(int xl, int yl, int w, int h) {
    int                     x, y, xh, yh;

    xh = xl + w + 1; yh = yl + h + 1;
    for (y = yl + 1; y <= yh - 1; ++y)
        for (x = xl + 1; x <= xh - 1; ++x)
            draw_tile(x, y, 0);
    for (x = xl + 1; x <= xh - 1; ++x) {
        draw_tile(x, yl, TILE_UI_BORDER_H);
        draw_tile(x, yh, TILE_UI_BORDER_H);
    }
    for (y = yl + 1; y <= yh - 1; ++y) {
        draw_tile(xl, y, TILE_UI_BORDER_V);
        draw_tile(xh, y, TILE_UI_BORDER_V);
    }
    draw_tile(xl, yl, TILE_UI_BORDER_BOX);
    draw_tile(xh, yl, TILE_UI_BORDER_BOX);
    draw_tile(xh, yh, TILE_UI_BORDER_BOX);
    draw_tile(xl, yh, TILE_UI_BORDER_BOX);
}


/*----------------------------------------------------------------------------*/
static void draw_map() {
    int                     x, y, ax, ay, ox, oy, ix, iy, id, sight;
    Uint8                   tx, ty, tz;
    const object_t          *obj;

    /* center view around avatar */
    ax = avatar.obj->x;
    ay = avatar.obj->y;
    tz = avatar.obj->z;
    ox = ax - (SCREEN_COLS / 2);
    oy = ay - (SCREEN_ROWS / 2);

    /* calc sight */
    sight = tz == 0 ? light_radius[avatar.time] : 1;
    if ((sight < TORCH_LIGHT_RADIUS) && (avatar.torch > 0))
        sight = TORCH_LIGHT_RADIUS;

    /* draw tiles */
    for (y = 0; y < SCREEN_ROWS - 1; ++y) {
        iy = y + oy; ty = iy;
        if (SDL_abs(iy - ay) > sight)
            continue;
        for (x = 0; x < SCREEN_COLS; ++x) {
            ix = x + ox; tx = ix;
            if (SDL_abs(ix - ax) > sight)
                continue;
            if ((id = objmap[tz][ty][tx]) > 0) {
                obj = &objects[id - 1];
                id = hurt_states[obj->id] > 0 ? TILE_HURT : obj->picture;
            } else {
                id = tilemap[tz][ty][tx];
                if ((id >= TILE_ANIMATED_FIRST) && (id <= TILE_ANIMATED_LAST))
                    id += frame_animation;
            }
            draw_tile(x, y + 1, id);
        }
    }
}


/*----------------------------------------------------------------------------*/
static void draw_hud() {
    draw_textf(0, 0, "%c%-3d %c%-3d %c%-3d %c %c %c   %c",
        TILE_HEART, avatar.obj->life,
        TILE_MONEY, avatar.money,
        TILE_KEY, avatar.keys,
        avatar.sword > 0 ? TILE_SWORD_0 + avatar.sword - 1 : ' ',
        avatar.armor > 0 ? TILE_ARMOR_0 + avatar.armor - 1 : ' ',
        avatar.torch > 0 ? TILE_TORCH : ' ',
        TILE_CLOCK_START + avatar.time / 32
    );
}


/*
================================================================================

        OBJECT FUNCTIONS

================================================================================
*/
/*----------------------------------------------------------------------------*/
static void remove_object(object_t *obj) {
    if (objmap[obj->z][obj->y][obj->x] == obj->id + 1)
        objmap[obj->z][obj->y][obj->x] = 0;
    obj->picture = 0;
}


/*----------------------------------------------------------------------------*/
static void move_object(object_t *obj, Uint8 x, Uint8 y, Uint8 z) {
    if (objmap[obj->z][obj->y][obj->x] == obj->id + 1)
        objmap[obj->z][obj->y][obj->x] = 0;
    obj->x = x; obj->y = y; obj->z = z % 2;
    objmap[obj->z][obj->y][obj->x] = obj->id + 1;
}


/*----------------------------------------------------------------------------*/
static void respawn_object(object_t *obj) {
    if ((obj->picture == 0) || (obj->life > 0))
        return;
    move_object(obj, obj->spawn_x, obj->spawn_y, obj->spawn_z);
    if ((obj->picture >= TILE_AVATAR_0) && (obj->picture <= TILE_AVATAR_1)) {
        obj->life = 15;
        /* preserve the keys!!! */
        avatar.sword = avatar.sword_life = 0;
        avatar.armor = avatar.armor_life = 0;
        avatar.torch = avatar.money = 0;
        avatar.obj = obj;
    } else if ((obj->picture >= TILE_MONSTER_FIRST) && (obj->picture <= TILE_MONSTER_LAST)) {
        obj->life = (obj->picture - TILE_MONSTER_FIRST + 1) * 2;
    }
}


/*----------------------------------------------------------------------------*/
static int spawn_object(Uint8 picture, Uint8 x, Uint8 y, Uint8 z) {
    int                     i;
    object_t                *obj;

    for (i = 0; i < NUM_OBJECTS; ++i) {
        obj = &objects[i];
        if (obj->picture == 0) {
            obj->id = (Uint16)i;
            obj->picture = picture;
            obj->spawn_x = x;
            obj->spawn_y = y;
            obj->spawn_z = z % 2;
            respawn_object(obj);
            return 1;
        }
    }
    return 0;
}


/*----------------------------------------------------------------------------*/
static void spawn_object_nearby(Uint8 picture, Uint8 x, Uint8 y, Uint8 z) {
    Uint8                   i, tx, ty;

    z %= 2;
    for (i = 0; i < 4; ++i) {
        for (ty = y - i; ty <= y + i; ++ty) {
            for (tx = x - i; tx <= x + i; ++tx) {
                if ((tilemap[z][ty][tx] < TILE_FLOOR_FIRST) || (tilemap[z][ty][tx] > TILE_FLOOR_LAST))
                    continue;
                if (objmap[z][ty][tx] > 0)
                    continue;
                if (spawn_object(picture, tx, ty, z))
                    return;
            }
        }
    }
}


/*----------------------------------------------------------------------------*/
static void hurt_object(object_t *obj, int damage) {
    if (damage < obj->life) {
        obj->life -= damage;
        hurt_states[obj->id] = SCREEN_FPS / 3;
    } else {
        obj->life = 0;
        objmap[obj->z][obj->y][obj->x] = 0;
    }
}


/*----------------------------------------------------------------------------*/
static void visit_power_tile(Uint8 x, Uint8 y, Uint8 z) {
    object_t                *obj;
    int                     id;

    switch (codemap[z][y][x]) {
        case TILE_SIGNAL_OFF:
            codemap[z][y][x] = TILE_SIGNAL_ON;
            visit_power_tile(x, y - 1, z);
            visit_power_tile(x + 1, y, z);
            visit_power_tile(x, y + 1, z);
            visit_power_tile(x - 1, y, z);            
            return;
        case TILE_SIGNAL_AND:
            if ((codemap[z][(y - 1) & 255][x] == TILE_SIGNAL_ON) && (codemap[z][(y + 1) & 255][x] == TILE_SIGNAL_ON))
                visit_power_tile(x + 1, y, z);
            return;
        case TILE_SIGNAL_OR:
            if ((codemap[z][(y - 1) & 255][x] == TILE_SIGNAL_ON) || (codemap[z][(y + 1) & 255][x] == TILE_SIGNAL_ON))
                visit_power_tile(x + 1, y, z);
            return;
        case TILE_SIGNAL_TILE:
            id = codemap[z][y][(x + 1) & 255];
            codemap[z][y][(x + 1) & 255] = tilemap[z][y][(x + 1) & 255];
            tilemap[z][y][(x + 1) & 255] = id;
            return;
    }

    if ((id = objmap[z][y][x]) > 0) {
        obj = &objects[id - 1];
        if (obj->picture == TILE_FLAG_OFF) {
            obj->picture = TILE_FLAG_ON;
        } else if (obj->picture == TILE_DOOR_MAGIC) {
            obj->picture = TILE_DOOR_CLOSED;
        }
    }
}


/*----------------------------------------------------------------------------*/
static void power_tile(Uint8 x, Uint8 y, Uint8 z) {
    visit_power_tile(x, y - 1, z);
    visit_power_tile(x + 1, y, z);
    visit_power_tile(x, y + 1, z);
    visit_power_tile(x - 1, y, z);
}


/*----------------------------------------------------------------------------*/
static int_fast8_t move_monster(object_t *obj, int dx, int dy) {
    Uint8                   new_x, new_y;
    int                     id, damage;
    object_t                *dst;

    new_x = obj->x + dx;
    new_y = obj->y + dy;

    if ((id = objmap[obj->z][new_y][new_x]) > 0) {
        dst = &objects[id - 1];
        if ((dst->picture >= TILE_AVATAR_0) && (dst->picture <= TILE_AVATAR_1)) {
            damage = (obj->picture - TILE_MONSTER_FIRST) * 2 + 1;
            if (avatar.armor > 0) {
                if ((damage -= avatar.armor * 2) < 1)
                    damage = 1;
                if (--avatar.armor_life == 0)
                    avatar.armor = 0;
            }
            hurt_object(dst, damage);
        }
        return 0;
    }
    id = tilemap[obj->z][new_y][new_x];
    if ((id < TILE_FLOOR_FIRST) || (id > TILE_FLOOR_LAST))
        return 0;
    
    move_object(obj, new_x, new_y, obj->z);
    return 1;
}


/*----------------------------------------------------------------------------*/
static void on_object_turn(object_t *obj) {
    if ((obj->picture >= TILE_MONSTER_FIRST) && (obj->picture <= TILE_MONSTER_LAST)) {
        int                 ax, ay;

        if ((obj->life == 0) || (obj->z != avatar.obj->z))
            return;

        ax = avatar.obj->x; ay = avatar.obj->y;
        if ((SDL_abs(obj->x - ax) > 8) || (SDL_abs(obj->y - ay) > 8))
            return;

        if (rand16()&1) {
            if      (obj->x < ax) move_monster(obj, 1, 0);
            else if (obj->x > ax) move_monster(obj, -1, 0);
        } else {
            if      (obj->y < ay) move_monster(obj, 0, 1);
            else if (obj->y > ay) move_monster(obj, 0, -1);
        }
    }
}


/*----------------------------------------------------------------------------*/
static void handle_all_objects() {
    int                     i;
    for (i = 0; i < NUM_OBJECTS; ++i)
        on_object_turn(&objects[i]);
}


/*----------------------------------------------------------------------------*/
static void move_avatar(int dx, int dy) {
    object_t                *obj, *dst;
    Uint8                   new_x, new_y, new_z;
    int                     id, sight;

    obj = avatar.obj;
    new_x = obj->x + dx;
    new_y = obj->y + dy;
    new_z = obj->z;

    if ((id = objmap[new_z][new_y][new_x]) > 0) {
        dst = &objects[id - 1];
        if ((dst->picture >= TILE_MONSTER_FIRST) && (dst->picture <= TILE_MONSTER_LAST)) {
            hurt_object(dst, avatar.sword * 2 + 1);
            if (avatar.sword_life > 0) {
                if (--avatar.sword_life == 0)
                    avatar.sword = 0;
            }
            if (dst->life == 0)
                spawn_object_nearby(TILE_MONEY, dst->x, dst->y, dst->z);
        } else if ((dst->picture == TILE_MONEY) && (avatar.money < 255)) {
            int     money = avatar.money + dice6() + dice6();
            if (money > 255) money = 255;
            avatar.money = money;
            remove_object(dst);
        } else if ((dst->picture == TILE_KEY) && (avatar.keys < 8)) {
            ++avatar.keys;
            remove_object(dst);
        } else if (dst->picture == TILE_FLAG_OFF) {
            power_tile(dst->x, dst->y, dst->z);
        } else if (dst->picture == TILE_CHEST_OPEN) {
            dst->picture = TILE_CHEST_CLOSED;
            spawn_object_nearby(TILE_MONEY, dst->x, dst->y, dst->z);
        } else if (dst->picture == TILE_DOOR_CLOSED) {
            objmap[dst->z][dst->y][dst->x] = 0;
            tilemap[dst->z][dst->y][dst->x] = TILE_DOOR_OPEN;
        } else if ((dst->picture == TILE_DOOR_LOCKED) && (avatar.keys > 0)) {
            obj->picture = TILE_DOOR_CLOSED;
            --avatar.keys;
        }
        return;
    }
    id = tilemap[new_z][new_y][new_x];
    if ((id < TILE_FLOOR_FIRST) || (id > TILE_FLOOR_LAST)) {
        switch (id) {
            case TILE_DOCK:
                avatar.sail_x = new_x - obj->x;
                avatar.sail_y = new_y - obj->y;
                ask_question(GAME_STATE_SAIL, GAME_STATE_PLAY, "Do you want to sail?");
                break;
            case TILE_FIRE_PLACE:
                ask_question(GAME_STATE_REST, GAME_STATE_PLAY, "A cosy fireplace.\nDo you want to rest?");
                return;
            case TILE_SIGN_POST:
            case TILE_STORY_0:
            case TILE_STORY_1:
            case TILE_STORY_2:
            case TILE_STORY_3:
                story_x = new_x; story_y = new_y; story_z = new_z;
                story_text = NULL; story_page = 0;
                enter_state(GAME_STATE_STORY);
                return;
            case TILE_HEALER_0:
            case TILE_HEALER_1:
                enter_state(GAME_STATE_HEALER);
                return;
            case TILE_SMITH_0:
            case TILE_SMITH_1:
                enter_state(GAME_STATE_SMITH);
                return;
            case TILE_TAVERN_0:
            case TILE_TAVERN_1:
                enter_state(GAME_STATE_TAVERN);
                return;
            default:
                return;
        }
    }

    move_object(obj, new_x, new_y, new_z);

    /* adjust torch life */
    sight = obj->z == 0 ? light_radius[avatar.time] : 1;
    if ((sight < TORCH_LIGHT_RADIUS) && (avatar.torch > 0))
        --avatar.torch;
}


/*----------------------------------------------------------------------------*/
static int on_avatar_turn() {
    if (btn & BUTTON_UP) {
        move_avatar(0, -1);
        return 1;
    } else if (btn & BUTTON_DOWN) {
        move_avatar(0, 1);
        return 1;
    } else if (btn & BUTTON_LEFT) {
        move_avatar(-1, 0);
        return 1;
    } else if (btn & BUTTON_RIGHT) {
        move_avatar(1, 0);
        return 1;
    }
    return 0;
}


/*
================================================================================

        GAME STATE FUNCTIONS

================================================================================
*/
/*----------------------------------------------------------------------------*/
static void on_nightfall() {
    int                     i;
    for (i = 0; i < NUM_OBJECTS; ++i)
        respawn_object(&objects[i]);
}


/*----------------------------------------------------------------------------*/
static void advance_time(int turns) {
    for (; turns > 0; --turns) {
        ++avatar.time;
        if (avatar.time == 0) {             /* jump out of resting... */
            enter_state(GAME_STATE_PLAY);
            return;
        } else if (avatar.time == 192) {    /* midnight */
            on_nightfall();
        }
    }
}


/*----------------------------------------------------------------------------*/
static void on_game_state_play() {
    int                     i;

    /* advance hurt states */
    for (i = 0; i < NUM_OBJECTS; ++i)
        if (hurt_states[i] > 0)
            --hurt_states[i];

    if (on_avatar_turn()) {
        handle_all_objects();
        advance_time(1);
    }

    clear_screen();
    draw_map();
    draw_hud();
}


/*----------------------------------------------------------------------------*/
static void on_game_state_rest(const int heal) {
    if (btnp & BUTTON_B) {
        enter_state(GAME_STATE_PLAY);
        return;
    }
    if ((heal) && (avatar.obj->life < 15))
        ++avatar.obj->life;
    handle_all_objects();
    advance_time(8);

    clear_screen();
    draw_map();
    draw_hud();

    draw_box(2, 2, SCREEN_COLS - 6, 1);
    draw_textf(3, 3, "Resting ... %c to cancel", TILE_BUTTON_B);
}


/*----------------------------------------------------------------------------*/
static void on_game_state_sail() {
    Uint8                   new_x, new_y, tmp;
    object_t                *obj = avatar.obj;

    new_x = obj->x + avatar.sail_x;
    new_y = obj->y + avatar.sail_y;
    move_object(obj, new_x, new_y, obj->z);
    advance_time(1);

    if (tilemap[obj->z][obj->y][obj->x] == TILE_DOCK)
        enter_state(GAME_STATE_PLAY);

    /* monkey patch the picture for drawing */
    tmp = obj->picture; obj->picture = TILE_SHIP;

    clear_screen();
    draw_map();
    draw_hud();

    obj->picture = tmp;
}


/*----------------------------------------------------------------------------*/
static void on_game_state_healer() {
    static int              value = 0;

    if (btnp & BUTTON_A) {
        avatar.money -= value * 3;
        avatar.obj->life += value * 2;
        enter_state(GAME_STATE_PLAY);
    } else if (btnp & BUTTON_B) {
        enter_state(GAME_STATE_PLAY);
    } else if (btn & (BUTTON_UP | BUTTON_LEFT)) {
        if (value > 0) --value;
    } else if (btn & (BUTTON_DOWN | BUTTON_RIGHT)) {
        ++value;
    }

    while ((avatar.obj->life + value * 2) > 255)    --value;
    while ((value * 3) > avatar.money)              --value;

    clear_screen();
    draw_map();
    draw_hud();

    draw_box(2, 2, SCREEN_COLS - 6, 5);
    draw_textf(3, 3,
        "Do you need some healing?\n\n"
        "%c %c%-3d for %c%-3d\n"
        "\n"
        "%c=Accept  %c=Deny",
        TILE_UI_ARROW_0 + frame_animation,
        TILE_HEART, value * 2,
        TILE_MONEY, value * 3,
        TILE_BUTTON_A, TILE_BUTTON_B
    );
}


/*----------------------------------------------------------------------------*/
static void on_game_state_smith() {
    static int              item = 0;
    int                     i, cost;

    if (btnp & BUTTON_A) {
        cost = ((item % 4) + 1) * 50;
        if (avatar.money >= cost) {
            if (item < 4) {
                i = item + 1;
                if (avatar.sword < i) {
                    avatar.money -= cost;
                    avatar.sword = i;
                    avatar.sword_life = 64;
                }
            } else {
                i = item - 4 + 1;
                if (avatar.armor < i) {
                    avatar.money -= cost;
                    avatar.armor = i;
                    avatar.armor_life = 64;
                }
            }
        }
    } else if (btnp & BUTTON_B) {
        enter_state(GAME_STATE_PLAY);
    } else if (btn & (BUTTON_UP | BUTTON_LEFT)) {
        if (item > 0) --item;
    } else if (btn & (BUTTON_DOWN | BUTTON_RIGHT)) {
        if (item < 7) ++item;
    }

    clear_screen();
    draw_map();
    draw_hud();

    draw_box(2, 2, SCREEN_COLS - 6, 12);
    draw_text(3, 3, "Finest weapons and armor!");
    for (i = 0; i < 4; ++i)
        draw_textf(5, 5 + i, "%c +%d damage for %c%-3d", TILE_SWORD_0 + i, (i + 1) * 2, TILE_MONEY, (i + 1) * 50);
    for (i = 0; i < 4; ++i)
        draw_textf(5, 9 + i, "%c +%d armor  for %c%-3d", TILE_ARMOR_0 + i, (i + 1) * 2, TILE_MONEY, (i + 1) * 50);
    draw_tile(3, 5 + item, TILE_UI_ARROW_0 + frame_animation);
    draw_textf(3, 14, "   %c=Buy   %c=Goodbye...", TILE_BUTTON_A, TILE_BUTTON_B);
}


/*----------------------------------------------------------------------------*/
static void on_game_state_tavern() {
    static int              item = 0;

    if (btnp & BUTTON_A) {
        switch (item) {
            case 0: /* rest */
                avatar.obj->spawn_x = avatar.obj->x;
                avatar.obj->spawn_y = avatar.obj->y;
                avatar.obj->spawn_z = avatar.obj->z;
                enter_state(GAME_STATE_REST2);
                break;
            case 1: /* torch */
                if ((avatar.money >= 15) && (avatar.torch < 255)) {
                    avatar.money -= 15;
                    avatar.torch = 255;
                }
                break;
            case 2: /* potion */
            case 3:
                if ((avatar.money >= 250) && (avatar.potions[item - 2] == 0)) {
                    avatar.money -= 250;
                    avatar.potions[item - 2] = 2;
                }
                break;
        }
    } else if (btnp & BUTTON_B) {
        enter_state(GAME_STATE_PLAY);
    } else if (btn & (BUTTON_UP | BUTTON_LEFT)) {
        if (item > 0) --item;
    } else if (btn & (BUTTON_DOWN | BUTTON_RIGHT)) {
        if (item < 3) ++item;
    }

    clear_screen();
    draw_map();
    draw_hud();

    draw_box(2, 2, SCREEN_COLS - 6, 8);
    draw_textf(3, 3,
        "Welcome to the tavern!\n"
        "\n"
        "  Rest here.\n"
        "  %c for %c%-3d\n"
        "  %c for %c%-3d\n"
        "  %c for %c%-3d\n"
        "\n"
        "   %c=Buy   %c=Goodbye...",
        TILE_TORCH, TILE_MONEY, 15,
        TILE_POTION_A, TILE_MONEY, 250,
        TILE_POTION_B, TILE_MONEY, 250,
        TILE_BUTTON_A, TILE_BUTTON_B
    );
    draw_tile(3, 5 + item, TILE_UI_ARROW_0 + frame_animation);
}


/*----------------------------------------------------------------------------*/
static void on_game_state_story() {
    if (btnp & BUTTON_A) {
        ++story_page; story_text = NULL;
    } else if (btnp & BUTTON_B) {
        enter_state(GAME_STATE_PLAY);
    }

    /* advance to next page if possible */
    if (story_text == NULL) {
        story_text = find_text(story_x, story_y, story_z, story_page);
        if (story_text == NULL) {
            enter_state(GAME_STATE_PLAY);
            return;
        }
        story_lines = count_lines(story_text);
    }
    
    clear_screen();
    draw_map();
    draw_hud();

    draw_box(0, 2, SCREEN_COLS - 2, story_lines + 2);
    draw_text(1, 3, story_text);
    draw_textf(SCREEN_COLS / 2 - 9, 3 + story_lines + 1, "%c=Continue   %c=Bye", TILE_BUTTON_A, TILE_BUTTON_B);
}


/*----------------------------------------------------------------------------*/
static void on_game_state_question() {
    if (btnp & BUTTON_A)
        enter_state(question_states[0]);
    else if (btnp & BUTTON_B)
        enter_state(question_states[1]);
    
    clear_screen();
    draw_map();
    draw_hud();

    draw_box(2, 2, SCREEN_COLS - 6, question_lines + 2);
    draw_text(3, 3, question_text);
    draw_textf(3, 3 + question_lines + 1, "%c=Yes %c=No", TILE_BUTTON_A, TILE_BUTTON_B);
}


/*----------------------------------------------------------------------------*/
static void on_tick() {
    switch (game_state) {
        case GAME_STATE_PLAY:       on_game_state_play(); break;
        case GAME_STATE_REST:       on_game_state_rest(0); break;
        case GAME_STATE_REST2:      on_game_state_rest(1); break;
        case GAME_STATE_SAIL:       on_game_state_sail(); break;
        case GAME_STATE_HEALER:     on_game_state_healer(); break;
        case GAME_STATE_SMITH:      on_game_state_smith(); break;
        case GAME_STATE_TAVERN:     on_game_state_tavern(); break;
        case GAME_STATE_STORY:      on_game_state_story(); break;
        case GAME_STATE_QUESTION:   on_game_state_question(); break;
    }
}


/*
================================================================================

        I/O FUNCTIONS

================================================================================
*/
/*----------------------------------------------------------------------------*/
static void load_world() {
    SDL_RWops               *rw;
    int                     x, y, z, i, id;

    /* reset all data */
    SDL_zero(tilemap);
    SDL_zero(codemap);
    SDL_zero(objects);
    SDL_zero(avatar);
    SDL_zero(objmap);
    SDL_zero(text_data);
    SDL_zero(text_info);

    /* read the maps */
    if ((rw = SDL_RWFromFile("world.dat", "rb")) == NULL)
        panic("SDL_RWFromFile() failed: %s", SDL_GetError());
    SDL_RWread(rw, tilemap, sizeof(tilemap), 1);
    SDL_RWread(rw, codemap, sizeof(codemap), 1);
    
    /* read the strings */
    SDL_RWread(rw, text_data, sizeof(text_data), 1);
    for (i = 0; i < NUM_STRINGS; ++i) {
        text_info[i].x = SDL_ReadU8(rw);
        text_info[i].y = SDL_ReadU8(rw);
        text_info[i].z = SDL_ReadU8(rw);
        text_info[i].offset = SDL_ReadLE16(rw);
    }

    SDL_RWclose(rw);

    /* spawn objects */
    for (z = 0; z < 2; ++z) {
        for (y = 0; y < 256; ++y) {
            for (x = 0; x < 256; ++x) {
                id = codemap[z][y][x];
                switch (id) {
                    case TILE_SIGNAL_TILE:
                        ++x;
                        break;
                    case TILE_AVATAR_0: case TILE_AVATAR_1:
                    case TILE_DOOR_CLOSED: case TILE_DOOR_LOCKED: case TILE_DOOR_MAGIC:
                    case TILE_CHEST_CLOSED: case TILE_FLAG_OFF:
                        spawn_object(id, x, y, z);
                        break;
                    default:
                        if ((id >= TILE_MONSTER_FIRST) && (id <= TILE_MONSTER_LAST))
                            spawn_object(id, x, y, z);
                        break;
                }
            }
        }
    }

    if (avatar.obj == NULL)
        panic("World has no avatar!");
}


/*
================================================================================

        EVENT LOOP

================================================================================
*/
/*----------------------------------------------------------------------------*/
static void mark_button(const int mask, const int down) {
    if (down)   { btn |= mask; btnp |= mask; }
    else        { btn &= ~mask;              }
}


/*----------------------------------------------------------------------------*/
static void handle_key_code(const SDL_KeyCode key, const int down) {
    int                     mask;

    if (down) {
        switch (key) {
            case SDLK_F9:   load_world(); break;
            default:        break;
        }
    }

    switch (key) {
        case SDLK_UP: case SDLK_w:              mask = BUTTON_UP; break;
        case SDLK_DOWN: case SDLK_s:            mask = BUTTON_DOWN; break;
        case SDLK_LEFT: case SDLK_a:            mask = BUTTON_LEFT; break;
        case SDLK_RIGHT: case SDLK_d:           mask = BUTTON_RIGHT; break;
        case SDLK_RETURN: case SDLK_k:          mask = BUTTON_A; break;
        case SDLK_SPACE: case SDLK_ESCAPE: case SDLK_l:           mask = BUTTON_B; break;
        default:                                mask = 0; break;
    }
    mark_button(mask, down);
}


/*----------------------------------------------------------------------------*/
static void handle_SDL_events() {
    SDL_Event               ev;

    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
            case SDL_QUIT:      game_state = GAME_STATE_QUIT; break;
            case SDL_KEYDOWN:   handle_key_code(ev.key.keysym.sym, 1); break;
            case SDL_KEYUP:     handle_key_code(ev.key.keysym.sym, 0); break;
        }
    }
}


/*----------------------------------------------------------------------------*/
static void run_event_loop() {
    Uint32                  last_tick, current_tick, frame_counter = 0;
    double                  delta_ticks = 0.0;

    clear_screen();
    clear_input();

    last_tick = SDL_GetTicks();
    while (game_state != GAME_STATE_QUIT) {
        handle_SDL_events();

        current_tick = SDL_GetTicks();
        delta_ticks += current_tick - last_tick;
        last_tick = current_tick;

        for (; delta_ticks >= SCREEN_FPS_TICKS; delta_ticks -= SCREEN_FPS_TICKS) {
            ++frame_counter;
            frame_animation = (frame_counter >> 2) & 1;
            on_tick();
            btnp = 0;
        }

        render_screen();
    }
}


/*
================================================================================

        INIT / SHUTDOWN

================================================================================
*/
/*----------------------------------------------------------------------------*/
static void shutdown_game() {
    if (texture != NULL)
        SDL_DestroyTexture(texture);
    if (renderer != NULL)
        SDL_DestroyRenderer(renderer);
    if (window != NULL)
        SDL_DestroyWindow(window);
    SDL_Quit();
}


/*----------------------------------------------------------------------------*/
static void initialize_game() {
    int                     w, h;
    SDL_DisplayMode         dm;
    SDL_Surface             *bmp;

    atexit(shutdown_game);
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER))
        panic("SDL_Init() failed: %s", SDL_GetError());

    /* determine best window resolution */
    w = SCREEN_COLS * 8; h = SCREEN_ROWS * 8;
    if (SDL_GetDesktopDisplayMode(0, &dm) == 0) {
        dm.w *= 0.8f; dm.h *= 0.8f;
        while ((w < dm.w) && (h < dm.h)) { w *= 2; h *= 2; }
        while ((w > dm.w) || (h > dm.h)) { w /= 2; h /= 2; }
    }

    /* init video system */
    if ((window = SDL_CreateWindow("Xarax", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE)) == NULL)
        panic("SDL_CreateWindow() failed: %s", SDL_GetError());
    if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL)
        panic("SDL_CreateRenderer() failed: %s", SDL_GetError());
    if (SDL_RenderSetLogicalSize(renderer, SCREEN_COLS * 8, SCREEN_ROWS * 8))
        panic("SDL_RenderSetLogicalSize() failed: %s", SDL_GetError());
    if ((bmp = SDL_LoadBMP("./dev/tiles.bmp")) == NULL)   
        panic("SDL_LoadBMP() failed: %s", SDL_GetError());
    texture = SDL_CreateTextureFromSurface(renderer, bmp);
    SDL_FreeSurface(bmp);
    if (texture == NULL)
        panic("SDL_CreateTextureFromSurface() failed: %s", SDL_GetError());

    /* init audio system */
    // TODO

    /* load resources */
    load_world();
}


/*----------------------------------------------------------------------------*/
int main(int argc, char **argv) {
    (void)argc; (void)argv;
    initialize_game();
    run_event_loop();
    return 0;
}


/*
================================================================================
================================================================================
*/
/*----------------------------------------------------------------------------*/
