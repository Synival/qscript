# QMaze commands.

command_look
{
   # Are we looking at the room?
   vars ($target);
   if (== (args (), 0), {
      = ($target, $$room);
   },
   # ...Or an object?
   {
      = ($str, arg(0));
      = ($target, get_object ($str));
      if (== (undefined, $target), {
         echo ("You don't see the '", $str, "' here.");
         return ();
      });
   });

   # Look at our target.
   look_at ($target);
}

command_help
{
   echo (multi (
bar ('-'),
"Basic Commands:",
bar ('-'),
"look           - Look at the room.",
"look (object)  - Look at something in the room.",
"help           - Show this message.",
"hint           - Get a hint for this room.",
"quit           - Leave QMaze without saving.",
"",
bar ('-'),
"Actions:",
bar ('-'),
"punch (object) - Punch something with your fists.",
"kick (object)  - Kick something with your foot.!",
"push (object)  - Put your weight into it and PUSH!",
"pull (object)  - Grab an object and pull it.",
"poke (object)  - Tee hee!"
));

}

command_hint
{
   # What hint should we be looking at?
   = ($room_hints, [variable (+ ("$$hints_", $$room_name))]);
   if (== (undefined, $room_hints[0]),
       = ($room_hints[0], 0));

   # Is there a corresponding hint?
   = ($hints, $$room.hints);
   if (< ($room_hints[0], length ($hints)), {
      echo ($hints[$room_hints[0]]);
      ++($room_hints[0]);
      ++($$hints);
      echo ("* You have now used ", $$hints, " hint",
         if (== ($$hints, 1), "", "s"), ".");
   }, {
      echo ("There are no more hints in this room.");
   });
}

command_quit
{
   echo ("Are you sure you want to quit?");
   print ("Type 'y' to quit: ");
   = ($prompt, prompt ());
   if (&& (!= ($prompt, ""), == ($prompt[0], 'y')),
      = ($$quit, 1));
   if (! ($$quit),
      echo ("Alright, let's keep playing."));
}

_command_action
{
   # If we only provided an action, complain.
   if (== (args (), 1), {
      = ($action, arg(0));
      -= ($action[0], 32);
      echo ($action, " what?");
      return ();
   });

   # Look for an object to interact with.
   args ($action, $target);
   = ($object, get_object ($target));
   if (== (undefined, $object), {
      echo ("You can't find the '", $target, "' to ", $action, ".");
      return ();
   });

   # Perform our action.
   echo ("You ", $action, " ", $object.name, ".");
   = ($func, $object.$action);
   if (== (undefined, $func), {
      echo ("Nothing happens.");
   }, {
      $func (this ());
   });
}

command_punch { _command_action ("punch", ~arg_list ()); }
command_hit   { _command_action ("punch", ~arg_list ()); }
command_kick  { _command_action ("kick",  ~arg_list ()); }
command_push  { _command_action ("push",  ~arg_list ()); }
command_pull  { _command_action ("pull",  ~arg_list ()); }
command_poke  { _command_action ("poke",  ~arg_list ()); }
command_rub   { _command_action ("rub",   ~arg_list ()); }
