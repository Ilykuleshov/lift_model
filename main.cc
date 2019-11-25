#include "lift.hh"
#include <thread>
#include <iostream>

int main()
{
    ticker chronos;

    int floor_amnt;
    int lift_amnt;
    LIFTSPEC specs;

    printf("floor amount/lift amount/lift capacity/ time: stage/open/idle/close/in/out\n");
    std::cin >> 
        floor_amnt    >> lift_amnt    >> specs.MAX   >> 
        specs.T_stage >> specs.T_open >> specs.T_idle >> 
        specs.T_close >> specs.T_in   >> specs.T_out;
    
    printf("Enter customers: time src dst\n");
    std::queue<event> timeline;
    while (true)
    {
        event customer;
        std::cin >> customer.time >> customer.src >> customer.dst;
        if (std::cin.eof()) break;
        timeline.push(customer);
    }

    printf("Got %d customers\n", timeline.size());
    
    std::vector<lift> lift_vec;

    dispatcher d(chronos, floor_amnt, lift_vec, timeline);
    std::thread disp_thr([&d] { d.run(); });

    for(int i = 0; i < lift_amnt; i++)
        lift_vec.emplace_back(chronos, d, specs);

    std::list<std::thread> lift_thr_list;
    for (auto& l : lift_vec) lift_thr_list.emplace_back([&l] { l.run(); });

    chronos.start();
    for (auto& t : lift_thr_list) if (t.joinable()) t.join();
    if (disp_thr.joinable()) disp_thr.join();
}