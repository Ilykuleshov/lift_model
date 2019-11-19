#include "dispatcher.hh"

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