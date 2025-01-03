#include <gtest/gtest.h>

#include <sow/sow.hpp>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
struct CustomData {
  int a{0};
  int b{0};
};

struct DataWriter {
  bool saved{false};

  void operator()(const CustomData& data) {
    (void)data;
    saved = true;
  }
};

TEST(SowTest, SowConstGet) {
  DataWriter writer;
  CustomData custom_data;
  sow::Sow sow{custom_data, &writer};

  EXPECT_EQ(sow.get().a, 0);
  EXPECT_EQ(sow.get().b, 0);
  EXPECT_FALSE(writer.saved);
}

TEST(SowTest, SowConstDeref) {
  DataWriter writer;
  CustomData custom_data;
  sow::Sow sow{custom_data, &writer};

  EXPECT_EQ((*sow).a, 0);
  EXPECT_EQ((*sow).b, 0);
  EXPECT_FALSE(writer.saved);
}

TEST(SowTest, SowConstArrow) {
  DataWriter writer;
  CustomData custom_data;
  sow::Sow sow{custom_data, &writer};

  EXPECT_EQ(sow->a, 0);
  EXPECT_EQ(sow->b, 0);
  EXPECT_FALSE(writer.saved);
}

TEST(SowTest, SowGuardOnce) {
  DataWriter writer;
  sow::Sow data{CustomData{}, &writer};

  // Mutates the data and drop the guard immediately. This triggers the save
  // function in the Sow
  data.get_mut()->a = 789;

  EXPECT_EQ(data->a, 789);
  EXPECT_EQ(data->b, 0);
  EXPECT_TRUE(writer.saved);
}

TEST(SowTest, SowGuardMultiple) {
  DataWriter writer;
  sow::Sow data{CustomData{}, &writer};

  {
    auto guard = data.get_mut();
    guard->a = 789;
    guard->b = 101112;
  }

  EXPECT_EQ(data->a, 789);
  EXPECT_EQ(data->b, 101112);
  EXPECT_TRUE(writer.saved);
}

TEST(SowTest, SowGuardMultipleWithFunc) {
  DataWriter writer;
  sow::Sow data{CustomData{}, &writer};

  sow::it(data, [](auto& data) {
    data.a = 789;
    data.b = 101112;
  });

  EXPECT_EQ(data->a, 789);
  EXPECT_EQ(data->b, 101112);
  EXPECT_TRUE(writer.saved);
}

TEST(SowTest, SowWithDefaultConstructor) {
  sow::Sow<CustomData, DataWriter> data;
  {
    auto guard = data.get_mut();
    guard->a = 789;
    guard->b = 101112;
  }

  EXPECT_EQ(data->a, 789);
  EXPECT_EQ(data->b, 101112);
}

TEST(SowTest, SowWithDefaultConstructedWriter) {
  sow::Sow<CustomData, DataWriter> data{CustomData{}};
  {
    auto guard = data.get_mut();
    guard->a = 789;
    guard->b = 101112;
  }

  EXPECT_EQ(data->a, 789);
  EXPECT_EQ(data->b, 101112);
}

TEST(SowTest, SowWithLambdaWriter) {
  bool saved = false;
  sow::Sow data{CustomData{}, [&saved](const CustomData&) {
                  saved = true;
                }};
  {
    auto guard = data.get_mut();
    guard->a = 789;
    guard->b = 101112;
  }

  EXPECT_EQ(data->a, 789);
  EXPECT_EQ(data->b, 101112);
  EXPECT_TRUE(saved);
}

void save_data(const CustomData&) {}

TEST(SowTest, SowWithFreeFuncWriter) {
  sow::Sow data{CustomData{}, save_data};
  {
    auto guard = data.get_mut();
    guard->a = 789;
    guard->b = 101112;
  }

  EXPECT_EQ(data->a, 789);
  EXPECT_EQ(data->b, 101112);
}

TEST(SowTest, SowTrivialDataType) {
  bool saved = false;
  sow::Sow data{42, [&saved](const auto&) {
                  saved = true;
                }};

  *data.get_mut() = 789;

  EXPECT_EQ(*data, 789);
  EXPECT_TRUE(saved);
}

TEST(SowTest, SowSTLType) {
  bool saved = false;
  sow::Sow data{std::vector<int>{}, [&saved](const auto&) {
                  saved = true;
                }};

  data.get_mut()->push_back(42);

  EXPECT_EQ(data->size(), 1);
  EXPECT_EQ(data->at(0), 42);
  EXPECT_TRUE(saved);
}
