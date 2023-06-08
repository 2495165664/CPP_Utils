#include "unistd.h"

#include "queue"
#include "thread"
#include "iostream"
#include "condition_variable"


// 消息队列结构体
class WornMessageQueue{
    private:
        std::queue<int> queue;
        std::mutex lock;
        std::condition_variable cv;

    public:
        int wait()
        {
            std::unique_lock<std::mutex> lck(lock);
            cv.wait(lck);

            int alarm = queue.front();
            queue.pop();

            return alarm;
        }

        void notify(int msg)
        {
            std::lock_guard<std::mutex> lck(lock);
            
            queue.push(msg);

            cv.notify_one();
            return ;
        }
};

WornMessageQueue queue;

void thread_main()
{
    while (true)
    {
        int msg = queue.wait();

        std::cout << "receive msg: " <<msg << std::endl;
    }
}

void thread_push()
{
    while (true)
    {
        // 随机延迟
        usleep(rand() % ((300 - 50) + 50) * 1000);

        // 获取一个随机数
        int rand_int = rand() % 300;
        std::cout << "push msg: " << rand_int << std::endl;
        queue.notify(rand_int);

    }
}

int main(int argc, char const *argv[])
{
    std::thread tMain(thread_main);
    std::thread tA(thread_push);

    tMain.join();
    tA.join();
    return 0;
}
