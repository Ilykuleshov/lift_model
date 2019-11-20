#include "ticker.hh"


void timed_obj::wait(int time, wait_type type)
{
    ticker::waiter w(time, type);
    if (priority_) chronos_.waiters_list.push_front(w);
    else           chronos_.waiters_list.push_back(w);
    w.cv.wait(lck_);
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
