#include <ai_agent.h>

int main(int argc, char **argv)
{
  ai::Agent::AgentDriver d(argc, argv);
  d.Run();
  return 0;
}
