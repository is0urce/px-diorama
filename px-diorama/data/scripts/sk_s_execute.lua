-- name: sk_s_execute.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

tag = "sk_melee"

-- state attributes

hostile = true
targeted = true
min_range = 0
max_range = 1

name = "Execute"
description = "Do 100% damage or 300% damage"

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
	local result_damage = math.floor(weapon_damage * 3.0)

	if hit then
		target:damage(result_damage, weapon_damage_type)
	end
end