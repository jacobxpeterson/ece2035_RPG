#include "map.h"

#include "globals.h"
#include "graphics.h"


/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 * In this file you need to define how the map will be structured. IE how will
 * you put values into the map, pull them from the map. Remember a "Map" variable
 * is a hashtable plus two ints (see below) 
 * You will have more than one map variable, one will be the main map with it's own hashtable.
 * Then you'll have a second map with another hashtable
 * You should store objects into the hashtable with different properties (spells
 * etc)
 */
struct Map {
    HashTable* items;
    int w, h;
};

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map maps[2];
static int active_map;

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned int XY_KEY(int X, int Y) {
    // Cantor pairing function
    unsigned int key;
    key = (X + Y) * (X+Y+1)/2 + X;
    return key;
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned int map_hash(unsigned key)
{
    return key % 5;
}

void maps_init()
{
    maps[0].items = createHashTable(map_hash, 5);
    maps[0].w = 50;
    maps[0].h = 50;
    
    maps[1].items = createHashTable(map_hash, 5);
    maps[1].w = 15;
    maps[1].h = 20;
    
}

Map* get_active_map()
{
  Map* m = &maps[active_map];
  return m;
        
}

Map* set_active_map(int m)
{
    active_map = m;
    Map* mp = &maps[m];
    return mp;
}

Map* get_map(int m) {
    Map* mp = &maps[m];
    return mp;
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P', 'N', 'C', 'S', '1', '2', '3', '4'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}

int map_width()
{
    return maps[active_map].w;
}

int map_height()
{
    return maps[active_map].h;
}

int map_area()
{
    int w = maps[active_map].w;
    int h = maps[active_map].h;
    return w*h;
}

MapItem* get_north(int x, int y)
{
    return get_here(x, y-1);
}

MapItem* get_south(int x, int y)
{
    return get_here(x, y+1);
}

MapItem* get_east(int x, int y)
{
    return get_here(x+1, y);
}

MapItem* get_west(int x, int y)
{
    return get_here(x-1, y);
}

MapItem* get_here(int x, int y)
{
    unsigned int key = XY_KEY(x,y);
    MapItem* m = (MapItem*) getItem(get_active_map()->items, key);
    return m;
}


void map_erase(int x, int y)
{
    unsigned int key = XY_KEY(x,y);
    MapItem* m = (MapItem*) removeItem(get_active_map()->items, key);
}

void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_plant(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PLANT;
    w1->draw = draw_plant;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_cave(int x, int y)
{
    MapItem* m1 = (MapItem*) malloc(sizeof(MapItem));
    m1->type = CAVE;
    m1->draw = draw_cave;
    m1->walkable = true;
    m1->data = (int*)1;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), m1);
    if (val) free(val);
}

void add_caveShell(int x, int y)
{
    MapItem* m1 = (MapItem*) malloc(sizeof(MapItem));
    m1->type = CAVESHELL;
    m1->draw = draw_caveShell;
    m1->walkable = false;
    m1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), m1);
    if (val) free(val);
}

void add_npc(int x, int y, char* name)
{
    MapItem* m1 = (MapItem*) malloc(sizeof(MapItem));
    m1->type = NPC;
    m1->draw = NULL;
    m1->walkable = false;
    m1->data = (void*) name;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), m1);
    if (val) free(val);
}

void add_house1(int x, int y)
{
    MapItem* m1 = (MapItem*) malloc(sizeof(MapItem));
    m1->type = H1;
    m1->draw = draw_house1;
    m1->walkable = false;
    m1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), m1);
    if (val) free(val);
}
void add_house2(int x, int y)
{
    MapItem* m1 = (MapItem*) malloc(sizeof(MapItem));
    m1->type = H2;
    m1->draw = draw_house2;
    m1->walkable = false;
    m1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), m1);
    if (val) free(val);
}
void add_house3(int x, int y)
{
    MapItem* m1 = (MapItem*) malloc(sizeof(MapItem));
    m1->type = H3;
    m1->draw = draw_house3;
    m1->walkable = false;
    m1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), m1);
    if (val) free(val);
}
void add_house4(int x, int y)
{
    MapItem* m1 = (MapItem*) malloc(sizeof(MapItem));
    m1->type = H4;
    m1->draw = draw_house4;
    m1->walkable = false;
    m1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), m1);
    if (val) free(val);
}

void add_light(int x, int y) 
{
    MapItem *m1 = (MapItem*)malloc(sizeof(MapItem));
    m1->type = LIGHT;
    m1->draw = draw_light;
    m1->walkable = false;
    m1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), m1);
    if (val) free(val);
}

void add_tree1(int x, int y)
{
    MapItem* m1 = (MapItem*)malloc(sizeof(MapItem));
    m1->type = T1;
    m1->draw = draw_tree1;
    m1->walkable = true;
    m1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), m1);
    if (val) free(val);
}
void add_tree2(int x, int y)
{
    MapItem* m1 = (MapItem*)malloc(sizeof(MapItem));
    m1->type = T2;
    m1->draw = draw_tree2;
    m1->walkable = true;
    m1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), m1);
    if (val) free(val);
}
void add_tree3(int x, int y)
{
    MapItem* m1 = (MapItem*)malloc(sizeof(MapItem));
    m1->type = T3;
    m1->draw = draw_tree3;
    m1->walkable = true;
    m1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), m1);
    if (val) free(val);
}
void add_tree4(int x, int y)
{
    MapItem* m1 = (MapItem*)malloc(sizeof(MapItem));
    m1->type = T4;
    m1->draw = draw_tree4;
    m1->walkable = true;
    m1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), m1);
    if (val) free(val);
}