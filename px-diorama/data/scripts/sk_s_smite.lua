-- name: melee.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

tag = "sk_s_smite"

-- state attributes

duration = 1
hostile = true
targeted = true
min_range = 0
max_range = 1

name = "Smite Weak"
alias = "Smite"
description = "Do 200% weapon damage"

-- returns true if skill can be used

condition = function(user, target)
	local distance = game:distance(user:position(), target:position())
	return user ~= target
		and not user:dead()
		and not target:dead()
--		and game:reputation(user, target) < 0 -- friend-or-foe check, negative is hostile
		and min_range <= distance and distance <= max_range -- in range
end

-- action taken if skill is used

action = function(user, target)
	local weapon_damage, variant, hit, critical = game:hit(user, target)
	local result_damage = math.floor(weapon_damage * 2.0)

	if hit then
		target:damage(1)
		game:popup(target:position(), result_damage, 0xffcc00, 1.0)
		game:vfx(target:position(), "e_swipe")
		--game:vfx_projectile(user:position(), target:position(), "@")
	end
end