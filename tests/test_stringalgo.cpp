/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2018 Pavel Kunavin <tusk.kun@gmail.com>
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
#include <algorithm>
#include "gtest/gtest.h"

#include "lib/StringAlgo.h"

using namespace ncxmms2;

TEST(forEachToken, EmptyString)
{
    std::vector<std::string> tokens;
    forEachToken<' '>("", [&tokens](const char *begin, const char *end) {
        tokens.emplace_back(begin, end);
    });
    EXPECT_TRUE(tokens.empty());
}

TEST(forEachToken, OneSeparator)
{
    const char *text = " This is a  string to tokenize ";
    std::vector<std::string> tokens;
    forEachToken<' '>(text, [&tokens](const char *begin, const char *end) {
        tokens.emplace_back(begin, end);
    });
    
    ASSERT_EQ((size_t)6, tokens.size());
    EXPECT_EQ("This",     tokens[0]);
    EXPECT_EQ("is",       tokens[1]);
    EXPECT_EQ("a",        tokens[2]);
    EXPECT_EQ("string",   tokens[3]);
    EXPECT_EQ("to",       tokens[4]);
    EXPECT_EQ("tokenize", tokens[5]);
}

TEST(forEachToken, TwoSeparators)
{
    const char *text = " This is a  list: 1,2,3,4,5, ";
    std::vector<std::string> tokens;
    forEachToken<' ', ','>(text, [&tokens](const char *begin, const char *end) {
        tokens.emplace_back(begin, end);
    });
    ASSERT_EQ((size_t)9, tokens.size());
    EXPECT_EQ("This",  tokens[0]);
    EXPECT_EQ("is",    tokens[1]);
    EXPECT_EQ("a",     tokens[2]);
    EXPECT_EQ("list:", tokens[3]);
    EXPECT_EQ("1",     tokens[4]);
    EXPECT_EQ("2",     tokens[5]);
    EXPECT_EQ("3",     tokens[6]);
    EXPECT_EQ("4",     tokens[7]);
    EXPECT_EQ("5",     tokens[8]);
}
