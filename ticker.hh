#include <memory>
#include <condition_variable>
#include <list>

class timed_obj;

//Time manager class
class ticker
{
private:
    struct waiter
    {
        int time;
        std::condition_variable cv;

        inline waiter(int t) :
            time(t),
            cv()
        {}
    };

    std::list<waiter&> waiters_list;
    std::mutex act_mut;

    friend class timed_obj;

public:
    ticker(const ticker& that) = delete;
    ticker() = default;

    //Starts time, cycles until all waiters are satisfied
    void start();
};

//Base class for deriving classes, which need wait(time) functionality
class timed_obj
{
private:
    ticker& chronos_;
    std::unique_lock<std::mutex> lck_;

public:
    timed_obj(const timed_obj& that) = delete;
    timed_obj(timed_obj&& that) = default;

    inline timed_obj(ticker& chronos) :
        chronos_(chronos),
        lck_(std::unique_lock(chronos.act_mut))
    {} 

    //Waits until "time" ticks pass in ticker::start main cycle
    void wait(int time);
};