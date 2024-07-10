program temp;
var
  c: char;
  a: char;
  m: real;
  b: real;
begin
  read(c);
  read(a);
  read(b);

  if c = 'C' then
  begin
    if a = 'K' then
      m := b + 273.15
    else
      m := (b * 9 / 5) + 32;
  end
  else if c = 'F' then
  begin
    if a = 'C' then
      m := (b - 32) / 1.8
    else
      m := ((b - 32) / 1.8) + 273.15;
  end
  else
  begin
    if a = 'C' then
      m := b - 273.15
    else
      m := ((b - 273.15) * 9 / 5) + 32;
  end;

  write(m:0:2);
end.
