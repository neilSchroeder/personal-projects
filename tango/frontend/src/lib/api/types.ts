/**
 * Types for the Tango game API
 */

export type PieceType = 'empty' | 'sun' | 'moon';

export type ConstraintType = 'none' | 'same' | 'different';

export interface GameState {
  game_id: string;
  board: PieceType[][];
  h_constraints: ConstraintType[][];
  v_constraints: ConstraintType[][];
  locked_tiles: boolean[][];
  is_complete: boolean;
  start_time: string;
  completion_time?: string;
  moves_count: number;
}

export interface MoveRequest {
  row: number;
  col: number;
  piece_type: PieceType;
}

export interface MoveResponse {
  success: boolean;
  message: string;
  game_state?: GameState;
  validation_errors?: string[];
}

export interface CreateGameResponse {
  game_id: string;
  game_state: GameState;
}

export interface LeaderboardEntry {
  time: number;
  date: string;
  formatted_time: string;
}

export interface LeaderboardResponse {
  entries: LeaderboardEntry[];
}

export interface GameValidationResult {
  is_valid: boolean;
  errors: string[];
  is_complete: boolean;
}

export interface HintResponse {
  found: boolean;
  row?: number;
  col?: number;
  piece_type?: PieceType;
  reasoning: string;
  confidence?: number;
  error?: string;
}

export interface ErrorResponse {
  error: string;
  detail?: string;
}
