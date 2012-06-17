# $* is prefix shared by target and dependent;  $@ is name of target file

all: MedReseq

CFLAGS = -c -O3 -g -D__USE_FIXED_PROTOTYPES__ -Isamtools-0.1.18 -Iprimer3-2.3.4/src 
OBJS = main.o sequenceRegions.o samWrapper.o primer3wrapper.o
NAME = MedReseq

$(NAME): $(OBJS)
	g++ -o $@ $(OBJS) -g -Llib -Lsrc -lbam -lprimer3 -ldpal -loligotm -lthal -lboulder -lz -lm -Wall -Wl

main.o:
	g++ $(CFLAGS) $ src/main.cpp -o $@
	
sequenceRegions.o:
	g++ $(CFLAGS) $ src/SequenceRegions.cpp -o $@
	
samWrapper.o:
	g++ $(CFLAGS) $ src/SamtoolsWrapper.cpp -o $@

primer3wrapper.o:
	g++ $(CFLAGS) $ src/Primer3Wrapper.cpp -o $@
	
clean:
	rm *.o
