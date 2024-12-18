#include <vector>
#include <thread>
#include <cassert>
#include <exception>
#include <iostream>
#include <mutex>

template<class T>
class cb_t {
   public:
    using pos_t = typename std::vector<T>::iterator;

   private:
    std::vector<T> buff;

    pos_t wr_p;
    pos_t rd_p;

   public:
    cb_t(std::size_t sz) : buff(sz, {}), wr_p(buff.begin()), rd_p(buff.begin()) {
        if(sz < 2) throw std::length_error("circular buffer must be at least 2 elements long");
        // buff.reserve(sz);
    }

    void resize(std::size_t sz) {
        std::size_t last_sz = buff.size();
        if (sz < last_sz) throw std::range_error("resize() allowed only upside");

        buff.resize(sz);

        /* If head at the beginning of queue */
        if(rd_p > wr_p) {
            auto diff = sz - last_sz;
            if(std::distance(buff.begin(), wr_p) > diff) {
                /* If we can not move entire head at the end */
                buff.insert(buff.end(), buff.begin(), buff.begin() + diff);
                buff.insert(buff.begin(), buff.begin() + diff, wr_p);
                wr_p -= diff;
            } else {
                /* If we can move entire head at the end */
                buff.insert(buff.end(), buff.begin(), buff.begin() + diff);
                wr_p += diff;
            }
        }
    }

    /* Push head */
    void push_back(T val) {
        if(wr_p == buff.end()-1) 
            wr_p = buff.begin();
        else wr_p++;


        
        // buff.insert(wr_p, val);
        *wr_p = val;

        /* Overwrite tail */
        if(wr_p == rd_p){
            if(rd_p == buff.end()-1) 
                rd_p = buff.begin();
            else rd_p++;
        }
    }

    /* Pop tail */
    T & pop_front() {
        /* UB */
        if(rd_p == wr_p) throw std::range_error("buffer is empty");

        if(rd_p == buff.end()) wr_p = buff.begin();
        else rd_p++;
        
        return *rd_p;
    }

    T & front() {
        /* UB */
        if(rd_p == wr_p) throw std::range_error("buffer is empty");
        
        return *(rd_p == buff.end() ? buff.begin() : rd_p + 1);
    }

    std::size_t size() {
        if(rd_p > wr_p)
            return std::distance(rd_p, buff.end()) + std::distance(buff.begin(), wr_p);
        else 
            return std::distance(rd_p, wr_p);
    }

    bool empty () { return size() == 0; }

};


/* Testing Threads */

std::mutex mtx;

void producer(cb_t<int>& q) {
    // for (int i = 0; i < 10; ++i) {
    for (;;) {
        static unsigned int new_value = 0;
        {
            std::lock_guard<std::mutex> lock(mtx);
            q.push_back(new_value++);
        }
        std::cout << "Produced: " << new_value << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(4)); // Simulate work
    }
}

void consumer(cb_t<int>& q) {
    while (true) {
        int value;
        {
            std::lock_guard<std::mutex> lock(mtx);
        
            if (!q.empty()) value = q.pop_front();
        }
        
        std::cout << "\t\t\tConsumed: " << value << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(4)); // Read every second
    }
}

int main (int argc, const char**argv) {

    cb_t<int> q(12);
 
    q.push_back(0); // back pushes 0
    q.push_back(1); // q = 0 1S
    q.push_back(2); // q = 0 1 2
    q.push_back(3); // q = 0 1 2 3
 
    assert(q.front() == 0);
    // assert(q.back() == 3);
    assert(q.size() == 4);
 
    assert(q.pop_front() == 0); // removes the front element, 0
    assert(q.size() == 3);
 
    // Print and remove all elements. Note that std::queue does not
    // support begin()/end(), so a range-for-loop cannot be used.
    std::cout << "q: ";
    for (; q.size(); /*q.pop_front()*/) {
        std::cout << q.pop_front() << ' ';
    }
    std::cout << '\n';

    assert(q.size() == 0);


    // Start producer thread
    std::thread producer_thread(producer, std::ref(q));

    // Start consumer thread
    std::thread consumer_thread(consumer, std::ref(q));

    // Wait for the producer to finish
    producer_thread.join();

    // Run the consumer for some more time or until it consumes all items
    std::this_thread::sleep_for(std::chrono::seconds(2));
    consumer_thread.detach(); // or join if you want to wait for the consumer to finish

    return 0;

}