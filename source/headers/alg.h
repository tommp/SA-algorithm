#ifndef ALG_H
#define ALG_H


/*Included headers*/
/*---------------------------------------------*/
#include "errorlogger.h"
/*---------------------------------------------*/

/*Included dependencies*/
/*---------------------------------------------*/
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h> 
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <assert.h>
/*---------------------------------------------*/

/*Header content*/
/*=============================================*/

namespace algvars{
	/*Necessary for the algorithm*/
	const float goalvalue = 1.0; /*If 1 searches for optimal value*/
	const float temp = 1.0; /*Initial temperature*/
	const float dtemp = 0.0001; /*Temp reduction in each iteration*/
	const int iterationdelay = 10; /*Microseconds, min delay between prints*/
	const int iterations = 20000; /*Iterations of the main for loop*/
	const int algiterations = 10; /*Iterations of the algorithm per for loop looping*/
	const int num_neighbours = 300; /*Number of neighbours generated for each loop in the algorithm*/
	const float templimit = 0.05; /*When temp drops below this value, improvement is enforced, set to negative value to disable*/

	/*For specific tasks*/

	/*Eggcarton puzzle*/
	const int m = 8;
	const int n = 8;
	const int k = 2;

	/*For rendering*/
	const int windowscale = 30;
	const int windowheight = windowscale*m;
	const int windowlength = windowscale*n;
	const int margin = 1;

	/*Colors*/
	const int emptycolor[] = {0, 0, 0};
	const int eggcolor[] = {0, 0, 255};
	const int textcolor[] = {0, 0, 0};
	const int tilecolor[] = {255, 255, 255};
}

/*General class template for the SA algorithm*/
template <class UserType> 
class SA{
	protected:
		UserType currentstate;
		float currentscore;
		float objective_target;
		float temp;
		float dtemp;

		UserType beststate;
		float bestscore;

	public:
		/*User defined*/
		virtual float objective_function(const UserType&) = 0;
		virtual std::vector<UserType> generate_n_neighbours(const int&) = 0;
		/*------------*/

		UserType getstate(){return currentstate;}
		float getscore(){return currentscore;}
		float gettemp(){return temp;}

		bool check_if_solution_found();
		bool simulated_annealing(const int& number_of_iterations, const int&num_neighbours);

		SA(const float& initialtemp, const float& initialdtemp, const float& targetscore, const UserType& startpoint);
};

/*Constuctor, initializes member variables*/
template <class UserType>
SA<UserType>::SA(const float& initialtemp, const float& initialdtemp, const float& targetscore, const UserType& startpoint) : currentstate(startpoint), beststate(startpoint){
	assert(initialtemp != 0);
	assert(initialdtemp != 0);
	assert(targetscore > 0.0);

	srand(time(0));
	temp = initialtemp;
	dtemp = initialdtemp;
	if(not currentstate){
		errorlogger("Failed to initialize currentstate data, check user defined class, make sure a constructor and the assignment operator = are defined");
		exit(0);
	}
	objective_target = targetscore;
	currentscore = 0.0;
	bestscore = 0.0;
	std::cout<<"Initialization complete!"<<std::endl;
}


/*SA-methods*/
/*-----------------------------------------------------------------*/

/*Checks if a satisfying solution has been found*/
template <class UserType> 
bool SA<UserType>::check_if_solution_found(){
	if(objective_target <= currentscore){
		return true;
	}
	else{
		return false;
	}
}

/*The SA algorithm*/
template <class UserType> 
bool SA<UserType>::simulated_annealing(const int& number_of_iterations, const int&num_neighbours){
	for(int iteration = 0; iteration < number_of_iterations; iteration++){
		if(check_if_solution_found()){
			return true;
		}

		float maxvalue = -1.0;
		float currentvalue = 0.0;
		UserType max_p = currentstate;

		/*Optional, enforces improvement if temp is below templimit, set templimit to a negative value to disable*/
		if(temp < algvars::templimit){
			maxvalue = bestscore;
			max_p = beststate;
		}
		
		/*Generate num_neighbours neighbours*/
		std::vector<UserType> neighbours = generate_n_neighbours(num_neighbours);
		for(typename std::vector<UserType>::iterator it = neighbours.begin(); it != neighbours.end();++it){

			/*Evaluate each neighbour*/
			currentvalue = objective_function(*it);

			/*Find neighbour with highest value*/
			if(maxvalue < currentvalue){
				maxvalue = currentvalue;
				max_p = *it;

			}/*IF*/
		}/*FOR*/

		/*Calculate q and check for 0 in denominator*/
		float q = maxvalue;
		if(currentscore > 0){
			q = (q - currentscore)/currentscore;
		}

		/*Calculate e (exponential), and use it to calculate p*/
		float exponential;
		float p = 1;
		if(temp > 0.001){
			exponential = exp((-q)/temp);
		}
		else{
			exponential = exp(-q/0.001);
		}

		if(exponential < p){
			p = exponential;
		}

		/*Generate a x between 0 and 1*/
		float x =  (float)rand()/(float)(RAND_MAX);

		/*If p<x, select the neighbour with the best score, if not explore in a random direction*/
		if(p < x){
			currentstate = max_p;
			currentscore = objective_function(max_p);
		}
		else{
			assert(neighbours.size() != 0);
			int randindex = rand()/(RAND_MAX/neighbours.size());
			currentstate = neighbours[randindex];
			currentscore = objective_function(neighbours[randindex]);
		}

		/*Store the global best, costly overhead, disable for certain tasks where it might not be efficient*/
		if(currentscore >= bestscore){
			bestscore = currentscore;
			beststate = currentstate;
		}

		/*Reduce the temperature*/
		temp = temp - dtemp;
		if(temp <= 0.0){
			temp = 0.0;
		}
	}/*FOR*/
	return false;
}/*FUNCTION*/


/*SPECIFIC PROBLEMS*/

/*UserType for the eggcarton puzzle, data represented as a 2d array of booleans where true symbolizes an egg*/
class EggCarton{
	friend class EggCartonPuzzle;
	private:
		int m;
		int n;
		int k;
		bool** mask;

	public:
		int getm()const{return m;}
		int getn()const{return n;}
		int getk()const{return k;}
		bool** getmask()const{return mask;}
		void print(SDL_Renderer *ren);

		EggCarton& operator=(const EggCarton& rhs);
		bool operator!();
		
		EggCarton(const EggCarton& rhs);
		EggCarton(int im, int in, int ik);
		~EggCarton();
		
};

/*Child class of the SA class with objective and neighbour functions overridden*/
class EggCartonPuzzle : public SA<EggCarton> {
	public:
		float objective_function(const EggCarton& point);
		std::vector<EggCarton> generate_n_neighbours(const int& num_neighbours);
		EggCartonPuzzle(const float& initialtemp, const float& initialdtemp, const float& targetscore, const EggCarton& startpoint) : SA(initialtemp, initialdtemp, targetscore, startpoint) {};
};

/*OVERRIDE SWITCHBOARD HERE*/

/*=============================================*/

#endif