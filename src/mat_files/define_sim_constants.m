%Defines constants for the C++/Octave interface
% LOAD_POINTS 		- (1,N_lp) matrix of buses that are load points
% NUM_LOAD_POINTS 	- the number of load points in the system
% CASE_FILE		- the MATPOWER case file to load

LOAD_POINTS = zeros(1,48-22);

for x=1:(48-22)
	LOAD_POINTS(1,x) = 21 + x;
end

NUM_LOAD_POINTS = size(LOAD_POINTS)(2);
CASE_FILE = 'rbts_bus5_phase_a';




TEST_LOADS = [
	10	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5;
	0.75	0.75	0.75	0.5	0.5	0.5	0.5	0.75	0.75	0.5	0.5	0.5	0.5	0.5	0.5	0.75	0.75	0.75	0.5	0.5	0.5	0.5	0.5	0.5	0.5	0.5;
];
