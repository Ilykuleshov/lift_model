#include "dispatcher.hh"

class lift : private timed_obj
{
private:
    dispatcher& disp_;

    int ord_floor_;
    int ord_dir_;

    int floor_;
    direction dir_;
    std::multiset<person> passengers;

    //Cycle step: from state <Lift arrived on floor "floor", doors closed>:
    //0. if dir_ == NONE => wait(1)(no orders), return
    //1. Check for ppl going out
    //2. Check ppl on that floor going in needed direction (through dispatcher). 
    //   (For now - everyone who didn't fit dies)
    //3. {Open doors & exchange ppl & close doors} if needed
    //4. If floor_ == ord_floor_ && dir_ == ord_dir_  => ord floor = -1; ord_dir_ = NONE;
    //5. if passengers != empty => move
    //6. else if ord_ != NONE => move to ord->floor_
    //7. else dir_ = NONE; wait(1)
    void step();

public:
    lift(ticker& chronos, dispatcher& disp);
};