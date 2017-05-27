-- name: sk_s_rend.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

tag = "sk_s_rend"

-- state attributes

duration = 1
hostile = true
targeted = true
min_range = 0
max_range = 1

name = "Rend Flesh"
alias = "Rend"
description = "Do 100% damage plus 200% damage over 5 turns"

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
	local result_damage = math.floor(weapon_damage * 3.0)

	if hit then
		target:damage(result_damage)

		game:popup(target:position(), result_damage, 0xffcc00, 1.0)
		local visual = "e_swoosh_positive"
		local tx = target:position():x()
		local mx = user:position():x()
		if (tx < mx) then
			visual = "e_swoosh_negative"
		end
		game:vfx(target:position(), visual)
	end
end