/**
 * Advanced board solver for Tango puzzles using depth-first search.
 * Ported from backend/app/services/game_service.py
 */

import { 
  PieceType, 
  ConstraintType, 
  BOARD_SIZE, 
  type HintResult 
} from './types';

export class TangoBoardSolver {
  private originalBoard: PieceType[][];
  private hConstraints: ConstraintType[][];
  private vConstraints: ConstraintType[][];
  private lockedTiles: boolean[][];
  private size = BOARD_SIZE;
  private emptyPositions: [number, number][];

  constructor(
    board: PieceType[][],
    hConstraints: ConstraintType[][],
    vConstraints: ConstraintType[][],
    lockedTiles: boolean[][]
  ) {
    this.originalBoard = board;
    this.hConstraints = hConstraints;
    this.vConstraints = vConstraints;
    this.lockedTiles = lockedTiles;

    // Find empty positions that need to be filled
    this.emptyPositions = [];
    for (let row = 0; row < this.size; row++) {
      for (let col = 0; col < this.size; col++) {
        if (!lockedTiles[row][col]) {
          this.emptyPositions.push([row, col]);
        }
      }
    }
  }

  /**
   * Find all valid solutions using depth-first search
   */
  findAllSolutions(maxSolutions: number = 10): PieceType[][][] {
    const solutions: PieceType[][][] = [];

    // Create initial board with only locked pieces
    const board = Array(this.size).fill(null).map(() => 
      Array(this.size).fill(PieceType.EMPTY)
    );
    
    for (let row = 0; row < this.size; row++) {
      for (let col = 0; col < this.size; col++) {
        if (this.lockedTiles[row][col]) {
          board[row][col] = this.originalBoard[row][col];
        }
      }
    }

    const backtrack = (pos: number): void => {
      if (solutions.length >= maxSolutions) {
        return;
      }

      if (pos === this.emptyPositions.length) {
        // Check if this is a complete valid solution
        if (this.isCompleteAndValid(board)) {
          // Make a deep copy of the solution
          const solution = board.map(row => [...row]);
          solutions.push(solution);
        }
        return;
      }

      const [row, col] = this.emptyPositions[pos];

      // Try both piece types
      for (const piece of [PieceType.SUN, PieceType.MOON]) {
        board[row][col] = piece;

        // Check if this placement is valid so far
        if (this.isPlacementValid(board, row, col)) {
          backtrack(pos + 1);
        }

        // Backtrack
        board[row][col] = PieceType.EMPTY;
      }
    };

    backtrack(0);
    return solutions;
  }

  /**
   * Check if a piece placement is valid (doesn't violate rules so far)
   */
  private isPlacementValid(board: PieceType[][], row: number, col: number): boolean {
    const piece = board[row][col];

    // Check consecutive rule in horizontal direction
    // Check left side (if we have 2+ pieces to the left)
    if (col >= 2) {
      if (board[row][col - 1] === board[row][col - 2] && 
          board[row][col - 1] === piece && 
          piece !== PieceType.EMPTY) {
        return false;
      }
    }
    
    // Check center (if we have 1 piece on each side)
    if (col >= 1 && col < this.size - 1) {
      if (board[row][col - 1] === piece && 
          piece === board[row][col + 1] && 
          piece !== PieceType.EMPTY) {
        return false;
      }
    }
    
    // Check right side (if we have 2+ pieces to the right)
    if (col < this.size - 2) {
      if (piece === board[row][col + 1] && 
          board[row][col + 1] === board[row][col + 2] && 
          piece !== PieceType.EMPTY) {
        return false;
      }
    }

    // Check consecutive rule in vertical direction
    // Check top side
    if (row >= 2) {
      if (board[row - 1][col] === board[row - 2][col] && 
          board[row - 1][col] === piece && 
          piece !== PieceType.EMPTY) {
        return false;
      }
    }
    
    // Check center
    if (row >= 1 && row < this.size - 1) {
      if (board[row - 1][col] === piece && 
          piece === board[row + 1][col] && 
          piece !== PieceType.EMPTY) {
        return false;
      }
    }
    
    // Check bottom side
    if (row < this.size - 2) {
      if (piece === board[row + 1][col] && 
          board[row + 1][col] === board[row + 2][col] && 
          piece !== PieceType.EMPTY) {
        return false;
      }
    }

    // Check constraints (only if adjacent pieces are filled)
    // Horizontal constraints
    if (col > 0 && this.hConstraints[row][col - 1] !== ConstraintType.NONE) {
      const leftPiece = board[row][col - 1];
      if (leftPiece !== PieceType.EMPTY) {
        if (this.hConstraints[row][col - 1] === ConstraintType.SAME && 
            leftPiece !== piece) {
          return false;
        }
        if (this.hConstraints[row][col - 1] === ConstraintType.DIFFERENT && 
            leftPiece === piece) {
          return false;
        }
      }
    }

    if (col < this.size - 1 && this.hConstraints[row][col] !== ConstraintType.NONE) {
      const rightPiece = board[row][col + 1];
      if (rightPiece !== PieceType.EMPTY) {
        if (this.hConstraints[row][col] === ConstraintType.SAME && 
            piece !== rightPiece) {
          return false;
        }
        if (this.hConstraints[row][col] === ConstraintType.DIFFERENT && 
            piece === rightPiece) {
          return false;
        }
      }
    }

    // Vertical constraints
    if (row > 0 && this.vConstraints[row - 1][col] !== ConstraintType.NONE) {
      const topPiece = board[row - 1][col];
      if (topPiece !== PieceType.EMPTY) {
        if (this.vConstraints[row - 1][col] === ConstraintType.SAME && 
            topPiece !== piece) {
          return false;
        }
        if (this.vConstraints[row - 1][col] === ConstraintType.DIFFERENT && 
            topPiece === piece) {
          return false;
        }
      }
    }

    if (row < this.size - 1 && this.vConstraints[row][col] !== ConstraintType.NONE) {
      const bottomPiece = board[row + 1][col];
      if (bottomPiece !== PieceType.EMPTY) {
        if (this.vConstraints[row][col] === ConstraintType.SAME && 
            piece !== bottomPiece) {
          return false;
        }
        if (this.vConstraints[row][col] === ConstraintType.DIFFERENT && 
            piece === bottomPiece) {
          return false;
        }
      }
    }

    return true;
  }

  /**
   * Check if a complete board is valid (all rules satisfied)
   */
  private isCompleteAndValid(board: PieceType[][]): boolean {
    // Check all positions are filled
    for (let row = 0; row < this.size; row++) {
      for (let col = 0; col < this.size; col++) {
        if (board[row][col] === PieceType.EMPTY) {
          return false;
        }
      }
    }

    // Check balance rule (3 suns and 3 moons per row/column)
    for (let row = 0; row < this.size; row++) {
      const sunCount = board[row].filter(p => p === PieceType.SUN).length;
      if (sunCount !== 3) {
        return false;
      }
    }

    for (let col = 0; col < this.size; col++) {
      const sunCount = Array.from({length: this.size}, (_, row) => board[row][col])
        .filter(p => p === PieceType.SUN).length;
      if (sunCount !== 3) {
        return false;
      }
    }

    // Check no more than 2 consecutive pieces
    for (let row = 0; row < this.size; row++) {
      for (let col = 0; col < this.size - 2; col++) {
        if (board[row][col] === board[row][col + 1] && 
            board[row][col + 1] === board[row][col + 2]) {
          return false;
        }
      }
    }

    for (let row = 0; row < this.size - 2; row++) {
      for (let col = 0; col < this.size; col++) {
        if (board[row][col] === board[row + 1][col] && 
            board[row + 1][col] === board[row + 2][col]) {
          return false;
        }
      }
    }

    // Check all constraints are satisfied
    for (let row = 0; row < this.size; row++) {
      for (let col = 0; col < this.size - 1; col++) {
        if (this.hConstraints[row][col] !== ConstraintType.NONE) {
          const left = board[row][col];
          const right = board[row][col + 1];
          if (this.hConstraints[row][col] === ConstraintType.SAME && left !== right) {
            return false;
          }
          if (this.hConstraints[row][col] === ConstraintType.DIFFERENT && left === right) {
            return false;
          }
        }
      }
    }

    for (let row = 0; row < this.size - 1; row++) {
      for (let col = 0; col < this.size; col++) {
        if (this.vConstraints[row][col] !== ConstraintType.NONE) {
          const top = board[row][col];
          const bottom = board[row + 1][col];
          if (this.vConstraints[row][col] === ConstraintType.SAME && top !== bottom) {
            return false;
          }
          if (this.vConstraints[row][col] === ConstraintType.DIFFERENT && top === bottom) {
            return false;
          }
        }
      }
    }

    return true;
  }

  /**
   * Get a hint for the next logical move
   */
  getHint(): HintResult {
    // Create current board state with only locked pieces
    const board = Array(this.size).fill(null).map(() => 
      Array(this.size).fill(PieceType.EMPTY)
    );
    
    for (let row = 0; row < this.size; row++) {
      for (let col = 0; col < this.size; col++) {
        if (this.lockedTiles[row][col]) {
          board[row][col] = this.originalBoard[row][col];
        }
      }
    }

    // Find positions that can be determined logically
    const logicalMoves = this.findLogicalMoves(board);

    if (logicalMoves.length > 0) {
      // Prioritize moves based on educational value (game rules > constraints)
      const educationalMoves = logicalMoves.filter(move => 
        move.reasoning.includes('Rule-based deduction') || 
        move.reasoning.includes('Balance rule deduction')
      );
      
      const constraintMoves = logicalMoves.filter(move => 
        move.reasoning.includes('Constraint-based deduction')
      );
      
      // Return the best educational move first, then constraint-based moves
      const bestMoves = educationalMoves.length > 0 ? educationalMoves : constraintMoves;
      const move = bestMoves.reduce((best, current) => 
        (current.confidence || 0) > (best.confidence || 0) ? current : best
      );
      
      return {
        found: true,
        row: move.row,
        col: move.col,
        pieceType: move.piece,
        reasoning: move.reasoning,
        confidence: move.confidence,
        hintType: 'logical_deduction',
        educationalValue: move.reasoning.includes('Rule-based') ? 'high' : 'medium',
      };
    }

    // If no logical moves, provide educational guidance about the board state
    const analysis = this.analyzeBoardState(board);
    if (analysis.hasOpportunities && analysis.suggestedRow !== undefined && 
        analysis.suggestedCol !== undefined && analysis.suggestedPiece !== undefined &&
        analysis.reasoning !== undefined && analysis.confidence !== undefined) {
      return {
        found: true,
        row: analysis.suggestedRow,
        col: analysis.suggestedCol,
        pieceType: analysis.suggestedPiece,
        reasoning: analysis.reasoning,
        confidence: analysis.confidence,
        hintType: 'strategic_guidance',
        educationalValue: 'medium',
      };
    }

    // Fallback to general strategic advice
    const fallbackMove = this.findBestGuess(board);
    if (fallbackMove) {
      return {
        found: true,
        row: fallbackMove.row,
        col: fallbackMove.col,
        pieceType: fallbackMove.piece,
        reasoning: fallbackMove.reasoning,
        confidence: fallbackMove.confidence,
        hintType: 'strategic_guess',
        educationalValue: 'low',
      };
    }

    return {
      found: false,
      reasoning: 'No valid moves found or puzzle is complete',
      hintType: 'none',
    };
  }

  /**
   * Find moves that can be determined through logical deduction
   */
  private findLogicalMoves(board: PieceType[][]): Array<{
    row: number;
    col: number;
    piece: PieceType;
    reasoning: string;
    confidence: number;
  }> {
    const logicalMoves: Array<{
      row: number;
      col: number;
      piece: PieceType;
      reasoning: string;
      confidence: number;
    }> = [];

    for (let row = 0; row < this.size; row++) {
      for (let col = 0; col < this.size; col++) {
        if (board[row][col] === PieceType.EMPTY) {
          // Check what pieces are possible at this position
          const possiblePieces: PieceType[] = [];

          for (const piece of [PieceType.SUN, PieceType.MOON]) {
            board[row][col] = piece;
            if (this.isPlacementValid(board, row, col)) {
              possiblePieces.push(piece);
            }
            board[row][col] = PieceType.EMPTY;
          }

          // If only one piece is possible, this is a logical move
          if (possiblePieces.length === 1) {
            const reasoning = this.analyzeWhyOnlyChoice(board, row, col, possiblePieces[0]);
            logicalMoves.push({
              row,
              col,
              piece: possiblePieces[0],
              reasoning,
              confidence: 100, // Certain move
            });
          }
        }
      }
    }

    return logicalMoves;
  }

  /**
   * Analyze why a particular piece is the only valid choice
   */
  private analyzeWhyOnlyChoice(
    board: PieceType[][], 
    row: number, 
    col: number, 
    piece: PieceType
  ): string {
    const reasons: string[] = [];
    const otherPiece = piece === PieceType.SUN ? PieceType.MOON : PieceType.SUN;

    // Test the other piece to see what rule it violates
    board[row][col] = otherPiece;

    // Check horizontal consecutive patterns
    if (col >= 2) {
      if (board[row][col - 1] === board[row][col - 2] && 
          board[row][col - 1] === otherPiece) {
        reasons.push(
          `placing ${otherPiece} would create three consecutive ${otherPiece}s in row ${row + 1} (game rule violation)`
        );
      }
    }
    if (col >= 1 && col < this.size - 1) {
      if (board[row][col - 1] === otherPiece && otherPiece === board[row][col + 1]) {
        reasons.push(
          `placing ${otherPiece} would create three consecutive ${otherPiece}s in row ${row + 1} (game rule violation)`
        );
      }
    }
    if (col < this.size - 2) {
      if (otherPiece === board[row][col + 1] && 
          board[row][col + 1] === board[row][col + 2]) {
        reasons.push(
          `placing ${otherPiece} would create three consecutive ${otherPiece}s in row ${row + 1} (game rule violation)`
        );
      }
    }

    // Check vertical consecutive patterns
    if (row >= 2) {
      if (board[row - 1][col] === board[row - 2][col] && 
          board[row - 1][col] === otherPiece) {
        reasons.push(
          `placing ${otherPiece} would create three consecutive ${otherPiece}s in column ${col + 1} (game rule violation)`
        );
      }
    }
    if (row >= 1 && row < this.size - 1) {
      if (board[row - 1][col] === otherPiece && otherPiece === board[row + 1][col]) {
        reasons.push(
          `placing ${otherPiece} would create three consecutive ${otherPiece}s in column ${col + 1} (game rule violation)`
        );
      }
    }
    if (row < this.size - 2) {
      if (otherPiece === board[row + 1][col] && 
          board[row + 1][col] === board[row + 2][col]) {
        reasons.push(
          `placing ${otherPiece} would create three consecutive ${otherPiece}s in column ${col + 1} (game rule violation)`
        );
      }
    }

    // Check balance constraints with detailed counting
    const rowSuns = board[row].filter(p => p === PieceType.SUN).length;
    const rowMoons = board[row].filter(p => p === PieceType.MOON).length;
    const colSuns = Array.from({length: this.size}, (_, r) => board[r][col])
      .filter(p => p === PieceType.SUN).length;
    const colMoons = Array.from({length: this.size}, (_, r) => board[r][col])
      .filter(p => p === PieceType.MOON).length;

    if (otherPiece === PieceType.SUN) {
      if (rowSuns >= 3) {
        reasons.push(`row ${row + 1} already has ${rowSuns} suns (max 3 per row)`);
      }
      if (colSuns >= 3) {
        reasons.push(`column ${col + 1} already has ${colSuns} suns (max 3 per column)`);
      }
    } else if (otherPiece === PieceType.MOON) {
      if (rowMoons >= 3) {
        reasons.push(`row ${row + 1} already has ${rowMoons} moons (max 3 per row)`);
      }
      if (colMoons >= 3) {
        reasons.push(`column ${col + 1} already has ${colMoons} moons (max 3 per column)`);
      }
    }

    board[row][col] = PieceType.EMPTY; // Reset

    if (reasons.length > 0) {
      const ruleReasons = reasons.filter(r => r.includes('game rule violation'));
      const balanceReasons = reasons.filter(r => r.includes('max 3'));
      
      // Prioritize game rule explanations over balance explanations
      let primaryReason: string;
      let explanationType: string;
      
      if (ruleReasons.length > 0) {
        primaryReason = ruleReasons[0];
        explanationType = 'Rule-based deduction';
      } else if (balanceReasons.length > 0) {
        primaryReason = balanceReasons[0];
        explanationType = 'Balance rule deduction';
      } else {
        primaryReason = reasons[0];
        explanationType = 'Constraint-based deduction';
      }
      
      return `${explanationType}: Only ${piece} works here because ${primaryReason}`;
    } else {
      return `Logical deduction: Only ${piece} is valid at position (${row + 1}, ${col + 1})`;
    }
  }

  /**
   * Analyze the current board state to provide strategic guidance
   */
  private analyzeBoardState(board: PieceType[][]): {
    hasOpportunities: boolean;
    suggestedRow?: number;
    suggestedCol?: number;
    suggestedPiece?: PieceType;
    reasoning?: string;
    confidence?: number;
  } {
    // Count pieces in each row and column
    const rowCounts: Array<{suns: number; moons: number; empty: number}> = [];
    const colCounts: Array<{suns: number; moons: number; empty: number}> = [];
    
    for (let i = 0; i < this.size; i++) {
      const rowSuns = board[i].filter(p => p === PieceType.SUN).length;
      const rowMoons = board[i].filter(p => p === PieceType.MOON).length;
      const rowEmpty = board[i].filter(p => p === PieceType.EMPTY).length;
      rowCounts.push({suns: rowSuns, moons: rowMoons, empty: rowEmpty});
      
      const colSuns = Array.from({length: this.size}, (_, r) => board[r][i])
        .filter(p => p === PieceType.SUN).length;
      const colMoons = Array.from({length: this.size}, (_, r) => board[r][i])
        .filter(p => p === PieceType.MOON).length;
      const colEmpty = Array.from({length: this.size}, (_, r) => board[r][i])
        .filter(p => p === PieceType.EMPTY).length;
      colCounts.push({suns: colSuns, moons: colMoons, empty: colEmpty});
    }

    // Look for rows/columns that are close to their limits
    for (let row = 0; row < this.size; row++) {
      if (rowCounts[row].empty > 0) {
        if (rowCounts[row].suns === 2) { // Almost at sun limit
          // Find empty cell in this row
          for (let col = 0; col < this.size; col++) {
            if (board[row][col] === PieceType.EMPTY) {
              return {
                hasOpportunities: true,
                suggestedRow: row,
                suggestedCol: col,
                suggestedPiece: PieceType.MOON,
                reasoning: `Strategic insight: Row ${row + 1} already has 2 suns. Focus on moon placements in this row to maintain balance.`,
                confidence: 70,
              };
            }
          }
        } else if (rowCounts[row].moons === 2) { // Almost at moon limit
          for (let col = 0; col < this.size; col++) {
            if (board[row][col] === PieceType.EMPTY) {
              return {
                hasOpportunities: true,
                suggestedRow: row,
                suggestedCol: col,
                suggestedPiece: PieceType.SUN,
                reasoning: `Strategic insight: Row ${row + 1} already has 2 moons. Focus on sun placements in this row to maintain balance.`,
                confidence: 70,
              };
            }
          }
        }
      }
    }

    // Look for columns with similar patterns
    for (let col = 0; col < this.size; col++) {
      if (colCounts[col].empty > 0) {
        if (colCounts[col].suns === 2) { // Almost at sun limit
          for (let row = 0; row < this.size; row++) {
            if (board[row][col] === PieceType.EMPTY) {
              return {
                hasOpportunities: true,
                suggestedRow: row,
                suggestedCol: col,
                suggestedPiece: PieceType.MOON,
                reasoning: `Strategic insight: Column ${col + 1} already has 2 suns. Focus on moon placements in this column to maintain balance.`,
                confidence: 70,
              };
            }
          }
        } else if (colCounts[col].moons === 2) { // Almost at moon limit
          for (let row = 0; row < this.size; row++) {
            if (board[row][col] === PieceType.EMPTY) {
              return {
                hasOpportunities: true,
                suggestedRow: row,
                suggestedCol: col,
                suggestedPiece: PieceType.SUN,
                reasoning: `Strategic insight: Column ${col + 1} already has 2 moons. Focus on sun placements in this column to maintain balance.`,
                confidence: 70,
              };
            }
          }
        }
      }
    }

    return {hasOpportunities: false};
  }

  /**
   * Find the best position to make an educated guess
   */
  private findBestGuess(board: PieceType[][]): {
    row: number;
    col: number;
    piece: PieceType;
    reasoning: string;
    confidence: number;
  } | null {
    // For now, just pick the first empty position
    for (let row = 0; row < this.size; row++) {
      for (let col = 0; col < this.size; col++) {
        if (board[row][col] === PieceType.EMPTY) {
          // Try both pieces and see which is more likely based on current state
          board[row][col] = PieceType.SUN;
          const sunValid = this.isPlacementValid(board, row, col);
          board[row][col] = PieceType.MOON;
          const moonValid = this.isPlacementValid(board, row, col);
          board[row][col] = PieceType.EMPTY;

          if (sunValid && !moonValid) {
            return {
              row,
              col,
              piece: PieceType.SUN,
              reasoning: 'Best logical guess based on current constraints',
              confidence: 75,
            };
          } else if (moonValid && !sunValid) {
            return {
              row,
              col,
              piece: PieceType.MOON,
              reasoning: 'Best logical guess based on current constraints',
              confidence: 75,
            };
          } else if (sunValid && moonValid) {
            return {
              row,
              col,
              piece: PieceType.SUN,
              reasoning: 'Either piece could work here - try sun first',
              confidence: 50,
            };
          }
        }
      }
    }

    return null;
  }
}
