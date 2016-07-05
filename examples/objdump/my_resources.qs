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
