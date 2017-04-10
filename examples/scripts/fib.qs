main {
   for (= ($i, 1), <= ($i, 40), ++($i),
      echo (_padded_str ($i, 2), ': ', fibonacci_recurse ($i)));
}

_padded_str {
   args ($val, $len);
   = ($str, string ($val));
   + (* (" ", - ($len, length ($str))), $str);
}

fibonacci_recurse {
   args ($n);
   if (<= ($n, 1), return ($n));      # Stop at zero and one.

   if (= ($fstored, _fib_fetch ($n)), # If this number has already been
      return ($fstored));             # calculated, use it.

   = ($rval, + (                      # Otherwise, calculate:
      fibonacci_recurse (- ($n, 1)),  #  f(n-1) + f(n-2)
      fibonacci_recurse (- ($n, 2))
   ));

   _fib_store ($rval);
   return ($rval);
}

_fib_fetch {
   args ($n);
   if (== (undefined, $@_fib_array),
      = ($@_fib_array, [1, 1]));
   ? (>= (- (length ($@_fib_array), 1), $n),
      $@_fib_array[$n],
      undefined);
}

_fib_store {
   += ($@_fib_array, arg(0));
}
