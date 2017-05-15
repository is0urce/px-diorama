-- name: sk_s_flurry.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

tag = "sk_s_flurry"

-- state attributes

hostile = true
targeted = true
min_range = 0
max_range = 1

name = "Flurry"
alias = "Flurry"
description = "Do 50% weapon damage three times in a row"

-- returns true if skill can be used

condition = function(user, target)
	local distance = game.distance(user:position(), target:position())
	local min, max = min_range, max_range
	return user ~= target
		and not user:dead()
		and not target:dead()
		and game.reputation(user, target) < 0 -- friend-or-foe check, negative is hostile
		and min <= distance and distance <= max -- in range
end

-- action taken if skill is used

action = function(user, target)
	local weapon_damage, weapon_damage_type, is_hit, is_crit = game.hit(user, target)
	local result_damage = math.floor(weapon_damage * 0.5)

	if hit then
		target:damage(result_damage, weapon_damage_type)
	end
end