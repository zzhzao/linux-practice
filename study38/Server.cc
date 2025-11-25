#include<iostream>
#include"Pipe.hpp"
int main()
{
    Fifo pipefile;
    pipefile.Build();
    pipefile.Open(ForRead);
    // sleep(10);

    std::string msg;
    while(true)
    {
        int n = pipefile.Recv(&msg);
        if(n>0)
        {
            std::cout << "client: " << msg << std::endl;
        }
        else
            break;
    }
    pipefile.Delete();

    return 0;
}