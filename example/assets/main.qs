# Our player, auto-instantiated.
main {
   # Get our player's name, just for fun.
   echo ("Hey there!  What's your name?");
   echo ();

   if (== (type (= ($$name, prompt ())), "undefined"),
      return ());
   if (== ("", $$name),
       = ($$name, "Nobody"));
   echo ();

   # Start our game!
   capitalize_var ([$$name]);
   echo (bar ());
   echo ("   Welcome to QMaze, ", $$name, "!");
   echo (bar ());
   echo ();

   # initialize our game state room.
   = ($$hints, 0);
   use_room (room_entry);
   echo ();

   # Read commands until forever.
   = ($$quit, 0);
   while (!($$quit), {
      # Get our command string.  Break on ^D.
      if (== (type (= ($$command, prompt ())), "undefined"),
         break ());

      # Tokenize string and turn into an run() execution list.
      # Make sure our command starts with 'command_'.
      = ($run, tokenize ($$command));
      if (== (length ($run), 0),
         continue ());
      = ($cmd, $run[0]);
      = ($run[0], + ("command_", $run[0]));

      # Does our room have something associated with this command?
      = ($func, + ("", $$room_name, "_", $run[0]));
      if (func_exists ($func), {
         = ($run[0], $func);
         run (~$run);
      },
      # Is there a general command?
      if (func_exists ($run[0]), {
         run (~$run);
      },
      # Else: Toss an error.
      {
         echo ("You cannot '", $cmd, "' in this room.");
      }));
      echo ();
   });

   # Let's be polite.
   echo ("Thank you for playing!");
}
