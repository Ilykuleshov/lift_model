#include "lift.hh"
#include <thread>
#include <iostream>

int main()
{
    ticker chronos;

    const int N = 1;
    std::queue<event> timeline;
    timeline.push({0, 1, 1});

    LIFTSPEC specs;
    specs.MAX_ = 5;
    specs.T_stage_ = 1;
    specs.T_open_ = 1;
    specs.T_idle_;
    specs.T_in;
    specs.T_out;
    
    std::vector<lift> lift_vec;

    dispatcher d(chronos, N, lift_vec, timeline);
    std::thread disp_thr([&d] { d.run(); });

    lift_vec.emplace_back(chronos, d, specs);
    std::thread l_thr([&l=lift_vec.front()] { l.run(); });

    chronos.start();
    l_thr.join();
    disp_thr.join();
}