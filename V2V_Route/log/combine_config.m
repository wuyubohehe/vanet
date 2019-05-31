clear all;
close all;
clc;

baseDir='E:\\���';

load('E:\\���\\highspeed_11\\log\\loss_package_distance.txt');

% �ļ���
dirNames={'\\highspeed_11','\\highspeed_12','\\highspeed_31','\\highspeed_41'};

% ��ͼ�õ�pattern
figPattern={'b-','r-','k-','g-'};

dirNum=length(dirNames);

% ��ͼ������������
interval=20;

% ��ͼ�������
pointNum=50;

% ������ͼ��Ϣ
figData=zeros(2,pointNum,dirNum);

for iter=1:1:dirNum
    [figData(1,:,iter),figData(2,:,iter)]=pdr_calculate(strcat(baseDir,dirNames(iter)),interval,pointNum);
end



figure(1);
hold on;
for iter=1:1:dirNum
    plot(figData(1,:,iter),figData(2,:,iter),figPattern{iter},'LineWidth',2);
end
hold off;
legend()
grid on;


