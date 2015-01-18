/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2014 Pavel Kunavin <tusk.kun@gmail.com>
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

#include "gtest/gtest.h"
#include "filesystembrowser/dir.h"

using namespace ncxmms2;

TEST(Dir, Construct)
{
    {
        Dir dir("/dir/subdir/");
        EXPECT_EQ("subdir", dir.name());
        EXPECT_EQ("/dir/subdir", dir.path());
    }
    
    {
        Dir dir("/dir1/../dir2");
        EXPECT_EQ("dir2", dir.name());
        EXPECT_EQ("/dir2", dir.path());
    }
    
    {
        Dir dir("smb://host/dir1/../dir2");
        EXPECT_EQ("dir2", dir.name());
        EXPECT_EQ("host/dir2", dir.path());
    }
}

TEST(Dir, ParseProtocol)
{
    {
        Dir dir("/dir");
        EXPECT_EQ("/dir", dir.path());
        EXPECT_EQ("file", dir.protocol());
        EXPECT_EQ("file:///dir", dir.url());
    }
    
    {
        Dir dir("file:///dir");
        EXPECT_EQ("/dir", dir.path());
        EXPECT_EQ("file", dir.protocol());
        EXPECT_EQ("file:///dir", dir.url());
    }
    
    {
        Dir dir("smb://host/");
        EXPECT_EQ("host", dir.path());
        EXPECT_EQ("smb", dir.protocol());
        EXPECT_EQ("smb://host", dir.url());
    }

}

TEST(Dir, name)
{
    {
        Dir dir("/dir/subdir");
        EXPECT_EQ("subdir", dir.name());
    }
    
    {
        Dir dir("/");
        EXPECT_EQ("/", dir.name());
    }
    
    {
        Dir dir("smb://host");
        EXPECT_EQ("host", dir.name());
    }
}

TEST(Dir, isRootPath)
{
    {
        Dir dir("/");
        EXPECT_TRUE(dir.isRootPath());
    }
    
    {
        Dir dir("/dir");
        EXPECT_FALSE(dir.isRootPath());
    }
    
    {
        Dir dir("smb://host");
        EXPECT_TRUE(dir.isRootPath());
    }
    
    {
        Dir dir("smb://host/share");
        EXPECT_FALSE(dir.isRootPath());
    }
}

TEST(Dir, cd)
{
    {
        Dir dir("/");
        dir.cd("dir");
        EXPECT_EQ("dir", dir.name());
        EXPECT_EQ("/dir", dir.path());
    }
    
    {
        Dir dir("/dir");
        dir.cd("subdir/");
        EXPECT_EQ("subdir", dir.name());
        EXPECT_EQ("/dir/subdir", dir.path());
    }
    
    {
        Dir dir("/dir/subdir");
        dir.cd("subsubdir");
        EXPECT_EQ("subsubdir", dir.name());
        EXPECT_EQ("/dir/subdir/subsubdir", dir.path());
    }
    
    {
        Dir dir("/dir");
        dir.cd("subdir/subsubdir with spaces");
        EXPECT_EQ("subsubdir with spaces", dir.name());
        EXPECT_EQ("/dir/subdir/subsubdir with spaces", dir.path());
    }
    
    {
        Dir dir("/dir");
        dir.cd("/");
        EXPECT_EQ("/", dir.path());
    }
    
    {
        Dir dir("/dir");
        dir.cd("/dir1");
        EXPECT_EQ("dir1", dir.name());
        EXPECT_EQ("/dir1", dir.path());
    }
    
    {
        Dir dir("/dir");
        dir.cd(".");
        EXPECT_EQ("/dir", dir.path());
    }
    
    {
        Dir dir("/");
        dir.cd("..");
        EXPECT_EQ("/", dir.path());
    }
    
    {
        Dir dir("/dir");
        dir.cd("..");
        EXPECT_EQ("/", dir.path());
    }
    
    {
        Dir dir("/dir/subdir");
        dir.cd("..");
        EXPECT_EQ("dir", dir.name());
        EXPECT_EQ("/dir", dir.path());
    }
    
    {
        Dir dir("/dir/subdir");
        dir.cd("../dir1/../dir2");
        EXPECT_EQ("/dir/dir2", dir.path());
    }
    
    {
        Dir dir("/dir/subdir");
        dir.cd("../../../../../../dir1/./dir2");
        EXPECT_EQ("/dir1/dir2", dir.path());
    }
    
    {
        Dir dir("/dir/subdir");
        dir.cd("/dir1/../dir2");
        EXPECT_EQ("/dir2", dir.path());
    }
    
    {
        Dir dir("/dir");
        dir.cd("file:///dir2");
        EXPECT_EQ("/dir2", dir.path());
        EXPECT_EQ("file", dir.protocol());
    }
    
    {
        Dir dir("/dir");
        dir.cd("ftp://locallhost");
        EXPECT_EQ("locallhost", dir.path());
        EXPECT_EQ("ftp", dir.protocol());
    }
    
    {
        Dir dir("ftp://locallhost");
        dir.cd("subdir");
        EXPECT_EQ("locallhost/subdir", dir.path());
        EXPECT_EQ("ftp://locallhost/subdir", dir.url());
        EXPECT_EQ("ftp", dir.protocol());
    }
    
    {
        Dir dir("ftp://locallhost");
        dir.cd("/dir");
        EXPECT_EQ("/dir", dir.path());
        EXPECT_EQ("file", dir.protocol());
    }
    
    {
        Dir dir("ftp://locallhost");
        dir.cd("file:///dir2");
        EXPECT_EQ("/dir2", dir.path());
        EXPECT_EQ("file", dir.protocol());
    }
    
    {
        Dir dir("ftp://locallhost");
        dir.cd("smb://host/share");
        EXPECT_EQ("host/share", dir.path());
        EXPECT_EQ("smb", dir.protocol());
    }
    
    {
        Dir dir("ftp://locallhost");
        dir.cd("..");
        EXPECT_EQ("locallhost", dir.path());
        EXPECT_EQ("ftp", dir.protocol());
    }
    
    {
        Dir dir("/dir");
        dir.cd("");
        EXPECT_EQ("/dir", dir.path());
    }
    
    {
        Dir dir("/dir");
        dir.cd("smb://");
        EXPECT_EQ("", dir.path());
        EXPECT_EQ("smb", dir.protocol());
    }
}

TEST(Dir, cdUp)
{
    {
        Dir dir("/");
        dir.cdUp();
        EXPECT_EQ("/", dir.path());
    }
    
    {
        Dir dir("/dir");
        dir.cdUp();
        EXPECT_EQ("/", dir.path());
    }
    
    {
        Dir dir("/dir/subdir");
        dir.cdUp();
        EXPECT_EQ("/dir", dir.path());
    }
    
    {
        Dir dir("ftp://locallhost");
        dir.cdUp();
        EXPECT_EQ("locallhost", dir.path());
    }
    
    {
        Dir dir("ftp://locallhost/share");
        dir.cdUp();
        EXPECT_EQ("locallhost", dir.path());
    }
    
    {
        Dir dir("/dir/subdir");
        dir.cd("smb://host/share/../share2");
        EXPECT_EQ("host/share2", dir.path());
    }
}
