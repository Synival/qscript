character_base {
   = (.str, 10.0);
   = (.def, 10.0);
   = (.agi, 10.0);
   = (.vit, 10.0);
   = (.int, 10.0);
   = (.wis, 10.0);
}

class_warrior {
   *= (.str, 1.60);
   *= (.def, 1.40);
   *= (.vit, 1.20);
   /= (.agi, 1.20);
   /= (.int, 1.40);
   /= (.wis, 1.60);
}

class_magician {
   *= (.int, 1.60);
   *= (.wis, 1.40);
   *= (.agi, 1.20);
   /= (.vit, 1.20);
   /= (.def, 1.40);
   /= (.str, 1.60);
}

buff_quad_damage {
   append (buffs, "Quad Damage");
   *= (.str, 4.00);
}

buff_well_fed {
   append (buffs, "Well-Fed");
   += (.str, 10);
}

append {
   args ($pname, $str);
   = ($prop, [property ($pname)]);
   if (== (undefined, $prop[0]),
      =  ($prop[0], $str),
      += ($prop[0], ", ", $str));
}
