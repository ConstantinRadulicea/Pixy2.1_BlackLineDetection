clear;
clc;
close;

img = readmatrix("image.csv", "Delimiter",",");
img = img .* 100;
newA = zeros(size(img)+2);
newA = newA + 255;
newA(2:end-1,2:end-1)=img;
img = newA;
image(img)


edges = readmatrix("edges.csv", "Delimiter",",");
A = zeros(size(img));
if ~isempty(edges)
    edges = edges + [2 2];
    A(sub2ind(size(img),edges(:, 2),edges(:, 1))) = 50;
end





img = img + A;
image(img)

