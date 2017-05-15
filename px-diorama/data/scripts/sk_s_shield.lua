-- name: shield.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

tag = "sk_melee"

-- state attributes

targeted = false

name = "Shield Up"
description = "90% resist for 2 turns"

-- returns true if skill can be used

condition = function(user, target)
	return not user:dead()
end

-- action taken if skill is used

action = function(user, target)
	-- apply buff
end