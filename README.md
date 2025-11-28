## Roadmap:
1. Create main.cc (Nov 19)
2. Write all interface files (Nov 20)
3. Write the Makefile (Nov 20)
4. Work individually
    - Work on Level, LevelFactory (Ron) (Nov 20)
    - Work on Block impl file (Daniel + Ron) (Nov 20-21)
    - Work on TUI Renderer (Chenyu) (Nov 20)
    - Work on InputHandler (Chenyu) (Nov 20-21)
    - Whoever finishes first does the GUI (Nov 21-22)
5. Work on Player impl files together (Nov 23)
6. Work on Board impl files together (Nov 23-24)
7. Work on Game impl file together (Nov 24)
8. Write report (Nov 25-28)

## NOTES:
- For block coordinates, set (r, c) (0, 0) as the 15th row first column, number increases downward and to the right
- For GUI renderers impl, everytime you `XFlush()`, I recommend `usleep()` for like 1ms (`usleep(1000)`)
---

## Style Guide:
1. Indent width 4
2. Max line length 100
3. No namespace, just `std::`
4. Class name: `UpperCaseCamel`
5. Constant: `UPPER_SNAKE_CASE`
6. Other: `lowerCaseCamel`
7. At declaration, if function has side effect or requirement, please comment so.
8. In implementation, comment if the code doing something not obvious

---

## using `make`
NGL, you can do `make help` and probably get all you need. But here is how you use it

Available targets:
  make        - Build main program (biq)
  make test   - Build and run tests
  make clean  - Remove all build artifacts
  make help   - Show this message

To add a new module:
  0. If needed, add new standard library module (e.g. iostream) <- search POINT0
  1. Add interface file to MODULE_INTERFACES (e.g., list.cc) <- search POINT1
  2. Add implementation file to MODULE_IMPLS (e.g., list-impl.cc) <- search POINT1
  3. Add dependency rule: `$(OBJ_DIR)/list-impl.o: list-impl.cc $(OBJ_DIR)/list.o` <- search POINT2

To add tests: create `tests/test_<module>.cc` files
They will be automatically discovered and compiled

**Don't mess with test_runner or test_main. I'll kill ya.**

### Writing tests
Up to you, you can use my framework if you want. Here is how to use it

1. Add `test/test_<module>.cc` file
2. `#include "test_runner.h"` and `import` all what you need
3. For each test case, just do this
```c++
TESTCASE(TestCaseName) {
    // ... test your thing
    bool ronIsCool = true;
    REQUIRE(ronIsCool == true);
}
```
4. Now you can do `make test`. That's it

