#pragma once

#include <memory>
#include <condition_variable>
#include <list>

class timed_obj;

enum wait_type {
    WAIT_FOR = 0,
    WAIT_TIL = 1
};

//Time manager class
class ticker
{
private:
    struct waiter
    {
        int time;
        std::condition_variable cv;
        wait_type type;

        inline waiter(int t, wait_type tp) :
            time(t),
            cv(),
            type(tp)
        {}
    };

    std::list<waiter> waiters_list;
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
    bool priority_;

public:
    timed_obj(const timed_obj& that) = delete;
    timed_obj(timed_obj&& that) = default;

    inline timed_obj(ticker& chronos, bool priority) :
        chronos_(chronos),
        lck_(std::unique_lock(chronos.act_mut)),
        priority_(priority)
    { wait(0, wait_type::WAIT_TIL); } 

    //Waits for "time" ticks or until time=="time" (depending on "type") in ticker::start main cycle
    void wait(int time, wait_type type);
};