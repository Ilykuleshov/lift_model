#include "dispatcher.hh"
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
}

void dispatcher::service(event customer)
{
    //Wait until newcomer.time
    wait(customer.time, wait_type::WAIT_TIL);

    //Lambda-comparer:
    //idle lifts < busy lifts;
    //closer idle < farther idle;
    //closer busy < farther busy
    auto liftdistcomp = [src=customer.src](const lift& a, const lift& b) -> bool 
    { 
        if (a.idle() == b.idle()) return a.distance(src) < b.distance(src);
        
        if (a.idle()) return true;
        else return false;
    };

    //Determine order direction
    direction dir = sgn(customer.dst - customer.src);

    //Order closest idle lift
    auto closest_lift = std::min_element(lift_vec_.begin(), lift_vec_.end(), liftdistcomp);
    if (closest_lift->idle()) closest_lift->order(customer.dst, dir);

    //Add person
    floors_[customer.src].emplace(std::make_unique<int>(customer.dst));

    //Place order
    ord_log_[customer.src] = ord_log_[customer.src] | dir_to_flag(dir);
}

dispatcher::dispatcher(ticker& chronos, std::vector<event> orders) :
    timed_obj(chronos, true)
{
    //Wait for time to start
    wait(0, wait_type::WAIT_TIL);

    //Sort timeline for correct behaviour (first wait for first)
    std::sort(orders.begin(), orders.end());

    for (auto& i : orders)
        service(i);
}