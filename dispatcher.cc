#include "lift.hh"
#include <algorithm>

std::multiset<person> dispatcher::clear_floor(int floor, direction dir)
{
    std::multiset<person> res;
    if (dir_to_flag(dir) & ord_log_[floor] == NONE) //There was no order on this floor!
    { return res; }

    auto& flr = floors_[floor];
    if (dir == -1) //Going down
        while(**(flr.begin()) < floor)  //Get ppl w\ smallest dst (start of sorted container)
            res.insert(std::move(flr.extract(flr.begin()).value()));

    else if (dir == 1)
        while(**(flr.rbegin()) > floor) //Get ppl w\ largest dst (end of sorted container)
            res.insert(std::move(flr.extract(std::prev(flr.end())).value()));

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

    while (timeline_.front().time == cur_time)
    {
        event customer = timeline_.front();
        timeline_.pop();

        floors_[customer.src].emplace(std::make_unique<int>(customer.dst));
        ord_log_[customer.src] = ord_log_[customer.src] | dir_to_flag(customer.dst);
        ord_queue_.push({ customer.src, dir_to_flag(sgn(customer.dst - customer.src))});
    }
}

void dispatcher::step()
{
    if (timeline_.empty())
    {
        for (auto& i : lift_vec_) 
        {
            //Wait for lift to finish & shutdown them
            while (i.idle()) wait(1, WAIT_FOR);
            i.shutdown();
        }
        shutdown();
    }

    //Check, if next customer from timeline has arrived
    check_timeline();

    if (!ord_queue_.empty()) 
    {
        order customer = ord_queue_.front();
        ord_queue_.pop();
    }

    wait(1, WAIT_FOR);
}