-- Rule to include config files
rule("Configs")
    on_config(function (target)
        local config_dir = "$(buildir)/Config"

        target:add("includedirs", config_dir, { public = true })
        target:add("options", "loglevel")

        target:set("configdir", config_dir)

        if has_config("loglevel") then
            cprint(format("${cyan}%s log verbosity configured to: ${green}%s${clear}", target:name(), get_config("loglevel")))

            local level = string.upper(get_config("loglevel"))

            if level == "TRACE" then
                target:set("configvar", "HAS_LOG_WARN", 1)
                target:set("configvar", "HAS_LOG_INFO", 1)
                target:set("configvar", "HAS_LOG_DEBUG", 1)
                target:set("configvar", "HAS_LOG_TRACE", 1)
            elseif level == "DEBUG" then
                target:set("configvar", "HAS_LOG_WARN", 1)
                target:set("configvar", "HAS_LOG_INFO", 1)
                target:set("configvar", "HAS_LOG_DEBUG", 1)
                target:set("configvar", "HAS_LOG_TRACE", 0)
            elseif level == "INFO" then
                target:set("configvar", "HAS_LOG_WARN", 1)
                target:set("configvar", "HAS_LOG_INFO", 1)
                target:set("configvar", "HAS_LOG_DEBUG", 0)
                target:set("configvar", "HAS_LOG_TRACE", 0)
            else
                target:set("configvar", "HAS_LOG_WARN", 1)
                target:set("configvar", "HAS_LOG_INFO", 0)
                target:set("configvar", "HAS_LOG_DEBUG", 0)
                target:set("configvar", "HAS_LOG_TRACE", 0)
            end
        else
            local level = is_mode("debug") and "Trace" or "Info"
            cprint(format("${cyan}%s(loglevel)${clear} config not set. Setting log verbosity level to: ${green}%s", target:name(), level))

            -- Warning and info is always enabled if config is not set
            target:set("configvar", "HAS_LOG_WARN", 1)
            target:set("configvar", "HAS_LOG_INFO", 1)

            if string.upper(level) == "INFO" then
                target:set("configvar", "HAS_LOG_DEBUG", 0)
                target:set("configvar", "HAS_LOG_TRACE", 0)
            end

            if string.upper(level) == "TRACE" then
                target:set("configvar", "HAS_LOG_DEBUG", 1)
                target:set("configvar", "HAS_LOG_TRACE", 1)
            end
        end

        target:add("configfiles", path.join("$(projectdir)", "Config", "*.h.in"))
    end)
rule_end()
