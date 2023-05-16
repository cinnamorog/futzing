#include <iostream>
using namespace std;

/*
Problem:
Remote (Slow/less-capable device) ---> Fast/high-capability device

Condition: remote can only push few/variable number of bytes at a time to the fast device.

  API called at some irregular frequency
  --> void callbackRawData(void *ptr, size_t numBytes);

Fast Device is responsible to construct a Data Packet serially from the bytes provided by the slow device.

  typedef struct {
    uint32_t a1;
    uint16_t a2;
    bool     a3;
    uint8_t  a4;
  } packetType;

  API: able to construct packets and provide the packet to an upper-layer entity via API.

  packetType createPacket(void *ptr, size_t numBytes);
  -- bool sendData(packetType *p);

*/

// Interface the fast device uses to store incoming bytes from the slow device. Lock/Unlock
// In the read and write methods for thread-safety.
class RingBuffer {
public:
  RingBuffer(size_t numBytes) :
  buf_(nullptr),
  max_size_(numBytes),
  read_c_(0),
  write_c_(0),
  num_available_(0) {
    // Assumes `max_size_` is nonzero and large enough to accommodate bytes coming
    // in as fast as they possibly can from the slow device.
    buf_ = new uint8_t[max_size_];
  }

  virtual ~RingBuffer() {
    delete [] buf_;
  }

  // Writes a single byte into the circular buffer, at the current position
  // of the write cursor.
  void WriteByte(uint8_t byte) {
    Lock();

    // We expect to have space for the incoming byte, because the fast
    // device has lots of memory and we don't expect bytes to be coming
    // in from the slow device faster than we can read them. If we fail
    // this check though, it means we need to make the ring buffer bigger.
    assert(num_available_ < max_size_);

    // Insert `byte` at in the next available slot, one more byte is available
    // to read. Wrap around to the start of the buffer we're over `max_size_`.
    buf_[(write_c_++)%max_size_] = byte;
    num_available_++;

    Unlock();
  }

  // Reads a single byte from the buffer, from the current position of the read
  // cursor. Returns `false` if there are no bytes available to read, `true`
  // otherwise.
  bool ReadByte(uint8_t* byte) {
    bool did_read = false;

    Lock();

    if (num_available_ > 0) {
      // There are bytes to read. Return the oldest byte we
      // have, thus freeing up one byte. Wrap around to the start
      // if we've read past the end.
      *byte = buf_[(read_c_++)%max_size_];
      num_available_--;
      did_read = true;
    }

    Unlock();

    return did_read;
  }

private:
  // For thread-safety, unspecified details but assume a mutex or semaphore...
  void Lock() {/* ... */}
  void Unlock() {/* ... */}

  // The buffer itself.
  uint8_t* buf_;

  // Size of the buffer.
  size_t max_size_;

  // Current read positon.
  int read_c_;

  // Current write position.
  int write_c_;

  // Number of bytes available to process.
  size_t num_available_;
};

// Something large enough to handle the incoming stream from the slow device.
const size_t kRingBufferSize = 2048;

// Global ring buffer, where incoming bytes are written by the producer thread
// and read by the consumer thread.
RingBuffer ring_buf_(kRingBufferSize);

//
// Assumed to be owned/running on the producer-thread.
//

void callbackRawData(void *ptr, size_t numBytes) {
  uint8_t *byte = (uint8_t*)ptr;
  for (int i = 0; i < numBytes; ++i) {
    ring_buf_.WriteByte(byte[i]);
  }
}

//
// Assumed to be owned/running on the consumer-thread, and
// that the following two methods exist:
//
// packetType createPacket(void *ptr, size_t numBytes);
// bool sendData(packetType *p);
//

void consumerThreadMain() {
  // Local buffer into which bytes are read out of the ring buffer and written
  // one at a time. It's full when we've received `kNumRawBytesPerPacket` and we
  // assume `createPacket` will return a valid packet.
  const size_t kNumRawBytesPerPacket = 32; // This is just a number I made up, because I
                                            // wasn't sure I should assume sizeof(packetType)
                                            // bytes itself constitutes a complete packet.
  uint8_t incoming_packet_raw[kNumRawBytesPerPacket];
  size_t num_packet_bytes = 0;

  while (1) {
    uint8_t byte;
    if (ring_buf_.ReadByte(&byte)) {
      incoming_packet_raw[num_packet_bytes++] = byte;
      if (num_packet_bytes == kNumRawBytesPerPacket) {
        // We have a packet, create and send it off.
        packetType pkt = createPacket(incoming_packet_raw, num_packet_bytes);
        sendData(&pkt);

        // Reset our buffer to receive the next packet.
        num_packet_bytes = 0;
      }
    }
}
