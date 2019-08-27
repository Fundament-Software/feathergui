local tunpack = unpack or table.unpack

return macro(function(...)
    local args = {...}
    local function build(store, condition, expression, ...)
        if expression == nil then condition, expression = expression, condition end
        local subargs = {...}

        if condition ~= nil then
            return quote
                if [condition] then
                    [store] = [expression]
                else
                    [build(store, tunpack(subargs))]
                end
            end
        else
            return quote
                [store] = [expression]
            end
        end
    end
    local _, expr1 = ...
    local store = symbol(expr1:gettype(), "condres")
    return quote
        var [store]
        [build(store, tunpack(args))]
    in
        [store]
    end
end)