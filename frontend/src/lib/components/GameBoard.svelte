<script lang="ts">
  import { gameStore } from '../stores/gameStore.svelte';
  import GameTile from './GameTile.svelte';
  import type { ConstraintType } from '../api/types';

  // Reactive values
  const { state } = gameStore;

  function getHorizontalConstraint(row: number, col: number): ConstraintType {
    if (!state.currentGame || col >= 5) return 'none';
    return state.currentGame.h_constraints[row]?.[col] || 'none';
  }

  function getVerticalConstraint(row: number, col: number): ConstraintType {
    if (!state.currentGame || row >= 5) return 'none';
    return state.currentGame.v_constraints[row]?.[col] || 'none';
  }

  function hasError(row: number, col: number): boolean {
    if (!state.delayedValidationErrors || state.delayedValidationErrors.length === 0) return false;
    
    // Parse validation errors to find tiles involved in violations
    for (const error of state.delayedValidationErrors) {
      // Parse constraint violation messages
      if (error.includes('Constraint violation')) {
        // Extract row/column information from error messages
        // Format: "Constraint violation: Row X, columns Y-Z must be same/different"
        // Format: "Constraint violation: Column X, rows Y-Z must be same/different"
        
        if (error.includes('Row')) {
          const rowMatch = error.match(/Row (\d+)/);
          const colMatch = error.match(/columns (\d+)-(\d+)/);
          if (rowMatch && colMatch) {
            const errorRow = parseInt(rowMatch[1]) - 1; // Convert to 0-based
            const col1 = parseInt(colMatch[1]) - 1;
            const col2 = parseInt(colMatch[2]) - 1;
            if (errorRow === row && (col === col1 || col === col2)) {
              return true;
            }
          }
        } else if (error.includes('Column')) {
          const colMatch = error.match(/Column (\d+)/);
          const rowMatch = error.match(/rows (\d+)-(\d+)/);
          if (colMatch && rowMatch) {
            const errorCol = parseInt(colMatch[1]) - 1; // Convert to 0-based
            const row1 = parseInt(rowMatch[1]) - 1;
            const row2 = parseInt(rowMatch[2]) - 1;
            if (errorCol === col && (row === row1 || row === row2)) {
              return true;
            }
          }
        }
      }
      
      // Parse other error types (consecutive pieces, too many pieces)
      if (error.includes('consecutive')) {
        // Format: "Row X has three consecutive suns/moons at columns Y-Z"
        // Format: "Column X has three consecutive suns/moons at rows Y-Z"
        if (error.includes('Row')) {
          const rowMatch = error.match(/Row (\d+)/);
          const colMatch = error.match(/columns (\d+)-(\d+)/);
          if (rowMatch && colMatch) {
            const errorRow = parseInt(rowMatch[1]) - 1;
            const col1 = parseInt(colMatch[1]) - 1;
            const col2 = parseInt(colMatch[2]) - 1;
            if (errorRow === row && col >= col1 && col <= col2) {
              return true;
            }
          }
        } else if (error.includes('Column')) {
          const colMatch = error.match(/Column (\d+)/);
          const rowMatch = error.match(/rows (\d+)-(\d+)/);
          if (colMatch && rowMatch) {
            const errorCol = parseInt(colMatch[1]) - 1;
            const row1 = parseInt(rowMatch[1]) - 1;
            const row2 = parseInt(rowMatch[2]) - 1;
            if (errorCol === col && row >= row1 && row <= row2) {
              return true;
            }
          }
        }
      }
      
      // Parse too many pieces errors
      if (error.includes('too many')) {
        // Format: "Row X has too many suns/moons (Y/3)"
        // Format: "Column X has too many suns/moons (Y/3)"
        if (error.includes('Row')) {
          const rowMatch = error.match(/Row (\d+)/);
          if (rowMatch) {
            const errorRow = parseInt(rowMatch[1]) - 1;
            if (errorRow === row) {
              return true;
            }
          }
        } else if (error.includes('Column')) {
          const colMatch = error.match(/Column (\d+)/);
          if (colMatch) {
            const errorCol = parseInt(colMatch[1]) - 1;
            if (errorCol === col) {
              return true;
            }
          }
        }
      }
    }
    
    return false;
  }

  function isHinted(row: number, col: number): boolean {
    return state.hintHighlight && 
           state.hintHighlight.row === row && 
           state.hintHighlight.col === col;
  }
</script>

<div class="game-board-container">
  {#if state.currentGame}
    <div class="game-board bg-white p-6 rounded-xl shadow-lg">
      <!-- Game grid -->
      <div class="grid grid-cols-6 gap-1 relative">
        {#each Array(6) as _, row}
          {#each Array(6) as _, col}
            <GameTile
              {row}
              {col}
              piece={state.currentGame.board[row][col]}
              isLocked={state.currentGame.locked_tiles[row][col]}
              horizontalConstraint={getHorizontalConstraint(row, col)}
              verticalConstraint={getVerticalConstraint(row, col)}
              hasError={hasError(row, col)}
              isHinted={isHinted(row, col)}
            />
          {/each}
        {/each}
      </div>
    </div>

    <!-- Game status -->
    <div class="mt-4 text-center">
      {#if state.currentGame.is_complete}
        <div class="text-green-600 font-bold text-xl">
          🎉 Puzzle Complete! 🎉
        </div>
        {#if state.currentGame.completion_time}
          <div class="text-gray-600 mt-2">
            Completed in {gameStore.formattedTime}
          </div>
        {/if}
      {:else}
        <div class="text-lg font-semibold">
          Time: {gameStore.formattedTime}
        </div>
        <div class="text-sm text-gray-600 mt-1">
          Moves: {state.currentGame.moves_count}
        </div>
      {/if}
    </div>

    <!-- Validation errors -->
    {#if state.validationErrors.length > 0}
      <div class="mt-4 p-3 bg-red-100 border border-red-300 rounded">
        <h4 class="font-semibold text-red-800 mb-2">Rule Violations:</h4>
        <ul class="text-sm text-red-700 list-disc list-inside">
          {#each state.validationErrors as error}
            <li>{error}</li>
          {/each}
        </ul>
      </div>
    {/if}

    <!-- Hint display -->
    {#if state.currentHint}
      <div class="mt-4 p-4 bg-blue-50 border border-blue-200 rounded-lg">
        <h4 class="font-semibold text-blue-800 mb-2">💡 Hint</h4>
        <p class="text-sm text-blue-700">
          {state.currentHint.reasoning}
        </p>
        {#if state.currentHint.found}
          <p class="text-xs text-blue-600 mt-2">
            Suggested move: Place a {state.currentHint.piece_type} at row {state.currentHint.row + 1}, column {state.currentHint.col + 1}
          </p>
        {/if}
      </div>
    {/if}
  {:else}
    <div class="game-board bg-gray-200 p-8 rounded-lg text-center">
      <p class="text-gray-600">No game loaded. Create a new game to start playing!</p>
    </div>
  {/if}
</div>

<style>
  .game-board {
    min-width: fit-content;
    width: fit-content;
    margin: 0 auto;
    transition: box-shadow 0.3s ease-in-out;
  }
  
  .game-board:hover {
    box-shadow: 0 10px 25px rgba(0, 0, 0, 0.1) !important;
  }
  
  .game-board-container {
    max-width: 600px;
    margin: 0 auto;
    display: flex;
    flex-direction: column;
    align-items: center;
  }
</style>
