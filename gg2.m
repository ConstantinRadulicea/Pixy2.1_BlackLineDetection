clear;
clc;
close all;

%% read image
% I=imread('Untitled.png');

img = readmatrix("image.csv", "Delimiter",",");
logical_img = cos(img.*(pi/2));
logical_img = round(logical_img);

%I=I(29:540,84:595,1);%cut out border
%binaryImage=I>128;
binaryImage = logical(logical_img);
%% method 1
result = false(size(binaryImage));
for row=1:size(binaryImage,1)
    if any(binaryImage(row,:))%correct for empty lines in the image
        ind=round(mean(find(binaryImage(row,:))));
        result(row, ind)=true;
    end
end
method1=result;
%% method 2
dist2edge=bwdist(~binaryImage);
result = false(size(binaryImage));
[~,ind]=max(dist2edge,[],2);
rowSub=(1:size(result,1))';%transpose to make the shape match ind
colSub=ind;
linind=sub2ind(size(result), rowSub, colSub);
result(linind)=true;
result(~binaryImage)=false;%correct for empty lines in the image
method2=result;
%% display
merged1=cat(3,binaryImage,method1,false(size(binaryImage)));
merged1=double(merged1);
merged2=cat(3,binaryImage,method2,false(size(binaryImage)));
merged2=double(merged2);
figure(1),clf(1)
subplot(2,3,[1 4])
imshow(binaryImage)
title('binary image')
subplot(2,3,2)
imshow(method1)
title('skeletonized image (method 1)')
subplot(2,3,3)
imshow(merged1)
title('merged image (method 1)')
subplot(2,3,5)
imshow(method2)
title('skeletonized image (method 2)')
subplot(2,3,6)
imshow(merged2)
title('merged image (method 2)')