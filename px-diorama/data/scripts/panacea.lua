-- name: melee.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

panacea = {
	tag = "sk_panacea",
	name = "Panacea",
	short = "Pncea",
	description = "Quaff...",

	hostile = false,
	targeted = false,
	cooldown = 20,

	cost = function(user, target)
		return 0
	end,

	range = function(user, target)
		return 0, 10000
	end,

	condition = function(user, target)
		return not user:dead()
	end,

	action = function(user, target)
		local restore = user:health() * 0.6
		user:restore(math.floor(restore))
	end
}