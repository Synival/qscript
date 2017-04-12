# The second room.
@room_plank {
   = (.name, "an infinite wooden plank");

   = (.intro, multi (
"   You steadily, carefully walk down the narrow wooden plank for a very long",
"while.  Looking back over your shoulder, you notice the cubic room is ",
"suddenly gone and in its place is more of this plank an infinite black void.",
"No choice but to keep going, it would seem.",
"   After a few more minutes of walking, you notice there is a sign in front",
"of you.  It seemingly appeared out of nowhere."));

   = (.desc, multi (
"   The wooden plank, less than a half-meter wide, goes on forever in two",
"directions.  The only other sight to behold is the infinite black void",
"surrounding you.  Surprisingly, the area is incredibly well-lit.  In fact,",
"it's so bright it hurts your eyes.",
"   There is a sign in front of you."));

   = (.objects, [
      [@@room_plank_sign,  [sign]],
      [@@room_plank_belly, [belly]]
     ]);

   = (.hints, [
      multi ("There's nothing left to do in this room but poke your belly.",
             "...and read more hints."),
      multi ("You can also punch, kick, push, and pull your belly."),
      multi ("Is there anything else you can do with your belly?"),
      multi ("That's it!  You've earned yourself a nice, relaxing day",
             "sipping coffee and eating cake.  Enjoy!")
     ]);
}

@room_plank_sign {
   = (.name, "a wooden sign");
   = (.desc, multi (
bar (-),
"                                   THE END",
"                              Thanks for playing!",
bar (-),
"Wait, that's it?  This has been a tech demo of QScript v0.1.0.  Thank you",
"for checking it out!  Maybe check out hints in the room before you go?"));
}

@room_plank_belly {
   = (.name, "your belly");
   = (.desc, multi (
"   It's a bit bigger than you'd like it to be. #FatShaming"));

   = (.punch, [{
      echo ("OOF!!  OH GOD OW.  WHY DID YOU DO THAT?");
   }]);
   = (.kick, [{
      echo ("...How in the world did you manage to do that?  I mean, OW!!");
   }]);
   = (.push, [{
      echo ("It undulates.  Time to lose some weight?");
   }]);
   = (.pull, [{
      echo (multi (
"No matter how hard you try, it won't come out!  Looks like diet and exercise",
"really is the only way to lose it.  Lame."));
   }]);
   = (.poke, [{
      echo ("Tee hee!  This is fun!");
   }]);
   = (.rub, [{
      echo (multi (
"Whee!  You try patting your head at the same time, but your brain refuses",
"to cooperate and you end up punching yourself in the gut.  OOF!  You're glad",
"nobody was around to see that..."));
   }]);
}
