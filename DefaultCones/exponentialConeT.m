
function zeroCone = exponentialConeT()
        % alpha field added just for interface; otherwise concatenating
        % structs not possible
        zeroCone = struct('coneName','ExponentialCone','dim',1,'alpha',0);
end