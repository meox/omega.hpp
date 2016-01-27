#![omega](/images/omega_logo.png).hpp
omega.hpp is a tiny, C++14 meta-library to manipulate tuple at compile time.

### Examples
All following examples are also implemented as google test unit tests (see *Test* folder)

#### 1. first example
```cpp
#include <iostream>
#include <tuple>
#include "omega.hpp"

void print(int a, int b, const std::string& n)
{
	std::cout << "a = " << a << ", b = " << b << ", n = " << n << std::endl;
}

int main()
{
  // create a tuple
	const auto t = std::make_tuple(5, 6, 7, "yuppi!", 8, 9, 10.5);
	
	// create another tuple based on t, using a list from 1 to 3 ([1,2,3])
	auto t2 = omega::make_list<1, 3>(t); // decltype(t2) = std::tuple<int, int, const char*>
	
	// use t2 as usual
	std::cout << std::get<0>(t2) << std::endl; // out: 6
	omega::invoke(print, t2); // out: a = 6, b = 7, n = yuppi!
	
	// reverse list are also supported: omega::make_reverse_list
	auto t4 = omega::make_reverse_list<5, 1>(t); // decltype(t4) = std::tuple<int, int, const char*, int, int>
	std::cout << std::get<2>(t4) << std::endl; // out: yuppi!
	
	// or we can use index : omega::make_index_list
	auto t3 = omega::make_index_list<3, 2, 1, 6>(t); // decltype(t3) = std::tuple<const char*, int, int, double>
	std::cout << std::get<3>(t3) << std::endl; // out: 10.5
	
	omega::invoke(print, omega::make_index_list<2, 5, 3>(t)); // out: a = 7, b = 9, n = yuppi!
}
```

#### 2. head, tail
```cpp
#include <iostream>
#include <tuple>
#include "omega.hpp"

int main()
{
	const auto t = std::make_tuple(5, 6, 7, "yuppi!", 8, 9, 10.5);
	std::cout << "t = " << t << std::endl; // out: t = <5, 6, 7, yuppi!, 8, 9, 10.5>
	
	auto t1 = omega::tail(t);
	std::cout << "tail(t) = " << t1 << std::endl; // out: tail(t) = <6, 7, yuppi!, 8, 9, 10.5>

	auto t2 = omega::head(t);
	std::cout << "head(t) = " << t2 << std::endl; // out: head(t) = <5>
}
```

#### 3. init, last, reverse
```cpp
#include <iostream>
#include <tuple>
#include "omega.hpp"

int main()
{
	const auto t = omega::reverse(std::make_tuple("hello", 3.14, 42));
	std::cout << "t = " << t << std::endl; // out: t = <42, 3.14, hello>

	const auto t1 = omega::last(t);
	std::cout << "last(t) = " << t1 << std::endl; // out: last(t) = <hello>

	const auto t2 = omega::init(t);
	std::cout << "init(t) = " << t2 << std::endl; // out: init(t) = <42, 3.14>
}
```

#### 4. take, drop
```cpp
#include <iostream>
#include <tuple>
#include "omega.hpp"

int main()
{
	const auto t = std::make_tuple(42, "wood", 5.1, "world!");
	const auto t1 = omega::drop<1>(t);
	std::cout << "drop<1>(t) = " << t1 << std::endl; // out: drop<1>(t) = <wood, 5.1, world!>

	const auto t2 = omega::take<3>(t);
	std::cout << "take<3>(t) = " << t2 << std::endl; // out: take<3>(t) = <42, wood, 5.1>
}
```
### Supported Platforms and Compilers:
1. Linux:
 - GCC 4.9.2+
 - Clang 3.5+
2. OSX:
 - Either Apple's provided clang (from Xcode >= 7.0) or standard Clang 3.5+

### Requires CMake >= 3.0

### How to compile
```bash
mkdir BUILD
cd BUILD
cmake <path-to-omega.hpp-folder>
make                        #Note: this will update the gtest submodule
{make test | ./_Test_Omega} #i.e. either run all tests under cmake, or explicitly invoke the gtest application
```

