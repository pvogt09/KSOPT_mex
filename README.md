# KSOPT_mex
This repository contains a Matlab MEX interface for the nonlinear constrained multiobjective optimization algorithm [KSOPT](https://software.nasa.gov/software/LAR-18488-1) by Gregory A. Wrenn.

The repository is based on the KSOPT code from [madebr/pyOpt](https://github.com/madebr/pyOpt/tree/master/pyOpt/pyKSOPT/source) converted from Fortran to C with [`f2c`](https://www.netlib.org/f2c/).

## Compiling the interface
The MEX file can be created by invoking
```matlab
compile.ksopt;
```
and copies the created MEX file `ksopt_optimize.mex*` to the root directory.

## MEX interface
The interface of `ksopt_optimize` is similar to `fmincon` or `fminimax` from the Optimization Toolbox.
The input arguments are:
* `fun`:		objective function to minimize
* `x_0`:		initial value or matrix of initial values or StartPointSet of initial values to start optimization from
* `A`:			matrix of linear inequlity constraints
* `b`:			upper limits for linear inequality constraints
* `Aeq`:		matrix for linear equality constraints
* `beq`:		upper limits for linear equality constraints
* `lb`:			fixed lower bounds on x
* `ub`:			fixed upper bounds on x
* `nonlcon`:	nonlinear inequality and equality constraints
* `options`:	settings for optimization
* `varargin`:	additional arguments for objective function

A description of the input arguments can be found in the Matlab documentation of e.g. [`fminimax`](https://de.mathworks.com/help/optim/ug/fminimax.html).
The argument `options` is a structure with the following fields
```matlab
options = struct(...
  'MaxIterations',                1000,...
  'MaxFunctionEvaluations',       1000,...
  'MaxTime',                      60,...
  'ObjectiveLimit',               -1E20,...
  'FiniteDifferenceStepSize',     1E-6,...
  'FiniteDifferenceStepSizeMin',  1E-6,...
  'OptimalityTolerance',          1E-6,...
  'OptimalityToleranceAbs',       1E-6,...
  'RhoIncrement',                 0.01,...
  'RhoMin',                       0.001,...
  'RhoMax',                       0.1,...
  'Display',                      'iter-detailed',...
  'Scale',                        1,...
  'ScaleVector',                  ones(size(x_0)),...
  'SpecifyObjectiveGradient',     true,...
  'SpecifyConstraintGradient',    true...
);
```
or can be of type `optimoptions` or `optimset`.

The function returns:
* `x`:			optimal value
* `fval`:		function value at optimal value
* `exitflag`:	optimization result indicator
* `output`:		structure with information about optimization

## Example
```matlab
a1 = [1, 1];
b1 = [-1, 1];
c1 = [0, -1];
a0 = 2;
b0 = -3;
c0 = 4;
fun = @(x) [a1*x + a0; b1*x + b0; c1*x + c0];
x_0 = [0; 0];
A = [];
b = [];
Aeq = [];
beq = [];
lb = [];
ub = [];
nonlcon = [];
[x, fval, exitflag, output] = ksopt_optimize(fun, x_0, A, b, Aeq, beq, lb, ub, nonlcon)
```

