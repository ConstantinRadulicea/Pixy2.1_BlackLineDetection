clear;
clc;
close;

img = readmatrix("image.csv", "Delimiter",",");
img = img .* 100;
image(img)


edges = readmatrix("edges.csv", "Delimiter",",")
A = zeros(size(img));
if ~isempty(edges)
    edges = edges + [1 1];
    A(sub2ind(size(img),edges(:, 2),edges(:, 1))) = 50;
end





img = img + A;
image(img)

