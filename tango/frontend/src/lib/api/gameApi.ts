/**
 * API client for the Tango game backend
 */

import type {
  CreateGameResponse,
  GameState,
  MoveRequest,
  MoveResponse,
  LeaderboardResponse,
  HintResponse,
  GameValidationResult
} from './types';

const API_BASE_URL = 'http://localhost:8000';

class ApiError extends Error {
  constructor(public status: number, message: string, public detail?: string) {
    super(message);
    this.name = 'ApiError';
  }
}

async function handleResponse<T>(response: Response): Promise<T> {
  if (!response.ok) {
    const errorData = await response.json().catch(() => ({}));
    throw new ApiError(
      response.status,
      errorData.error || `HTTP ${response.status}`,
      errorData.detail
    );
  }
  return response.json();
}

export class GameApi {
  /**
   * Create a new game
   */
  static async createGame(): Promise<CreateGameResponse> {
    const response = await fetch(`${API_BASE_URL}/api/game/create`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
    });

    return handleResponse<CreateGameResponse>(response);
  }

  /**
   * Get the current state of a game
   */
  static async getGameState(gameId: string): Promise<GameState> {
    const response = await fetch(`${API_BASE_URL}/api/game/${gameId}`, {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
      },
    });

    return handleResponse<GameState>(response);
  }

  /**
   * Make a move in the game
   */
  static async makeMove(gameId: string, move: MoveRequest): Promise<MoveResponse> {
    const response = await fetch(`${API_BASE_URL}/api/game/${gameId}/move`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(move),
    });

    return handleResponse<MoveResponse>(response);
  }

  /**
   * Validate the current game state
   */
  static async validateGame(gameId: string): Promise<GameValidationResult> {
    const response = await fetch(`${API_BASE_URL}/api/game/${gameId}/validate`, {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
      },
    });

    return handleResponse<GameValidationResult>(response);
  }

  /**
   * Get the leaderboard
   */
  static async getLeaderboard(): Promise<LeaderboardResponse> {
    const response = await fetch(`${API_BASE_URL}/api/game/leaderboard`, {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
      },
    });

    return handleResponse<LeaderboardResponse>(response);
  }

  /**
   * Delete a game to free memory
   */
  static async deleteGame(gameId: string): Promise<void> {
    const response = await fetch(`${API_BASE_URL}/api/game/${gameId}`, {
      method: 'DELETE',
      headers: {
        'Content-Type': 'application/json',
      },
    });

    if (!response.ok) {
      throw new ApiError(response.status, `Failed to delete game: ${response.status}`);
    }
  }

  /**
   * Get a hint for the next logical move
   */
  static async getHint(gameId: string): Promise<HintResponse> {
    const response = await fetch(`${API_BASE_URL}/api/game/${gameId}/hint`, {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
      },
    });

    return handleResponse<HintResponse>(response);
  }
}

export { ApiError };
