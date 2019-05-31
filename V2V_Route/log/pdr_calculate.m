function [centerPackageLossDistance,numPackageLossDistance]=pdr_calculate(basePath,interval,pointNum)

%% PRR
path1=strcat(basePath,'\\log\\loss_package_distance.txt');
path2=strcat(basePath,'\\log\\distance.txt');

% path1�Ǹ�cell���ͣ�����תΪ�ַ���������Ϊ·�����Ǻ�
PackageLossDistance=load(path1{1});
PackageTransimitDistance=load(path2{1});

%IntersectDistance=intersect(unique(PackageLossDistance),unique(PackageTransimitDistance));
IntersectDistance=0:interval:interval*(pointNum-1);

[numPackageLossDistance,centerPackageLossDistance]=hist(PackageLossDistance',IntersectDistance);
[numPackageTransimitDistance,~]=hist(PackageTransimitDistance',IntersectDistance);

numPackageLossDistance=numPackageLossDistance./numPackageTransimitDistance;


