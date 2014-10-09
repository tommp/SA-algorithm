#ifndef MAIN_H
#define MAIN_H


/*Included headers*/
/*---------------------------------------------*/
#include "./errorlogger.h"
#include "./alg.h"
/*---------------------------------------------*/

/*Included dependencies*/
/*---------------------------------------------*/
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <string>
#include <sstream> 
#include <unistd.h>

/*---------------------------------------------*/

/*Header content*/
/*=============================================*/

void waitForEvent();
void pollevent(bool& var);

/*=============================================*/

#endif