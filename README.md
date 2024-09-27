### NoClueLang by NoClueBruh
**Most stuff in NoClueLang work a little different compared to other languages (that actually good)!**

> This language was made without the use of any tutorial or any research at all.
> 
> And it really shows, just take a look at the source code

<sup> NoClueBruh not responsible for any harm caused </sup>

# Types 
There are 3 types supported by NoClueLang and they are the following;
- _int_
- _float_
- _string_
 
> [!IMPORTANT]
> Yep, there aren't any booleans because I was lazy!
>
>  more on that later!!!

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
> [!WARNING]
> Keep in mind that you can only assign values to variables that way!
>
> That means no +=, -=, *=, ++ etc

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

```
break;
```

> [!WARNING]
> I was too lazy to add the continue thing

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
> [!CAUTION]
> **In NoClueLang, Array types don't exist!**

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
> Keep in mind that you can easily add your own functions and collections through the source code!

### Math Collection
```
Math::getE()  -> float
  - returns that cool constant everyone keeps talkin about

Math::getPI() -> float
  - returns that really cool constant everyone and their mom keep talkin about

Math::abs(<floatValue>) -> float
  - returns the absolute value of <floatValue>

Math::sin(<floatValue>) -> float
  - returns the sine of the angle <floatValue> rad

Math::cos(<floatValue>) -> float
  - returns the cosine of the angle <floatValue> rad

Math::tan(<floatValue>) -> float
  - returns the tangent of the angle <floatValue> rad

Math::max(<floatValue0>, <floatValue1>) -> float
  - returns the largest float given

Math::min(<floatValue0>, <floatValue1>) -> float
  - returns the smallest float given

Math::round(<floatValue>) -> int
  - returns <floatValue> rounded to a int

Math::ceil(<floatValue>) -> int
  - returns the smallest int greater or equal to <floatValue> 

Math::floor(<floatValue>) -> int
  - returns the largest int less or equal to <floatValue> 
```

### String Collection
```
String::equals(<stringA>, <stringB>) -> int
  - returns 1 if the strings match or 0 if they don't!

String::len(<stringValue>) -> Int
  - returns the length of <stringValue>

String::splice(<stringValue>, <index>, <len>)
  - removes <len> characters from <stringValue> starting at <index>

String::charAt(<stringValue>, <index>) -> string
  - returns the character from <stringValue> at <index>

String::charCodeAt(<stringValue>, <index>) -> int
  - returns the character code from <stringValue> at <index>
```

### IO Collection
```
IO::print(<message>)
  - prints <message> to the console

IO::println(<message>)
  - prints <message> with a new line to the console

IO::readLine() -> string
  - reads user input ( 300 character limit because why not )

IO::readChar() -> string
  - reads a single character from the user ( doesn't require the user to hit enter if you're using windows )
```

### Sys Collection
```
Sys::toInt(<stringValue>) -> int
  - returns the int represented by <stringValue>

Sys::toFloat(<stringValue>) -> float
  - returns the float represented by <stringValue>
```

# Other information and how to interp a file
## Expressions
Expressions in NoClueLang have the following available operators
- *Basic operators*:    '+',  '-',  '*',  '/',  '^',  '%'
- *Compare operators*:  '>',  '>=',  '==',  '!=',  '<=',  '<'

> In NoClueLang, there doesn't exist a 'not' operator ( that '!' that flips booleans )
>
> Instead, you can do 1 - int!

**You can also cast expressions!**

( _sort of_ )
- You can cast any expression to a string expression by adding the 'string' keyword at the start like this
```
string idk = string (2 + 1);
```
- You can also case a int expression to a float expression the same way
```
float num = float 2;
```
> [!IMPORTANT]
>**You can only cast expressions to certain types**
>
>- If you want to cast a float expression to a int expression, you'll need to use Math::floor, Math::Ceil or Math::round
>- Casting a string expression to a int or float expression won't make any difference


## Editor
Use whatever editor you want but I made a cool little language setup for Notepad++!

The language file is inside the 'other' folder!
Take a look at it:>

![image](https://github.com/user-attachments/assets/a8458c4a-cc56-485c-950e-bb64641905cf)

_not sure if thats the best solution for the FizzBuzz thingie_

## How to interp a file!
- *First step* : Compile the project with your gcc and whatnot
- *Second step*: Run it with an argument that is the location of the file you wish to interp
- *Third step* : yeah thats it
