# Libvmm Style Guide

<!--For low-level styling (spaces, parentheses, brace placement, etc), all code should follow the format specified in `.clang-format` in the project root.-->

<!--**Important: Make sure you use `clang-format` version 10 or later!**-->

This document describes the coding style used for C++ code in the Libvmm project.

[](#general)
## 🎟 General

[](#general-line-length)
### 💥 Limit lines to 80 characters

##### Exceptions

* Function names with one or zero parameters may exceed this limit.

[](#general-return-type)
### 💥 Use trailing return types for functions

```cpp
auto a() -> int;
auto b() -> void;
```

[](#general-variable-types)
### 💥 Use `auto` for variable types

Specify variable types as `auto` wherever possible. When type ambiguity is not
permitted, explicitly construct the type on the right-hand side to make the
variable's type clear.

```cpp
auto a = false;
auto b = uint64_t{1};
auto c = Widget{};

class Widget {
    private:
        auto m_foo = false;
        char m_bar;  // OK: we can't use auto since we don't initialize m_foo here
}
```

[](#names)
## 🎟 Names

[](#names-basic)
### 💥 Use `snake_case` for variable and function names, `CamelCase` for everything else

```cpp
// snake_case
auto my_var = 0;
auto my_func() -> void;

// CamelCase
class MyWidget;
struct MyGadget;
enum class MyEnum { MyMember };
template<typename T, auto MyNonType> class Widget;
```

##### Exceptions

* TODO: The KVM ioctl classes are an exception.

[](#names-data-members)
### 💥 Prefix Class Members with `m_`

```cpp
class Widget {
    int *m_foo;
    char m_bar;
}
```

[](#names-setter-getter)
### 💥 Prefix setter methods with `set_`. Use bare names for getter methods

```cpp
class Register { ... }

class Widget {
    private:
        auto m_value = int{0};
        auto m_status = false;
    public:
        auto status() -> bool;

        auto get_value(int& value) -> void;
        auto set_value(int value) -> void;

        auto read_register(Register&) -> void;
}
```

##### Exceptions

* If a getter method returns values through out parameters, then it should be
  prefixed with `get_`.

* If a getter method's function is ambiguous even with the `get_` prefix,
  another prefix may be used such as `read_`.

<!--[](#names-if-exists) When there are two getters for a variable, and one of
them automatically makes sure the requested object is instantiated, prefix that
getter function which with `ensure_`. As it ensures that an object is created,
it should consequently also return a reference, not a pointer.-->

[](#names-variable-name-in-function-decl)
### 💥 Omit redundant variable names from function declarations

This rule usually means a name is only given to bool, string, and numerical
parameters in function declarations.

```cpp
auto a(int flag, bool status) -> void;
auto b(Context*) -> void;
```

[](#names-enum-to-bool)
### 💥 Prefer enums over bools for function parameters

```cpp
enum class Status {
    Off,
    On
};

auto func(Status) -> void;
auto set_widget(bool) -> void;
```

##### Exception

* For setter functions, a bool parameter is fine since the name of the function
  makes clear what the parameter is.

<!--[](#names-const-to-define) Prefer `const` to `#define`. Prefer inline-->
<!--functions to macros.-->

<!--[](#names-define-constants) `#defined` constants should use all uppercase
names with words separated by underscores.-->

[](#header-guards)
### 💥 Use `#pragma once` instead of include guards for header files


[](#classes)
## 🎟 Classes

[](#classes-braces)
### 💥 A class constructor's braces depends on the body's contents

If a constructor's body is empty, braces should appear on the same line as the
function name or member initializer list. If the body is not empty, braces
should appear on their own line.

```cpp
class Foo {
    private:
        int m_foo = 0;
        bool m_bar = false;
    public:
        MyClass() {}

        MyClass(int foo)
            : m_foo(foo) {}

        MyClass(bool bar)
            : m_bar(bar)
        {
            std::cout << "I'm on a new line!" << std::endl;
        }
};
```

[](#classes-member-init)
### 💥 Member Initialization

Where possible, initialize class members at member definition. Otherwise,
initialize members with initializer lists. Each member (or base class) in an
initializer list should be indented on a separate line, with commas at the end
of each non-last line.

```cpp
class Foo {
    private:
        int m_foo = 0;
        bool m_bar = false;
    public:
        MyClass(int foo)
            : BaseClass(),
              m_foo{foo} {}

        MyClass() : BaseClass() {}
};
```

[](#pointers)
## 🎟 Pointers and References

[](#pointers-name)
### 💥 Naming

Pointer and reference types should be written with no space between the
variable name and the `*` or `&`.

[](#pointers-out-argument)
### 💥 Out Arguments

An out argument of a function should be passed by reference except rare cases
where it is optional in which case it should be passed by pointer.

[](#using)
## 🎟 `using` Statements

[](#using-imports)
### 💥 Imports

Do not use `using` statements to import names. Directly qualify the names at the point they're used instead.

```cpp
std::swap(a, b);
```

[](#types)
## 🎟 Types

[](#types-unsigned)
### 💥 Unsigned

Omit `int` when using `unsigned` modifier. Do not use `signed` modifier. Use
`int` by itself instead.

[](#classes)
## 🎟 Classes

[](#classes-explicit)
### 💥 `explicit` Keyword

Mark constructors with single parameters as `explicit` unless implicit
conversion is desired and the type conversion is intuitive and fast.

[](#comments)
## 🎟 Comments

Comments should be written as proper sentences. One exception is end-of-line
comments like this: `if (x == y) // false for NaN`.

[](#virtual)
## 🎟 Virtual Methods

[](#virtual-override)
### 💥 Overriding

The declaration of a virtual method inside a class must be declared with the
`virtual` keyword. All subclasses of that class must either specify the
`override` keyword when overriding the virtual method or the `final` keyword
when overriding the virtual method and requiring that no further subclasses can
override it.
