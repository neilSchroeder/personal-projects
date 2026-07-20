class_name ArrowTower
extends "res://scenes/towers/base_tower.gd"

func _ready() -> void:
	attack_interval_seconds = 0.75
	range_radius = 190.0
	damage_per_shot = 3
	super._ready()
