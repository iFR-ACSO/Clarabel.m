function soc = SecondOrderConeT(dim)
    % alpha field added just for interface; otherwise concatenating
        % structs not possible
        soc = struct('coneName','SecondOrderCone','dim',dim,'alpha',0);
end