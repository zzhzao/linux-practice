#include<iostream>

int main()
{
    int a = 0;
    {
        int a = 1;
        std::cout << a <<std::endl;
    }
    std::cout << a <<std::endl;
    return 0;
}