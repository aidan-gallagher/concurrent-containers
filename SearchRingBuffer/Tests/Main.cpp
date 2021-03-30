#define CATCH_CONFIG_MAIN
#include "catch.hpp"

/*
//*********** Start of multi-threaded tests**********
SCENARIO("Populate the buffer with one thread and read from it from another")
{
        GIVEN("A circular buffer")
        {
                CircularBuffer<int, 10000> circBuff;

                WHEN("A thread is populating the buffer")
                {

                        std::thread producer([&circBuff]() {
                                for (int i = 0; i < 10000; ++i)
                                {
                                        circBuff.push(i, i);
                                }
                        });

                        // Sleep to give the publisher a head start.
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));

                        THEN("Another thread can concurrently read from the buffer")
                        {
                                std::thread consumer([&circBuff]() {
                                        for (int i=0; i < 10000; ++i)
                                        {
                                                int output;
                                                circBuff.get_at(i, output);
                                                CHECK(output == i);
                                        }
                                });
                                consumer.join();
                        }
                        producer.join();

                }
        }
}
//*********** End of multi-threaded tests**********

//*********** Start of white box tests**********
SCENARIO("Verify the buffer wraps when the number of elements inserted is greater than the size")
{
        GIVEN("A buffer of size 3") {
                CircularBuffer<int, 3> circBuff;
                WHEN("7 elements are inserted")
                {
                        circBuff.push(10, 1);
                        circBuff.push(20, 2);
                        circBuff.push(30, 3);
                        circBuff.push(40, 4);
                        circBuff.push(50, 5);
                        circBuff.push(60, 6);
                        circBuff.push(70, 7);

                        THEN("The oldest value is 5")
                        {
                                int output;
                                FoundStatus result;
                                result = circBuff.get_at(10, output);
                                CHECK(result == FoundStatus::TOO_OLD);
                                CHECK(output == 5);
                        }
                        THEN("The values at each index are as expected")
                        {
                CHECK(circBuff.mBuffer.at(0).second == 7);
                CHECK(circBuff.mBuffer.at(1).second == 5);
                CHECK(circBuff.mBuffer.at(2).second == 6);
                        }
                }
        }
}

SCENARIO("The 'mNewest' and 'mOldest' iterators are in the correct place")
{
        GIVEN("A buffer of size 3")
        {
                CircularBuffer<int, 5> circBuff;
                WHEN("The buffer is wraps")
                {
                        circBuff.push(10, 1);
                        circBuff.push(20, 2);
                        circBuff.push(30, 3);
                        circBuff.push(40, 4);
                        circBuff.push(50, 5);
                        circBuff.push(60, 6);
                        circBuff.push(70, 7);

                        THEN("The iterator positions are as expected")
                        {
                CHECK(circBuff.mNewest->second == 7);
                CHECK(circBuff.mOldest->second == 3);
                        }
                }
        WHEN("the buffer is not full")
                {
                        circBuff.push(10, 1);
                        circBuff.push(20, 2);
                        circBuff.push(30, 3);

                        THEN("Iterator positions are as expected")
                        {
                CHECK(circBuff.mNewest->second == 3);
                CHECK(circBuff.mOldest->second == 1);
                        }
                }
        }
}
*/
//*********** End of white box tests**********