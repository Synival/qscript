main {
   echo ("Go!");
   echo (fibonacci_recurse (1));
   echo (fibonacci_recurse (2));
   echo (fibonacci_recurse (3));
   echo (fibonacci_recurse (4));
   echo (fibonacci_recurse (5));
   echo (fibonacci_recurse (6));
   echo (fibonacci_recurse (7));
   echo (fibonacci_recurse (8));
   echo (fibonacci_recurse (9));
   echo (fibonacci_recurse (10));
   echo (fibonacci_recurse (11));
   echo (fibonacci_recurse (12));
   echo (fibonacci_recurse (13));
   echo (fibonacci_recurse (14));
   echo (fibonacci_recurse (15));
   echo ("Whew.");
}

fibonacci_recurse {
   args ($n);
   if (<= ($n, 1), return ($n));      # Stop at zero and one.
   + (                                # Otherwise, return:
      fibonacci_recurse (- ($n, 1)),  #  f(n-1) + f(n-2)
      fibonacci_recurse (- ($n, 2))
   );
}
