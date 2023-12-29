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
% newA = zeros(size(img)+2);
% newA = newA + 255;
% newA(2:end-1,2:end-1)=img;
% img = newA;


edges = readmatrix("edges.csv", "Delimiter",",");
A = zeros(size(img));
if ~isempty(edges)
     edges = edges + [1 1];
    ind = sub2ind(size(img),edges(:, 2),edges(:, 1));
    A(ind) = 1;
    edges = A;
end

pixels_readden = readmatrix("black_pixels_readden.csv", "Delimiter",",");
A = zeros(size(img));
if ~isempty(pixels_readden)
     pixels_readden = pixels_readden + [1 1];
    ind = sub2ind(size(img),pixels_readden(:, 2),pixels_readden(:, 1));
    A(ind) = 1;
    pixels_readden = A;
end

white_pixels_readden = readmatrix("white_pixels_readden.csv", "Delimiter",",");
A = zeros(size(img));
if ~isempty(white_pixels_readden)
     white_pixels_readden = white_pixels_readden + [1 1];
    ind = sub2ind(size(img),white_pixels_readden(:, 2),white_pixels_readden(:, 1));
    A(ind) = 1;
    white_pixels_readden = A;
end






white_pixels_readden = white_pixels_readden .* 200;
edges = edges.* 50;
pixels_readden = pixels_readden .* 20;
img = img + edges + pixels_readden + white_pixels_readden;
figure
image(img)

