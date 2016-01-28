#include "omega/omega.hpp"

#include <gtest/gtest.h>

#include <tuple>
#include <sstream>

auto const t = std::make_tuple(5, 6, 7, "yuppi!", 8, 9, 10.5);

TEST(Omega, make_list)
{
    auto const t1 = std::make_tuple(6, 7, "yuppi!");
    auto const t2 = omega::make_list<1, 3>(t); // decltype(t2) = std::tuple<int, int, const char*>

    EXPECT_EQ(t1, t2);
}

TEST(Omega, invoke)
{
    std::ostringstream result;
    auto function = [&result](int a, int b, std::string const &n)
    {
        result << "a=" << a << ", b=" << b << ", n=" << n;
    };

    auto const t1 = omega::make_list<1, 3>(t);

    omega::invoke(function, t1);
    EXPECT_EQ(result.str(), "a=6, b=7, n=yuppi!");

}

TEST(Omega, reverse)
{
    auto const t1 = omega::make_list<6, 2>(t); // decltype(t4) = std::tuple<int, int, const char*, int, int>

    auto const t2 = omega::make_reverse_list<6, 2>(t); // decltype(t4) = std::tuple<int, int, const char*, int, int>

    EXPECT_EQ(std::get<3>(t1), std::get<6-3>(t));
    EXPECT_EQ(std::get<3>(t2), std::get<6-3>(t));

}

TEST(Omega, index_list)
{
    auto const t1 = omega::make_index_list<3, 2, 1, 6>(t); // decltype(t3) = std::tuple<const char*, int, int, double>

    EXPECT_EQ (std::get<3>(t1), std::get<6>(t));
}

TEST(Omega, head_tail)
{
    auto const t1 = omega::tail(t);
    EXPECT_EQ (t1, std::make_tuple(6, 7, "yuppi!", 8, 9, 10.5));

    auto const t2 = omega::head(t);
    EXPECT_EQ (t2, std::make_tuple(5));
}

TEST(Omega, init_last_reverse)
{
    auto const t1 = omega::reverse(std::make_tuple("hello", 3.14, 42));

    EXPECT_EQ (t1, std::make_tuple(42, 3.14, "hello"));

    const auto t2 = omega::last(t1);
    EXPECT_EQ (t2, std::make_tuple("hello"));

    const auto t3 = omega::init(t1);
    EXPECT_EQ (t3, std::make_tuple(42, 3.14));
}

TEST(Omega, take_drop)
{
    const auto t1 = omega::drop<2>(t);
    EXPECT_EQ (t1, std::make_tuple(7, "yuppi!", 8, 9, 10.5));

    const auto t2 = omega::take<2>(t);
    EXPECT_EQ (t2, std::make_tuple(5, 6));
}
