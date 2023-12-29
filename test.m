clear;
clc;
close all;


fullFileName = 'https://ww2.mathworks.cn/matlabcentral/answers/uploaded_files/988570/road.png';
rgbImage = imread(fullFileName);
grayImage = rgb2gray(rgbImage);
figure
imshow(grayImage)

grayImage(grayImage<170)=0;
figure
imshow(grayImage, []);

%impixelinfo;
mask = logical(grayImage > 140 & grayImage < 255); 
mask = bwareafilt(mask, 2); 
mask = bwskel(mask);
mask2 = imopen(mask, strel('line', 3, 0));
mask(mask2) = 0;
mask = logical(mask);
figure
imshow(mask);
% make center line
[r,c] = find(mask);
rs = sort(unique(r));
cens = [];
for i = 1 : length(rs)
    mi = mask(rs(i),:);
    [~,ci] = find(mi);
    if max(ci) - min(ci) < 1e1
        continue;
    end
    cens = [cens; mean([max(ci) min(ci)]) rs(i)];
end
% make line
p = polyfit(cens(:,2), cens(:,1), 2);
yt = linspace(min(rs), max(rs), 1e3);
xt = polyval(p, yt);
hold on; plot(xt, yt, 'g-', 'LineWidth',3)

figure; imshow(rgbImage);
hold on; plot(xt, yt, 'g-', 'LineWidth',3)
