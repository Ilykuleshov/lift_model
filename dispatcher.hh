#include <vector>
#include <set>

#include "ticker.hh"

class lift;

enum direction
{
    NONE = 0b00,
    UP   = 0b01,
    DOWN = 0b10
};

#define DEFFLAGOP(OP, AMPR) \
inline direction ## AMPR operator ## OP (direction ## AMPR a,  direction b) \
{ return (direction ## AMPR)((int ## AMPR)a OP (int) b); }

DEFFLAGOP(|,)
DEFFLAGOP(&,)
DEFFLAGOP(|=,&)
DEFFLAGOP(&=,&)

#undef DEFFLAGOP

enum personstate
{
    WAITING = 0,
    LIFTING = 1
};

struct person_data
{
    int dst;
    personstate status;
};

typedef std::unique_ptr<person_data> person;

struct event
{
    int time;
    person dude;
};

class dispatcher : private timed_obj
{
private:
    std::vector<lift> lift_vec_;
    std::vector<std::list<person>> floors_;
    std::vector<event> customer_timeline_; //sorted by time - smallest first
    std::vector<direction> ord_log_;

    //Wait until newcomer.time
    //Determine order direction & floor, place order
    //Assign closest lift to order
    void service(event newcomer);

public:
    //sort timeline & start cycle
    dispatcher(ticker& chronos, std::vector<event> orders); 

    inline bool check_ord(int floor, direction dir)
    { return (ord_log_[floor] | dir) != direction::NONE; }

    //Check floor for people going in specified direction, move them to multiset & return
    std::multiset<person> clear_floor(int floor, direction dir);
};