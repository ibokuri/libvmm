# Libvmm Coding Style Guide

<!--For low-level styling (spaces, parentheses, brace placement, etc), all code should follow the format specified in `.clang-format` in the project root.-->

<!--**Important: Make sure you use `clang-format` version 10 or later!**-->

This document describes the coding style used for C++ code in the Libvmm project. All new code should conform to this style.

<!--We'll definitely be tweaking and amending this over time, so let's consider it a living document. :)-->

[](#general)
## General

[](#general-line-length)
### Line Length

Libvmm uses an 80-character line limit. The only exceptions are function names
with either one or no parameters as they can't be broken across lines in a
readable fashion.

[](#names)
## Names

[](#names-basic)
### Basic

Use `CamelCase` for classes, structs, enumeration members, and template
parameters. Use `snake_case` for variable and function names.

[](#names-data-members)
### Data Members

Data members in C++ classes should be prefixed by `m_`.

[](#names-setter-getter)
### Setters/Getters

Setter and getter names should match the variables being set/gotten.

Setters should be prefixed with `set\_`. Getters should only be preceded with `get_` if they return values through out parameters.

<!--[](#names-if-exists) When there are two getters for a variable, and one of them automatically makes sure the requested object is instantiated, prefix that getter function which with `ensure_`. As it ensures that an object is created, it should consequently also return a reference, not a pointer.-->

[](#names-variable-name-in-function-decl)
### Parameter Names
Omit redundant variable names from function declarations. Usually, there should
be a parameter name for bools, strings, and numerical types.

<!--[](#names-enum-to-bool) Prefer enums to bools on function parameters if callers are likely to be passing constants, since named constants are easier to read at the call site. An exception to this rule is a setter function, where the name of the function already makes clear what the boolean is.-->

<!--[](#names-const-to-define) Prefer `const` to `#define`. Prefer inline functions to macros.-->

<!--[](#names-define-constants) `#defined` constants should use all uppercase names with words separated by underscores.-->

<!--[](#header-guards) Use `#pragma once` instead of `#define` and `#ifdef` for header guards.-->

[](#classes)
## Classes

[](#classes-braces)
### Braces

If a constructor's body is empty, body braces should appear on the same line as
the function name or member initializer list. If the body is not empty, the
braces should appear on a line after the function name or member initializer
list.

#### Right:

```cpp
class Foo {
    int m_foo = 0;
    bool m_bar = false;

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
### Member Initialization

Where possible, initialize class members at member definition. Otherwise,
initialize members with initializer lists. Each member (or superclass) in an
initializer list should be indented on a separate line, with commas at the end
of each non-last line.

#### Right:

```cpp
class Foo {
    int m_foo = 0;
    bool m_bar = false;

    MyClass(int foo)
        : BaseClass(),
          m_foo(foo) {}

    MyClass() : BaseClass() {}
};
```

[](#pointers)
## Pointers and References

[](#pointers-name)
### Naming

Pointer and reference types should be written with no space between the
variable name and the `*` or `&`.

[](#pointers-out-argument)
### Out Arguments

An out argument of a function should be passed by reference except rare cases where it is optional in which case it should be passed by pointer.

[](#using)
## `using` Statements

[](#using-imports)
### Imports

Do not use `using` statements to import names. Directly qualify the names at the point they're used instead.

###### Right:

```cpp
std::swap(a, b);
```

###### Wrong:

```cpp
using std::swap;
swap(a, b);

// OR

using namespace std;
swap(a, b);
```

[](#types)
## Types

[](#types-unsigned)
### Unsigned

Omit "int" when using "unsigned" modifier. Do not use "signed" modifier. Use
"int" by itself instead.

[](#classes)
## Classes

[](#classes-explicit)
### `explicit` Keyword

Mark constructors with single parameters as `explicit` unless implicit
conversion is desired and the type conversion is intuitive and fast.

[](#comments)
## Comments

Comments should be written as proper sentences. One exception is end-of-line
comments like this: `if (x == y) // false for NaN`.

[](#virtual)
## Virtual Methods

[](#virtual-override)
### Overriding

The declaration of a virtual method inside a class must be declared with the
`virtual` keyword. All subclasses of that class must either specify the
`override` keyword when overriding the virtual method or the `final` keyword
when overriding the virtual method and requiring that no further subclasses can
override it.
