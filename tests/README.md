# 测试说明

项目使用 `Catch 2` 进行测试，具体用法详见 [官方教程](https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#top) 。catch 头文件放在 `tests/catch.cpp` 中，直接包含即可。

## 测试方法

执行 `make` 之后会在 build 目录生成一个 `testall` 程序，可以直接运行这个程序，或者使用 `ctest -V` 执行测试。（推荐直接执行，因为有高亮显示）

```bash
cd build
cmake ..
make
./testall
```

也可以为 testall 指定参数，执行指定名称或者标签的测试，不写参数的话会执行所有测试。

```bash
./testall "[vdisk]"		# 只执行标签为 vdisk 的所有测试
./testall "Example*"	# 执行名字以 Example 开头的所有测试
./testall "[vdisk],[utils]"	# 执行标签为 vdisk 或者 utils 的所有测试
./testall "[vdisk][utils]"	# 执行同时拥有 vdisk 和 utils 标签的所有测试
```



## 测试用例编写方法

在 `tests` 目录中，`test.cpp` 是测试的主文件，其开头有 `#define CATCH_CONFIG_MAIN` 和 `#include "catch.hpp"` 两行语句，一般不直接在这个文件中编写测试。

在 `tests` 目录中新建 `xx.cpp` 文件以编写针对特定部分功能的测试，只要格式没写错，文件中的测试会被自动执行，无需进行什么额外操作。

在 `xx.cpp` 中要 `#include "catch.hpp"` ，但**不要**再次 `#define CATCH_CONFIG_MAIN` ！

由于 Catch 主要是针对 C++ 的测试程序，所以要用 C++ 来写测试内容，但无伤大雅，我们依然可以使用

```cpp
extern "C" {
#include "utils/myadd.h"
}
```

这样的形式包含我们要测试的库。测试用例编写如下：

```cpp
TEST_CASE("Sum of to integers", "[what]") {
    int a = 1, b = 2;
    REQUIRE(myadd(a, b) == 3);
}
```

TEST_CASE 宏的第一个参数是测试用例的名字（需要唯一），第二个参数 Tag 用方括号括起来，这个参数是可选的。在进行测试时，我们可以根据名字和 Tag 有选择的执行测试用例。

`REQUIRE()` 宏大致相当于 ASSERT，括号内的条件不为真时结果为 fail.

`INFO()` 用于测试时输出信息，如 `INFO("We are testing vdisk!");`

`CAPTURE()` 用于捕获并显示变量的信息，会在变量值前面显示变量名。如 `CAPTURE(a, b, c, a + b);`

执行测试得到如下所示的结果：

```bash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
testall is a Catch v2.13.0 host application.
Run with -? for options

-------------------------------------------------------------------------------
Wrong sum
-------------------------------------------------------------------------------
/home/peng/OS_Course_Design/unix-filesystem/tests/test_utils.cpp:13
...............................................................................

/home/peng/OS_Course_Design/unix-filesystem/tests/test_utils.cpp:15: FAILED:
  REQUIRE( mywrongadd(a, b) == 3 )
with expansion:
  4 == 3

===============================================================================
test cases: 2 | 1 passed | 1 failed
assertions: 2 | 1 passed | 1 failed

```

可以看出，结果显示了通过的用例数、未通过的用例数，以及实际的输出和期望的输出等信息。