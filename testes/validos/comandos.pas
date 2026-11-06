program Comandos;
var
 x, y : integer;
 r : real;
 c : char;
begin
 r := .5;
 c := '\t';
 if x > 0 then
   if y > 0 then write(x);
   else write(y);
 repeat
   begin
     x := x - 1;
     y := y + 1;
   end;
 until x = 0;
 while x < 3 do x := x + 1;
 begin
 end;
end.
