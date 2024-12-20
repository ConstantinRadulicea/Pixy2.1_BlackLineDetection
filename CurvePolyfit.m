function [poly_x, poly_y, t] = CurvePolyfit(x, y, num_interp, poly_degree)
% CurvePolyfit interpolates data points and fits polynomials of specified degree
%
% Inputs:
%   x           - Vector of x-coordinates (independent variable)
%   y           - Vector of y-coordinates (dependent variable)
%   num_interp  - Number of interpolated points between each pair of data points
%   poly_degree - Degree of the polynomial to fit
%
% Outputs:
%   poly_x      - Polynomial coefficients for x(t)
%   poly_y      - Polynomial coefficients for y(t)
%   t           - Interpolated parameter vector

    % Validate inputs
    if length(x) ~= length(y)
        error('Vectors x and y must be of the same length.');
    end
    if num_interp < 1
        error('num_interp must be a positive integer greater than or equal to 1.');
    end
    if poly_degree < 1
        error('poly_degree must be at least 1.');
    end

    % Generate new finer t vector
    % t_original = 1:length(x); % Original indices
    t_original = 0:num_interp / (length(x)-1):num_interp;
    % t_fine = linspace(1, length(x), (length(x)-1)*num_interp + 1);
    t_fine = linspace(t_original(1), t_original(end), (t_original(end)-1)*num_interp + 1);

    % Interpolate using interp1
    x_fine = interp1(t_original, x, t_fine, 'linear');
    y_fine = interp1(t_original, y, t_fine, 'linear');

    % Assign interpolated values
    x = x_fine;
    y = y_fine;
    t = t_fine;

    % Fit polynomials for x and y with respect to t
    poly_x = polyfit(t, x, poly_degree); % Polynomial coefficients for x
    poly_y = polyfit(t, y, poly_degree); % Polynomial coefficients for y;
end
