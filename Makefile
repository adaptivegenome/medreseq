#/*********************************************************************
# *
# * Makefile:  The makefile for the Medical resequencing project.
# *
# * Author: Sunil Kamalakar, VBI
# * Last modified: 22 June 2012
# *
# *********************************************************************
# *
# * This file is released under the Virginia Tech Non-Commercial
# * Purpose License. A copy of this license has been provided in
# * the Medical Re-sequencing root directory.
# *
# *********************************************************************/

all: MedReseq

#define constants
CPP        = g++
O_OPTS     = -O3
CC_OPTS    = -c -g -Wall
INCLUDES   = -Isamtools -Iprimer3/src
LIBS	   = -Lprimer3/src -Lsamtools -lbam -lprimer3 -ldpal -loligotm -lthal -lboulder -lz -lm

CFLAGS  = $(CC_OPTS) $(O_OPTS)

OBJS = utility.o  config.o  sequenceRegions.o vcfExtractor.o sequenceAdapter.o samWrapper.o primer3wrapper.o  main.o
NAME = MedReseq

$(NAME): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(LIBS)
	
utility.o:
	$(CPP) $(CFLAGS) $(INCLUDES) $ src/Utility.cpp -o $@

config.o:
	$(CPP) $(CFLAGS) $(INCLUDES) $ src/ConfigurationLoader.cpp -o $@
	
sequenceRegions.o:
	$(CPP) $(CFLAGS) $(INCLUDES) $ src/SequenceRegions.cpp -o $@

vcfExtractor.o:
	$(CPP) $(CFLAGS) $(INCLUDES) $ src/VCFAdapter.cpp -o $@
	
sequenceAdapter.o:
	$(CPP) $(CFLAGS) $(INCLUDES) $ src/SequencesAdapter.cpp -o $@
	
samWrapper.o:
	$(CPP) $(CFLAGS) $(INCLUDES) $ src/SamtoolsWrapper.cpp -o $@

primer3wrapper.o:
	$(CPP) $(CFLAGS) $(INCLUDES) $ src/Primer3Wrapper.cpp -o $@

main.o:
	$(CPP) $(CFLAGS) $(INCLUDES) $ src/main.cpp -o $@
	
clean:
	rm *.o $(NAME)
