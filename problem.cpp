/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "problem.h"
#include <iostream>
#include <math.h>

namespace bjs
{
  namespace Scavenger
  {
    Problem::Problem(ai::Search::State *initial_state_in, Model  *world_in, bool u, bool r)
      : ai::Search::Problem(initial_state_in),
        world(world_in), mUnvisitedGoal(u), recharge(r),base_state(0),
        goal_state(0)
    {
      /* empty */
    }
    Problem::~Problem()
    {
      if(this->world)
        {
          delete this->world;
          this->world = 0;
        }
      if(this->goal_state)
        {
          delete this->goal_state;
          this->goal_state = 0;
        }
      if(this->base_state)
        {
          delete this->base_state;
          this->base_state = 0;
        }
    }

    std::vector<ai::Search::Action *> Problem::Actions(const ai::Search::State * const state_in)
    {
      std::vector<ai::Search::Action *> actions;
          const State * const state = dynamic_cast<const State * const>(state_in);
          bool noquit;
          if(state->GetCharge() < 0.0){
              std::cout << "NOT ENOUGH CHARGE FOR THIS ACTION" << std::endl;
                 return actions;
          }
          Cell * mCell = state->GetCell();
        if ((mCell->msnorth == "plain"|| mCell->msnorth == "mud"|| mCell->msnorth == "rocks"))
        {
            Action * new_action = new Action();
             new_action->SetType(((int)ai::Scavenger::Action::GO_NORTH));
             actions.push_back(new_action);
             noquit = true;
        }
        if ((mCell->mseast ==  "plain" || mCell->mseast == "mud" || mCell->mseast == "rocks"))
        {
            Action * new_action = new Action();
            new_action->SetType(((int)ai::Scavenger::Action::GO_EAST));
             actions.push_back(new_action);
             noquit = true;
        }
        if ((mCell->mswest ==  "plain" || mCell->mswest == "mud" || mCell->mswest == "rocks"))
        {
            Action * new_action = new Action();
            new_action->SetType(((int)ai::Scavenger::Action::GO_WEST));
             actions.push_back(new_action);
             noquit = true;
        }
        if ((mCell->mssouth ==  "plain" || mCell->mssouth == "mud" || mCell->mssouth == "rocks"))
        {
            Action * new_action = new Action();
           new_action->SetType(((int)ai::Scavenger::Action::GO_SOUTH));
             actions.push_back(new_action);
             noquit = true;
        }
        if(!noquit)
        {
            Action * new_action = new Action();
            std::cout << "*************************************************QUIT ACTION DUE TO BEING STUCK**********************************************************" << std::flush << std::endl;
            new_action->SetType(((int)ai::Scavenger::Action::QUIT));
             actions.push_back(new_action);
        }
      return actions;
    }
      
    ai::Search::State *Problem::Result(const ai::Search::State * const state_in, const ai::Search::Action * const action_in)
    {
      const State * const state = dynamic_cast<const State * const>(state_in);
      State  * new_state  = new State();
      const Action * const action = dynamic_cast<const Action * const>(action_in);
      int dx[4] = { 0, 0, 1000, -1000 };
      int dy[4] = {  1000, -1000, 0, 0 };
      int i = action->GetType();
      int new_x = state->GetX() + dx[i];
      int new_y = state->GetY() + dy[i];
      std::pair<int, int> pair;
      pair.first = new_x;
      pair.second = new_y;
      std::map<std::pair<int, int>, Cell*>::const_iterator it;
      it = this->mCells.find(pair);
      if(it != mCells.end())
      {
          new_state->SetX(new_x);
          new_state->SetY(new_y);
          Cell * cell = mCells[pair];
          new_state->SetCell(*cell);
          new_state->SetZ(cell->mcell_z);
          new_state->SetCellNum(cell->mcell_num);
          new_state->SetCharge(state->GetCharge());
          new_state->SetHP(state->GetHP());
          double step = StepCost(state, action, new_state);
          new_state->SetCharge(new_state->GetCharge()- step);
          double hpcost = HPCost(state, action, new_state);
          new_state->SetHP(new_state->GetHP() - hpcost);
          return new_state;
      }
      new_state->SetX(new_x);
      new_state->SetY(new_y);
      new_state->SetZ(state->GetZ());
      std::cout << "Is it ever not in there" << state->GetCharge() << std::endl;
      return new_state;
    }
      
    bool Problem::GoalTest(const ai::Search::State * const state_in) const
    {
      const State * const state = dynamic_cast<const State * const>(state_in);
      if(mUnvisitedGoal){
          return !state->GetCell()->visited;
      }
      else {
//      bool test = (*(this->goal_state) == *state);
//      std::cout << "Goal Test: " <<test  << " " << state->GetX() << " " << state->GetY() << " " << std::flush << std::endl;
        return *(this->goal_state) == *state;              
        }
        return false;
    }
    
    double Problem::StepCost(const ai::Search::State  * const state1_in,const ai::Search::Action * const action_in,const ai::Search::State  * const state2_in) const
    {
      const State  * const state1 = dynamic_cast<const State * const>(state1_in);
      const Action * const action = dynamic_cast<const Action * const>(action_in);
      const State  * const state2 = dynamic_cast<const State * const>(state2_in);
      int i = action->GetType();
      double elevationchange = state2->GetZ() - state1->GetZ(); //new minus original
      double step = 1.0;
      step += elevationchange / 1000.0;
      if ((state1->mCell->msnorth == "mud" && i == 0) || (i == 1 && state1->mCell->mssouth == "mud") || (i == 2 && state1->mCell->mseast == "mud") || (i == 3 && state1->mCell->mswest == "mud"))
      {   
          step += 1.0;
      }
      else if ((state1->mCell->msnorth == "rocks" && i == 0) || (i == 1 && state1->mCell->mssouth == "rocks") || (i == 2 && state1->mCell->mseast == "rocks") || (i == 3 && state1->mCell->mswest == "rocks"))
      {   
          step += 1.0;
      }
      return step;
    }
    double Problem::HPCost(const ai::Search::State  * const state1_in,const ai::Search::Action * const action_in,const ai::Search::State  * const state2_in) const
    {
      const State  * const state1 = dynamic_cast<const State * const>(state1_in);
      const Action * const action = dynamic_cast<const Action * const>(action_in);
      const State  * const state2 = dynamic_cast<const State * const>(state2_in);
      int i = action->GetType();
      double elevationchange = state2->GetZ() - state1->GetZ(); //new minus original
      double step = 0.0;
      step += elevationchange / 1000.0;
      step = 0.0;
      if ((state1->mCell->msnorth == "rocks" && i == 0) || (i == 1 && state1->mCell->mssouth == "rocks") || (i == 2 && state1->mCell->mseast == "rocks") || (i == 3 && state1->mCell->mswest == "rocks"))
      {   
          step += 2.0;
      }
      return step;
    }
    
    double Problem::Heuristic(const ai::Search::State  * const state_in) const
    {
        const State  * const state1 = dynamic_cast<const State * const>(state_in);
        double x = state1->GetX();
	double y = state1->GetY();
       double z = state1->GetZ();
	double gY = goal_state->GetY();
	double gX = goal_state->GetX();
        double gZ = goal_state->GetZ();
	double dist = (fabs(gX - x) + fabs(y-gY) + (gZ - z))/1000;
//        std::cout << "heuristic: " << gY << " " << gX << " xyz: " << x << " " << y << " " << z << " " << dist << " = distance " << std::endl;
	return dist;
    }
      
    bool Problem::SetGoal(State *goal_state_in)
    {
      this->goal_state = goal_state_in;
      return true;
    }
  }
}