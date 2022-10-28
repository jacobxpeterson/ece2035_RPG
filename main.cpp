// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"

// Functions in this file
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map ();
int main ();
void go_up();
void go_left();
void go_right();
void go_down();
MapItem* what_to_do();
void fix_speechDraw();
void cave_fun();
void init_map2();
void npc_fun(MapItem* m);
void light_fun();
int house_fun();
/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_key;
    int talk_count;
    int omni;
    int dogFollow;
    int health;
    // You can add other properties for the player here
} Player;

FILE *wave_file;

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
int get_action(GameInputs inputs)
{   
    //action button has priority
    if(!inputs.b1) {
        return ACTION_BUTTON;
    }
    if(!inputs.b2) {
        return MENU_BUTTON;
    }
    //pc.printf("x: %f, y: %f\r\n", inputs.ax, inputs.ay);
    //wait(5);
    if ( (fabs(inputs.ax) > 0.2) || (fabs(inputs.ay) > 0.2)) {
        if( fabs(inputs.ax) > fabs(inputs.ay) ) {
            int r = (inputs.ax < 0) ?  GO_RIGHT: GO_LEFT;
            return r;
        }else {
            int r = (inputs.ay < 0) ?  GO_UP: GO_DOWN;
            return r;
        }
    }
    
    return NO_ACTION;
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;
    

    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        case GO_UP: 
        { 
            go_up();   
            break;
        }
        case GO_LEFT:
        {
            go_left();
            break;     
        }       
        case GO_DOWN:
        {
            go_down();
            break;
        }
        case GO_RIGHT:
        {
            go_right();
            break;
        }
        case ACTION_BUTTON:
        {
            wave_file=fopen("/sd/cs.wav","r");
            waver.play(wave_file);
            fclose(wave_file); 
            
            MapItem* m = what_to_do();
            int p = m->type;
            if (p == CAVE) cave_fun();
            if (p == NPC) npc_fun(m);
            if (p == LIGHT) light_fun();
            if (p == H1 || p == H2 || p == H3 || p == H4)
            {
                int r = house_fun();
                return r;
            }
            break;
        }
        case MENU_BUTTON: 
        {
            Player.omni = 1;
            break;
        }
        default:        break;
    }
    return NO_RESULT;
}

/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    //pc.printf("Beg. draw: x-%d, y-%d\r\n", Player.x, Player.y);
    // Draw game border first
    if(init) draw_border();
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            //Big Tree check
            int behind = 1;
            MapItem* curr = get_here(Player.x, Player.y);
            if (curr != NULL && curr->type > 9) {
                behind = 0;
            }
            
            //npc check
            int nCheck = 0;
            char* name;
            
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (i == 0 && j == 0 && behind) // Only draw the player at i=0 j= 0
            {
                draw_player(u, v, Player.has_key);
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        if(Player.omni) curr_item->walkable = true;
                        draw = curr_item->draw;
                        if (curr_item->type == NPC) {
                            nCheck = 1;
                            name = (char*)curr_item->data;
                        }
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            //to differentiate between npc sprites
            if (nCheck) {
                draw_npc(u, v, name);
            }
            else if (draw) draw(u, v);
        }
    }

    //pc.printf("x-%d, y-%d\r\n", Player.x, Player.y);
    // Draw status bars    
    draw_upper_status(Player.x, Player.y);
    
    draw_lower_status(Player.omni, Player.health);
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */
void init_main_map()
{
    // "Random" plants
    Map* map = set_active_map(0);
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_plant(i % map_width(), i / map_width());
    }
    pc.printf("plants\r\n");
    
    //WALLS
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");

    //CAVE
    for(int k = 36;  k < 41; k++) {
        add_caveShell(k, 16);
    }
    for(int x = 35; x < 42; x++) {
        for (int y = 17; y < 19; y++) {
            add_caveShell(x, y);
        }
    }
    for(int x = 36; x < 41; x++) {
        for (int y = 17; y < 19; y++) {
            add_cave(x,y);
        }
    }
    //add big tree
    add_tree1(20, 15);
    add_tree2(21, 15);
    add_tree3(20, 16);
    add_tree4(21, 16);
    //Mom NPC
    add_npc(15, 15, "Mom");
    
    //house
    add_house1(10, 15);
    add_house2(11,15);
    add_house3(10,16);
    add_house4(11, 16);
    print_map();
}

//initializes the cave map
void init_map2() 
{
    Map* map = set_active_map(1);
    
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");
    
    add_npc(7, 10, "Dog");
    add_light(6, 17);
    add_light(6, 18);
    add_npc(6, 8, "Alien");
}



/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    //uLCD.filled_rectangle(64, 64, 74, 74, RED); //DELETE OR COMMENT THIS LINE
    
    // Initialize the maps
    maps_init();
    init_main_map();
    init_map2();
    
    // Initialize game state
    set_active_map(0);
    Player.x = Player.y = 10;
    Player.has_key = false;
    Player.talk_count = 0;
    Player.omni = 0;
    Player.dogFollow = 0;
    Player.health = 15;
    
    
    //start screen
    start_menu();
    uLCD.cls();
    
    //continue?
    int cnt = 0;

    // Initial drawing
    draw_game(true);
    // Main game loop
    //pc.printf("Init:%d\r\n", Player.y);
    while(!cnt)
    {
        // Timer to measure game update speed
        Timer t; t.start();
        
        GameInputs in = read_inputs();
        int action = get_action(in);
        cnt = update_game(action);
        
        draw_game(false);
        
        //player dead?
        if (Player.health <= 0) {
            cnt = 1;
        }
        
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
    uLCD.cls();
    uLCD.locate(0,8);
    uLCD.printf("GAME OVER");
    if (Player.health <= 0) {
        uLCD.locate(0,12);
        uLCD.printf("Restart to try \n\ragain");
    }
}

void go_up() 
{
    MapItem* mi = get_north(Player.x, Player.y);
    if(mi->walkable) {
       // pc.printf("go_up:%d\r\n",Player.y);
        Player.y -=1;
        //pc.printf("go_up2:%d\r\n",Player.y);
    }
}

void go_left()
{
    MapItem* mi = get_west(Player.x, Player.y);
    if(mi->walkable) {
        //pc.printf("go_left:%d\r\n",Player.y);
        Player.x -= 1;
        //pc.printf("go_left2:%d\r\n",Player.y);
    }
}

void go_down()
{
    MapItem* mi = get_south(Player.x, Player.y);
    if(mi->walkable) {
        //pc.printf("go_down:%d\r\n",Player.y);
        Player.y += 1;
        //pc.printf("go_down2:%d\r\n",Player.y);
    }
}

void go_right()
{
    MapItem* mi = get_east(Player.x, Player.y);
    if(mi->walkable) {
        //pc.printf("go_right:%d\r\n",Player.y);
        Player.x +=1;
        //pc.printf("go_right2:%d\r\n",Player.y);
    }
}    

//determine if Player is 
//pressing the action button on
//a valid map item
MapItem* what_to_do() 
{
    //above
    MapItem* m = get_north(Player.x, Player.y);
    if (m) { 
        
        if(m->type != PLANT && m->type != CAVESHELL) return m; 
    }
    
    //right
    m = get_east(Player.x, Player.y);
    if (m) {
        if(m->type != PLANT && m->type != CAVESHELL) return m;
    }
    
    //below
    m = get_south(Player.x, Player.y);
    if (m) {
      if(m->type != PLANT && m->type != CAVESHELL) return m;
    }
    
    //left
    m = get_west(Player.x, Player.y);
    if (m) {
       if(m->type != PLANT && m->type != CAVESHELL) return m;
    }
    //default
    return NULL;
}

//restores the screen after text bubble disappears
void fix_speechDraw() 
{
    //iterate over tiles the speech bubble covers
    for (int i = -5; i <=5; i++) {
        for (int j = 3; j <= 4; j++) {
            //location on map
            int x = i + Player.x;
            int y = j + Player.y;
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) {
                //map item at location
                MapItem* curr_item = get_here(x, y);
                if(curr_item != NULL) {
                    curr_item->draw(u, v);
                }
            } else {
                draw_wall(u, v);
            }
        }
    }
    wait(0.3);
}

//either presents a message
//or transfers to second map
void cave_fun() 
{
    if (Player.has_key == true) {
        char* l1 = "(No reason to go";
        char* l2 = "back in there)";
        speech(l1, l2);
        fix_speechDraw();
    } 
    else if (Player.talk_count == 0) {
        const char* l[4] = {"(I should prob-", "ably talk to ", "Mom before ven-", "turing off.)"};
        long_speech(l, 4);
        fix_speechDraw();
    } 
    else if (Player.talk_count == 1) {
        uLCD.cls();
        set_active_map(1);
        Player.x = 6;
        Player.y = 10;
        draw_game(true);
    }
    
}

//determines what npc the player is talking to and acts
// accordingly
void npc_fun(MapItem* m)
{
    char* name = (char*) m->data;
    if (strcmp(name,"Mom") == 0) 
    {
        int i = Player.talk_count;
        switch(i)
        {
            case 0:
            {
                const char* l[6] = {"A meterorite ", "landed in the ", "cave East of", "here! The dog", "ran after it! ", "Bring him home!"};
                long_speech(l, 6);
                fix_speechDraw();
                Player.talk_count++;
                break;
            }
            case 1:
            {
                char* l1 = "Get the dog ";
                char* l2 = "back home! ";
                speech(l1, l2);
                fix_speechDraw();
                break;
            }
            case 2:
            {
                const char* l[5] = {"Thanks for ", "bringing him ", "home, I'm glad ", "you're safe. ", "Let's go to bed."};
                long_speech(l,5);
                fix_speechDraw();
                Player.talk_count++;
                break;
            }
            case 3:
            {
                char* l1 = "Time to go  ";
                char* l2 = "to bed. ";
                speech(l1, l2);
                fix_speechDraw();
                break;
            }
        }
    }
    if (strcmp(name,"Dog") == 0) 
    {
        if (Player.has_key == false) {
            const char* l[8] = {"Bark! Bark!", "(I've been kid-", "napped by this", "Alien. You have", "to save me!", "He's weak to", "BLUNT force", "attacks.)"};
            long_speech(l, 8);
            fix_speechDraw();
        } else {
            const char* l[4] = {"Bark! Bark!", "(Nice! I'll", "see you at", "home.)"};
            long_speech(l, 4);
            fix_speechDraw();
            Player.dogFollow = 1;
            map_erase(7, 10);
            draw_game(true);
        }
    } 
    if (strcmp(name, "Alien") == 0)
    {
        int success = combat_menu();
        if (success) {
            fix_speechDraw();
            Player.has_key = 1;
            Player.talk_count++;
            map_erase(6, 8);
            draw_game(true);
        } else {
            fix_speechDraw();
            Player.health -= 5;
        }
    }
}

//transfers player back to main map if they have the key
void light_fun()
{
    if (Player.has_key == false) {
        char* line1 = "I have to save";
        char* line2 = "the dog first!";
        speech(line1, line2);
        fix_speechDraw();
    } else {
        uLCD.cls();
        set_active_map(0);
        Player.x = 37;
        Player.y = 19;
        add_npc(16, 15, "Dog");
        draw_game(true);
    }
}

//ends game if player has key
int house_fun() 
{
    if (Player.has_key == true) return 1;
    else {
        char* line1 = "I have to save";
        char* line2 = "the dog!";
        speech(line1, line2);
        fix_speechDraw();
        return 0;
    }
}
