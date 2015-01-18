/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2015 Pavel Kunavin <tusk.kun@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <string>
#include "gtest/gtest.h"
#include "xmmsutils/result.h"

using namespace ncxmms2::xmms2;

TEST(Expected, Basics)
{
    {
        Expected<int> v(ExpectedValueTag(), 10);
        EXPECT_TRUE(v.isValid());
        EXPECT_FALSE(v.isError());
        EXPECT_EQ(10, v.value());
        EXPECT_EQ(10, *v);
        
        v.value() = 15;
        EXPECT_TRUE(v.isValid());
        EXPECT_EQ(15, v.value());
        EXPECT_EQ(15, *v);
        
        *v = 25;
        EXPECT_TRUE(v.isValid());
        EXPECT_EQ(25, v.value());
        EXPECT_EQ(25, *v);
    }
    
    {
        Expected<int> v(ExpectedErrorTag(), "error");
        EXPECT_FALSE(v.isValid());
        EXPECT_TRUE(v.isError());
        EXPECT_EQ("error", v.error());
    }
    
    {
        const auto v = expectedFromValue(10); // 10 is rvalue
        EXPECT_TRUE(v.isValid());
        EXPECT_FALSE(v.isError());
        EXPECT_EQ(10, v.value());
        EXPECT_EQ(10, *v);
    }
    
    {
        int value = 20;
        auto v = expectedFromValue(value); // value is lvalue
        EXPECT_TRUE(v.isValid());
        EXPECT_FALSE(v.isError());
        EXPECT_EQ(value, v.value());
    }
    
    {
        const int value = 30;
        auto v = expectedFromValue(value); // value is const lvalue
        EXPECT_TRUE(v.isValid());
        EXPECT_FALSE(v.isError());
        EXPECT_EQ(value, v.value());
    }
    
    {
        auto v = expectedFromError<int>("error");
        EXPECT_FALSE(v.isValid());
        EXPECT_TRUE(v.isError());
        EXPECT_EQ("error", v.error());
    }
}

TEST(Expected, VariadicConstructor)
{
    {
        auto v = expectedConstructValue<std::string>();
        EXPECT_TRUE(v->empty());
    }
    
    {
        auto v = expectedConstructValue<std::string>("str");
        EXPECT_EQ("str", v.value());
    }
    
    {
        auto v = expectedConstructValue<std::string>(3, 'x');
        EXPECT_EQ("xxx", v.value());
    }
}

TEST(Expected, MoveSemantics)
{
    auto v1 = expectedFromValue(20);
    Expected<int> v2(std::move(v1));
    EXPECT_TRUE(v2.isValid());
    EXPECT_EQ(20, v2.value());
    
    auto v3 = expectedFromError<int>("error");
    v2 = std::move(v3);
    EXPECT_TRUE(v2.isError());
    EXPECT_EQ("error", v2.error());    
}
