#include "SearchRingBuffer.h"
#include <thread>

using sysClock = std::chrono::system_clock;
using minutes = std::chrono::minutes;

SCENARIO("Populate the buffer with one thread and read from it from another")
{
  GIVEN("A circular buffer")
  {
    Concurrent::SearchRingBuffer<int, 10000> circBuff;

    WHEN("A thread is populating the buffer")
    {
      auto time = sysClock::now();

      std::thread producer([&circBuff, time]() {
        for (int i = 0; i < 10000; ++i)
        {
          circBuff.push((time + minutes(i)), i);
        }
      });

      // Sleep to give the publisher a head start.
      std::this_thread::sleep_for(std::chrono::milliseconds(1));

      THEN("Another thread can concurrently read from the buffer")
      {
        std::thread consumer([&circBuff, time]() {
          for (int i = 0; i < 10000; ++i)
          {

            CHECK(i == circBuff.read(time + minutes(i)));
          }
        });
        consumer.join();
      }
      producer.join();
    }
  }
}