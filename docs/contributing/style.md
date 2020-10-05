# Libvmm Coding Style Guide

<!--For low-level styling (spaces, parentheses, brace placement, etc), all code should follow the format specified in `.clang-format` in the project root.-->

<!--**Important: Make sure you use `clang-format` version 10 or later!**-->

This document describes the coding style used for C++ code in the Libvmm project. All new code should conform to this style.

<!--We'll definitely be tweaking and amending this over time, so let's consider it a living document. :)-->


### Names

[](#names-basic) Use CamelCase for classes, structs, enumeration members, and template parameters. Use snake\_case for variable and function names.

[](#names-data-members) Data members in C++ classes should be prefixed by "m\_".

[](#names-setter-getter) Setter and getter function names should match the variables being set/gotten. Setters should be prefixed with "set\_".

[](#names-out-parameter) Precede getters value return values through out parameters with "get\_".

<!--[](#names-if-exists) When there are two getters for a variable, and one of them automatically makes sure the requested object is instantiated, prefix that getter function which with `ensure_`. As it ensures that an object is created, it should consequently also return a reference, not a pointer.-->

[](#names-variable-name-in-function-decl) Omit redundant variable names from function declarations. A good rule of thumb is if the parameter type contains the parameter name, then a parameter name isn't needed. Usually, there should be a parameter name for bools, strings, and numerical types.

[](#names-enum-to-bool) Prefer enums to bools on function parameters if callers are likely to be passing constants, since named constants are easier to read at the call site. An exception to this rule is a setter function, where the name of the function already makes clear what the boolean is.

<!--###### Right:-->

<!--```cpp-->
<!--do_something(something, AllowFooBar::Yes);-->
<!--paint_text_with_shadows(context, ..., text_stroke_width > 0, is_horizontal());-->
<!--set_resizable(false);-->
<!--```-->

[](#names-const-to-define) Prefer `const` to `#define`. Prefer inline functions to macros.

[](#names-define-constants) `#defined` constants should use all uppercase names with words separated by underscores.

[](#header-guards) Use `#pragma once` instead of `#define` and `#ifdef` for header guards.

### Classes

[](#classes-braces) If a constructor's body is empty, body braces should appear on the same line as the function name or member initializer list. If the body is not empty, the braces should appear on a line after the function name or member initializer list.

###### Right:

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

[](#classes-member-init) Where possible, prefer initializing class members at member definition. Otherwise, initialize members with member initializer lists. Each member (or superclass) in an initializer list should be indented on a separate line, with commas at the end of each non-last line.

###### Right:

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

### Pointers and References

[](#pointers-name) Pointer and reference types should be written with no space between the variable name and the `*` or `&`.

[](#pointers-out-argument) An out argument of a function should be passed by reference except rare cases where it is optional in which case it should be passed by pointer.

<!--###### Right:-->

<!--```cpp-->
<!--void MyClass::get_some_value(OutArgumentType& out_argument) const-->
<!--{-->
    <!--out_argument = m_value;-->
<!--}-->

<!--void MyClass::do_something(OutArgumentType* out_argument) const-->
<!--{-->
    <!--do_the_thing();-->
    <!--if (out_argument)-->
        <!--*out_argument = m_value;-->
<!--}-->
<!--```-->

<!--###### Wrong:-->

<!--```cpp-->
<!--void MyClass::get_some_value(OutArgumentType* outArgument) const-->
<!--{-->
    <!--*out_argument = m_value;-->
<!--}-->
<!--```-->

### `using` Statements

[](#using-imports) Do not use `using` statements to import names. Directly qualify the names at the point they're used instead.

###### Right:

```cpp
std::swap(a, b);
```

###### Wrong:

```cpp
using std::swap;
swap(a, b);
```

###### Wrong:

```cpp
using namespace std;
swap(a, b);
```

### Types

[](#types-unsigned) Omit "int" when using "unsigned" modifier. Do not use "signed" modifier. Use "int" by itself instead.

### Classes

[](#classes-explicit) Mark constructors with single parameters as `explicit` unless implicit conversion is desired and the type conversion is intuitive and fast.

### Comments

[](#comments-sentences) Comments should be written as proper sentences. One exception is end-of-line comments like this: `if (x == y) // false for NaN`.

### Overriding Virtual Methods

[](#override-methods) The declaration of a virtual method inside a class must be declared with the `virtual` keyword. All subclasses of that class must either specify the `override` keyword when overriding the virtual method or the `final` keyword when overriding the virtual method and requiring that no further subclasses can override it.
