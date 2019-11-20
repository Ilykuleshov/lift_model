OBJS = ${patsubst %.cc,%.o,$(wildcard *.cc)}
CXX = g++-8
CXXFLAGS = -fsanitize=address -Werror -std=c++17 -g
LINKERFLAGS = 

.PHONY = clean

lift: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LINKERFLAGS)

clean:
	rm -f *.o