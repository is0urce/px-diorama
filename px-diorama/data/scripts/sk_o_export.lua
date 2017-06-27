-- name: sk_o_export.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

-- editor mode: exports selected object

tag = "sk_o_export"

-- state attributes

targeted = true

name = "Export"
description = "Exports target"

-- returns true if skill can be used

condition = function(user, target)
	return true
end

-- action taken if skill is used

action = function(user, target)
	game:export_unit(target:position())
end