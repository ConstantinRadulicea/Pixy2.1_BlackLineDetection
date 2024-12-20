% Example data
clear;
clc;

x = [1 76 78 60 53 54 52 44 40 36 37 33 31 26];
y = [80 80 82 121 130 132 137 150 160 164 165 172 180 184];
t = 1:length(x);

[px py t] = CurvePolyfit(x, y, 10, 15);

% Generate the fitted curve
t_fit = linspace(min(t), max(t), 100);
x_fit = polyval(px, t_fit);
y_fit = polyval(py, t_fit);

% Plot original data and fitted curve
plot(x, y, 'ro', 'DisplayName', 'Original Data'); hold on;
plot(x_fit, y_fit, 'b-', 'DisplayName', 'Fitted Curve');
xlabel('x'); ylabel('y'); legend; grid on;
title('Parametric Polynomial Fit');


x = [313 253 251 251 279 297 301 305];
y = [80 80 82 87 141 180 185 187];
t = 1:length(x);

[px py t] = CurvePolyfit(x, y, 10, 15);

% Generate the fitted curve
t_fit = linspace(min(t), max(t), 100);
x_fit = polyval(px, t_fit);
y_fit = polyval(py, t_fit);

% Plot original data and fitted curve
plot(x, y, 'ro', 'DisplayName', 'Original Data'); hold on;
plot(x_fit, y_fit, 'b-', 'DisplayName', 'Fitted Curve');
xlabel('x'); ylabel('y'); legend; grid on;
title('Parametric Polynomial Fit');


x = [207 211 214 224 314];
y = [12 15 23 37 37];
t = 1:length(x);

[px py t] = CurvePolyfit(x, y, 10, 15);

% Generate the fitted curve
t_fit = linspace(min(t), max(t), 100);
x_fit = polyval(px, t_fit);
y_fit = polyval(py, t_fit);

% Plot original data and fitted curve
plot(x, y, 'ro', 'DisplayName', 'Original Data'); hold on;
plot(x_fit, y_fit, 'b-', 'DisplayName', 'Fitted Curve');
xlabel('x'); ylabel('y'); legend; grid on;
title('Parametric Polynomial Fit');


x = [110 100 96 0];
y = [14 33 37 38];
t = 1:length(x);

[px py t] = CurvePolyfit(x, y, 10, 15);

% Generate the fitted curve
t_fit = linspace(min(t), max(t), 100);
x_fit = polyval(px, t_fit);
y_fit = polyval(py, t_fit);

% Plot original data and fitted curve
plot(x, y, 'ro', 'DisplayName', 'Original Data'); hold on;
plot(x_fit, y_fit, 'b-', 'DisplayName', 'Fitted Curve');
xlabel('x'); ylabel('y'); legend; grid on;
title('Parametric Polynomial Fit');
