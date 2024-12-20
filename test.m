close all;
clear;
clc;

values_intersection1 = {
        {[1 76 78 60 53 54 52 44 40 36 37 33 31 26],...
         [80 80 82 121 130 132 137 150 160 164 165 172 180 184]},...
        {[313 253 251 251 279 297 301 305],...
         [80 80 82 87 141 180 185 187]},...
        {[207 211 214 224 314],...
         [12 15 23 37 37]},...
        {[110 100 96 0],...
         [14 33 37 38]}
    };

values_curve2 = {
        {[145 171 195 219 238 252 275 298 298 305 306 312 311 313 314],...
         [24 27 32 41 51 56 71 96 101 111 116 127 131 133 140]},...
        {[0 22 31 43 50 53 53 48 35 1],...
         [44 44 46 53 60 68 71 82 98 127]}
    };

values_straight_with_start_lines = {
        {[122 125 125 116 17 18 15 12],...
         [14 16 18 30 142 145 145 148]},...
        {[217 301 312 311 314],...
         [25 121 135 137 143]}
    };



% values = values_intersection1;
% values = values_curve2;
values = values_straight_with_start_lines;


polynoms = {};

for i = 1:length(values)
    x = values{i}{1};
    y = values{i}{2};
    t = 1:length(x);
    
    [px py t] = CurvePolyfit(x, y, 1000, 15);

    polynoms{end+1} = {px, py, t};
        
    
    
    % Generate the fitted curve
    t_fit = linspace(min(t), max(t), 100);
    x_fit = polyval(px, t_fit);
    y_fit = polyval(py, t_fit);
    
    % Plot original data and fitted curve
    plot(x, y, 'ro', 'DisplayName', 'Original Data');
    hold on;
    plot(x_fit, y_fit, 'b-', 'DisplayName', 'Fitted Curve');
    hold on;
end


for i = 1:2:length(polynoms)
    px_1 = polynoms{i}{1};
    py_1 = polynoms{i}{2};
    t_1 = polynoms{i}{3};

    px_2 = polynoms{i+1}{1};
    py_2 = polynoms{i+1}{2};
    t_2 = polynoms{i+1}{3};


    px_mid = (px_1 + px_2) ./ 2;
    py_mid = (py_1 + py_2) ./ 2;
    t_mid = [t_1 t_2];
    % t_min = max(min(t_1), min(t_2))+150;
    % t_max = min(max(t_1), max(t_2))-150;
    t_min = max(min(t_1), min(t_2));
    t_max = min(max(t_1), max(t_2));

    % Generate the fitted curve
    t_fit = linspace(t_min, t_max, 100);
    x_fit = polyval(px_mid, t_fit);
    y_fit = polyval(py_mid, t_fit);

    plot(x_fit, y_fit, 'm-', 'DisplayName', 'Middle line');
    hold on;
end

xlabel('x'); ylabel('y'); legend; grid on;
title('Parametric Polynomial Fit');

