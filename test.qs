# Test a lot of stuff!
main {
   # print_resource ();

   # Proper echo() behavior with some strings, numbers, and statements.
   echo ("Numbers and addition vs. concatenation tests:");
   echo ("   nums: ", 1, ", ", 2.00, ", ", -3.250);
   echo ("   add:  ", + (1, 2, 3));
   echo ("   cat:  ", + ("", 1, 2, 3));
   echo ("   cat2: ", + ("test", "1", "2", "4", ** (2, 3)));
   echo ();

   # Math evaluation demonstration for integers vs. floating points.
   echo ("Integer vs. floating point math tests:");
   echo ("   7    * 11   = ",  * ( 7,   11));
   echo ("   7    * 11.5 = ",  * ( 7,   11.5));
   echo ("   7.1  * 11   = ",  * ( 7.1, 11));
   echo ("   7.1  * 11.5 = ",  * ( 7.1, 11.5));
   echo ("   8   ** 2    = ", ** ( 8,   2));
   echo ("   8   ** 2.5  = ", ** ( 8,   2.5));
   echo ("   8.0 ** 2    = ", ** ( 8.0, 2));
   echo ("   8.0 ** 2.5  = ", ** ( 8.0, 2.5));
   echo ("   13   % 5    = ",  % (13,   5));
   echo ("   13   % 5.9  = ",  % (13,   5.9));
   echo ("   13.0 % 5.9  = ",  % (13.0, 5.9));
   echo ();

   # Test standard logic.
   echo ("Comparison functions:");
   echo ("   a == b: ", == (a, b), " | 1 == 2: ", == (1, 2));
   echo ("   a != b: ", != (a, b), " | 1 != 2: ", != (1, 2));
   echo ("   a <  b: ", <  (a, b), " | 1 <  2: ", <  (1, 2));
   echo ("   a <= b: ", <= (a, b), " | 1 <= 2: ", <= (1, 2));
   echo ("   a >  b: ", >  (a, b), " | 1 >  2: ", >  (1, 2));
   echo ("   a >= b: ", >= (a, b), " | 1 >= 2: ", >= (1, 2));
   echo ();

   # Test and demonstrate logical operators.
   echo ("Logical operators:");
   echo ("   ?(0) = ", ?(0), ", ?(5) = ", ?(5));
   echo ("   !(0) = ", !(0), ", !(5) = ", !(5));
   echo ("   ----------------------------");
   echo ("    ||: (0,0)=",  || (0,0)," (0,1)=", || (0,1)," (1,1)=", || (1,1));
   echo ("    &&: (0,0)=",  && (0,0)," (0,1)=", && (0,1)," (1,1)=", && (1,1));
   echo ("    ^^: (0,0)=",  ^^ (0,0)," (0,1)=", ^^ (0,1)," (1,1)=", ^^ (1,1));
   echo ("   !||: (0,0)=", !|| (0,0)," (0,1)=",!|| (0,1)," (1,1)=",!|| (1,1));
   echo ("   !&&: (0,0)=", !&& (0,0)," (0,1)=",!&& (0,1)," (1,1)=",!&& (1,1));
   echo ("   !^^: (0,0)=", !^^ (0,0)," (0,1)=",!^^ (0,1)," (1,1)=",!^^ (1,1));
   echo ("     7 |  12 = ", | (7, 12));
   echo ("     7 &  12 = ", & (7, 12));
   echo ("     7 ^  12 = ", ^ (7, 12));
   echo ("     7 & ~12 = ", & (7, inv(12)));
   echo ();

   echo ("Logic / math tests:");
   = ($logic_func, [{
      args ($expr);
      if ($expr, "true", "false");
   }]);
   echo ("   1 + 2 = 3 : ", $logic_func (== (+ (1, 2), 3)));
   echo ("   1 + 2 = 4 : ", $logic_func (== (+ (1, 2), 4)));
   echo ("         yes : ", $logic_func (yes));
   echo ("        true : ", $logic_func (true));
   echo ("          on : ", $logic_func (on));
   echo ("       other : ", $logic_func (other));
   echo ();

   # Test all variable types, including 'undefined'.
   echo ("Variable types and scopes tests:");
   = ($a, 150);
   = ($b, "i am a single string");
   = ($c,
      'i '
      'am '
      'a '
      'compound '
      'string');
   = ($d, [{+ (1, 2, 3);}]);
   # intentionally leaving $e undefined.

   echo ("    1 = 1 (", scope (1), ")");
   echo ("   $a = ", $a, " (", scope ($a), ")");
   echo ("   $b = ", $b, " (", scope ($b), ")");
   echo ("   $c = ", $c, " (", scope ($c), ")");
   echo ("   $d = ", $d, " (", scope ($d), ")");
   echo ("   $e = ", $e, " (", scope ($e), ")");
   = ($$global, "one");
   echo ("   $$global = ", $$global, " (", scope ($$gobal), ")");
   += ($$global, " two");
   echo ("   $$global = ", $$global, " (", scope ($$gobal), ")");
   echo ();

   # func1() runs func2() and func3() in a funky order.
   echo ("Nested function tests: ");
   func1 ();
   echo ();

   # return values in multiple scopes that set variables.
   echo ("Variable scope and return values tests: ");
   echo ({
      = ($$global, echo ("   ", {
         = ($test, + ("We ", {
            = ($p, "should ");
         }));
      }, "print this!"));
   });
   echo ($$global);
   echo ("   ", $test);
   echo ("   ...did you see it three times?  Plus one <undefined>?");
   echo ();

   # Show off our language types and variable scoping.
   echo ("Type tests:");
   echo ("   undefined | ", type (undefined));
   echo ("   string    | ", type (string));
   echo ("   'a'       | ", type ('a'), " (", index ('a'), ")");
   echo ("   string[1] | ", type (string[0]), " (", index (string[1]), ")");
   echo ("   0         | ", type (0));
   echo ("   1.00      | ", type (1.00));
   echo ("   [string]  | ", type ([string]));
   echo ("   @@object  | ", type (@@object));
   echo ("   ~[string] | ", type (~[string]));
   echo ("   ----------|---------------");
   echo ("     x:      | ", type (  a), " (", scope (  a), ")");
   echo ("    $x:      | ", type ( $a), " (", scope ( $a), ")");
   echo ("   $$x:      | ", type ($$a), " (", scope ($$a), ")");
   echo ();

   # Complex variable assignments with unwrap (~) tokens and pointer functions.
   echo ("Unwrap tests:");
   = (~[$$global], "Test 1");
   echo ("   $$global = ", $$global);
   = (~ptr_func1 (), "Test 2");
   echo ("   $$global = ", $$global);
   = ($b, $a);
   = (~ptr_func2 (), "Test 3");
   echo ("   $a       = ", $a, " (was ", $b, ")");
   = ($a, "Test 4");
   echo ('   variable ("a"): ', variable ("a"));
   echo ();

   # This function should add arg(0) to $$counter, store into $$counter,
   # and return the result.
   = ($$counter, 1);
   echo ("Global + lambda tests:");
   = ($func, [{
      += ($$counter, arg (0));
      return ($$counter);
   }]);
   echo ("   type: ", type ($func));
   echo ("   ", $$counter, ", ",
                $func ($$counter), ", ",
                $func ($$counter), ", ",
                $func ($$counter), ", ",
                $func ($$counter));
   echo ();

   # Short-circuit logic gates && and ||.
   echo ("Short-circuit logic tests:");
   print ("   && (1,1,0,1): "); && (print(1), print(1), print(0), print(1));
      echo ();
   print ("   || (0,0,1,0): "); || (print(0), print(0), print(1), print(0));
      echo ();
   echo ();

   # simple for loop.
   echo ("for() loop test 1:");
   for (= ($i, 1), < ($i, 10), ++ ($i), {
      if (== ($i, 1),
         print ("   "),
         print (", "));
      print ($i);
   });
   echo ();

   # for loop with function reference.
   echo ("for() loop test 2:");
   = ($my_func, [{
      if (== ($i, 1),
          print ("   "),
          print (", "));
      print ($i);
   }]);
   for (= ($i, 1), < ($i, 10), ++ ($i),
        ~$my_func);
   echo ();

   # our own function imitating a for a loop.
   echo ("Custom for() loop test 1:");
   my_for_1 (= ($i, 1),
             < ($i, 10),
             ++ ($i),
             ~$my_func);
   echo ();

   # our own function 
   echo ("Custom for() loop test 2:");
   = ($i, 100);
   my_for_2 ([= ($i, 1)],
             [< ($i, 10)],
             [++ ($i)],
             $my_func);
   echo ();
   echo ();

   # Make sure variables in lambda function blocks get tossed out.
   echo ("Lambda variable scope tests:");
   = ($test_var, "value outside block");
   echo ("   $test_var = ", $test_var);
   [{
      args ($x); vars (test_var);
      = ($test_var, $x);
      echo (multi (
         "   {",
      + ("      $test_var = ", $test_var),
         "   }"));
   }] ("value inside block");
   echo ("   $test_var = ", $test_var);
   = ($l, [{ args ($x);
             + ("argument inside lambda = ", $x);
           }]);
   echo ("   $test_var = ", = ($test_var, $l (
      "string from outside lambda")));
   echo ();

   # Make sure run() can be used in the most extreme cases.
   echo ("Run() tests: ");
   echo ("   ", fancy_string ("Test 1"));
   echo ("   ", run (fancy_string, "Test 2"));
   echo ("   ", run (+, "*-* ", "Test 3", " *-*"));
   echo ("   ", run (fancy_string,
      [{
         args ($a, $b, $c);
         + ($a, $b, $c);
      }] ("Te", "st", " 4")));
   run ([{
      args ($a, $b, $c);
      echo ("   ", $a, " ", $b, " ", $c);
   }], "*-*", "Test 5", "*-*");
   run (run, run, run, run, run, run, [{
      args ($a, $b, $c);
      run (=, $$rval, + ("   ", {
         return (+ ("", $a, " ", $b, " ", $c));
      }));
   }], "*-*", "Test 6", "*-*");
   echo ($$rval);
   echo ();

   # String indexing w/ modification.
   echo ("String modification tests:");
   echo ("   $string = ", = ($string, "derp"));
   echo ("   $string[0] = ", = ($string[0], 'h'));
   echo ("   $string = ", $string);
   echo ("   $string[0] = 100 (", = ($string[0], 100), ")");
   echo ("   $string = ", $string);
   echo ("   $string[0]-- (", -- ($string[0]), ")");
   echo ("   $string = ", $string);
   echo ("   $string[3]++ (", ++ ($string[3]), ")");
   echo ("   $string = ", $string);
   echo ("   $string[1] += 4 (", += ($string[1], 4), ")");
   echo ("   $string = ", $string);
   echo ();

   # Make sure object references do what they're supposed to.
   = ($self,  this ());
   = ($selfn, object (name (this ())));
   = ($selfi, object (id   (this ())));
   = ($obj1, @main);
   = ($obj2, @@my_object);
   = ($obj3, @@bad_object);
   echo ("Object reference tests:");
   echo ("   $self = ", name ($self), "        (id = ", id ($self), ")");
   echo ("  $selfn = ", name ($selfn),"        (id = ", id ($selfn),")");
   echo ("  $selfi = ", name ($selfi),"        (id = ", id ($selfi),")");
   echo ("   ----------------------------------");
   echo ("   $obj1 = ", name ($obj1), "        (id = ", id ($obj1), ")");
   echo ("   $obj2 = ", name ($obj2),       "  (id = ", id ($obj2), ")");
   echo ("   $obj3 = ", name ($obj3),        " (id = ", id ($obj3), ")");
   echo ("   ----------------------------------");
   echo ("   $self == $obj1 : ", == ($self, $obj1), " <-- should pass");
   echo ("   $obj1 == $obj2 : ", == ($obj1, $obj2));
   echo ("   $obj2 == $obj3 : ", == ($obj2, $obj3));
   echo ("   $obj3 == @fail : ", == ($obj3, @fail));
   echo ("   $obj3 == $obj3 : ", == ($obj3, $obj3), " <-- should pass");
   echo ("   ----------------------------------");
   echo ("   $self != $obj1 : ", != ($self, $obj1));
   echo ("   $obj1 != $obj2 : ", != ($obj1, $obj2), " <-- should pass");
   echo ("   $obj2 != $obj3 : ", != ($obj2, $obj3), " <-- should pass");
   echo ("   $obj3 != @@bad : ", != ($obj3, @@bad), " <-- should pass");
   echo ("   $obj3 != $obj3 : ", != ($obj3, $obj3));
   echo ("   ----------------------------------");
   echo ("   ? ($obj1)  : ", ? ($obj1), " <-- should pass");
   echo ("   ! ($obj1)  : ", ! ($obj1));
   echo ("   ? (@@bad)  : ", ? (@@bad));
   echo ("   ! (@@bad)  : ", ! (@@bad), " <-- should pass");
   echo ();

   # Do some fancy stuff with properties.
   echo ("Object property tests:");
   echo ("   @@myobject.int     = ",  @@my_object.int);
   echo ("   @@myobject.string  = ",  @@my_object.string);
   echo ("  ~@@myobject.list    = ", ~@@my_object.list);
   echo ("   @@myobject.func    = ",  @@my_object.func);
   echo ("--------------------------------------------------");
   echo ("   @@myobject.+ (str, ing)     = ", @@my_object.+ (str, ing));
   echo ("   @@myobject.+ (fun, c)       = ", @@my_object.+ (fun, c));
   echo ("   @@myobject.func'(one)       = ", @@my_object.func'(one));
   echo ("   @@myobject.+ (fun, c)'(two) = ", @@my_object.+ (fun, c)'(two));
   echo ();

   # Do as many wacky casts as we can.
   = (.string,   "This is a property of type 'string'.");
   = ($string, "This is a block variable of type 'string'.");
   = ($$string, "This is an rlink variable of type 'string'.");
   echo ("Casting tests:");
   echo ("   + (int ('12340'), 5)      = ", + (int ('12340'), 5));
   echo ("   * (float (4), 1.5)        = ", * (float (4), 1.5));
   echo ("   char ('string')           = ", char ('string'));
   echo ("   char (65)                 = ", char (65));
   echo ("   char (65.7)               = ", char (65.7));
   echo ("   char (0)                  = ", char (0));
   echo ("   char (256)                = ", char (256));
   echo ("   + (char (65), 1)          = ", + (char (65), 1));
   echo ("   string (65)               = ", string (65));
   echo ("   string (65.7)             = ", string (65.7));
   echo ("   int ([a, b, c])           = ", int ([a, b, c]));
   echo ("   string ([a, b, c])        = ", string ([a, b, c]));
   echo ("   variable ('string')       = ", variable ('string'));
   echo ("   variable ('$string')      = ", variable ('$string'));
   echo ("   property ('string')       = ", property ('string'));
   echo ("   object ('@my_object').int = ", object ("@my_object").int);
   echo ();

   # A complicated (and ridiculously slow) recursive test.
   echo ("Recursive function test:");
   = ($count, 10);
   echo ("   First ", $count, " numbers in the Fibonacci sequence:");
   = ($recurse, [{
      = ($limit, arg (0));
      print ("   ");
      for (= ($i, 1), <= ($i, $limit), ++ ($i), {
         print (arg (1) ($i));
         if (== ($i, $limit),
            echo (),
            print (", "));
      });
   }]);

   $recurse ($count, fibonacci_recurse_1);
   $recurse ($count, fibonacci_recurse_2);
   $recurse ($count, fibonacci_recurse_3);
   echo ();

   # Everything here should call an error!  Use lambda for convenience.
   echo ("Error tests: ");
   = ($error_test, [{
         print ("   ");
         args ($check);
         = ($error, $check);
         echo  ("      $error = ", $error);
      }]
   );
   $error_test (+ ());
   $error_test (for ());
   $error_test (made-up-function ());
   $error_test (= (not-a-variable, value));
   $error_test (variable (0));
   $error_test (variable (this ()));
   $error_test (run (derp));
   $error_test (run (for));
   $error_test (- (a, b, c));
   $error_test (| (5.0, 1.0));
   $error_test ({+= ($not_defined_yet, 100, 200);});
   $error_test (++ ($not_defined_yet));
   $error_test (arg (0));
   $error_test ("test"[4]);
   $error_test ("test"[-1]);
   $error_test (= ("test"[0], 'x'));
   $error_test (= (= ($string, "test")[0], 'not char'));
   $error_test (= ($string[0], 0));
   $error_test (= ($string[0], 256));
   $error_test (= ([a, b, c][0], "derp"));
   $error_test (< ([a, b, c], [x, y, z]));
   $error_test (+ (@@main, @@my_object));
   $error_test (< (@@main, @@my_object));
   $error_test (== (@@main, "@@my_object"));
   $error_test (= (@@my_object.string, "this shouldn't work"));
   $error_test (= (this(), @@my_object));
   $error_test (new (derp, [resource-that-doesnt-exist]));
   $error_test (new (derp, [my_object, derp]));
   echo ("   Error test concluded.");
   echo ();
}

func1 {
   print ("   Veni, ");
   func2 ();
}

func2 {
   func3 ();
   echo ("vici!");
}

func3 {
   print ("vidi, ");
}

ptr_func1 {
   return ([$$global]);
}
ptr_func2 {
   return ([$a]);
}

my_for_1 {
   arg (0);
   while (arg (1), { 
      arg (3);
      arg (2);
   });
}

my_for_2 {
   args ($init, $condition, $loop, $action);
   $init ();
   while ($condition (), { 
      $action ();
      $loop ();
   });
}

fancy_string {
   args ($x);
   return (+ ("*-* ", $x, " *-*"));
}

@my_object {
   # set some primitive properties.
   = (.int,    100);
   = (.string, "foo bar baz");
   = (.list,   [One, Two, Three]);
   = (.func,   [{
      args ($value);
      return (+ ("The argument is '", $value, "'."));
   }]);
}

# comment outside of resource
comment_test {
   # comment inside block
   [a, #comment inside list
    b,
    c,
   ];
   # comment at end of block
}


# fibonacci_recurse_1,2,3 (n)
#    Various recursive methods to calculate and return
#    Fibonacci number n using recursion.
#    TODO: Only method 1 is fast enough to actually use.
fibonacci_recurse_1 {
   args ($n);
   if (<= ($n, 1),                     # Stop at zero and one.
      return ($n));                    # Otherwise, return:
   = ($a, fibonacci_recurse_1 (- ($n, 1))); # f(n-1) +
   = ($b, fibonacci_recurse_1 (- ($n, 2))); # f(n-2)
   + ($a, $b);
}
fibonacci_recurse_2 {
   args ($n);
   if (<= ($n, 1), return ($n));       # Stop at zero and one.
   + (                                 # Otherwise, return:
      fibonacci_recurse_2 (- ($n, 1)), #  f(n-1) + f(n-2)
      fibonacci_recurse_2 (- ($n, 2))
   );
}
fibonacci_recurse_3 {
   args ($n);
   if (<= ($n, 1), $n,                    # Stop at zero and one.
      + (                                 # Otherwise, return:
         fibonacci_recurse_3 (- ($n, 1)), #  f(n-1) + f(n-2)
         fibonacci_recurse_3 (- ($n, 2))
      )
   );
}
