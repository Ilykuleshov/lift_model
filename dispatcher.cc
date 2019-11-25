#include "lift.hh"
#include <algorithm>

#define printf(str, ...) printf(ANSI_COLOR_MAGENTA str __VA_OPT__(,) __VA_ARGS__)

std::multiset<person> dispatcher::clear_floor(int floor, direction dir)
{
    std::multiset<person> res;
    if (dir_to_flag(dir) & ord_log_[floor] == NONE) //There was no order on this floor!
    { return res; }

    auto& flr = floors_[floor];

    printf("Lift on floor [%d]: %d ppl, order dir: %d | lift dir %d\n", floor, (int) flr.size(), ord_log_[floor], dir);
    for (auto& i : flr) printf("Person going to %d\n", *i);

    if (dir == -1) //Going down
        while(!flr.empty() && (**(flr.begin())) < floor)  //Get ppl w\ smallest dst (start of sorted container)
            res.insert(std::move(flr.extract(flr.begin()).value()));

    else if (dir == 1) //Going up
        while(!flr.empty() && (**(--flr.end())) > floor) //Get ppl w\ largest dst (end of sorted container)
            res.insert(std::move(flr.extract(--flr.end()).value()));

    ord_log_[floor] = (direction_flag)(ord_log_[floor] - dir_to_flag(dir)); //Order is done

    return res;
}

void dispatcher::clear_done_ords()
{
    if (ord_queue_.empty()) return;
    for (order customer = ord_queue_.front(); !check_ord(customer); ord_queue_.pop()) 
    {
        if (ord_queue_.empty()) return;
        customer = ord_queue_.front();
    }
}

void dispatcher::try_order_lift()
{
    clear_done_ords();
    if (ord_queue_.empty()) return;
    order customer = ord_queue_.front();

    //Lambda-comparer:
    //idle lifts < busy lifts;
    //closer idle < farther idle;
    //closer busy < farther busy
    auto liftdistcomp = [src=customer.floor](const lift& a, const lift& b) -> bool 
    { 
        if (a.idle() == b.idle()) return a.distance(src) < b.distance(src);
        
        if (a.idle()) return true;
        else return false;
    };

    //Find closest lift
    auto closest_lift = std::min_element(lift_vec_.begin(), lift_vec_.end(), liftdistcomp);

    //If it's idle, order it && remove order
    if (closest_lift->idle()) 
    {
        ord_queue_.pop();
        closest_lift->give_order(customer);
    }
}

void dispatcher::check_timeline()
{
    int cur_time = get_time();
    if (timeline_.empty()) return;
    printf("Check timeline: %d ?= %d\n", cur_time, timeline_.front().time);
    while (!timeline_.empty() && timeline_.front().time == cur_time)
    {
        event customer = timeline_.front();
        timeline_.pop();
        printf("New customer (%d -> %d)\n", customer.src, customer.dst);

        direction_flag customer_dir = dir_to_flag(sgn(customer.dst - customer.src));
        floors_[customer.src].emplace(std::make_unique<int>(customer.dst));
        ord_log_[customer.src] = ord_log_[customer.src] | customer_dir;
        ord_queue_.push({ customer.src, dir_to_flag(sgn(customer.dst - customer.src))});
    }
}

void dispatcher::step()
{
    printf("!Dispatcher step! (%d orders)\n", ord_queue_.size());
    if (timeline_.empty() && ord_queue_.empty())
    {
        printf("Start shutdown process\n");
        for (auto& i : lift_vec_) 
        {
            printf("Wait for lifts\n");
            //Wait for lift to finish & shutdown them
            while (!i.idle()) wait(1, WAIT_FOR);
            i.shutdown();
            printf("Lifts all idle\n");
        }

        printf("Shutdown\n");
        shutdown();
        return;
    }

    //Check, if next customer from timeline has arrived
    check_timeline();
    try_order_lift();

    printf("Cycle end\n");
    wait(1, WAIT_FOR);
}

#undef printf