# Show the description of anything here.
look_at
{
   # What are we looking at?
   args ($target);

   # Get properties locally.
   = ($$name, capitalize ($target.name));
   = ($$desc,  $target.desc);

   # Print whatever we found.
   if (!= (undefined, $$name), {
      capitalize ([$$name]);
      echo ($$name, ":");
   });
   if (!= (undefined, $$desc), {
      echo ($$desc);
   });
}

# Capitalize a global variable.  Passed as [$$var].
capitalize_var {
   # Get a pointer to a string we passed.  Get its length.
   args ($strp);
   = ($len, length ($strp[0]));

   # Capitalize the first letter and all first letters after whitespace.
   = ($cap, 1);
   for (= ($i, 0), < ($i, $len), ++($i), {
      = ($ch, $strp[0][$i]);
      if ($cap, {
         if (&& (>= ($ch, 'a'), <= ($ch, 'z')),
            -= ($strp[0][$i], 32));
         = ($cap, 0);
      },
      if (== ($ch, ' '), {
         = ($cap, 1);
      }));
   });

   # Return a pointer to the string.
   return ($strp);
}

# Returns a capitalized copy of arg(0).
capitalize {
   # Get the string we passed.  Get its length.
   args ($str);
   if (!= (type ($str), "string"),
      return (undefined));
   = ($len, length ($str));

   # Capitalize the first letter and all first letters after whitespace.
   = ($cap, 1);
   for (= ($i, 0), < ($i, $len), ++($i), {
      = ($ch, $str[$i]);
      if ($cap, {
         if (&& (>= ($ch, 'a'), <= ($ch, 'z')),
            -= ($str[$i], 32));
         = ($cap, 0);
      },
      if (== ($ch, ' '), {
         = ($cap, 1);
      }));
   });

   # Return the string we just made.
   return ($str);
}

# Return an 80-character long set of equals, or whatever is provided by arg(0).
bar {
   if (== (args (), 0), {
      return (
         "========================================"
         "========================================"
      );
   }, {
      = ($ch, arg(0));
      = ($rval, "");
      for (= ($i, 0), < ($i, 80), ++ ($i),
         += ($rval, $ch));
      return ($rval);
   });
}

use_room {
   # Get our new room. Don't do anything if we're already there.
   args ($new_room);
   if (== ($new_room, $$room_name),
      return ());

   # Set our room.
   = ($$room_name, $new_room);
   = ($$room, object (+ ("@", $$room_name)));

   # If we haven't seen this room before, give us a message.
   = ($seen, [variable (+ ("$$seen_", $$room_name))]);
   if (== (undefined, $seen[0]), {
      echo ($$room.intro);
      = ($seen[0], 1);
   });
}

get_object {
   # Attempt to get our object list.
   args ($str);
   if (== (undefined, = ($objects, $$room.objects)),
      return (undefined));

   # Look for a matching object.
   for_each ($objects, $o,
      for_each ($o[1], $word,
         if (== ($word, $str),
            return ($o[0]))));

   # Looks like we didn't find one.
   return (undefined);
}
