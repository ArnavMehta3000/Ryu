-- Rule to include packages
rule("CommonPackages")
	on_load(function (target)
		target:add("packages", "spdlog")
		target:add("packages", "backward-cpp")
		target:add("packages", "nlohmann_json")
	end)
rule_end()
