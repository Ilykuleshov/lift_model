#pragma once

#include <vector>
#include <set>
#include <queue>

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
#define DEFFLAGOP(OP) \
inline direction_flag operator OP (const direction_flag a,  const direction_flag& b) \
{ return (direction_flag)((int)a OP (int) b); }

DEFFLAGOP(|)
DEFFLAGOP(&)
DEFFLAGOP(^)

#undef DEFFLAGOP

//Actual lift direction type (-1, 0, +1)
typedef int direction;

//Converter
inline direction_flag dir_to_flag(const direction& that)
{
    static const std::array<direction_flag, 3> DIR_TO_FLAG = 
    {
        direction_flag::DOWN, 
        direction_flag::NONE, 
        direction_flag::UP
    };

    return DIR_TO_FLAG[that + 1];
}

const std::array<int, 3> FLAG_TO_DIR = { 0, 1, -1 };

inline int sgn(int val)
{ return (0 < val) - (val < 0); }

//Person type for better code
typedef std::unique_ptr<int> person;

inline bool operator<(const person& a, const person& b)
{ return *a < *b; }

struct order
{
    int floor;
    direction_flag dir;

    inline operator bool()
    { return dir != NONE; }
};

struct event
{
    int src;
    int dst;
    int time;
};

class dispatcher : public timed_obj
{
private:
    std::vector<lift> lift_vec_;
    std::vector<std::multiset<person>> floors_;
    std::vector<direction_flag> ord_log_;
    std::queue<order> ord_queue_;

    std::queue<event> timeline_;

    //Remove finished orders from the front of the queue
    void clear_done_ords();

    //Try to order a lift front of ord_queue. Do nothing, if no lifts availible
    void try_order_lift();
    void check_timeline();

    virtual void step();

public:
    //It is implied, that timeline is sorted (lowest time first)
    dispatcher(ticker& chronos, std::queue<event> timeline) :
        timed_obj(chronos, true),
        timeline_(timeline)
    {}

    inline bool check_ord(order ord)
    { return (ord_log_[ord.floor] | ord.dir) != direction_flag::NONE; }

    //Check floor for people going in specified direction, move them to multiset & return
    std::multiset<person> clear_floor(int floor, direction dir);
};