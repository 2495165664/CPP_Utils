#include <unistd.h>

#include "map"
#include "thread"
#include "iostream"
#include "condition_variable"

class Semaphore {
    public:
        void init(int max_count)
        {
            for (int i=0;i< max_count; i++)
            {
                this->ids.insert(std::make_pair(i, i));
            }

            this->max_count = max_count;
        }
    private:
        // 等待锁
        std::mutex wait_mutex1;
        std::mutex wait_mutex2;
        // 条件变量
        std::condition_variable cvariable1;
        std::condition_variable cvariable2;

        std::map<int, int> ids;
        std::map<int, int> ids2;

        int max_count = 0;
    public:
        void waitP1()
        {
            std::unique_lock<std::mutex> lock(wait_mutex2);
            cvariable2.wait(lock);

            usleep(2000);
            ids2.clear();
            return ;
        }

    void SignalP1(int id)
    {
        std::unique_lock<std::mutex> lock(wait_mutex2);
        if (ids2.find(id) == ids2.end())
        {
            ids2.insert(std::make_pair(id, id));
        }

        if (ids2.size() == max_count)
        {
            cvariable2.notify_all();
        }
        return ;
    }

    public:
        void waitP2(int id)
        {
            std::unique_lock <std::mutex> lock(wait_mutex1);
            // while (!ready)
            // std::cout << "id等待中: " << id << std::endl;
            cvariable1.wait(lock);
            if (ids.find(id) == ids.end())
            {
                ids.insert(std::make_pair(id, id));
            }
            // std::cout << "id接收到: " << id << std::endl;
            return ;
        }

        void SignalP2()
        {
            std::unique_lock<std::mutex> lock(wait_mutex1);

            if (ids.size() == max_count)
            {
                cvariable1.notify_all();
                // std::cout << "退出" << std::endl;

                ids.clear();
            }

            return ;
        }
};


Semaphore sem;

// 主线程
void thread_main()
{
    while (true)
    {
        // TODO: 主线程操作
        std::cout << "Thream Main runing ...................... " << std::endl;
        sem.SignalP2();

        sem.waitP1();
        std::cout << std::endl;
    }
}
// 线程A
void thread_a()
{
    while (true)
    {
        sem.waitP2(1);
        // TODO: A线程操作
        std::cout << "Thream A runing " << std::endl;
        // 随机延迟
        usleep(rand() % ((80 - 10) + 10) * 1000);

        sem.SignalP1(1);
    }
}
// 线程B
void thread_b()
{
    while (true)
    {
        sem.waitP2(2);
        // TODO: B线程操作
        std::cout << "Thream B runing" << std::endl;
        // 随机延迟
        usleep(rand() % ((300 - 50) + 50) * 1000);
        
        sem.SignalP1(2);
    }
}

int main() {
    sem.init(2);
    std::thread tMain(thread_main);
    std::thread tA(thread_a);
    std::thread tB(thread_b);

    tMain.join();
    tA.join();
    tB.join();
    
    return 0;
}