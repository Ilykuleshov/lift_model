#pragma once
#include "dispatcher.hh"

struct LIFTSPEC
{
    int MAX;
    int T_stage;
    int T_open;
    int T_idle;
    int T_close;
    int T_in;
    int T_out;
};

#define printf(str, ...) printf(ANSI_COLOR_CYAN str __VA_OPT__(,) __VA_ARGS__)

class lift : public timed_obj
{
private:
    dispatcher& disp_;
    const LIFTSPEC specs_;

    order ord_;

    bool doors_open_ = false;
    int floor_ = 0;
    int idle_time_ = 0;
    direction dir_ = NONE;
    std::multiset<person> passengers;

    //Makes sure doors are open
    inline void open_doors()
    { 
        if (!doors_open_) 
        {
            printf("Opening doors...\n");
            wait(specs_.T_open, wait_type::WAIT_FOR);
            doors_open_ = true;
            printf("Doors open!\n");
        }
    }

    //Makes sure doors are closed
    inline void close_doors()
    { 
        if (doors_open_)
        {
            printf("Closing doors...\n");
            wait(specs_.T_close, wait_type::WAIT_FOR);
            doors_open_ = false;
            printf("Doors closed!\n");
        }
    }

    //Moves elevator in direction dir_
    inline void mov()
    { 
        close_doors();
        if (dir_ != 0) 
        {
            printf("Moving %d...\n", dir_);
            wait(specs_.T_stage, wait_type::WAIT_FOR);
            printf("Moved!\n");
        }
        floor_ += dir_;
    }

    inline void setdir(int floor)
    { dir_ = sgn(floor - floor_); }

    //Cycle step: from state <Lift arrived on floor "floor", doors closed>:
    virtual void step();

public:
    inline lift(ticker& chronos, dispatcher& disp, const LIFTSPEC specs) :
        timed_obj(chronos, false),
        disp_(disp),
        specs_(specs),
        ord_()
    {}

    //Give the lift an order
    inline void give_order(order ord)
    { if (!ord_) ord_ = ord; }

    //Is the lift currently idle?
    inline bool idle() const
    { return idle_time_ != -1; }

    inline int distance(int floor) const
    { return std::abs(floor_ - floor); }
};

#undef printf