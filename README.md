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
- Conditions in NoClueLang are basically int expressions.

- If during Run-Time that expression is equal to 0, it means *false*.

- Otherwise, if that expression isn't equal to 0, it means *true*.

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

In NoClueLang, there aren't any 'for' or 'do while' and whatnot.
