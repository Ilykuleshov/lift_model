#include <vector>
#include <set>

#include "ticker.hh"

class lift;

//Direction flag - for economic floor-order storage
enum direction_flag
{
    NONE = 0b00,
    UP   = 0b01,
    DOWN = 0b10
};

//Define flag operations for direction_flag
#define DEFFLAGOP(OP, AMPR) \
inline direction_flag ## AMPR operator ## OP (direction_flag ## AMPR a,  direction_flag b) \
{ return (direction_flag ## AMPR)((int ## AMPR)a OP (int) b); }

DEFFLAGOP(|,)
DEFFLAGOP(&,)
DEFFLAGOP(|=,&)
DEFFLAGOP(&=,&)

#undef DEFFLAGOP

//Actual lift direction type (-1, 0, +1)
typedef int direction;

//Converter
direction_flag dir_to_flag(const direction& that)
{
    static const std::array<direction_flag, 3> DIR_TO_FLAG = {DOWN, NONE, UP};
    return DIR_TO_FLAG[that + 1];
}

//Person is the floor he needs, yet he can't be copied
typedef std::unique_ptr<int> person;

inline bool operator<(const person& a, const person& b)
{ return *a < *b; }

struct event
{
    int time;
    int src;
    int dst;
};

class dispatcher : private timed_obj
{
private:
    std::vector<lift> lift_vec_;
    std::vector<std::list<person>> floors_;
    std::vector<event> timeline_; //sorted by time - smallest first
    std::vector<direction_flag> ord_log_;

    //Wait until newcomer.time
    //Determine order direction & floor, place order
    //Assign closest lift to order
    void service(event newcomer);

public:
    //sort timeline & start cycle
    dispatcher(ticker& chronos, std::vector<event> orders); 

    inline bool check_ord(int floor, direction_flag dir)
    { return (ord_log_[floor] | dir) != direction_flag::NONE; }

    //Check floor for people going in specified direction, move them to multiset & return
    std::multiset<person> clear_floor(int floor, direction dir);
};