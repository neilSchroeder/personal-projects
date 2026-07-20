extends CanvasLayer

@onready var wave_label: Label = %WaveLabel
@onready var energy_label: Label = %EnergyLabel
@onready var scrap_label: Label = %ScrapLabel
@onready var base_health_label: Label = %BaseHealthLabel
@onready var status_label: Label = %StatusLabel

func set_wave_text(wave_number: int) -> void:
	wave_label.text = "Wave: %d" % wave_number

func set_energy_text(current_energy: int) -> void:
	energy_label.text = "Energy: %d" % current_energy

func set_scrap_text(current_scrap: int) -> void:
	scrap_label.text = "Scrap: %d" % current_scrap

func set_base_health_text(current_base_health: int) -> void:
	base_health_label.text = "Base HP: %d" % current_base_health

func set_status_text(text: String) -> void:
	status_label.text = text
