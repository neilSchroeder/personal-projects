class_name FastEnemy
extends "res://scenes/enemies/base_enemy.gd"

func _ready() -> void:
	speed = 180.0
	max_health = 9
	scrap_reward = 1
	base_damage = 1
	super._ready()
