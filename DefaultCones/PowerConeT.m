function PowerCone = PowerConeT(dim)
    % alpha field added just for interface; otherwise concatenating
        % structs not possible
        PowerCone = struct('coneName','PowerCone','dim',dim,'alpha',0);
end