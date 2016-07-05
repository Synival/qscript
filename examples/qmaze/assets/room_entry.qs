# Our first room.
@room_entry
{
   = (.name, "an empty, white room");
   = (.intro, multi (
"   You awaken with a jolt, panicked, breathing heavily on the floor.  Your",
"surroundings are unfamiliar - most definitely not where you had gone to bed.",
"Confused, disoriented, and angry, you climb to your feet and begin searching",
"for answers.",
"",
"Useful commands:",
bar ('-'),
"look - Look at the room",
"help - Display a full list of commands",
"quit - Stop playing QMaze"));

   = (.desc, multi (
"   A cubic room the size of your bed chamber.  Each side is made of stone ",
"bricks and painted white. Perhaps if you look at the room a little closer, ",
"you might find some clues."));

   = (.hints, [
"Have you tried looking at the walls?",
"You can probably physically interact with the walls in some way.",
"Try to push, pull, kick, punch, hit, and poke stuff.",
"Punching or kicking the wall should reveal something.",
"KICK THE WALL.",
"There's a loose brick in here.  What happens if you push it?",
"PUSH THE BRICK.",
"I, uh... am not really sure what else to tell you at this point...",
"Brushing and flossing regularly will reduce the risk of tooth decay."
      ]);

   = (.objects, [
      [@@room_entry_wall,  [wall, walls, room]],
      [@@room_entry_brick, [brick, bricks]]
   ]);
}

@room_entry_wall
{
   = (.name, "the walls");
   = (.desc, multi (
"   Each brick on each wall of this cubic room, including the ceiling and",
"floor, is white, perfectly smooth, and identical.  It would appear that ",
"simply looking at the room isn't enough to get out of here.  Better think of",
"something else."));

   = (.punch, [{
      echo (multi (
"Ouch!  You've caused considerable damage to your hand with that one.",
"You notice a loose single brick shake slightly with hollow rattling sound."));
      = ($$brick_revealed, 1);
   }]);
   = (.kick, [{
      echo (multi (
"Thud!  You foot hits the wall.",
"You notice a loose single brick shake slightly with hollow rattling sound."));
      = ($$brick_revealed, 1);
   }]);
   = (.push, [{
      echo ("Even with all your weight and might, the wall refuses to move.");
   }]);
   = (.pull, [{
      echo ("Unfortunately, you can't find anywhere to grab.");
   }]);
   = (.poke, [{
      echo ("Tee hee!  Silly wall.");
   }]);
}

@room_entry_brick {
   = (.name, "the bricks");
   = (.desc, multi (
"   You look at each individual brick, one at a time.  They're all white, ",
"they're all smooth, and they're all bricks.  Nothing revealing there."));

   = (.pull, [{
      if ($$brick_revealed, {
         echo (multi (
"   You wrap your fingers around the as much of the loose brick as you ",
"can and attempt to tug.  It doesn't to much.  Maybe the other direction?"));
      }, {
         echo ("Tugging at the corners of random bricks doesn't do much.");
      });
   }]);
   = (.push, [{
      if ($$brick_revealed, {
         echo (multi (
"   After applying only the slightest amount of pressure, the brick abrubtly",
"slides into the wall at a slow, steady pace.  The room shakes and fills",
"with the sound of rumbling stone.  Looking around the room frantically,",
"you realize the wall opposite the loose brick has split in two down the",
"middle and each side is slowly sliding apart.  Heavy dust picks up forms a",
"thick cloud, momentarily blinding your view.",
"   As the cloud subsides, beams of light begin to trickle in from the ",
"new opening.  It's bright - but completely black.  An endless void, except",
"for a long, seemingly infinite wooden plank extending out from the floor",
"of this room.  It looks like you have no choice but to walk along it."));
         use_room ("room_plank");
      }, {
         echo ("Applying pressure to random bricks doesn't do much.");
      });
   }]);
   = (.punch, .push);
   = (.kick,  .push);
   = (.poke,  .push);
}
