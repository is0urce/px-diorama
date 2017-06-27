-- name: sk_o_import.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

-- editor mode: exports targeted objects

tag = "sk_o_import"

-- state attributes

targeted = false

name = "Export"
description = "Exports objects in targeted area"

-- returns true if skill can be used

condition = function(user, target)
	return true
end

-- action taken if skill is used

action = function(user, target)
	game:import("m_skeleton", target:position())
end