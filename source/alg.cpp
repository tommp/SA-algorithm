#include "./headers/alg.h"

/*User defined-----------------------------------------------------*/

int EggCarton::numeggs(){
	int eggs = 0;
	for(int row = 0; row < m; row++){
		for(int col = 0; col < m; col++){
			if(mask[row][col]){
				eggs++;
			}
		}
	}
	return eggs;
}

void EggCarton::print(SDL_Renderer *ren){
	SDL_Rect rect = {0, 0, algvars::windowscale - algvars::margin*2, algvars::windowscale - algvars::margin*2};
	SDL_SetRenderDrawColor(ren, 
						algvars::emptycolor[0], 
						algvars::emptycolor[1], 
						algvars::emptycolor[2], 255);
	SDL_RenderClear(ren);

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

	rect.x = 0;
	rect.y = 0;
	for (int row = 0; row < m; row++){
		for (int col = 0; col < n; col++){
			if(mask[row][col] == true){
				filledCircleRGBA (ren, 
						rect.x+algvars::windowscale/2, 
						rect.y+algvars::windowscale/2,
						algvars::windowscale/4,
						algvars::eggcolor[0], 
						algvars::eggcolor[1], 
						algvars::eggcolor[2], 255);
			}
			rect.x = algvars::windowscale*col;
			rect.y = algvars::windowscale*row;
		}
	}
}

/*Destuctor and constructors*/

EggCarton::~EggCarton(){
	for(int row = 0; row < m; row++){
    	delete[] mask[row];
    }
    delete[] mask;
}

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
}

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
	int x = (rand()-1)/(RAND_MAX/neighbour.m);
	int y = (rand()-1)/(RAND_MAX/neighbour.n);
	for(int i = 0; i < num_neighbours; i++){
		if(neighbour.mask[x][y]){ 
			int dir = (rand()-1)/(RAND_MAX/4);
			switch (dir) {
				case 0:
					neighbour.mask[x][y] = false;
					if(x < currentstate.m-1){
						neighbour.mask[x+1][y] = true;
					}
					break;
				case 1:
					neighbour.mask[x][y] = false;
					if(y < currentstate.n-1){
						neighbour.mask[x][y+1] = true;
					}
					break;
				case 2:
					neighbour.mask[x][y] = false;
					if(x > 0){
						neighbour.mask[x-1][y] = true;
					}
					break;
				case 3:
					neighbour.mask[x][y] = false;
					if(y > 0){
						neighbour.mask[x][y-1] = true;
					}
					break;
			}
		}
		else{
			neighbour.mask[x][y] = true;
		}
		neighbours.push_back(neighbour);
		neighbour = currentstate;
	}
	return neighbours;
}

int EggCartonPuzzle::objective_function(const EggCarton& carton){
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
	std::fill(ldiagon.begin(), ldiagon.end(), 1);

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

	/*Add points for non violating eggs*/
	int value = 0;
	for(int row = 0; row < carton.getm(); row++){
		for(int col = 0; col < carton.getn(); col++){
			if((carton.mask[row][col]) && (columns[col]<=carton.k) && (rows[row]<=carton.k) && (rdiagon[carton.getm()-row + col - 1] <= carton.k) && (ldiagon[row + col]<=carton.k)){
				value++;
			}
		}
	}
	return value;
}

