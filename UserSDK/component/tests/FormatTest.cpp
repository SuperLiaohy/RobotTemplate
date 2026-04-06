#include <gtest/gtest.h>
#include "../Format.h"

using namespace EP::Component;

TEST(FormatTest, CompileTimeParsing) {
    constexpr Str fmt1{"Hello {} World {}"};
    static_assert(fmt1.formatCount() == 2);
    
    constexpr Str fmt2{"A{3.2}B{}"};
    static_assert(fmt2.formatCount() == 2);
    
    constexpr auto features = fmt2.formatFeature<2>();
    static_assert(features[0].int_width == 3);
    static_assert(features[0].float_width == 2);
    static_assert(features[1].int_width == 0);
    static_assert(features[1].float_width == 0);
}

TEST(FormatTest, IntegerFormatting) {
    char buf[100] = {0};
    std::size_t len = format<Str{"Int: {}, UInt: {}"}>(buf, -42, 100u);
    buf[len] = '\0';
    EXPECT_EQ(buf[len-1], '\0');
    EXPECT_STREQ(buf, "Int: -42, UInt: 100");
}

TEST(FormatTest, BooleanFormatting) {
    char buf[100] = {0};
    std::size_t len = format<Str{"Bools: {}, {}"}>(buf, true, false);
    buf[len] = '\0';
    EXPECT_STREQ(buf, "Bools: True, False");
}

TEST(FormatTest, FloatFormatting) {
    char buf[100] = {0};
    std::size_t len = format<Str{"Float: {0.2}"}>(buf, 3.14159f);
    buf[len] = '\0';
    EXPECT_STREQ(buf, "Float: 3.14");
    
    len = format<Str{"Float: {0.0}"}>(buf, 3.14159f);
    buf[len] = '\0';
    EXPECT_STREQ(buf, "Float: 3");
}

TEST(FormatTest, StringFormatting) {
    char buf[100] = {0};
    const char str[] = "Test";
    std::size_t len = format<Str{"Str: {}"}>(buf, str);
    buf[len] = '\0';
    EXPECT_STREQ(buf, "Str: Test\0");
}
