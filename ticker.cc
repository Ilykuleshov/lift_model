#include "ticker.hh"

void timed_obj::wait(int time, wait_type type)
{
    if (priority_) 
    {
        auto& w = chronos_.waiters_list.emplace_front(time, type); //Construct waiter in-place
        w.cv.wait(lck_); //Condition-variable wait
    }
    else //Same, but add to end
    {
        auto& w = chronos_.waiters_list.emplace_back (time, type);
        w.cv.wait(lck_);
    }
}

void ticker::start()
{
    for (long time = 0; !waiters_list.empty(); time++)
    {   
        //Process waiters list
        for (auto it = waiters_list.begin(); it != waiters_list.end();)
        {
            if (it->time < 0) throw std::out_of_range("Waiter's time < 0");

            bool time_out = false;
            switch(it->type)
            {
                //wait_for waiters
                case WAIT_FOR:
                time_out = (it->time == 0);
                it->time--;
                break;

                case WAIT_TIL:
                time_out = (it->time == time);
                if (it->time < time) throw std::out_of_range("Waiter skipped");
                break;
            }

            if (time_out)
            {
                it->cv.notify_one();
                it = waiters_list.erase(it);
            }
            else it++;
        }
    }
}
