-- name: teleport.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

tag = "sk_teleport"

-- state attributes

hostile = false
targeted = false
instant = true

name = "Teleport"
short = "Tport"
description = "Teleports you to target location"

-- returns true if skill can be used

condition = function (user, target)
	return not user:dead()
end

-- action taken if skill is used

action = function (user, target)
	user:place(target)
end