#include "lift.hh"

lift::lift(ticker& chronos, dispatcher& disp, const LIFTSPEC specs) :
    timed_obj(chronos, false),
    disp_(disp),
    specs_(specs),
    ord_floor_(-1),
    ord_dir_(0),
    doors_open_(false),
    floor_(0),
    idle_time_(0),
    dir_(0),
    passengers()
{
    //Lift 'til you drop
    while(!shutdown_)
        step();
}

void lift::step()
{
    //If dir is 0, then lift is idle, skip turn and close doors if needed
    if (dir_ == 0) 
    {
        if (idle_time_ == specs_.T_idle_) close_doors();
        idle_time_++;
        wait(1, WAIT_FOR);
        return;
    }

    //Process leavers
    int leaving = 0;
    for (auto i = passengers.begin(); i != passengers.end();) 
    {
        if (**i == floor_) 
        {
            open_doors();
            wait(specs_.T_out, WAIT_FOR);
            i = passengers.erase(i);
        }
        else i++;
    }

    //Check ppl on that floor going in needed direction (through dispatcher).
    if (disp_.check_ord(floor_, dir_to_flag(dir_)))
    {
        open_doors();
        std::multiset<person> newcomers = disp_.clear_floor(floor_, dir_);

        for (auto i = newcomers.begin(); i != newcomers.end() && passengers.size() < specs_.MAX_; i++)
        {
            person newcomer = std::move(newcomers.extract(i).value());
            wait(specs_.T_in, WAIT_FOR);
            passengers.insert(std::move(newcomer));
        }
        //For now - everyone who didn't fit dies)
    }

    //The order could have been completed by now
    if (disp_.check_ord(ord_floor_, dir_to_flag(ord_dir_)))
    {
        ord_floor_ = -1;
        ord_dir_ = 0;
    }

    //If there are passengers, fullfill their targets
    if (!passengers.empty()) mov();
    //Else move to order, if ther is one
    else if (ord_dir_ != 0)
    {
        setdir(ord_floor_);
        mov();
    }
    //Otherwise we're idle
    else dir_ = 0;
}

void lift::order(int floor, direction dir)
{
    ord_floor_ = floor;
    ord_dir_ = dir;
    idle_time_ = 0;
}