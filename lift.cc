#include "lift.hh"

#define printf(str, ...) printf(ANSI_COLOR_CYAN str __VA_OPT__(,) __VA_ARGS__)

void lift::step()
{
    printf("!Lift step! (floor=[%d], dir=[%d])\n", floor_, dir_);
    //If dir is 0, then lift is empty, close doors & check for orders
    if (dir_ == NONE) 
    {
        if (doors_open_)
        {
            printf("Waiting for idle time\n");
            wait(specs_.T_idle, WAIT_FOR);
            close_doors();
        }

        //Cycle, waiting for orders
        idle_time_ = 0;
        while (!ord_)
        {
            printf("Idle, waiting for orders\n");

            if (shutdown_)
            {
                printf("Lift shutdown\n");
                return;
            }

            idle_time_++;
            wait(1, WAIT_FOR);
        }

        printf("Order{ dir %d, floor %d }\n", (int) ord_.dir, ord_.floor);
        idle_time_ = -1;
        dir_ = FLAG_TO_DIR[ord_.dir];
    }

    //Process leavers
    for (auto i = passengers.begin(); i != passengers.end();) 
    {
        if (**i == floor_) 
        {
            open_doors();
            printf("Waiting for person to leave...\n");
            wait(specs_.T_out, WAIT_FOR);
            i = passengers.erase(i);
            printf("Person left!\n");
        }
        else i++;
    }

    //Check ppl on that floor going in needed direction (through dispatcher).
    if (disp_.check_ord({floor_, dir_to_flag(dir_)}))
    {
        printf("People entering (floor %d)...\n", floor_);
        open_doors();
        std::multiset<person> newcomers = disp_.clear_floor(floor_, dir_);

        for (auto i = newcomers.begin(); i != newcomers.end() && passengers.size() < specs_.MAX; i++)
        {
            person newcomer = std::move(newcomers.extract(i).value());
            printf("Person (%d->%d) going in...\n", floor_, *newcomer);
            wait(specs_.T_in, WAIT_FOR);
            printf("Went in!\n");
            passengers.insert(std::move(newcomer));
        }
        //For now - everyone who didn't fit dies)
    }
    else printf("No one entering\n");

    //The order could have been completed by now
    if (ord_.floor != -1 && !disp_.check_ord(ord_))
    {
        printf("Order completed\n");
        ord_.floor = -1;
        ord_.dir = NONE;
    }

    //If there are passengers, fullfill their targets
    if (!passengers.empty())
    {
        printf("Moving for passengers\n");
        mov();
    }
    //Else move to order, if ther is one
    else if (ord_)
    {
        printf("Moving towards order (floor %d, dir %d)\n", ord_.floor, ord_.dir);
        setdir(ord_.floor);
        mov();
    }
    //Otherwise we're idle
    else dir_ = 0;
}

#undef prinf