close all;
clear;
clc;

% Input datasets
values = {
        {[1 76 78 60 53 54 52 44 40 36 37 33 31 26],...
            [80 80 82 121 130 132 137 150 160 164 165 172 180 184]},...
        {[313 253 251 251 279 297 301 305],...
            [80 80 82 87 141 180 185 187]},...
        {[207 211 214 224 314],...
            [12 15 23 37 37]},...
        {[110 100 96 0],...
            [14 33 37 38]}
    };

polynoms = {}; % Store polynomial fits

% Fit polynomials for each dataset
for i = 1:length(values)
    x = values{i}{1};
    y = values{i}{2};
    t = 1:length(x);
    
    % Fit polynomial with interpolation
    [px, py, t] = CurvePolyfit(x, y, 100, 15);
    polynoms{end+1} = {px, py, t}; % Store polynomial coefficients and t values
    
    % Generate the fitted curve for visualization
    t_fit = linspace(min(t), max(t), 100);
    x_fit = polyval(px, t_fit);
    y_fit = polyval(py, t_fit);
    
    % Plot original data and fitted curve
    plot(x, y, 'ro', 'DisplayName', sprintf('Original Data %d', i));
    hold on;
    plot(x_fit, y_fit, 'b-', 'DisplayName', sprintf('Fitted Curve %d', i));
end

% Calculate and plot middle polynomials
for i = 1:2:length(polynoms)-1
    % Retrieve consecutive polynomials
    px_1 = polynoms{i}{1};
    py_1 = polynoms{i}{2};
    t_1 = polynoms{i}{3};

    px_2 = polynoms{i+1}{1};
    py_2 = polynoms{i+1}{2};
    t_2 = polynoms{i+1}{3};

    % Determine overlapping t range
    t_min = max(min(t_1), min(t_2));
    t_max = min(max(t_1), max(t_2));
    t_fit = linspace(t_min, t_max, 500); % High resolution for better sharpness

    % Interpolate values for both polynomials
    x_1 = polyval(px_1, t_fit);
    y_1 = polyval(py_1, t_fit);
    x_2 = polyval(px_2, t_fit);
    y_2 = polyval(py_2, t_fit);

    % Compute derivatives to identify sharp regions
    dx_1 = gradient(x_1, t_fit);
    dy_1 = gradient(y_1, t_fit);
    dx_2 = gradient(x_2, t_fit);
    dy_2 = gradient(y_2, t_fit);

    % Compute curvature (approximated by the magnitude of the derivative)
    curvature_1 = sqrt(dx_1.^2 + dy_1.^2);
    curvature_2 = sqrt(dx_2.^2 + dy_2.^2);

    % Dynamically assign weights based on sharpness
    total_curvature = curvature_1 + curvature_2;
    weights_1 = curvature_1 ./ total_curvature;
    weights_2 = curvature_2 ./ total_curvature;

    % Compute dynamically weighted midpoints
    x_mid = weights_1 .* x_1 + weights_2 .* x_2;
    y_mid = weights_1 .* y_1 + weights_2 .* y_2;

    % Fit new polynomials to the midpoints
    px_mid = polyfit(t_fit, x_mid, 15);
    py_mid = polyfit(t_fit, y_mid, 15);

    % Generate the midline curve
    x_fit_mid = polyval(px_mid, t_fit);
    y_fit_mid = polyval(py_mid, t_fit);

    % Plot the midline
    plot(x_fit_mid, y_fit_mid, 'g--', 'DisplayName', sprintf('Middle Curve %d-%d', i, i+1));
end

% Add labels, legend, and grid
xlabel('x');
ylabel('y');
legend('Location', 'best');
grid on;
title('Parametric Polynomial Fit with Enhanced Sharp Midlines');
hold off;
