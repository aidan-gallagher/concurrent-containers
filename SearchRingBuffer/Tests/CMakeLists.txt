add_executable (SearchRingBufferTest Main.cpp
                                     SequentialTests.cpp)

target_link_libraries (SearchRingBufferTest PRIVATE Catch2)           # link to the testing library
target_link_libraries (SearchRingBufferTest PRIVATE Concurrent::SearchRingBuffer)  # link to the business logic to be tested

# Add test to CTest. Define the command used to run the test (catch flags)
# Catch2 flag info: https://github.com/catchorg/Catch2/blob/master/docs/command-line.md
add_test(NAME SearchRingBufferTest
         COMMAND SearchRingBufferTest --reporter junit
                                      --name Containers
                                      --filenames-as-tags
                                      --success
                                      --out ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SearchRingBufferTestResults.xml)

# Add CTest label. Tests with the same label are grouped together.
set_tests_properties (SearchRingBufferTest PROPERTIES LABELS Containers)
