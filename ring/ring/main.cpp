//
//  main.cpp
//  ring
//
//  Created by Roger Tinkoff on 3/20/23.
//

#include <iostream>
#include <memory.h>

class RingBuffer {
public:
    RingBuffer(int num_bytes) :
        buf_(new uint8_t[num_bytes]),
        size_(num_bytes),
        read_c_(0),
        write_c_(0),
        num_available_(0)
    {
        memset(buf_, 0, size_);
    }
    
    virtual ~RingBuffer()
    {
        delete [] buf_;
        buf_ = nullptr;
    }
    
    void write(uint8_t val)
    {
        if (num_available_ == size_) {
            uint8_t tmp;
            read(&tmp);
        }
        
        buf_[(write_c_++)%size_] = val;
        num_available_++;
    }
    
    bool read(uint8_t* val)
    {
        if (num_available_ <= 0) {
            return false;
        }
        
        *val = buf_[(read_c_++)%size_];
        num_available_--;

        return true;
    }
    
private:
    uint8_t* buf_;
    int size_;
    int read_c_;
    int write_c_;
    int num_available_;
};

int main(int argc, const char * argv[])
{
    {
        RingBuffer rb(1);
        uint8_t v;
        bool r = rb.read(&v);
        assert(!r);
        rb.write(3);
        r = rb.read(&v);
        assert(r);
        assert(v == 3);
        rb.write(4);
        rb.write(5);
        rb.read(&v);
        assert(v == 5);
    }
    
    {
        RingBuffer rb(2);
        rb.write(3);
        rb.write(5);
        uint8_t v;
        bool r = rb.read(&v);
        assert(r);
        assert(v == 3);
        r = rb.read(&v);
        assert(r);
        assert(v == 5);
    }
    
    {
        RingBuffer rb(2);
        rb.write(3);
        rb.write(5);
        rb.write(7);
        uint8_t v;
        bool r = rb.read(&v);
        assert(r);
        assert(v == 5);
        r = rb.read(&v);
        assert(r);
        assert(v == 7);
        r = rb.read(&v);
        assert(!r);
    }
    
    {
        RingBuffer rb(4);
        rb.write(3);
        rb.write(5);
        rb.write(6);
        rb.write(7);
        rb.write(9);
        uint8_t v;
        bool r = rb.read(&v);
        assert(r);
        assert(v == 5);
        r = rb.read(&v);
        assert(r);
        assert(v == 6);
        r = rb.read(&v);
        assert(r);
        assert(v == 7);
        r = rb.read(&v);
        assert(r);
        assert(v == 9);
        r = rb.read(&v);
        assert(!r);
    }
    
    {
        RingBuffer rb(4);
        rb.write(3);
        rb.write(5);
        rb.write(6);
        rb.write(7);
        rb.write(9);
        uint8_t v;
        bool r = rb.read(&v);
        assert(r);
        assert(v == 5);
        r = rb.read(&v);
        assert(r);
        assert(v == 6);
        r = rb.read(&v);
        assert(r);
        assert(v == 7);
        rb.write(10);
        rb.write(34);
        r = rb.read(&v);
        assert(r);
        assert(v == 9);
    }
    
    {
        RingBuffer rb(8);
        for (int i = 2; i < 18; ++i) {
            rb.write(i);
        }
        
        uint8_t v;
        bool r = rb.read(&v);
        assert(v == 10);
    }
    
    return 0;
}
