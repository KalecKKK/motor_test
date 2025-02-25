#include "definitions.hpp"
#include <chrono>
#include <cmath>
#include <iostream>
#include <pthread.h>
#include <string>
#include <thread>

using namespace EcanVCI;

constexpr int MAX_CHANNELS = 2;
constexpr int CHECK_POINT = 200;
constexpr int RX_WAIT_TIME = 0;
constexpr int RX_BUFF_SIZE = 1;

unsigned int gDevType = 0;
unsigned int gDevIdx = 0;
unsigned int gChMask = 0;
unsigned int gBaud = 0;
unsigned char gTxType = 0;
unsigned int gTxSleep = 0;
unsigned int gTxFrames = 0;

void msleep(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

unsigned int s2n(const std::string &s) {
  unsigned l = s.length();
  unsigned v = 0;
  unsigned h = (l > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'));
  unsigned char c;
  unsigned char t;
  if (!h)
    return std::stoi(s);
  if (l > 10)
    return 0;
  for (auto it = s.begin() + 2; it != s.end(); ++it) {
    c = *it;
    if (c >= 'A' && c <= 'F')
      c += 32;
    if (c >= '0' && c <= '9')
      t = c - '0';
    else if (c >= 'a' && c <= 'f')
      t = c - 'a' + 10;
    else
      return 0;
    v = (v << 4) | t;
  }
  return v;
}

void generate_frame(CAN_OBJ &can) {
  can = {};
  can.SendType = gTxType;
  can.DataLen = 1 + (rand() % 8); // random data length: 1~8

  for (unsigned i = 0; i < can.DataLen; i++) {
    can.Data[i] = rand() & 0xff; // random data
    can.ID ^= can.Data[i];       // id: bit0~7, checksum of data0~N
  }
  can.ID |= ((unsigned)can.DataLen - 1) << 8; // id: bit8~bit10 = data_length-1
  can.ExternFlag = rand() % 2;                // random frame format
  if (!can.ExternFlag)
    return;
  can.ID |= can.ID << 11; // id: bit11~bit21 == bit0~bit10
  can.ID |= can.ID << 11; // id: bit22~bit28 == bit0~bit7
}

bool verify_frame(const CAN_OBJ &can) {
  if (can.DataLen > 8)
    return false; // error: data length
  unsigned bcc = 0;
  for (unsigned i = 0; i < can.DataLen; i++)
    bcc ^= can.Data[i];
  if ((can.ID & 0xff) != bcc)
    return false; // error: data checksum
  if (((can.ID >> 8) & 7) != (can.DataLen - 1))
    return false; // error: data length
  if (!can.ExternFlag)
    return true; // std-frame ok
  if (((can.ID >> 11) & 0x7ff) != (can.ID & 0x7ff))
    return false; // error: frame id
  if (((can.ID >> 22) & 0x7f) != (can.ID & 0x7f))
    return false; // error: frame id
  return true;    // ext-frame ok
}

struct RX_CTX {
  unsigned channel; // channel index, 0~3
  bool stop;        // stop RX-thread
  unsigned total;   // total received
  bool error;       // error(s) detected
};

void *rx_thread(void *data) {
  RX_CTX *ctx = static_cast<RX_CTX *>(data);
  ctx->total = 0; // reset counter

  CAN_OBJ can[RX_BUFF_SIZE]; // buffer
  int cnt;                   // current received

  unsigned check_point = 0;
  while (!ctx->stop && !ctx->error) {
    cnt = Receive(gDevType, gDevIdx, ctx->channel, can, RX_BUFF_SIZE,
                  RX_WAIT_TIME);
    if (!cnt)
      continue;

    for (int i = 0; i < cnt; i++) {
      if (verify_frame(can[i]))
        continue;
      std::cerr << "CAN" << ctx->channel << ": verify_frame() failed\n";
      ctx->error = true;
      break;
    }
    if (ctx->error)
      break;

    ctx->total += cnt;
    if (ctx->total / CHECK_POINT >= check_point) {
      std::cout << "CAN" << ctx->channel << ": " << ctx->total
                << " frames received & verified\n";
      check_point++;
    }
  }

  std::cout << "CAN" << ctx->channel << " RX thread terminated, " << ctx->total
            << " frames received & verified: "
            << (ctx->error ? "error(s) detected" : "no error") << "\n";

  pthread_exit(0);
}

int test() {
  // ----- init & start -------------------------------------------------

  INIT_CONFIG config;
  config.AccCode = 0;
  config.AccMask = 0xffffffff;
  config.Filter = 0;
  config.Mode = 0;
  config.Timing0 = gBaud & 0xff;
  config.Timing1 = gBaud >> 8;

  for (int i = 0; i < MAX_CHANNELS; i++) {
    if ((gChMask & (1 << i)) == 0)
      continue;

    if (!InitCAN(gDevType, gDevIdx, i, &config)) {
      std::cerr << "InitCAN(" << i << ") failed\n";
      return 0;
    }

    std::cout << "InitCAN(" << i << ") succeeded\n";

    if (!StartCAN(gDevType, gDevIdx, i)) {
      std::cerr << "StartCAN(" << i << ") failed\n";
      return 0;
    }

    std::cout << "StartCAN(" << i << ") succeeded\n";
  }

  // ----- RX-timeout test ----------------------------------------------

  CAN_OBJ can;
  auto tm1 = std::chrono::system_clock::now();
  auto tm2 = std::chrono::system_clock::now();
  for (int i = 0; i < 3; i++) {
    tm1 = std::chrono::system_clock::now();
    Receive(gDevType, gDevIdx, 0, &can, 1, (i + 1) * 1000 /*ms*/);
    tm2 = std::chrono::system_clock::now();
    std::cout
        << "Receive returned: time ~= "
        << std::chrono::duration_cast<std::chrono::seconds>(tm2 - tm1).count()
        << " seconds\n";
  }

  // ----- create RX-threads --------------------------------------------

  RX_CTX rx_ctx[MAX_CHANNELS];
  pthread_t rx_threads[MAX_CHANNELS];
  for (int i = 0; i < MAX_CHANNELS; i++) {
    if ((gChMask & (1 << i)) == 0)
      continue;

    rx_ctx[i].channel = i;
    rx_ctx[i].stop = false;
    rx_ctx[i].total = 0;
    rx_ctx[i].error = false;

    pthread_create(&rx_threads[i], nullptr, rx_thread, &rx_ctx[i]);
  }

  // ----- wait --------------------------------------------------------

  std::cout << "<ENTER> to start TX: " << gTxFrames
            << " frames/channel, baud: t0=0x" << std::hex << config.Timing0
            << ", t1=0x" << std::hex << config.Timing1 << "...\n";
  std::cin.get();

  // ----- start transmit -----------------------------------------------

  tm1 = std::chrono::system_clock::now();
  bool err = false;
  unsigned tx;

  for (tx = 0; !err && tx < gTxFrames; tx++) {
    for (int i = 0; i < MAX_CHANNELS; i++) {
      if ((gChMask & (1 << i)) == 0)
        continue;

      generate_frame(can);
      if (1 != Transmit(gDevType, gDevIdx, i, &can, 1)) {
        std::cerr << "CAN" << i << " TX failed: ID=" << std::hex << can.ID
                  << "\n";
        err = true;
        break;
      }
    }
    if (gTxSleep)
      msleep(gTxSleep);
  }
  tm2 = std::chrono::system_clock::now();

  // ----- stop TX & RX -------------------------------------------------

  msleep(1000);
  std::cout << "TX stopped, <ENTER> to terminate RX-threads...\n";
  std::cin.get();

  for (int i = 0; i < MAX_CHANNELS; i++) {
    if ((gChMask & (1 << i)) == 0)
      continue;

    rx_ctx[i].stop = true;

    pthread_join(rx_threads[i], nullptr);

    if (rx_ctx[i].error)
      err = true;
  }

  // ----- report -------------------------------------------------------

  if (err) {
    std::cerr << "error(s) detected, test failed\n";
    return 0;
  }

  std::cout
      << "\n ***** " << gTxFrames << " frames/channel transferred, "
      << std::chrono::duration_cast<std::chrono::seconds>(tm2 - tm1).count()
      << " seconds elapsed *****\n";
  if (std::chrono::duration_cast<std::chrono::seconds>(tm2 - tm1).count())
    std::cout << "        performance: "
              << gTxFrames /
                     std::chrono::duration_cast<std::chrono::seconds>(tm2 - tm1)
                         .count()
              << " frames/channel/second\n";

  return 1;
}

int main(int argc, char *argv[]) {
  if (argc < 7) {
    std::cerr << "test [DevType] [DevIdx] [ChMask] [Baud] [TxType] [TxSleep] "
                 "[TxFrames]\n"
              << "    example: test 16 0 3 0x1400 0 1 1000\n"
              << "                  |  | | |      | | |\n"
              << "                  |  | | |      | | |1000 frames / channel\n"
              << "                  |  | | |      | |\n"
              << "                  |  | | |      | |tx > sleep(3ms) > tx > "
                 "sleep(3ms) ....\n"
              << "                  |  | | |      |\n"
              << "                  |  | | |      |0-normal, 1-single, "
                 "2-self_test, 3-single_self_test, 4-single_no_wait....\n"
              << "                  |  | | |\n"
              << "                  |  | | |0x1400-1M, 0x1c03-125K, ....\n"
              << "                  |  | |\n"
              << "                  |  | |bit0-CAN1, bit1-CAN2, bit2-CAN3, "
                 "bit3-CAN4, 3=CAN1+CAN2, 7=CAN1+CAN2+CAN3\n"
              << "                  |  |\n"
              << "                  |  |Card0\n"
              << "                  |\n"
              << "                  |1-usbcan,  ....\n";
    return 0;
  }

  gDevType = s2n(argv[1]);
  gDevIdx = s2n(argv[2]);
  gChMask = s2n(argv[3]);
  gBaud = s2n(argv[4]);
  gTxType = s2n(argv[5]);
  gTxSleep = s2n(argv[6]);
  gTxFrames = s2n(argv[7]);
  std::cout << "DevType=" << gDevType << ", DevIdx=" << gDevIdx << ", ChMask=0x"
            << std::hex << gChMask << ", Baud=0x" << std::hex << gBaud
            << ", TxType=" << std::dec << gTxType << ", TxSleep=" << gTxSleep
            << ", TxFrames=0x" << std::hex << gTxFrames << "(" << std::dec
            << gTxFrames << ")\n";

  if (!OpenDevice(gDevType, gDevIdx, 0)) {
    std::cerr << "OpenDevice failed\n";
    return 0;
  }
  std::cout << "OpenDevice succeeded\n";

  test();

  CloseDevice(gDevType, gDevIdx);
  std::cout << "CloseDevice\n";
  return 0;
}
