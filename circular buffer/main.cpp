#include <vector>
#include <thread>
#include <cassert>
#include <exception>
#include <iostream>
#include <mutex>
#include <iterator>

template<typename T>
class cb_t {
        friend class cb_it_t;

   public:
    class cb_it_t {
        cb_t<T> &buf;
       public:
        std::size_t pos;
        
    //    public:
        cb_it_t(cb_t<T> &buf, std::size_t pos) : buf(buf), pos(pos) {}

        T & operator *() {
            /* as we are frieds */
            return buf.buf[pos];
        }

        cb_it_t & operator ++() {
            std::cout << "incremented(buf sz:" << buf.buf.size() << ") pre: " << pos;
            
            pos++;
            if(pos == buf.buf.size())
                pos = 0;

            std::cout << " -> " << pos << std::endl;
            return *this;
        }

        cb_it_t & operator --() {
            std::cout << "decremented(buf sz:" << buf.buf.size() << ") pre: " << pos;

            if(pos == 0)
                pos = buf.buf.size() - 1;
            else
                pos--;
            
            std::cout << " -> " << pos << std::endl;
            return *this;
        }

        cb_it_t operator ++(int) {
            std::cout << "incremented(buf sz:" << buf.buf.size() << ") pos: " << pos;

            auto last = *this;
            pos++;
            if(pos == buf.buf.size())
                pos = 0;

            std::cout << " -> " << pos << std::endl;
            return last;
        }

        cb_it_t operator --(int) {
            std::cout << "decremented(buf sz:" << buf.buf.size() << ") pos: " << pos;

            auto last = *this;
            if(pos == 0)
                pos = buf.buf.size() - 1;
            else
                pos--;

            std::cout << " -> " << pos << std::endl;
            return last;
        }

        bool operator == (const cb_it_t &val) const {
            return &val.buf == &buf && val.pos == pos;
        }

        bool operator != (const cb_it_t &val) const {
            return !operator ==(val);
        }

        std::ptrdiff_t operator - (const cb_it_t &val) const {
            std::cout << "{ diff pointers: local val: " << pos << "; r val: }" << val.pos;

            if (pos >= val.pos) return pos - val.pos;
            return buf.buf.size() - (val.pos - pos);
        }
    };

   private:
    std::vector<T> buf;

    cb_it_t wr_p;
    cb_it_t rd_p;

   public:
    explicit cb_t(std::size_t sz) : buf{sz, {}}, wr_p{begin()}, rd_p{begin()} {
        if(sz < 2) throw std::length_error("circular buffer must be at least 2 elements long");

        std::cout << "Constfuct circular buffer with " << sz << " elements, size: " << buf.size() << std::endl;
        buf.resize(sz);
    }

    // void resize(std::size_t sz) {
    //     std::size_t last_sz = buff.size();
    //     if (sz < last_sz) throw std::range_error("resize() allowed only upside");

    //     buff.resize(sz);

    //     /* If head at the beginning of queue */
    //     if(rd_p > wr_p) {
    //         auto diff = sz - last_sz;
    //         if(std::distance(buff.begin(), wr_p) > diff) {
    //             /* If we can not move entire head at the end */
    //             buff.insert(buff.end(), buff.begin(), buff.begin() + diff);
    //             buff.insert(buff.begin(), buff.begin() + diff, wr_p);
    //             wr_p -= diff;
    //         } else {
    //             /* If we can move entire head at the end */
    //             buff.insert(buff.end(), buff.begin(), buff.begin() + diff);
    //             wr_p += diff;
    //         }
    //     }
    // }

    /* Push head */
    void push_back(const T &val) {
        *wr_p = val;
        ++wr_p;

        /* Overwrite tail */
        if(wr_p == rd_p) {
            std::cout << "tail overwritten! (wr: " << wr_p.pos << "; rd: " << wr_p.pos << ")" <<  std::endl;
            ++rd_p;
        }
    }

    /* Pop tail */
    T & pop_front() {
        /* UB */
        if(rd_p == wr_p) throw std::range_error("buffer is empty");

        return *rd_p++;
    }

    T & front() {
        /* UB */
        if(rd_p == wr_p) throw std::range_error("buffer is empty");
        
        return *rd_p;
    }

    std::size_t size() {
        /* Circular buffer never have negative size */
        return static_cast<std::size_t>(wr_p - rd_p);
    }

    bool empty () { return rd_p == wr_p; }

    cb_it_t begin() {
        return {*this, 0};
    }
};


/* Testing Threads Routines */
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
    std::cout << "cb_t size: " << q.size() << std::endl;
    std::fflush(NULL);
    
    q.push_back(1); // q = 0 1S
    std::cout << "cb_t size: " << q.size() << std::endl;
    std::fflush(NULL);
    
    q.push_back(2); // q = 0 1 2
    std::cout << "cb_t size: " << q.size() << std::endl;
    std::fflush(NULL);

    q.push_back(3); // q = 0 1 2 3
    std::cout << "cb_t size: " << q.size() << std::endl;
    std::fflush(NULL);
 
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