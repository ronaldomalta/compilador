program TesteFinal;
var
    x, y : integer;
    valor : real;
    letra : char;
begin
    x := 10;
    y := 2 * 3 + 4;
    valor := .5;
    letra := '_';

    if x > y and not (y = 0) then
        write('a');

    while x > 0 do
        begin
            x := x - 1;
        end;

    repeat
        y := y + 1;
    until y >= 10;

    write('\n');
end.
