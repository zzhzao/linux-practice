#include"Shm.hpp"
int main()
{
    Shm sharedmem;
    sharedmem.Get();
    sharedmem.Attach();

    char *shm_start = (char *)sharedmem.Addr();
    int size = sharedmem.Size();
    int index = 0;
    while (true)
    {
        std::cout << "please enter : ";
        char ch;
        std::cin >> ch;
        shm_start[index++] = ch;
        index %= size;
    }
    

    sharedmem.Detach();
    return 0;
}