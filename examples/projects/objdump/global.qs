@global_object {
   = (.message, "I'm a global object!");
   = (.array, [
         true,
         "I'm a string!",
         120,
         3.14159,
         @@global_object
      ]);
}
