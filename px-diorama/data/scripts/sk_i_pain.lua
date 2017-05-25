-- name: melee.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

tag = "sk_i_pain"

-- state attributes

duration = 1
hostile = true
targeted = true

name = "World of Pain: Pain"
alias = "Pain"
description = "Do 500% damage over 20 rounds"

-- returns true if skill can be used

condition = function(user, target)
	local distance = game:distance(user:position(), target:position())
	return user ~= target
		and not user:dead()
		and not target:dead()
--		and game:reputation(user, target) < 0 -- friend-or-foe check, negative is hostile
end

-- action taken if skill is used

action = function(user, target)
	local weapon_damage, variant, hit, critical = game:hit(user, target)
	local result_damage = math.floor(weapon_damage * 5.0)

	if hit then
		target:damage(1)
		game:popup(target:position(), result_damage, 0xffcc00, 1.0)
		game:vfx_missile(user:position(), target, "e_zap")
	end
end