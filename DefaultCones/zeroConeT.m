function zeroCone = zeroConeT(dim)
    % alpha field added just for interface; otherwise concatenating
        % structs not possible
        zeroCone = struct('coneName','ZeroCone','dim',dim,'alpha',0);
end