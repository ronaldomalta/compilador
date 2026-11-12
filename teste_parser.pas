program TesteParser;

var
   x, y : integer;
   val : real;
   ch : char;
begin
   x := 2 + 3 * 4;
   y := (2 + 3) * 4;
   x := 10 div 2 + 3;
   val := .5;

   if x > 5 and y < 10 or x = y then
      write(x);

   if not (x + 2 * 3 >= y) and (x <> 0 or y = 10) then
      write(x);

   val := 3.1415;
   ch := 'a';

   if x + y * 2 > 10 and x = 5 then
      write(x);

   if not (x = y) then
      write(y);
end.
