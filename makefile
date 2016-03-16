SOURCES=main.cpp CoV.cpp global/timers.cpp global/global_timer.cpp global/global_rand.cpp input.cpp timevector.cpp heuristics/ga.cpp assets/smartappliance.cpp customer.cpp aggregator.cpp psychology/IPsychology.cpp psychology/PsychRatio.cpp psychology/PsychAlpha.cpp uniformdecision.cpp global/global_utility.cpp

ifeq ($(mode),debug)
	CFLAGS=-c -g -O0 -Wall 
	OPT1=-g
	EXECUTABLE=sim_debug
else ifeq ($(mode),debug_fast)
	CFLAGS=-c -g
	OPT1=-g
	EXECUTABLE=sim_debug
else
	CFLAGS=-c -O3
	OPT1=
	EXECUTABLE=sim
endif

ifeq ($(pw),y)
	LINK=mkoctfile --link-stand-alone
	CFLAGS3=-D INCLUDE_POWERWORLD
	SOURCES+= octaveinterface.cpp
else
	LINK=g++
	CFLAGS3=
endif

ifeq ($(par),y)
	CFLAGS2=-D PAR -I /apps/gsl-1.15 #this will enable MPI and include the gsl library
	LDFLAGS2=
	CC=CC
	LINK=CC
else
	CFLAGS2=
	LDFLAGS2=-lboost_chrono -lboost_system
	CC=g++
endif 

LDFLAGS= -lgsl -lgslcblas -lm 
OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK) $(OPT1) $(OBJECTS) -o $@ $(LDFLAGS) $(LDFLAGS2)

.cpp.o:
	$(CC) $(CFLAGS) $(CFLAGS2) $(CFLAGS3) $< -o $@

clean:
	rm *.o
	rm global/*.o
	rm heuristics/*.o
	rm assets/*.o
	rm sim*
	
	
