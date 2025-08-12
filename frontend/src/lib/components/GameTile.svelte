<script lang="ts">
  import type { PieceType, ConstraintType } from '../api/types';
  import { gameStore } from '../stores/gameStore.svelte';

  // Props
  interface Props {
    row: number;
    col: number;
    piece: PieceType;
    isLocked: boolean;
    horizontalConstraint?: ConstraintType;
    verticalConstraint?: ConstraintType;
    hasError?: boolean;
    isHinted?: boolean;
  }
  
  let { 
    row, 
    col, 
    piece, 
    isLocked, 
    horizontalConstraint = 'none', 
    verticalConstraint = 'none',
    hasError = false,
    isHinted = false
  }: Props = $props();

  // Click cycle: empty -> sun -> moon -> empty
  async function handleClick() {
    if (isLocked || gameStore.state.isMakingMove) return;
    
    let nextPiece: PieceType;
    switch (piece) {
      case 'empty':
        nextPiece = 'sun';
        break;
      case 'sun':
        nextPiece = 'moon';
        break;
      case 'moon':
        nextPiece = 'empty';
        break;
      default:
        nextPiece = 'empty';
    }
    
    await gameStore.makeMove(row, col, nextPiece);
  }

  function getConstraintSymbol(constraint: ConstraintType): string {
    switch (constraint) {
      case 'same': return '=';
      case 'different': return '×';
      default: return '';
    }
  }

  function getPieceSymbol(piece: PieceType): string {
    switch (piece) {
      case 'sun': return '●';  // Solid circle for sun - can be styled with CSS color
      case 'moon': return '◐'; // Half-moon symbol - can be styled with CSS color
      default: return '';
    }
  }
</script>

<div class="relative">
  <!-- Main tile -->
  <button
    class="game-tile {piece === 'sun' ? 'game-tile-sun' : ''} {piece === 'moon' ? 'game-tile-moon' : ''} {isLocked ? 'game-tile-locked' : ''} {hasError ? 'game-tile-error' : ''} {isHinted ? 'game-tile-hinted' : ''}"
    onclick={handleClick}
    disabled={isLocked || gameStore.state.isMakingMove}
    aria-label="Game tile at row {row + 1}, column {col + 1}: {piece}"
  >
    <span class="game-piece">  <!-- Using custom CSS class for precise piece size control -->
      {getPieceSymbol(piece)}
    </span>
  </button>

  <!-- Horizontal constraint (to the right) -->
  {#if horizontalConstraint !== 'none'}
    <div 
      class="constraint-horizontal absolute top-1/2 transform -translate-y-1/2"
      style="right: -0.875rem;"
    >
      {getConstraintSymbol(horizontalConstraint)}
    </div>
  {/if}

  <!-- Vertical constraint (below) -->
  {#if verticalConstraint !== 'none'}
    <div 
      class="constraint-vertical absolute left-1/2 transform -translate-x-1/2"
      style="bottom: -0.875rem;"
    >
      {getConstraintSymbol(verticalConstraint)}
    </div>
  {/if}
</div>
