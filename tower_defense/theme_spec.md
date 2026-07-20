# Theme & Core Design Spec: Project VoidGrit

This document establishes the mechanical, visual, and tonal blueprint for AI coding assistants. All generated scripts, UI designs, card mechanics, and text descriptions must strictly align with this world-state and mechanical crossover.

---

## 🌌 1. The Core Pitch
A sci-fi western, dirty, grungy roguelike deckbuilder crossed with a deeply tactical grid-based tower defense game. 
- **The Loop:** Draft cards (towers, scrap, combat maneuvers) to survive waves of scrap-mutants and rival syndicate raiders while defending your mechanical extraction rig.
- **Inspirations:** *Slay the Spire 2* (Deck control, relic loops, procedural node mapping), *GemCraft* (Combining/modding defensive constructs, dense waves, tactical mana/energy manipulation), and *Griftlands* (Grungy, highly stylized narrative sci-fi western tone).

---

## 🎨 2. Visual & Tonal Identity (Griftlands Style)
- **Atmosphere:** Dirty, industrial, frontier-sci-fi. Everything is pieced together from rusted steel, oxidized copper, leaking hydraulic fluid, and glowing plasma lines.
- **Palette Guidelines:** 
  - *Base:* Low-saturation industrial tones (slate gray, oily blacks, rusted iron browns).
  - *Accents:* High-contrast, toxic neon highlights to indicate active tech (plasma blue, radioactive green, unstable cybernetic purple).
- **UI Styling:** Heavy, boxed, modular layouts with subtle scuffs, scan lines, and industrial beveling. No clean, sleek futuristic glass.

---

## 🃏 3. Deckbuilding Mechanics (Slay the Spire 2 Style)
The deck contains both **Tower Blueprint Cards** and **Utility/Combat Cards**.
- **The Resource (Energy):** Players have a finite amount of Energy per turn/wave to play cards.
- **Card Archetypes:**
  - *Constructs:* Places a physical defensive tower or trap on a free map tile.
  - *Modifications:* Targets an active tower to alter its firing rate, elements, or mechanics (mirroring GemCraft's gem-slotting).
  - *Maneuvers:* Instant player actions (e.g., "Overdrive Target Tower", "EMP Stun Row").

---

## 🏗️ 4. Tower Defense System (GemCraft Style)
- **Construct Combination:** Towers should not be purely static. Players can play specific modifier cards onto existing towers to combine elements (e.g., playing a *Acid Core* onto a *Plasma Sentry* yields a *Corrosive Burn Stream*).
- **Targeting Nuance:** Towers possess highly specialized logic toggles (Target Least HP, Target Strongest, Target Shielded, Target Closest to Extraction Rig).
- **Grid Layout:** Fixed isometric or top-down grid paths where players can manipulate pathing layout or place barriers to redirect the flow of raiders.

---

## 🧑‍🎤 5. Character Archetypes & Faction Decks

When designing cards, relics, or character specific nodes, use these definitions:

### Character 1: The Scrapper (Mechanist / Rusty Scrap Metal Deck)
*   **Vibe:** Blue-collar engineer, grease-covered, cynical survivalist.
*   **Deck Style:** Scrap accumulation and physical projectile recycling.
*   **Key Mechanics:** 
    *   *Scrap Stack:* Enemies drop a static resource called Scrap on death. The Scrapper uses Scrap to upgrade towers mid-wave without spending cards.
    *   *Kinetic Impact:* Towers focus on heavy armor-piercing kinetic flak and mechanical barricades.

### Character 2: The Hacker (Cyberpunk / Rogue Syndicate Deck)
*   **Vibe:** Cybernetically enhanced outlaw, smooth-talking data thief.
*   **Deck Style:** Software overrides, electronic warfare, and high-energy manipulation.
*   **Key Mechanics:**
    *   *Malware Injection:* Towers apply digital debuffs that make enemies turn on each other or spread cascading electric status arches.
    *   *Energy Theft:* Drains energy directly from shield-bearing mechanical enemies to draw extra cards mid-wave.

### Character 3: The Zealot (Bio-Tech / Unstable Alien Organics Deck)
*   **Vibe:** Fanatical frontier cultist grafting alien mutations onto industrial chassis.
*   **Deck Style:** Risk-versus-reward high volatility, self-damage, and lingering damage over time.
*   **Key Mechanics:**
    *   *Bio-Corrosion:* Towers fire toxic acid streams that melt enemy health meters exponentially over time but have fragile internal durability.
    *   *Consuming Sacrifice:* Permanently corrode cards from your active hand to temporarily give towers a boost. Corroding different types / rarities of cards gives different effects / strength.

---

## 🤖 AI Prompting Instructions
- When coding UI elements, always inject styling parameters that reflect an industrial, grungy monitor look (heavy margins, dark backgrounds, distinct mechanical button frames).
- When prototyping cards or mechanics, include explicit inline comments explaining how the card interacts with both the *Slay the Spire 2 style deck engine* and the *GemCraft style map objects*.
- Ensure variable and constant names inside `constants.gd` use thematic identifiers (e.g., use `ENERGY` instead of `mana`, use `SCRAP` or `CHIPS` instead of `money`).