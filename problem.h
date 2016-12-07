/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   problem.h
 * Author: bstone
 *
 * Created on October 20, 2016, 9:52 AM
 */

#ifndef PROBLEM_H
#define PROBLEM_H

#include <ai_search.h>
#include "model.h"
#include "state.h"
#include <map>
#include "Swampert.h"

namespace ems
{
  namespace Scavenger
  {
      
//      class Model;
//      class Cell;
//      class State;
      class Problem : public ai::Search::Problem
      {
         public:
          Problem(ai::Search::State *initial_state_in, Model *world_in, bool unvisited, bool recharge);
          virtual ~Problem();

          virtual std::vector<ai::Search::Action *> Actions(const ai::Search::State * const state_in);

          virtual ai::Search::State *Result(const ai::Search::State * const state_in, const ai::Search::Action * const action_in);
          double HPCost(const ai::Search::State  * const state1_in,
                const ai::Search::Action * const action_in,
                const ai::Search::State  * 
                const state2_in) const;
          
          virtual bool   GoalTest(const ai::Search::State * const state_in) const;
          virtual double StepCost(const ai::Search::State  * const state1_in,
                                  const ai::Search::Action * const action_in,
                                  const ai::Search::State  * const state2_in) const;

          virtual double Heuristic(const ai::Search::State  * const state_in) const;

            bool SetGoal(State *goal_state_in);
            bool SetBase(State *base_state_in);
            std::map<std::pair<double,double>, Cell *> mCells;
        protected:
          Model *world;
            bool mUnvisitedGoal, recharge;
          State *base_state;
          State *goal_state;
        private:
      };
  }
}
#endif /* PROBLEM_H */

