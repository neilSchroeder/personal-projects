extends Node

const GC = preload("res://common/constants.gd")

signal game_state_changed(new_state: int)
signal wave_changed(wave_number: int)
signal base_health_changed(new_value: int)
signal run_ended(victory: bool)

var game_state: int = GC.GameState.BOOT
var current_wave: int = 0
var base_health: int = 20
var energy: int = GC.ENERGY_START_PER_WAVE
var scrap: int = GC.SCRAP_START
var chips: int = GC.CHIPS_START
var total_waves: int = 5

func _ready() -> void:
	set_game_state(GC.GameState.MAIN_MENU)

func start_run() -> void:
	current_wave = 0
	base_health = 20
	energy = GC.ENERGY_START_PER_WAVE
	scrap = GC.SCRAP_START
	chips = GC.CHIPS_START
	base_health_changed.emit(base_health)
	wave_changed.emit(current_wave)
	set_game_state(GC.GameState.WAVE_PREP)

func set_game_state(new_state: int) -> void:
	if game_state == new_state:
		return
	game_state = new_state
	game_state_changed.emit(game_state)

func start_wave(wave_number: int) -> void:
	current_wave = wave_number
	energy = GC.ENERGY_START_PER_WAVE
	wave_changed.emit(current_wave)
	set_game_state(GC.GameState.WAVE_ACTIVE)

func on_wave_cleared() -> void:
	if current_wave >= total_waves:
		set_game_state(GC.GameState.VICTORY)
		run_ended.emit(true)
		return
	set_game_state(GC.GameState.WAVE_COMPLETE)

func on_enemy_reached_base(base_damage: int) -> void:
	base_health = max(base_health - base_damage, 0)
	base_health_changed.emit(base_health)
	if base_health == 0:
		set_game_state(GC.GameState.GAME_OVER)
		run_ended.emit(false)

func can_continue_run() -> bool:
	return game_state != GC.GameState.GAME_OVER and game_state != GC.GameState.VICTORY

func add_scrap(amount: int) -> void:
	scrap += amount

func spend_energy(cost: int) -> bool:
	if cost > energy:
		return false
	energy -= cost
	return true
