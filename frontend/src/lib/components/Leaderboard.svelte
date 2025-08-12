<script lang="ts">
  import { gameStore } from '../stores/gameStore.svelte';
  import { onMount } from 'svelte';

  const { state } = gameStore;

  // Load leaderboard on mount
  onMount(() => {
    gameStore.loadLeaderboard();
  });

  function formatDate(dateString: string): string {
    const date = new Date(dateString);
    return date.toLocaleDateString() + ' ' + date.toLocaleTimeString([], { 
      hour: '2-digit', 
      minute: '2-digit' 
    });
  }
</script>

<div class="leaderboard bg-white rounded-lg shadow-lg p-4">
  <h2 class="text-lg font-bold mb-3 text-center text-gray-800">
    🏆 Leaderboard
  </h2>

  {#if state.isLoadingLeaderboard}
    <div class="text-center py-3">
      <div class="animate-spin rounded-full h-6 w-6 border-b-2 border-blue-500 mx-auto"></div>
      <p class="text-gray-600 mt-2 text-sm">Loading...</p>
    </div>
  {:else if state.leaderboard.length === 0}
    <div class="text-center py-3 text-gray-600">
      <p class="text-sm">No completed games yet.</p>
      <p class="text-xs mt-1">Be the first to finish a puzzle!</p>
    </div>
  {:else}
    <div class="space-y-2 max-h-48 overflow-y-auto">
      {#each state.leaderboard as entry, index}
        <div class="leaderboard-entry {index === 0 ? 'bg-yellow-50 border-yellow-200' : ''}">
          <div class="flex items-center">
            <span class="text-lg mr-2">
              {index === 0 ? '🥇' : index === 1 ? '🥈' : index === 2 ? '🥉' : `${index + 1}.`}
            </span>
            <div class="flex-1">
              <div class="font-semibold text-blue-600 text-sm">
                {entry.formatted_time}
              </div>
              <div class="text-xs text-gray-500">
                {formatDate(entry.date)}
              </div>
            </div>
          </div>
        </div>
      {/each}
    </div>

    <div class="text-center">
      <button
        class="mt-3 px-6 py-3 bg-blue-500 text-white text-sm rounded-full hover:bg-blue-600 disabled:bg-gray-300 disabled:cursor-not-allowed transition-colors duration-200 shadow-md hover:shadow-lg"
        onclick={() => gameStore.loadLeaderboard()}
        disabled={state.isLoadingLeaderboard}
      >
        Refresh
      </button>
    </div>
  {/if}
</div>
