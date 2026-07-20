class_name SplashTower
extends "res://scenes/towers/base_tower.gd"

@export var splash_radius: float = 95.0

func _ready() -> void:
	attack_interval_seconds = 1.35
	range_radius = 175.0
	damage_per_shot = 2
	super._ready()

func _attack_target(target: Node2D) -> void:
	_spawn_shot_vfx(target.global_position)
	for node: Node in get_tree().get_nodes_in_group("enemies"):
		var enemy: Node2D = node as Node2D
		if enemy == null:
			continue
		var is_active: Variant = enemy.get("is_active")
		if is_active == null or not bool(is_active):
			continue
		if enemy.global_position.distance_to(target.global_position) <= splash_radius:
			if enemy.has_method("take_damage"):
				enemy.call("take_damage", damage_per_shot)
