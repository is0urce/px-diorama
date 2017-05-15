-- name: fireball.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

fireball = {
	tag = "sk_fireball",
	name = "Fireball",
	short = "Fireball",
	description = "Do 100% damage to everyone in 6 tile radius around target location",

	hostile = true,
	targeted = false, -- target location, not units
	cooldown = 0,

	cost = function(user, target)
		return 10
	end,

	range = function(user, target)
		return 0, 15
	end,

	condition = function(user, target)
		local distance = game.distance(user:position(), target)
		return not user:dead()
	end,

	action = function(user, target)
		local current = game.first_in_area(target, 6, true)
		while current:valid() do
			-- apply effects
			local dmg, type, hit, crit = game.hit(user, current)
			if hit then
				current:damage(math.floor(dmg * 1.0))
			end

			-- to next target
			current = game.next()
		end
		user:drain(fireball.cost(user, target))
	end
}