
#include <ai_agent.h>

#include "Swampert.h"
#include "Manual.h"

namespace ai
{
  namespace Agent
  {
    AgentProgram *AgentProgramSelect(Options *opts)
    {
      AgentProgram *agent_program = 0;
      switch(opts->GetArgChr("agent"))
        {
        case 's':
          agent_program = new ems::Scavenger::Swampert(opts);
          break;
        case 'm':
          agent_program = new ai::Sample::Manual();
          break;

        default:
          std::cout << "Unknown agent program " << opts->GetArgChr("agent") << std::endl
                    << "s : Swampert" << std::endl
                    << "m : Manual" << std::endl
      
		    << std::endl;
          exit(1);
          break;
        }
      return agent_program;
    }
  }
}
