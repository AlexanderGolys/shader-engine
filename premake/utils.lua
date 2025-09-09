function splitString(inputStr, sep)
    if sep == nil then
        sep = "%s"
    end
    local t = {}
    for str in string.gmatch(inputStr, "([^" .. sep .. "]+)") do
        table.insert(t, str)
    end
    return t
end

local function _trim(s)
    local i, j = 1, #s
    while i <= j and (s:sub(i,i) == ' ' or s:sub(i,i) == '\t' or s:sub(i,i) == '\r') do i = i + 1 end
    while j >= i and (s:sub(j,j) == ' ' or s:sub(j,j) == '\t' or s:sub(j,j) == '\r') do j = j - 1 end
    if j < i then return "" end
    return s:sub(i, j)
end

function readConfig(filePath, keyword)
    local file = io.open(filePath, "r")
    if not file then
        if keyword then return nil end
        return {}
    end

    local all = {}
    for line in file:lines() do
        local t = _trim(line)
        if t ~= "" then
            if t:sub(1,1) ~= "#" and t:sub(1,2) ~= "//" then
                local pos = t:find(":", 1, true)
                if pos then
                    local k = _trim(t:sub(1, pos - 1))
                    if k ~= "" then
                        local v = _trim(t:sub(pos + 1))
                        all[k] = v
                        if keyword and k == keyword then
                            file:close()
                            return v
                        end
                    end
                end
            end
        end
    end

    file:close()
    if keyword then return nil end
    return all
end

function parseJSON(filePath)
    local file = io.open(filePath, "r")
    if not file then return nil end

    local content = file:read("*a")
    file:close()

    -- Convert JSON to Lua table syntax (basic)
    content = content:gsub('"%s*:', '" :') -- ensure colon spacing
    content = content:gsub('"(.-)"%s*:', '[%1]=') -- "key": -> [key]=
    content = content:gsub('"(.-)"', "'%1'") -- "value" -> 'value'
    content = content:gsub("true", "true")
    content = content:gsub("false", "false")
    content = content:gsub("null", "nil")

    local status, result = pcall(function() return load("return " .. content)() end)
    if status then
        return result
    else
        return nil
    end
end
