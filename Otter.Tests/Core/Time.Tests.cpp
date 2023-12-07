#include <gtest/gtest.h>

#include "Core/Time.h"

using Time = Otter::Time;
using TimeConfiguration = Otter::TimeConfiguration;

TEST(Time, Initialisation_Default)
{
    TimeConfiguration config{ 30.0, 60.0, 0.01 };

    Time time(config, []() { return 0.0; });

    EXPECT_EQ(time.GetFixedDeltaTime(), 0.01);
}

TEST(Time, Initialisation_Invalid)
{
    EXPECT_DEATH(Time time(TimeConfiguration{ }, nullptr), "");
    EXPECT_DEATH(Time time(TimeConfiguration{ 60.0, 30.0, 0.01 }, []() { return 0.0; }), "");
    EXPECT_DEATH(Time time(TimeConfiguration{ 30.0, 60.0, -0.01 }, []() { return 0.0; }), "");
}

TEST(Time, Start_Refresh)
{
    bool called = false;

    Time time({ 0.0, 0.0, 0.01 },
              [&called]()
              {
                  if (!called)
                  {
                      called = true;
                      return 0.0;
                  }
                  else
                  {
                      return 0.1;
                  }
              });

    time.Start();
    time.Refresh();

    UInt16 fixedUpdates = 0;

    while (time.HasFixedStepsLeft())
        ++fixedUpdates;

    EXPECT_GT(time.GetDeltaTime(), 0.0); // Doesn't work with EXPECT_EQ(dt, 0.1) because of floating point precision.
    EXPECT_EQ(fixedUpdates, 10);
}

TEST(Time, LimitMinFrameRate)
{
    bool called = false;

    Time time({ 30.0, 0.0, 0.01 },
              [&called]()
              {
                  if (!called)
                  {
                      called = true;
                      return 0.0;
                  }
                  else
                  {
                      return 0.01;
                  }
              });

    time.Start();
    time.Refresh();

    UInt16 fixedUpdates = 0;

    while (time.HasFixedStepsLeft())
        ++fixedUpdates;

    EXPECT_GT(time.GetDeltaTime(), 0.0); // Doesn't work with EXPECT_EQ(dt, 0.1) because of floating point precision.
    EXPECT_EQ(fixedUpdates, 3);
}

TEST(Time, LimitMaxFrameRate)
{
    bool called = false;

    Time time({ 0.0, 60.0, 0.01 },
              [&called]()
              {
                  if (!called)
                  {
                      called = true;
                      return 0.0;
                  }
                  else
                  {
                      return 0.1;
                  }
              });

    time.Start();
    time.Refresh();

    UInt16 fixedUpdates = 0;

    while (time.HasFixedStepsLeft())
        ++fixedUpdates;

    EXPECT_GT(time.GetDeltaTime(), 0.0); // Doesn't work with EXPECT_EQ(dt, 0.1) because of floating point precision.
    EXPECT_EQ(fixedUpdates, 1);
}