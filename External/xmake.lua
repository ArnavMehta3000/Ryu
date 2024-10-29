-- Create phony targets for each external
for _, dir in ipairs(os.dirs(os.scriptdir() .. "/External/**")) do
	local ext_name = path.basename(dir)
	target(ext_name)
		set_group("Ryu/External")
		set_kind("phony")
		add_includedirs(dir, { public = false })
		add_headerfiles(dir .."/**.h")

		-- Check if target has cpp files
		on_config(function(target)
			for _, files in ipairs(os.filedirs(dir .. "/**.cpp")) do
				target:add("files", dir .. "/**.cpp")
				break;  -- Dip out on first cpp found
			end
		end)

	target_end()
end


-- A phony target to include all external source files
target("RyuExternals")
	set_group("Ryu/External")
	set_kind("phony")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_files("**.cpp")

	on_config(function(target)
		for _, dir in ipairs(os.dirs("$(scriptdir)/External/**")) do
			local ext_name = path.basename(dir)
			target:add("deps", ext_name)
		end
	end)
target_end()
