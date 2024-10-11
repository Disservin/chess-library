# Getting Started

chess-library is a single header library.

Thus to use it in your project just copy the header file `chess.hpp` into your project  
and include in the necessary files.

The entire library is behind the `chess` namespace so for easy usage just say

## Example

```cpp
#include "chess.hpp"

using namespace chess;

```

That's it!

Also see [my examples.](/pages/examples)

::: tip
The library internally uses `assert` and may print debug information to `std::cerr`.
To disable this define `NDEBUG` when compiling.
:::
