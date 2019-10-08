=========================================================================================================
                                Pointers
=========================================================================================================
---------------------------------------------------------------------------------------------------------
                                NULL and nullptr
---------------------------------------------------------------------------------------------------------
Pointer may intialized and point to some valid address but there are cases we don`t want to point somewhere
or have any object at current moment. To define such pointer, we initiliza it with NULL in C and nullptr
in C++. NULL has platform dependent definition but usually it is macro for integer 0. Usage of NULL may
make it not clear if it is value or pointer, therefore breaking some code, for instance function oveloading
void foo(int *ptr);
void foo(int value);
int *x = NULL;
foo(x);     // calls foo(int*)
foo(0);     // calls foo(int)
foo(NULL);  // likely to call foo(int)
C++11 introduced nullptr and should be used instead of NULL. This value is at the same type keyword and
r-value, therefore you can assign it to pointer. It is always implicilty converted to pointer.
There is, aswell, type definining nullpts, std::nullptr_t, it can be used to explicitly define that
argument should be nullptr (any usage?)
---------------------------------------------------------------------------------------------------------
                               Function pointers
---------------------------------------------------------------------------------------------------------
Function pointer is flexible mechanism allowing you to define pointer for function prototype and assign
any valid function to it. The syntax is following
<return_type> (*pointer_name)(argument_list);
int foo(){}
int (*fncPtr)();
int (*fncPtr)() = foo;
    fncPtr = foo;
    fncPtr();
    (*fncPtr)();
There is no need to take address of function, it is implicitly done by compiler.
Common use case is algorithm taking function as predicat. Default function as argument is valid.
void selectionSort(int *array, int size, bool (*comparisonFcn)(int, int))
void selectionSort(int *array, int size, bool (*comparisonFcn)(int, int) = ascending);
bool ascending(int x, int y)
bool descending(int x, int y)
selectionSort(array, sizeof(ascending), ascending);
selectionSort(array, sizeof(ascending));

Syntax is quite ugly, therefore you simplify it with typedef.
bool validate(int x, int y, bool (*fcnPtr)(int, int)); // ugly
using validateFcn = bool(*)(int, int);                 // type alias
bool validate(int x, int y, validateFcn pfcn)          // clean

You can use auto type to simplify pointer declaration.
auto pFoo = foo;
---------------------------------------------------------------------------------------------------------
                                 std::function
---------------------------------------------------------------------------------------------------------
C++11 introduced alternative method of defining and storing callable objects called std::function.
Although this reads a little more verbosely, it’s also more explicit, as it makes it clear what the return
type and parameters expected are (whereas the typedef method obscures them).

#include <functional>
bool validate(int x, int y, std::function<bool(int, int)> fcn);
std::function<int()> fcnPtr = foo;
fcnPtr = goo;
std::cout << fcnPtr();

The profit of using it, you can assign any callable, like function pointer or functor, to this.
struct FooFunctor {
    void operator()(int i) {
        std::cout << i;
    }
};
FooFunctor func;
std::function<void (int)> f(func);

Another reason to use `function` is capturing lambda that has capture, since it can`t be decayed to
function pointer. The problem is the state contained by lambda, passing it as pointer makes us loose
state, therefore, it is applicable for lambdas that has no capture/state.
void do_something_function(std::function<void()> callback) {
  callback();
}
void do_something_pointer(void (*callback)()) {
  callback();
}
auto callback=[&](){ ... };
do_something_pointer(callback);         // compilation error
do_something_function(callback);        // Ok, capture info is saved


http://bannalia.blogspot.com/2016/07/passing-capturing-c-lambda-functions-as.html
https://stackoverflow.com/questions/25848690/should-i-use-stdfunction-or-a-function-pointer-in-c/25848890
https://stackoverflow.com/questions/9054774/difference-between-stdfunction-and-a-standard-function-pointer
=========================================================================================================
                            Poointer vs Reference
=========================================================================================================
A pointer is a variable that holds memory address of another variable. A pointer needs to be dereferenced
with * operator to access the memory location it points to.
A reference variable is an alias, that is, another name for an already existing variable. A reference,
like a pointer, is also implemented by storing the address of an object.  A reference can be thought of
as a constant pointer  with automatic indirection, i.e the compiler will apply the * operator for you.

1. Initialization. You can change pointer whenever you want, unless it is constant. Reference must be
   initialized when delared, unless it class member, then it is initialized through member list.
2. Reasignment. As previously mentioned, you can change pointer but not reference
3. Memory Address. Both of them allocate some space on stack. Pointer contains address of pointee and
   if you take address of reference, it will return address of read variable. To get real address of
   reference use `&(&ref)`
4. NULL. You can have pointer pointing to nothing.
5. Inderection. Pointer may point to another pointer.
6. Arithmetic operations. You get/change address of pointer by performing addition or substraction.

Use reference:
1. In function parametes and return types if you want to avoid coppying or pointer decay matters.
_. Let`s you avoid object slicing

Use pointers:
1. Use when you need flexibilty of having NULL
2. To implement data structures like linked-lists

Use references when you can, and pointers when you have to. References are usually preferred over pointers
whenever you don’t need “reseating”. This usually means that references are most useful in a class’s public
interface. References typically appear on the skin of an object, and pointers on the inside.

Another example is to use pointers to raw memory for specific memory manipulations. That should be hidden
and localized in very narrow parts of the code, to help limit the dangerous parts of the whole code base.

---------------------------------------------------------------------------------------------------------
                              Reference to pointer
---------------------------------------------------------------------------------------------------------
If we can define references to simple variables, it would make sense to have references to pointers as
well. This can be done, one usecase is to avoid ** syntax when you want to change pointer

void foo(int*& ptr) {
    ptr = new int;
}

https://www.geeksforgeeks.org/passing-reference-to-a-pointer-in-c/
---------------------------------------------------------------------------------------------------------
                                     Notes
---------------------------------------------------------------------------------------------------------
Note, that internally it is most likely implemented the same way as pointer, although you shouldn`t rely
on this. You can find this by disassembling code using references and pointers and comparing executed
instructions. The profit of using references is stronger guarantees (compiler can assume that it is never
null) than for pointers, therefore it can be optimized more aggressively.

Reference must be assigned a valid value, therefore following code is illegal even if it compiles
T* p = nullptr;
T& r = *p;  // illegal

References were introduced since they make it easier to write safer, cleaner code.

https://stackoverflow.com/questions/7058339/when-to-use-references-vs-pointers
https://stackoverflow.com/questions/31262942/c-references-are-they-just-syntactic-sugar
https://stackoverflow.com/questions/3954764/how-is-reference-implemented-internally
=========================================================================================================
                               Pointers pitfalls
=========================================================================================================
Additive operators. If you try to substract pointers pointing two the same consecutive memory e.g. array,
result will be not difference in addresses but in indexes
int arr[10];
int *s = arr;
int *e = &arr[9];
int n = e - s; # result is 9

Pointer decay. There is an issue when passing array to function. Since we pass pointer by value, it is
just a copy of pointer to the first element, there is no information describing if it is one object or
the first of an array as well as dimension information. Therefore sizeof will provide wrong result.
To prevent this:
1. Pass array size as parameter, so there is no need to calculate it
2. Pass array into function as reference. This prevents conversion of array to pointer hence no decay.

void by_value(const T* array)           // const T array[] means the same, sizeof gives wrong result
void by_pointer(const T (*array)[U])    // decays
void by_reference(const T (&array)[U])  // OK

You may find people defining size of array in function declaration. This does nothing, pointer will still
decay. This can be used for documnting purpose, e.g. it is expected that array of such size will be
passed to function.

Note, that there are execeptions for decaying:
1. When it's the argument of the & (address-of) operator.
2. When it's the argument of the sizeof operator.
3. When it's a string literal of type char [N + 1] or a wide string literal of type wchar_t [N + 1]
   (N is the length of the string) which is used to initialize an array, as in char str[] = "foo";
   or wchar_t wstr[] = L"foo";.
And some additional for C11 and C++

https://stackoverflow.com/questions/17752978/exceptions-to-array-decaying-into-a-pointer
https://stackoverflow.com/questions/39984816/when-subtracting-two-pointers-in-c
https://stackoverflow.com/questions/5187224/why-should-i-declare-a-c-array-parameters-size-in-a-function-header
https://stackoverflow.com/questions/1461432/what-is-array-decaying
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
                                 Smart Pointers
=========================================================================================================
Problems of raw pointers:
1. Its declaration doesn’t indicate whether it points to a single object or to an array.
2. Its declaration reveals nothing about whether you should destroy what it points to when you’re done 
   using it, i.e., if the pointer owns the thing it points to.
3. If you determine that you should destroy what the pointer points to, there’s no way to tell how.
   Should you use delete, or is there a different destruction mechanism (e.g., a dedicated destruction
   function the pointer should be passed to)?
4. If you manage to find out that delete is the way to go, Reason 1 means it may not be possible to know
   whether to use the single-object form (“delete”) or the array form (“delete []”). If you use the wrong
   form, results are undefined.
5. Assuming you ascertain that the pointer owns what it points to and you discover how to destroy it, it’s
   difficult to ensure that you perform the destruction exactly once along every path in your code
   (including those due to exceptions). Missing a path leads to resource leaks, and doing the destruction
   more than once leads to undefined behavior.
6. There’s typically no way to tell if the pointer dangles, i.e., points to memory that no longer holds
   the object the pointer is supposed to point to. Dangling pointers arise when objects are destroyed
   while pointers still point to them.
Smart pointer is wrapper around raw pointer that tries to solve this issues. There are 4 smart pointers
in C++11: auto_ptr, unique_ptr, shared_ptr, weak_ptr. Where auto_ptr is a deprecated leftover from C++98
and is replaced with unique_ptr.

---------------------------------------------------------------------------------------------------------
                                    Auto_ptr
---------------------------------------------------------------------------------------------------------
Before C++11, there was no move semantics mechanism in syntax therefore it was emulated using copying.
Example of smart_pointer implemented this way:
It would be nice to have a wrapper around an object that is automatically alocated therefore clean on
stuck unwinding. Well, this seems to be not taugh to implement
template<class T>
class Auto_ptr {
    T* m_ptr;
public:
    Auto_ptr1(T* ptr=nullptr) :m_ptr(ptr) { }
    ~Auto_ptr1() { delete m_ptr; }
    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }
}
But there is no copy constructor, resulting to shallow copying
Auto_ptr<int> ptr1(new int);
Auto_ptr<int> ptr2(ptr1);
// Double free, undefined behaviour, crash
Possible solution is to make copy constructor/operator private or delete, however, how will we pass/return
arguments to/from function. We need to define them but they must move responsibility from operands to
assignee therefore preventing resource leakage and undefined behaviours.
    // A copy constructor that implements move semantics
    Auto_ptr2(Auto_ptr2& a) // note: not const
    {
        m_ptr = a.m_ptr; // transfer our dumb pointer from the source to our local object
        a.m_ptr = nullptr; // make sure the source no longer owns the pointer
    }
    // An assignment operator that implements move semantics
    Auto_ptr2& operator=(Auto_ptr2& a) // note: not const
    {
        if (&a == this)
            return *this;
 
        delete m_ptr; // make sure we deallocate any pointer the destination is already holding first
        m_ptr = a.m_ptr; // then transfer our dumb pointer from the source to the local object
        a.m_ptr = nullptr; // make sure the source no longer owns the pointer
        return *this;
    }
The next problem is how do we return automatic variable from function. We can`t return a pointer nor
reference since it will be dangling, The only solution is to return by value while moving a responsibility.

Problems of such approach:
1. Any assignment moves pointer, every time you pass it by value as operand or return from function, which
   requires you to watch for object which is not resposible, otherwise you could forget about it and try
   to use null smart_pointer.
2. Standart implementation used non array delete which resulted to undefined behaviour when such was used
3. Didn`t work with some containers and algorithms that expect object to be copied and not moved

Currently it is deprecated and replaced with unique_ptr, expected to be removed in C++17.
=========================================================================================================
                        Unique_ptr (exclusive-ownership)
=========================================================================================================
std::unique_ptr embodies exclusive ownership semantics. You can`t copy it but you can move which transfers
ownership from the source pointer to the destination pointer, source pointer is set to null. Upon destruction
a non-null unique_ptr destroys its resource by calling delete on a raw poiner. A common use is a factory
returning type for objects in a hierarchy.
---------------------------------------------------------------------------------------------------------
                         Object allocation/dealocation
---------------------------------------------------------------------------------------------------------
unique_ptr, as well as others, expect to get pointer as parameter. Usually you just call `new Type` and
pass result to pointer constructor. However, object constructor may take plenty of arguments or names
are just too long to use in place. For this case, there is a special function that forwards arguments to
object constructor and returns pointer for you.
unique_ptr<Type> f1 = std::make_unique<Type>(arg1, arg2, arg3, ...);
auto f1 = std::make_unique<Type>(arg1, arg2, arg3, ...); // even better

Another reason to call make_unique is exception safety. Imagine case when you pass inplace created pointer
as function argument and function call, as another argument, and it may throw. In such case you could
allocate object, then call function and only then try to construct unique_ptr.
some_function(std::unique_ptr<T>(new T), function_that_can_throw_exception());
If make_unique is used, whether unique_pointer is created first and will be properly cleaned, or function
throws and pointer was not allocated before.



Deletion can be performed with custom function
auto delType = [](Type* pOjb) { delete pObj; };
unique_ptr<Type, decltype(delType)> unique_ptr pType(nullptr, delType);
pType.reset(new Type( std::forward<Ts>(params)...));
---------------------------------------------------------------------------------------------------------
                               Using as argument
---------------------------------------------------------------------------------------------------------
It can be safely returned from a function by a value, move semantics will be used.
You can pass by value to function but it requires explicit move, copy operations are deleted.
You can pass by reference, then function can use it without assuming ownership. Instead, it`s better to
just pass the resource itself. This allows the function to remain agnostic of how the caller is managing
its resources. Raw pointer is accessed with get() member function.

---------------------------------------------------------------------------------------------------------
                                    Pitfalls
---------------------------------------------------------------------------------------------------------
Don`t let multiple classes manage the same resource
Resource *res = new Resource;
unique_ptr<Resource> res1(res);
unique_ptr<Resource> res2(res);

Don`t manually delete resource underneath it.
Resource *res = new Resource;
unique_ptr<Resource> res1(res);
free(res);

Both issues are preventable with make_uniques.
=========================================================================================================
                            Shared_ptr
=========================================================================================================
Shared_ptr is meant to solve the case where you need multiple smart pointers co-owning a resource, meaming
it is fine to have multiple owners of a resource. Internally class maintains a reference counter of owners.
As lons as at least one shared_ptr points to the resource, it is not deallocated.

	Resource *res = new Resource;             // Resouce is allocated
	std::shared_ptr<Resource> ptr1(res);
	{
		std::shared_ptr<Resource> ptr2(ptr1); // ptr2 is copy initialized from ptr1
		std::cout << "Killing one shared pointer\n";
	}                                        // ptr2 goes out of scope here, but nothing happens
	std::cout << "Killing another shared pointer\n";
                                             // object gets deallocated

---------------------------------------------------------------------------------------------------------
                        Controll block
---------------------------------------------------------------------------------------------------------
Under the hood, sharred_ptr holds two pointer, one for resource and another for block holding reference
counter and some other fields. When the first shared pointer for a resource is allocated, block is allocated
as well, futher pointer will just assign their pointer to block. Calling `make_shared` may allocate both
pointer and block in one allocation making faster.
---------------------------------------------------------------------------------------------------------
A shared_ptr can be initialized from unique_ptr using rvalue move constructor or assignment operator.
This means that you can forward ownership from unique_ptr to shared through move. But there is no easy
way to convert shared into unique.

---------------------------------------------------------------------------------------------------------
                                    Pitfalls
---------------------------------------------------------------------------------------------------------
If shared_pointer was dynamically allocated and missed to be freed or is part of such object, it`s leaked.
It has no proper implemantation to manage arrays in C++14/17.

=========================================================================================================
                                    Weak_ptr
=========================================================================================================
Weak_ptr comes to solve an issue of cyclyc dependencies withing objects controlled by shared_ptr.
The problem is that, when there is a cycle between shared pointer, reference counter will not go to zero
and resource will be leaked. A weak_ptr points to a shared_ptr but does not increase its reference count,
therefore, object can be deleted independent of pointer.
This works because we create an instance of shared_ptr from weak for whenever one wants to use the
underlying object. If object was previously deleted, empty instance of shared_ptr is returned.

class A { shared_ptr<B> b; ... };
class B { shared_ptr<A> a; ... };
shared_ptr<A> x(new A);  // 1 reference for A
x->b = new B;            // 1 reference for B
x->b
// Ref count of 'x' is 2.
// Ref count of 'x->b' is 1.
// When 'x' leaves the scope, there will be a memory leak:
// 2 is decremented to 1, and so both ref counts will be 1.
// (Memory is deallocated only when ref count drops to 0)->a = x;             // 2 references for A

The programmer needs to be aware of the ownership relationship among the objects, or needs to invent an
ownership relationship if no such ownership exists naturally.
// Ref count of 'x' is 1.
// Ref count of 'x->b' is 1.
// When 'x' leaves the scope, its ref count will drop to 0.
// While destroying it, ref count of 'x->b' will drop to 0.
// So both A and B will be deallocated.

The downside of weak_ptr is that it is not directly usable (has no operator->), instead you should convert
it to shared_ptr first, using `lock()` member function. 
shared_ptr<A> ptr_s(new A);
weak_ptr<A> ptr_w(ptr_s);
auto ptr_s2 = ptr_w.lock();


http://geekwentfreak.com/posts/c/cpp_create_avoid_cyclic_dep/
https://stackoverflow.com/questions/4984381/shared-ptr-and-weak-ptr-differences
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
                              Function overloading
=========================================================================================================
Function overloading is a way to reduce symantic duplications by providing multiple function with
different funtion declaration and maybe definition but using the same name, e.g.
bool max(int,int)
bool max(double,double)
bool max(int,double,short)
It is realized by mangling function name and then on compilation phase, put needed call depending on
arguments. Mangling means encoding additional data into function name, e.g. number of parameters, their
type, therefore they can be differentiated even with the same code name.
There are cases when you have changed some types but it doesn`t compile. Return type are not considered
for uniqieness since it leaves ambiguos which function should be choosen.
int getRandomValue();       # Won`t compile
double getRandomValue();
Another moment, typedefs are not distinct, since they does not introduce a new type but create an alias.
typedef char *string;       # Won`t compile
void print(string value);
void print(char *value);
---------------------------------------------------------------------------------------------------------
                        Matching algo
---------------------------------------------------------------------------------------------------------
When an overloaded function is called, C++ goes through the following process to determine which version
of the function will be called:
1. Compiler looks for an exact match
   void print(char *value);
   void print(int value);
   print(0); // exact match with print(int)
2. Compiler looks for a match through a promotion
   void print(char *value);
   void print(int value);
   print('a'); // promoted to match print(int)
3. If no promotion is possible, compiler tries to find a match through standard conversion.
   Standard conversions include:
   Any numeric type will match any other numeric type, including unsigned (e.g. int to float)
   Enum will match the formal type of a numeric type (e.g. enum to float)
   Zero will match a pointer type and numeric type (e.g. 0 to char*, or 0 to float)
   A pointer will match a void pointer 
4. Compiler uses user defined convertions
   class X; // with user-defined conversion to int
   void print(float value);
   void print(int value);
   X value; // declare a variable named value of type class X
   print(value); // value will be converted to an int and matched to print(int)

You can expcitly define type of the passed object using cast or literal suffix
print(static_cast<unsigned int>(x));
print(0u);

---------------------------------------------------------------------------------------------------------
                               Default arguments
---------------------------------------------------------------------------------------------------------
It is possible to define default value for function argument which is used if user doesn`t provide one.
It is possible to supply multiple default arguments but they must be the last in argument list
void printValues(int x=10, int y=20, int z=30)
{
    std::cout << "Values: " << x << " " << y << " " << z << '\n';
}
Default argument can be declared only onced and not redefined. The best pracrive is to define function in
header with default arguments and definiton in source file without it.
It is possible to overload function with default parameters, but they will not take part overloading
decision unless explicitly used.
void print(std::string string);
void print(char ch=' ');
print(); // calls one with a default value

void printValues(int x);
void printValues(int x, int y=20);
printValues(1);  // Compile time error, ambiguos call
---------------------------------------------------------------------------------------------------------
Demangling is usually implemented in high level languages, but is not present in low level like C. There
are some ways to simulate oveloading:
1. Parsing vararg arguments
void va_overload2(int p1, int p2) {
	printf("va_overload2 %d %d\n", p1, p2);
}

void va_overload3(int p1, int p2, int p3) {
	printf("va_overload3 %d %d %d\n", p1, p2, p3);
}
static void va_overload(int p1, int p2, ...) {
	if (p2 == 7)
	{
		va_list v;
		va_start(v, p2);
		int p3 = va_arg(v, int);
		va_end(v);
		va_overload3(p1, p2, p3);
		return;
	}
	va_overload2(p1, p2);
}
2. Using tagged union as argument
typedef enum {
    T_INT,
    T_FLOAT,
    T_CHAR,
} my_type;
typedef struct {
    my_type type;
    union {
        int a; 
        float b; 
        char c;
    } my_union;
} my_struct;
3. Using C11 _Generic()

https://locklessinc.com/articles/overloading/
=========================================================================================================
                                     Vararg
=========================================================================================================
C has mechanism to pass variable length list of parameter to function. Such function takes list lenght
as the first parameter and elipsis as the second.
void func(int length, ...) { ... }

Reasons to avoid it:
1. Type checking is suspended.
   You can call func(5, 1, 1.1, 'c' ...) which may result in undefined behaviour. Most likely, you will
   cast all variables to one type, like int, without noticing.
2. You should mannualy count arguments. Possible solutions:
   Passing sential and performing while(true) around it.
   Passing some kind of data encoding type and order of following parameters.
   Mannually counting and passing number of arguments.

https://www.learncpp.com/cpp-tutorial/714-ellipsis-and-why-to-avoid-them/
=========================================================================================================
                                Const/Constexpr
=========================================================================================================
Const is a reserved keyword meanning that variable cannot be modified.
For simple variables, keyword be used before type or after, most people prefer to put it before
const int x == int const x, there is no such syntax for pointers, since it would be ambiguos. To make
pointer constant, use const specifier after *.
int const val           # constant variable
const int val           # 
int *const val          # constant pointer for modifiable variable
const int* val          # modifiable pointer for constant variable
const int* const val    # constant pointer for constant data
It is quite easy to read such declaration from rigth to left, e.g
const int* const val    # val is a constant pointer for a constant data

---------------------------------------------------------------------------------------------------------
                                   Constexpr
---------------------------------------------------------------------------------------------------------
There are two kinds of constants: Runtime and compile-time. Run-time, value is not determined at compile
time and will be resolved at run-time. Compile-time, those whose initialization values can be resolved at
compile-time. In most cases, this doesn’t matter, but there are a few odd cases where C++ requires a
compile-time constant instead of a run-time constant (such as when defining the length of a fixed-size
array). To help provide more specificity, C++11 introduced new keyword constexpr, which ensures that a
constant must be a compile-time constant:
constexpr int sum { 4 + 5 }; // ok, the value of 4 + 5 can be resolved at compile-time
std::cin >> age;
constexpr int myAge { age }; // not okay, age can not be resolved at compile-time

Best practice: Any variable that should not be modifiable after initialization and whose initializer is
known at compile-time should be declared as constexpr. Any variable that should not be modifiable after
initialization and whose initializer is not known at compile-time should be declared as const.

In typicall application you may have multiple constants that are used throught your project. Instead of
redefining these every time they are needed, it’s better to declare them once in a central location and
use them wherever needed. Just create a header and namespace for them.
constants.h:
#HEADER_GUARD...
namespace constants
{
    constexpr double pi { 3.14159 };
    constexpr double avogadro { 6.0221413e23 };
    constexpr double my_gravity { 9.2 }; // m/s^2 -- gravity is light on this planet
    // ... other related constants
}
#END
To avoid duplication in each file, forward declare then in header file as extern and define in source file.
namespace constants
{
    extern const double pi;
    extern const double avogadro;
    extern const double my_gravity;
}
But this following drawback: they are not compile time constants and it`s more heavy in term of
optimization. But any changes to .cpp file require only it`s recompilation + linkage.

Constexpr for functions will make compiler call it at compile time if it is possible and used somewhere.
int size() { return 3; }
contexpr int size() { return 3; }
int arr[size()];
If first definition is used, code will not compile since it is not valid to define automcatic stack at
runtime. The second definitons fixes the issue.


https://www.learncpp.com/cpp-tutorial/42-global-variables/
https://www.learncpp.com/cpp-tutorial/const-constexpr-and-symbolic-constants/
---------------------------------------------------------------------------------------------------------
                                    Probles
---------------------------------------------------------------------------------------------------------
Note that making variable const is contract with compiler, if it notices that you change const variable
it will stop compilation with error. But if it doesn`t notice it, you can easily modify the value, for
instance, through pointer.

// Compiler with warning about pointer casting
int main() 
{ 
    const int var = 10; 
    int *ptr = &var; 
    *ptr = 12; 
    printf("var = %d\n", var);  // 12
    return 0; 
} 

---------------------------------------------------------------------------------------------------------
                                    Typedef
---------------------------------------------------------------------------------------------------------
There is a small pitfall when using typedef with pointer
typedef char *string;
const string astring;
This code expands to char * const, not const char *. This cone be avoided by avoiding typedef with
pointers or by putting const after type.
---------------------------------------------------------------------------------------------------------
                            Const-correctness: semantic and syntactic
---------------------------------------------------------------------------------------------------------
There are two components about using the keyword const in C++: syntactic and semantic:
Syntactic is what the compiler figures out at compile time. If declare a const variable, compiler won`t
let us modify it.
int const cantModify = 42;
cantModify = 33; //ERROR


---------------------------------------------------------------------------------------------------------

Const object can call only const methods.

https://stackoverflow.com/questions/1143262/what-is-the-difference-between-const-int-const-int-const-and-int-const
=========================================================================================================
                                  Declaration
=========================================================================================================
There is a technique known as the ``Clockwise/Spiral Rule'' which lets any C programmer to parse in their
head any C declaration! 
1. Starting with the unknown element, move in a spiral/clockwise direction;
2. Keep doing this in a spiral/clockwise direction until all tokens have been covered. 
3. Always resolve anything in parenthesis first! 
http://c-faq.com/decl/spiral.anderson.html

=========================================================================================================
                                 Linkage/Duration Scopes
=========================================================================================================
An identifier’s scope determines where it is accessible. An identifier that is out of scope can not be
accessed.
Variables with block scope / local scope can only be accessed within the block in which they are declared
(including nested blocks). This includes: Local variables, Function parameters, Locally-defined types.
Variables and functions with global scope / file scope can be accessed anywhere in the file. This includes:
Global variables, Regular functions (the kind we’ve discussed so far), Globally-defined types

A variable’s duration determines when it is created and destroyed.
Variables with automatic duration are created at the point of definition, and destroyed when the block they
are part of is exited. This includes:
Local variables. Variables with static duration are created when the program begins and destroyed when
the program ends. This includes: Global variables, Static local variables.
Variables with dynamic duration are created and destroyed by programmer request. This includes: Dynamically
allocated variables.

Each variable has a scope in which it is defined. Each scope has a set of variables it can access:
variables define in its scope ,those defined in global or file scope, definitions within class or within
wrapping namespace  and corner case is closures which may capture variables from scope and be executed
when there is no scope. It is possible to hide outer scope variables by iner scope ones (called shadowing).
There is a way to get outerscope variables even if they shadowed using `::` scope operator.
There are ways to prevent user to access you variables, you can change linkage of symbol by using
static/const/extern modifiers.

By default all globally defined symbols (functions, defined variables) are globally accessible. You can
just prefix it with extern within another compilation unit and linker will relocate need symbol.
If you want to make global variable local to a file, we can use static or const keyword to do so:
static int g_x; // g_x is static, and can only be used within this file
const int g_x = 10;
But you can still make const variable globally accessible by explicilty defining extern, this doesn`t for
static since they mean staitic and extern are oposite things:
extern static g_x; // won`t compile?
extern const ing g_x = 10; // variable will be globally accessible even through const

https://www.learncpp.com/cpp-tutorial/4-3a-scope-duration-and-linkage-summary/
https://www.learncpp.com/cpp-tutorial/42-global-variables/
---------------------------------------------------------------------------------------------------------
                                     Extern
---------------------------------------------------------------------------------------------------------
Imagine having global variable that you want to use among multiple source failes. If you put it to header,
its defenition get duplicated into each source file and you are getting separate copy whithin each file
or multiple definiton error, behaviour is compiler dependant. What you want, is linker allocationg space
for one variable and just fixing up references to it.
To solve this issue, just define it in one source file and declare as extern in all others. Or a more
maintainable solution is to put extern declaration in header file and include it where needed. If you
include this to file with definition, it is not considered to be an error. Note, that declaration can be
done multiple types with only one definition.

This is precisely how the global errno variable is implemented in the standard library. The errno.h header
file contains the declaration: extern int errno;

Note, that anything defined in global scope is by default extern accessible.

It is valide to declare and define at the same moment a variable
extern int var = 0;
int main(void) {
 var = 10;
 return 0;
}

Order of declaration/usage of global variables doesnt matter
int main(void) {
 var = 10;
 return 0;
}
extern int var = 0;

Extern ca be used to prevent variable defenition and start lookup process
int y = 2;
int main(){
    extern int y;
    printf("%d" ,y); //2
} 
=========================================================================================================
                                    Extern
=========================================================================================================
Extern is a reserved keyword used to declare objects in scope of one object file that is defined is scope
of another or to clarify demangling behaviour for compiler, i.e. if you call C function from cpp code,
it should be called without mangling.

---------------------------------------------------------------------------------------------------------
                                   Demangling
---------------------------------------------------------------------------------------------------------
Use extern "C" in following cases:
1. Need to use some C library in C++
   extern "C" int foo(int);
2. Need export some C++ code to C
   extern "C" int foo(int) { something; }
3. We need an ability to resolve symbol in shared library -- so we need to get rid mangling
   extern "C" int foo(int) { something; }
   ///
   typedef int (*foo_type)(int);
   foo_type f = (foo_type)dlsym(handle,"foo")
4. When linking to a library that was compiled as C code
   extern "C" {
     #include "c_only_header.h"
   }

https://stackoverflow.com/questions/1292138/when-to-use-extern-c-in-c
---------------------------------------------------------------------------------------------------------
https://www.quora.com/Whats-the-use-of-extern-with-variable-in-C
http://www.goldsborough.me/c/c++/linker/2016/03/30/19-34-25-internal_and_external_linkage_in_c++/
=========================================================================================================
                                     Static
=========================================================================================================
static is a reserver keyword that can be used to change behaviour of variable allocation, function call,
program linkage. If function/variable is declared on file scope as static, it is visible for this file
only. 
---------------------------------------------------------------------------------------------------------
                                   Functions
---------------------------------------------------------------------------------------------------------
Static functions can be called without objects. Example usage, you may want to count number of class object
created. If we used nonstatic function there is a problem when no object was no created, and we have no
instance of which to call such function. Another common usage is `Singleton` pattern. Compiler won`t pass
`*this` to such functions.
class Test{ 
    static int x; 
    Test() { x++; } 
    static int getX() {return x;} 
};
    cout << Test::getX() << " ";    // 0
    Test t[5]; 
    cout << Test::getX();           // 5
Since you have no object constructed, during such function call, you can`t access non static members/functions.
---------------------------------------------------------------------------------------------------------
                                 Class members
---------------------------------------------------------------------------------------------------------
Static may be used for class members meaning that it is shared among all instances. Since there may be
no instance at all, or at current moment, this variable can be used without instance. You may query class
for such member using scope operator. Note, declaration of static member, won`t allocate/define it,
consider it to be a forward declration, it should be done explicitly outside of class body. If you haven`t
defined such member, expect a linker error. As well, it doesn`t care about access controls. Even if it is
private member, you can still define and initialize it outside of class.
class foo {
    static int x;
    foo();
}
int foo::x = 1; // Initialization
cout << foo::x;
foo f;
cout << f.x;   // Same as above

There are some shortcuts for initialization. If static member is as well const integral type, it can be
initialized inside the class definition.
---------------------------------------------------------------------------------------------------------
                                    Problems
---------------------------------------------------------------------------------------------------------
There is no easy way to use purelly static class (called monostate). There is no easy way to have copy
of it used for different purpose (maybe somehow calling clone on it). It`s instance would be a global
object making it more hard to maintain/debug. It is not possible to have static constructor.
What if you need to execute some code to initialize such values.

https://www.learncpp.com/cpp-tutorial/812-static-member-functions/ TODO Monostate
---------------------------------------------------------------------------------------------------------
Static keyword before function is supposed to be used in class body and not outside, otherwise cimpilation
error
class Test {
    void fun();
};
static void Test::fun(){
    std::cout<<"fun() is static\n";
}
int main() {
    Test::fun();
    return 0;
}
[Error] cannot declare member function ‘void Test::fun()’ to have static linkage [-fpermissive]
=========================================================================================================
                                   Namespaces
=========================================================================================================
Namespace defines an area of code in which all identifiers are guaranteed to be unique. By default, global
variables and normal functions are defined in the global namespace which may result in name collision if
multiple identifiers with the same name are used. In order to help avoid issues where two independent
pieces of code have naming collisions with each other when used together, C++ allows us to declare our
own namespaces via the namespace keyword. You need to use scope operator `::` to get symbols from namespace.
namespace <name>{
    .. code ..
}
<name>::<local_symbol_name>;

It’s legal to declare namespace blocks in multiple locations (either across multiple files, or multiple
places within the same file). All declarations within the namespace block are considered part of the namespace.

---------------------------------------------------------------------------------------------------------
                                    Aliases
---------------------------------------------------------------------------------------------------------
Namespaces can be nested. Because typing the fully qualified name of a variable or function
inside a nested namespace can be painful, C++ allows you to create namespace aliases.
namespace Inner_Allias = Outer::Inner
Inner_Allias::symbol;
Note, that they were not designed to implement information hierarchy, but to avoid naming collisions.
As evidence of this, note that the entirety of the standard template library lives under the singular
namespace std::. Some newer languages (such as C#) differ from C++ in this regard.

You can simplify namespace expansion for often occuring symbols using `using` declaration e.g.
`using std::cout; using std::cin`. When compiler sees `cout` it will assume that we mean `std::cout`.
You can include entire namespace with `using namespace <name>`. This declaration follows scoping rules.


=========================================================================================================
                            Casting && Explicit
=========================================================================================================
Cast is a process of type convertsion which can happen in many different cases:
1. Assigning to or initializing a variable with a value of a different data type
2. Passing a value to a function where the function parameter is of a different data type
3. Returning a value from a function where the function return type is of a different data type
4. Using a binary operator with operands of different types.

In all of these cases (and quite a few others), C++ will use type conversion to convert data from one type
to another. There are two basic types of type conversion: implicit type conversion, where the compiler
automatically transforms one fundamental data type into another, and explicit type conversions, where
the developer uses a casting operator to direct the conversion.

Implicit type conversion (also called automatic type conversion or coercion) is performed whenever one
fundamental data type is expected, but a different fundamental data type is supplied, and the user does
not explicitly tell the compiler how to perform this conversion. There are two basic types of implicit
type conversion: promotions and conversions.

Numeric promotion or widening. Whenever a value from one type is converted into a value of a larger similar
data type. For example, an int can be widened into a long, or a float promoted into a double. This is
further subdivide into:
- Integral promotion involves the conversion of integer types narrower than int  to an int (if possible)
  or an unsigned int (otherwise).
- Floating point promotion involves the conversion of a float to a double.
Promotions are save and no data loss will result. Under the hood, promotions generally involve extending
the binary representation of a number (e.g. for integers, adding leading 0s).

Numeric conversions. When we convert a value from a larger type to a similar smaller type, or between
different types. Unlike promotions, which are always safe, conversions may or may not result in a loss of
data. Because of this, any code that does an implicit conversion will often cause the compiler to issue a
warning (on the other hand, if you do an explicit conversion using a cast, the compiler will assume you
know what you’re doing and not issue a warning). Under the hood, conversions typically require converting
the underlying binary representation to a different format.
double d = 3; // convert integer 3 to a double (between different types)
short s = 2; // convert integer 2 to a short (from larger to smaller type)

---------------------------------------------------------------------------------------------------------
                                Casts
---------------------------------------------------------------------------------------------------------
There are following casts in C++: static_cast, dynamic_cast, reinterpret_cast, cost_cast.
It is preferred to use C++ casts over C style casts, since C++ style casts are checked by the compiler,
while C style casts aren't and can fail at runtime. As well C++ casts can make intent of developer more
clear.

---------------------------------------------------------------------------------------------------------
Static_cast
---------------------------------------------------------------------------------------------------------
Static_cast. This is a formal form of implicit conversion, with a few restrictions and additions.
This cast performs no runtime check, therefore it is expected that you know that you refer to an object
of a specific type, and thus check wiuld be unnecessary. Should be used instead of C style casting.
Should be used for upcasting and can be used with care for downcasting, you should be certain that there
is a child part and it`s type, otherwise undefined behaviour. Developer may be tempted to wrap usage of
static cast into some form of RTTI which is bad idea in terms of code quality/performance.
void func(void *data) {
  MyClassBase *b = static_cast<MyClassBase*>(data);
  MyClassChild *c = static_cast<MyClassChild*>(b);
  ...
}


Don`t do following
enum ClassID
{
	BASE,
	DERIVED
	// Others can be added here later
};
virtual ClassID getClassID() {...}
Base *b = getObject(true);
if (b->getClassID() == DERIVED)
{
	// We already proved b is pointing to a Derived object, so this should always succeed
	Derived *d = static_cast<Derived*>(b);
	std::cout << "The name of the Derived is: " << d->getName() << '\n';
}
---------------------------------------------------------------------------------------------------------
Dynamic_cast
---------------------------------------------------------------------------------------------------------
Dynamic_cast. Used when you don`t know what the dynamic type of object is. It returns a null pointer if
the object referred to doesn`t contain the type casted to as a base class (when you cast to reference,
a bad_cast exception is thrown in that case). It is more safe then static_cast but has considerable
performance overhead.
if (Class1 *c1 = dynamic_cast<Class1*>(&data)) {
  ...
} else if (Class2 *c2 = dynamic_cast<ExprStm*>(&data)) {
  ...
}

try
{ 
  MyChild &child = dynamic_cast<MyChild&>(*base);
}
catch(std::bad_cast &e) 
{ 
  std::cout << e.what(); // bad dynamic_cast
}
Note, you can`t use dynamic_cast for downcast when argument is not polymorphic (class without virtual
functions).

In general, using a virtual function should be preferred over downcasting. However, there are times when
downcasting is the better choice:
1. When you can not modify the base class to add a virtual function (e.g. because the base class is part
   of the standard library)
2. When you need access to something that is derived-class specific (e.g. an access function that only
   exists in the derived class)
3. When adding a virtual function to your base class doesn’t make sense (e.g. there is no appropriate
   value for the base class to return). Using a pure virtual function may be an option here if you don’t
   need to instantiate the base class.

---------------------------------------------------------------------------------------------------------
Reinterpret_cast
---------------------------------------------------------------------------------------------------------
Reinterpret_cast. The same as C-style cast, is simply perform a binary copy of the data without altering
the underlying bit patter.
char c = 10;
int *r = reinterpret_cast<int*>(&c); # takes address of character and converts it to int pointer
---------------------------------------------------------------------------------------------------------
Const_cast
---------------------------------------------------------------------------------------------------------
Const_cast. Used to remove/add cv modifier. Casting const away may let you to change value of variable
but doing so is still invalid code that may cause a run-time error (what if variable is located in a
section of read-only memory). It is used for cases, when library function takes non-const variable without
changing it. Or to prevent code duplication, you can define const variant and call it from nonconst by
casting `this` pointer. Or if you want to force usage of const function during function overloading.

Const and Volatile are orthogonal. Const means the data is read-only and is probably in read-only memory.
Volatile means the variable could be changing due to external reasons so the compiler needs to read the
variable from memory time it is referenced. Note, you can avoid volatile usage with memory barrier wich
force the compiler into not caching the variable.

https://stackoverflow.com/questions/2491495/may-volatile-be-in-user-defined-types-to-help-writing-thread-safe-code
---------------------------------------------------------------------------------------------------------
C cast
---------------------------------------------------------------------------------------------------------
Regular cast (C cast). It is the same to calling all C++ cast in defined order and taking cast that works
without considering dynamic_cast. It`s really powerfull but not safe. Note, that while reinterpret_cast
performs bitwise copting, regular cast may be smarted and know how to convert some pritive types so they
stil have some valid value e.g. if you reinterpret_cast int to float, it will have no sense and value
will be undefined, C will use convertion rulles, as well as static_cast.
---------------------------------------------------------------------------------------------------------
                               Upcast && Downcast
---------------------------------------------------------------------------------------------------------
There are two possible directions of cast in classes: derived class can be casted to base and base can
be casted to derived (if it dynamic type allows). Both upcasting and downcasting do not change object by
itself. When you use upcasting or downcasting you just "label" an object in different ways.

class Employee {};
class Manager: class Employee {};
class Clerk: class Employee {};

void do_work(Employee *ptr);

Upcasting is a process of treating a pointer or a reference of derived class object as a base class pointer.
You do not need to upcast manually. You just need to assign derived class pointer (or reference) to base
class pointer:

Manager m1;
Employee *emp = &m1;                    // implicit upcasting
do_work(m1)                             // implicit upcasting
do_work(static_cast<Employee*>(m1));     // explicit upcast

Downcasting is an opposite process for upcasting. It converts base class pointer to derived class pointer.
Downcasting must be done manually. It means that you have to specify explicit type cast. This one is
unsafe and requires explicit cast as well check if it was successfull.
Employee* emp = new Manager();
Manager* m = (Manager*)(emp);   // C style, not safe, prefer dynamic cast
Dynamic_cast work is type of RTTI (Run-time Type identification), it answers the question of whether we
cas safely assign the address of an object to a pointer of particular type. There is one more operator
for RTTI called `typeid()`. It provides some compiler dependent output that can be used as follows
if (typeid(Class) == typeid(object_of_class)) {
    Class *ptr = (Class*)&object_of_class;
    ptr->doWork();
}
It returns of object of type_info class object. Usually, you want to avoid any form of RTTI in production
code, but it feasable for testing purposes. For instance you may have test to see if factory returns
expected class. If you find yourself needing to write code that behaves differently based on the class of
an object, consider one of the following alternatives to querying the type:
Virtual methods are the preferred way of executing different code paths depending on a specific subclass
type. This puts the work within the object itself.
If the work belongs outside the object and instead in some processing code, consider a double-dispatch
solution, such as the Visitor design pattern. This allows a facility outside the object itself to determine
the type of class using the built-in type system.
Decision trees based on type are a strong indication that your code is on the wrong track. Such code will
break when new classes are added. If properties of a subclass change, it is difficult to find and modify
all the affected code segments.

Because RTTI has a pretty significant space performance cost, some compilers allow you to turn RTTI off
as an optimization. Needless to say, if you do this, dynamic_cast won’t function correctly.

https://www.bogotobogo.com/cplusplus/dynamic_cast.php
---------------------------------------------------------------------------------------------------------
                                Access rights
---------------------------------------------------------------------------------------------------------
C style cast allow you to safely cast to a private base-class, while static_cast would give you a compile
time error for that. 
---------------------------------------------------------------------------------------------------------
                                    Explicit
---------------------------------------------------------------------------------------------------------
The compiler is allowed to make one implicit conversion to resolve the parameters to a function,
If a class has a constructor which can be called with a single argument, then this constructor becomes
conversion constructor because such a constructor allows conversion of the single argument to the class
being constructed. This limit was lifted in C++11 whith introduction if uniform initialization syntax.
This implicit convertion can be prevented for one constructor a time with keyword explit, before
constructor name.

class Foo {
  Foo (int foo) : m_foo (foo) { }
  int GetFoo () { return m_foo; }
  int m_foo;
};
void DoBar (Foo foo) {
  int i = foo.GetFoo ();
}
DoBar (42);
If constructor is define as `explicit Foo (int foo) : m_foo (foo) {}`, previous call provides an eror.
You can explicitly call constructor of wanted class or cast argument to class and pass it as argument.
DoBar (Foo(42));
DoBar ((Foo)42);

class String {
    String(int n); // allocate n bytes to the String object
    String(const char *p); // initializes object with char *p
};
String mystring = 'x';  // calls int constructor

Another implicit cast, that you can ban is operator cast.
https://stackoverflow.com/questions/121162/what-does-the-explicit-keyword-mean
---------------------------------------------------------------------------------------------------------
                                  Boost casts
---------------------------------------------------------------------------------------------------------
TODO.
---------------------------------------------------------------------------------------------------------
                                Double dispatch
---------------------------------------------------------------------------------------------------------
So, you are in the situation when you’ve already considered adding another virtual function on the base
class, but you’re worried about how many virtual functions are already there, or the impact that this might
have on consumers of the base class.
You desided to =>
1. Use dynamic_cast to find out actual type of a pointer, then moving on to do what you really want
2. Check some enum in a base class to see which derived class it really is so that you can perform type
   specific operations on it. (At least, don`t use enum/string and prefer typeid)

Double-dispatch Visitor pattern is possible solution for this type of a problem. With it, you can trim down
long if-else if blocks, separate responsibility into manageable pieces, and stabilize your interface better.

You may be tempted to create a table mapping class type to action, but this is the poor implementation
of virtual table.
ReactionHash&amp; Person::GetReactionFunctions() const{
  return {
           {CAT, &amp;TryToPet},
           {DOG, &amp;RunAwayFrom},
           {PIG, &amp;TryToPet},
           // etc.
         };
}
 
void Person::ReactTo(Animal* _animal){
  const auto&amp; reactionFunctions = GetReactionFunctions();
  reactionFunctions[_animal-&gt;GetType()](_animal);
}

The first variation of visitor is Single-Dispatch. We need to define visitor, responsible for holding
possible visitors, and virtual function `Visit` to object whose dynamic type we don`t know.
struct ReactionVisitor
{
  explicit ReactionVisitor(Person* _person) : person_{_person} {}
  Person* person_ = nullptr; // person doing the reacting
};
struct Animal
{
  virtual std::string Noise() const = 0;
  virtual ~Animal() = default;
  virtual void Visit(ReactionVisitor& _visitor) = 0;
};
void Dog::Visit(ReactionVisitor& _visitor){
  Person* personWhoIsReacting = _visitor.person_;
  if (my_breed == DogBreed.Daschund)
    personWhoIsReacting.TryToPet(this);
  else
    personWhoIsReacting.RunAwayFrom(this);
}

Person reaction looks like
void Person::ReactTo(Animal* _animal){
  ReactionVisitor visitor{this};
  _animal->Visit(visitor);
}

This way, we removed any derived information from base class, separated all the per-animal reaction logic,
it`s fairly easy to add a new derived Animal class and enforce that Person has to react to in in some way.
If we want to allow for a default action, then we can implement the virtual function on the base Animal
class itself. Performance-wise, it’s the same as RTTI approach.

Problems of such approach:
1. Dog should not dictate how a Person reacts to it. Implementation details of the Person class are leaked.
2. What if the Person class has other behaviors they want to implemented?
   We’ve separated responsibility but at the cost of being extremely invasive to the Animal classes.

Solutiong is to make animal detect it`s own dynamic type and pass `this` to visitor which overrides definiton
for all required childs. Calls stach will look like follows
void Person::ReactTo(Animal* _animal) {
    ReactionVisitor visitor{this};
    _animal->Visitor(&visitor);
}
void Cat::Visitor(AnimalVisitor* _visitor) {
    _visitor->Visit(this);
}
void ReactionVisitor::Visit(Cat* _cat) {
    ...
}
https://gieseanw.wordpress.com/2018/12/29/stop-reimplementing-the-virtual-table-and-start-using-double-dispatch/
=========================================================================================================
                                  Enumerations
=========================================================================================================
An enumerated type (also called an enumeration or enum) is a data type where every possible value is
defined as a symbolic constant (called an enumerator). Enumerations are defined via the enum keyword.
Enumerated types are used for code documentation and readability purposes when you need to represent
a specific, predefined set of states.
// Define a new enumeration named Color
enum Color
{
    // Here are the enumerators
    // These define all the possible values this type can hold
    // Each enumerator is separated by a comma, not a semicolon
    COLOR_RED,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_MAGENTA, // trailing comma not allowed before C++11
}; // however the enum itself must end with a semicolon
By defaul, the first symbolic constant is set to 0 and any non-defined enumerator equals to previous+1.
These integer values can be positive or negative and can share the same value as other enumerators
Best practice: Don’t assign specific values to your enumerators.
Rule: Don’t assign the same value to two enumerators in the same enumeration unless there’s a good reason.

Complier won`t inplicitly conver value to enumerator, therefore explicit cast is required
Animal animal = 5; // will cause compiler error

Each enumerated type is considered a distinct type. Consequently, trying to assign enumerators from one
enum type to another enum type will cause a compile error:
Animal animal = COLOR_BLUE; // will cause compiler error

Enum types are considered part of the integer family of types, and it’s up to the compiler to determine
how much memory to allocate for an enum variable. The C++ standard says the enum size needs to be large
enough to represent all of the enumerator values. Most often, it will make enum variables the same size
as a standard int.

Because the compiler needs to know how much memory to allocate for an enumeration, you cannot forward
declare enum types. However, there is an easy workaround. Because defining an enumeration does not allocate
any memory, if an enumeration is needed in multiple files, it is fine to define the enumeration in a header,
and #include that header wherever needed.
---------------------------------------------------------------------------------------------------------
                                  Classes
---------------------------------------------------------------------------------------------------------
Value can`t be implicitly casted to enum but enum to int value is allowed. This result in some ambiguos
code that is considered valid by compiler.
Color color = RED;
Fruit fruit = BANANA;
if (color == fruit) ...     // Not cool

C++11 provide mechanism to make enum strongly typed and strongly scoped, called enum class (scoped enum).
To make an enum class, we use the keyword class after the enum keyword.
enum class Fruit {...}
enum class Color {...}
Color color = Color::RED;
Fruit fruit = Fruit::BANANA;
if (color == fruit) ...         // Doesn`t compile
if (color == Color::RED) ...    // Ok

Note, it is possible to use struct instead of class but it is not recommended not commonly used.

https://www.learncpp.com/cpp-tutorial/4-5a-enum-classes/
=========================================================================================================
                                Typedef vs Using
=========================================================================================================
Typedefs allow the programmer to create an alias for a data type, and use the aliased name instead of the
actual type name. By convention, typedef names are declared using a “_t” suffix to idicate that they are
types and not variables. Typedef does not define a new type, rather, it is simply an alias 
Reasons to use typedef:
1. It can be used to describe purpose of a value
   typedef int testScore_t;
   testScore_t GradeTest();
2. Code maintance. You can use typedef around code and change underlying type whenever it is needed.
   This saves your time.
3. Hiding platform dependent types
   #ifdef INT_2_BYTES
   typedef int int16_t;
   typedef long int32_t;
   #else
   typedef short int16_t;
   typedef int int32_t;
   #endif
4. Making comples names simpler
   typedef std::vector<std::pair<std::string, int> > pairlist_t; // simple name

However, it is quite ugly and there are problems with pointer that developer should be aware of. To help
address this issues, C++11 presented impoved syntax for typedef called type alias. Note, this and one
used for namespaces are different.
typedef double distance_t; //
using distance_t = double; // C++11
One important reason to use new syntax is templates. You can`t have templated typedef but can have using.
template<class T>
using Vec = vector<T, Alloc<T>>;

=========================================================================================================
                        OOP | Object Oriented Programming
=========================================================================================================
OOP is built on 4 major principles: Encapsulation, Data Abstraction, Polymorphism and Inheritence
---------------------------------------------------------------------------------------------------------
Encapsulation
---------------------------------------------------------------------------------------------------------
Mechanism of hiding data implementation by restricting access to public methods. Instance variables are
kept private and accessor methods are made public to achieve this. This way, user is independent of
implementation as far as interface is the same, making it easy to change implementation without user
noticing. Otherwise user would be required to change his code using your class whenver you changed it.

---------------------------------------------------------------------------------------------------------
Abastraction
---------------------------------------------------------------------------------------------------------
Abstract means a concept or an Idea which is not associated with any particular instance. Using abstract
class/Interface we express the intent of the class rather than the actual implementation.  Usually
implemented with some form of interface. Anyone working with instance of it, doesn`t need to know
implementation details, knowing interface is enough.
This is needed for big/scalable projects where it is much easier and maintainable to provide some high
level mechanism for using it. Preferably, this mechanism should be easy to use and should rarely change
over time.

---------------------------------------------------------------------------------------------------------
Inheritance
---------------------------------------------------------------------------------------------------------
Often you have mamy simmilar objects that may use some common definiton and redefine it if needed.
Inheritance expresses `is-a` relationship, making it possible to reuse implementation of super class
withoud duplicating the code. Such code is considered to be reusable, and it is much more easy to maintain.
If you find probleb in derived method, you need to fix it once and each class will have proper definition.

---------------------------------------------------------------------------------------------------------
Polymorphism
---------------------------------------------------------------------------------------------------------
Means one name for many forms. There are static and dynamic polymorphism. The first is implemented
using method overloading, you know which function will be called at compile time. The second is implemented
using method overriding, you don`t know which definiton is called until runtime.
---------------------------------------------------------------------------------------------------------
https://www.freecodecamp.org/news/object-oriented-programming-concepts-21bb035f7260/
https://medium.com/@cancerian0684/what-are-four-basic-principles-of-object-oriented-programming-645af8b43727
=========================================================================================================
                            Classes
=========================================================================================================
---------------------------------------------------------------------------------------------------------
                Public | Private | Protected | Access specifiers
---------------------------------------------------------------------------------------------------------
Access speicifiers are one of the mechanisms used to create incapsulation.
There are three different specifiers: public, private and protected.
1. Public. Members of a struct or class can be accessed from outside of the struct or class
2. Private. Members of a class that can only be accessed by other members of the class. This can be
   another member function with the same constness or friend class.
3. Protected.
Fields in class/struct get specifier of previous field. There may multiple access specifiers per class.
class foo {
    int private_member1;
    public: int public_member1;
    private: int private_member2;
    public: int public_member2;
};

By default all members of struct are public and private for class, the same for default inheritence.
=========================================================================================================
                                  Destructors
=========================================================================================================
Destructor. Specil kind of class member function that is executed to deinitialize memory before free.
Destructor knows how to clean stack allocated members, how to call their destructors but it doesn`t know
how to deinitialize resources. It must be done mannually by developer. There can be only one destructor
per class and it must not use any parameters. As well as constructor, it has no return type.

You should never throw from destructor/constructor, since there maybe an array of such object, and if
atleast two of them throw, you loose throw reason of the first one. This is considered to be critical
error and program aborts, since there is no way to get reason behind the first issue, it is not clear if
the second was some undefined bahviout, side effect of the first or it is valid. As well it may happen
during stack unwinding, which is performed when you leave scope, or another exception is thown.
If you really care about failure in destructor, consider logging instead.

Destructor can`t take parameters. This would make some little sense in place new, since it is applicable
to call destructor expilictly but puprose of such parameter is not clear. User would be capable to define
only destructor with parameters, then compiler would have no way to clean automatic variables, unless
there is a way to oveload such destructor.  This problem does not have much of use and creates many problems.
One reason, it would break you program 

Array are constructed in order from 0 to size and destructer is the reverse
T arr[10] => arr[0], arr[1] ... arr[9] ... arr[1], arr[0]

It is not allowed to call destructor of automatic variable, since it will be called once more when scope
is unwinded which result to undefined behaviour.
=========================================================================================================
                                  Constructor
=========================================================================================================
Constructor. Special member function called to allocate and initialize memory. It has special rules for
how they should be names:
1. Constructors must have the same name as the class (with the same capitalization)
2. Constructors have no return type (not even void)
Constructor that takes no parameters (or has all parameters assigned default value) is called default
constructor. It is used if no user defined initialization is provided.
class foo{
    foo(){};            // Default
    foo(int x){};       // Non-default
    foo(const foo&){};  // Non-default, Copy constructor
}
foo f1;     // Calls default construcotre
foo();      // same
new foo;    // same
foo(1)      // non-default
f1 = foo()  // non-default, assignment operator

Don`t call pure virtual functions in constructor/desctructor, since it won`t call definition of derived
class. At this moment, since function is pure, it has no definition and program will fail to link.
This cannot be done, since, derived class was not yet initialized.
If function is virtual but not pure and has definition, than code will work, but only base class function
will be called
class A {
  A() { fn(); }
  virtual void fn() { _n = 1; }
  int getn() { return _n; }
  int _n;
};
class B : public A
{
  B() : A() {}
  virtual void fn() { _n = 2; }
};
  B b;
  int n = b.getn(); // 1

If construct fails, user is supposed to handle issue and destructor is not called.

There is a notion of conversion constructor, a constructor that takes one parameter and allows automatic
conversion to the class being constructed. It can be disabled, by making such constructor explicit
class Test {
   int x; 
   Test(int i) {x = i;} 
};
 Test t(20);
 t = 30; // conversion constructor is called here. 

Constructor cannot be vritual. Virtual is mechanism allowing us to call a function knowing only interface
of class without having to know exect type of the object. There are numerous reasons why there can be no
virtual costructs:
1. Constructor requires full information about class for allocation and initialization while virtual
   means that we have only interface. As well, it is not clear how it would work, which dynamic type is
   choosen.

There is an idiom called virtual construct that involves calling virtual interface name clone.
class Base {
    virtual Base* clone() { return (new Base(*this)); }
};
class Derived: public Base{
    virtual Derived* clone() { return (new Derived(*this)); }
};
void foo(Base *b) {
    // Base *tmp = b; // Does not what you would expect, b is sliced
    Base *tmp = b->clone();
}
Derived d;
foo(d):

Ability to overload function when return type differ but one is derived from another is called co-variant
return type.

? There is virtual constructor in other languages, how does it work?

---------------------------------------------------------------------------------------------------------
                      Constructor Member Initializer List.
---------------------------------------------------------------------------------------------------------
Member initializer lists allow us to initialize our members rather than assign values to them.
It is a special syntax used to initialized base classes in inheritance and member variables. One important
use case is initialization of const pointers and references since they are supposed to be intialized with
declaration without further possibility to be changed. The syntax is following:
class foo {
    const int *iPtr;
    int &iRef;
    foo(int *ptr): iPtr(ptr), iRef(ptr) { ... };
};
List is inserted after the constructor parameters, beginning with a colon(:) and then lists each variable
to initialize along witht the value for that variable separated by a comma. Note, member variable and
parameter may have the same name, it is not ambiguos in this context, however you will need to explicitly
use variable through *this in body.
It can be used as alternative mechanism to provide default value
foo(): member1(default_value), member2(default_value) {}
Note, it is different from 
foo(val1 = default_value, val2 = default_value): member1(val1), member2(val2) {}

Variables are initialized not in list order but in order they were declared. You need to be carefull
avoiding variables that depends on each other during initialization. Otherwise you will have quite
unnoticable bug that won`t be explicitly warned by some compilers without all warnings enabled.
---------------------------------------------------------------------------------------------------------
                              Constructor delegation
---------------------------------------------------------------------------------------------------------
Often you find yourself duplicating some part of code to all constructors which is not cool. There is a
solution which lets you call multiple constructors but create only one object, therefore you can leave
some common code in one constructor and some instane specific in another construct. This mechanism is
called constructor delegation. For instance.
class foo {
public:
    foo() {
        // code A
    }
    foo (args) {
        // code A
        // code B
    }
};
Instead, you can write next
class foo {
public:
    foo() {
        // code A
    }
    foo (args): foo() {     // Call default construcor first
        // code B
    }
};
Since this feature is avaialable only starting with C++11, you may mimic such functionality by putting
common login in a separate method and calling it. This reduces code duplication to the minimum.
---------------------------------------------------------------------------------------------------------

https://scc.ustc.edu.cn/zlsc/sugon/intel/ssadiag_docs/pt_reference/references/sc_cpp_pure_virtual_call_in_ctor.htm
=========================================================================================================
                                Copy constructor
=========================================================================================================
A copy constructor is a special type of constructor used to create a new object as a copy of an existing
object. If you have previously initialized a variable and want to reasign it from another instnace,
you will use assignment operator.
class Test {
    Test() {}
    Test(const Test &t)
    {
        cout<<"Copy constructor called "<<endl;
    }
    Test& operator = (const Test &t) 
    {
        cout<<"Assignment operator called "<<endl; 
        return *this; 
    }
}
Test t1, t2;
t2 = t1;            // assignment operator
Test t3 = t1;       // copy constructor
Test t4 (t1);       // copy constructor

Copy consutroctor can classified by how they initialize instance of a class:
1. Memberwise/Shallow copy. Performed by default generated copy constructor, each member of the copy is
   directly initialized from the member of the class being copied, like `this->member = other->member`
   for each member. In C, this is implicitly performed when you assign one instnace of a struct to another.
2. Deep copy. When designing classs that is responsible for alocating/dealocating dynamic memory.
   In this case, member wise copy may give access to resource for instance of another class.
   As result, both of them may try to dealocate/reinitialize the resource, making program behaviour
   undefined.

Copy initialization is peformed in a few more cases, except these described above.
1. Returing by value, but there are exceptions.
Test foo(Test t) {
    return t;
}
Test t1;
foo(t1);             // Calls copy constructor one to pass instance and once to return
Test t2 = foo(t1);   // Copy constructor during return is elided.

---------------------------------------------------------------------------------------------------------
                               Preventing copying
---------------------------------------------------------------------------------------------------------
You may want to have some class that can`t be copy initialized, for instance some global supervisor or
singleton. Another common example it the cout/cin stream which are expected to have one instance for an
entire program. To do this, you need to make copy constructor private or deleted.

---------------------------------------------------------------------------------------------------------
                                  Copy elision
---------------------------------------------------------------------------------------------------------
Consider following case:
class foo{
    foo(){ cout << "Constructor" << std::endl; }
    foo(const foo&) { cout << "Copy constructor" << std::endl; }
};
foo instance(foo());

Firstly, we intialize anonymous object and then copy initialize `instance` from it. Expected output:
Constructor
Copy Constructor

But this is really equals to directly intializing of `instnace` therefore compiler is allowed to skip
constructor and directly intialize object. Or follwing example
foo make(){
    return {};
}
foo instance = make();

Here, make creates returns a temporary value that won`t be changed during the call, therefore it can
be directly used by instance without copying.

Prior to C++17, copy elision is an optimization the compiler can make. As of C++17, some cases of copy
elision (including the example above) have been made mandatory. Under the hood, C++17 performs deferred
temporary materialization. Variable get name and scope at the moment of a assignment and not where
defined/initialized. During return statement we can see if it is assigned to variable of the same type
and maybe move resource ownership to it.

Note, if copy construct is not accessible, even if copy elision is valid in that code, compile will
generate an error, since it is semantically incorrect to use copy constructor in such case.

https://devblogs.microsoft.com/cppblog/guaranteed-copy-elision-does-not-elide-copies/
=========================================================================================================
                              Assignment operator
=========================================================================================================
Operator used to assign value to already allocated instance of class. It has must to be overloaded as a
member function. If you define one out of class and compiler generates implicit one, it is ambiguos which
one should be called.
---------------------------------------------------------------------------------------------------------
                                    Pitfalls
---------------------------------------------------------------------------------------------------------
C++ allows self asignment
Is simple case, when object does not hold any dynamically allocated resources, it does no harm except
wastin processor time. However if instance is holding some pointer, it is a change to shoot yourself in leg.
class foo{
    char *m_data;
    foo& operator=(const foo& r) {
        if(m_data) delete m_data;
        m_data = r.m_data;              // r.m_data is deleted in previous statement;
        retunr *this:

    }
};
foo f1;
f1 = f1;
The solution is to check for self assignment
foo& operator=(const foo& r) {
    if(this == &r) {
        return *this;
    }
    if(m_data) delete m_data;
    m_data = r.m_data;
}

---------------------------------------------------------------------------------------------------------
Partial allocation/dealocation of resources when exception is thown
Following code has a problem
TFoo& TFoo::operator=(const TFoo& that) {
    if (this != &that) {
        TSuperFoo::operator=(that);
        TBar* bar1 = 0;
        try {
            bar1 = new TBar(*that.fBar1);
        }
        catch (...) {
            delete bar1;
            throw;
        }
        delete fBar1;
        fBar1 = bar1;
    }
    return *this;
}
First of all, you may reasign all values of parent class but then fail to allocate member making object
half copied. You may wan`t to put assignment after catch, but what if assignment operator can thow?
The solution is to call parent operator after successfully allocating members.
TFoo& TFoo::operator=(const TFoo& that) {
    if (this != &that) {
        TBar* bar1 = 0;
        try {
            bar1 = new TBar(*that.fBar1);
            TSuperFoo::operator=(that);
        }
        catch (...) {
            delete bar1;
            throw;
        }
        delete fBar1;
        fBar1 = bar1;
    }
    return *this;
}
http://www.icu-project.org/docs/papers/cpp_report/the_assignment_operator_revisited.html
=========================================================================================================
                                 Copy elision
=========================================================================================================
Copy elision is an optimization implemented by most compilers to prevent extra (potentially expensive)
copies in certain situations. It makes returning by value or pass-by-value feasible in practice.
Copy elision can be applied even if copying/moving the object has side-effects, therefore, you shouldn`t
have critical logic inside copy/move constructor or destructors, as you can`t rely on them being called.

Object need to have public constructor, this ensures that copy elision does not allow copying objects
which are not normally copyable, e.g. because they have a private or deleted copy/move constructor.

Copy elision happens whenever an object is initialized by copying another object of the same type, and
the source object is no longer accessible afterwards, e.g. leave current scope. In this case, compiler
treats it as two objects are holding the same place, just skip the copy constructor and replace the place
with the new name.
Under the hood, caller allocated memory for variable and compiler, instead of allocationg new buffer for
function variable, will use your one. Therefore when function is finished, memory allocated by caller, is
initialized without need of copying.

There are two major cases where copy elision would happen: returning a local variable inside a function,
and initializing a variable with a temporary value.

struct C {
  C() {}
  C(const C&) { std::cout << "A copy was made.\n"; }
};

C f() {
  return C(); //Definitely performs copy elision
}
C g() {
    C c;
    return c; //Maybe performs copy elision
}
int main() {
  std::cout << "Hello World!\n";
  C obj = f(); //Copy constructor isn't called
}

The primary limitation of copy elision is that it applies only during initialization of the destination
object (i.e. when it’s first created), and requires the source object to be completely inaccessible after
the copy.
---------------------------------------------------------------------------------------------------------
                                     [N]RVO
---------------------------------------------------------------------------------------------------------
Copy elision has a couple of forms. The most common form is [N]RVO([Named] Return Value Optimization).
It involves eliminating the temporary object created to hold a function’s return value.
If returned value had a name, it is called NRVO, otherwise, RVO or URVO
As long as there are no two distinct names for the same value, it will not invoke the copy constructor.

struct OneStruct {
  vector<string> v;
  OneStruct(vector<string> in) {
    v.swap(in);
  }
};

string GenString() {
  string result("foo");
  return result;            // NRVO
}

string GenString() {
  return "foo";            // RVO
}

vector<string> GenVector(int n) {
  vector<string> result;
  for (int i = 0; i < n; ++i) {

    result.push_back(GenString());
  }
  return result;
}
OneStruct one_struct(GenVector(10));    // No copy constructor called

In the above example, the string can be named as v[0], in[0] or even without name at all (the return
value of GenString()), but it never has more than one name at a time. The value is passed by two features:
copy elision and move semantics.

---------------------------------------------------------------------------------------------------------
                                 [N]RVO vs move
---------------------------------------------------------------------------------------------------------
There are three opertations tried to return variable by a value in order: copy/move elision, move, copy.
Move is performed if elision(RVO) is not applicable. One of requisits of RVO is that return type of function
and returned variable are the same (what about cv?). If you perform move explicitly, argument type will be
considered as different.
RVO's conditions are that:
Object is of the same type as the function return type.
Object is local.

T fn() {
  T t;
  return std::move (t);     // RVO is not applicable, breaks the first rule
}
Some compilers may warn you about preventing copy elision.

https://stackoverflow.com/questions/17473753/c11-return-value-optimization-or-move
https://www.ibm.com/developerworks/community/blogs/5894415f-be62-4bc0-81c5-3956e82276f3/entry/RVO_V_S_std_move?lang=en
https://www.reddit.com/r/cpp_questions/comments/5ro90i/rvo_vs_stdmove/
https://developers.redhat.com/blog/2019/04/12/understanding-when-not-to-stdmove-in-c/
---------------------------------------------------------------------------------------------------------
                             When should i use it?
---------------------------------------------------------------------------------------------------------
Generally speaking, you should prefer simpler, safer, more readable code, and only go for something more
complex if you have concrete evidence that the complex version performs better and that the difference
matters. That principle certainly applies to this technique: passing by const reference is simpler and
safer, so it’s still a good default choice. However, if you’re working in an area that’s known to be
performance-sensitive, or your benchmarks show that you’re spending too much time copying function
parameters, passing by value can be a very useful tool to have in your toolbox.
---------------------------------------------------------------------------------------------------------
https://stackoverflow.com/questions/12953127/what-are-copy-elision-and-return-value-optimization/12953150#12953150
https://zpjiang.me/2018/08/01/Copy-Elision-in-C/
https://stackoverflow.com/questions/48955310/how-does-c-abi-deal-with-rvo-and-nrvo
https://en.cppreference.com/w/cpp/language/copy_elision
=========================================================================================================
                                     *This
=========================================================================================================
There may be some confusion about how compiler knows on which object its member function was called.
Under the hood compiler sends used object as the first argument, therefore it is clear which object
will get modified. This looks something like this.
class foo {
    int m_i;
    bar(int i){                  // User defined
      m_i = i;
    }
  //bar(foo* const this, int i){ // Preprocessed by compiler
  //  this->m_i = i;
  //}

}
foo f;
f.bar(1);
// bar(&f, 1); // Preprocessed by compiler

Usually, you don`t need to use `this` explicitly except a couple of cases.
1. If parameter passed to a function has the same name as class member, use this to to make it clear which
   variable is used.
2. You want to add/remove cv qualifiers of class object.
3. You return handle like `this` to object, therefore it is possible to chain multiple function calls.


=========================================================================================================
      RAII (Resource Acquisition Is Initialization) | SBRM (Scope Based Resource Managment)
=========================================================================================================
It is a programmig technique whereby resource use is tied to the lifetime of objects with automatics
duration. In C++, it is implemeter with Constructor/Destructor. The idea behinde it is preventing of
resource leak, as all resource-holding objects are cleaned up automatically.
Under RAII, objects holding resource should not be dynamically allocated, since such objects are freed
when scope is left and there is no need to worry about a resource eventually getting cleaned up.
For dynamically allocated objects, the user is responsible for deletion -- if user miss to do it, memory
used for object and resource is leaked.

Rule: If your class dynamically allocates memory, use the RAII paradigm, and don’t allocate objects of
your class dynamically.

https://embeddedartistry.com/blog/2017/7/17/migrating-from-c-to-c-take-advantage-of-raiisbrm
=========================================================================================================
                                  Rule of 3/5
=========================================================================================================
C++ has a special member functions, the ones that are generated on its own. These are: default constructor,
the destructor, copy constructor, copy assignment operator, move copy constructor and move assignment
operator. All functions are implicitly inlined, public, and non virtuall except destructor.


If a class defines one (or more) of destructor / copy constuctor / copy assignment operator, it should
probably explicitly define all three/five. If class has no user defined constructors, compiler will
automatically generate a public defalt constructor for you. This is sometimes called an implicit constructor.
class foo{
    // Explicitly defined
    private:
     int member = 100;
    // Implicitly defined
    public:
    foo(){};
}
foo obj;    // Works without explicitly defined constructor
Such constructor is need to creat POD structes or some simple object that doesn`t require logic in
constructor. Note, it woun`t be implicitly defiend if user has provided any constructor by himself.
class foo{
    public: foo(int x){};
}
foo obj1(1); // Ok
foo obj;     // Nok, no appropriate constructor defined
Rule: Provide at least one constructor for your class, even if it’s an empty default constructor.

Member objects are constructed using their defaul constructor before entering constructor body. You
explicitly choose constructor with `constructor member initializer list`.
---------------------------------------------------------------------------------------------------------
                              Function dependecies
---------------------------------------------------------------------------------------------------------
1. Default constructor. Is genetared only if no constructor is declared by user.
2. Copy constructor. Is generated only if move constructor/assignment operator are not defined
3. Copy assignment operator. The same as for copy constructor
4. Destructor
5. Move constructor. Genreted if none of other are declared by user.
6. Move assignment operator. The same as for move constructor.
Empty class has all functions generated.

---------------------------------------------------------------------------------------------------------
                        Deleting functions
---------------------------------------------------------------------------------------------------------
If you don`t want another developer to call some function, you just make it private or don`t define it.
But what should you do when function is generated instead of you?
Before C++98 you would declare such function by yourself, without defining, and make declaration private.
This approach fails when there are friends. Failure will be postponed untill linkage, providing some
cpytic error as always.
Proper way to handle this, is to declare function delted. In such case no one, even friends, can use
declared method. Error is generated at compile time, but there is a convention, deleted function are
public, otherwise some compilers will argue about access specifier instead.

As well, when there is ambiguity for function parameter, and it is expected to be casted by compiler, you
can explicitly ban such cast. Just delete overload for used parameter.
bool foo(int);
bool foo(double) = delete;
foo(1,5);       // compilation error

Another usage is banning some types for templates.
---------------------------------------------------------------------------------------------------------
=========================================================================================================
                                     Friend
=========================================================================================================
                                    Classes
---------------------------------------------------------------------------------------------------------
Occassionally you want to have data private for some part of you system but public for another, how would
you do this ? Example of such situation: you have two separate classes, one for displaying data another
for keeping data and hiding it`s implementation details to simlify its maintenance. However it is fine
for display to know internals of data. Possible solutions: providing separate set of apis that may
profide all needed information to display but this clutter intreface of class and this function will be
accessible for all others wich breaks incapsulation. To solve this issue, C++ has mechanism of friend
classes. Friend class can access private parts of class.

class Storage{
private:
    int m_nValue;
    double m_dValue;
public:
    Storage(int nValue, double dValue) {
        m_nValue = nValue;
        m_dValue = dValue;
    }
    friend class Display;
};
class Display {
private:
    bool m_displayIntFirst;
public:
    Display(bool displayIntFirst) { m_displayIntFirst = displayIntFirst; }
    void displayItem(const Storage &storage)
    {
        if (m_displayIntFirst)
            std::cout << storage.m_nValue << " " << storage.m_dValue << '\n';
        else // display double first
            std::cout << storage.m_dValue << " " << storage.m_nValue << '\n';
    }
};
 
Storage storage(5, 6.7);
Display display(false);
display.displayItem(storage);

---------------------------------------------------------------------------------------------------------
                                   Functions
---------------------------------------------------------------------------------------------------------
The same idea can be applied to functions. You can define some normal function or function member of another
class as friend. Just declare such method in class with keyword `friend` at the beggining. It doesn`t
matter if it is declared in private/public section of class. Note, we need to pass instance of class to
such functions since they are not members of class.
class foo {
private:
    int value;
    friend void bar(foo &obj);
}
void bat(foo &obj){
    obj.value = 10;     // Ok
}

Situation is more complex for friend member functions, since compiler needs to see full function (or class?) definiton,
before allowing it as friend, forward declaring class is not working. This may involve rearanging of
class and some forward declarations, but it can be avoided if classes have separate sets of files.

class Storage;
class Display {
private:
    bool m_displayIntFirst;
public:
    Display(bool displayIntFirst) { m_displayIntFirst = displayIntFirst; }
    void displayItem(const Storage &storage)
    {
        if (m_displayIntFirst)
            std::cout << storage.m_nValue << " " << storage.m_dValue << '\n';
        else // display double first
            std::cout << storage.m_dValue << " " << storage.m_nValue << '\n';
    }
};

class Storage{
private:
    int m_nValue;
    double m_dValue;
public:
    Storage(int nValue, double dValue) {
        m_nValue = nValue;
        m_dValue = dValue;
    }
    friend void Display::displayItem(const Storage &storage);
};
 
Storage storage(5, 6.7);
Display display(false);
display.displayItem(storage);
---------------------------------------------------------------------------------------------------------

Friending is uncommonly used when two or more classes need to work together in an intimate way, or much
more commonly, when defining overloading operators.
=========================================================================================================
                              Operator overloading
=========================================================================================================
In C++, operators are implemented as functions. By using function overloading on the operator functions,
you can define your own versions of the operators that work with different data types (including classes
that you’ve written). Using function overloading to overload operators is called operator overloading.

You commonly use different operators on different objects, often with different types, without noticing
that each such operation requires different implementation. For instance you use addition operator to
add ints, int and float, concatenate string with any special syntax. This primitve operations are
implemented by compiler (maybe, except string unless it is built in), but there are user defined types
and it would make sense that user can define all this operators for his objects as well. This is what
operator overloading used for.
When you do something like `a+b`, it can be thought as operator+(a,b). Compiler looks at used types and
determine which version is supposed to be used. It is determined using following algorithm:
1. If all operands are fundamental types, the compiler calls bult-in routine if one exists.
   If there is no routine, compilation error is returned.
2. If any of operands are user data types, the compiler looks to see whether the type has a matching
   overloaded operator function that it can call. If it can`t find one, it will try to convert one more
   user data types to fundamental data types so it can use matching built-in operator.
   Otherwise, compilation error is returned.

---------------------------------------------------------------------------------------------------------
                                  Limitations
---------------------------------------------------------------------------------------------------------
You cannot overload: ternar(?:), sizeof, scope(::), member selector(.) and member pointer selector (.*).
You can`t create new operators, only overload existing ones.
At least one of operands must be user defined type => you can`t overload operators for fundamental data types.
It is not possible to change number of arguments operator supports.
Operator precedence can`t be changed.

Common mistake: you may won`t to overload ^ to do exponentiation, however, it has lower precedence level
than the basic arithmetic operators, which causes expressions to evaluate incorrectly.

Rule. When overloading operators, it’s best to keep the function of the operators as close to the original
      intent of the operators as possible.
Rule: If the meaning of an operator when applied to a custom class is not clear and intuitive, use a named
      function instead.
---------------------------------------------------------------------------------------------------------
                                 Implementation
---------------------------------------------------------------------------------------------------------
There are three ways to overload operators: member function, friend function, normal function.
To declare operator function you need to define name as `<ret_type> operator<op>(<operands>...)`

---------------------------------------------------------------------------------------------------------
Friend function is more intuitive for most binary operators, for instance:
class customInteger {
    int value;
    customInteger(int val){ value = val };
    friend customInteger operator+(const customInteger &i1, const customInteger &i2);
}
customInteger operator+(const customInteger &i1, const customInteger &i2) {
    return customInteger(i1.value + i2.value);
}
customInteger int1(1);
customInteger int2(2);
customInteger sum = int1 + int2;

Not everything can be overloaded as a friend function. The assignment (=), subscript ([]),
function call (()), and member selection (->) operators must be overloaded as member functions, because
the language requires them to be. The reasoning is that it prevents from some obscure errors, but not
clear which exactly. It won`t be change to preserve consistancy.

Note, friend function can be defiend within class, even though it is not a member. This can be done for
trivial operations but not recommended for something more cumbersome.

https://www.quora.com/C++-Why-do-operators-and-cant-be-overloaded-as-non-member-functions-I-couldnt-find-any-proper-answer
---------------------------------------------------------------------------------------------------------
Normal function. Access to private data, provided by friend keyword, may be non needed. In such cases you
can do the same without declaring friend function within class.
class customInteger {
    int value;
    customInteger(int val){ value = val };
    public: int getValue(){ return value; };
}
customInteger operator+(const customInteger &i1, const customInteger &i2) {
    return customInteger(i1.getValue() + i2.getValue());
}
customInteger int1(1);
customInteger int2(2);
customInteger sum = int1 + int2;

In general, a normal function should be preferred over a friend function if it’s possible to do so with
the existing member functions available (the less functions touching your classes’s internals, the better).
However, don’t add additional access functions just to overload an operator as a normal function instead
of a friend function!

Rule: Prefer overloading operators as normal functions instead of friends if it’s possible to do so without
      adding additional functions.
---------------------------------------------------------------------------------------------------------
Member function. It is simmilar to friend function variant, except left operand becoming implicit *this
therefore you binary operators will take only one arguemt.
class customInteger {
    int value;
    customInteger(int val){ value = val };
    customInteger operator+(const customInteger &i2) {
        return customInteger(value + i2.value);     // value is member of *this
    }
}
customInteger int1(1);
customInteger int2(2);
customInteger sum = int1 + int2;

Not everything can be overloaded as a member function. Operator that require some another class as left
operand require friend function. For instance, IO operators, stream is always left.

---------------------------------------------------------------------------------------------------------
                                Pitfalls
---------------------------------------------------------------------------------------------------------
There is a concept of parameter inheritence, where pointer/reference to base class is taken, you can
pass an instance of derived class. This doesn`t work for virtual functions since arguments and return
type are supposed to be of the same type. Operator is actually inherited but not virtuall.
This has one use case, you can make parent operator vitual and override withing child. When static
type of object is parent and dynamic is child, virtual operator will use child implementation.

Another problem that would arise if virtual functions supported parameter inheritance would be function
oveloading. Let`s suppose that base class has virtual operator and child has overriden version.
If we try to call this operator and pass child as left argument (this) and parent as right argument,
this would call overriden version even though right operator is parent, likely, resulting into
crash/undefined behavioure.

Usually, you don`t need virtuall operator, just call parent version from child is enough.
https://stackoverflow.com/questions/669818/virtual-assignment-operator-c
---------------------------------------------------------------------------------------------------------
Overloading || and &&
https://www.youtube.com/watch?v=hCGadTsT0S0
---------------------------------------------------------------------------------------------------------
                                Definiton lookup
---------------------------------------------------------------------------------------------------------
Operators may be overloaded, one important use case is fixing ordering issue.
customType + int => calls operator+(const customType&, const int&);
int + customType => calls operator+(const int&, const customType&);
As you can see, often you need to define two versions of same operator, otherwise expression like
`int + customType`, wouldn`t find appropriate operator and try converting customType to int, which is
not what you expect.

---------------------------------------------------------------------------------------------------------
                                I/O overloading
---------------------------------------------------------------------------------------------------------
One of the common cases is shift operator used for printing data. This is done by defining shift operator
that takes stream as the first operand and data type as the second.
friend std::ostream& operator<< (std::ostream &out, const UserType &instance) {
    out << "Custom type: "<< instance.member << std::endl;
    return out; // return stream to allow chaining
}
friend std::istream& operator>> (std::istream &in, UserType &instance) {
    in >> instnace.member;
    return in;
}

Note, you need to define separate definition for each class, since it can`t be virtual. Workaround is
defining a separate virtual function and forwarding arguments to it.
class Base {
public: Base() {}
	friend std::ostream& operator<<(std::ostream &out, const Base &b) {
		return b.print(out);
	}
	virtual std::ostream& print(std::ostream& out) const {
		out << "Base";
		return out;
	}
};
class Child: public Base{
public: Child() {}
	virtual std::ostream& print(std::ostream& out) const {
		out << "Child";
		return out;
	}
};
Note, that compiler will perform implicit upcast for derived class, therefore there is no need to define
separate operator for derived class.

---------------------------------------------------------------------------------------------------------
                              Comparsion operators
---------------------------------------------------------------------------------------------------------
Quite often it is not clear how to apply less or greater operator to objects, therfore, it is recommended
to avoid defining them. However, it may have some sense to sort object buy some criteria. Often, this
is done by some algorithm provided by standart library, however, it`s required to have a way of comparing
objects. In this case you can define comparsion operators.

---------------------------------------------------------------------------------------------------------
                          Prefix && Postfix operators
---------------------------------------------------------------------------------------------------------
There are actually two versions of the increment and decrement operators: a prefix increment and decrement
(e.g. ++x; --y;) and a postfix increment and decrement (e.g. x++; y--;). 
Because the increment and decrement operators are both unary operators and they modify their operands, 
they’re best overloaded as member functions.
class customInteger {
    int value;
    customInteger(int val){ value = val };
    customInteger operator++() {   // prefix
        value++;
        return this;
    }
    customInteger operator--() {   // prefix
        value--;
        return this;
    }
    customInteger operator++(int) {   // postfix
        int tmp = value;
        ++(*this);
        return this;
    }
    customInteger operator--(int) {  // postfix
        int tmp = value;
        --(*this);
        return this;
    }
}
Note that we’ve distinguished the prefix from the postfix operators by providing an integer dummy parameter
on the postfix version. Because the dummy parameter is not used in the function implementation, we don`t
have to name it.
---------------------------------------------------------------------------------------------------------
                                Unary operators
---------------------------------------------------------------------------------------------------------
Sometimes you will have operators taking one operand, called unary operators. There are following unary
operators: positive(+), negative(-) and logical not (!). Because they only operate on the object they are
applied to, typically unary operator overloads are implemented as member functions.
class customInteger {
    int value;
    customInteger(int val){ value = val };
    customInteger operator+() {
        return +value;
    }
    customInteger operator-() {
        return -value;
    }
    customInteger operator!() {
        return !value;
    }
}
customInteger i(0);
if (!i) { std::cout << "Null" << std::endl; }
if (-i > 0) { std::cout << "Negative" << std::endl; }
if (+i > 0) { std::cout << "Positive" << std::endl; }

---------------------------------------------------------------------------------------------------------
                               Subscript operator
---------------------------------------------------------------------------------------------------------
When working with arrays, we typically use the subscript operator[] to index specific elements of an array.
There are couple moments to be considered if you want to write it right.
class Array{
    int arr[10];
public:
    int& operator[](int index) {
        assert( index >= 0 && index < 10 )
        return arr[index];
    }
    const int& operator[](int index) const {
        assert( index >= 0 && index < 10 )
        return arr[index]
    }
}
This operator is commonly expect to return array pointer on index position making it possible to assign
value (subscript has higher precedence then assignment).
If you have const object, don`t miss that returned value should be const as well.

This can used for error checking, however, there is convention that subscript doesn`t fail. If invalid
argument is passed, it will terminate the program. If you don`t want to terminate program, define method
int& at(int index), which performs checking and throws if something goes wrong or returns NULL.

Don`t mix pointer to objects and overloaded []. For instance:
Array *arr = new Array();
arr[0] = 1;    // You dereference Array[] instead of calling operator[] on one instance
               // If index used is higher that size of dynamically alocated array, you`r lucky, crash
               // If index in range, you`r unlucky, undefined behaviour
(*arr)[0] = 1; // Ugly but OK

Note, that operator parameter is not supposed to be exactly integer. It is up to you which type is passed.

---------------------------------------------------------------------------------------------------------
                                  Parenthesis
---------------------------------------------------------------------------------------------------------
All of operators have predefined number of parameters except perenthesis operator.
Keep in mind, it needs to be a member function.
class Matrix {
    double& operator()(int row, int col);
};

double& Matrix::operator()(int row, int col)
{
    assert(col >= 0 && col < 4);
    assert(row >= 0 && row < 4);
    return data[row][col];
}
Marix m;
m(0,1) = 1;

It is strongly discouraged to be used since it is rarelly clear what operator is doing without look under
the hood. However, it is commonly used for special type of object called functors (function object) that
operate like a function with state.

class Accumulator {
    int m_counter = 0;
    int operator() (int i) { return (m_counter += i); }
};
Accumulator acc;
std::cout << acc(10) << std::endl; // prints 10
std::cout << acc(20) << std::endl; // prints 30

---------------------------------------------------------------------------------------------------------
                                Which to choose
---------------------------------------------------------------------------------------------------------
When dealing with binary operators that don’t modify the left operand (e.g. operator+), the normal or
friend function version is typically preferred, because it works for all parameter types (even when the
left operand isn’t a class object, or is a class that is not modifiable). The normal or friend function
version has the added benefit of “symmetry”, as all operands become explicit parameters (instead of the
left operand becoming *this and the right operand becoming an explicit parameter).

When dealing with binary operators that do modify the left operand (e.g. operator+=), the member function
version is typically preferred. In these cases, the leftmost operand will always be a class type, and
having the object being modified become the one pointed to by *this is natural. Because the rightmost
operand becomes an explicit parameter, there’s no confusion over who is getting modified and who is getting
evaluated.

Unary operators are usually overloaded as member functions as well, since the member version has no parameters
---------------------------------------------------------------------------------------------------------
                                    Typecast
---------------------------------------------------------------------------------------------------------
User-defined conversions allow us to convert our class into another data type.
Image that you have some simple class and it would make sence to be able converting it to some fundamental
type, for instance you have RGB class and you can convert it as alternative representation of int. Or maybe
convert it to string describing color.
struct RGB{
    int r,g,b;
    operator char*() {...};
    operator int(){...};
    operator HSL() {...};
    operator HexString() {...};
}
void print(char*);
RGB color{0,0,0};
print(color);           // converts to char* "Black"
int int_color = color   // returns all colors packed into int

Things to note:
1. To overload the function that casts our class to an int, we write a new function in our class called
   operator int(). Note that there is a space between the word operator and the type we are casting to.
2. User-defined conversions do not take parameters, as there is no way to pass arguments to them.
3. User-defined conversions do not have a return type. C++ assumes you will be returning the correct type.

=========================================================================================================
                              Object relationships
=========================================================================================================
Programming is also full of recurring patterns, relationships and hierarchies. Particularly when it comes
to programming objects, the same patterns that govern real-life objects are applicable to the programming
objects we create ourselves. By examining these in more detail, we can better understand how to improve
code reusability and write classes that are more extensible.

There are many different kinds of relationships two objects may have in real-life, and we use specific
"relation type" words to describe these relationships. There are following relations: part-of, has-a,
uses-a, depends-on, and member-of.
---------------------------------------------------------------------------------------------------------
                                  Composition
---------------------------------------------------------------------------------------------------------
Object composition. Process of building complex objects from simpler ones. In real-life, complex objects
are often built from smaller, simpler objects. For example, a car is built using a metal frame, an engine
Object composition models a “has-a” relationship between two objects. A car “has-a” transmission. Your
computer “has-a” CPU.

There are two basic subtypes of object composition: composition and aggregation.
To qualify as a composition, an object and a part must have the following relationship:
The part (member) is part of the object (class)
The part (member) can only belong to one object (class) at a time
The part (member) has its existence managed by the object (class)
The part (member) does not know about the existence of the object (class)

While object composition models has-a type relationships (a body has-a heart, a fraction has-a denominator),
we can be more precise and say that composition models “part-of” relationships (a heart is part-of a body,
a numerator is part of a fraction). Composition is often used to model physical relationships, where one
object is physically contained inside another.

Although most compositions directly create their parts when the composition is created and directly destroy
their parts when the composition is destroyed, there are some variations of making it more flexible.
1. A composition may defer creation of some parts until they are needed. For example, a string class may
   not create a dynamic array of characters until the user assigns the string some data to hold.
2. A composition may opt to use a part that has been given to it as input rather than create the part itself.
3. A composition may delegate destruction of its parts to some other object (e.g. to a garbage collection routine).

You may question yourself when do you need to use a subclass instead of directly implementing a feature.
1. Each individual class can be kept relatively simple and straightforward, focused on performing one task
   well. This makes those classes easier to write and much easier to understand, as they are more focused.
2. Each subclass can be self-contained, which makes them reusable. You may want to reuse this class in
   another application. As well, it is quite simple to change implementation within class definition
   instead of changing parts around entire application.
3. The parent class can have the subclasses do most of the hard work, and instead focus on coordinating
   the data flow between the subclasses. This helps lower the overall complexity of the parent object,
   because it can delegate tasks to its children, who already know how to do those tasks.

A good rule of thumb is that each class should be built to accomplish a single task. That task should
either be the storage and manipulation of some kind of data (e.g. Point2D, std::string), OR the
coordination of subclasses (e.g. Creature). Ideally not both.

---------------------------------------------------------------------------------------------------------
                                  Aggregation
---------------------------------------------------------------------------------------------------------
To qualify as an aggregation, a whole object and its parts must have the following relationship:
The part (member) is part of the object (class)
The part (member) can belong to more than one object (class) at a time
The part (member) does not have its existence managed by the object (class)
The part (member) does not know about the existence of the object (class)

Unlike a composition, parts can belong to more than one object at a time, and the whole object is not
responsible for the existence and lifespan of the parts. When an aggregation is created, the aggregation
is not responsible for creating the parts. When an aggregation is destroyed, the aggregation is not
responsible for destroying the parts.
Consider a car and an engine. A car engine is part of the car. And although the engine belongs to the car,
it can belong to other things as well, like the person who owns the car. We can say that aggregation models
“has-a” relationships 

Term destroy is not always applicable to such realtions, instead you should understand it as freeing
instance from object responsibility.

Since, aggregation and composition are so close, their implementatation is close as well. The difference
is more semantic then syntactic. For compostion you would use simple members and maybe pointers, if class
is responsible for dynamic memory. Aggregation means using pointers and references to define relation
between objects.

For a variety of historical and contextual reasons, unlike a composition, the definition of an aggregation
is not precise -- so you may see other reference material define it differently from the way we do. That’s
fine, just be aware.

Note, there are terms that mean not would you expect.
Aggregate data type. Data types (structs/classes) that group multiple variables together.
Aggregate class. Struct/Class that has no provided constructor, destructor, overloaded assignment, has
all public members, and does not use inheritance - POD, plain old data structure.

---------------------------------------------------------------------------------------------------------
                                Association
---------------------------------------------------------------------------------------------------------
To qualify as an association, an object and another object must have the following relationship:
The associated object (member) is otherwise unrelated to the object (class)
The associated object (member) can belong to more than one object (class) at a time
The associated object (member) does not have its existence managed by the object (class)
The associated object (member) may or may not know about the existence of the object (class)

Unlike a composition or aggregation, where the part is a part of the whole object, in an association, the
associated object is otherwise unrelated to the object. Just like an aggregation, the associated object
can belong to multiple objects simultaneously, and isn’t managed by those objects. However, unlike an
aggregation, where the relationship is always unidirectional, in an association, the relationship may be
unidirectional or bidirectional (where the two objects are aware of each other).

For instnace, doctor and patiente are associated but not parts of each other. Doctor may have multiple
patience, one after another, as well as patient may go different doctors.
We can say that association models as “uses-a” relationship. The doctor “uses” the patient (to earn income).
The patient uses the doctor (for whatever health purposes they need).

Associtation may be used in multile different ways but the most common it the pointer.

Reflexive association. Relationship with other objects of the same type. A good example of a reflexive
association is the relationship between a university course and its prerequisites (which are also
university courses).
---------------------------------------------------------------------------------------------------------
Property                                | Composition    | Aggregation    | Association
Relationship type                       | Whole/part     | Whole/part     | Otherwise unrelated
Members can belong to multiple classes  | No             | Yes            | Yes
Members existence managed by class      | Yes            | No             | No
Directionality                          | Unidirectional | Unidirectional | Unidirectional or bidirectional
Relationship verb                       | Part-of        | Has-a          | Uses-a

=========================================================================================================
                                  Inheritance
=========================================================================================================
Object composition is perfect for building new objects that have a “has-a” relationship with their parts.
Another way is through inheritance, which models an “is-a” relationship between two objects.
Unlike object composition, which involves creating new objects by combining and connecting other objects,
inheritance involves creating new objects by directly acquiring the attributes and behaviors of other
objects and then extending or specializing them. The idea, as well as for other relations, was taken from
like, for intance you inherete genes from your parents.

---------------------------------------------------------------------------------------------------------
                               Construction order
---------------------------------------------------------------------------------------------------------
Instnace of class is devided into parent parts and child. A parent is the first to be initialized and
last to be freed, this is a side effect of memory layout. Be carefull with member intialization lists
since they intialize member in order they were declared.
Child is responsible for calling wanted parent constructor, otherwise default one is used. This can be
done with constructor forwarding. It is important, since there are reference or const variables that can
only be intialized form constuctor member initialization list. After that, Child left to initalize it`s
own fields. 
---------------------------------------------------------------------------------------------------------
                                Access specifiers
---------------------------------------------------------------------------------------------------------
Private. Child class can`t access private data of parent directly.
Protected. Members are not accessible for outside of the class but accessible for childs and friend classes.
This should be used with care, since every time you change protected member, you have to change it`s usage
withing childs and friends.

---------------------------------------------------------------------------------------------------------
                                    Types
---------------------------------------------------------------------------------------------------------
There are three different ways for classes to inherit from other classes: public, private, and protected.
Default is private for class and public for struct.
When members are inherited, the access specifier for an inherited member may be changed (in the derived
class only) depending on the type of inheritance used. Put another way, members that were public or
protected in the base class may change access specifiers in the derived class.

Keep in mind the following rules:
1. A class can always access its own (non-inherited) members.
2. The public accesses the members of a class based on the access specifiers of the class it is accessing.
3. A class accesses inherited members based on the access specifier inherited from the parent class.
   This varies depending on the access specifier and type of inheritance used.

Public inheritance works as decribed above.

Private inheritance makes all members of parent to be private.
Note that this does not affect the way that the derived class accesses members inherited from its parent!
It only affects the code trying to access those members through the derived class.
class Base {
public: int m_public;
private: int m_private;
protected: int m_protected;
};
 
class Pri: private Base {
public:
    Pri()
    {
        m_public = 1; // okay: m_public is now private in Pri
        m_private = 2; // not okay: derived classes can't access private members in the base class
        m_protected = 3; // okay: m_protected is now private in Pri
    }
};
    Base base;
    base.m_public = 1; // okay: m_public is public in Base
    base.m_private = 2; // not okay: m_private is private in Base
    base.m_protected = 3; // not okay: m_protected is protected in Base
 
    Pri pri;
    pri.m_public = 1; // not okay: m_public is now private in Pri
    pri.m_private = 2; // not okay: m_private is inaccessible in Pri
    pri.m_protected = 3; // not okay: m_protected is now private in Pri

Private inheritance can be useful when the derived class has no obvious relationship to the base class,
but uses the base class for implementation internally. In such a case, we probably don’t want the public
interface of the base class to be exposed through objects of the derived class (as it would be if we
inherited publicly). In practice, private inheritance is rarely used.

Protected inheritance is the last method of inheritance. It is almost never used, except in very particular
cases. With protected inheritance, the public and protected members become protected, and private members
stay inaccessible.

---------------------------------------------------------------------------------------------------------
                                   Destructor
---------------------------------------------------------------------------------------------------------
When a derived class is destroyed, each destructor is called in the reverse order of construction.

---------------------------------------------------------------------------------------------------------
                              Functionality hiding
---------------------------------------------------------------------------------------------------------
There is no way to remove or restrict functionality from a base class without modifing code. However, in
a derived class, it is possible to hide functionality that exists in the vase class so it can not be
accesswd through the derived class. This can be done by changing the relevant access specifier. Another
soultion is to inherit privately or mark publicly inherited functionality as deleted.

---------------------------------------------------------------------------------------------------------
                           Changing access specifiers
---------------------------------------------------------------------------------------------------------
C++ gives us the ability to change an inherited member’s access specifier in the derived class. This is
done by using a using declaration to identify the (scoped) base class member that is having its access
changed in the derived class, under the new access specifier.

class Base {
private: int m_value;
public: Base(int value) : m_value(value) {}
protected: void printValue() { std::cout << m_value; }
};

class Derived: public Base {
public: Derived(int value) : Base(value) { }
    // Base::printValue was inherited as protected, so the public has no access
    // But we're changing it to public via a using declaration
    using Base::printValue; // note: no parenthesis here
};
    Derived derived(7);
    derived.printValue(); // prints 7

Note, derived class can`t change access specifier of members it has not access to (private members).
Syntax used above is C++11 variant and there is variant without `using` used in some older codebase.

---------------------------------------------------------------------------------------------------------
                              Mutiple Inheritance
---------------------------------------------------------------------------------------------------------
Multiple inheritance enables a derived class to inherit members from more than one parent. This may seem
to be quite use full for free but really it is a source for much of ambiguity. For instance
class USBDevice {
private: long m_id;
public: USBDevice(long id) : m_id(id) { }
        long getID() { return m_id; }
};
class NetworkDevice {
private: long m_id;
public: NetworkDevice(long id) : m_id(id) { }
        long getID() { return m_id; }
};
class WirelessAdapter: public USBDevice, public NetworkDevice {
public: WirelessAdapter(long usbId, long networkId) : USBDevice(usbId), NetworkDevice(networkId) {}
};
    WirelessAdapter c54G(5442, 181742);
    std::cout << c54G.getID(); // Which getID() do we call?

Multiple classes may provide function with the same name and declaration making it ambiguos which one to
call. Definition can be explicitly choosen with scope operator but it`s primary purpose is different and
you should avoid it in such context.

most of the problems that can be solved using multiple inheritance can be solved using single inheritance
as well. Many object-oriented languages (eg. Smalltalk, PHP) do not even support multiple inheritance.
Many relatively modern languages such as Java and C# restrict classes to single inheritance of normal
classes, but allow multiple inheritance of interface classes (which we will talk about later). The driving
idea behind disallowing multiple inheritance in these languages is that it simply makes the language too
complex, and ultimately causes more problems than it fixes.

It should be used judiciously. Example of usage is stream library.
---------------------------------------------------------------------------------------------------------
                                     Final
---------------------------------------------------------------------------------------------------------
If you want to prevet class from being inherited, use final keyword.
struct Base1 final {};
struct Derived1 : Base1 {}; // ill-formed because the class Base1 has been marked final
---------------------------------------------------------------------------------------------------------
                              Virtual Inheritance
---------------------------------------------------------------------------------------------------------
There is a famouse diamond problem, when you have a common base amoung parent classes. In this situation
it is not clear whether you want to access Base part through one parent class or another.
class A { };
class B:public A { };
class C:public A { };
class D:public B, public C { };

You can ask compiler to avoid duplication of such base using virtual inheritance. But another problem
arisses, who is responsible for initialization of such base class. The answeel is the the most derived
class has the most information and he is responsible for inializing both parents and base. If parents
call base constructor during initialization, it will be ignored in this case.
All classes inheriting a virtual base class will have a virtual table, even if they would normally not
have one otherwise, and thus be larger by a pointer.
class B : virtual public A { };
class C : virtual public A { };
class D : public B, public C {
    D():B(),C(),D(){
        ...
    }
};

int main()
{
    D obj;
    obj.show();
}

=========================================================================================================
                                    Binding
=========================================================================================================
Binding generally refers to a mapping of one thing to another. In the context of compiled languages,
binding is the link between a function call and the function definition. When a function is called in C++,
the program control binds to the memory address where that function is defined.
There are two type of binding: dynamic and static.
Static means that requested function call is determined at compile time and funcion addresses are linked
at compile time. It is performed when there is all neede information at time of compilation. Usual
function, overloaded operators/functions are statically binded.
Dynamic means that needed function is determined at runtime, e.g. through virtual methods. This makes
code more flexible but decreases execution speed, since time is spent for dispatch table allocation and
method lookup?

=========================================================================================================
                               Virtuall Functions
=========================================================================================================
For every class that contains virtual functions, the compiler constructs a virtual table, a.k.a vtable.
The vtable contains an entry for each virtual function accessible by the class and stores a pointer to its
definition. Only the most specific function definition callable by the class is stored in the vtable.
Entries in the vtable can point to either functions declared in the class itself, or virtual functions
inherited from a base class. That vtables exist at the class level, meaning there exists a single vtable
per class, and is shared by all instances. Each instance has a vpointer for it`s vtable so it can to
dispatch method. When a call to a virtual function on an object is performed, the vpointer of the object
is used to find the corresponding vtable of the class. Next, the function name is used as index to the
vtable to find the correct (most specific) routine to be executed. Done!

---------------------------------------------------------------------------------------------------------
                                Vtable and Vptr
---------------------------------------------------------------------------------------------------------
Vtable is a dispatch table that contains addresses to all last overriden definitions of class. It is
instantiated once per class, not object. Each object has a pointer to it. When you call a virtual function,
pointer dereferences a table, fetches address of the function and it gets executed.
Vtable is probably created at compiletime, since all needed information is present.
Vpointer is probably initialized after construction of base subobjects and before construction of members.

https://stackoverflow.com/questions/3849498/when-is-vtable-in-c-created
---------------------------------------------------------------------------------------------------------
                                   Destructor
---------------------------------------------------------------------------------------------------------
Usually you want to declare destructor virtual as well, it should clean all derived dynamic types and then
the base, however, if have destructor method overrided with the definition of dynamic type, how do we
call destructor for everything. This is probably works as following, compiler inject code before and after
user defined destructor. Code before is responsible for pointing vptr to corrent table. Code after, cleans
members objects, by calling theirs destructors, and calls next parent class. It`s enough to clean all
members at once and then we are left to clean all parents one by one. This may work somewhat different for
case with virtual inheritance

If we so ofter want to have virtual destructor, why shouldn`t we make it virtual by defaul and use some
identifier to make it no virtual when not needed. Well, default destructor would require generating
vtable/pointer for even POD classes which would be high overhead for nothing. As well this would break
already existing code. Another reason, it quite common to have POD or base classes that has no usage
of virtual destructor.

Calling non virtuall destructor when you have derived dynamic type is undefined behaviour.

https://softwareengineering.stackexchange.com/questions/284561/when-not-to-use-virtual-destructors
https://stackoverflow.com/questions/7750280/how-does-virtual-destructor-work-in-c
---------------------------------------------------------------------------------------------------------
                               Override specifier
---------------------------------------------------------------------------------------------------------
If you want to be sure that class is overriding parents function, use override identifier
class base {
    virtual int foo(float x) = 0; 
};
class derived: public base {
     int foo(float x) override { ... do stuff with x and such ... }
}
class derived2: public base {
     int foo(int x) override { ... }    // Error
};

If you want to prevent function overriding, use final identifier
struct Base2 { virtual void f() final; };
struct Derived2 : Base2 {
    void f(); // ill-formed because the virtual function Base2::f has been marked final
};
As well, it let`s compiler perform some optimizations, like function inlining.


Note that neither override nor final are language keywords. They are technically identifiers; they only
gain special meaning when used in those specific contexts. In any other location, they can be valid
identifiers.
int const final = 0;     // ok
int const override = 1;  // ok

---------------------------------------------------------------------------------------------------------
                                     Final
---------------------------------------------------------------------------------------------------------
There may be cases where you don’t want someone to be able to override a virtual function, or inherit from
a class. The final specifier can be used to tell the compiler to enforce this. User trying to override it
will receive a compilation error;
class A {
public: virtual const char* getName() { return "A"; }
};
class B : public A {
public:
	// note use of final specifier on following line -- that makes this function no longer overridable
	virtual const char* getName() override final { return "B"; } // okay, overrides A::getName()
};
class C : public B {
public: virtual const char* getName() override { return "C"; } // compile error: overrides B::getName(), which is final
};

---------------------------------------------------------------------------------------------------------
                Parameter inheritance and covariant return type
---------------------------------------------------------------------------------------------------------
Virtuall functions support covariant return type, meaning that parent and child has the same virtual function
but parent return ptr/ref to it instance if parent class and child does the same but for instnace of child
class. Even though it is require for functions to have same declaration, this two will be considered the
same.
However, this doesn`t work with parameter iheritance. Parameter inheritance means that you can pass ptr/ref
to child where parent is require, this works for simple function but not for virtual. That is the reason
why operator are inherited but not overriding child ones, they are considered to be different functions.

---------------------------------------------------------------------------------------------------------
                             Pure virtual functions
---------------------------------------------------------------------------------------------------------
There is an option to make function virtual without defining its body and force child to define it.
Just assign 0 to virtual function (doesn`t work with non virtual ones). Such function is called pure
virtual function and class an abstract base class, which means it can not be instantiated.

A pure virtual function is useful when we have a function that we want to put in the base class, but only
the derived classes know what it should return. A pure virtual function makes it so the base class can not
be instantiated, and the derived classes are forced to define these functions before they can be
instantiated.  This helps ensure the derived classes do not forget to redefine functions that the base
class was expecting them to.

Pure virtual function may have body defined, providing default implementation for child classes. Note,
it requires to be explicitly called since child should define function by itself. Such definition must
be provided separetly and not inline. 

---------------------------------------------------------------------------------------------------------
                                Interface class
---------------------------------------------------------------------------------------------------------
An interface class is a class that has no member variables, and where all of the functions are pure virtual!
In other words, the class is purely a definition, and has no actual implementation. Interfaces are useful
when you want to define the functionality that derived classes must implement, but leave the details of
how the derived class implements that functionality entirely up to the derived class.
Interface classes are often named beginning with an I. Here’s a sample interface class:
class IErrorLog {
public:
    virtual bool openLog(const char *filename) = 0;
    virtual bool closeLog() = 0;
    virtual bool writeError(const char *errorMessage) = 0;
    virtual ~IErrorLog() {} // make a virtual destructor in case we delete an IErrorLog pointer, so the proper derived destructor is called
};
Note, parameter inheritance works with interfaces therefore you may use ptr/ref for instnace of interface.
---------------------------------------------------------------------------------------------------------
                                Pitfalls
---------------------------------------------------------------------------------------------------------
Defaul values are not inherited. Which default is used -- ie, the base class' or a derived class' -- is
determined by the static type used to make the call to the function. If you call through a base class
object, pointer or reference, the default denoted in the base class is used. Conversely, if you call
through a derived class object, pointer or reference the defaults denoted in the derived class are used.

struct A {
  virtual void f(int a = 7);
};
struct B : public A {
  void f(int a);
};
  B* pb = new B;
  A* pa = pb;
  pa->f(); //OK, calls pa->B::f(7)
  pb->f(); //error: wrong number of arguments for B::f()

https://stackoverflow.com/questions/3533589/can-virtual-functions-have-default-parameters
---------------------------------------------------------------------------------------------------------
https://stackoverflow.com/questions/8824587/what-is-the-purpose-of-the-final-keyword-in-c11-for-functions
=========================================================================================================
                             New && Delete
=========================================================================================================
To allocate a signle variable dynamicallym we use the scalar (non-array) form of the new operator, you
can initialize it the same time:
int *ptr = new int;
int *ptr = new int (1);
int *ptr = new int {1};
delete ptr;

Pointer that points to deallocated memory is called a dangling pointer. Dereferencing or deleting it leads
to undefined behaviour. But deleting nullpts is has no sideefects, there it is preferable to null pointer
after cleaning it for the first time.
int *ptr = new int;
delete ptr;
std::cout << "Value of dungling pointer " << ptr

If new fails to allocate memory, it throws bad_alloc which is quite cumbersome to handle. You can make
new return `nullptr` instead using `new (std::nothrow) type`.

It is posible to allocate/dealocate arrays at runtime of variable lenght by using brackets.
int *ptr = new type[lenght];
int *ptr = new type[lenght]();              // 0 initialized
int *ptr = new type[lenght]{1, 2, 3, 4};
char *ptr = new type[lenght]{"String"};

delete[] ptr;

Note, calling delete[] after new or delete after new[] is undefined behaviour.
Exception. typedef T type[N]; T * pT = new type; delete[] pT;

---------------------------------------------------------------------------------------------------------
                                   Reasoning
---------------------------------------------------------------------------------------------------------
One the reasons to have a `new` operator is, that we need to cast void* returned from malloc resulting to
int *p = static_cast<int*>(malloc(sizeof(*p)));
which is quite unsage and ugly, therefore it can more cleanly written as
int *p = new int;

---------------------------------------------------------------------------------------------------------
                                 Placement new
---------------------------------j------------------------------------------------------------------------
Operator new performs both allocation and intialization but what if you need only to allocate memory and
use it later. You can use malloc or preferable way is to use special variant of operator new, called
placement new. For instance, you have an dynamically allocated array of data that is oftern freed and
then allocated againt with different content. It would be much fuster to initialize/deinitialize memory
without allocating it each time. Another use case, you have a piece of code that shouldn`t fail,
therefore, all buffers are allocated before.
char *buf  = new char[sizeof(string)];        // pre-allocated buffer
string *p = new (buf+offset) string("hi");    // placement new
string *q = new string("hi");                 // ordinary heap allocation
p->~String();                                 // deinitializing the memory

Note, this is the one moment when you can explicitly call destructor on instance.



https://stackoverflow.com/questions/222557/what-uses-are-there-for-placement-new
---------------------------------------------------------------------------------------------------------
                                      This
---------------------------------------------------------------------------------------------------------
It is legal to delete `this` but it becomes dangling after that therefore you shouldn`t dereference it
after. This include implicit dereferencing unsing member variables. It usually found in reference-conted
classes that, when the ref-count is decremented to 0, calls release function and deletes this.

It is considered to be bad idea, since it is easy to accidentally use member variables after delete or
caller function may not expect that object was deleted. As well, it means that your code might not have a
symemetric strategy for object ownership (who allocates and who deletes).

As well, you can`t use `delete this` in a destructor, since it result to infinit loop.
Don`t forget, object should be allocated with new before deleting.

https://stackoverflow.com/questions/550189/is-it-safe-to-delete-this
=========================================================================================================
                            Variable Initialization
=========================================================================================================
There are 3 types of initialization supported by C++: direct, uniform and copy initialization.
The most simple and cumbersome is copy initialization.
struct foo {
   int i;
   double f;
}
stuct foo obj;
obj.i = ...;
obj.f = ...;

C++ introduced more easy to use initialization syntax using {}. Initially, it came from C where you could
use it to initialize members directly, there it was called aggregate initialization. It still applicable
for C++ classes/structs without constructor. If constructor is present, parameter will be used as arguments
for that constructor. Then, C++11 introduced initalization_list, which is the type deduced for {}.
If there is a constructor taking initalization_list, it will be prefered over others for such initialization.
So many use cases for this syntax gave it a name, uniform initialization.

struct foo obj = { 1, 1.0 };
struct foo obj { 1, 1.0 };          // Or using another form called uniform initialization
class foo {
    int x;
    string y;
};
foo f = { 1, "1" };                 // no constructor defined therefore aggregate initialization
vector<int> v = {1, 2, 3, 4};       // initializer_list

There may be some profit of using uniform intialization over direct
1. Minimizes redundant typenames
vec3 GetValue()             |     vec3 GetValue()
{                           |     {
  return vec3(x, y, z);     |       return {x, y, z};
}                           |     }
2, Parssing issues, unifrom initialization is never interpreted as function prototype.
class Bar;
void Func() {
  int foo(Bar());  // foo is a function, taking as parameter function, returing a Bar
  int foo{Bar{}};  // foo is a variable
}

Arguments against
1. It is not clear what type will be constructed without looking for declration
DoSomething("A string.");
DoSomething({strValue, strLen});
2. No narrowing support, otherwise there would many ways to cast argument and find fitting function
int val{1.1};       // doesn`t compile
vector<int> v{100}; // vector with the one element 100


https://softwareengineering.stackexchange.com/questions/133688/is-c11-uniform-initialization-a-replacement-for-the-old-style-syntax
---------------------------------------------------------------------------------------------------------
                Static variables
---------------------------------------------------------------------------------------------------------
Starting with C++11, it`s possible to give non-static members a default value, but it is incompatible with
the initializer list and uniform initialization syntax. In C++14 this restriction was removed.
struct foo {
   int i = 1;
   double f = 1.1;
}
struct foo obj1; // Ok
obj1.i = 2;      // Ok
struct foo obj2 { 1, 1.0 } // C++11 Nok, won`t compile
                           // C++14 Ok
---------------------------------------------------------------------------------------------------------

As well, this syntax can be used to initialized value after declaring/defining (C++11 only?)
struct foo obj2;
obj2 = { 1, 1.0 }

Direct initialization is less preferable over uniform initialization.
Rule: Use uniform initialization to initialize class objects
int x(5);
struct foo obj (1, 1.0);    // will compile?
class foo obj (1, 1.0);



---------------------------------------------------------------------------------------------------------
                             Designated initializer
---------------------------------------------------------------------------------------------------------
It is a quick way to initialize specific elements/ranges of an array, or struct/union members
int foo[10] = { [3] = 1, [5] = 2 };
int a[] = {[0 ... 9] = 1, [10 ... 99] = 2, [100] = 3 };
int whitespace[256] = { [' '] = 1, ['\t'] = 1, ['\h'] = 1, ['\f'] = 1, ['\n'] = 1, ['\r'] = 1 };
struct Foo { int a, b; };
struct Foo foo { .a = 1, .b = 2 }
struct Foo foo { a: 1, b: 2 }
struct Foo foo { 1, 2 }
struct point ptarray[10] = { [2].y = yv2, [2].x = xv2, [0].x = xv0 };
All explistly non initialized members are zero initialized.
Note, that array gets length of the highest index, therefore in previous example it will have 6 items.

https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/designators.htm
https://gcc.gnu.org/onlinedocs/gcc/Designated-Inits.html
https://stackoverflow.com/questions/47202557/what-is-a-designated-initializer-in-c
---------------------------------------------------------------------------------------------------------
https://mbevin.wordpress.com/2012/11/16/uniform-initialization/
=========================================================================================================
                                Value categories
=========================================================================================================
Every expression in C++ has two properties: a type (which is used for type checking), and a value category
(which is used for certain kinds of syntax checking, such as whether the result of the expression can be
assigned to). Eexpression has some non-reference type, and each expression belongs to exactly one of the
three primary value categories: prvalue, xvalue, and lvalue.
---------------------------------------------------------------------------------------------------------
Glvalue (generalized lvalue). An expression whose evaluation determines the identity of an object,
bit-field, or function; lvalue or an xvalue. lvalues are defined as all glvalues which are not xvalues.
If expression returns reference (T&), or refers to the variable by it’s name, or rvalue reference (T&&)
to the object it’s a glvalue expression.

All expressions, that are glvalues actually yield a handle to some variable with some value. Thus it’s
not possible to have the expression of glvalue type, that would have (const volatile) void type (it’s not
possible to have object of type void). Glvalues are references to the objects.

---------------------------------------------------------------------------------------------------------
Prvalue (pure rvalue). An expression whose evaluation either
-  Computes the value of the operand of an operator (no result object)
-  Initializes an object or a bit-field.
Class and array prvalues have a result object even if it is discarded.
Temporary materialization occurs to create a temporary as the result object;
int x = class_constructor().member;
An rvalue that is not an xvalue.

The result of the prvalue expression is a value, that can be stored in the variable. At the same time,
it’s possible to have the prvalue of type (const volatile) void. Prvalue is different from lvalue and
xvalue with the fact that it does not refer to any kind of object, but it is just a value. Meaning in
short, that if expression yields type, which is a value (T) not a reference (T& or T&&) and expression
is not a name of a variable, then it’s a prvalue expression.

struct Foo {};
Foo(); // prvalue
Foo test(){/*implementation*/};
test(); // prvalue


---------------------------------------------------------------------------------------------------------
Xvalue (eXpiring value). Refers to an object, usually near the end of its lifetime (so it can be moved)
It is an result of certain kinds of expressions involving rvalue references (returned rvalue is an xvalue).

Xvalues are glvalues, so if expression yields a xvalue reference, then this reference references some
concrete instance of the object. The xvalues have one important attribute: an xvalue is a glvalue that
denotes an object or bit-field whose resources can be reused (usually because it is near the end of its
lifetime). In other words, xvalue used to be an lvalue but was marked that its resources can be reused or
in other words, the object should be moved instead of copied. 

---------------------------------------------------------------------------------------------------------
L-value references to const objects can be initialized with l-values and r-values alike. However, those
values can’t be modified. L-value references to const objects are particularly useful because they allow
us to pass any type of argument (l-value or r-value) into a function without making a copy of the argument.

R-value references have two properties that are useful. First, r-value references extend the lifespan of
the object they are initialized with to the lifespan of the r-value reference (l-value references to const
objects can do this too). Second, non-const r-value references allow you to modify the r-value!
Test &&t1 = Test(1,2,3...);

As an anonymous object, Test() would be copied into object and dissapear, copying may be skipped. In C++11
this type of intialization is mandatory to skip copying and instead performs direct intialization. This
can be considered as extending lifetime of anonymous object. However it is not ofter used for simple
variables but commonly for function arguments.

Functions can be overloaded based on whether argument is rvalue/lvalue parameter
void foo(const Test &lvalue){}
void foo(Test &&foo){}
Test t1;
Test &&t2 = Test();
foo(t1);     // lvalue
foo(t2);     // lvalue
foo(Test()); // rvalue
Note, && for named variables collapses into single &, therefore lvalue variant is called. The confusion
stems from the use of the term r-value in two different contexts. Think of it this way: Named-objects are
l-values. Anonymous objects are r-values. The type of the named object or anonymous object is independent
from whether it’s an l-value or r-value. Or, put another way, if r-value reference had been called anything
else, this confusion wouldn’t exist.

You shouldn`t return rvalue reference, for the same reason you should not return lvalue reference. In most
cases, you`ll end up returning a hanging reference when the referenced object goes out of scope at the
end of the function.


Expressions that refer to memory locations are called "l-value" expressions. An l-value represents a storage
region's "locator" value, or a "left" value, implying that it can appear on the left of the equal sign (=).
L-values are often identifiers.
R-value is an expression that provides some data that may have no memory address, e.g. in place constructed
variable or result of arithmetic expression. You can assign lvalue from rvalud but not otherside, therefore
everything that is on the right side of assignment operator can be considered an rvalue. All lvalue can
be an rvalue but not otherside. Usually it`s value is usable during one operation e.g. `int x = 1+2`,
result of `1+2` is present and accessible during assignment, but you may extend it lifespan for current
scope with rvalue reference.

---------------------------------------------------------------------------------------------------------
                                   Reasoning
---------------------------------------------------------------------------------------------------------
Every expression in language is requires to have value category, making it clear which operations we
can perform on it and don`t. This categoryes describe some charectistics of expression, like it`s life
time, if we can assign value to it or can use it for assignment.
Before C++11, 2 categories l/rvalue were enough to describe semantics. Then C++11 introdeced move semantics.
This is a way to write code that can avoid copying therefore reducing overhead. To support this, language
introduced new categorie xvalue that is going to be freed unless some one is taking responsibily for it.
Xvalue is somewhere between rvalue and lvalue, therfore, their combinations were generalized into two
separate categories l+x into gl and x+pr into r


TODO 
---------------------------------------------------------------------------------------------------------


You have option whether you want to oveload function for both l/rvalue cases and then maintain them, or
define and maintain one universal reference function.

https://stackoverflow.com/questions/3601602/what-are-rvalues-lvalues-xvalues-glvalues-and-prvalues
=========================================================================================================
                                Move semantics
=========================================================================================================
C++11 introduces 2 new special functions to class: move constructor and move assignment operator. These
are used to move ownership from one instance to another while intializing it. They work preatty much the
same way, but take nonconst rvalue reference instead of const reference. Instead of deep copying, we
move(steal) the source object`s resources. This involves shallowing copying ,then setting source pointer
to null.
class Test {
    public:
    int *data;
    Test(int *ptr) {
        data = new int;
        *data = *ptr;
    }
    ~Test() { delete ptr; }
    Test(const Test&) { .. deep copy, data is cleaned, allocated and initialized .. };
    Test& operator=(const Test& t) {  .. deep copy, data is cleaned, allocated and initialized .. };
    Test(Test&& t) {
        std::cout << "Move constructed" << std::endl;
        delete data;
        data = t.data;
        t.data = nullptr;
    }
    Test& operator=(Test&& t){
        std::cout << "Move assigned" << std::endl;
        if(this == &t)
            return *this;

        delete data;
        data = t.data;
        t.data = nullptr;
    }
    Test() {
        std::cout << "Test allocated" << std::endl;
    }
    ~Test() {
        std::cout << "Test deallocated" << std::endl;
    }
};

Test generateResource() {
   Test t(new int);
   return t;
}
int main(){
    Test t;
    t = generateResource();
}

Expected result with copying:
1. Test is allocated
2. If there is no copy elision, it is copied to return temporary value      // Deep copy constructor
3. Local variable is cleened withing function
4. Temporary value is copied to real variable                               // Deep copy constructor
5. Test is dealocated

Expected result with moving:
1. Test is allocated
2. Copying is elisioned
3. Local variable is freed
4. Resource is moved into value                                             // Shallow copy
5. Test is dealocated

The flow of the program is exactly the same as before. However, instead of calling the copy constructor
and copy assignment operators, this program calls the move constructor and move assignment operators.

The move constructor and move assignment are called when those functions have been defined, and the
argument for construction or assignment is an r-value. Most typically, this r-value will be a literal or
temporary value.

Note, default move constructor and move assignment do the same thing as the default copy constructor and
copy assignment (make copies, not do moves).

Note, move is not applicable for const objects, since, we can`t take ownership proparly from const object.
---------------------------------------------------------------------------------------------------------
                                  Logic behind
---------------------------------------------------------------------------------------------------------
If we construct an object or do an assignmemt where the argument is an l-value, we can`t change an argument
since you woudn`t expect expression `a=b` have a side effect on b. Therefore we copy initialize.

However if construct from an r-value or an temporary object, which is a temporary object and is valid in
the scope of expression, you can steal resource without having side effects. This is much cheaper then
doing deep copy.

However, this should be carefully and move should leave object in a well-defined state. Object passed for
move may be a lvalue, and it won`t end it`s life after expression.
---------------------------------------------------------------------------------------------------------
                                 Implicit move
---------------------------------------------------------------------------------------------------------
The C++ standard specifies situations where an implicit move operation is safe and the compiler can use it
in cases were the (Named) Return Value Optimization) can't be used. There are three things that may happen
when returning by value, in following order: Elision (no move/copy), Move, Copy.

T demo(T arg) {
  T local;
  return T();              // guaranteed elision as of C++17,
                           // commonplace optimization since 1990s ("URVO")
  return local;            // commonplace optimization since 1990s ("NRVO")
                           // if disabled (-fno-elide-constructors), it's a move
  return std::move(local); // elision prohibited (!), it's a move
  return arg;              // elision prohibited; it's a move

}

The specification has a special rule that says automatic objects returned from a function by value can be
moved even if they are l-values. Note, that entire copy/move can be elided.
https://www.quora.com/Does-C-always-move-a-temporary-local-object-in-a-return-statement-or-does-it-copy-it-What-are-the-rules-for-the-general-implicit-move-without-using-std-move
---------------------------------------------------------------------------------------------------------
                                 Explicit move()
---------------------------------------------------------------------------------------------------------
Sometimes, you want to pass owneship from one lvalue to another but it doesn`t fit move argument tipe.
Additional reference can be appended using static_cast, but it is not really readable, istead, you want
to use move()  
string str="123";
vector<string> v;
v.push_back(str);       // calls copy constructor
cout << str << endl;    // str == 123
v.push_back(move(str)); // v has 2 string elements
cout << str << endl;    // str ==

This can be usefull for some operations involving object exchange, e.g. sorting algorithms with moving.

---------------------------------------------------------------------------------------------------------
                       T&& vs const T&& vs const T& vs T
---------------------------------------------------------------------------------------------------------
You can overload function with all of these parameters at the same time. Following pairs are ambiguos
const T& | T&& | T      # all may take an rvalue
const T& | T&           # both may take an lvalue

Note T as parameter can be intialized with though explicit move and not sure about implicit?
const T&& is the most rare one, it can be used during overloading, possible usage is disabling usage of
temporary values.
foo(const X&)            => takes modifiable/const lvalues
foo(const X&&) = delete; => rejects modifiable/const rvalues
template <typename T> foo(const T&&) = delete;


https://www.bfilipek.com/2017/01/const-move-and-rvo.html
https://stackoverflow.com/questions/4938875/do-rvalue-references-to-const-have-any-use
https://www.youtube.com/watch?v=dTeKf5Oek2c&feature=youtu.be&t=2501
https://www.codesynthesis.com/~boris/blog/2012/07/24/const-rvalue-references/
---------------------------------------------------------------------------------------------------------
                               Perfect forwarding
---------------------------------------------------------------------------------------------------------
When you pass an rvalue to function, you expect it to be forwarted to other functions as rvalue too.
This is not a case, when argument is passed to function, since named rvalues are treated as lvalues. You
need help of compiler to save information about value categorie. This is done with special `forward`
function and some rules when templates are used.

Reference collapsing rules:
T&  &   => T&
T&  &&  => T&
T&& &   => T&
T&& &&  => T&&
T&& is called a universal reference since it can take both an rvalue and an lvalue

template<typename T>
void relay(T&& arg){
    foo( forward<T>(arg));
}

Type is deduced from argument therefore
relay(1) => T = int && => T&& = int&& && => int&&
relay(x) => T = int & => T&& = int&& & => int&&

---------------------------------------------------------------------------------------------------------
Implementation of `forward` and `move`
template<class T>
T&& forward(typename remove_reference<T>::type& arg) {
    return static_cast<T&&>(arg);
}

template <typename T>
typename remove_reference<T>::type&& move(T&& arg) {
  return static_cast<typename remove_reference<T>::type&&>(arg);
}
---------------------------------------------------------------------------------------------------------
As you can see, return type differes. move always return an rvalue while forward return value depens on
deduced argument type.

---------------------------------------------------------------------------------------------------------
                                    Pitfalls
---------------------------------------------------------------------------------------------------------
Don`t forget to use move in move constructor when performing constructor forwarding or constructing parent
class.
Derived(Derived&& rhs): Base(move(rhs)) {
    ...
}
http://thbecker.net/articles/rvalue_references/section_05.html
---------------------------------------------------------------------------------------------------------
http://thbecker.net/articles/rvalue_references/section_01.html
=========================================================================================================
                                 Type deduction
=========================================================================================================
Starting with C++11, you can use special data type called `auto` which makes compiler understand what
type is expected from its initializer. If you have no initializer, it won`t work.
auto d = 5.0;                           // d is double
auto i = 1 + 2;                         // i is integer
auto deduced_from_return = add(1,2);
auto var;                               // compilation error
     var = 1;

Reasons for using type deduction:
1. Frees you of spelling out types that are obviour or redundant.
2. Make code more adaptable, you don`t need change type of variable if initializer changes.
Resons against:
1. Hard to reason about type deduced by compiler.
2. Requires solid understanding to use

---------------------------------------------------------------------------------------------------------
                            Type Deduction
---------------------------------------------------------------------------------------------------------
Lets considere following template function, are types of expression passed and one used in declaration
always the same?
template <typename T>   // I will call it ParamType
void f(const ParamType& param)
int expr = 0;
f(expr);       // T is deduced as int but parameter type is const int&

It`s natural to expect that type deduced for T is the same as the argument passed to function but it
not always the case. Type deduction both depends on the type of expr and form of ParamType used in function
There are three cases:
1. Param is pointer or reference but not unvirsal reference
2. Param is the universal reference
3. Param is neither a pointer not a reference

---------------------------------------------------------------------------------------------------------
Case 1:
If expr is reference, discard reference during type deduction but not constness

template <typename T>
void f(T& param)
int i;
const int ci = i;
const int &ri = i;
f(i);           // T is int, param`s type is int&
f(ci);          // T is const int, param`s type is const int&
f(ri);          // T is const int, param`s type is const int&

Situation is a bit another for an r-value. If we change T to const T, then const can be discarded from
deduced type.
template <typename T>
void f(const T& param)
int i;
const int ci = i;
const int &ri = i;
f(i);           // T is int, param`s type is const int&
f(ci);          // T is int, param`s type is const int&
f(ri);          // T is int, param`s type is const int&

The same rules are applied to [const] pointers, e.g. pointer is discarded for param type but constness
is saved.
template <typename T>
void f(T* param)
int i;
const int *pi = &i;
f(&i);           // T is int, param`s type is const int*
f(pi);           // T is const int, param`s type is const int*

---------------------------------------------------------------------------------------------------------
Case 2:
Things are less obvious when template is taking universal reference parameters. Such parameters are
declared as rvalue references T&&, but they behave differently when lvalue arguments are passed in.
1. If expression is lvalue reference, T and ParamType are deduced to be lvalue references
2. If expression is rvalue, case 1 rules are applied (constness is ignored).
template <typename T>
void f(T&& param)
int i;
const int ci = i;
const int &ri = i;
f(i);           // i is lvalue, T is int&, param type is int&
f(ci);          // ci is lvalue, T is const int&, param`s type is const int&
f(ri);          // ri is lvalue, T is const int&, param`s type is const int&
f(1);           // rvalue, T is int, param type is int&&

---------------------------------------------------------------------------------------------------------
Case 3:
When ParamType is not a reference not a pointer, it means pass-by-value therefore we should remove any
cv modifiers as well as reference
template <typename T>
void f(T param)
int i = 27;
const int ci = i;
const int& ri = i;
const int* const pi = &ci;
f(i);                // T's and param's types are both int
f(ci);               // T's and param's types are again both int
f(ri);               // T's and param's types are still both int
f(cpi);              // T is const int*, param as well

---------------------------------------------------------------------------------------------------------
Corner case:
Don`t forget about piunter decay. This is not a problem if paramType is T&
template <typename T>
void f(T param)
const int arr[10];
const int *ptr = arr;
f(arr);              // T and param`s type are both const int *
f(ptr);              // T and param`s type are both const int *

template <typename T>
void f(T& param)
const int arr[10];
f(arr);              // T is const char[10], and parameter is const char (&)[10];

Ability yo declare references to array enables creation of a template that deduces the number of elements
that an array contains
template<typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept
{
  return N;
}

---------------------------------------------------------------------------------------------------------
                                Auto
---------------------------------------------------------------------------------------------------------
Type deduction performed by auto is very close to template one. You can imagine template function with
auto used instead of T, meaning it still depends if there is const before and reference after.
auto x = 27;          // case 3 (x is neither ptr nor reference)
const auto cx = x;    // case 3 (cx isn't either)
const auto& rx = x;   // case 1 (rx is a non-universal ref.)
auto&& uref1 = x;     // x is int and lvalue, so uref1's type is int&
auto&& uref2 = cx;    // cx is const int and lvalue, so uref2's type is const int&
auto&& uref3 = 27;    // 27 is int and rvalue,
                      // so uref3's type is int&&

One pitfall of auto is usage on uniform initialization. There is no way to understand whuch type is
expected from this initialization, since it is commonly used for initialization of both variables and
structs. Therefore, it was decided to use some generic class that would hold all passed variable as list.
But there is an limitation, all parameters must have the same type.
int x1 = 1;       // Ok
int x2(1);        // Ok
int x3 = {1};     // Ok
int x4{1};        // Ok
auto x5 = 1;      // Ok, deduced type is int
auto x6(1);       // Ok, deduced type is int
auto x7 = {1};    // Nok, deduced type is initializer_list<int>, but will compile
auto x8 {1};      // Nok, deduced type is initializer_list<int>, but will compile
auto x9 {1, 1.0}; // Nok, won`t compile, can`t deduce type for T

Note, the case with initializer_list performs two type deduction.  The first one is deduction that variable
is meant to be initializer_list, the second it type of parameter used by list, int in this case. This
is important when you send such parameter in template function
template<typename T>
void f(T param);
f({1,2,3});     // Error, can`t deduce type of 

template<typename T>
void f(initializer_list<T> param);
f({1,2,3});     // Ok, T is int

You can`t use auto as return type for uniform initialization for the same reasons. You explicitly define
trailing return type, but it is purpose not fixing auto deduction login, therefore don`t do this.
auto createInitList(){ return {1,2,3}; }            // error: can`t deduced type for {1,2,3}
auto func = [](const auto& param) { ... }({1,2,3}); // error: can`t deduced type for {1,2,3}


---------------------------------------------------------------------------------------------------------
                              Trailing return type
---------------------------------------------------------------------------------------------------------
Startinf from C++14, you define return type as auto. If it`s ambiguos which type is supposed to be returned,
trailing return type may be used. But it is not recomended for usage, since it makes code more vague.
auto switch(double i) -> double {
    if (i) return i;        // Ok, deduced type is int
    else return 0;          // 0 is int, making it ambiguos , since previous statement was expected to
                            // return double
}

=========================================================================================================
                                    Decltype
=========================================================================================================
Primary use of decltype is function templates where the function`s return type depends on its parameter
types. For instance you take some family of containers and you want return result of container function
and you don`t know return type upfront. Maybe you would expect that return type is the same as conteiner
parameter type.
template<typename Container>
Contrainer::object_type getObject(Container &c, int index) {
    return c[i];
}

But this may be wrong. What if there is more optimal to hold data instead of it`s original type and
you prefer to return this new object. Then, function defined above is wrong. The common emaple is bool
vector which is internally implemented as bitset.
Another solution is to return auto, maybe you will need to clarify return type using trailing decltype.
In this case you would use auto as return type but it type deduction may provide wrong type since it
ignores references and constness.
getObject(container, 1) = 10;   // won`t work, reference was ingored

The solution of this problem is using decltype which override behaviour of auto, making it use declaration
type of variable or return type of function. This way, if function returns reference, auto will save it.
decltype(auto) getObject(Container &c, int index) {
    return c[i];
}
getObject(contaienr, 1) = 10;   // Ok

In general, decltype provides intuitevly expected types:
const int i = 0;           // decltype(i) is const int
bool f(const Widget& w);   // decltype(w) is const Widget&
                           // decltype(f) is bool(const Widget&)
struct Point {
  int x, y;                // decltype(Point::x) is int
};                         // decltype(Point::y) is int
Widget w;                  // decltype(w) is Widget
if (f(w)) …                // decltype(f(w)) is bool
template<typename T>       // simplified version of std::vector
class vector {
public:
  …
  T& operator[](std::size_t index);
  …
};
vector<int> v;             // decltype(v) is vector<int>
…
if (v[0] == 0) …           // decltype(v[0]) is int&

But there is a case that you would not expect. Probably, you won`t face such a corner case unless you are
some kind of library writer
int x;
decltype(x);        // int as expected
decltype((x));      // int&
This can be understood in following wat, x is a variable access and variable has type int, while, (x) is
a lvalue expressing returning value of variable, therefore we append reference. Such, a seemingly trival
change in the way you write a return statement can affect the deduced type for a function;
decltype(auto) f1() {
    int x = 0;
    reuturn x;          // decltype(x) is int, so f1 returns int
}
decltype(auto) f2() {
    int x = 0;
    reuturn (x);          // decltype((x)) is int&, so f1 returns int&
}
Note, this code returns reference to local variable which is undefined behaviour.

https://stackoverflow.com/questions/14115744/significance-of-parentheses-in-decltypec
https://stackoverflow.com/questions/3097779/decltype-and-parentheses/3097803
=========================================================================================================
                                    Lambdas
=========================================================================================================
Lambda expression is a convinient way of defining an anonymout function object (a closure) right at the
location where it is invoked or passed as an argument to a function. Typically used to encapsulate
encapsulate algorithms so that they can be passed to another function. However, it is possible to execute
a lambda immediately upon definition.

Reasons why lambdas were introduced:
1. There are pleany of generic functions in STL that can take function/functor but defining new function
   every time is not convinient/redable/usable (not all compilers allow to use nested functions, but this
   function may collapse with another in current namespace).
2. Functors can be heavy in terms of resources, memory to allocate, take allot of window space to define
   them, making code less readable
C++11 introudec lambdas allow you to write an inline, anonymous functor to replace the struct/class you
would write by hand. It is cleaner to read and potentially simpler to maintain, compiler can generate
more optimal code then you would write by hand.

Lambda looks like `[capture close][optional parameter list] mutable throw()->int(trailing ret type){body}` 
Usage example:
void func(std::vector<int> &v) {
  std::for_each(v.begin(), v.end(), [](int) { /* do something here*/ });
}
[]{ .. code .. }(); // immediately executed lambda expression
{ .. code .. }      // functionally equivalent

You can use lambda expressions to initialize variables based on the result of an algorithm
int a = []( int b ){ int r=1; while (b>0) r*=b--; return r; }(5);

You can pass lambda to another lambda
[&]( std::function<void()> algorithm ) // wrapper section
   { algorithm(); } (
    [&]{ .. code .. }
   );

Note, lambda is not a function pointer and it is not an anonymous function, but capture-less lambdas can
be implicitly converted to a function pointer.

---------------------------------------------------------------------------------------------------------
                        Return type
---------------------------------------------------------------------------------------------------------
Usually, type is deduced by the compiler, but when it is ambiguos, you can define helper trailing return
type. Think of return type as `decltype(return_statement)`

    std::transform(v.begin(), v.end(), v.begin(),
        [](double d) -> double {    // otherwise it is not clear if 0 is int or double
            if (d < 0.0001) {
                return 0;
            } else {
                return d;
            }
        });

---------------------------------------------------------------------------------------------------------
                            Capture
---------------------------------------------------------------------------------------------------------
Capture close is C++14 feature, that let`s you introduce new variable and how they are captured to body.
This is a convinient way to save environment without passing everuthing to functor constructor.
Empty close means that no variables are capture from enclosing scope, otherwise:
[&] => all variables that you refer to are captured by reference
[=] => captured by value, but it is constant and requires mutable to be changed

You can use any of these modes as defaul one and then override it for separate variables
[&, factor, &factor]

Default and identifiers can`t be repeated, default can be duplicate for further variables, e.g.
[&, &var]  // NOK
[this, this] // NOK
[var, var] //NOK

It is possible to expand variadics in capture, it is called pack expansion
template<class... Args>
void f(Args... args) {
    auto x = [args...] { return g(args...); };
    x();
}

---------------------------------------------------------------------------------------------------------
                                 Implementation
---------------------------------------------------------------------------------------------------------
Under the hood, lambda is a class generated by the compiler, with all parameters from environment passed
as constructor arguments, and defined function operator to execute previously defined body. Evironment is
saved depending on closure used. This is just a more convinient way to implement functor.
Note, lambda has a default constructor deleted but copy/move constructor defined, therefore one can`t be
constructed but cam be copoied/moved.
auto lambda = [](){};
decltype(lambda) l1;                // Compilation error, no defaul constructor
decltype(lambda) l1{lambda};        // Ok

Since it is just a compiler generated class, we can inherit from it.
template<typename L1, typename L2>
struct S : L1,L2 {
    S(L1 l1, L2 l2) : L1(std::move(l1)), L2(std::move(l2)) {}
    using L1::operator();
    using L2::operator();
}
auto l1 = [](){ return 4; };
auto l2 = [](int i){ return i * 10; };
auto combined = S(l1,l2);
combined();     // calls l1
combined(10);   // calls l2

https://www.youtube.com/watch?v=3wm5QzdddYc
https://www.youtube.com/watch?v=W-xTpqj31mI
---------------------------------------------------------------------------------------------------------
                                   Constness
---------------------------------------------------------------------------------------------------------
By default, function call operator is defined as const and any variables taken by value can`t be changed.
If you still want to change them, operator should be defined as mutable. This is done in the following way

int var = 1;
auto l1 = [var] {  var = 2; };          // Compilation error, can`t change const variable
auto l2 = [var] () mutable { var = 2 }; // Ok

Starting with C++14, you can declare variables in capture list. This is called a statefull lambda.
auto l3 = [var_new = 1, p = std::make_unique<int>(new int)] { ... };

If you define noncopyable variables in capture, lambda becomes non copyable as well
auto l4 = l3;       // Compilation error

---------------------------------------------------------------------------------------------------------
                          Generalized capture (C++14)
---------------------------------------------------------------------------------------------------------
In C++14 you can introduce and initialize new vatiables in capture clause, without need to have them
in enclosing scope. The initialization can be expressed as any arbitrary expression; the type of the new
variable is deduced from the type produced by the expression. One benefit of this feature is that you
can capture move-only variables from the surrounding scope and use them in a lambda
pNums = make_unique<vector<int>>(nums);
//...
      auto a = [ptr = move(pNums)]()
        {
           // use ptr
        };

int x = 4;
auto y = [&r = x, x = x+1]()->int {
            r += 2;
            return x+2;
         }();

---------------------------------------------------------------------------------------------------------
                                    Functor
---------------------------------------------------------------------------------------------------------
Sometimes you need a function that will change it`s flow depending on environment and you don`t want
to pass entire environment as one of input arguments. One possible solution is global varibales but it
is not good coding practive. Another solution is a functor. It is an object that can be treated as
though it is a function or function pointer. Since it is an object, it can hold state. It is commonly
used with stl withing algorithms.
class increment {
    int num;
    increment(int n) : num(n) {  }
    int operator () (int arr_num) const {
        return num + arr_num;
    }
};
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr)/sizeof(arr[0]);
    int to_add = 5;
    transform(arr, arr+n, arr, increment(to_add));
    for (int i=0; i<n; i++)
        cout << arr[i] << " ";

---------------------------------------------------------------------------------------------------------
                            Parameter list
---------------------------------------------------------------------------------------------------------
Lambda can take input parameters, but it is optional. Usefull for some small function or when functor
is required

---------------------------------------------------------------------------------------------------------
                                Generic Lambdas
---------------------------------------------------------------------------------------------------------
auto lambda = [](auto x, auto y) {return x + y;};
---------------------------------------------------------------------------------------------------------
                                    Pitfalls
---------------------------------------------------------------------------------------------------------
Reference captures introduce lifetime dependency, therefore you should be certail that local variables
are still accessible during the call. In other words, you should not capture by reference/pointer local
variables and then use lambda out of scope. For instance return lambda as the result of a function or
pass to async.

---------------------------------------------------------------------------------------------------------
https://stackoverflow.com/questions/7627098/what-is-a-lambda-expression-in-c11

=========================================================================================================
                     Lambda vs Functor vs bind vs function
=========================================================================================================
Bind a function taking callable, placeholders and arguments that will be bound to placeholders.
This can be though as making copy of function but with some explicilty choosen parameters having a default
value. Call to bind returns some implementation defined value that is not know for developer and must be
assigned to some auto variable.
Note, it copies passed arguments

void print1(int i) {
    std::cout << i << '\n';
}
void print2(int i, string &s) {
    std::cout << i << " " << s << '\n';
}
auto f1 = bind(&print1, 5);
f1();                               // prints 5
int i = 5;
auto f2 = bind(&print1, i);
f1()                                // prints 5
i = 6;
f1()                                // still prints 5 since value of i was copied
auto f2 = bind(&print1, ref(i));
i = 5;
f2();                               // print 5 even i during registration was 6
auto f3 = bind(&print2, _1, 1);
f3("string");                       // prints 1 string
f3("another string");               // prints 1 another string
auto f4 = bind(&print2, _2, _1);    // swap parameters
f4("string", 2);                    // prints 2 string
f4("string", 2, 1, 2, 3, 'a', ...); // prints 2 string, extra parameters are swallowed

template<typename T>
void print3(T i) {
    std::cout << i << '\n';
}
auto f5 = bind(&print3<int>, ref(i));
i = 5;
f5();                               // print 5 even i during registration was 6
std::functional<void(int)> f6(f5);  // works

Issue of bind is bit compile time overhead in terms of speed and memory require as well as runtime in
term of executed instruction count and speed. Usually you wll just use lambda but in general it is hard
compare them since they are doing different things. bind forwards a call maybe with some parameter
taking another slot while lambda is entire function that should be look through to understand what does
that mean.

std::function is generalized form of function pointer which can be initialized from function pointer or
any other callable object. Problem of it, is the amount of memory require to be allocated, which may even
involve heap allocation. As well it take more time calling then auto variable.
One usefull usage of function is usage within arrays since it is easy to convert lambda to function and
push to vector. Otherwise you need to use `decltype(lambda_var)`.
Note, lambda without capture can be converted to function pointer.

https://www.youtube.com/watch?v=JtUZmkvroKg
=========================================================================================================
                                For-each loop
=========================================================================================================
Loop is a conviniet way to iterate though an array but it is prone to off-by-one errors.
C++11 introduces a new type called a for-each loop (or a range-based for loop) that provides a simpler
and safer method for cases where we want to iterate through every element in an array (or other list-type
structure).
int arr[] = { 0, 1, 2, 3, 4};
for (int number : arr) // iterate over array 
   std::cout << number << ' ';

Commonly variable type is define as auto.
for (auto number : arr)
for (auto &number : arr) // no copying is made
Note, that variable number is not an index. It`s assigned the value of the array element for the current
loop iterator.

It can be used with list-type structes like vectors, queue, lists, map and so on.
std:vector<int> arr[] = { 0, 1, 2, 3, 4};
for (const auto& number : arr) // iterate over array 
   std::cout << number << ' ';

For each doesn`t work with pointer to array, since it needs size information. Because arrays decay to
pointer, there is no information need by for-each providing compile time error.

You don`t need to dereference iterator used in for-each loops since it is taken by value or reference,
except pointer cases. While, iterator used for simple loops are generized pointers and require
dereferencing.

=========================================================================================================
                                 Object slicing
=========================================================================================================

This problem can be faced with data wrappers like vectors, lists.
    std::vector<Base> v;
    v.push_back(Base(5));    // add a Base object to our vector
    v.push_back(Derived(6)); // derived is sliced to base
You may try to use refernence or pointer and that is the last one is valid but it is not fully clear how
to apply refernce in such case. The stundart library provied a usefull workaround: the reference_wrapper.
It is a class that works like a reference but allows assignment and copying. When you create
reference_wrapped object, it can`t be anonymous since it would result to dungling pointer, as well, you
to get object from wrapper before using.
	std::vector<std::reference_wrapper<Base> > v;
	Base b(5); // b and d can't be anonymous objects
	Derived d(6);
	v.push_back(b); // add a Base object to our vector
	v.push_back(d); // add a Derived object to our vector
	// Print out all of the elements in our vector
	for (int count = 0; count < v.size(); ++count)
        // we use .get() to get our element from the wrapper
		std::cout << "I am a " << v[count].get().getName() << " with value " << v[count].get().getValue() << "\n";

Another common issue is trying to assign dynamic type though static.
    Derived d1(5);
    Derived d2(6);
    Base &b = d2;
    b = d1; // this line is problematic
This assignments calls base class assignment operator. There’s no easy way to prevent this from happening,
usually you will try to avoid such situation. Othwise, you have an option to provide virtual assignment
operator for child case.

=========================================================================================================
                                   Assertions
=========================================================================================================
Assertion is mechani sm allowing to detect a violated assumtion, along with printing error message and
terminating the program if condition is false.

An assert statement is a preprocessor macro that evaluates a conditional expression at runtime. If the
conditional expression is true, the assert statement does nothing. If the conditional expression evaluates
to false, an error message is displayed and the program is terminated. This error message contains the
conditional expression that failed, along with the name of the code file and the line number of the assert.
This makes it very easy to tell not only what the problem was, but where in the code the problem occurred.

int getArrayValue(const std::array<int, 10> &array, int index)
{
    assert(index >= 0 && index <= 9 && "Index out of range");
    return array[index];
}

Syntex is following `assert( condition [ && message ] )`. Failing assert prints condition statement,
if the first condition of && failt, entire statement is considered failed and printer, otherwise
condition is not failed and messaged is converted to valid pointer.

Assert makes creates some efficience penalty to perform condition checking therefore it should be enabled
on debug builds but not production one. Some IDE`s come with special macro to enable/disable asserts,
like `#define NDEBUG`.

---------------------------------------------------------------------------------------------------------
                                Static_assert
---------------------------------------------------------------------------------------------------------
C++11 introduce method of performing compile time assert, calles static_assert, It should use trick
as C assert with logical operators, instead it takes two parameters: condition and message.
It has one limit, condition is supposed to be applicable for compile time check;
static_assert(sizeof(long) == 8, "long must be 8 bytes");
static_assert(sizeof(int) == 4, "int must be 4 bytes");
std::cin >> var; static_assert(var == 123, "Message")  // Won`t compile

=========================================================================================================
                                   Templates
=========================================================================================================
In C++, function templates are functions that serve as a pattern for creating other similar functions.
The basic idea behind function templates is to create a function without having to specify the exact type(s)
of some or all of the variables. Instead, we define the function using placeholder types, called template
type parameters.
When you call a template function, the compiler makes a copy of the template, replacing the placeholder
types with the actual variable types from the parameters in your function call!

To create a template you need to define definition and placeholdetrs. They are usually defined together
using what is called a template parameter declaration:
template <typename T> // this is the template parameter declaration
T max(T x, T y) {
    return (x > y) ? x : y;
}
Here T is a placeholder, conventially standing for `type`. You can have multiple types by defining them
in parameter declaration.

Placeholder type is deduced from argument from parameters but it won`t compile if there is an ambiguity.
max(1, 1.0); // There is only one T but we pass both int and double
Used type can be explicitly defined
max<int>(1, 1.0);  // 1.0 is implicitly converted to int.

---------------------------------------------------------------------------------------------------------
                                 Instantiation
---------------------------------------------------------------------------------------------------------
Templates are not compiled directly with all other code. Instead, when compiler sees call to template, it
will instantiate definition for passed arguments.

There are a couple of problems with template class separation between files. For instance you have a
template class in header and definition in source file.
foo.h
template<typename T>
class foo{
    T getLength();
}

foo.cpp
template<typename T>
T foo::getLengh(){ return ...};

main.cpp
main(){
    foo<int> f;
    f.getLenght();
}
Example above won`t compile with linker error, since it couldn`t find definiton. The problem is following,
compiler needs to see definition to instantiate it. Since definition in separate compilation unit,
instantiation for user type is skiped, class gets instantiated without function definition.
It is allowed to not instantiate functions/members that are not used, except virtual functions (you
can`t be sure that one won`t be called)

Solutions:
1. Put definiton to header file
   Easy, but increases compile and link time.(linker removes duplicate to avoid bloat of executable?)
2. Include .cpp into header
   Non-standart usage of include
3. 3 file approach
   Create file that will hold all needed instantiations. Include both .h and .cpp to it. Whenver you need
   instantiation of a class, append it to file
   template class Array<int>; // Explicitly instantiate template Array<int>
   Requires maintaining of the file, requires more space in executable since everything is instantiated
   without exception.

https://stackoverflow.com/questions/183108/is-object-code-generated-for-unused-template-class-methods
---------------------------------------------------------------------------------------------------------
                        Partial Instantiation
---------------------------------------------------------------------------------------------------------
Since, compiler is allowed to skip instantion of member that are not used, this has side effects that are
commonly used in metaprogramming.
template <class T>
class Xyzzy {
public:
    void CallFoo() { t.foo(); }  // Invoke T::foo()
    void CallBar() { t.bar(); }  // Invoke T::bar()
    T t;
};
class FooBar {
public:
    void foo() { ... }
    void bar() { ... }
};
class BarOnly {
public:
    void bar() { ... }
};
    Xyzzy<FooBar>  foobar;    // Xyzzy<FooBar> is instantiated
    Xyzzy<BarOnly> baronly;   // Xyzzy<BarOnly> is instantiated
    foobar.CallFoo();         // Calls FooBar::foo()
    foobar.CallBar();         // Calls FooBar::bar()
    baronly.CallBar();        // Calls BarOnly::bar()
This compiles since there is no need to instantiate `foo()` for `BarOnly`

https://stackoverflow.com/questions/183108/is-object-code-generated-for-unused-template-class-methods
---------------------------------------------------------------------------------------------------------
                              Non-Type parameters
---------------------------------------------------------------------------------------------------------
Template classes and functions can make use of another kind of template parameter known as a non-type
parameter. It is a special type of parameter that does not substitute for a type, but is instead replaced
by a value. A non-type parameter can be any of the following:
- A value that has an integral type or enumeration
- A pointer or reference to a class object
- A pointer or reference to a function
- A pointer or reference to a class member function
- std::nullptr_t

For instance, template array with user defined size.
template <class T, int size> // size is the non-type parameter
class StaticArray
{
    T m_array[size];
    T* getArray();
    T& operator[](int index);
};
StaticArray<int, 10> arr;
Note, this value is supposed to be const or constexpr.

---------------------------------------------------------------------------------------------------------
                            Template specialization
---------------------------------------------------------------------------------------------------------
Instead of having one definition for all parameters, you may want the same definiton for most parameters
and separate for exceptions. For instance `max` function require different definition for intergers and
strings. To do so, we use a function template specialization to create a specialalized version of the
max() function for string.

template<typename T>
string& max(T& x, T& y) { ... }

template<>
string& max<string>( string& x, string& y) { ... }

When the compiler goes to instantiate max<string>, it will see already explicitly defined that function,
and use it istead of defining new one.
Another common use case is specializng pointer data types to prevent shallow copy.

The same mechanism can be used for classes.
---------------------------------------------------------------------------------------------------------
                        Partial template specialization
---------------------------------------------------------------------------------------------------------
Full template specialization has a problem, it requires specializaition for all template argument, even
non-type ones.  This may be problematic, for instance array wrapper:
template<typename T, int size>
class Array{...}

template<typename T, int size>
void print_array(Array<T, size> arr){...};

Again, you may wan`t to have special definition of function `print_array` for some type. You may try to
use full specialization. This will work but since full specialization is required, it will work only for
array of specified size.

template<>
void print_array<char, 10>(Array<char, 10> arr){...};
Array<int, 10> arr1;
Array<int, 11> arr2;
print_array(arr1);      // Ok
print_array(arr2);      // Nok

Instead you can define function for specified type with template non-type parameter

template<int size>
void print_array(Array<char, size> arr){...};

Note, this only works for class specialization, you cannot partially specialize functions (functions
must be fully specialized). This creates a limitation for member functions.
template <class T, int size> // size is the expression parameter
class Array {
    T m_array[size];
public:
    void print() {
        for (int i = 0; i < size; ++i)
            std::cout << m_array[i] << ' ';
        std::cout << "\n";
    }
}
What if you want to partially specialize `print`
template<int size>
void Array<doble, size>::print(){
    for (int i = 0; i < size; ++i)
        std::cout << std::scientific << m_array[i] << " ";
    std::cout << "\n";
}
This doesn’t work, because we’re trying to partially specialize a function, which is disallowed.
Possible solutions:
1. Partial specialization of class instead of function.
   Requires alot of code duplication
2. Inheritance of the common base


template <class T, int size>
class BaseArray {
    T m_array[size];
};

template <class T, int size>
class Array: BaseArray<T, size> {
public:
    void print() {...}
}

template<int size>
class Array<doble, size>: BaseArray<double, size>{
public:
    print(){
        for (int i = 0; i < size; ++i)
            std::cout << std::scientific << m_array[i] << " ";
        std::cout << "\n";
    }
};
---------------------------------------------------------------------------------------------------------
What if you wan`t to make one template used for non-pointer data types and another for pointers. Class
template can be partially specialized for this.
template<typename T>
class Storage {         // for non-pointer
    T value;
    T get(int);
}

template<typename T>
class Storage<T*> {     // for pointer
    T* value;
    T* get(int);
}

---------------------------------------------------------------------------------------------------------
=========================================================================================================
                                   Exceptions
=========================================================================================================
When writing reusable code, error handling is a necessity. One of the most common ways to handle potential
errors is via return codes. The primary virtue of this approach is that it is extremely simple but it has
plenty of drawbacks:
1. Returned value. like -1, is cryptick and you need to look through entire function to find what it may
   mean. Don`t forget, it could be use in multiple places through function.
2. Usually it is hard to return both result and error.
3. Error code should be checked constantly, after each command.
4. Functions that have no return value. You can`t return from constructor if something goes wrong.
5. If caller doesn`t handle an error, it is lost.

Exception handling provides a mechanism to decouple handling of errors or other exceptional circumstances
from the typical control flow of your code. This allows more freedom to handle errors when and how ever
is most useful for a given situation, alleviating many (if not all) of the messiness that return codes cause.

Exceptions are implemented using three keywords that work together: throw, try, and catch.

Throw is used to signal an exception or error case has occurred. It is commonly called raising an exception.
To use thow statement, use the thow keyword, with a value of any data type you wish to signal exception.
throw -1;                                           // throw a literal integer value
throw ENUM_INVALID_INDEX;                           // throw an enum value
throw "Can not take square root of negative number";
throw dX;
throw MyException("Error");

Exception should be handled in some block. If nobody does it, program terminates.
Try block is defined in following way:
try {
 ... code ...
 throw -1;
}
This doesn`t handle exception, but sends it to catch block.
Catch block is defined try block but it takes arguments. There may be multiple catch blockes chained
together.
catch(int x) {
    ... handle ...
}
catch(double) { ... }
Exceptions of fundamental types can be caught by value, but exceptions of non-fundamental types should be
caugh by const reference to avoid making an unnecessary copy. As well, compiler won`t promote or covert
values to match catch block.

If exceptions gets to catch block, even if it is empty, considered to be handled, however, usually you
are supposed to do something usefull: print/log error, return error, throw error again.

When exception is not handled in current scope, stack get`s unwinded, and next scope tries to handle an
exception. This is done untill somebody handles it or entire program terminates. This is quite flexible,
meaning you can avoid handling exception and instead pass it to somebody higher in the stack.

Exception handling is best used when all of the following are true:
The error being handled is likely to occur only infrequently.
The error is serious and execution could not continue otherwise.
The error cannot be handled at the place where it occurs.
There isn’t a good alternative way to return an error code back to the caller.

---------------------------------------------------------------------------------------------------------
                    Catch-all handlers
---------------------------------------------------------------------------------------------------------
There may be to many types of object passed as signal or maybe you are not interested at exception at all.
Maybe you are using new library and don`t know what exceptions are possible at all. For such cases you can
use special catch that will accept anything but won`t provide handle to signal. This is called catch-all
handler. 
try {
    throw 5; // throw an int exception
} catch (double x) {
    std::cout << "We caught an exception of type double: " << x << '\n';
} catch (...) { // catch-all handler
    std::cout << "We caught an exception of an undetermined type\n";
}
It should be last in catch list, some compiler may warn you about this.

---------------------------------------------------------------------------------------------------------
                              Exception specifiers
---------------------------------------------------------------------------------------------------------
Exception specifiers are a mechanism that allows us to use a function declaration to specify whether a
function may or will not throw exceptions. This can be useful in determining whether a function call needs
to be put inside a try block or not. As well, compiler may be more aggressive in optimization of function
that won`t throw.
int function() throw() => implies that function won`t throw outside. It may throw but it is fine if handled
internally. If exception is thrown to external scope, program should terminate.
int function() throw(double) => function will throw only doubles.
int function() throw(...) => function may throw anything
int function() noexcept => same as thow() ?

---------------------------------------------------------------------------------------------------------
                             Exception classes
---------------------------------------------------------------------------------------------------------
Default signals that are used are often vague and tell not much of usefull information. You may use
strings everywhere but this won`t know exact reason that should be described in message before handling it
most of the tiems. 
One wat to solve this problem is using of exception classes. It is a normal class that is designet
specifically to be theown as an exception. But there may be multiple types of exceptions, therefore you
will design an entire tree of inheritance. However how to handle defaul ones like bad reference cast.
Actually, there is an already define class that is used as base for all default excpetions, and ,probably,
you should use it as well.

class ArrayException: public std::exception {
    std::string m_error;
public:
    ArrayException(std::string error) : m_error(error) {}
    const char* what() const noexcept { return m_error.c_str(); } // C++11 version
};
class IntArray {
    int m_data[3]; // assume array is length 3 for simplicity
public: IntArray() {} 
    int getLength() { return 3; }
    int& operator[](const int index)
    {
        if (index < 0 || index >= getLength())
            throw ArrayException("Invalid index");
        return m_data[index];
    }
};
 
int main() {
    IntArray array;
    try {
        int value = array[5];
    }
    catch (ArrayException &exception) // derived catch blocks go first
    {
        std::cerr << "An array exception occurred (" << exception.what() << ")\n";
    }
    catch (std::exception &exception)
    {
        std::cerr << "Some other std::exception occurred (" << exception.what() << ")\n";
    }
}
---------------------------------------------------------------------------------------------------------
                                    Rethrow
---------------------------------------------------------------------------------------------------------
Occasionally you may run into a case where you want to catch an exception, but not want to (or have the
ability to) fully handle it at the point where you catch it. This is common when you want to log an error,
but pass the issue along to the caller to actually handle.
There are couple tipical mistaces with doing this.
1. Taking exception by value
try{ ... }
catch (int exception) {
     throw exception;
}
2. Slicing an object
try{ ... }
catch (Base &exception) {
    throw exception;   // it is copy initialized by Base copy constructor under hood
}
The proper way is calling throw again without parameters
try{ ... }
catch (Base &exception) {
    throw;
}
---------------------------------------------------------------------------------------------------------
                               Function try block
---------------------------------------------------------------------------------------------------------
Function try blocks are designed to allow you to establish an exception handler around the body of an
entire function, rather than around a block of code. They are used to handle exception during parent
class constructions, if child is expect to do so. They can be used for other member functions but it is
rare.
class A
{
    int m_x;
public: A(int x) : m_x(x) {
        if (x <= 0)
            throw 1;
    }
};

class B : public A {
public:
    B(int x) try : A(x) // note addition of try keyword here
    { }
    catch (...) // note this is at same level of indentation as the function itself
    {
         // Exceptions from member initializer list or constructor body are caught here
         // If an exception isn't explicitly thrown here, the current exception will be implicitly rethrown
    }
};

try {
    B b(0);
}
catch (int) {
    return -1;
}
Note the addition of the “try” keyword before the member initializer list. This indicates that everything
after that point (until the end of the function) should be considered inside of the try block. Any exception
thrown between the try keyword and the end of the function body will be eligible to be caught here.
if either A or B’s constructor throw an exception, it will be caught by the try block around B’s constructor.
Unlike normal catch blocks, which allow you to either resolve an exception, throw a new exception, or
rethrow an existing exception, with function-level try blocks, you must throw or rethrow an exception.
---------------------------------------------------------------------------------------------------------
                                    Pitfalls
---------------------------------------------------------------------------------------------------------
Variabled declared in try block is not available in catch block, therefore it may be impossible to clean
leaked resource.
try {
    Person *john = new Person("John", 18, PERSON_MALE);

catch (PersonException &exception)
{}  // john is not accessible here

Simple solution is to declare variables above block.
Person *john = nullptr;
try {
    john = new Person("John", 18, PERSON_MALE);
}
catch (PersonException &exception) { 
    free(john);
}
Another way is to use variables that can clean themselfs after leaving scope, e.g. smart pointers
try {
    Person *john = new Person("John", 18, PERSON_MALE);
    unique_ptr<Person> upJohn(john);
}
catch (PersonException &exception)
{}
---------------------------------------------------------------------------------------------------------
You should never throw in destructor. The problem occurs when an exception is thrown from a destructor
during the stack unwinding process. If that happens, the compiler is put in a situation where it doesn’t
know whether to continue the stack unwinding process or handle the new exception. The end result is that
your program will be terminated immediately.
---------------------------------------------------------------------------------------------------------
Perfomance concerns.
Exceptions require additional space in a executable as well as addition time to perform error checking
and all other generated code. Main perfomance issue appears when exception is thrown and you need to
find handler as well as unwind stack which may be deeply nested.

Some modern computer architectures support an exception model called zero-cost exceptions. Zero-cost exceptions,
if supported, have no additional runtime cost in the non-error case (which is the case we most care about
performance). However, they incur an even larger penalty in the case where an exception is found.

=========================================================================================================
                                   Attributes
=========================================================================================================
Attributes allow you mark functions, vaiables, and other entities with compiler-specific or standart
properties.  C++17 adds three attributes for programmers to better express
their intent to the compiler and readers of the code: maybe_unused, fallthrough, nodiscard.  Before C++17
attributes are noreturn, deprecated (entity is going to be removed in a later version, usage results in a
compiler warning) and carries_dependency(used for optimizing atomic operations). 
[[maybe_unused]] => var_type var_name => now it is a standart way to avoid compiler warnings, maybe usefull
                    when variable usage is closed under defines and is used in release/debug version
[[falthrough]]   => indicates a fallthrough in a switch statement is intentional, otherwise compiler may
                    warn abouy missed break/return.
[[nodiscard]]    => function declared with it indicate that return value shouldn`t be ignored by the caller.
                    Usefull for ensuring caller check a return value,or that some scope guard object has
                    a resonable liftime.
                    [[nodiscard]] struct lock_guard;
                    lock_guard lock(mutex& m);
                    {
                        lock(mutex);    // Warning, ignored returned value
                    }

https://blog.tartanllama.xyz/c++17-attributes/
=========================================================================================================
                               Most Vexing Parse
=========================================================================================================
C++ has a rule, anything that could be considered as a function declaration, the compiler should parse it
as a function declaration.
struct B {
    explicit B(int x){}
};
struct A {
    A (B const& b){}
    void doSomething(){}
};
struct C {
    void doSomething(){}
};
int main()
{
    int x = 42;
    A var(B(x));  // doesn`t compile, var is a function, taking function, taking x and returning B
    A var(B());   // wouldn`t compile if there was a default constructor in B for the same reason
    C c();        // doesn`t compile
}
Usually it is fixed using uniform initialization, but before C++11, you would wrap arguments into
parenthesis.

https://en.wikipedia.org/wiki/Most_vexing_parse
https://www.fluentcpp.com/2018/01/30/most-vexing-parse/
=========================================================================================================
                                     Inline
=========================================================================================================
Function defined in class body are considered implicitly inlined, therefore they are not considered to
result in multiple defitions when such class in included with header.

=========================================================================================================
                                    C vs C++
=========================================================================================================
---------------------------------------------------------------------------------------------------------
                                    Features
---------------------------------------------------------------------------------------------------------
C has VLA (Variable Length arrays) =>
~ Allocation on stack, but you can`t return stack allocated variables from functions
~ Variable lenght on creating, but can`t be changed after
+ No need to deallocate
- Can`t be extern/static, only works if placed in the end of a structure.
- Cannot have an initializer
C++ has pleanty of generic data structures that can somewhat replace VLAs, for instance, vector.

Difference is void pointer casting. In C, pointer conversions to and from void* are always implicit.
C++ is more strongly typed therefore converting T* to void* are implicit, but void* to anything else 
requires a cast.

Initializers. In C you have ability to initize the data in the struct directly, but it makes no much sense
for C++ with its OOP and private membmers.

---------------------------------------------------------------------------------------------------------
Array without explicit size defined
---------------------------------------------------------------------------------------------------------
The number of elemnts of an array must be detemined either at compile time or since C99 can be evaluated
at runtime at the point of creation. You can define an aeeay without explicit suze for the leftmost
dimension if you provice an initialized.  The compiler will infer size from intializer
int m[][2] = {{ 1, 2 }, { 3, 4 }};  // equivalent to int m[2][2] = {{ 1, 2 }, { 3, 4 }};
int m[][] = {{ 1, 2 }, { 3, 4 }};  // not valid

=========================================================================================================
                                    Size of
=========================================================================================================
Sizeof is pure compile time in C++ and C prior to C99 based on compile time information. Starting with C99
there are variable length array size of which is handled at runtime. Any other operands/types (not allocated
dynamically) are computed at compile time. When using C99+, sizeof doesn`t return at compile time therfore
it`s value cannot be used in scope where such value is requied - for example initializing static variables,
unless a compiler specific extension allows it.


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
                                Output buffering
=========================================================================================================
Flushing for stdout is determined by its buffering behaviour. The buffering can be set to three modes:
_IOFBF full buffering: waits until fflush() if possible,
_IOLBF line buffering: newline triggers automatic flush,
_IONBF direct write always used.
Support for these characteristics is implementation-deﬁned, and may be affected via the setbuf() and setvbuf() functions.

https://stackoverflow.com/questions/13932932/why-does-stdout-need-explicit-flushing-when-redirected-to-file/13933741#13933741


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
                                    To study
=========================================================================================================
https://en.m.wikibooks.org/wiki/More_C%2B%2B_Idioms
http://codexpert.ro/blog/2014/10/17/c-gems-ref-qualifiers/      ref qualifiers
https://foonathan.net/2018/03/rvalue-references-api-guidelines/
https://stackoverflow.com/questions/130117/throwing-exceptions-out-of-a-destructor
https://stackoverflow.com/questions/136880/sell-me-on-const-correctness
https://www.cprogramming.com/tutorial/const_correctness.html
http://www.possibility.com/Cpp/const.html
https://habr.com/ru/company/infopulse/blog/341264/
https://stackoverflow.com/questions/105014/does-the-mutable-keyword-have-any-purpose-other-than-allowing-the-variable-to
https://arne-mertz.de/2017/10/mutable/
https://arne-mertz.de/2016/07/const-correctness/
    # relase/commit semantics
constness and lambdas
