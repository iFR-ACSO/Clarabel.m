function NonnegativeCone = NonnegativeConeT(dim)
    % alpha field added just for interface; otherwise concatenating
        % structs not possible
        NonnegativeCone = struct('coneName','NonnegativeCone','dim',dim,'alpha',0);
end