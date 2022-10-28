#ifndef SPEECH_H
#define SPEECH_H

/**
 * Display a speech bubble.
 */
void speech(const char* line1, const char* line2);

/**
 * Display a long speech bubble (more than 2 lines).
 * 
 * @param lines The actual lines of text to display
 * @param n The number of lines to display.
 */
void long_speech(const char* lines[], int n);

//combat menu
int combat_menu();

//try again if they pick wrong weapon
int try_again();

//if player picks right weapon
int success();

//display start menu
void start_menu();


#endif // SPEECH_H