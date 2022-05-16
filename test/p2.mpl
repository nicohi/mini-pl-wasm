program p1;
begin
var nTimes : int;
nTimes := 0;
writeln("How many times? ");
read(nTimes);
var x : int;
while (x < nTimes) do
begin
writeln(x, " : Hello, World!\n");
x := x + 1;
end;
assert(x = nTimes);
end;
