local JSON = require 'libcjson'


local tab = "    "
local tos

local quoted = function(str)
    str = str:gsub('"', '\\"')
    str = str:gsub('\n', '\\n')
    str = str:gsub('\r', '\\r')
    str = str:gsub('\t', '\\t')
    return str
end

-- Colors:
local c = {
    none = '\27[0m',
    black = '\27[0;30m',
    red = '\27[0;31m',
    green = '\27[0;32m',
    yellow = '\27[0;33m',
    blue = '\27[0;34m',
    magenta = '\27[0;35m',
    cyan = '\27[0;36m',
    white = '\27[0;37m',
    Black = '\27[1;30m',
    Red = '\27[1;31m',
    Green = '\27[1;32m',
    Yellow = '\27[1;33m',
    Blue = '\27[1;34m',
    Magenta = '\27[1;35m',
    Cyan = '\27[1;36m',
    White = '\27[1;37m',
    _black = '\27[40m',
    _red = '\27[41m',
    _green = '\27[42m',
    _yellow = '\27[43m',
    _blue = '\27[44m',
    _magenta = '\27[45m',
    _cyan = '\27[46m',
    _white = '\27[47m'
}

-- Apply:
local apply
if torch and false and torch.isatty(io.stdout) then
    apply = function(color, txt) return c[color] .. txt .. c.none end
else
   apply = function(color, txt) return txt end
end

local function colorize(object)
   -- Type?
    if object == nil then
        return apply('Black', 'nil')
    elseif type(object) == 'number' then
        return apply('cyan', tostring(object))
    elseif type(object) == 'boolean' then
        return apply('blue', tostring(object))
    elseif type(object) == 'string' then
        return apply('Yellow', object)
    elseif type(object) == 'function' then
        return apply('magenta', tostring(object))
    elseif type(object) == 'userdata' or type(object) == 'cdata' then
        local tp = torch.typename(object) or ''
        if tp:find('torch.*Tensor') then
            tp = sizestr(object)
        elseif tp:find('torch.*Storage') then
            tp = sizestr(object)
        else
            tp = tostring(object)
        end
        if tp ~= '' then
            return apply('red', tp)
        else
            return apply('red', tostring(object))
        end
    elseif type(object) == 'table' then
        return apply('green', tostring(object))
    else
        return apply('black', tostring(object))
    end
end


local tostringArray = function(jsonArray, indent)
    indent = indent or ""
    local str = { "[" }

    for i = 1, #jsonArray do
        table.insert(str, 
            indent .. tab .. ( tos(jsonArray[i], indent .. tab) or "nil") .. ',' )
    end

    table.insert(str, indent .. "]")
    return table.concat(str, "\n")
end
local tostringObject = function(value, indent)
    indent = indent or ""
    local str = { "{" }

    local keys = JSON.keys(value)

    for i = 1, #keys do
        table.insert(str, 
            indent .. tab .. apply('Green',quoted(keys[i]))
            .. " : " .. ( tos(value[keys[i]], indent .. tab) or "nil") .. ',' )
    end

    table.insert(str, indent .. "}")
    return table.concat(str, "\n")
end

tos = function(value, indent)
    indent = indent or ""

    if getmetatable(value) == JSON.mt then
        local t = JSON.type(value)
        if t == "JsonArray" then
            return tostringArray(value, indent)
        elseif t == "JsonObject" then
            return tostringObject(value, indent)
        end
    else
        if type(value) == "string" then
            return colorize('"' .. quoted(value) .. '"' )
        else
            return colorize(value)
        end
    end
end

JSON.mt.__tostring = tos

JSON.pairs = function( self )
    assert( getmetatable(self) == JSON.mt, "expected JsonArray or JsonObject")

    local t = JSON.type(self)
    if t == "JsonArray" then
        local len = #self
        local f = function(_, i )
            if i == len then
                return nil
            else
                return i+1, self[i+1]
            end
        end
        return f, nil, 0
    elseif t == "JsonObject" then
        local keys = JSON.keys(self)
        local len = #keys
        local i = 0
        local f = function()
            i = i + 1
            if i > len then
                return nil
            else
                return keys[i], self[ keys[i] ]
            end
        end
        return f, i, 0
    end
end
JSON.ipairs = JSON.pairs

local old_pairs = pairs
pairs = function(val)
    if getmetatable(val) == JSON.mt then
        return JSON.pairs(val)
    else
        return old_pairs(val)
    end
end

local old_ipairs = ipairs
ipairs = function(val)
    if getmetatable(val) == JSON.mt then
        return JSON.ipairs(val)
    else
        return old_ipairs(val)
    end
end

return JSON
