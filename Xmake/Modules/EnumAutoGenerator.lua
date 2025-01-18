import("core.project.project")
import("core.base.json")

-- Patterns to match enum declarations and values
local ENUM_PATTERN = "TENUM%s*%(.-%)%s*\n%s*enum%s+class%s+([%w_]+)"
-- Match both formats:
-- 1. Name = Value
-- 2. Just Name
local ENUM_VALUE_PATTERN = "([%w_]+)%s*(?:=[^,\n}]+)?,?"
local NAMESPACE_PATTERN = "namespace%s+([%w_:]+)%s*{"

function extract_namespace(content)
	local namespace = ""
	for ns in content:gmatch(NAMESPACE_PATTERN) do
		namespace = ns
	end
	return namespace
end

function extract_enums(content)
	local enums = {}
	local current_pos = 1
	
	while true do
		local enum_start, enum_end, enum_name = content:find(ENUM_PATTERN, current_pos)
		if not enum_start then break end
		
		-- Find the matching closing brace for the enum
		local brace_count = 0
		local enum_body_start = content:find("{", enum_end)
		local enum_body_end = enum_body_start
		
		for i = enum_body_start, #content do
			local char = content:sub(i, i)
			if char == "{" then
				brace_count = brace_count + 1
			elseif char == "}" then
				brace_count = brace_count - 1
				if brace_count == 0 then
					enum_body_end = i
					break
				end
			end
		end
		
		-- Extract enum body
		local enum_body = content:sub(enum_body_start, enum_body_end)
		
		-- Extract enum values
		local values = {}
		-- Extract enum values
		local values = {}
		
		-- First try to match entries with explicit values (name = value)
		for line in enum_body:gmatch("[^\n]+") do
			-- Skip comments and empty lines
			if not line:match("^%s*//") and not line:match("^%s*$") and not line:match("^%s*{") then
				-- Try to match the enum name before any equals sign
				local name = line:match("^%s*([%w_]+)%s*=")
				if name and not name:match("^DECLARE") then
					table.insert(values, name)
				else
					-- Match simple entries without equals sign (handles both comma and closing brace)
					local simple_name = line:match("^%s*([%w_]+)%s*[,}]")
					if simple_name and not simple_name:match("^DECLARE") then
						table.insert(values, simple_name)
					end
				end
			end
		end
		
		-- Store enum info
		table.insert(enums, {
			name = enum_name,
			values = values
		})
		
		current_pos = enum_body_end
	end
	
	return enums
end

function generate_tostring_implementation(namespace, enum)
	local lines = {}
	table.insert(lines, string.format([[
template <>
inline constexpr std::string_view Ryu::EnumToString(%s::%s value)
{
	switch(value)
	{
		using enum %s::%s;]], namespace, enum.name, namespace, enum.name))
	
	-- Add cases for each enum value
	for _, value in ipairs(enum.values) do
		-- Skip entries starting with underscore (they're reserved/internal)
		if not value:match("^_") then
			table.insert(lines, string.format("        case %s: return \"%s\";", value, value))
		end
	end
	
	-- Add default case and closing braces
	table.insert(lines, [[
		default: return "<Unknown>";
	}
}]])
	
	return table.concat(lines, "\n")
end

function process_file(target, header_file)
	-- Read header file content
	local content = io.readfile(header_file)
	
	-- Extract namespace and enums
	local namespace = extract_namespace(content)
	local enums = extract_enums(content)
	
	if #enums > 0 then
		-- Generate output file path
		local output_file = path.join(target:autogendir(), path.basename(header_file) .. ".gen.h")
		
		-- Generate content
		local lines = {}
		table.insert(lines, "#pragma once")
		-- Get relative path from autogen dir to original header
		local rel_header_path = path.relative(header_file, target:scriptdir())
		--table.insert(lines, string.format("#include \"%s\"", rel_header_path))
		
		-- Add ToString implementation for each enum
		for _, enum in ipairs(enums) do
			table.insert(lines, generate_tostring_implementation(namespace, enum))
			table.insert(lines, "")  -- Add blank line between implementations
		end
		
		-- Write to file
		io.writefile(output_file, table.concat(lines, "\n"))
		return true, output_file
	end
	return false, nil
end

function process_target(target_name)
	local target = project.target(target_name)
	local header_files = os.files(path.join(target:scriptdir(), "**.h"))
	
	for _, header_file in ipairs(header_files) do
		local success, output_file = process_file(target, header_file)
		if success then
			cprint(format("${dim green}[EnumAutoGenerator]${clear} ${dim}Generated file: ${dim}%s${clear}", output_file))
		end
	end
end

function main(target_name)
	if target_name then
		process_target(target_name)
	end
end
