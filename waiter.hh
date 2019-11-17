#include <condition_variable>

class waiter
{
private:
    int time_;
    std::condition_variable ifdone_;

public:
    inline waiter(int time) :
        time_(time), 
        ifdone_()
    {}

    inline std::shared_ptr<std::condition_variable> getcv()
    { return ifdone_; }
};