/**
 * Main game service that coordinates puzzle generation, solving, and validation
 * Ported from backend/app/services/game_service.py
 */

import { 
  PieceType, 
  ConstraintType, 
  createEmptyBoard,
  createEmptyHConstraints,
  createEmptyVConstraints,
  createEmptyLockedTiles,
  type GameState,
  type GameResult,
  type Hint,
  type PuzzleConfig,
  type Stats,
  PUZZLE_CONFIGS 
} from './types';
import { TangoBoardSolver } from './TangoBoardSolver';
import { PuzzleGenerator } from './PuzzleGenerator';

export class GameService {
  private currentPuzzle: {
    solutionBoard: PieceType[][];
    hConstraints: ConstraintType[][];
    vConstraints: ConstraintType[][];
    lockedTiles: boolean[][];
  } | null = null;

  private generator = new PuzzleGenerator();
  private currentDifficulty: string = 'standard';

  /**
   * Start a new game with the specified difficulty
   */
  newGame(difficulty: string = 'standard'): GameState {
    console.log(`🎮 Starting new ${difficulty} game`);
    this.currentDifficulty = difficulty;

    const config = PUZZLE_CONFIGS[difficulty];
    if (!config) {
      throw new Error(`Unknown difficulty: ${difficulty}`);
    }

    // Generate the puzzle
    const puzzle = this.generator.generatePuzzle(config);
    
    // Store the complete solution for validation
    this.currentPuzzle = {
      solutionBoard: this.reconstructSolution(puzzle),
      hConstraints: puzzle.hConstraints,
      vConstraints: puzzle.vConstraints,
      lockedTiles: puzzle.lockedTiles
    };

    const gameState: GameState = {
      board: puzzle.board,
      hConstraints: puzzle.hConstraints,
      vConstraints: puzzle.vConstraints,
      lockedTiles: puzzle.lockedTiles,
      isComplete: false,
      isValid: true,
      difficulty,
      startTime: new Date(),
      moveCount: 0
    };

    console.log('✅ New game generated successfully');
    return gameState;
  }

  /**
   * Reconstruct the complete solution from a puzzle
   */
  private reconstructSolution(puzzle: {
    board: PieceType[][];
    hConstraints: ConstraintType[][];
    vConstraints: ConstraintType[][];
    lockedTiles: boolean[][];
  }): PieceType[][] {
    const solver = new TangoBoardSolver(
      puzzle.board, 
      puzzle.hConstraints, 
      puzzle.vConstraints, 
      puzzle.lockedTiles
    );

    const solutions = solver.findAllSolutions(1);
    if (solutions.length === 0) {
      throw new Error('Generated puzzle has no solution!');
    }

    return solutions[0];
  }

  /**
   * Make a move on the board
   */
  makeMove(gameState: GameState, row: number, col: number, piece: PieceType): GameState {
    if (gameState.lockedTiles[row][col]) {
      throw new Error('Cannot modify locked tiles');
    }

    if (piece !== PieceType.EMPTY && piece !== PieceType.SUN && piece !== PieceType.MOON) {
      throw new Error('Invalid piece type');
    }

    // Create new board with the move
    const newBoard = gameState.board.map(boardRow => [...boardRow]);
    newBoard[row][col] = piece;

    // Validate the move
    const isValid = this.validateBoard(newBoard, gameState.hConstraints, gameState.vConstraints);

    // Check if complete
    const isComplete = this.isBoardComplete(newBoard) && isValid;

    const newGameState: GameState = {
      ...gameState,
      board: newBoard,
      isValid,
      isComplete,
      moveCount: gameState.moveCount + 1
    };

    return newGameState;
  }

  /**
   * Validate a board against all rules and constraints
   */
  validateBoard(
    board: PieceType[][], 
    hConstraints: ConstraintType[][], 
    vConstraints: ConstraintType[][]
  ): boolean {
    const size = board.length;

    // Rule 1: No more than 2 consecutive pieces
    for (let row = 0; row < size; row++) {
      for (let col = 0; col < size - 2; col++) {
        if (board[row][col] !== PieceType.EMPTY &&
            board[row][col] === board[row][col + 1] &&
            board[row][col + 1] === board[row][col + 2]) {
          return false;
        }
      }
    }

    for (let row = 0; row < size - 2; row++) {
      for (let col = 0; col < size; col++) {
        if (board[row][col] !== PieceType.EMPTY &&
            board[row][col] === board[row + 1][col] &&
            board[row + 1][col] === board[row + 2][col]) {
          return false;
        }
      }
    }

    // Rule 2: Equal numbers of each piece per row/column (only check complete rows/cols)
    for (let row = 0; row < size; row++) {
      const pieces = board[row].filter(p => p !== PieceType.EMPTY);
      if (pieces.length === size) { // Only check complete rows
        const suns = pieces.filter(p => p === PieceType.SUN).length;
        const moons = pieces.filter(p => p === PieceType.MOON).length;
        if (suns !== size / 2 || moons !== size / 2) {
          return false;
        }
      } else {
        // For incomplete rows, check we don't exceed the limit
        const suns = pieces.filter(p => p === PieceType.SUN).length;
        const moons = pieces.filter(p => p === PieceType.MOON).length;
        if (suns > size / 2 || moons > size / 2) {
          return false;
        }
      }
    }

    for (let col = 0; col < size; col++) {
      const pieces = Array.from({length: size}, (_, row) => board[row][col])
        .filter(p => p !== PieceType.EMPTY);
      if (pieces.length === size) { // Only check complete columns
        const suns = pieces.filter(p => p === PieceType.SUN).length;
        const moons = pieces.filter(p => p === PieceType.MOON).length;
        if (suns !== size / 2 || moons !== size / 2) {
          return false;
        }
      } else {
        // For incomplete columns, check we don't exceed the limit
        const suns = pieces.filter(p => p === PieceType.SUN).length;
        const moons = pieces.filter(p => p === PieceType.MOON).length;
        if (suns > size / 2 || moons > size / 2) {
          return false;
        }
      }
    }

    // Rule 3: Constraint validation
    for (let row = 0; row < size; row++) {
      for (let col = 0; col < size - 1; col++) {
        const constraint = hConstraints[row][col];
        if (constraint !== ConstraintType.NONE) {
          const left = board[row][col];
          const right = board[row][col + 1];
          
          if (left !== PieceType.EMPTY && right !== PieceType.EMPTY) {
            if (constraint === ConstraintType.SAME && left !== right) {
              return false;
            }
            if (constraint === ConstraintType.DIFFERENT && left === right) {
              return false;
            }
          }
        }
      }
    }

    for (let row = 0; row < size - 1; row++) {
      for (let col = 0; col < size; col++) {
        const constraint = vConstraints[row][col];
        if (constraint !== ConstraintType.NONE) {
          const top = board[row][col];
          const bottom = board[row + 1][col];
          
          if (top !== PieceType.EMPTY && bottom !== PieceType.EMPTY) {
            if (constraint === ConstraintType.SAME && top !== bottom) {
              return false;
            }
            if (constraint === ConstraintType.DIFFERENT && top === bottom) {
              return false;
            }
          }
        }
      }
    }

    return true;
  }

  /**
   * Check if the board is completely filled
   */
  private isBoardComplete(board: PieceType[][]): boolean {
    return board.every(row => row.every(cell => cell !== PieceType.EMPTY));
  }

  /**
   * Get a hint for the current board state
   */
  getHint(gameState: GameState): Hint {
    if (!this.currentPuzzle) {
      throw new Error('No active puzzle to provide hints for');
    }

    const solver = new TangoBoardSolver(
      gameState.board,
      gameState.hConstraints,
      gameState.vConstraints,
      gameState.lockedTiles
    );

    const hintResult = solver.getHint();
    
    // Convert HintResult to Hint interface
    const hint: Hint = {
      type: hintResult.hintType === 'logical_deduction' ? 'logical' : 
            hintResult.hintType === 'strategic_guidance' ? 'constraint' :
            hintResult.hintType === 'strategic_guess' ? 'rule' : 'none',
      message: hintResult.reasoning,
      reasoning: hintResult.reasoning
    };

    if (hintResult.row !== undefined && hintResult.col !== undefined) {
      hint.position = { row: hintResult.row, col: hintResult.col };
    }

    if (hintResult.pieceType !== undefined) {
      hint.suggestedPiece = hintResult.pieceType;
    }

    return hint;
  }

  /**
   * Get all available difficulty levels
   */
  getDifficulties(): { [key: string]: PuzzleConfig } {
    return PUZZLE_CONFIGS;
  }

  /**
   * Submit a completed game for scoring
   */
  submitGame(gameState: GameState): GameResult {
    if (!gameState.isComplete || !gameState.isValid) {
      throw new Error('Game is not complete or valid');
    }

    const duration = Date.now() - gameState.startTime.getTime();
    const config = PUZZLE_CONFIGS[gameState.difficulty];
    
    // Calculate score based on time and moves
    const timeBonus = Math.max(0, config.parTime - duration / 1000);
    const moveBonus = Math.max(0, config.parMoves - gameState.moveCount);
    const score = Math.round(
      config.baseScore + timeBonus * config.timeWeight + moveBonus * config.moveWeight
    );

    return {
      score,
      duration,
      moves: gameState.moveCount,
      difficulty: gameState.difficulty,
      completed: true
    };
  }

  /**
   * Get game statistics
   */
  getStats(): Stats {
    // In a real implementation, this would read from persistent storage
    // For now, return default stats
    return {
      gamesPlayed: 0,
      gamesWon: 0,
      averageScore: 0,
      bestScore: 0,
      averageTime: 0,
      bestTime: 0,
      difficultyStats: {}
    };
  }

  /**
   * Validate that the current solution matches the expected solution
   */
  validateSolution(gameState: GameState): boolean {
    if (!this.currentPuzzle) {
      return false;
    }

    if (!gameState.isComplete || !gameState.isValid) {
      return false;
    }

    // Check if the current board matches the expected solution
    for (let row = 0; row < gameState.board.length; row++) {
      for (let col = 0; col < gameState.board[row].length; col++) {
        if (gameState.board[row][col] !== this.currentPuzzle.solutionBoard[row][col]) {
          return false;
        }
      }
    }

    return true;
  }

  /**
   * Auto-solve the current puzzle (for testing or giving up)
   */
  autoSolve(gameState: GameState): GameState {
    if (!this.currentPuzzle) {
      throw new Error('No active puzzle to solve');
    }

    return {
      ...gameState,
      board: this.currentPuzzle.solutionBoard.map(row => [...row]),
      isComplete: true,
      isValid: true,
      moveCount: gameState.moveCount + 1 // Count auto-solve as one move
    };
  }

  /**
   * Reset the board to initial state
   */
  resetBoard(gameState: GameState): GameState {
    const resetBoard = createEmptyBoard();
    
    // Restore locked pieces
    for (let row = 0; row < gameState.board.length; row++) {
      for (let col = 0; col < gameState.board[row].length; col++) {
        if (gameState.lockedTiles[row][col]) {
          resetBoard[row][col] = this.currentPuzzle?.solutionBoard[row][col] || PieceType.EMPTY;
        }
      }
    }

    return {
      ...gameState,
      board: resetBoard,
      isComplete: false,
      isValid: true,
      moveCount: 0 // Reset move count
    };
  }

  /**
   * Check if a specific move would be valid
   */
  isValidMove(gameState: GameState, row: number, col: number, piece: PieceType): boolean {
    if (gameState.lockedTiles[row][col]) {
      return false;
    }

    const testBoard = gameState.board.map(boardRow => [...boardRow]);
    testBoard[row][col] = piece;

    return this.validateBoard(testBoard, gameState.hConstraints, gameState.vConstraints);
  }

  /**
   * Get detailed analysis of the current board state
   */
  analyzeBoard(gameState: GameState): {
    errorCount: number;
    constraintViolations: Array<{row: number; col: number; type: string}>;
    ruleViolations: Array<{row: number; col: number; type: string}>;
    completeness: number;
  } {
    const size = gameState.board.length;
    const constraintViolations: Array<{row: number; col: number; type: string}> = [];
    const ruleViolations: Array<{row: number; col: number; type: string}> = [];

    // Check rule violations
    // Consecutive pieces
    for (let row = 0; row < size; row++) {
      for (let col = 0; col < size - 2; col++) {
        if (gameState.board[row][col] !== PieceType.EMPTY &&
            gameState.board[row][col] === gameState.board[row][col + 1] &&
            gameState.board[row][col + 1] === gameState.board[row][col + 2]) {
          ruleViolations.push({row, col, type: 'consecutive_horizontal'});
        }
      }
    }

    for (let row = 0; row < size - 2; row++) {
      for (let col = 0; col < size; col++) {
        if (gameState.board[row][col] !== PieceType.EMPTY &&
            gameState.board[row][col] === gameState.board[row + 1][col] &&
            gameState.board[row + 1][col] === gameState.board[row + 2][col]) {
          ruleViolations.push({row, col, type: 'consecutive_vertical'});
        }
      }
    }

    // Check constraint violations
    for (let row = 0; row < size; row++) {
      for (let col = 0; col < size - 1; col++) {
        const constraint = gameState.hConstraints[row][col];
        if (constraint !== ConstraintType.NONE) {
          const left = gameState.board[row][col];
          const right = gameState.board[row][col + 1];
          
          if (left !== PieceType.EMPTY && right !== PieceType.EMPTY) {
            if (constraint === ConstraintType.SAME && left !== right) {
              constraintViolations.push({row, col, type: 'horizontal_same'});
            }
            if (constraint === ConstraintType.DIFFERENT && left === right) {
              constraintViolations.push({row, col, type: 'horizontal_different'});
            }
          }
        }
      }
    }

    for (let row = 0; row < size - 1; row++) {
      for (let col = 0; col < size; col++) {
        const constraint = gameState.vConstraints[row][col];
        if (constraint !== ConstraintType.NONE) {
          const top = gameState.board[row][col];
          const bottom = gameState.board[row + 1][col];
          
          if (top !== PieceType.EMPTY && bottom !== PieceType.EMPTY) {
            if (constraint === ConstraintType.SAME && top !== bottom) {
              constraintViolations.push({row, col, type: 'vertical_same'});
            }
            if (constraint === ConstraintType.DIFFERENT && top === bottom) {
              constraintViolations.push({row, col, type: 'vertical_different'});
            }
          }
        }
      }
    }

    // Calculate completeness
    const totalCells = size * size;
    const filledCells = gameState.board.flat().filter(cell => cell !== PieceType.EMPTY).length;
    const completeness = Math.round((filledCells / totalCells) * 100);

    return {
      errorCount: constraintViolations.length + ruleViolations.length,
      constraintViolations,
      ruleViolations,
      completeness
    };
  }
}
