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
	const int m = 15;
	const int n = 15;
	const int k = 7;
	const int goalvalue = 50;
	const float temp = 1.0;
	const float dtemp = 0.0005;

	const float templimit = 0.9;

	const int windowscale = 30;
	const int windowheight = windowscale*m;
	const int windowlength = windowscale*n;
	const int margin = 1;

	const int emptycolor[] = {0, 0, 0};
	const int eggcolor[] = {0, 0, 255};
	const int textcolor[] = {0, 0, 0};
	const int tilecolor[] = {255, 255, 255};

	const int iterationdelay = 10; /*Microseconds*/
	const int iterations = 100000;
	const int algiterations = 1;
	const int num_neighbours = 300;

	const bool allowillegal = false;

}

template <class UserType> class SA{
	protected:
		UserType currentstate;
		int currentscore;
		int objective_target;
		float temp;
		float dtemp;

		UserType beststate;
		int bestscore;

	public:
		/*User defined*/
		virtual int objective_function(const UserType&) = 0;
		virtual std::vector<UserType> generate_n_neighbours(const int&) = 0;
		/*------------*/

		UserType getstate(){return currentstate;}
		int getscore(){return currentscore;}
		float gettemp(){return temp;}

		bool check_if_solution_found();
		bool simulated_annealing(const int& number_of_iterations, const int&num_neighbours);

		SA(const float& initialtemp, const float& initialdtemp, const int& targetscore, const UserType& startpoint);
};

template <class UserType>
SA<UserType>::SA(const float& initialtemp, const float& initialdtemp, const int& targetscore, const UserType& startpoint) : currentstate(startpoint), beststate(startpoint){
	assert(initialtemp != 0);
	assert(initialdtemp != 0);
	assert(targetscore > 0);

	srand(time(0));
	temp = initialtemp;
	dtemp = initialdtemp;
	if(not currentstate){
		errorlogger("Failed to initialize currentstate data, check user defined class, make sure a constructor and the assignment operator = are defined");
		exit(0);
	}
	objective_target = targetscore;
	currentscore = 0;
	bestscore = 0;
	std::cout<<"Initialization complete!"<<std::endl;
}


/*SA-methods*/
/*-----------------------------------------------------------------*/

template <class UserType> 
bool SA<UserType>::check_if_solution_found(){
	if(objective_target <= currentscore){
		return true;
	}
	else{
		return false;
	}
}

template <class UserType> 
bool SA<UserType>::simulated_annealing(const int& number_of_iterations, const int&num_neighbours){
	for(int iteration = 0; iteration < number_of_iterations; iteration++){
		if(algvars::allowillegal){
			if(check_if_solution_found()){
				return true;
			}
		}
		int maxvalue = -1;
		int currentvalue = 0;
		UserType max_p = currentstate;
		if(temp < algvars::templimit){
			maxvalue = bestscore;
			max_p = beststate;
		}
		

		std::vector<UserType> neighbours = generate_n_neighbours(num_neighbours);
		for(typename std::vector<UserType>::iterator it = neighbours.begin(); it != neighbours.end();++it){
			currentvalue = objective_function(*it);
			if(maxvalue < currentvalue){
				maxvalue = currentvalue;
				max_p = *it;

			}/*IF*/
		}/*FOR*/

		float q = (float)(maxvalue);
		if(currentscore > 0){
			q = (q - (float)(currentscore))/(float)(currentscore);
		}


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

		float x =  (float)(rand())/(float)(RAND_MAX);

		if(p < x){
			currentstate = max_p;
			currentscore = objective_function(max_p);
		}
		else{
			assert(neighbours.size() != 0);
			int randindex = rand()/(RAND_MAX/neighbours.size());
			currentstate = neighbours[randindex];
			currentscore = currentvalue;
		}

		if(currentscore > bestscore){
			bestscore = currentscore;
			beststate = currentstate;
		}

		temp = temp - dtemp;
		if(temp <= 0.0){
			temp = 0.0;
		}
	}/*FOR*/
	if(not algvars::allowillegal){
		if(check_if_solution_found()){
			return true;
		}
	}
	return false;
}/*FUNCTION*/


/*SPECIFIC PROBLEMS*/

class EggCarton{
	friend class EggCartonPuzzle;
	private:
		int m;
		int n;
		int k;
		bool** mask;

	public:
		int numeggs();
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


class EggCartonPuzzle : public SA<EggCarton> {
	public:
		int objective_function(const EggCarton& point);
		std::vector<EggCarton> generate_n_neighbours(const int& num_neighbours);
		EggCartonPuzzle(const float& initialtemp, const float& initialdtemp, const int& targetscore, const EggCarton& startpoint) : SA(initialtemp, initialdtemp, targetscore, startpoint) {};
};



/*
class Switchboard{
	private:
		int lenghtofwire;

	public:
		int getwirelength()const{return lenghtofwire;}
};

class SwitchboardPuzzle : public SA<Switchboard> {
	public:
		int objective_function(const Switchboard& point);
		std::vector<Switchboard> generate_n_neighbours(const int& num_neighbours);
};

int SwitchboardPuzzle::objective_function(const Switchboard& point){
	return point.getwirelength();
}
*/


/*=============================================*/

#endif