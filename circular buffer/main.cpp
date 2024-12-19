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
        std::size_t pos;
        
       public:
        cb_it_t(cb_t<T> &buf, std::size_t pos) : buf(buf), pos(pos) {}

        T & operator *() {
            /* as we are frieds */
            return buf.buf[pos];
        }

        cb_it_t & operator ++() {
            if(++pos == buf.buf.size())
                pos = 0;

            return *this;
        }

        cb_it_t & operator --() {
            if(pos == 0)
                pos = buf.buf.size() - 1;
            else
                pos--;

            return *this;
        }

        cb_it_t operator ++(int) {
            auto last = *this;
            if(++pos == buf.buf.size())
                pos = 0;
            
            return last;
        }

        cb_it_t operator --(int) {
            auto last = *this;
            if(pos == 0)
                pos = buf.buf.size() - 1;
            else
                pos--;

            return last;
        }

        bool operator == (const cb_it_t &val) const {
            return &val.buf == &buf && val.pos == pos;
        }

        bool operator != (const cb_it_t &val) const {
            return !operator ==(val);
        }

        std::ptrdiff_t operator - (const cb_it_t &val) const {
            if (pos >= val.pos) return pos - val.pos;
            return buf.buf.size() - (val.pos - pos);
        }

        std::size_t get_pos() const { return pos; };        
    };

   private:
    std::vector<T> buf;

    cb_it_t wr_p;
    cb_it_t rd_p;

   public:
    explicit cb_t(std::size_t sz) : buf{}, wr_p{begin()}, rd_p{begin()} {
        if(sz < 2) throw std::length_error("circular buffer must be at least 2 elements long");

        std::cout << "Constfuct circular buffer with " << sz << " elements, size: " << buf.size() << std::endl;
        buf.resize(sz);
    }

    // void resize(std::size_t sz) {
    //     std::size_t last_sz = buf.size();
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
        if(wr_p == rd_p){
            ++rd_p;
            std::cerr << "Tail Owerwritten!" << std::endl;
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
/** @todo encapsulate with cb_t q; cond_var_t exit; */
std::mutex mtx;

void producer_fn(cb_t<int>& q) {
    // for (int i = 0; i < 10; ++i) {
    for (;;) {
        static unsigned int new_value = 0;
        {
            std::lock_guard<std::mutex> lock(mtx);
            q.push_back(new_value++);
        }
        std::cout << "Produced: " << new_value << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }
}

void consumer_fn(cb_t<int>& q) {
    while (true) {
        int value;
        {
            std::lock_guard<std::mutex> lock(mtx);
        
            if (!q.empty()) value = q.pop_front();
        }
        std::cout << "\t\t\tConsumed: " << value << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

int main (int argc, const char**argv) {

    cb_t<int> q(64);
 
    /* Push some */
    q.push_back(0); /* first element: 0 */
    q.push_back(1); /* q = 0 1 */
    q.push_back(2); /* q = 0 1 2 */
    q.push_back(3); /* q = 0 1 2 3 */
 
    /* Check some */
    std::cout << "cb_t size: " << q.size() << std::endl;
    assert(q.front() == 0);
    assert(q.size() == 4);
    assert(q.pop_front() == 0);
    assert(q.size() == 3);
 
    /* Pop all elements */
    std::cout << "q: ";
    for (; q.size(); ) {
        std::cout << q.pop_front() << ' ';
    }
    std::cout << std::endl;

    /* Check size after Pop */
    assert(q.size() == 0);

    /* Start producer & consumer threads */
    std::thread producer_th(producer_fn, std::ref(q));
    std::thread consumer_th(consumer_fn, std::ref(q));

    /* Run the for some time then resize the queue */
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // q.resize(72);

    /* Exit  */
    std::getc(stdin);

    /** @todo exit CV & join() */
    producer_th.detach();
    consumer_th.detach();

    return 0;

}