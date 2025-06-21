-- Enum to header logic
import("core.base.option")
import("core.base.json")

local function _write_header(enum_name, base, enable_bitmask_ops, types, outfile, tostringfile)
	-- Each table entry is a line in the file
	local file_lines = {}

	-- Enum class declaration
	if base == json.null then
		table.insert(file_lines, format("enum class %s", enum_name))
	else
		table.insert(file_lines, format("enum class %s : %s", enum_name, base))
	end
	table.insert(file_lines, "{")

	-- Types
	for _, kv in ipairs(types) do
		-- No value for this enum type
		if kv.Value == json.null then
			table.insert(file_lines, format("\t%s,", kv.Key))
		else  -- Value provided
			if type(kv.Value) == "number" then
				table.insert(file_lines, format("\t%s = %u,", kv.Key, kv.Value))
			elseif type(kv.Value) == "string" then
				table.insert(file_lines, format("\t%s = %s,", kv.Key, kv.Value))
			end
		end
	end
	table.insert(file_lines, "}")

	-- Bitmask ops
	if enable_bitmask_ops then
		table.insert(file_lines, format("RYU_ENUM_ENABLE_BITMASK_OPERATORS(%s)", enum_name))
	end

	-- Write the main generated file
	io.writefile(outfile, table.concat(file_lines, "\n"))

	-- EnumToString
	local tostringfile_lines = {}
	table.insert(tostringfile_lines, "template<>")
	table.insert(tostringfile_lines, format("inline constexpr std::string_view EnumToString<%s>(%s value)", enum_name, enum_name))
	table.insert(tostringfile_lines, "{")
	table.insert(tostringfile_lines, "\tswitch(value)")
	table.insert(tostringfile_lines, "\t{")
	for _, kv in ipairs(types) do
		table.insert(tostringfile_lines, format("\t\tcase %s: return \"%s\";", kv.Key, kv.Key))
	end
	table.insert(tostringfile_lines, "\t\tdefault: return \"<UNKNOWN>\";")
	table.insert(tostringfile_lines, "\t}")
	table.insert(tostringfile_lines, "}")

	io.writefile(tostringfile, table.concat(tostringfile_lines, "\n"))
	print(format("[Ryu::EnumToHeader] Enum header generated for type: %s", enum_name))
end

function main()
	local source_files = option.get("files")
	local output_dir = option.get("output_dir")

	for _, json_file in ipairs(source_files) do
		-- Read file as json
		local enum_json = json.loadfile(json_file)

		-- Get info
		local enum_name = enum_json["Name"]
		local enum_base = enum_json["Base"]
		local enable_bitmask_ops = enum_json["EnableBitMaskOps"]
		local types = enum_json["Types"]

		-- Get output file (AutogenDir + FileName + "<ToString>.Generated.h")
		local outfile = path.join(output_dir, path.basename(json_file) .. ".Generated.h")
		local tostring_outfile = path.join(output_dir, path.basename(json_file) .. "ToString.Generated.h")
		_write_header(enum_name, enum_base, enable_bitmask_ops, types, outfile, tostring_outfile)
	end
end
