function save_power_flow(load_profile,folder)
	define_sim_constants;
	define_constants;
	pf_case = loadcase(CASE_FILE);

	TIME_POINTS = size(load_profile)(1);

	%OPEN FILES
	mkdir(folder);
	PASS = fopen(strcat(folder,'pass.txt'),'wt');
	
	%BUS FILES
	mkdir(strcat(folder,'bus/'));
	BUS_P = fopen(strcat(folder,'bus/P.txt'),'wt');
	BUS_Q = fopen(strcat(folder,'bus/Q.txt'),'wt');
	BUS_V = fopen(strcat(folder,'bus/V.txt'),'wt');
	BUS_A = fopen(strcat(folder,'bus/a.txt'),'wt');

	%GEN FILES
	mkdir(strcat(folder,'gen/'));
	GEN_P = fopen(strcat(folder,'gen/P.txt'),'wt');
	GEN_Q = fopen(strcat(folder,'gen/Q.txt'),'wt');

	%BRN FILE
	mkdir(strcat(folder,'branch/'));
	BRN_S = fopen(strcat(folder,'branch/S.txt'),'wt');

	for t = 1:TIME_POINTS
		_out = _local_runpf(pf_case,load_profile(t,:));
		success = _test_pf(_out);

		% OUTPUT HERE
		fprintf(PASS,'%i\n',success);

		%BUS FILES
		fprintf(BUS_P,'%f,',_out.bus(:,PD));
		fprintf(BUS_Q,'%f,',_out.bus(:,QD));
		fprintf(BUS_V,'%f,',_out.bus(:,VM));
		fprintf(BUS_A,'%f,',_out.bus(:,VA));

		%GEN FILES
		fprintf(GEN_P,'%f,',_out.gen(:,PG));
		fprintf(GEN_Q,'%f,',_out.gen(:,QG));	

		%BRANCH FILE
		fprintf(BRN_S,'%f,',_get_S(_out.branch(:,PF), _out.branch(:,QF)));

		%END LINE
		fprintf(BUS_P,'\n');
		fprintf(BUS_Q,'\n');
		fprintf(BUS_V,'\n');
		fprintf(BUS_A,'\n');

		fprintf(GEN_P,'\n');
		fprintf(GEN_Q,'\n');

		fprintf(BRN_S,'\n');
	end

	%CLOSE FILES
	fclose(BUS_P);
	fclose(BUS_Q);
	fclose(BUS_V);
	fclose(BUS_A);

	fclose(GEN_P);
	fclose(GEN_Q);

	fclose(BRN_S);

	fclose(PASS);
end
