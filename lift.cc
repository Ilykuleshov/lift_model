#include "lift.hh"

void lift::step()
{
    printf("!!Lift step: order %d\n", ord_.dir);
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
            if (shutdown_) return;
            idle_time_++;
            printf("IDLE, WAITING (ord dir %d)\n", (int) ord_.dir);
            wait(1, WAIT_FOR);
        }

        printf("GOT ORDER %d\n", (int) ord_.dir);
        idle_time_ = -1;
        dir_ = FLAG_TO_DIR[ord_.dir];
    }

    printf("LIFT NON IDLE\n");

    //Process leavers
    printf("PEOPLE LEAVING (floor %d):\n", floor_);
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
        printf("Ppl entering (floor %d)...\n", floor_);
        open_doors();
        std::multiset<person> newcomers = disp_.clear_floor(floor_, dir_);

        for (auto i = newcomers.begin(); i != newcomers.end() && passengers.size() < specs_.MAX_; i++)
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