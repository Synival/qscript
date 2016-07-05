# *qscript*:

***(qscript is currently under development.  Not all features listed in this document may be currently implemented.  To see project status, please read 'STATUS.md'.)***

---

##Table of Contents:

- [What is *qscript*?](#what-is-qscript)
- [How *qscript* works](#how-qscript-works)
- [Using *qscript*](#using-qscript)
- [Why make this?](#why-make-this)
- [Scripting language](#scripting-language)
- [`qscript` standard library](#qscript-standard-library)
- [Formal grammar](#formal-grammar)
- [Internal stuff](#internal-stuff)
- [Pre-defined values](#pre-defined-values)
- [Glossary of terms](#glossary-of-terms)

---

# What is *qscript*?

This library is designed to create a persistent-state world of generic objects with arbitrary code injection/ejection, inter-object relationships that automatically update, and a sensible interface between the scripting environment and your project.

***qscript* is useful for projects that need to do things like:**

1. Create a persistent game world with thousands of objects over an indefinite amount of time with guaranteed data consistency
2. Accurately and efficiently maintain the positions of elements in a web document using a complex stylesheet
3. Create dynamic polymorphic objects with no strict definition

---

# How *qscript* works:

To use *qscript*, your project must first define a *scheme* that will create a *world* which contains *resources* and *objects*.

*Resources* are scripts defined by files using the ".qs" extension.  *qscript* uses its own scripting language designed specifically to accomplish the project's goals.  It strives to be a functional language to ensure consistent results and employ a minimal set of grammatical rules to maintain simplicity.

*Objects* are containers for *properties* and are instantiated at runtime.  Their properties are defined through a chain of *rlinks* (links to resources) that can be modified and rearranged dynamically.

All object property modifications are non-destructive and pushed onto a stack so they can be safely reverted.  Rlink chains are modified by *unwinding* (rolling back) to the point of modification, *injecting* (inserting) or *ejecting* (removing) the desired script, then *rewinding* (re-executing scripts that have been unwound).  Rlinks in the *rlink chain* are ordered by their *priority* (an arbitrary integer value) from lowest to highest.

The *world* tracks all object *influences* (properties/variables read from anything other than itself) and ensures that influenced objects update properly when the external value is changed.

---

# Using *qscript*:

- [Sample object "player"](#sample-object-player)
- [Player's scripts](#players-scripts)
- [Implementation](#implementation)
- [Running *objdump*](#running-objdump)

**The resources and C code below can be found in the example program "examples/objdump" in this repository.**

In the following tutorial, we'll be creating a program that instantiates a player object as the groundwork for a tiny RPG.  The player will have some base stats, a class, some equipment, and some buffs/debuffs attached from the start.

## Sample object "player":

Here is our desired rlink chain:

 Priority | Resource         | Effect
----------|------------------|----------------------------------------
 0        | `player_base`    | Define base values for all properties
 10       | `class_wizard`   | Modify stats and abilities to turn this character into a wizard
 20       | `purple_robe_on` | Change appearance and stats
 20       | `magic_wand_on`  | Increase strength and grant a new ability
 30       | `buff_invisible` | (Temporary) modify appearance and increase sneaking to 100%
 40       | `too_hot`        | (Applied from environment) Character is slower and dumber

Before implementing *qscript* in your project, let's define all of the resources listed above.

## Player's scripts:

(from 'examples/objdump/my_resources.qs'):

```
player_base {
   # set base properties.
   = (.str, 10.00);      # strength
   = (.def, 10.00);      # defense
   = (.int, 10.00);      # intelligence
   = (.agi, 10.00);      # agility
   = (.class, "none");   # we don't have a class yet.
   = (.color, "beige");  # character is super bland.
   = (.skills, []);      # our skills is an empty list.
   = (.sneak, 0.00);     # we're not very sneaky.
}

class_wizard {
    = (.class, "wizard");       # define our class:
   *= (.str, 0.50);             # weak,
   *= (.def, 0.50);             # fragile,
   *= (.int, 2.00);             # but super smart,
   += (.skills, "spell_fire");  # and with a spell.
}

purple_robe_on {
    = (.color, "purple");  # we're purple now.
   += (.def, 5.00);        # small buff to defense,
   += (.int, 10.00);       # and a big one to intelligence.
}

magic_wand_on {
   += (.str, 3.00);                      # slight strength increase,
   if (== (.class, "wizard"),            # if we're a wizard...
      += (.skills, "spell_invisible"));  # ...add a new spell.
}

buff_invisible {
   = (.color, "clear");  # can't see us!
   = (.sneak, 100.00);   # 100% sneaking
}

too_hot {
   *= (.agi, 0.75);  # 75% agility
   *= (.int, 0.75);  # 75% intelligence
}
```

To create this object, we'll need to:

1. create a scheme for our qscript world,
2. instantiate a blank object,
3. inject rlinks into the object's rlink chain, and
4. update our world to finish our object.

## Implementation:

(from 'examples/objdump/main.c'):

```
/* create our scheme, load its resources, and update changes. */
qs_scheme_t *scheme = qs_scheme_new ();
qs_parse_file (scheme, "my_resources.qs");
qs_scheme_update (scheme);

/* instantiate our player and add rlinks to its resource chain. */
qs_object_t *obj = qs_object_new (scheme, "player");
qs_rlink_inject (obj, qs_resource_get (scheme, "player_base"),    0);
qs_rlink_inject (obj, qs_resource_get (scheme, "class_wizard"),   10);
qs_rlink_inject (obj, qs_resource_get (scheme, "purple_robe_on"), 20);
qs_rlink_inject (obj, qs_resource_get (scheme, "magic_wand_on"),  20);
qs_rlink_inject (obj, qs_resource_get (scheme, "buff_invisible"), 30);
qs_rlink_inject (obj, qs_resource_get (scheme, "too_hot"),        40);

/* update changes again so 'obj' is run. */
qs_scheme_update (scheme);

/* (do everything we want with our object) */
...

/* clean-up time!  free all objects, resources, and the scheme itself. */
qs_scheme_free (scheme);
```

Executing the test program 'objdump' via 'objdump.sh' will describe the resulting object like so:

## Running *objdump*:

```
> ./objdump.sh

player:
   str    (float)  = 8
   def    (float)  = 10
   int    (float)  = 22.5
   agi    (float)  = 7.5
   class  (string) = "wizard"
   color  (string) = "clear"
   skills (list)   = ["spell_fire", "spell_invisible"]
   sneak  (float)  = 100
```

---

# Why make this?

One of my favorite type of games to make are text-based multiplayer RPGs called MUDs (multi-user dungeons), which are early forerunners to MMOs.  These are reasonable projects to tackle because you don't need to make a million assets and the game runs 100% server-side.  For one MUD I was working on, my goal was to have a persistent world with as many of its resources put into text files as possible so I could define new monsters, classes, and abilities without hard-coding anything in.  The purpose of that was to make edits quick and easy without recompilation and allow hot-loading so the server would never have to reboot.  If I wanted to add stuff in or make changes, I could log into the server, load the new files, and play right away.  Neat!

Now, if the server was going to run potentially for months at a time, small bugs here and there would break objects, maps, and (worst of all) players, requiring some hands-on fixes when the world state gets corrupted.  MMOs are very complex and only get bigger over time, so as the world and player base grows, the likelihood that something will get bugged-out is off the charts and such bugs would be difficult if not impossible to track.  There are too many examples to list, but here are a few anyway:

1. Buffs/debuffs not turning themselves off properly,
2. Temporary stat modifiers not reverting correctly,
3. Triggers for quests not activating or deactivating,
4. Puzzles not resetting properly,
5. Item duplication or destruction bugs,

...and so on.

*qscript* aims to avoid these potential problems by developing a layer of object management that only performs non-destructive object modifications, eliminating the possibility of object state corruption.  Any change made to an object must be guaranteed to be revertible, which means all property modifications must be pushed onto a stack that can be safely popped later.  It works like this: If a player acquires a *Quad Damage* buff, it will push a small script that multiplies damage output by 4.  When *Quad Damage* wears off, that script and its modifications are popped, reverting damage output back to normal.

In addition to preventing state corruption, *qscript* also strives to ensure that multiple buffs would applied in a consistent, predictable order.  Let's say there's a buff called *Extra Damage* that adds 10 to your character's damage output.  If *Quad Damage* is also applied, which executes first?  For a character that inflicts 25 damage per hit, the script order makes a big difference for the resulting damage:

1. *Quad Damage*, then *Extra Damage*: `damage = ((25 * 4) + 10) = 110`
2. *Extra Damage*, then *Quad Damage*: `damage = ((25 + 10) * 4) = 140`

Let's assume the first behavior is desired.  To make sure this happens, each script is given a number called a *priority* that determines its order of execution.  *Quad Damage* has a lower priority than *Extra Damage* which means its effects are always applied first.

(keep writing...)

---

# Scripting language:

- [Values, actions, and function calls](#values-actions-and-function-calls)
- [Simple real primitives](#simple-real-primitives)
- [Complex real primitives](#complex-real-primitives)
- [Abstract primitives](#abstract-primitives)
- [Examples](#examples)

*qscript*'s scripting environment strives to stay as grammatically simple as possible while remaining flexible and powerful.  The language is used to define *resources* and their *scripts* in the form of a single code *block*.  Each resource can be:

1. instantiated as the base of an *object*,
2. *injected* into an object's *rlink chain* as an *rlink*, or
3. automatically instantiated immediately after compilation as a *global object*.

At first glance, a simple resource definition resembles that of a C-family programming language:

```
main {
   echo ("Hello, world!");
   return (0);
}
```

There are, however, some important distinctions.  There are no built-in procedures for code flow or math; each of these features are defined as functions that need to be invoked with a typical function's syntax.  Notice the `return (0);` line above.  Same same applies for functions like `if()`, `for()`, `+()`, `==()`, and `=()`.  For specific usage on these functions, please refer to the ***qscript* standard library** section.

## Values, actions, and function calls:

All code is composed of *values* followed by zero or more *actions*.  When a value is encountered, it is first *evaluated*, a process which dereferences variables, properties, and executes blocks until a *real primitive* is found.  If a value has an action, the evaluation is inputted into that action whose evaluation is fed into the next action and so on.  The final evaluation is then returned to the value's caller.

The most common action is a *function call*, which interprets its input as a function name and attempts to execute it.  Function calls are declared with a set a parentheses containing a comma-separated *list* of values that are passed to the function as *parameters*.  A function name can be any string matching either an internal function or a resource.

The following are all valid function calls that produce the same result:

```
# Each evaluation will print "foo" followed by a newline.

echo           ("foo");  # "echo" is a string matching a function.
"echo"         ("foo");  # This is equally valid.
+ ("ec", "ho") ("foo");  # +() returns "echo" which is then called by the next action.
{ "echo"; }    ("foo");  # This block will return "echo" which is also called.
if (1, "echo") ("foo");  # if() returns the matching case, which is "echo".
```

## Simple real primitives:

All values are ultimately stored as *real primitives*.  Real primitives are classified as either *simple* or *complex*.  Simple primitives contain all their data directly in the value structure `qs_value_t` and require no extra processing.

 Type          | Description                                              | Declaration
---------------|----------------------------------------------------------|-------------
 **undefined** | Placeholder type for uninitialized variables and errors. | `undefined`
 **string**    | Any text value.                    | `string`, `"string"`, `'string'`
 **int**       | Integer value.                                           | `[+/-]123456`
 **float**     | Floating-point decimal value.                            | `[+/-]123456.789`

**Note:** Single or double-quoted strings can be combined into a single string thusly:

```
echo ("This " "is " "one " 'big ' 'string ');
```

## Complex real primitives:

- [char](#char)
- [list](#list)
- [object](#object)

Complex primitives have unique characteristics that may contain additional processing or memory allocation.

 Type       | Description                                                | Declaration
------------|------------------------------------------------------------|-------------
 **char**   | Single character.                                          | `'a'`
 **list**   | Zero or more values that can be indexed.                   | `[]`, `[1, 2, 3]`
 **object** | Reference to an instantiated object for property look-ups. | `@object`, ``@@global-object``

### char:

The *char* primitive can be modified by mathematical operations as if it's ASCII character value.  They can also refer to a character belonging to a string indexed by `[<value>]` just like a C-string.  In that case, modifying the value will update the string it belongs to.

```
+ ('a', 1);          # evaluates to 'b'
- ('a', 32);         # evaluates to 'A'

= ($string, "wow");  # define a string 'wow'...
= ($string[0], 'c'); # ...and turn it into 'cow'
```

### list:

The *list* primitive can be indexed by `[<value>]` in the same way as a string.  In that case, modifying the value will update the list it belongs to.  Lists can also have their contents used as function call parameters with a preceding `~` character.  This is called *unfolding*.

```
= ($list, [1, 2, 3]);      # A list with three numbers.
= ($math, + (~$list));     # Same as `= ($math, + (1, 2, 3));`
echo ($math);              # prints '6'

= ($list[2], 30);          # List becomes `[1, 2, 30]`
echo (+ (~$list));         # prints '33'
```

### object:

The *object* primitive points to an object by either its name or unique ID.  When evaluated, no lookup is performed directly - only by the action that requires it.  If the object cannot be found, the action will typically return the `<no object>` error.

The primary purpose of interacting with objects is to read their *properties*.  Properties can only be read from other objects, not modified.  For more information on properties, refer to **Abstract Primitives** -> **properties**.

```
@my_object {               # Instantiate resource with the name '@my_object'.
   = (.color, "blue");     # Set property 'color' to 'blue'.
   = (.shape, "sphere");   # Set property 'shape' to 'sphere'.
}

copy_object {              # This script, when injected, will copy properties from @my_object.
   = ($obj, @@my_object);  # The '@' refers to an object named '@my_object'.
   = (.color, $obj.color); # Match color of object.
   = (.shape, $obj.shape); # Also match our shape.

   = ($obj.shape, "box");  # @my_object.shape is read-only; this will report an error.
}
```

## Abstract primitives:

Values can also contain primitives that require execution or referencing during evaluation.  These are called *abstract primitives*.

(continue writing this)

## Examples:

- [Injection](#injection)
- [Utility functions](#utility-functions)
- [Objects](#objects)

### Injection:

Resources injected into objects will typically modify properties.  The conditions of those modifications can 

```
# Multiply strength by 4.
buff_quad_damage {
   *= (.str, 4);
}

# Make left-handed people right-handed and vice-versa.
switch_handedness {
   if (==(.handed, "left"),  =(.handed, "right"), # left  ==> right
       ==(.handed, "right"), =(.handed, "left")); # right ==> left
}

# Change our color to match the color of the room.
buff_chameleon {
   # Property '.room' refers to an object such as:
   #    @room_jungle_entrance

   = (.color, .room'.color);
}
```

### Utility Functions:

These examples are designed to showcase various forms of loops.

```
# fibonacci_for (n):
#    Calculate and return the nth Fibonacci number using a for() loop.
fibonacci_for {
   args ($n);
   if (<= ($n, 2),
      return (1));

   = ($prev,    1);
   = ($current, 1);
   for (= ($i, 2), < ($i, $n), ++($i), {
      =  ($tmp, $prev);
      =  ($prev, $current);
      += ($current, $tmp);
   });
}

# echo_indented (space_count, string):
#    Echo a string with preceeding spaces.
echo_indented {                                   # Takes (int, string).
   args ($indent, $string);                       # Assign arguments to variables.
   = ($istr, "");                                 # Initialize indented spaces.
   for (= ($i, 0), < ($i, $indent), ++ ($i),      # Do this $length times:
      += ($istr, ' '));                           # Append a space to $istr
   return (echo ($istr, $string));                # Echo/return string w/ indents.
}

# echo_doge ():
#    Echo a meme, more nicely-formatted than it deserves to be.
echo_doge {
   = ($my_string, "so string, very tokens, wow"); # Let's tokenize a meme.
   = ($count, 0);                                 # Keep track of indentation.
   for_each (tokenize ($my_string), $token, {     # Tokenize.
      -= ($token[0], 32);                         # Capitalize the first letter.
      echo_indented (* (2, $count), $token);      # Print our token with indents.
      ++ ($count);                                # Increment counter.
   });
}

# fibonacci_recurse (n):
#    Calculate and return the nth Fibonacci number using recursion.
fibonacci_recurse {
   args ($n);
   if (<= ($n, 1), $n,                    # Stop at zero and one.
      + (                                 # Otherwise, return:
         fibonacci_recurse (- ($n, 1)),   #   f(n-1) + f(n-2)
         fibonacci_recurse (- ($n, 2))
      )
   );
}
```

## Objects:

```
# Base object used for instantiation of characters.
character_base {
   # Base values modified by class and level later.
   = (.str,    10);
   = (.def,    10);
   = (.agi,    10);
   = (.max_hp, 50);

   # Some default values that will need additional scripts.
   = (.class,  "Unemployed");
   = (.race,   "Being");
   = (.gender, "Neutral");

   # Variables assigned by an object variable so they can be modified permanently.
   = (.level,  %level);
   = (.exp,    %exp);
   = (.gold,   %gold);
}

# Global object that stores useful information.
@world {
   # Get our global time (in hours).
   = ($time, %%time);

   # Set a "time of day" phrase for everything to use.
   = (.time_of_day,
      if (< ($time,  1.00), "midnight",
          < ($time,  5.00), "late night",
          < ($time,  6.00), "dawn",
          < ($time,  9.00), "morning",
          < ($time, 12.00), "forenoon",
          < ($time, 13.00), "noon",
          < ($time, 16.00), "afternoon",
          < ($time, 19.00), "evening",
          < ($time, 22.00), "dusk",
                            "night"));
}
```

---

# *qscript* standard library:

- [Text output](#text-output)
- [Math / logical](#math--logical)
- [Informative](#informative)
- [Assigning / modifying](#assigning--modifying)
- [Code flow](#code-flow)
- [Argument processing](#argument-processing)
- [Casting](#casting)
- [Object manipulation](#object-manipulation)

(coming later!)

## Text output:

1. `print`
2. `echo`
3. `print_resource`
4. `print_value`

## Math / logical:

1. `+`
2. `-`
3. `*`
4. `/`
5. `**`
6. `|`
7. `&`
8. `^`
9. `%`
10. `==`
11. `!=`
12. `>`
13. `>=`
14. `<`
15. `<=`
16. `||`
17. `&&`
18. `^^`
19. `!||`
20. `!&&`
21. `!^^`
22. `multi`

## Informative:

1. `?`
2. `!`
3. `inv`
4. `type`
5. `scope`
6. `length`
7. `this`
8. `id`
9. `name`
10. `func_exists`
11. `tokenize`

## Assigning / modifying:

1. `+=`
2. `-=`
3. `*=`
4. `/=`
5. `**=`
6. `|=`
7. `&=`
8. `^=`
9. `%=`
10. `=`
11. `++`
12. `--`
13. `vars`

## Code flow:

1. `if`
2. `return`
3. `break`
4. `continue`
5. `while`
6. `for`
7. `switch`
8. `run`
9. `eval`
10. `for_each`

## Argument processing:

1. `args`
2. `arg`
3. `arg_list`

## Casting:

1. `int`
2. `string`
3. `float`
4. `char`
5. `object`
6. `variable`
7. `property`

## Object manipulation:

1. `new`

---

# Public macros:

- [Definitions](#definitions)
- [Inside `qs_func` call](#inside-qs_func-call)

(coming later!)

## Definitions:

1. `QS_FUNC`
2. `QS_STACK_FUNC`
3. `QSV_DEFINE`

## Inside `qs_func` call:

1. `QS_ARGV`
2. `QS_ARGS`
3. `QS_ARGF`
4. `QS_ARGI`
5. `QS_ARGL`
6. `QS_ARGO`
7. `QS_ARG_ERROR`
8. `QS_RETS`
9. `QS_RETF`
10. `QS_RETI`
11. `QS_RETC`
12. `QS_RETURN`
13. `QS_BREAK`
14. `QS_CONTINUE`

---

# Public functions:

(coming later!)

- `qs_arg_float`
- `qs_arg_int`
- `qs_arg_string`
- `qs_arg_value`
- `qs_arg_list`
- `qs_arg_value`
- `qs_execute_get_block`
- `qs_execute_get_call`
- `qs_execute_pop`
- `qs_execute_push`
- `qs_func_break`
- `qs_func_continue`
- `qs_func_error`
- `qs_func_return`
- `qs_func_run`
- `qs_list_copy`
- `qs_list_free`
- `qs_list_internalize`
- `qs_list_new`
- `qs_list_value`
- `qs_object_free`
- `qs_object_get`
- `qs_object_get_by_id`
- `qs_object_instantiate`
- `qs_object_new`
- `qs_parse_content`
- `qs_parse_init`
- `qs_parse_file`
- `qs_print_action`
- `qs_print_list`
- `qs_print_resource`
- `qs_print_value`
- `qs_property_get`
- `qs_property_value`
- `qs_resource_get`
- `qs_resource_get_auto_instance`
- `qs_resource_get_by_id`
- `qs_return_char`
- `qs_return_float`
- `qs_return_int`
- `qs_return_string`
- `qs_return_value_new`
- `qs_rlink_is_child_of`
- `qs_rlink_pop`
- `qs_rlink_push`
- `qs_rlink_push_at`
- `qs_rlink_unwind`
- `qs_rlink_wind`
- `qs_scheme_cleanup`
- `qs_scheme_free`
- `qs_scheme_funcs`
- `qs_scheme_get_func`
- `qs_scheme_heap_value`
- `qs_scheme_new`
- `qs_scheme_update`
- `qs_value_can_modify`
- `qs_value_cleanup`
- `qs_value_cleanup_self`
- `qs_value_contains`
- `qs_value_copy`
- `qs_value_copy_const`
- `qs_value_init`
- `qs_value_length`
- `qs_value_list`
- `qs_value_list_internalize`
- `qs_value_lvalue`
- `qs_value_object`
- `qs_value_property`
- `qs_value_read`
- `qs_value_restring`
- `qs_value_truth`
- `qs_value_type`
- `qs_value_update_from_string`
- `qs_value_variable`
- `qs_variable_contains`
- `qs_variable_get`
- `qs_variable_value`

---

# Formal Grammar:

## Root:

```
(<resource>)*
```

## Comments:

The character `#` can be used when any value, resource, or action is expected.
All text following `#` will be ignored by the parser.

## `<resource>`:

```
[@]<string> <block>
```
**Defines a resource named `<string>` containing code `<block>`.**

The '@' character indicates that the resource is to be automatically
instantiated as an object named '@resource-name'.

## `<block>`:

```
'{'
   <value>1;   # Examples:
   <value>2;   #    = ($variable, 10);
   ...         #    echo ("My variable is: ", $variable);
   <value>n;   #    $variable;  <--- Last value is always returned
'}'
```
**Creates a series of zero or more values for evaluation.**

Block contents are immediately evaluated whenever they are referenced by
values or call actions.  They return the last evaluation, which includes the
value passed to `return()`, `break()`, or `continue()`.  If the block
contains zero values, `QSV_UNDEFINED` is returned.

## `<value>`:

```
[~]<primitive> <action>1 ... <action>n
```
**Universal symbol for literal primitives, function calls, and other actions.**

The '~' indicates that the result is to be *unfolded* into the action from
which it is called.  This requires the evaluation to be of type `list`,
otherwise it will report an error.  The contents of the list will be inserted
into the parameter list of the action from which it was called.

## `<primitive>`:

*Matches the first of the following, starting from the top:*
```
<int>
<float>
<variable>
<property>
<list>
<block>
<char>
<object>
<undefined>
<string>
```

### `<int>`:

*Regular expression*: `[-+]?[0-9]+` 
(One or more digits optionally preceded by '-' or '+'.)

**Creates an integer value.**

### `<float>`:

*Regular expression*: `[-+]?[0-9]+\.[0-9]+` 
(One or more digits optionally preceded by '-' or '+', followed by a decimal
point and one or more digits.)

**Creates a floating-point value.**

### `<variable>`:

*Regular expression*: `[\\$]+[a-zA-Z0-9_]+` 
(One or more `$` characters followed by one or more alphanumeric/underscore
characters.)

**Creates a reference to a block- or rlink-scope variable.**

A single `$` refers to a *block scope* variable.  Two (`$$`) characters refers
to an *rlink scope* variable.
If the variable is not found in the current scope, it will be created.

### `<property>`:

```
.<value>[']
```

**Creates a reference to a property belonging to the current object's.
Can be used as an `<action>`, in which case it creates a reference to a property
belonging to the evaluated parent value.**

The optional quote acts as a closing parenthesis, allowing actions to
be called on the evaluation of the property.
If omitted, all proceeding actions will be considered belonging to `<value>`.

If called as an action, an error will be reported if the evaluation of the
parent value is not of type `<object>`.

### `<list>`:

```
'[' <value>1, <value>2, ... <value>n ']'
```

**Creates a list of zero or more values.**

Values contained in the list are not evaluated until they are indexed.

### `<char>`:

*Regular expression*: `'.'` 
(Any single character surrounded by single quotes.)

### `<object>`:

*Regular expression*: `[@]+[a-zA-Z_]+` 
(One or more `@` characters followed by one or more alphanumeric/underscore
characters.)
A single `@` refers to externally instantiated objects.  Two (`@@`) refers to
an auto-instantiated object from a resource.

### `<undefined>`:

*Exact string match*: `undefined`

**Evaluates to **`QSV_UNDEFINED`**.  Does not create a new value.**

### `<string>`:

## `<action>`:

*Matches the first of the following, starting from the top:*

```
<call>
<index>
<property>
```

### `<call>`:

```
(<value>1, <value>2, ... <value>n)
```

**Executes a resource, function, or list, supplying zero or more parameters.**

Resources are attempted to be evaluated first.  Any resource can be executed,
including auto-instantiated resources.  Parameters are read as arguments via
the `arg()`, `args()`, and `arg_list()` qfuncs.

Any function supplied by the scheme, including the standard, will then be
executed.

If the parent evaluation is type `<list>`, it is considered a *lambda
functions* and all list elements will be evaluated as if they are a resource.
Internal calls to `arg()`, `args()`, and `arg_list()` will pull arguments from
the lambda function's parameter list.  It is recommended that list elements be
defined as `<block>`s for the sake of formatting consistency.

### `<index>`:

```
'[' <value> ']'
```

**Returns an element of a **`<list>`** or a single character in a **`<string>`
**.**

Indexes are considered *lvalues* and can be assigned with any assignment
function.  However, indexing `<string>`s is very strict; the indexed value
will always be of type `<char>` and only values valid for assignment must
safely cast to type `<char>`.

### `<property>`:

(See `<property>` definition under `<primitive>`.)

---

# Internal stuff:

(coming later!)

## Structures:

1. `qs_object_t`
2. `qs_value_t`
3. `qs_resource_t`
4. `qs_rlink_t`
5. `qs_scheme_t`
6. `qs_func_t`
7. `qs_variable_t`
8. `qs_execute_t`
9. `qs_list_t`
10. `qs_action_t`
11. `qs_stack_t`
12. `qs_property_t`
13. `qs_modify_t`

## Enumerated values:

### Tags:

1. `QSCRIPT_INVALID`
2. `QSCRIPT_UNDEFINED`
3. `QSCRIPT_ROOT`
4. `QSCRIPT_COMMENT`
5. `QSCRIPT_RESOURCE`
6. `QSCRIPT_RFLAGS`
7. `QSCRIPT_RNAME`
8. `QSCRIPT_BLOCK`
9. `QSCRIPT_LIST`
10. `QSCRIPT_OUTER_LIST`
11. `QSCRIPT_VALUE`
12. `QSCRIPT_VFLAGS`
13. `QSCRIPT_PRIMITIVE`
14. `QSCRIPT_OUTER_BLOCK`
15. `QSCRIPT_SEPARATOR`
16. `QSCRIPT_STRING`
17. `QSCRIPT_SIMPLE_STRING`
18. `QSCRIPT_COMPLEX_STRING`
19. `QSCRIPT_NUMBER`
20. `QSCRIPT_FLOAT`
21. `QSCRIPT_INT`
22. `QSCRIPT_VARIABLE`
23. `QSCRIPT_ACTION`
24. `QSCRIPT_CALL`
25. `QSCRIPT_INDEX`
26. `QSCRIPT_CHAR`
27. `QSCRIPT_OBJECT`
28. `QSCRIPT_PROPERTY`

### Action types:

1. `QS_ACTION_CALL`
2. `QS_ACTION_INDEX`
3. `QS_ACTION_PROPERTY`

### Variable scopes:

1. `QS_SCOPE_AUTO`
2. `QS_SCOPE_RLINK`
3. `QS_SCOPE_BLOCK`

### Execution state types:

1. `QS_EXE_LAMBDA`
2. `QS_EXE_FUNC`
3. `QS_EXE_RESOURCE`
4. `QS_EXE_BLOCK`
5. `QS_EXE_LOOP`

---

# Pre-defined values:

 Name                   | Type              | Value
------------------------|-------------------|-----------------------
`QSV_ZERO`              | int               | `0`
`QSV_ONE`               | int               | `1`
`QSV_INVALID_TYPE`      | undefined         | `"<invalid_type>"`
`QSV_NOT_VARIABLE`      | undefined         | `"<not_variable>"`
`QSV_NOT_PROPERTY`      | undefined         | `"<not_property>"`
`QSV_CANNOT_MODIFY`     | undefined         | `"<cannot_modify>"`
`QSV_NOT_FUNC_CALL`     | undefined         | `"<not_func_call>"`
`QSV_INVALID_INDEX`     | undefined         | `"<invalid_index>"`
`QSV_NO_INDEX`          | undefined         | `"<no_index>"`
`QSV_INVALID_SUB_FUNC`  | undefined         | `"<invalid_sub_func>"`
`QSV_NOT_ENOUGH_ARGS`   | undefined         | `"<not_enough_args>"`
`QSV_INVALID_FUNC`      | undefined         | `"<invalid_func>"`
`QSV_INVALID_OPER`      | undefined         | `"<invalid_oper>"`
`QSV_DIVISION_BY_ZERO`  | undefined         | `"<division_by_zero>"`
`QSV_MOD_BY_ZERO`       | undefined         | `"<mod_by_zero>"`
`QSV_INVALID_RESOURCE`  | undefined         | `"<invalid_resource>"`
`QSV_CANNOT_EXECUTE`    | undefined         | `"<cannot_execute>"`
`QSV_CANNOT_UNWRAP`     | undefined         | `"<cannot_unwrap>"`
`QSV_SCOPE_LITERAL`     | string            | `"literal"`
`QSV_SCOPE_RLINK`       | string            | `"rlink"`
`QSV_SCOPE_BLOCK`       | string            | `"block"`
`QSV_SCOPE_PROPERTY`    | string            | `"property"`
`QSV_SCOPE_UNKNOWN`     | string            | `"unknown"`
`QSV_UNDEFINED`         | undefined         | `"<undefined>"`
`QSV_INVALID_VALUE`     | undefined         | `"<invalid_value>"`
`QSV_NO_OBJECT`         | undefined         | `"<no_object>"`
`QSV_ALREADY_WOUND`     | undefined         | `"<already_wound>"`
`QSV_INVALID_PROPERTY`  | undefined         | `"<invalid_property>"`

---

# Glossary of Terms:

***scheme***:

***world***:

***resource***:

***object***:

***rlink***:

***rlink chain***:

***priority***:

***injecting***:

***ejecting***:

***winding***:

***unwinding***:

***rewinding***:

***influence***:
