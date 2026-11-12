program SomaImpares;
var
    n : integer;
    i, proximoImpar, soma : integer;
begin
    n := 4;
    i := 0;
    soma := 0;
    while i < n do
        begin
            proximoImpar := 2*i + 1;
            soma := soma + proximoImpar;
            i := i + 1;
        end;
    write(soma);
    write('\n');
end.
