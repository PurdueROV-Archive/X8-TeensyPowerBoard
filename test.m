fid = fopen('testvals.txt','w');
fid2 = fopen('mloutput.txt','w');
for x = 1:50
   v = randi(10,1,6)';
   fprintf(fid,[sprintf('%d, ',v(1:5)) '%d\n'],v(6));
   a = val * v; 
   fprintf(fid2,[sprintf('%d, ',a(1:7)) '%d\n'],a(8));
end