# Coding Style



## General

### Limit lines to 80 characters

### Use trailing return types for functions

### Use `auto` for variable types

Specify variable types as `auto` wherever possible. When type ambiguity is not
permitted, explicitly construct the type on the right-hand side to make the
variable's type clear.



## Naming Convention

### Use `snake_case` for variable and function names, `CamelCase` for everything else

Note that there are exceptions, like the ioctl classes in the KVM API wrapper.
Generally if a type is something I consider to be a primitive in the context
of the module that it is in, then I like to use `snake_case`, though I'll limit
myself to a single word in such cases.

### Prefix Class Members with `m_`

### Prefix setter methods with `set_`. Use bare names for getter methods

The name of setter/getter methods should match (ignoring any prefixes) the
variable that they are setting/getting.

There are cases where a `get_` prefix may be used for getter methods.
Specifically, the prefix may be used when a getter method's bare name
conflicts with a keyword/well-known name (e.g., `allocator`), if what the
method does isn't obvious/accurate from the bare name, or if the method
returns values through out parameters. If `get` does not accurately describe a
getter method's functionality, another prefix may be used (e.g., `read_`).

### Omit redundant variable names from function declarations

Generally, this means only bools, strings, and numerical parameters should
have names in function declarations.

### Prefer enums over bools for function parameters

For setter methods, a `bool` parameter is fine since the name of the method
makes clear what the parameter is.

### Use `#pragma once` instead of include guards for header files



## Classes

### A class constructor's braces depends on the body's contents

If a constructor's body is empty, the opening brace should appear on the same
line as the function name or member initializer list. If the body is not
empty, the opening brace should appear on its own line.

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

### Member Initialization

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



## Pointers and References

### Naming

Pointer and reference types should be written with no space between the
variable name and the `*` or `&`.

### Out Arguments

An out argument of a function should be passed by reference except rare cases
where it is optional in which case it should be passed by pointer.



## Classes

### `explicit` Keyword

Mark constructors with single parameters as `explicit` unless implicit
conversion is desired and the type conversion is intuitive and fast.



## Virtual Methods

### Overriding

The declaration of a virtual method inside a class must be declared with the
`virtual` keyword. All subclasses of that class must either specify the
`override` keyword when overriding the virtual method or the `final` keyword
when overriding the virtual method and requiring that no further subclasses can
override it.
