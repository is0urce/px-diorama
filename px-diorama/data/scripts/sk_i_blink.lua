-- name: sk_i_blink.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

blink = {
	tag = "sk_blink",
	name = "Blink",
	short = "Blink",
	description = "Teleports you 5 (and only 5) steps away",

	hostile = false,
	targeted = false, -- target location, not units
	cooldown = 20,

	cost = function(user, target)
		return 0
	end,

	range = function(user, target)
		return 5, 5
	end,

	condition = function(user, target)
		local distance = game.distance(user:position(), target)
		return not user:dead()
			and distance == 5
	end,

	action = function(user, target)
		user:move(target)
	end
}