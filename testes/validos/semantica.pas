program Semantica;
var
 x, y : integer;
 r : real;
 c, d : char;
begin
 r := x + 2.5;
 r := x / y;
 r := -r * 2.0;
 x := x div 2 - -y;
 if c = d or c <> 'a' then write(c);
 if r > x and not (x = y) then write(r);
 while x < 10 do x := x + 1;
 repeat y := y - 1; until y <= 0;
 write(x + y);
end.
