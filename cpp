=========================================================================================================
Ways of passing value to functions
=========================================================================================================
1. By value
extern std::vector<int> test;        // Writable global variable
extern const std::vector<int> test2; // Read-only global variable
extern std::vector<int> somefunc();  // A function that returns a temp value

void testfunc(std:vector<int> val)
{
    test = val;         // Readign from val
    val = {1,2,3};      // Writing into val
}

void caller()
{
    testfunc(test);         // Pass a variable
    testfunc(somefunc())    // Pass a temp
    testfunc(test2)         // Pass a read-only variable
}

It is fine for primitives and simple structures ,however it is quite slowe in case
of something more complex, since value will be compied from caller to callee
---------------------------------------------------------------------------------------------------------
2. By reference
extern std::vector<int> test;        // Writable global variable
extern const std::vector<int> test2; // Read-only global variable
extern std::vector<int> somefunc();  // A function that returns a temp value

void testfunc(const std:vector<int>& val)
{
    test = val;         // Readign from val
    //val = {1,2,3};    // writing to const val is not permited
                        // can only access const methods of const object
}

void caller()
{
    testfunc(test);         // Pass a variable
    testfunc(somefunc())    // Pass a temp
    testfunc(test2)         // Pass a read-only variable
}

You can read but you cannot write, therefore no copying of data is needed.
This allows to pass in place allocated, temporary variables by caller.
---------------------------------------------------------------------------------------------------------
3. By L-value reference
extern std::vector<int> test;        // Writable global variable
extern const std::vector<int> test2; // Read-only global variable
extern std::vector<int> somefunc();  // A function that returns a temp value

void testfunc(std:vector<int>& val)
{
    test = val;
    val = {1,2,3};
}

void caller()
{
    testfunc(test);            // Pass a variable
    // testfunc(somefunc())    // Pass a temp, not permitted, there is no
                                  sense to pass temporary variable if you want
                                  to rewrite it
    // testfunc(test2)         // writing to const val is not permited
}

More effective then coppying but since callee is allowed to overwrite value, it
is not alloved to send const values, and ther is no sense to pass temporary values
which live only during a call
---------------------------------------------------------------------------------------------------------
4. By R-value reference
extern std::vector<int> test;        // Writable global variable
extern const std::vector<int> test2; // Read-only global variable
extern std::vector<int> somefunc();  // A function that returns a temp value

void testfunc(std:vector<int>&& val)
{
    test = std::move(val);         // Readign from val
    val = {1,2,3};      // writing to const val is not permited
                        // can only access const methods of const object
    val.clear();
}

void caller()
{
    testfunc(std::move(test)); // Pass a variable (transfer ownership of data)
                               // variable is now expacted to be cleared
    testfunc(somefunc())       // Pass a temp
    // testfunc(test2)         // passing a read only not possible
}

It is not possible to use named arguments without cast with std::move.
Rvalue is used when you don`t need value no more and want to transfer responsibility.
Best practice, clear object before returning unless forwarding responsibility.


=========================================================================================================
                    Attributes
=========================================================================================================
Attributes allow you mark functions, vaiables, and other entities with compiler-specific
or standart properties.  C++17 adds three attributes for programmers to better express
their intent to the compiler and readers of the code: maybe_unused, fallthrough, nodiscard.
Before C++17 attributes are noreturn, deprecated (entity is going to be removed in a later
version, usage results in a compiler warning) and carries_dependency(used for optimizing
atomic operations). 
[[maybe_unused]] var_type var_name => now it is a standart way to avoid compiler warnings,
maybe usefull when variable usage is closed under defines and is used in release/debug version
[[falthrough]] => indicates a fallthrough in a switch statement is intentional, otherwise
compiler may warn abouy missed break/return.
[[nodiscard]] => function declared with it indicate that return value shouldn`t be ignored
by the caller. Usefull for ensuring caller check a return value,or that some scope guard object
has a resonable liftime.
[[nodiscard]] struct lock_guard;
lock_guard lock(mutex& m);

{
    lock(mutex);    // Warning, ignored returned value
}

https://blog.tartanllama.xyz/c++17-attributes/
=========================================================================================================
Array without explicit size defined
The number of elemnts of an array must be detemined either at compile time or since
C(( can be evaluated at runtime at the point of creation. You can define an aeeay
without explicit suze for the leftmost dimension if you provice an initialized.
The compiler will infer size from intializer
int m[][2] = {{ 1, 2 }, { 3, 4 }};  // equivalent to int m[2][2] = {{ 1, 2 }, { 3, 4 }};
int m[][] = {{ 1, 2 }, { 3, 4 }};  // not valid


=========================================================================================================
                                Output buffering
=========================================================================================================
Flushing for stdout is determined by its buffering behaviour. The buffering can be set to three modes:
_IOFBF full buffering: waits until fflush() if possible,
_IOLBF line buffering: newline triggers automatic flush,
_IONBF direct write always used.
Support for these characteristics is implementation-deﬁned, and may be affected via the setbuf() and setvbuf() functions.

https://stackoverflow.com/questions/13932932/why-does-stdout-need-explicit-flushing-when-redirected-to-file/13933741#13933741
=========================================================================================================
                                  Rule of 3/5
=========================================================================================================
If a class defines one (or more) of destructor / copy constuctor / copy assignment operator, it should
probably explicitly define all three

=========================================================================================================
                                    Size of
=========================================================================================================
Sizeof is pure compile time in C++ and C prior to C99 based on compile time information. Starting with C99
there are variable length array size of which is handled at runtime. Any other operands/types (not allocated
dynamically) are computed at compile time. When using C99+, sizeof doesn`t return at compile time therfore
it`s value cannot be used in scope where such value is requied - for example initializing static variables,
unless a compiler specific extension allows it.

=========================================================================================================
                                    Logging
=========================================================================================================
Usually there are next log levels =>
1.Trace
2.Debug
3.Info
4.Warning => nothing is broken but something may go wrong in the future
5.Error
6.Critical 
=========================================================================================================
                                     Mixins
=========================================================================================================
Mixin is a class that contains methods for use by other classes without having to be the parent class of
those otherclasses. How those other classes gain access to the mixin`s methods depends on the language.
Ther are sometimes described as being included rather than inherited.

Advantages
It provides a mechanism for multiple inheritance by allowing multiple classes to use the common functionality,
but without the complex semantics of multiple inheritance.
Code reusability: Mixins are useful when a programmer wants to share functionality between different
classes. Instead of repeating the same code over and over again, the common functionality can simply be
grouped into a mixin and then included into each class that requires it.
Mixins allow inheritance and use of only the desired features from the parent class, not necessarily all
of the features from the parent class.

=========================================================================================================
                              Contract programming
=========================================================================================================
https://dlang.org/spec/contracts.html
=========================================================================================================
To study
https://en.m.wikibooks.org/wiki/More_C%2B%2B_Idioms
=========================================================================================================
Polymorphism => object oriented strategy used when designing object models, to help
simplify the code. At it`s core polymorphism is the ability to define simillar yet
different objects and to then treat them as if they are the same. For instance =>
- string ("1") + 1          => overlaoding of + operator
- int foo(int)              => function overloading
  double foo(dobule)

Polymorphism is the ability to associate different specific behaviours with a single
generic notation 

Templates can be decent compile time overhead instead of runtime polymorhism.


Class deconstruction
It is safe to delete NULL

