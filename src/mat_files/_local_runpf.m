function _pf_out = _local_runpf(pf_case,loads)
	define_constants;
	define_sim_constants;

	for l = 1:size(loads)(2)
		pf_case.bus(LOAD_POINTS(l),PD) = loads(l);
	end

	_pf_out = runpf(pf_case, mpoption('VERBOSE', 0, 'OUT_ALL',0));

end
