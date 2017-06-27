-- name: sk_o_import.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

-- editor mode: spawn object

tag = "sk_o_import"

-- state attributes

targeted = false

name = "import"
description = "Spawn object"

-- returns true if skill can be used

condition = function(user, target)
	return true
end

-- action taken if skill is used

action = function(user, target)
	game:spawn("m_snail", target)
end