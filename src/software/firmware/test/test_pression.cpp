/**
 * @file test_pression.cpp
 * @author Makers For Life
 * @brief Unit tests for pression.cpp
 * @version 0.1
 * @date 2020-04-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <gtest/gtest.h>
#include <vector>
#include <iostream>
#include "../includes/pression.h"

/**
 * @brief Test Fixture to test the pression file
 * 
 */
class PressionTest : public ::testing::Test 
{
protected:
    PressionTest()
    {

    }

};


TEST_F(PressionTest, testReadPressureSensor)
{
    std::vector<uint16_t> input = {0, 100, 1000, 5000, 8000, 16000, 24000, 65535};
    std::vector<uint16_t> output = {0, 0, 144, 983, 2181, 4543, 7837, 17761};

    for(int i = 0; i < input.size(); i++)
    {
        ASSERT_EQ(output[i], convertSensor2Pressure(input[i]));
    }
}
