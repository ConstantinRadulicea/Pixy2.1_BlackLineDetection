clear;
clc;
close all;


fullFileName = 'C:\Users\BadBoy_ThinkBook\Desktop\403647780_349539491154334_1837665137385227264_n.jpg';
rgbImage = imread(fullFileName);
[H S V]=rgb2hsv(rgbImage);
figure
imshow(V)
