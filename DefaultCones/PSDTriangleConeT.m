function psd = PSDTriangleConeT(dim)
    % alpha field added just for interface; otherwise concatenating
        % structs not possible
        psd = struct('coneName','PSDTriangleCone','dim',dim,'alpha',0);
end