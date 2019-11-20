#pragma once

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

inline int sgn(int val)
{ return (0 < val) - (val < 0); }

//Person type for better code
typedef std::unique_ptr<int> person;

inline bool operator<(const person& a, const person& b)
{ return *a < *b; }

struct event
{
    int time;
    int src;
    int dst;
};

inline bool operator<(const event& a, const event& b)
{ return a.time < b.time; }

class dispatcher : private timed_obj
{
private:
    std::vector<lift> lift_vec_;
    std::vector<std::multiset<person>> floors_;
    std::vector<direction_flag> ord_log_;

    //Service a new customer
    void service(event customer);

public:
    //sort timeline & start cycle
    dispatcher(ticker& chronos, std::vector<event> orders);

    inline bool check_ord(int floor, direction_flag dir)
    { return (ord_log_[floor] | dir) != direction_flag::NONE; }

    //Check floor for people going in specified direction, move them to multiset & return
    std::multiset<person> clear_floor(int floor, direction dir);
};