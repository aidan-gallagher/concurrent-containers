#include "SearchRingBuffer.h"
#include "catch.hpp"

#include <string>

using minutes = std::chrono::minutes;
using hours = std::chrono::hours;
using sysClock = std::chrono::system_clock;

SCENARIO("Basic use of circular buffer using std::string data")
{
  GIVEN("A buffer of size 10 is created")
  {
    Concurrent::SearchRingBuffer<std::string, 10> circBuff;
    WHEN("std::string data is inserted along side its associated double time")
    {
      auto time = sysClock::now();
      circBuff.push(time, "Hello");
      circBuff.push(time + minutes(1), "world");
      circBuff.push(time + hours(1), "Bye");
      THEN("The data can be retrieved using the time as a key")
      {
        CHECK(circBuff.read(time) == "Hello");
        CHECK(circBuff.read(time + minutes(1)) == "world");
        CHECK(circBuff.read(time + hours(1)) == "Bye");
      }
    }
  }
}

SCENARIO("Requesting data from an empty buffer")
{
  bool exception_triggered{ false };

  GIVEN("An empty buffer")
  {
    Concurrent::SearchRingBuffer<int, 20> circBuff;
    WHEN("An element is requested")
    {
      try
      {
        auto result = circBuff.read(sysClock::now());
      }
      catch (const Concurrent::SearchRingBuffer<int, 20>::BufferEmpty& e)
      {
        CHECK("Error trying to read data from empty circular buffer" == std::string{ e.what() });
        exception_triggered = true;
      }

      THEN("An exception will be triggered") { CHECK(exception_triggered == true); }
    }
  }
}

/*
struct NotDefaultConstructable{
        NotDefaultConstructable(int val){};
};
SCENARIO ("Store non default constructable data")
{

    GIVEN  ("A buffer non default constructable data")
        {
        Concurrent::SearchRingBuffer<NotDefaultConstructable, 10> circBuff;
        WHEN ("Items are stored")
                 {
                        // NotDefaultConstructable a{5};
                         /*
                        auto time = sysClock::now();
            circBuff.push(time,	NotDefaultConstructable{5});
            circBuff.push(time + minutes(1), NotDefaultConstructable{10});
            THEN ("The data can be retrieved using the time as a key")
                        {
                        CHECK ( circBuff.read(time) == NotDefaultConstructable{5});
                                CHECK ( circBuff.read(time + minutes(1)) == NotDefaultConstructable{10});
           }
                   *//*
        }
    }
}
*/
SCENARIO("Requesting data from a buffer with only 1 element")
{
  GIVEN("A buffer with one item")
  {
    Concurrent::SearchRingBuffer<int, 20> circBuff;
    circBuff.push(sysClock::now(), 1);

    WHEN("An element is requested")
    {
      auto item = circBuff.read(sysClock::now());
      THEN("The item is returned") { CHECK(item == 1); }
    }
  }
}

SCENARIO("Retrieving data which is outwith the limits")
{
  GIVEN("A buffer with some data")
  {
    Concurrent::SearchRingBuffer<int, 20> circBuff;
    auto                                  time = sysClock::now();
    circBuff.push(time, 1);
    circBuff.push(time + minutes(1), 2);
    circBuff.push(time + minutes(2), 3);
    WHEN("Requesting data which is too old")
    {
      auto item = circBuff.read(time - minutes(1));
      THEN("The item is the oldest element") { CHECK(item == 1); }
    }
    WHEN("Requesting data which is too new")
    {
      auto item = circBuff.read(time + minutes(3));
      THEN("The item is the newest element") { CHECK(item == 3); }
    }
  }
}

SCENARIO("A buffer with a lot of elements can be used")
{
  GIVEN("A buffer of size 47832")
  {
    Concurrent::SearchRingBuffer<int, 47832> circBuff;
    WHEN("100,000 elements are inserted")
    {
      auto time = sysClock::now();
      for (int i = 0; i < 100000; ++i)
      {
        circBuff.push(time + hours(i), i);
      }

      THEN("The 600000th element can be retrieved") { CHECK(circBuff.read(time + hours(60000)) == 60000); }
    }
  }
}

SCENARIO("When two elements have the same time the earliest one will be retrieved")
{
  GIVEN("A buffer with two elements with the same time")
  {
    Concurrent::SearchRingBuffer<int, 5> circBuff;
    auto                                 time = sysClock::now();

    circBuff.push(time, 1);
    circBuff.push(time + minutes(1), 2);
    circBuff.push(time + minutes(1), 3);
    circBuff.push(time + minutes(2), 4);

    WHEN("10 is requested")
    {
      int result = circBuff.read(time + minutes(1));
      THEN("the earlier element with time = 10 will be retrieved") { CHECK(result == 2); }
    }
  }
}

SCENARIO("When an even sized buffer has wrapped")
{
  GIVEN("A buffer of size of 6")
  {
    Concurrent::SearchRingBuffer<int, 6> circBuff;

    WHEN("More than 6 elements are pushed in")
    {
      auto time = sysClock::now();

      circBuff.push(time, 'a');
      circBuff.push(time + minutes(2), 'b');
      circBuff.push(time + minutes(3), 'c');
      circBuff.push(time + minutes(4), 'd');
      circBuff.push(time + minutes(5), 'e');
      circBuff.push(time + minutes(6), 'f');
      circBuff.push(time + minutes(7), 'g');
      circBuff.push(time + minutes(8), 'h');
      circBuff.push(time + minutes(9), 'i');
      THEN("the last 6 elements can be retrieved")
      {
        CHECK(circBuff.read(time + minutes(4)) == 'd');
        CHECK(circBuff.read(time + minutes(9)) == 'i');
      }
    }
  }
}

SCENARIO("When an odd sized buffer has wrapped")
{
  GIVEN("A buffer of size of 7")
  {
    Concurrent::SearchRingBuffer<char, 7> circBuff;

    WHEN("More than 7 elements are pushed in")
    {
      auto time = sysClock::now();
      circBuff.push(time + minutes(1), 'a');
      circBuff.push(time + minutes(2), 'b');
      circBuff.push(time + minutes(3), 'c');
      circBuff.push(time + minutes(4), 'd');
      circBuff.push(time + minutes(5), 'e');
      circBuff.push(time + minutes(6), 'f');
      circBuff.push(time + minutes(7), 'g');
      circBuff.push(time + minutes(8), 'h');
      circBuff.push(time + minutes(9), 'i');
      THEN("The last 7 elements can be retrieved")
      {
        CHECK(circBuff.read(time + minutes(3)) == 'c');

        CHECK(circBuff.read(time + minutes(9)) == 'i');
      }
    }
  }
}

SCENARIO("Attempt to insert an item which is older than the newest item")
{
  bool exception_triggered{ false };
  GIVEN("A buffer where the most recent element has a time of +3 hours")
  {
    Concurrent::SearchRingBuffer<char, 3> circBuff;
    auto                                  time = sysClock::now();
    circBuff.push(time, 'a');
    circBuff.push(time + hours(1), 'b');
    circBuff.push(time + hours(2), 'c');
    circBuff.push(time + hours(3), 'd');

    WHEN("An old item (less than +3 hours) is inserted")
    {
      try
      {
        circBuff.push(time + hours(3) - minutes(1), 'e');
      }
      catch (const Concurrent::SearchRingBuffer<char, 3>::ItemTooOld& e)
      {
        CHECK("Error trying to insert old data into the buffer" == std::string{ e.what() });
        exception_triggered = true;
      }
      THEN("An exception is thrown") { CHECK(exception_triggered == true); }
      THEN("the element is not in the buffer") { CHECK(circBuff.read(time + hours(4)) == 'd'); }
    }
  }
}

SCENARIO("The circular buffer is full but has not wrapped yet")
{
  GIVEN("A buffer of size 5 with 5 elements in it")
  {
    Concurrent::SearchRingBuffer<char, 5> circBuff;
    auto                                  time = sysClock::now();

    circBuff.push(time + minutes(4), 'a');
    circBuff.push(time + minutes(20), 'b');
    circBuff.push(time + minutes(50), 'c');
    circBuff.push(time + minutes(100), 'd');
    circBuff.push(time + minutes(150), 'e');

    THEN("The data is retrievable")
    {
      CHECK(circBuff.read(time + minutes(4)) == 'a');
      CHECK(circBuff.read(time + minutes(150)) == 'e');
    }
  }
}

SCENARIO("The circular buffer is not full")
{
  GIVEN("A buffer of size 50 with 5 elements in it")
  {
    Concurrent::SearchRingBuffer<char, 50> circBuff;
    auto                                   time = sysClock::now();
    circBuff.push(time + minutes(4), 'a');
    circBuff.push(time + minutes(20), 'b');
    circBuff.push(time + minutes(50), 'c');
    circBuff.push(time + minutes(100), 'd');
    circBuff.push(time + minutes(150), 'e');

    THEN("The data is retrievable")
    {
      CHECK(circBuff.read(time + minutes(4)) == 'a');
      CHECK(circBuff.read(time + minutes(150)) == 'e');
    }
  }
}

SCENARIO("The requested time is in between")
{
  GIVEN("A buffer of size 7 with 5 elements in it")
  {
    Concurrent::SearchRingBuffer<char, 50> circBuff;
    auto                                   time = sysClock::now();
    circBuff.push(time + minutes(10), 'a');
    circBuff.push(time + minutes(20), 'b');
    circBuff.push(time + minutes(30), 'c');
    circBuff.push(time + minutes(40), 'd');
    circBuff.push(time + minutes(50), 'e');
    circBuff.push(time + minutes(60), 'f');
    circBuff.push(time + minutes(70), 'g');
    circBuff.push(time + minutes(80), 'h');
    circBuff.push(time + minutes(90), 'i');
    circBuff.push(time + minutes(100), 'j');

    THEN("The data is retrievable") { CHECK(circBuff.read(time + minutes(73)) == 'g'); }
    THEN("The data is retrievable") { CHECK(circBuff.read(time + minutes(78)) == 'h'); }
  }
}

SCENARIO("When an element is requested that does not exist the closest match shall be returned")
{
  GIVEN("A buffer without the value 4 or 6")
  {
    Concurrent::SearchRingBuffer<char, 6> circBuff;
    auto                                  time = sysClock::now();
    circBuff.push(time + minutes(1), 1);
    circBuff.push(time + minutes(1), 2);
    circBuff.push(time + minutes(10), 10);
    circBuff.push(time + minutes(20), 20);
    circBuff.push(time + minutes(30), 30);
    circBuff.push(time + minutes(40), 40);
    WHEN("4 is requested")
    {
      int result = circBuff.read(time + minutes(4));
      THEN("the closest match (10) will be retrieved") { CHECK(result == 2); }
    }
    WHEN("6 is requested")
    {
      int result = circBuff.read(time + minutes(6));
      THEN("the closest match (10) will be retrieved") { CHECK(result == 10); }
    }
    WHEN("The smallest element is requested")
    {
      int result = circBuff.read(time - minutes(5));
      THEN("the closest match (1) will be retrieved") { CHECK(result == 1); }
    }
    WHEN("The largest element is requested")
    {
      int result = circBuff.read(time + hours(5));
      THEN("the closest match (1) will be retrieved") { CHECK(result == 40); }
    }
  }
}