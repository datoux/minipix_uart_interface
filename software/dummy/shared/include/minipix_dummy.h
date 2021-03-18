#ifndef MINIPIX_DUMMY_H
#define MINIPIX_DUMMY_H

#include <atomic>
#include <list>
#include <mutex>

#include <stdint.h>

#include <llcp.h>
#include <llcp_minipix_messages.h>

class MinipixDummy {

public:
  MinipixDummy();

  virtual void sendByte([[maybe_unused]] const uint8_t &byte_out){};
  virtual void sendString([[maybe_unused]] const uint8_t *bytes_out, [[maybe_unused]] const uint16_t &len){};
  void         serialDataCallback(const uint8_t *bytes_in, const uint16_t &len);
  virtual void sleep([[maybe_unused]] const uint16_t &milliseconds){};

protected:
  LLCP_Receiver_t llcp_receiver_;

  uint16_t boot_count_ = 0;

  void update(void);

private:
  uint8_t tx_buffer_[LLCP_RX_TX_BUFFER_SIZE];

  void ingegralFrameMeasurement(const uint16_t &acquisition_time);

  void clearToSend(void);
  std::atomic<bool> clear_to_send_ = true;

  std::list<LLCP_Message_t> message_buffer_;
  std::mutex                mutex_message_buffer_;

  void sendMessage([[maybe_unused]] const uint8_t *bytes_out, [[maybe_unused]] const uint16_t &len);
};

#endif  // MINIPIX_DUMMY_H
