function [poly_x, poly_y, t] = CurvePolyfit(x, y, num_interp, poly_degree)

% Generate new finer t vector
t_original = 1:length(x); % Original indices
t_fine = linspace(1, length(x), (length(x)-1)*num_interp + 1);

% Interpolate using interp1
x_fine = interp1(t_original, x, t_fine, 'linear');
y_fine = interp1(t_original, y, t_fine, 'linear');

x = x_fine
y = y_fine
t = t_fine
%curve2 = np.array([[313, 80], [251, 82], [251, 87], [297, 180], [305, 187]])

% Fit polynomials for x and y with respect to t
n = poly_degree; % Degree of polynomial
poly_x = polyfit(t, x, n); % Polynomial coefficients for x
poly_y = polyfit(t, y, n); % Polynomial coefficients for y
end