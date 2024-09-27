### NoClueLang by NoClueBruh
**Most stuff in NoClueLang work a little bit different from other languages!**

**I made this language on my own without any tutorials because that would ruin the fun wouldn't it**
# Types 
There are 3 types supported by NoClueLang and they are the following;
- _int_
- _float_
- _string_
  
Yep, there aren't any booleans because I was lazy!

more on that later!!!

# Variables
Variables in NoClueLang can be defined as so
```
int x = 0;
int x1;

float y = 0;
float y1;

string z = 0;
string z1;
```

And you can assign them different values like this
```
x = 1;
x = -x;
...
```
Keep in mind that you can only assign values to variables that way!

That means no +=, -=, *=, ++ etc

And of course, assigning values of different types will result in an error!

# Functions
Functions in NoCluelang can be defined as so
```
function <returnType> <name> (<params>)
  ...
end
```

For example
```
function int sum(int a, int b)
  return a + b;
end

int x = sum(1, 2);
```

# If and While
_**Conditions in NoClueLang are basically int expressions.**_

- 0 means *false*.

- everything else means *true*.

Honestly, here's just an example
```
if x > 0 do
  ...
elseif x < 0 do
  ...
else
  ...
end

```

You can also make while loops
```
while x < 4 do
  ...
end
```

You can at any point break out of a while loop using the 'break' keyword!

_( no 'continue' feature because I was lazy )_

```
break;
```

> In NoClueLang, there aren't any 'for' or 'do while' and whatnot.
# Collections
Collections in NoClueLang are lists of functions under a title.

Here are some examples
```
float min = Math::min(3, 2);
string c = String::charAt('Hello, World', 0);
```

_*list of all collections and their functions later._
# Arrays
**In NoClueLang, Arrays don't exist!**

You can instead simulate an array like this
```
string int_array_id    = Array::inew(); ~ This function makes a new INT    array ~
string float_array_id  = Array::fnew(); ~ This function makes a new Float  array ~
string string_array_id = Array::snew(); ~ This function makes a new String array ~
``` 
Here are some array functions worth mentioning here
```
Array::len(<arrayID>)
  - returns the length of the given array

Array::type(<arrayID>)
  - returns the type of the given array

Array::remove(<arrayID>, <index>)
  - removes the element at <index> of the given array

Array::splice(<arrayID>, <index>, <n>)
  - removes <n> elements starting from <index> of the given array

Array::alloc(<arrayID>, <len>)
  - resizes (if needed) the given array to store <len> elements
```

Other array methods are specific to each array type
```
Array::ipush(<arrayID>, <intElement>)
Array::fpush(<arrayID>, <floatElement>)
Array::spush(<arrayID>, <stringElement>)

Array::iinsert(<arrayID>, <index>, <intElement>)
Array::finsert(<arrayID>, <index>, <floatElement>)
Array::sinsert(<arrayID>, <index>, <stringElement>)

Array::iset(<arrayID>, <index>, <intValue>)
Array::fset(<arrayID>, <index>, <floatValue>)
Array::sset(<arrayID>, <index>, <stringValue>)

Array::iget(<arrayID>, <index>) -> int
Array::fget(<arrayID>, <index>) -> float
Array::sget(<arrayID>, <index>) -> string
```
> These are all the functions inside the 'Array' collection!
## Other collections and their functions
