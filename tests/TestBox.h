//
// Created by arpradier on 24/09/2024.
//


#pragma once
#include <gtest/gtest.h>


class TestBox : public testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;
};
