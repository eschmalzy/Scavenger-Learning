#include "Manual.h"
#include <cstdio>

namespace ai
{
  namespace Sample
  {

    Manual::Manual()
    {
      SetName("Manual");
    }
    
    Manual::~Manual()
    {
    }
    
    ai::Agent::Action * Manual::Program(const ai::Agent::Percept * percept)
    {
      ai::Scavenger::Action *action = new ai::Scavenger::Action;
      unsigned int i;
      
      std::cout << std::endl;
      std::cout << "Percepts:" << std::endl;
      for(i = 0; i < percept->NumAtom(); i++)
	{
	  ai::Agent::PerceptAtom a = percept->GetAtom(i);
	  std::cout << a.GetName() << ": " << a.GetValue() << std::endl;
	}
      std::cout << "Commands:" << std::endl;
      std::cout << "go north|south|east|west" << std::endl;
      std::cout << "look north|south|east|west" << std::endl;
      std::cout << "pickup|drop|deposit|examine object_id" << std::endl;
      std::cout << "recharge" << std::endl;
      std::cout << "quit" << std::endl;
      std::cout << "noop" << std::endl;
      std::cout << "Action? ";
      bool done = false;
      while(!done)
	{
	  char buf[128];
	  char dir[128];
	  std::cin.getline(buf, 128);
	  done = true;
	  if(std::sscanf(buf, "go %s", dir) == 1)
	    {
	      if(std::strcmp(dir, "north") == 0)
		{
		  action->SetCode(ai::Scavenger::Action::GO_NORTH);
		}
	      else if(std::strcmp(dir, "south") == 0)
		{
		  action->SetCode(ai::Scavenger::Action::GO_SOUTH);
		}
	      else if(std::strcmp(dir, "east") == 0)
		{
		  action->SetCode(ai::Scavenger::Action::GO_EAST);
		}
	      else if(std::strcmp(dir, "west") == 0)
		{
		  action->SetCode(ai::Scavenger::Action::GO_WEST);
		}
	      else
		{
		  std::cout << "Unknown direction for go" << std::endl;
		  done = false;
		}
	    }
	  else if(std::sscanf(buf, "look %s", dir) == 1)
	    {
	      if(std::strcmp(dir, "north") == 0)
		{
		  action->SetCode(ai::Scavenger::Action::LOOK);
		  action->SetDirection(ai::Scavenger::Location::NORTH);
		}
	      else if(std::strcmp(dir, "south") == 0)
		{
		  action->SetCode(ai::Scavenger::Action::LOOK);
		  action->SetDirection(ai::Scavenger::Location::SOUTH);
		}
	      else if(std::strcmp(dir, "east") == 0)
		{
		  action->SetCode(ai::Scavenger::Action::LOOK);
		  action->SetDirection(ai::Scavenger::Location::EAST);
		}
	      else if(std::strcmp(dir, "west") == 0)
		{
		  action->SetCode(ai::Scavenger::Action::LOOK);
		  action->SetDirection(ai::Scavenger::Location::WEST);
		}
	      else
		{
		  std::cout << "Unknown direction for look" << std::endl;
		  done = false;
		}
	    }
	  else if(std::strcmp(buf, "recharge") == 0)
	    {
	      action->SetCode(ai::Scavenger::Action::RECHARGE);
	    }
	  else if(std::sscanf(buf, "pickup %s", buf) == 1)
	    {
	      action->SetCode(ai::Scavenger::Action::PICKUP);
	      action->SetObjectId(buf);
	    }
	  else if(std::sscanf(buf, "drop %s", buf) == 1)
	    {
	      action->SetCode(ai::Scavenger::Action::DROP);
	      action->SetObjectId(buf);
	    }
	  else if(std::sscanf(buf, "deposit %s", buf) == 1)
	    {
	      action->SetCode(ai::Scavenger::Action::DEPOSIT);
	      action->SetObjectId(buf);
	    }
	  else if(std::sscanf(buf, "examine %s", buf) == 1)
	    {
	      action->SetCode(ai::Scavenger::Action::EXAMINE);
	      action->SetObjectId(buf);
	    }
	  else if(std::strcmp(buf, "quit") == 0)
	    {
	      action->SetCode(ai::Scavenger::Action::QUIT);
	    }
	  else if(std::strcmp(buf, "noop") == 0)
	    {
	      action->SetCode(ai::Scavenger::Action::NOOP);
	    }
	  else
	    {
	      std::cout << "Unknown command" << std::endl;
	      done = false;
	    }
	}
      
      std::cout << std::endl << std::endl;
      
      return action;
    }
  }
}
