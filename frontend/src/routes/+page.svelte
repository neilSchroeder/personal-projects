<script lang="ts">
  import { onMount, onDestroy } from 'svelte';
  import GameBoard from '$lib/components/GameBoard.svelte';
  import GameControls from '$lib/components/GameControls.svelte';
  import Leaderboard from '$lib/components/Leaderboard.svelte';
  import { gameStore } from '$lib/stores/gameStore.svelte';

  // Load leaderboard on mount
  onMount(() => {
    gameStore.loadLeaderboard();
  });

  // Cleanup on destroy
  onDestroy(() => {
    gameStore.destroy();
  });
</script>

<svelte:head>
  <title>Tango Puzzle Game</title>
  <meta name="description" content="A challenging logic puzzle game with suns and moons" />
</svelte:head>

<main class="min-h-screen bg-gradient-to-br from-blue-50 to-indigo-100 p-4">
  <div class="container mx-auto max-w-7xl">
    <!-- Header -->
    <header class="text-center mb-6">
      <h1 class="text-3xl font-bold text-gray-800 mb-2">
        🌞 Tango Puzzle 🌙
      </h1>
      <p class="text-gray-600 text-sm">
        Place suns and moons following the rules to complete the puzzle
      </p>
    </header>

    <!-- Main game layout -->
    <div class="grid grid-cols-1 lg:grid-cols-5 gap-4 items-start">
      <!-- Game controls (left sidebar on large screens, top on mobile) -->
      <div class="lg:order-1 order-2 lg:col-span-1">
        <GameControls />
      </div>

      <!-- Game board (center) -->
      <div class="lg:order-2 order-1 lg:col-span-3">
        <GameBoard />
      </div>

      <!-- Leaderboard (right sidebar) -->
      <div class="lg:order-3 order-3 lg:col-span-1">
        <Leaderboard />
      </div>
    </div>

    <!-- Footer -->
    <footer class="text-center mt-8 text-xs text-gray-500">
      <p>
        Built with SvelteKit and FastAPI | 
        <a 
          href="https://github.com" 
          class="text-blue-500 hover:text-blue-700 underline"
          target="_blank"
          rel="noopener noreferrer"
        >
          View Source
        </a>
      </p>
    </footer>
  </div>
</main>
