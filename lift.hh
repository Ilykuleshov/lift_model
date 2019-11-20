#pragma once
#include "dispatcher.hh"

struct LIFTSPEC
{
    int MAX_;
    int T_stage_;
    int T_open_;
    int T_idle_;
    int T_in;
    int T_out;
};

class lift : private timed_obj
{
private:
    dispatcher& disp_;
    const LIFTSPEC specs_;

    int ord_floor_;
    int ord_dir_;

    bool doors_open_;
    int floor_;
    int idle_time_;
    direction dir_;
    std::multiset<person> passengers;

    bool shutdown_;

    //Makes sure doors are open
    inline void open_doors()
    { if (doors_open_) wait(specs_.T_open_, wait_type::WAIT_FOR); }

    //Makes sure doors are closed
    inline void close_doors()
    { if (!doors_open_) wait(specs_.T_open_, wait_type::WAIT_FOR); }

    //Moves elevator in direction dir_
    inline void mov()
    { 
        close_doors();
        if (dir_ != 0) wait(specs_.T_stage_, wait_type::WAIT_FOR);
        floor_ += dir_;
    }

    inline void setdir(int floor)
    { dir_ = sgn(floor - floor_); }

    //Cycle step: from state <Lift arrived on floor "floor", doors closed>:
    void step();

public:
    lift(ticker& chronos, dispatcher& disp, const LIFTSPEC specs);

    //Give the lift an order
    void order(int floor, direction dir);

    //Is the lift currently idle?
    inline bool idle() const
    { return dir_ == 0; }

    inline void shutdown()
    { shutdown_ = 0; }

    inline int distance(int floor) const
    { return std::abs(floor_ - floor); }
};