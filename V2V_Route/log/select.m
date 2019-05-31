[data1,data2] = textread('select.txt','%n%n');
plot(data1(1),data2(1),'rX');
hold on;
plot(data1(2),data2(2),'rX');
hold on;
for i=3:1:length(data1)-1
    plot(data1(i),data2(i),'bX');
    hold on;
end
plot(data1(length(data1)),data2(length(data2)),'rX');
hold on;
for i=-17320:3464:17320
    for j=-10:4:10
        plot(i/10,j,'go');
        hold on;
    end
end