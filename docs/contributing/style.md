# Coding Style



## General

### Line Length

Try to keep lines under 80 columns. If a line can't be broken across multiple
lines in a readable manner, you may go up to 100 characters.

### Indentation

Use 4-space tabs for indentation. Tabs should be converted to spaces.

If a function is being broken up across multiple lines at its parameters,
prefer indenting all non-first parameters to align with the first one. If an
if/else/for/while condition, class inheritance line, or constructor member
initializer list is broken across multiple lines, the broken off lines should
be by two tabs.

```cpp
auto long_function_name1(arg1, arg2, arg3,
                         arg4, arg5);

auto long_function_name2(really_long_parameter1,
                         really_long_parameter2,
                         really_long_parameter3);

if (cond1 && cond2 &&
        cond3) {
    ...
}
```

### Braces

Opening braces should always be on the same line as whatever came before it
(e.g., if/else/for/while, function name, class/struct name, etc.).

```cpp
auto foo() {
    if (condition) {
        ...
    }
    else {
        ...
    }
}

class Widget {
    private:
        bool m_foo;
    public:
        Widget() : m_foo{false} {}
}
```



## Naming Conventions

Generally, `snake_case` is used for variable, function, and file names,
`CAPS_SNAKE_CASE` for macros, and `CamelCase` is used for everything else.
There are some exceptions though. In particular, if a type can be described by
a single word, is publicly available to users, and is something that can be
reasonably considered as a primitive in the context of the module that it is
in, then I like to use all lowercase letters. For example, the KVM API wrapper
has classes `vmm::kvm::vm` and `vmm::kvm::vcpu`, both of which are absolutely
fundamental to how the API functions and therefore may be considered KVM
"primitives".

For classes, there are a few rules to follow as well. To start, all non-static
data members should be prefixed with `m_`. As for methods, all setter methods
should be prefixed with `set_`, while getter methods should use bare names.
The name of a setter/getter method should match (ignoring any prefixes) the
variable that they are setting/getting. Now, there are cases where a `get_`
prefix may be used for getter methods. Specifically, the prefix may be used
when a getter method's bare name conflicts with a keyword or well-known name
(e.g., `allocator`), if what the method does isn't obvious/accurate from the
bare name, or if the method returns values through out parameters. In addition,
if the word "get" does not accurately describe a getter method's functionality,
another prefix may be used (e.g., `read_`).

For function declarations, omit any redundant parameter names. That is, if what
a parameter is is clear from its type, then it does not need a name. Generally,
this leaves names for just bools, strings, and numerical parameters.



## Types

Variable types should be specified as `auto` wherever possible. When type
ambiguity is not permitted, explicitly construct the type on the right-hand
side to ensure that the variable's type is clear to readers. Similarly,
trailing return types should be used for functions.

Prefer enums over bools for function parameters. For setter methods, bools are
fine since the name of the method makes clear what the parameter is.



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
