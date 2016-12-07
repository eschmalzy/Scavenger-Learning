
/* Example percepts and values.
 * These are all of the percepts needed for the
 * ScavengerSearch assignment.
 * Charge remaining for your agent.
 CHARGE: 100.00
 * Hit Points (health) remaining for your agent.
 HP: 100.00
 * X location of your agent (in meters)
 X_LOC: 0.00
 * Y location of your agent (in meters)
 Y_LOC: 0.00
 * Elevation of your agent (in meters)
 Z_LOC: 0.00
 * Unique identifier of your agent's base.  Not needed.
 BASE: 0x1dfd8c0
 * Example cell definition.  Notice that the percept
 * name is "CELL_" followed by numbers.  The numbers
 * are the cell's ID.  You will receive such a percept
 * for every cell.  You'll need to parse the ID from
 * the name, and the values from the value string.
 * The values are x,y,z,north,south,east,west.
 * The direction values are the type of interface.
 * You can expect: wall, cliff, rocks, mud, plain.
 CELL_28: 0.000000,0.000000,0.000000,wall,wall,wall,wall
 * x,y,z of the desired location for your agent.
 GOAL_LOC: 0.000000,0.000000,0.000000
*/

/* String processing:
 *
 */
void dummy_function()
{
  ai::Agent::PerceptAtom a = percept->GetAtom(i);
  /* strncmp looks at the first n characters for equality.  Returns 0 if equal. */
  /* .c_str() extracts a c-string pointer from a std::string. */
  if(std::strncmp(a.GetName().c_str(), "CELL_", 5) == 0)
    {
      /* sscanf looks inside of a string, given a format string and extracts values. */
      std::sscanf(a.GetName().c_str(), "CELL_%d", &id);
      char x_str[16],y_str[16],z_str[16],n_str[16],s_str[16],e_str[16],w_str[16];
      double x,y,z;
      std::sscanf(a.GetValue().c_str(), "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]",
                  x_str, y_str, z_str, n_str, s_str, e_str, w_str);
      x = std::atof(x_str);
      y = std::atof(y_str);
      z = std::atof(z_str);
    }
  else if(std::strcmp(a.GetName().c_str(), "CHARGE") == 0)
    {
      double charge = std::atof(a.GetValue().c_str());
    }
  /* etc... */

}
