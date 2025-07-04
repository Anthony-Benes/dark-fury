-- Simple JSON encoder function
local function encode_json(obj)
    local function encode_value(value)
        if type(value) == "table" then
            -- Check if it's an array (indexed by numeric keys)
            local is_array = true
            for k, _ in pairs(value) do
                if type(k) ~= "number" then
                    is_array = false
                    break
                end
            end

            local items = {}
            if is_array then
                -- It's an array, encode as a JSON array
                for _, v in ipairs(value) do
                    table.insert(items, encode_value(v))
                end
                return "[" .. table.concat(items, ",") .. "]"
            else
                -- It's an object, encode as a JSON object
                for k, v in pairs(value) do
                    local key = type(k) == "string" and '"' .. k .. '"' or k
                    table.insert(items, key .. ":" .. encode_value(v))
                end
                return "{" .. table.concat(items, ",") .. "}"
            end
        elseif type(value) == "string" then
            return '"' .. value:gsub('"', '\\"') .. '"'
        elseif type(value) == "number" then
            return tostring(value)
        elseif type(value) == "boolean" then
            return value and "true" or "false"
        else
            return "null"
        end
    end

    return encode_value(obj)
end

-- Define a table to store the project data
local project_data = {}

-- Iterate through the projects defined in the workspace
for _, prj in ipairs(workspace().projects) do
    local prj_info = {
        name = prj.name,
        files = {},
        includedirs = {},
        links = {}
    }

    -- Collect source files (ensure they're in an array format)
    for _, file in ipairs(prj.files) do
        table.insert(prj_info.files, file)
    end

    -- Collect included directories (ensure they're in an array format)
    for _, include_dir in ipairs(prj.includedirs) do
        table.insert(prj_info.includedirs, include_dir)
    end

    -- Collect linked libraries (ensure they're in an array format)
    for _, link in ipairs(prj.links) do
        table.insert(prj_info.links, link)
    end

    -- Add project data to the list
    table.insert(project_data, prj_info)
end

-- Write the project data to a JSON file
local json_data = encode_json({ projects = project_data }) -- Wrap in 'projects' key
local output_file = io.open("premake.json", "w")
output_file:write(json_data)
output_file:close()
