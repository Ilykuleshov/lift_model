#include "lift.hh"
#include <thread>
#include <iostream>

int main()
{
    ticker chronos;

    const int floor_amnt = 2;
    std::queue<event> timeline;
    timeline.push({0, 1, 1});

    LIFTSPEC specs;
    specs.MAX_ = 5;
    specs.T_stage_ = 1;
    specs.T_open_ = 1;
    specs.T_idle_ = 1;
    specs.T_in = 1;
    specs.T_out = 1;
    
    std::vector<lift> lift_vec;

    dispatcher d(chronos, floor_amnt, lift_vec, timeline);
    std::thread disp_thr([&d] { d.run(); });

    lift_vec.emplace_back(chronos, d, specs);

    std::list<std::thread> lift_thr_list;
    for (auto& l : lift_vec) lift_thr_list.emplace_back([&l] { l.run(); });

    chronos.start();
    for (auto& t : lift_thr_list) if (t.joinable()) t.join();
    if (disp_thr.joinable()) disp_thr.join();
}