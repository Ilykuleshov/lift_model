#include "lift.hh"

lift::lift(ticker& chronos, dispatcher& disp, const LIFTSPEC specs) :
    timed_obj(chronos, false),
    disp_(disp),
    specs_(specs),
    ord_({0, NONE}),
    doors_open_(false),
    floor_(0),
    idle_time_(0),
    dir_(0),
    passengers()
{}

void lift::step()
{
    //If dir is 0, then lift is empty, close doors & check for orders
    if (dir_ == NONE) 
    {
        if (doors_open_)
        {
            wait(specs_.T_idle_, WAIT_FOR);
            close_doors();
        }

        //Cycle, waiting for orders
        idle_time_ = 0;
        while (!ord_)
        {
            idle_time_++;
            wait(1, WAIT_FOR);
        }

        idle_time_ = -1;
        dir_ = FLAG_TO_DIR[ord_.dir];
    }

    //Process leavers
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
    if (disp_.check_ord({floor_, dir_to_flag(dir_)}))
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
    if (disp_.check_ord(ord_))
    {
        ord_.floor = -1;
        ord_.dir = NONE;
    }

    //If there are passengers, fullfill their targets
    if (!passengers.empty()) mov();
    //Else move to order, if ther is one
    else if (ord_)
    {
        setdir(ord_.floor);
        mov();
    }
    //Otherwise we're idle
    else dir_ = 0;
}