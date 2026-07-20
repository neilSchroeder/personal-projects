extends Node2D

const GC = preload("res://common/constants.gd")

@onready var path: Path2D = %EnemyPath
@onready var towers: Node2D = %Towers
@onready var hud: CanvasLayer = %HUD

const TOTAL_WAVES: int = 5

func _default_path_points() -> PackedVector2Array:
	return PackedVector2Array([
		Vector2(80.0, 560.0),
		Vector2(440.0, 560.0),
		Vector2(840.0, 420.0),
		Vector2(1260.0, 640.0),
		Vector2(1780.0, 560.0),
	])

var _normal_enemy_scene: PackedScene = preload("res://scenes/enemies/normal_enemy.tscn")
var _fast_enemy_scene: PackedScene = preload("res://scenes/enemies/fast_enemy.tscn")
var _arrow_tower_scene: PackedScene = preload("res://scenes/towers/arrow_tower.tscn")
var _splash_tower_scene: PackedScene = preload("res://scenes/towers/splash_tower.tscn")

var _wave_plans: Array[Dictionary] = [
	{"normal": 4, "fast": 0, "interval": 1.10},
	{"normal": 5, "fast": 1, "interval": 1.00},
	{"normal": 6, "fast": 2, "interval": 0.95},
	{"normal": 7, "fast": 2, "interval": 0.90},
	{"normal": 8, "fast": 3, "interval": 0.85},
]

var _spawn_queue: Array[PackedScene] = []
var _current_spawn_interval: float = 1.0
var _alive_enemies: int = 0
var _is_spawning_wave: bool = false
var _run_active: bool = false
var _spawn_cooldown: float = 0.0
var _path_visual: Line2D
var _debug_elapsed: float = 0.0
var _did_spawn_probe: bool = false

func _ready() -> void:
	if path == null:
		push_warning("EnemyPath is missing from Level1 scene.")
		return
	_ensure_path_curve()
	_setup_path_visual()
	_setup_towers()
	_connect_signals()
	_start_run()
	print("[Level1] Ready. curve_points=", path.curve.point_count, " curve_length=", path.curve.get_baked_length())

func _process(delta: float) -> void:
	_tick_wave_spawning(delta)
	_debug_elapsed += delta
	if _debug_elapsed >= 1.0:
		_debug_elapsed = 0.0
		var curve_points: int = 0
		var curve_length: float = 0.0
		if path != null and path.curve != null:
			curve_points = path.curve.point_count
			curve_length = path.curve.get_baked_length()
		print("[Level1] wave=", GameManager.current_wave, " queue=", _spawn_queue.size(), " alive=", _alive_enemies, " spawning=", _is_spawning_wave, " curve_points=", curve_points, " curve_length=", curve_length)
	_update_status_overlay()
	queue_redraw()

func _tick_wave_spawning(delta: float) -> void:
	if not _run_active or not _is_spawning_wave:
		return
	if not GameManager.can_continue_run():
		_is_spawning_wave = false
		return
	_spawn_cooldown -= delta
	if _spawn_cooldown > 0.0:
		return
	if _spawn_queue.is_empty():
		_is_spawning_wave = false
		_check_wave_completion()
		return
	_spawn_enemy(_spawn_queue.pop_front())
	_spawn_cooldown = _current_spawn_interval
	if _spawn_queue.is_empty():
		_is_spawning_wave = false

func _ensure_path_curve() -> void:
	if path.curve == null:
		path.curve = Curve2D.new()
	if _is_curve_usable(path.curve):
		return
	path.curve.clear_points()
	for point: Vector2 in _default_path_points():
		path.curve.add_point(point)
	print("[Level1] Rebuilt invalid path curve with defaults.")

func _is_curve_usable(curve: Curve2D) -> bool:
	if curve.point_count < 2:
		return false
	var baked_length: float = curve.get_baked_length()
	if baked_length < 32.0:
		return false
	var rect: Rect2 = Rect2(curve.get_point_position(0), Vector2.ZERO)
	for i: int in range(1, curve.point_count):
		rect = rect.expand(curve.get_point_position(i))
	if rect.size.x < 8.0 and rect.size.y < 8.0:
		return false
	return true

func _setup_path_visual() -> void:
	_path_visual = Line2D.new()
	_path_visual.default_color = Color(0.95, 0.72, 0.20, 0.95)
	_path_visual.width = 5.0
	_path_visual.antialiased = true
	_path_visual.z_index = 20
	add_child(_path_visual)
	var baked_points: PackedVector2Array = path.curve.get_baked_points()
	if baked_points.size() < 2:
		_path_visual.points = _default_path_points()
		return
	_path_visual.points = baked_points

func _connect_signals() -> void:
	GameManager.wave_changed.connect(_on_wave_changed)
	GameManager.base_health_changed.connect(_on_base_health_changed)
	GameManager.run_ended.connect(_on_run_ended)

func _start_run() -> void:
	GameManager.total_waves = TOTAL_WAVES
	GameManager.start_run()
	_run_active = true
	_update_hud_values()
	_start_next_wave()

func _start_next_wave() -> void:
	if not _run_active or not GameManager.can_continue_run():
		return
	var next_wave: int = GameManager.current_wave + 1
	if next_wave > TOTAL_WAVES:
		return
	GameManager.start_wave(next_wave)
	_prepare_wave_queue(next_wave)
	_is_spawning_wave = true
	_spawn_cooldown = 0.01

func _prepare_wave_queue(wave_number: int) -> void:
	_spawn_queue.clear()
	var wave_data: Dictionary = _wave_plans[wave_number - 1]
	_current_spawn_interval = float(wave_data.get("interval", 1.0))
	var normal_count: int = int(wave_data.get("normal", 0))
	var fast_count: int = int(wave_data.get("fast", 0))
	for _i: int in range(normal_count):
		_spawn_queue.append(_normal_enemy_scene)
	for _j: int in range(fast_count):
		_spawn_queue.append(_fast_enemy_scene)

func _spawn_enemy(enemy_scene: PackedScene) -> void:
	var follow: PathFollow2D = PathFollow2D.new()
	follow.rotates = false
	follow.loop = false
	follow.progress_ratio = 0.0
	follow.z_index = 25
	path.add_child(follow)
	var baked_length: float = 0.0
	if path != null and path.curve != null:
		baked_length = path.curve.get_baked_length()
	if baked_length <= 0.0:
		push_warning("[Level1] EnemyPath baked length is 0.0 at spawn.")
	if not _did_spawn_probe:
		_did_spawn_probe = true
		_run_spawn_follow_probe(follow, baked_length)
	var enemy: Node2D = enemy_scene.instantiate() as Node2D
	if enemy == null:
		follow.queue_free()
		return
	follow.add_child(enemy)
	if enemy.has_signal("defeated"):
		enemy.connect("defeated", Callable(self, "_on_enemy_defeated").bind(follow))
	if enemy.has_signal("reached_base"):
		enemy.connect("reached_base", Callable(self, "_on_enemy_reached_base").bind(follow))
	_alive_enemies += 1
	print("[Level1] Spawned enemy. remaining_queue=", _spawn_queue.size(), " alive=", _alive_enemies)

func _run_spawn_follow_probe(follow: PathFollow2D, baked_length: float) -> void:
	var before_pos: Vector2 = follow.position
	var before_ratio: float = follow.progress_ratio
	var probe_progress: float = 64.0
	if baked_length > 0.0:
		probe_progress = min(64.0, baked_length * 0.5)
	follow.progress = probe_progress
	var after_pos: Vector2 = follow.position
	var after_ratio: float = follow.progress_ratio
	print(
		"[Level1] Spawn probe. length=", baked_length,
		" before_pos=", before_pos,
		" after_pos=", after_pos,
		" before_ratio=", before_ratio,
		" after_ratio=", after_ratio,
		" probe_progress=", probe_progress
	)
	follow.progress = 0.0

func _on_enemy_defeated(scrap_reward: int, follow: PathFollow2D) -> void:
	GameManager.add_scrap(scrap_reward)
	_update_hud_values()
	_unregister_enemy(follow)
	_check_wave_completion()

func _on_enemy_reached_base(base_damage: int, follow: PathFollow2D) -> void:
	GameManager.on_enemy_reached_base(base_damage)
	_update_hud_values()
	_unregister_enemy(follow)
	if not GameManager.can_continue_run():
		return
	_check_wave_completion()

func _unregister_enemy(follow: PathFollow2D) -> void:
	_alive_enemies = max(_alive_enemies - 1, 0)
	if is_instance_valid(follow):
		follow.queue_free()

func _check_wave_completion() -> void:
	if _is_spawning_wave:
		return
	if _alive_enemies > 0:
		return
	GameManager.on_wave_cleared()
	if GameManager.game_state == GC.GameState.VICTORY:
		return
	if GameManager.game_state == GC.GameState.WAVE_COMPLETE:
		await get_tree().create_timer(1.0).timeout
		_start_next_wave()

func _setup_towers() -> void:
	for child: Node in towers.get_children():
		child.queue_free()
	var arrow_tower: Node2D = _arrow_tower_scene.instantiate() as Node2D
	var splash_tower: Node2D = _splash_tower_scene.instantiate() as Node2D
	if arrow_tower != null:
		arrow_tower.position = Vector2(580.0, 500.0)
		towers.add_child(arrow_tower)
	if splash_tower != null:
		splash_tower.position = Vector2(1040.0, 610.0)
		towers.add_child(splash_tower)

func _on_wave_changed(_wave_number: int) -> void:
	_update_hud_values()

func _on_base_health_changed(_new_value: int) -> void:
	_update_hud_values()

func _on_run_ended(victory: bool) -> void:
	_run_active = false
	if hud.has_method("set_status_text"):
		if victory:
			hud.call("set_status_text", "Victory")
		else:
			hud.call("set_status_text", "Game Over")

func _update_hud_values() -> void:
	if hud.has_method("set_wave_text"):
		hud.call("set_wave_text", GameManager.current_wave)
	if hud.has_method("set_energy_text"):
		hud.call("set_energy_text", GameManager.energy)
	if hud.has_method("set_scrap_text"):
		hud.call("set_scrap_text", GameManager.scrap)
	if hud.has_method("set_base_health_text"):
		hud.call("set_base_health_text", GameManager.base_health)

func _update_status_overlay() -> void:
	if hud.has_method("set_status_text"):
		var curve_points: int = 0
		if path != null and path.curve != null:
			curve_points = path.curve.point_count
		var msg: String = "Q:%d A:%d S:%s C:%d" % [_spawn_queue.size(), _alive_enemies, str(_is_spawning_wave), curve_points]
		hud.call("set_status_text", msg)

func _draw() -> void:
	if path == null or path.curve == null:
		return
	var baked_points: PackedVector2Array = path.curve.get_baked_points()
	if baked_points.size() < 2:
		baked_points = _default_path_points()
	draw_polyline(baked_points, Color(1.0, 0.82, 0.22, 0.95), 6.0)
	for p: Vector2 in baked_points:
		draw_circle(p, 4.0, Color(1.0, 0.92, 0.38, 1.0))
