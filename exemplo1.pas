program TestaParidade;
var
    n : integer;
begin
    n := 2 * 13 + 5;
    if n = (n div 2)*2 then
        write('p');
    else
        write('i');
end.