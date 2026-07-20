extends SceneTree

const REQUIRED_SCENES: Array[String] = [
	"res://scenes/gameplay/level_1.tscn",
	"res://scenes/towers/base_tower.tscn",
	"res://scenes/enemies/base_enemy.tscn",
	"res://scenes/ui/hud.tscn",
]

const REQUIRED_SCRIPTS: Array[String] = [
	"res://common/constants.gd",
	"res://common/game_manager.gd",
	"res://scenes/gameplay/level_1.gd",
	"res://scenes/towers/base_tower.gd",
	"res://scenes/enemies/base_enemy.gd",
	"res://scenes/ui/hud.gd",
]

func _initialize() -> void:
	var failures: Array[String] = []
	_print_header()
	_check_project_boot_settings(failures)
	_check_autoload(failures)
	_check_scripts_load(failures)
	_check_scenes_load_and_instantiate(failures)
	_finish_suite(failures)

func _print_header() -> void:
	print("[SMOKE] Starting smoke suite for VoidGrit")

func _check_project_boot_settings(failures: Array[String]) -> void:
	var main_scene: Variant = ProjectSettings.get_setting("application/run/main_scene", "")
	if typeof(main_scene) != TYPE_STRING or String(main_scene).is_empty():
		failures.append("Missing application/run/main_scene project setting.")
	elif not ResourceLoader.exists(String(main_scene)):
		failures.append("Main scene path does not exist: %s" % String(main_scene))

func _check_autoload(failures: Array[String]) -> void:
	var autoload_path: Variant = ProjectSettings.get_setting("autoload/GameManager", "")
	if typeof(autoload_path) != TYPE_STRING or String(autoload_path).is_empty():
		failures.append("Missing autoload/GameManager setting.")
		return
	var normalized_path: String = String(autoload_path).trim_prefix("*")
	if not ResourceLoader.exists(normalized_path):
		failures.append("Autoload GameManager script missing: %s" % normalized_path)

func _check_scripts_load(failures: Array[String]) -> void:
	for script_path: String in REQUIRED_SCRIPTS:
		if not ResourceLoader.exists(script_path):
			failures.append("Missing script: %s" % script_path)
			continue
		var script_resource: Resource = load(script_path)
		if script_resource == null:
			failures.append("Script failed to load: %s" % script_path)

func _check_scenes_load_and_instantiate(failures: Array[String]) -> void:
	for scene_path: String in REQUIRED_SCENES:
		if not ResourceLoader.exists(scene_path):
			failures.append("Missing scene: %s" % scene_path)
			continue
		var packed_scene: PackedScene = load(scene_path) as PackedScene
		if packed_scene == null:
			failures.append("Scene failed to load as PackedScene: %s" % scene_path)
			continue
		var instance: Node = packed_scene.instantiate()
		if instance == null:
			failures.append("Scene failed to instantiate: %s" % scene_path)
			continue
		instance.queue_free()

func _finish_suite(failures: Array[String]) -> void:
	if failures.is_empty():
		print("[SMOKE] PASS: boot settings, autoload, scripts, and scenes loaded successfully.")
		quit(0)
		return
	printerr("[SMOKE] FAIL: %d issue(s) detected." % failures.size())
	for failure: String in failures:
		printerr(" - %s" % failure)
	quit(1)
