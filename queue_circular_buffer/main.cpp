#include <vector>
#include <thread>
#include <cassert>
#include <exception>
#include <iostream>
#include <mutex>
#include <iterator>

/** @brief Circular Buffer */
template<typename T>
class cb_t {
        friend class it_t;
   public:
    /** @brief Circular Iterator
     *
     *  @warning As we are frieds
     *  @note buf.buf is bad, because its a violation of cb_t encaps
      * @todo Improve cb_t iface
     */
    class it_t {
        cb_t<T> &buf;
        std::ptrdiff_t pos;
        
       public:
        it_t(cb_t<T> &buf, std::size_t pos) : buf(buf), pos(pos) {}

        T & operator *() {
            return buf.buf[pos];
        }

        it_t & operator ++() {
            if(++pos == buf.buf.size())
                pos = 0;
            return *this;
        }

        it_t & operator --() {
            if(pos == 0)
                pos = buf.buf.size() - 1;
            else
                pos--;
            return *this;
        }

        it_t operator ++(int) {
            auto last = *this;
            if(++pos == buf.buf.size())
                pos = 0;
            return last;
        }

        it_t operator --(int) {
            auto last = *this;
            if(pos == 0)
                pos = buf.buf.size() - 1;
            else
                pos--;
            return last;
        }

        bool operator == (const it_t &val) const {
            return &val.buf == &buf && val.pos == pos;
        }

        bool operator != (const it_t &val) const {
            return !operator ==(val);
        }

        void operator -= (ptrdiff_t val) {
            if(val >= buf.buf.size())
                throw std::length_error("Circular buffer maximum decrement size is buffer size");

            pos -= val;
            if(pos < 0) pos += buf.buf.size();
        }

        std::ptrdiff_t operator - (const it_t &val) const {
            if (pos >= val.pos) return pos - val.pos;
            return buf.buf.size() - (val.pos - pos);
        }

        /** @warning this is encaps violation */
        std::size_t get_pos() const { return pos; };
    };

   private:
    std::vector<T> buf;

    it_t wr_p;
    it_t rd_p;

   public:
    explicit cb_t(std::size_t sz) : buf{}, wr_p{begin()}, rd_p{begin()} {
        if(sz < 2) throw std::length_error("Circular buffer must be at least 2 elements long");
        buf.resize(sz);

        std::cout << "Construct circular buffer with " << sz << std::endl;
    }

    void resize(std::size_t sz) {
        std::size_t last_sz = buf.size();
        if (sz < last_sz) throw std::range_error("resize() allowed only upside");

        /* realloc */
        buf.resize(sz);
        auto diff_sz = sz - last_sz;

        /* Move data and pointers if head at the beginning of queue */
        /** @todo encapsulate position comparision logic into it_t instead of get_pos() */
        if(rd_p.get_pos() > wr_p.get_pos()) {
            if(wr_p.get_pos() > diff_sz) {
                /* If we cann`t move entire head at the end */
                /* move partial back to new space until buffer end */
                auto i = 0;
                for(; i < diff_sz; i++)
                    buf[last_sz + i] = std::move(buf[i]);
                /* move last back to space at the begining */
                for(auto k = 0; i < wr_p.get_pos(); i++)
                    buf[k] = std::move(buf[i]);
                wr_p -= diff_sz;
            } else {
                /* If we can move entire head at the end */
                /* move back to new space */
                for(auto i = 0; i < wr_p.get_pos(); i++)
                    buf[last_sz + i] = std::move(buf[i]);
                wr_p -= diff_sz;
            }
        }
    }

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
        if(rd_p == wr_p) throw std::range_error("Buffer is empty");

        return *rd_p++;
    }

    T & front() {
        /* UB */
        if(rd_p == wr_p) throw std::range_error("Buffer is empty");
        
        return *rd_p;
    }

    std::size_t size() {
        /* Circular buffer never have negative size */
        return static_cast<std::size_t>(wr_p - rd_p);
    }

    bool empty () { return rd_p == wr_p; }

    it_t begin() {
        return {*this, 0};
    }

    /** @todo Add size and random access methods
              in order to fix it_t description warning */
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
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
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
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main (int argc, const char**argv) {

    cb_t<int> q(12);
 
    /* Push some */
    q.push_back(0); /* first element: 0 */
    q.push_back(1); /* q = 0 1 */
    q.push_back(2); /* q = 0 1 2 */
    q.push_back(3); /* q = 0 1 2 3 */
 
    /* Check some */
    std::cout << "cb_t size: " << q.size() << std::endl;
    assert(q.front() == 0);
    assert(q.size() == 4);
    assert(q.pop_front() == 0); /** @note pop first element */
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

    /* Resize under lock */
    {
        std::lock_guard<std::mutex> lock(mtx);
        q.resize(18);
    }
    std::cout << ">~~< Queue Resized! >~~<" << std::endl;

    /* Exit  */
    std::cout << std::endl << std::endl << "...press ^M to exit" << std::flush;
    std::cin.get();

    /** @todo exit CV & join() */
    producer_th.detach();
    consumer_th.detach();

    std::cout << ">~~<   Exit 0    >~~<" << std::endl;
    return 0;
}