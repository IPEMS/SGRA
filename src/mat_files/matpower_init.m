% needs to be called by C++ program before power flows can be run

define_sim_constants;
define_constants;

CASE = loadcase(CASE_FILE);
