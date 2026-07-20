class_name BaseTower
extends Area2D

@export var attack_interval_seconds: float = 1.0
@export var range_radius: float = 96.0
@export var damage_per_shot: int = 2
@export var projectile_visual_speed: float = 900.0
@export var projectile_visual_radius: float = 5.0
@export var projectile_visual_ttl: float = 0.22

var _cooldown_seconds: float = 0.0
var _shot_vfx: Array[Dictionary] = []

func _ready() -> void:
	set_physics_process(true)
	set_process(true)
	queue_redraw()

func _process(delta: float) -> void:
	if _shot_vfx.is_empty():
		return
	for i: int in range(_shot_vfx.size() - 1, -1, -1):
		var shot: Dictionary = _shot_vfx[i]
		shot["elapsed"] = float(shot.get("elapsed", 0.0)) + delta
		var ttl: float = max(float(shot.get("ttl", projectile_visual_ttl)), 0.01)
		var t: float = clamp(float(shot.get("elapsed", 0.0)) / ttl, 0.0, 1.0)
		shot["t"] = t
		_shot_vfx[i] = shot
		if t >= 1.0:
			_shot_vfx.remove_at(i)
	queue_redraw()

func _physics_process(delta: float) -> void:
	_cooldown_seconds = max(_cooldown_seconds - delta, 0.0)
	if _cooldown_seconds > 0.0:
		return
	var target: Node2D = _acquire_target()
	if target == null:
		return
	_attack_target(target)
	_cooldown_seconds = attack_interval_seconds

func _acquire_target() -> Node2D:
	var best_target: Node2D = null
	var best_progress: float = -1.0
	for node: Node in get_tree().get_nodes_in_group("enemies"):
		var enemy: Node2D = node as Node2D
		if enemy == null:
			continue
		var is_active: Variant = enemy.get("is_active")
		if is_active == null or not bool(is_active):
			continue
		if enemy.global_position.distance_to(global_position) > range_radius:
			continue
		var progress_ratio: Variant = enemy.get("path_progress_ratio")
		if progress_ratio == null:
			continue
		if float(progress_ratio) > best_progress:
			best_progress = float(progress_ratio)
			best_target = enemy
	return best_target

func _attack_target(target: Node2D) -> void:
	_spawn_shot_vfx(target.global_position)
	if target.has_method("take_damage"):
		target.call("take_damage", damage_per_shot)

func _spawn_shot_vfx(target_global_position: Vector2) -> void:
	var distance: float = global_position.distance_to(target_global_position)
	var computed_ttl: float = distance / max(projectile_visual_speed, 1.0)
	computed_ttl = clamp(computed_ttl, 0.06, projectile_visual_ttl)
	_shot_vfx.append({
		"start": global_position,
		"end": target_global_position,
		"elapsed": 0.0,
		"ttl": computed_ttl,
		"t": 0.0,
	})

func _draw() -> void:
	draw_circle(Vector2.ZERO, 18.0, Color(0.22, 0.75, 0.92, 1.0))
	draw_arc(Vector2.ZERO, range_radius, 0.0, TAU, 64, Color(0.22, 0.75, 0.92, 0.25), 2.0)
	for shot: Dictionary in _shot_vfx:
		var start_global: Vector2 = shot.get("start", global_position)
		var end_global: Vector2 = shot.get("end", global_position)
		var t: float = float(shot.get("t", 0.0))
		var start_local: Vector2 = to_local(start_global)
		var end_local: Vector2 = to_local(end_global)
		var projectile_local: Vector2 = start_local.lerp(end_local, t)
		var alpha: float = 1.0 - (t * 0.55)
		draw_line(start_local, projectile_local, Color(1.0, 0.9, 0.45, alpha), 2.0)
		draw_circle(projectile_local, projectile_visual_radius, Color(1.0, 0.82, 0.2, alpha))
