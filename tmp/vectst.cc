#include <vector>
#include <iostream>

using namespace std;

int main()
{
    vector<int> a = { 0, 1, 2, 3, 4, 5 };
    auto& b = a;

    b[0] = 10;

    std::cout << a[0];
}