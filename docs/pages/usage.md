# Usage

chess-library is a single header library. Thus to use it in your project just copy the header file `chess.hpp` into your project  
and include in the necessary files.

The entire library is behind the `chess` namespace so for easy usage just say

## Example

```cpp
#include "chess.hpp"

using namespace chess;

```

That's it!

Also see [my examples.](/pages/example)

## Operator Overloads

::: danger
The usage of `enum classes` provides more type safety compared to plain enums because they prohibit implicit conversion to integers.

If you know what you are doing, you can make use of the `ADD_BASE_OPERATORS_FOR` and `ADD_INCR_OPERATORS_FOR` macros. Which will overload the enum class with the following operators.
:::

| ADD_BASE_OPERATORS_FOR | ADD_INCR_OPERATORS_FOR |
| ---------------------- | ---------------------- |
| +                      | ++ _prefix_            |
| -                      | -- _prefix_            |
| -                      | ++ _postfix_           |
| +=                     | -- _postfix_           |
| -=                     |                        |

### Usage

```cpp
ADD_INCR_OPERATORS_FOR(Piece)
```

::: tip
Only `Square` is currenlty implemented without an enum class. So you dont need to overload any operators for this.
:::
