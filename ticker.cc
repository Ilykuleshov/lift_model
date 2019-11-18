#include "ticker.hh"


void timed_obj::wait(int time)
{
    ticker::waiter w(time);
    chronos_.waiters_list.push_back(w);
    w.cv.wait(lck_);
}

void ticker::start()
{
    for (size_t time = 0; !waiters_list.empty(); time++)
    {
        for (auto it = waiters_list.begin(); it != waiters_list.end();)
        {
            if (it->time == 0)
            {
                it->cv.notify_one();
                it = waiters_list.erase(it);
            }
            else 
            {
                it->time--;
                it++;
            }
        }
    }
}
