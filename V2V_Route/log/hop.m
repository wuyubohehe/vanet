clear all;
close all;
clc;

[data1] = textread('route_udp_hop.txt','%n');
data1=data1+1;
y=0:1:20;
[number,center]=hist(data1,y);
number=number./(sum(number));
bar(center,number);

average=sum(data1)./length(data1);