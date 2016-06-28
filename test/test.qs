# Test a lot of stuff!

*my_object {
   # Proper echo() behavior with some strings, numbers, and statements.
   echo ("Numbers and addition vs. concatenation test:");
   echo ("   nums: ", 1, ", ", 2.00, ", ", -3.250);
   echo ("   add:  ", + (1, 2, 3));
   echo ("   cat:  ", + ("", 1, 2, 3));
   echo ("   cat2: ", + ("test", "1", "2", "4", ** (2, 3)));
   echo ();

   # Math evaluation demonstration for integers vs. floating points.
   echo ("Integer vs. floating point math test:");
   echo ("   7    * 11   = ",  * (  7, 11));
   echo ("   7    * 11.5 = ",  * (  7, 11.5));
   echo ("   7.1  * 11   = ",  * (7.1, 11));
   echo ("   7.1  * 11.5 = ",  * (7.1, 11.5));
   echo ("   8   ** 2    = ", ** (  8, 2));
   echo ("   8   ** 2.5  = ", ** (  8, 2.5));
   echo ("   8.0 ** 2    = ", ** (8.0, 2));
   echo ("   8.0 ** 2.5  = ", ** (8.0, 2.5));
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
   echo ("   !0 = ", !(0), ", !1 = ", !(1));
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

   echo ("Logic / math test:");
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
   echo ("Variable types and scopes test:");
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

   echo ("   1 = 1 (", scope (1), ")");
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
   echo ("Nested function test: ");
   func1 ();
   echo ();

   # return values in multiple scopes that set variables.
   echo ("Variable scope and return values test: ");
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

   echo ("Type test:");
   echo ("   string   | ", type (string));
   echo ("   0        | ", type (0));
   echo ("   1.00     | ", type (1.00));
   echo ("   [string] | ", type ([string]));
   echo ("  ~[string] | ", type (~[string]));
   echo ("   ---------|---------------");
   echo ("     x:     | ", type (  a), " (", scope (  a), ")");
   echo ("    $x:     | ", type ( $a), " (", scope ( $a), ")");
   echo ("   $$x:     | ", type ($$a), " (", scope ($$a), ")");
   echo ();

   echo ("Reference test:");
   = (~[$$global], "Test 1");
   echo ("   $$global = ", $$global);
   = (~ptr_func1 (), "Test 2");
   echo ("   $$global = ", $$global);
   = ($b, $a);
   = (~ptr_func2 (), "Test 3");
   echo ("   $a       = ", $a, " (was ", $b, ")");
   = ($a, "Test 4");
   echo ("   variable ('$a'): ", variable ("$a"));
   echo ();

   = ($$counter, 1);
   echo ("Global + lambda test:");
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
   echo ("Short-circuit logic test:");
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

   echo ("Lambda test:");
   = ($test_var, "value outside block");
   echo ("   $test_var: ", $test_var);
   [{
      args ($x); vars (test_var);
      = ($test_var, $x);
      echo ("   { $test_var: ", $test_var, " }");
   }] ("value inside block");
   echo ("   $test_var: ", $test_var);
   = ($l, [{ args ($x);
             + ("arg1 = ", $x);
           }]);
   echo ("   ", $l ("argument read inside lambda"));
   echo ();

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

   echo ("Error test: ");
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
   $error_test (variable (no-scope));
   $error_test (run (derp));
   $error_test (run (for));
   $error_test (- (a, b, c));
   $error_test (/ (1, 0));
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
