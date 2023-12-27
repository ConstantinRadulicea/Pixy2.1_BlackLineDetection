clear;
clc;
close all;

img = readmatrix("image.csv", "Delimiter",",");
logical_img = cos(img.*(pi/2));
logical_img = round(logical_img);
logical_img = logical(logical_img);
skel = bwskel(logical_img);
% figure
% imshow(skel)
% figure
% imshow(bwmorph(logical_img,'skel',Inf))

img = img .* 100;
newA = zeros(size(img)+2);
newA = newA + 255;
newA(2:end-1,2:end-1)=img;
img = newA;


edges = readmatrix("edges.csv", "Delimiter",",");
A = zeros(size(img));
if ~isempty(edges)
    edges = edges + [2 2];
    ind = sub2ind(size(img),edges(:, 2),edges(:, 1));
    A(ind) = 1;
end







A = A.* 50;
img = img + A;
figure
image(A)

