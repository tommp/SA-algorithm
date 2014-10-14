#include "./headers/alg.h"

/*User defined-----------------------------------------------------*/

/*Prints the current boardmask to a given renderer*/
void EggCarton::print(SDL_Renderer *ren){
	SDL_Rect rect = {0, 0, algvars::windowscale - algvars::margin*2, algvars::windowscale - algvars::margin*2};
	SDL_SetRenderDrawColor(ren, 
						algvars::emptycolor[0], 
						algvars::emptycolor[1], 
						algvars::emptycolor[2], 255);
	SDL_RenderClear(ren);

	/*Make tiles*/
	for (int row = 0; row < m; row++){
		for (int col = 0; col < n; col++){

			rect.x = algvars::margin + algvars::windowscale*col;
			rect.y = algvars::margin + algvars::windowscale*row;

			SDL_SetRenderDrawColor(ren, 
					algvars::tilecolor[0], 
					algvars::tilecolor[1], 
					algvars::tilecolor[2], 255);
			SDL_RenderFillRect(ren, &rect);
			
		}
	}

	/*Place the eggs*/
	for (int row = 0; row < m; row++){
		for (int col = 0; col < n; col++){
			if(mask[row][col]){
				rect.x = algvars::windowscale*col;
				rect.y = algvars::windowscale*row;

				filledCircleRGBA (ren, 
						rect.x+algvars::windowscale/2, 
						rect.y+algvars::windowscale/2,
						algvars::windowscale/4,
						algvars::eggcolor[0], 
						algvars::eggcolor[1], 
						algvars::eggcolor[2], 255);
			}
		}
	}
}

/*Destuctor and constructors*/

/*Destructor*/
EggCarton::~EggCarton(){
	for(int row = 0; row < m; row++){
    	delete[] mask[row];
    }
    delete[] mask;
}

/*Constructor*/
EggCarton::EggCarton(int im, int in, int ik){
	m = im;
	n = in;
	k = ik;
	mask = new bool*[m];
	for(int row = 0; row < m; row++){
    	mask[row] = new bool[n];
    }

	for(int row = 0; row < m; row++){
    	for(int col = 0; col < n; col++){
    		mask[row][col] = false;
    	}
    }

    int eggmark = 0;

    /*Place k random eggs on the restricting axis (if columns < rows place in columns and vice versa)*/
    if(im < in){
    	for(int row = 0; row < im; row++){
	    	for(int eggsplaced = 0; eggsplaced < ik; eggsplaced++){
	    		eggmark = (rand()-1)/(RAND_MAX/in);
	    		for(int col = 0; col < in; col++){
		    		if(mask[row][eggmark]){
		    			if(eggmark != in-1){
		    				eggmark++;
		    			}
		    			else{
		    				eggmark = 0;
		    			}
		    		}
		    		else{
		    			break;
		    		}
		    	}

	    		mask[row][eggmark] = true;
	    	}
	    }
    }
    else{
    	for(int col = 0; col < in; col++){
	    	for(int eggsperrow = 0; eggsperrow < ik; eggsperrow++){
	    		eggmark = (rand()-1)/(RAND_MAX/in);
	    		for(int col = 0; col < in; col++){
		    		if(mask[eggmark][col]){
		    			if(eggmark != in-1){
		    				eggmark++;
		    			}
		    			else{
		    				eggmark = 0;
		    			}
		    		}
		    	}

	    		mask[eggmark][col] = true;
	    	}
	    }
    }
}

/*Copyconstructor*/
EggCarton::EggCarton(const EggCarton& rhs){
    m = rhs.getm();
    n = rhs.getn();
    k = rhs.getk();

    mask = new bool*[m];
	for(int row = 0; row < m; row++){
    	mask[row] = new bool[n];
    }

    for(int row = 0; row < m; row++){
    	for(int col = 0; col < n; col++){
    		mask[row][col] = rhs.getmask()[row][col];
    	}
    }
}

/*Operator overloads*/

bool EggCarton::operator!(){
	if(mask == nullptr){
		return true;
	}
	return false;
}

EggCarton& EggCarton::operator=(const EggCarton& rhs)
{
    if(this == &rhs)
       return *this;
    m = rhs.getm();
    n = rhs.getn();
    k = rhs.getk();

    if(mask == nullptr){
    	mask = new bool*[m];
		for(int row = 0; row < m; row++){
    		mask[row] = new bool[n];
    	}
    }

    for(int row = 0; row < m; row++){
    	for(int col = 0; col < n; col++){
    		mask[row][col] = rhs.getmask()[row][col];
    	}
    }
    return *this;
}

/*Overloaded functions*/

std::vector<EggCarton> EggCartonPuzzle::generate_n_neighbours(const int& num_neighbours){
	std::vector<EggCarton> neighbours;
	EggCarton neighbour = currentstate;
	int x = (rand())/(RAND_MAX/neighbour.m);
	int y = (rand())/(RAND_MAX/neighbour.n);
	for(int i = 0; i < num_neighbours; i++){

		/*Find a neighbour on a random row or column*/
		while(not neighbour.mask[x][y]){
			if(neighbour.m < neighbour.n){
				if(y != neighbour.n-1){
					y++;
				}
				else{
					y = 0;
				}
			}
			else{
				if(x != neighbour.m-1){
					x++;
				}
				else{
					x = 0;
				}
			}
		}

		/*Set direction to move the egg randomly*/
		int seconddir = (rand()-1)/(RAND_MAX/2);
		if(seconddir){
			seconddir = -1;
		}
		else{
			seconddir = 1;
		}

		/*Move the egg in a direction given by the restricting axis*/
		if(neighbour.m < neighbour.n){
			if(!(y+seconddir < 0) && !(y+seconddir > neighbour.n-1)){
				if(!(neighbour.mask[x][y + seconddir])){
					neighbour.mask[x][y] = false;
					neighbour.mask[x][y + seconddir] = true;
				}
			}
		}
		else{
			if(!(x+seconddir < 0) && !(x+seconddir > neighbour.m-1)){
				if(!(neighbour.mask[x + seconddir][y])){
					neighbour.mask[x][y] = false;
					neighbour.mask[x + seconddir][y] = true;
				}
			}
		}

		/*Add and reset the neighbour*/
		neighbours.push_back(neighbour);
		neighbour = currentstate;
	}
	return neighbours;
}

float EggCartonPuzzle::objective_function(const EggCarton& carton){
	std::vector<int> columns;
	columns.resize(carton.getn());
	std::fill(columns.begin(), columns.end(), 0);

	std::vector<int> rows;
	rows.resize(carton.getm());
	std::fill(rows.begin(), rows.end(), 0);

	std::vector<int> rdiagon;
	rdiagon.resize(carton.getn()+carton.getm()-1);
	std::fill(rdiagon.begin(), rdiagon.end(), 0);

	std::vector<int> ldiagon;
	ldiagon.resize(carton.getn()+carton.getm()-1);
	std::fill(ldiagon.begin(), ldiagon.end(), 0);

	/*Increment number of eggs in columns, rows and diagonals according to the eggs position*/
	for(int row = 0; row < carton.getm(); row++){
		for(int col = 0; col < carton.getn(); col++){
			if(carton.mask[row][col]){
				columns[col]++;
				rows[row]++;
				rdiagon[carton.getm()-row + col - 1]++;
				ldiagon[row + col]++;
			}
		}
	}

	/*Add points for eggs that are placed in legal positions*/
	int value = 0;
	for(int row = 0; row < carton.getm(); row++){
		for(int col = 0; col < carton.getn(); col++){
			if((carton.mask[row][col]) && (columns[col]<=carton.k) && (rows[row]<=carton.k) && (rdiagon[carton.getm()-row + col - 1] <= carton.k) && (ldiagon[row + col]<=carton.k)){
				value++;
			}
		}
	}

	/*Return a float representing degree of optimality, 1.0 implies an optimal solution*/
	if(carton.getm() < carton.getn()){
		return (float)value / ((float)carton.getm() * (float)carton.getk());
	}
	else{
		return (float)value / ((float)carton.getn() * (float)carton.getk());
	}
}