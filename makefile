OBJS = ${patsubst %.cc,%.o,$(wildcard *.cc)}
DEPS = ${wildcard *.hh}
.SUFFIXES: .hh
CXX = g++-8
CXXFLAGS = -fsanitize=address -Werror -std=c++17 -g
LINKERFLAGS = 

.PHONY = clean

lift: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LINKERFLAGS)

%.o : %.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o