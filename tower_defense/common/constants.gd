class_name GameConstants
extends RefCounted

enum GameState {
	BOOT,
	MAIN_MENU,
	WAVE_PREP,
	WAVE_ACTIVE,
	WAVE_COMPLETE,
	GAME_OVER,
	VICTORY,
}

enum TowerType {
	ARROW,
	SPLASH,
}

enum EnemyType {
	NORMAL,
	FAST,
}

const ENERGY_START_PER_WAVE: int = 3
const SCRAP_START: int = 0
const CHIPS_START: int = 0
const TARGET_WIDTH: int = 1920
const TARGET_HEIGHT: int = 1080
const TARGET_ORIENTATION: String = "landscape"
