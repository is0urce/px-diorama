-- name: indigo.lua
-- type: lua script
-- auth: is0urce
-- desc: skill script

tag = "sk_d_vial"

name = "Indigo Vial"
alias = "IndigoV"
description = "Restore 20% of maximum energy and health"

hostile = false
targeted = false
cooldown = 20

condition = function(user, target)
	return not user:dead() -- sorry, it's too late
end

action = function(user, target)
	local mp = math.floor(user:energy_max() * 0.4)
	local hp = math.floor(user::health_max() * 0.4)
	user:innervate(mp)
	user:heal(hp)
end