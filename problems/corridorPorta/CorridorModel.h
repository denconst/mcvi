/**
   This problem is different from the problem in the published paper
   "Monte Carlo Value Iteration for Continuous-State POMDPs". The
   results are not directly comparable.
*/

#ifndef __CORRIDORMODEL_H
#define __CORRIDORMODEL_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "Model.h"
#include "Obs.h"
#include "GaussModel.h"

/**
   Constants and types used in corridor model.
*/
const double Discount = 0.95;

const int NumDoors = 4;
//Position of doors, the first door is good door
//Note that door should inside the interval
// [-CorridorEndLength+CorridorEndLength, CorridorLength-CorridorEndLength]
const double DoorPositions[] = {3, -11, -5, 9};
const double DoorRadius = 1.0;
const double CorridorLength = 20;
//const double CorridorEndLength = 5;
//const double EnterReward = 1;
//const double WrongPenalty = -2;

const double MovementCost = 0; // Cost of any other actions
const long NumStateVars = 2; // enter success, coordinates of rover
const long NumObsVars = 1;
enum directions {ActLeft, ActRight, ActEnter}; // Action names
enum observations {ObsLeftEnd, ObsRightEnd, ObsDoor, ObsCorridor};
const double MovementVariance = 0.05;
const double ActionScale = 2.0;

const long NumActs = 3; // Number of actions

//const long TermObs = -1;
const long TermState = -1; // indicates state is at terminal state

// Same as actions but repeat action till macro states change.
const long NumMacroActs = 0;
const long NumInitPolicies = 1;

class ParticlesBelief;

/**
   @class CorridorModel
   @brief Continuous Corridor Problem
   @details ...
   @author Haoyu Bai
   @date 14 Mar 2010
*/
class CorridorModel : public Model
{
public:
    CorridorModel();

    double sample(const State& currState, const Action& action, State* nextState, Obs* obs, RandStream* randStream );

    //XXX No macro act
    double sample(const State& currState, const Action& macroAction, long controllerState, State* nextState, long* nextControllerState, Obs* obs, RandStream* randStream );

    /**
       Greedy policy that selects the action that moves left
    */
    double initPolicy(const State& currState, const Action& initAction, long controllerState, State* nextState, long* nextControllerState, Obs* obs, RandStream* randStream );

    State sampleInitState();
    ParticlesBelief* getInitBelief(int numStates);


    /* actually do not need to be real prob, any kind of weight is ok */
    double getObsProb(const Action& action, const State& state, const Obs& obs);

    double upperBound(const State& state);

    double getMaxReward() { return MaxReward;};

    double getMinReward() { return MinReward;};

    bool allowableAct(const Belief& belief, const Action& action);

    inline obsType getObsType(const Obs& obs) { return OtherObs; };

    inline void setObsType(Obs* obs, obsType type) {
        obs->obs[0] = type;
    };

    inline bool isTermState(const State& state) { return (static_cast<long>(state[0]) == TermState);};

    /**
       Output an ascii visual display of the state.
    */
    void displayState(State state, long type = 0);

  private:
    void initRewardModel();
    void initObsModel();
    void initInitState();

    double probRandom;
    Gauss MovementModel[3];
    GaussMixture RewardModel[3];
    GaussMixture InitState;
    GaussModel ObsModel;
    double MaxReward;
    double MinReward;
};

#endif
