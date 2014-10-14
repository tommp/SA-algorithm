#include "./headers/main.h"

int main(int argc, char** argv){

	/*Initializes SDL for graphical display*/
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		SDLerrorLogger("SDL initialization");
		std::cout<<"Failed to initialize SDL, see errorlog for details."<<std::endl;
		return 1;
	}

	/*Disables pesky screensavers while our wonderful graphics are beeing displayed*/
	SDL_DisableScreenSaver();

	/*Initializes a window to render graphics in*/
	SDL_Window *win = SDL_CreateWindow("SA", 0, 0, algvars::windowlength, algvars::windowheight, 0);
	if (win == nullptr){
		SDLerrorLogger("SDL_CreateWindow");
		std::cout<<"Failed to create SDL window, see errorlog for details."<<std::endl;
		return 1;
	}

	/*Initializes a window to render data in*/
	SDL_Window *datawin = SDL_CreateWindow("SA-DATA", 0, algvars::windowheight+24, algvars::windowlength, 100, 0);
	if (win == nullptr){
		SDLerrorLogger("SDL_CreateWindow");
		std::cout<<"Failed to create SDL window, see errorlog for details."<<std::endl;
		return 1;
	}

	/*Initializes the renderer to draw in*/
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDLerrorLogger("SDL_CreateRenderer");
		std::cout<<"Failed to create SDL renderer, see errorlog for details."<<std::endl;
		return 1;
	}

	/*Initializes the renderer to draw data in*/
	SDL_Renderer *dataren = SDL_CreateRenderer(datawin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDLerrorLogger("SDL_CreateRenderer");
		std::cout<<"Failed to create SDL renderer, see errorlog for details."<<std::endl;
		return 1;
	}

	/*Used for printing text*/
	gfxPrimitivesSetFont(0, 0, 0);
	std::string score = "Score:         ";
	std::string temperature = "Temperature:   ";
	std::string convert;

	/*Initialize eggcarton puzzle*/
	EggCarton carton(algvars::m, algvars::n, algvars::k);
	EggCartonPuzzle puzzle(algvars::temp, algvars::dtemp, algvars::goalvalue, carton);

	/*Renders and prints the initial windows*/
	puzzle.getstate().print(ren);
	SDL_SetRenderDrawColor(dataren, 
						algvars::emptycolor[0], 
						algvars::emptycolor[1], 
						algvars::emptycolor[2], 255);
	SDL_RenderClear(dataren);
	SDL_RenderPresent(dataren);
	SDL_RenderPresent(ren);

	bool algdone = false;
	for(int i = 0; i < algvars::iterations; i++){

		/*Run the algorithm for algiterations iterations 
		with num_neighbours neighbours and check if a satisfying solution was found*/
		if(puzzle.simulated_annealing(algvars::algiterations, algvars::num_neighbours)){
			algdone = true;
		}

		/*Print puzzle board*/
		puzzle.getstate().print(ren);

		/*Clear textwindow*/
		SDL_SetRenderDrawColor(dataren, 
						algvars::emptycolor[0], 
						algvars::emptycolor[1], 
						algvars::emptycolor[2], 255);
		SDL_RenderClear(dataren);

		/*Print text*/
		stringRGBA(dataren, 5, 5, score.c_str(),0,0,0,0);
		convert = std::to_string(puzzle.getscore());
		stringRGBA(dataren,115,5, convert.c_str(),0,0,0,0);

		stringRGBA(dataren, 5, 15, temperature.c_str(),0,0,0,0);
		convert = std::to_string(puzzle.gettemp());
		stringRGBA(dataren, 115, 15, convert.c_str(),0,0,0,0);

		/*Render both windows*/
		SDL_RenderPresent(dataren);
		SDL_RenderPresent(ren);

		/*Quit if a key is typed*/
		pollevent(algdone);
		if(algdone){
			break;
		}

		usleep(algvars::iterationdelay);
	}
	
	/*Print result to terminal*/
	std::cout<<"\n------------------------------------------------------------"  <<std::endl;
	std::cout<<"Solution found!"<<std::endl;
	std::cout<<"Final score:"<< puzzle.getscore() <<std::endl;
	std::cout<<"------------------------------------------------------------\n"  <<std::endl;
	
	/*Wait for a key to be pressed before quitting*/
	waitForEvent();
}







void pollevent(bool& var){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		switch (event.type){
			case SDL_QUIT:
				exit(0);
				break;

			case SDL_KEYDOWN:
				var = true;
				break;
			default:
				break;
		}
	}
}


void waitForEvent(){
	SDL_Event event;
	bool done = false;
	while((!done) && (SDL_WaitEvent(&event))) {
        switch(event.type) {
    
            case SDL_KEYDOWN:
                done = true;
                break;


            case SDL_QUIT:
                done = true;
                exit(0);
                break;
                
            default:
                break;
        } 
            
    }
}