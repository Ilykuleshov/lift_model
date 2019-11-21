#include "ticker.hh"

#include <iostream>

void timed_obj::wait(int time, wait_type type)
{
    if (priority_) 
    {
        auto& w = chronos_.waiters_list.emplace_front(time, type); //Construct waiter in-place
        w.cv.wait(lck_); //Condition-variable wait
    }
    else //Same, but add to end
    {
        auto& w = chronos_.waiters_list.emplace_back(time, type);
        w.cv.wait(lck_);
    }
}

void ticker::start()
{
    for (; !waiters_list.empty(); time++)
    {   
        std::cout << time << "\n";

        //New waiters are added to list during cycle, we want to iterate only on old, from begin to last
        auto last = std::prev(waiters_list.end());
        bool final_cycle = false;

        //Process waiters list
        int i = 0;
        for (auto it = waiters_list.begin(); !final_cycle;)
        {
            printf("Waiter #%d\n", i);
            i++;
            if (it == last) final_cycle = true;
            if (it->time < 0) throw std::out_of_range("Waiter's time < 0");

            bool time_out = false;
            switch(it->type)
            {
                case WAIT_FOR:
                it->time--;
                printf("WAITER TIME %d\n", it->time);
                time_out = (it->time == 0);
                break;

                case WAIT_TIL:
                time_out = (it->time == time);
                if (it->time < time) throw std::out_of_range("Waiter skipped");
                break;
            }

            if (time_out)
            {
                printf("WAIT TYPE %d\n", it->type);
                it->cv.notify_all();
                printf("LOCKING\n");
                std::unique_lock<std::mutex> lck(act_mut);
                printf("LOCKED\n");
                it = waiters_list.erase(it);
            }
            else it++;
        }
    }
}
