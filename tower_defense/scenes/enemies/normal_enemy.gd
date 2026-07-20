class_name NormalEnemy
extends "res://scenes/enemies/base_enemy.gd"

func _ready() -> void:
	speed = 115.0
	max_health = 14
	scrap_reward = 1
	base_damage = 1
	super._ready()
