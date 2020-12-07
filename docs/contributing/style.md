# Coding Style

## General

### Line Length

Try to keep lines under 80 characters. If a line surpasses that limit, but
can't be broken across multiple lines in a readable manner, you may go up to
100 characters.

### Tabs

Use 4-space tabs for indentation. Tabs should be converted into spaces.

### Braces

For functions and classes, opening braces go on a separate line unless the
function or class is empty.

```cpp
void foo()
{
    if (condition) {
        ...
    }
    else {
        ...
    }
}

class Widget
{
    private:
        bool m_foo;
    public:
        Widget() : m_foo{false} {}
}
```

### Multi-Liners

#### Functions

For multi-line function names, break along the function's parameters and align
noninitial lines with the first parameter.

```cpp
void long_function_name1(arg1, arg2, arg3,
                         arg4, arg5);

void long_function_name2(really_long_parameter1,
                         really_long_parameter2,
                         really_long_parameter3);
```

#### Classes

For long derived class names, break at the base-clause (`: <base>`).

```cpp
class LongWidgetName
    : BaseWidget;
```

For long control statements (`if`, `else`, `for`, `while`), break wherever
appropriate, but indent noninitial lines by 2 tabs.

```cpp
if (cond1 && cond2 &&
        cond3) {
    ...
}

while (really_long_parameter == 0 ||
        really_long_parameter == 1) {
    ...
}
```

For everything else, opening braces do not go on a separate line.

## Naming Conventions

### `snake_case` vs. `CamelCase`

Generally, `snake_case` is used for variable, function, and file names,
`CAPS_SNAKE_CASE` for macros, and `CamelCase` is used for everything else.

### Classes

#### Data Members

All non-static data members should be prefixed with `m_`.

#### Methods

All setter methods should be prefixed with `set_`. And generally, getter
methods should use bare names. In either case, the name of the method should,
ignoring any prefixes, match the data member that they are setting/getting.

In cases where a getter method using a bare name conflicts with a keyword or
well-known name, returns values via out parameters, or is not accurately
described by the word "get," another prefix may be used (e.g., `read_`).

#### Functions

Omit redundant parameter names in function declarations.

```cpp
void foo(Widget, MsrIndexList&, std::vector<int>& bitmap);
```

## Types

### `auto`

Wherever possible, prefer specifying types as `auto`. When type ambiguity is
not permitted/desired, explicitly specify the type on the right-hand side to ensure
that the variable's type is clear to readers.

Similarly, trailing return types should be used for functions.

```cpp
auto foo() noexcept -> void;

auto a = 0;
auto b = int{};
auto c = std::vector<int>{};
```

### Enums

Prefer enum classes over bools for function parameters. However, for setter
methods, bools are fine since the name of the method makes clear what the
parameter is.

## Classes

### Member Initialization

Where possible, initialize class members at member definition. Otherwise,
initialize members via initializer lists.

```cpp
class Foo {
    private:
        int m_foo = 0;
        bool m_bar = false;
    public:
        MyClass(int foo)
            : BaseClass(),
              m_foo{foo}, m_bar{true} {}

        MyClass() : BaseClass() {}
};
```

### `explicit`

Mark constructors with single parameters as `explicit` unless implicit
conversion is desired and the type conversion is intuitive and fast.

## Pointers and References

### Naming

Pointer and reference types should be written with no space between the
variable name and the `*` or `&`.

### Out Parameters

An out parameter should be passed by reference except rare cases where it is
optional, in which case it should be passed by pointer.

## Virtual Methods

### Overriding

The declaration of a virtual method inside a class must be declared with the
`virtual` keyword. All subclasses of that class must either specify the
`override` keyword when overriding the virtual method or the `final` keyword
when overriding the virtual method and requiring that no further subclasses can
override it.
