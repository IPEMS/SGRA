% takes as input a matrix of size (TIME_POINTS, NUM_LOAD_POINTS)
% returns 1 if successful, else 0

function success = run_power_flow(load_profile)
	define_sim_constants;
	define_constants;
	pf_case = loadcase(CASE_FILE);

	TIME_POINTS = size(load_profile)(1);

	success = 1;

	%make powerflow not output

	for t = 1:TIME_POINTS
		_out = _local_runpf(pf_case,load_profile(t,:));
		success = _test_pf(_out);

		if(success == 0)
			break;
		end
	end
end
