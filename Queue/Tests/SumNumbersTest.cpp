#include <future>
#include <random>
#include <vector>

#include "ConcurrentQueue.h"
#include "catch.hpp"

int
pushData(Concurrent::Queue<int>& queue, std::random_device::result_type seed)
{
  std::mt19937 generator(seed); // Standard mersenne_twister_engine

  // Generate randomly sized vector with random numbers
  std::uniform_int_distribution dist(10000, 10000000); // Used to generate random vector size with min size and max size
  int                           vector_size = dist(generator);
  std::vector<int>              nums{ vector_size };
  std::generate(nums.begin(), nums.end(), generator);

  // Push each number into the concurrent queue
  // and calculate the total number
  int total{ 0 };
  for (auto num : nums)
  {
    queue.push(num);
    total += num;
  }
  return total;
}

int
consumeData(Concurrent::Queue<int>& queue, std::atomic<bool>& producersComplete)
{
  int total{ 0 };

  while (!producersComplete) // While producers are still pushing data.
  {
    while (auto data = queue.tryGet()) // While there is still data in the queue.
    {
      total += *data;
    }
  }
  return total;
}

// The producers
// - generate random data
// - push it to the queue
// - total the numbers they have generated
// - return the total number.
// In parallel the consumers
// - read data from the queue
// - maintain a total for all the numbers they have read.
// The consumers stop reading when there is no more data in the queue and the producers have stopped pushing data.
// A check is performed to ensure the total of the numbers the producers put in the queue is the same as the total of the numbers
// the consumers read from the queue.
// Catch2's INFO macro is not thread safe so the seed has to be generated & printed in main thread rather than in producers thread.
TEST_CASE("Sum numbers")
{
  Concurrent::Queue<int>          queue;
  std::atomic<bool>               producersComplete{ false };
  std::random_device              rd;
  std::random_device::result_type seed = rd();
  INFO("Using seed: " << seed);

  std::future<int> consumer1Total = std::async(consumeData, std::ref(queue), std::ref(producersComplete));
  std::future<int> producer1Total = std::async(pushData, std::ref(queue), seed++);
  std::future<int> producer2Total = std::async(pushData, std::ref(queue), seed++);
  std::future<int> consumer2Total = std::async(consumeData, std::ref(queue), std::ref(producersComplete));
  std::future<int> producer3Total = std::async(pushData, std::ref(queue), seed++);

  int prodTotal = producer1Total.get() + producer2Total.get() + producer3Total.get();
  producersComplete = true;
  int consTotal = consumer1Total.get() + consumer2Total.get();

  CHECK(prodTotal == consTotal);
}
