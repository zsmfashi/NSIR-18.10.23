#include "thread.h"

Thread::Thread()
{

}

void Thread::run()
{
    Network ne;
    ne.init("210.30.97.233","5000");
    ne.download_file("123","123.jpg");
}
