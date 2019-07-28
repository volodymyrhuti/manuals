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
                                 Macro pitfalls
=========================================================================================================
Macro definitions do not have to have balanced parentheses. By writing an unbalanced open parenthesis in
a macro body, it is possible to create a macro call that begins inside the macro body but ends outside of
it. For example,

#define strange(file) fprintf (file, "%s %d",
…
strange(stderr) p, 35)
     → fprintf (stderr, "%s %d", p, 35)

The ability to piece together a macro call can be useful, but the use of unbalanced open parentheses in
a macro body is just confusing, and should be avoided. 
---------------------------------------------------------------------------------------------------------
Operator Precedence Problems
#define square(a) a * a
int z = square(x + y) * 2  <==> int z = x + y * x + y * 2  ==> z = x + (y * x) + y * 2
---------------------------------------------------------------------------------------------------------
swallowing the Semicolon
It is common to write some loop as macro, but macro looks like function therefore it is less ambiguos and
more readable to finish call to it with semicolon, like `find_smth_in_arr(arr,smth);`. Now it is question
how to properly write this macro definition. For instance
#define SKIP_SPACES(p, limit)  \
{ char *lim = (limit);         \
  while (p < lim) {            \
    if (*p++ != ' ') {         \
      p--; break; }}}
This looks correct and will works with semicollon and without, but what if we put it under if, like
if (true)
    SKIP_SPACES(p,limit);
else
    ...
Here, column will finish if statement and leave standalone if, which is an error. There are two solutions:
don`t use if without {} braces, which may go against code style, or maybe you don`t like how it looks;
you can rewrite macro using do{}while(0)
#define SKIP_SPACES(p, limit)     \
do { char *lim = (limit);         \
     while (p < lim) {            \
       if (*p++ != ' ') {         \
         p--; break; }}}          \
while (0)
---------------------------------------------------------------------------------------------------------
Function call side effects
Note, if you pass function call as one of the parameters to macro, it is not executed before replacing
therefore you will have a call for each symbol substitution, resulting in sideefects
#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
min (x, foo(x)) ==> ( x < foo(x) ? x : foo(x))
Workaround, provide additional variables that hold result of possible expressions, like
#define min(X, Y)                \
({ typeof (X) x_ = (X);          \
   typeof (Y) y_ = (Y);          \
   (x_ < y_) ? x_ : y_; })
Note, that operators like typeof are not always present
---------------------------------------------------------------------------------------------------------
Self referential macros
If macros refences itself, it is expanded only once, therefore macros like
#define foo (4 + foo) expands to (4 + foo)

If a macro x expands to use a macro y, and the expansion of y refers to the macro x, that is an indirect
self-reference of x. x is not expanded in this case either. Thus, if we have
#define x (4 + y)
#define y (2 * x)
then x and y expand as follows:
x    ==> (4 + y)
     ==> (4 + (2 * x))
y    ==> (2 * x)
     ==> (2 * (4 + y))
---------------------------------------------------------------------------------------------------------
Newlines in Arguments
Macross expansion results in one line, therfore if you split call into multiple, compiler/debugger may provide
missleading line information

macross_call( arg1,     <== compiler will point here
              arg2,
              error );  <== but error is here

https://gnu.huihoo.org/gcc/gcc-7.1.0/cpp/Macro-Pitfalls.html#Macro-Pitfalls
=========================================================================================================
                   Zero-length arrays in C | Flexible arrays
=========================================================================================================
Zero-length array is a way to have array members but didn`t want to have to decleare the size upfront.
Initially there was a trick with using a length 1 array and overalocating
struct name {
    int namelen;
    char namestr[1];
};
struct name *makename(char *newname)
{
    struct name *ret =
        malloc(sizeof(struct name)-1 + strlen(newname)+1);
                /* -1 for initial [1]; +1 for \0 */
    if(ret != NULL) {
        ret->namelen = strlen(newname);
        strcpy(ret->namestr, newname);
    }

    return ret;
}
Despite its popularity, the technique is also somewhat notorious: Dennis Ritchie has called it
``unwarranted chumminess with the C implementation,''. It is considered a hack because it either complicates
the call to malloc, or wastes space. Therefore, GCC offered a language extension with array of size 0.
Then in C99 it was made even simpler, since you could define them without size like
`char name[];` This lets you to issue only one malloc to allocate structure and offload, like:

struct str *var = malloc(sizeof(struct str) + extra_payload);

There are a few restrictions on flexible array members that ensure that code using them makes sense.
For example, there must be at least one other member, and the flexible array must occur last
Note, otherwise, zero length array is makes behaviour undefined, but if use it like `char name[];`
compiler will warn you about this.

One advantage is data locality, all members and array are saved continiously, ortherwise you would have
pointer for array, and data is allocated somewhere on heap, far from other fields. You could go around
by allocationg struct + payload in one malloc, like
struct name {
    int namelen;
    char *namep;
};
struct name *makename(char *newname)
{
    char *buf = malloc(sizeof(struct name) +
                strlen(newname) + 1);
    struct name *ret = (struct name *)buf;
    ret->namelen = strlen(newname);
    ret->namep = buf + sizeof(struct name);
    strcpy(ret->namep, newname);

    return ret;
}
However, piggybacking a second region onto a single malloc call like this is only portable if the second
region is to be treated as an array of char. For any larger type, alignment (see questions 2.12 and 16.7)
becomes significant and would have to be preserved.

https://stackoverflow.com/questions/14643406/whats-the-need-of-array-with-zero-elements
https://shivankaul.com/blog/c/languages/coding/bug/2017/07/03/zero-length-arrays.html
http://c-faq.com/struct/padding.html 2.12
http://c-faq.com/strangeprob/ptralign.html 16.7
http://c-faq.com/struct/structhack.html
=========================================================================================================
                              Pointer poisoning
=========================================================================================================
Pointer poisoning is a technic used to ensure that a poisoned pointer can`t be used without causing a
crash. You just assign invalid pointer to an easily identified address value out of user space like
0xdead000000000000. In case, you try to dereference it somewhere, you will have crash, but it will be
clear from address, which object is resposposible for it. You may was have multiple poisons, used with
different objects.

https://stackoverflow.com/questions/27801360/what-is-the-meaning-of-0xdead000000000000
https://www.openwall.com/lists/oss-security/2015/05/07/3
https://en.wikipedia.org/wiki/Hexspeak
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

