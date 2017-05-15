-- name: ambush.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

ambush = {
	tag = "sk_ambush",
	name = "Ambush",
	short = "Ambsh",
	description = "Do 100% damage or 300% damage if target has health",

	hostile = true,
	targeted = true,
	cooldown = 0,

	cost = function(user, target)
		return 5
	end,

	range = function(user, target) -- melee distance
		return 0, 1
	end,

	condition = function(user, target)
		local distance = game.distance(user, target)
		local min, max = ambush.range(user, target)
		return user ~= target
			and not user:dead()
			and not target:dead()
			and user:energy() >= ambush.cost(user, target) -- have mana
			and min <= distance and distance <= max -- in range
			and game.reputation(user, target) < 0 -- friend-or-foe check, negative is hostile
	end,

	action = function(user, target)
		user:drain(ambush.cost(user, target))

		local dmg, type, hit, crit = game.hit(user, target)
		local msg = "miss"

		if hit then
			if target:health() == target:health_max() then
				dmg = dmg * 3
			end
			
			target:damage(dmg)

			msg = -dmg
			if crit then
				msg = msg + " critical!"
			end
		end
		game.message(target, msg)
	end
}