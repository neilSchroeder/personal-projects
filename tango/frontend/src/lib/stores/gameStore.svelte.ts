/**
 * Game store using Svelte 5 runes for state management
 */

import { GameApi, ApiError } from '../api/gameApi';
import type { 
  GameState, 
  MoveRequest, 
  MoveResponse,
  PieceType, 
  LeaderboardEntry,
  HintResponse
} from '../api/types';

interface GameStore {
  // Current game state
  currentGame: GameState | null;
  
  // Loading states
  isLoading: boolean;
  isCreatingGame: boolean;
  isMakingMove: boolean;
  isLoadingLeaderboard: boolean;
  isLoadingHint: boolean;
  
  // Error states
  error: string | null;
  validationErrors: string[];
  delayedValidationErrors: string[];
  
  // Hint system
  currentHint: HintResponse | null;
  hintHighlight: { row: number; col: number } | null;
  
  // Leaderboard
  leaderboard: LeaderboardEntry[];
  
  // Game timer
  elapsedTime: number;
}

function createGameStore() {
  // Initialize state using runes
  let state = $state<GameStore>({
    currentGame: null,
    isLoading: false,
    isCreatingGame: false,
    isMakingMove: false,
    isLoadingLeaderboard: false,
    isLoadingHint: false,
    error: null,
    validationErrors: [],
    delayedValidationErrors: [],
    currentHint: null,
    hintHighlight: null,
    leaderboard: [],
    elapsedTime: 0
  });

  // Timer interval reference
  let timerInterval: number | null = null;

  // Error delay timeout reference
  let errorDelayTimeout: number | null = null;

  // Computed values
  const isGameActive = $derived(state.currentGame && !state.currentGame.is_complete);
  const formattedTime = $derived.by(() => {
    const minutes = Math.floor(state.elapsedTime / 60);
    const seconds = state.elapsedTime % 60;
    return `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
  });

  // Private helper functions
  function startTimer() {
    if (timerInterval) clearInterval(timerInterval);
    
    timerInterval = setInterval(() => {
      if (state.currentGame && !state.currentGame.is_complete) {
        const startTime = new Date(state.currentGame.start_time);
        const now = new Date();
        state.elapsedTime = Math.floor((now.getTime() - startTime.getTime()) / 1000);
      }
    }, 1000);
  }

  function stopTimer() {
    if (timerInterval) {
      clearInterval(timerInterval);
      timerInterval = null;
    }
  }

  function clearError() {
    state.error = null;
    state.validationErrors = [];
    state.delayedValidationErrors = [];
    
    // Clear any pending error delay timeout
    if (errorDelayTimeout) {
      clearTimeout(errorDelayTimeout);
      errorDelayTimeout = null;
    }
  }

  // Public API
  async function createGame(): Promise<void> {
    try {
      clearError();
      state.isCreatingGame = true;
      
      const response = await GameApi.createGame();
      state.currentGame = response.game_state;
      state.elapsedTime = 0;
      
      startTimer();
    } catch (error) {
      if (error instanceof ApiError) {
        state.error = `Failed to create game: ${error.message}`;
      } else {
        state.error = 'An unexpected error occurred while creating the game';
      }
    } finally {
      state.isCreatingGame = false;
    }
  }

  async function makeMove(row: number, col: number, pieceType: PieceType): Promise<boolean> {
    if (!state.currentGame) {
      state.error = 'No active game';
      return false;
    }

    try {
      clearError();
      clearHint(); // Clear any active hint when making a move
      state.isMakingMove = true;
      
      const response = await GameApi.makeMove(state.currentGame.game_id, {
        row,
        col,
        piece_type: pieceType
      });

      if (response.success && response.game_state) {
        state.currentGame = response.game_state;
        
        // If game is complete, stop timer and refresh leaderboard
        if (response.game_state.is_complete) {
          stopTimer();
          await loadLeaderboard();
        }
      } else {
        state.error = response.message;
      }

      // Update validation errors with delay
      const newValidationErrors = response.validation_errors || [];
      state.validationErrors = newValidationErrors;
      
      // Clear any existing error delay timeout
      if (errorDelayTimeout) {
        clearTimeout(errorDelayTimeout);
      }
      
      // If there are new validation errors, show them after a delay
      if (newValidationErrors.length > 0) {
        // Clear delayed errors immediately when new errors come in
        state.delayedValidationErrors = [];
        
        // Set the delayed errors after a short delay (750ms)
        errorDelayTimeout = setTimeout(() => {
          state.delayedValidationErrors = newValidationErrors;
          errorDelayTimeout = null;
        }, 750);
      } else {
        // No errors, clear delayed errors immediately
        state.delayedValidationErrors = [];
      }
      
      return response.success;
    } catch (error) {
      if (error instanceof ApiError) {
        state.error = `Move failed: ${error.message}`;
      } else {
        state.error = 'An unexpected error occurred while making the move';
      }
      return false;
    } finally {
      state.isMakingMove = false;
    }
  }

  async function loadGame(gameId: string): Promise<void> {
    try {
      clearError();
      state.isLoading = true;
      
      const gameState = await GameApi.getGameState(gameId);
      state.currentGame = gameState;
      
      if (!gameState.is_complete) {
        startTimer();
      } else {
        stopTimer();
      }
    } catch (error) {
      if (error instanceof ApiError) {
        state.error = `Failed to load game: ${error.message}`;
      } else {
        state.error = 'An unexpected error occurred while loading the game';
      }
    } finally {
      state.isLoading = false;
    }
  }

  async function loadLeaderboard(): Promise<void> {
    try {
      state.isLoadingLeaderboard = true;
      
      const response = await GameApi.getLeaderboard();
      state.leaderboard = response.entries;
    } catch (error) {
      if (error instanceof ApiError) {
        state.error = `Failed to load leaderboard: ${error.message}`;
      } else {
        state.error = 'An unexpected error occurred while loading the leaderboard';
      }
    } finally {
      state.isLoadingLeaderboard = false;
    }
  }

  async function getHint(): Promise<void> {
    if (!state.currentGame) {
      state.error = 'No active game to get hint for';
      return;
    }

    try {
      state.isLoadingHint = true;
      clearError();
      
      const hintResponse = await GameApi.getHint(state.currentGame.game_id);
      state.currentHint = hintResponse;
      
      if (hintResponse.found && hintResponse.row !== undefined && hintResponse.col !== undefined) {
        state.hintHighlight = { row: hintResponse.row, col: hintResponse.col };
        
        // Clear highlight after 5 seconds
        setTimeout(() => {
          state.hintHighlight = null;
        }, 5000);
      }
    } catch (error) {
      if (error instanceof ApiError) {
        state.error = `Failed to get hint: ${error.message}`;
      } else {
        state.error = 'An unexpected error occurred while getting hint';
      }
    } finally {
      state.isLoadingHint = false;
    }
  }

  function clearHint(): void {
    state.currentHint = null;
    state.hintHighlight = null;
  }

  function resetGame(): void {
    stopTimer();
    state.currentGame = null;
    state.elapsedTime = 0;
    clearError();
  }

  // Cleanup function
  function destroy(): void {
    stopTimer();
  }

  return {
    // Reactive state
    get state() { return state; },
    
    // Computed values
    get isGameActive() { return isGameActive; },
    get formattedTime() { return formattedTime; },
    
    // Actions
    createGame,
    makeMove,
    loadGame,
    loadLeaderboard,
    getHint,
    clearHint,
    resetGame,
    clearError,
    destroy
  };
}

// Create and export the store instance
export const gameStore = createGameStore();
