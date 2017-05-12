-- name: teleport.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

tag = "sk_teleport"

hostile = false
targeted = false
cooldown = 20
cost = 0

name = "Teleport"
short = "Tport"
description = "Teleports you to target location"

range = function (user, target)
	return 0, 10000
end

condition = function (user, target)
	return not user:dead()
end

action = function (user, target)
	user:move(target)
end