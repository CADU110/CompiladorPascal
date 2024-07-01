program AnalisadorLexicoTeste;
var
  numero: integer;
  texto: string;
begin
  numero := 10;
  texto := 'Ola, mundo!';
  writeln("O numero e: ", numero);
  if numero > 5 then
    writeln("O numero e maior que 5.")
  else
    writeln('O numero e menor ou igual a 5.');
  for numero := 1 to 5 do
    writeln('Numero atual: ', numero);
  repeat
    writeln('Loop infinito');
  until false;
end.

