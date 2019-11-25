#include "ticker.hh"

#include <iostream>

void timed_obj::wait(int time, wait_type type)
{
    if (type == WAIT_FOR) time += chronos_.time;
    if (priority_)
    {
        auto& w = chronos_.waiters_list.emplace_front(time); //Construct waiter in-place
        w.cv.wait(lck_); //Condition-variable wait
    }
    else //Same, but add to end
    {
        auto& w = chronos_.waiters_list.emplace_back(time);
        w.cv.wait(lck_);
    }

    chronos_.cv_waiter_act.notify_one();
    printf("WAITER UNLOCKED\n");
}

void ticker::start()
{
    for (; !waiters_list.empty(); time++)
    {   
        std::cout << time << "\n";

        //Process waiters list
        for (auto it = waiters_list.begin(); it != waiters_list.end();)
        {
            if (it->time < time) throw std::out_of_range("Waiter skipped");

            if (it->time == time)
            {
                //Aquire act_mut
                std::unique_lock<std::mutex> waiter_lck(act_mut);

                //Notify current waiter (will be immediately blocked, locking act_mut)
                it->cv.notify_all();

                //Unblock waiter, wait untill thread waits again
                printf("WAITING FOR WAITER-------------------\n");
                cv_waiter_act.wait(waiter_lck);
                printf("WAITER DONE--------------------------\n");
                
                //Remove waited waiter, start from the start (in case he added waiters)
                waiters_list.erase(it);
                it = waiters_list.begin();
            }
            else it++;
        }
    }
}
