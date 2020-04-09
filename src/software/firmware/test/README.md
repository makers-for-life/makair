# How to build and run the tests

## Install Google Test

1. Install cmake
2. Get the last version of google test from https://github.com/google/googletest/releases and unzip it
3. Go to the google test folder
4. Run:
   ```
   cmake .
   sudo cmake --build . --target install
   ```

## Build the tests

1. Create a build folder and navigate to it
2. Run:
    ```
    cmake PATH/TO/src/software/firmware/test
    cmake --build .
    ```
In the build folder, the test executable will be created.

## Run the tests
The test executable can be run individualy or by calling `ctest`

# How to add Tests

## Create test source code
In a Cpp file, create a test fixture class. For example, see the following code (taken from https://www.linuxembedded.fr/2014/09/introduction-a-google-c-testing-framework/)

```cpp
// The fixture for testing class Foo.
class FooTest : public ::testing::Test 
{
    protected:
    // You can remove any or all of the following functions if its body
    // is empty.
    FooTest() 
    {
        // You can do set-up work for each test here.
    }
    virtual ~FooTest() 
    {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    virtual void SetUp() 
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    virtual void TearDown() 
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }
    // Objects declared here can be used by all tests in the test case for Foo.
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(FooTest, MethodBarDoesAbc) 
{
    const string input_filepath = "this/package/testdata/myinputfile.dat";
    const string output_filepath = "this/package/testdata/myoutputfile.dat";
    Foo f;
    EXPECT_EQ(0, f.Bar(input_filepath, output_filepath));
}

// No Need for main function
``` 

## Add the test source file to CMakeLists.txt

We need to create a new executable for the new test fixture. Add the following code to the CMakeList.txt

```cmake
set(NEW_TEST_SRC test_foo.cpp
                 # Other needed source files                        
)

add_executable(test_foo ${NEW_TEST_SRC})
target_link_libraries(test_foo GTest::GTest GTest::Main)

add_test(TestFoo test_foo) # add the test to the registry to be run with ctest
```

