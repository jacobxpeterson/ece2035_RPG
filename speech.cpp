#include "speech.h"

#include "globals.h"
#include "hardware.h"


/**
 * Draw the speech bubble background.
 * Use this file to draw speech bubbles, look at the uLCD libraries for printing
 * text to the uLCD (see graphics.cpp note for link)
 */
static void draw_speech_bubble();


/**
 * Erase the speech bubble.
 */
static void erase_speech_bubble();

/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
static void draw_speech_line(const char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

//draw start menu
static void draw_start();

//wait on start until the player starts the game
static void start_wait();

void draw_speech_bubble()
{
    //top
    uLCD.line(3, 92, 124, 92, GREEN);
    //bottom
    uLCD.line(3, 114, 124, 114, GREEN);
    //left
    uLCD.filled_rectangle(0, 92, 2, 114, GREEN);
    //right
    uLCD.filled_rectangle(124, 92, 127, 114, GREEN);
    //bubble background
    uLCD.filled_rectangle(3, 93, 123, 113, BLACK);
   
}



void erase_speech_bubble()
{
    //fill border
    uLCD.filled_rectangle(0, 90, 2, 114, WHITE);
    uLCD.filled_rectangle(124, 90, 127, 114, WHITE);
    uLCD.filled_rectangle(3, 114, 123, 117, WHITE);
    //patch lower status bar
    uLCD.line(0, 118, 127, 118, GREEN);
    
    uLCD.line(3, 92, 124, 92, BLACK);
    //delete text
    uLCD.filled_rectangle(3, 93, 123, 113, BLACK);
  
}

void draw_speech_line(const char* line, int which)
{
    uLCD.textbackground_color(BLACK);
    if (which == TOP) {
        uLCD.locate(1,12);
        for(int i = 0; i < strlen(line); i++) {
            uLCD.printf("%c", line[i]);
            wait(0.02);
        }
    }
    else {
        uLCD.locate(1,13);
        for(int i = 0; i < strlen(line); i++) {
            uLCD.printf("%c", line[i]);
            wait(0.02);
        }
    }
}

void speech_bubble_wait()
{
    GameInputs in = read_inputs();
    
    while (in.b1) {
        uLCD.filled_circle(118, 120, 3, GREEN);
        in = read_inputs();
        wait_us(120);
        uLCD.filled_circle(118, 120, 3, BLACK);

    }
    uLCD.filled_circle(118, 120, 3, BLACK);
}

void speech(const char* line1, const char* line2)
{
    draw_speech_bubble();
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
    speech_bubble_wait();
    erase_speech_bubble();
}

void long_speech(const char* lines[], int n)
{
    for (int i = 0; i < n; i+=2) {
        speech(lines[i], lines[i+1]);
    }
}

void draw_menu()
{
    uLCD.locate(1,12);
    uLCD.set_font_size(0.5, 0.5);
    char* line = "Bat Knife BBgun";
    for(int i = 0; i < strlen(line); i++) {
            uLCD.printf("%c", line[i]);
            wait(0.02);
        }
    uLCD.set_font_size(1, 1);
    
}
int menu_wait()
{
    GameInputs in = read_inputs();
    int circAxis = 17;
    while(in.b1) {
        wait(0.2);
        uLCD.filled_circle(circAxis, 108, 2, GREEN);
        in = read_inputs();
        wait_us(250);
        uLCD.filled_circle(circAxis, 108, 2, BLACK);
        
        //if statements that change the selection
        if (circAxis == 17) {
            if ( (fabs(in.ax) > 0.2) && (in.ax < 0) )
            {
                circAxis = 52;
            }
        }
        wait(0.2);
        uLCD.filled_circle(circAxis, 108, 2, GREEN);
        in = read_inputs();
        wait(0.2);
        uLCD.filled_circle(circAxis, 108, 2, BLACK);
        if (circAxis == 52) {
            if ( (fabs(in.ax) > 0.2) && (in.ax < 0) )
            {
                circAxis = 94;
            } else if ((fabs(in.ax))> 0.2 && (in.ax > 0))
            {
                circAxis = 17;
            }
        }
        wait(0.2);
        uLCD.filled_circle(circAxis, 108, 2, GREEN);
        in = read_inputs();
        wait(0.2);
        uLCD.filled_circle(circAxis, 108, 2, BLACK);
        if (circAxis == 94) {
            if ( (fabs(in.ax) > 0.2) && (in.ax > 0) )
            {
                circAxis = 52;
            }
        }
        wait_us(200);
    } 
    if (circAxis == 52 || circAxis == 94)  return try_again();
    if(circAxis == 17)  return success();
    return 0;
}   

int try_again()
{
    const char* lines[6] = {"It had no", "effect! The", "Alien attacks!", "You took five", "damage!", " "};
    long_speech(lines, 6);
    return 0;
}

int success()
{
    uLCD.set_font_size(2,2);
    uLCD.text_bold(1);
    char* l1 = "craAACK";
    char* l2 = "";
    speech(l1, l2);
    uLCD.text_bold(0);
    uLCD.set_font_size(1,1);
    l1 = "The Alien was";
    l2 = "Defeated";
    speech(l1, l2);
    return 1;
}
int combat_menu()
{
    draw_speech_bubble();
    draw_menu();
    return menu_wait();
}

void draw_start()
{
    //uLCD.triangle(91, 25, 36, 82, 102, 93, 0x00FF00);
    uLCD.background_color(0x0000FF);
    uLCD.textbackground_color(0x0000FF);
    uLCD.cls();
    uLCD.locate(0,5);
    uLCD.text_width(2);
    uLCD.text_height(2);
    uLCD.color(0xFFFF00);
    uLCD.printf("DOGALIEN!");
    
    uLCD.text_width(1);
    uLCD.text_height(1);
    uLCD.locate(6,11);
    uLCD.printf("START");
    
    //restore defaults
    uLCD.textbackground_color(0x000000);
    uLCD.background_color(0x000000);
    uLCD.color(0x00FF00);

}

void start_wait()
{
    GameInputs in = read_inputs();
    
    while (in.b1) {
        uLCD.filled_circle(82, 91, 3, 0xFFFF00);
        in = read_inputs();
        wait_us(120);
        uLCD.filled_circle(82, 91, 3, 0x0000FF);

    }
}

void start_menu()
{
    draw_start();
    start_wait();
}
