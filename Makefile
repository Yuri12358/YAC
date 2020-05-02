d_src=src/
d_inc=include/
d_hdr=$(d_inc)YAC/
d_obj=obj/
d_bin=bin/

flags=-c -g
headers=-I$(d_inc)
objs=$(patsubst $(d_src)%.cpp,$(d_obj)%.o,$(wildcard $(d_src)*))
bin=$(d_bin)yac

all: $(bin)

$(bin): $(objs)
	g++ $(objs) -o $@

$(d_obj)main.o: $(d_src)main.cpp $(d_obj)yac.o
	g++ $(flags) $< -o $@ $(headers)

$(d_obj)yac.o: $(d_src)yac.cpp $(d_hdr)yac.hpp $(d_obj)compressor.o
	g++ $(flags) $< -o $@ $(headers)

$(d_obj)compressor.o: $(d_src)compressor.cpp $(d_hdr)compressor.hpp
	g++ $(flags) $< -o $@ $(headers)

$(d_obj)extractor.o: $(d_src)extractor.cpp $(d_hdr)extractor.hpp
	g++ $(flags) $< -o $@ $(headers)

clean:
	-rm $(bin) $(objs)

