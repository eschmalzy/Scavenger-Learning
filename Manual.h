#ifndef _MANUAL_H_
#define _MANUAL_H_

#include <ai_scavenger.h>

namespace ai
{
  namespace Sample
  {
    class Manual : public ai::Agent::AgentProgram
    {
    public:
      Manual();
      ~Manual();
      virtual ai::Agent::Action * Program(const ai::Agent::Percept * percept);
    protected:
    private:
    };
  }
}

#endif /* _MANUAL_H_ */
