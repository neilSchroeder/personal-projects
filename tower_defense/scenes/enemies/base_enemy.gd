class_name BaseEnemy
extends Node2D

signal defeated(scrap_reward: int)
signal reached_base(base_damage: int)

@export var speed: float = 100.0
@export var max_health: int = 10
@export var scrap_reward: int = 1
@export var base_damage: int = 1

var current_health: int = 10
var is_active: bool = true
var path_progress_ratio: float = 0.0
var _debug_elapsed: float = 0.0

func _ready() -> void:
	current_health = max_health
	add_to_group("enemies")
	queue_redraw()

func _process(delta: float) -> void:
	if not is_active:
		return
	var follow: PathFollow2D = get_parent() as PathFollow2D
	if follow == null:
		return
	follow.progress += speed * delta
	path_progress_ratio = clamp(follow.progress_ratio, 0.0, 1.0)
	_debug_elapsed += delta
	if _debug_elapsed >= 1.0:
		_debug_elapsed = 0.0
		print("[Enemy] pos=", global_position, " progress=", follow.progress, " ratio=", path_progress_ratio)
	if path_progress_ratio >= 0.999:
		_reach_base()

func take_damage(amount: int) -> void:
	if not is_active:
		return
	current_health = max(current_health - amount, 0)
	if current_health == 0:
		is_active = false
		defeated.emit(scrap_reward)
		queue_free()

func _reach_base() -> void:
	if not is_active:
		return
	is_active = false
	reached_base.emit(base_damage)
	queue_free()

func _draw() -> void:
	draw_circle(Vector2.ZERO, 16.0, Color(0.96, 0.24, 0.18, 1.0))
