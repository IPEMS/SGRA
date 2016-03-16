////////////////////////////////////////////////////////////////////////////
//// INCLUDES
////////////////////////////////////////////////////////////////////////////
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <unistd.h>

//local includes
#include "global/global_rand.h"
#include "global/global_timer.h"

#include "input.h"

#include "assets/smartappliance.h"
#include "customer.h"
#include "aggregator.h"

//FOR OCTAVE
#ifdef INCLUDE_POWERWORLD
#include <octave-3.8.1/octave/oct.h>
#include <octave-3.8.1/octave/octave.h>
#include <octave-3.8.1/octave/parse.h>
#include <octave-3.8.1/octave/toplev.h>
#endif

#ifdef PAR
#include <omp.h>
#include <mpi.h>
//#include <cstdio>
#define MY_ROOT	0
#endif


using namespace std;

//search INDECES_PER_HOUR   == 4

////////////////////////////////////////////////////////////////////////////
//// LOCAL FUNCTION PROTOTYPES
////////////////////////////////////////////////////////////////////////////
//command line parameter functions
char * getCmdOption(char ** begin, char ** end, const string& option);
bool cmdOptionExists(char ** begin, char ** end, const string& option);

////////////////////////////////////////////////////////////////////////////
//// MAIN
////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[]){
#ifdef PAR
	int mysize, myid;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &mysize);
#endif

	////////////////////////////////////////////////////////////////////////
	//// LOCAL VARIABLES
	////////////////////////////////////////////////////////////////////////
	init_global_timer();

#ifndef PAR
	init_global_rand((long)(time(NULL) * getpid()));
#else
	init_global_rand((long)(time(NULL) * getpid() * myid));
#endif

	ifstream fs;
	string * in_fname;
	string out_folder = "";
	int numberOfTrials = 1;

	////////////////////////////////////////////////////////////////////////
	//// CHECK COMMAND LINE FLAGS/PARAMETERS
	////////////////////////////////////////////////////////////////////////
	//help
	if(cmdOptionExists(argv,argv+argc,"-h")){
		cout << "Usage:\n\t[-f inputfile]\t\t - required, xml input file\n\t[-n numberofruns]\t - optional, default 1\n\t[-o outfolder]" << endl;
		return 0;
	}

	//number of trials
	if(cmdOptionExists(argv,argv+argc,"-n")){
		numberOfTrials = atoi(getCmdOption(argv,argv+argc,"-n"));
	}

	//filename
	if(cmdOptionExists(argv,argv+argc,"-f")){
		in_fname = new string(getCmdOption(argv,argv+argc,"-f"));
	}else{
		cout << "Error: require input file.  -f [filename].  -h for help." <<endl;
		return 0;
	}

	//output folder
	if(cmdOptionExists(argv,argv+argc,"-o")){
		out_folder = string(getCmdOption(argv,argv+argc,"-o"));

		//make directory recursively
		stringstream ss;
		ss << "mkdir -p -m 0777 " << out_folder;
		int temp = system(ss.str().c_str());
	}else{
		cout << "Error: require output folder.  -o [folder].  -h for help." <<endl;
		return 0;
	}

	cout << "Number of Trials = " << numberOfTrials << endl;
	cout << "Input File: " << (*in_fname) << endl;
	cout << "Output Folder: " << out_folder << endl;

	////////////////////////////////////////////////////////////////////////
	//// BEGIN HERE
	////////////////////////////////////////////////////////////////////////
	fs.open((*in_fname).c_str(),ifstream::in);

	if(!fs.is_open()){
		cout << "File (" << *in_fname << ") failed to open, exiting." << endl;
		return 0;
	}

	int init_timer = start_new_global_timer("initialization");
	aggregator * agg = parseXmlFile(&fs);
	fs.close();

	stop_global_timer(init_timer);
	print_global_timer(init_timer);

	int run_timer = start_new_global_timer("runtime");
	agg->run_heuristics(out_folder);

	stop_global_timer(run_timer);
	print_global_timer(run_timer);

	save_global_timer(out_folder + "timings.txt");

	////////////////////////////////////////////////////////////////////////
	//// CLEAR MEMORY
	////////////////////////////////////////////////////////////////////////
	del_global_rand();
	del_global_timer();

	delete in_fname;
	delete agg;

#ifdef INCLUDE_POWERWORLD
	clean_up_and_exit(0); //safe exit
#else
	return 0;
#endif
}

////////////////////////////////////////////////////////////////////////////
//// LOCAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////
char * getCmdOption(char ** begin, char ** end, const string& option){
	char ** itr = find(begin,end,option);
	if(itr != end && ++itr != end){
		return *itr;
	}
	return NULL;
}

bool cmdOptionExists(char ** begin, char ** end, const string& option){
	return find(begin,end,option) != end;
}
