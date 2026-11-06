program Precedencia;
var
 a, b, c : integer;
begin
 a := 2 + 3 * 4;
 a := (2 + 3) * 4;
 a := 10 - 2 - 3;
 a := 20 div 2 div 5;
 a := -b * 2;
 a := b - -c;
 if a < b or b < c and a = c then write(a);
 if not (a = b) and b = c then write(b);
end.
