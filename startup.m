warning('on', 'verbose');
[path, ~, ~] = fileparts(mfilename('fullpath'));
if ispc
	startupexist = strcmpi(path, fullfile(matlabroot, 'bin')) == 0;
else
	startupexist = strcmp(path, fullfile(matlabroot, 'bin')) == 0;
end
if startupexist
	if exist(fullfile(matlabroot, 'bin', 'startup.m'), 'file')
		run(fullfile(matlabroot, 'bin', 'startup.m'));
	end
end
%% set path
addpath(fullfile(path));
addpath(fullfile(path, 'lib'));
%%
clear startupexist path;
dbstop if error;