#include <iostream>
#include <chrono>
#include <vector>

using namespace std;

enum Square {
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1
};

enum { white, black, both };

enum Dir {
	dN, dE, dS, dW, dNE, dNW, dSE, dSW
};

enum Piece {
	P, N, B, R, Q, K, p, n, b, r, q, k
};

const char* squareToCoords[] = {
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

// bishop relevant occupancy bit count for every square on board
const int bishopRelevantBits[64] = {
	6, 5, 5, 5, 5, 5, 5, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 5, 5, 5, 5, 5, 5, 6
};

// rook relevant occupancy bit count for every square on board
const int rookRelevantBits[64] = {
	12, 11, 11, 11, 11, 11, 11, 12,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	12, 11, 11, 11, 11, 11, 11, 12
};

uint64_t board[12];
uint64_t occupied[3];

class Move {
public:
	int from;
	int to;
	int piece;

	bool isCapture;

	Move()
	{
		from = 0;
		to = 0;
		piece = 0;
		isCapture = false;
	}

	Move(int from, int to, int piece, bool isCapture = false)
	{
		this->from = from;
		this->to = to;
		this->piece = piece;
		this->isCapture = isCapture;
	}
};

const uint64_t notAFile = 0xfefefefefefefefe;
const uint64_t notHFile = 0x7f7f7f7f7f7f7f7f;
const uint64_t notGHFile = 0x3f3f3f3f3f3f3f3f;
const uint64_t notABFile = 0xfcfcfcfcfcfcfcfc;

int wKingSqr, bKingSqr;

bool whiteToMove = true;

vector<Move> moveList;
vector<Move> moveHistory;

Move bestMove;
int positions = 0;

uint64_t pawnAttacks[2][64];
uint64_t knightAttacks[64];
uint64_t kingAttacks[64];
uint64_t bishopAttacks[64];
uint64_t rookAttacks[64];
uint64_t queenAttacks[64];

#define setBit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define getBit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define popBit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

int countSetBits(uint64_t bb)
{
	int count = 0;
	while (bb)
	{
		bb &= bb - 1;
		count++;
	}
	return count;
}

// gets the least significant first bit
int getFirstSetBit(uint64_t bb)
{
	int count = 0;

	while ((bb & 1) == 0)
	{
		count++;
		bb >>= 1;
	}

	return count;
}

uint64_t setOccupancy(int index, int bitsInMask, uint64_t attackMask)
{
	uint64_t occupancy = 0ULL;

	for (int count = 0; count < bitsInMask; count++)
	{
		int square = getFirstSetBit(attackMask);

		popBit(attackMask, square);

		if (index & (1 << count))
			occupancy |= (1ULL << square);
	}

	return occupancy;
}

void initBitboards()
{
	board[P] = 0b0000000011111111000000000000000000000000000000000000000000000000;
	board[p] = 0b0000000000000000000000000000000000000000000000001111111100000000;
	board[R] = 0b1000000100000000000000000000000000000000000000000000000000000000;
	board[r] = 0b0000000000000000000000000000000000000000000000000000000010000001;
	board[N] = 0b0100001000000000000000000000000000000000000000000000000000000000;
	board[n] = 0b0000000000000000000000000000000000000000000000000000000001000010;
	board[B] = 0b0010010000000000000000000000000000000000000000000000000000000000;
	board[b] = 0b0000000000000000000000000000000000000000000000000000000000100100;
	board[Q] = 0b0000100000000000000000000000000000000000000000000000000000000000;
	board[q] = 0b0000000000000000000000000000000000000000000000000000000000001000;
	board[K] = 0b0001000000000000000000000000000000000000000000000000000000000000;
	board[k] = 0b0000000000000000000000000000000000000000000000000000000000010000;
	occupied[white] = board[P] | board[N] | board[B] | board[R] | board[Q] | board[K];
	occupied[black] = board[p] | board[n] | board[b] | board[r] | board[q] | board[k];
	occupied[both] = occupied[white] | occupied[black];
}

void printBitboard(uint64_t bb)
{
	for (int i = 0; i < 64; i++)
	{
		if (i == a1 || i == a2 || i == a3 || i == a4 || i == a5 || i == a6 || i == a7)
			cout << endl;

		cout << (int)getBit(bb, i);
	}

	cout << endl << endl;
}

uint64_t maskPawnAttacks(int sqr, bool whiteToMove)
{
	uint64_t bitboard = 0ULL;
	uint64_t attacks = 0ULL;

	setBit(bitboard, sqr);

	if (whiteToMove)
	{
		if ((bitboard >> 7) & notAFile) attacks |= (bitboard >> 7);
		if ((bitboard >> 9) & notHFile) attacks |= (bitboard >> 9);
	}
	else
	{
		if ((bitboard << 7) & notHFile) attacks |= (bitboard << 7);
		if ((bitboard << 9) & notAFile) attacks |= (bitboard << 9);
	}

	return attacks;
}

uint64_t maskKnightAttacks(int sqr)
{
	uint64_t bitboard = 0ULL;
	uint64_t attacks = 0ULL;

	setBit(bitboard, sqr);

	if ((bitboard >> 15) & notAFile) attacks |= bitboard >> 15;
	if ((bitboard >> 17) & notHFile) attacks |= bitboard >> 17;
	if ((bitboard >> 10) & notGHFile) attacks |= bitboard >> 10;
	if ((bitboard >> 6) & notABFile) attacks |= bitboard >> 6;

	if ((bitboard << 15) & notHFile) attacks |= bitboard << 15;
	if ((bitboard << 17) & notAFile) attacks |= bitboard << 17;
	if ((bitboard << 10) & notABFile) attacks |= bitboard << 10;
	if ((bitboard << 6) & notGHFile) attacks |= bitboard << 6;

	return attacks;
}

uint64_t maskKingAttacks(int sqr)
{
	uint64_t bitboard = 0ULL;
	uint64_t attacks = 0ULL;

	setBit(bitboard, sqr);

	if ((bitboard >> 7) & notAFile) attacks |= bitboard >> 7;
	if (bitboard >> 8) attacks |= bitboard >> 8;
	if ((bitboard >> 9) & notHFile) attacks |= bitboard >> 9;
	if ((bitboard >> 1) & notHFile) attacks |= bitboard >> 1;
	if ((bitboard << 7) & notHFile) attacks |= bitboard << 7;
	if (bitboard << 8) attacks |= bitboard << 8;
	if ((bitboard << 9) & notAFile) attacks |= bitboard << 9;
	if ((bitboard << 1) & notAFile) attacks |= bitboard << 1;

	return attacks;
}

uint64_t maskBishopAttacks(int sqr)
{
	uint64_t attacks = 0ULL;

	int r, f;

	int tr = sqr / 8;
	int tf = sqr % 8;

	for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
	for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
	for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
	for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));

	return attacks;
}

uint64_t maskRookAttacks(int sqr)
{
	uint64_t attacks = 0ULL;

	int r, f;

	int tr = sqr / 8;
	int tf = sqr % 8;

	for (r = tr + 1; r <= 6; r++) attacks |= 1ULL << (r * 8 + tf);
	for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
	for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
	for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));

	return attacks;
}

uint64_t bishopAttacksOTF(int sqr, uint64_t blockers)
{
	uint64_t attacks = 0ULL;

	int r, f;

	int tr = sqr / 8;
	int tf = sqr % 8;

	for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}
	for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}
	for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}
	for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}

	return attacks;
}

uint64_t rookAttacksOTF(int square, uint64_t blockers)
{
	uint64_t attacks = 0ULL;

	int r, f;

	int tr = square / 8;
	int tf = square % 8;

	for (r = tr + 1; r <= 7; r++)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if ((1ULL << (r * 8 + tf)) & blockers) break;
	}

	for (r = tr - 1; r >= 0; r--)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if ((1ULL << (r * 8 + tf)) & blockers) break;
	}

	for (f = tf + 1; f <= 7; f++)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if ((1ULL << (tr * 8 + f)) & blockers) break;
	}

	for (f = tf - 1; f >= 0; f--)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if ((1ULL << (tr * 8 + f)) & blockers) break;
	}

	return attacks;
}

void initLeaperAttacks()
{
	for (int sqr = 0; sqr < 64; sqr++)
	{
		pawnAttacks[white][sqr] = maskPawnAttacks(sqr, true);
		pawnAttacks[black][sqr] = maskPawnAttacks(sqr, false);

		knightAttacks[sqr] = maskKnightAttacks(sqr);

		kingAttacks[sqr] = maskKingAttacks(sqr);

		bishopAttacks[sqr] = maskBishopAttacks(sqr);

		rookAttacks[sqr] = maskRookAttacks(sqr);
	}
}

void initSliderAttacks()
{
	for (int sqr = 0; sqr < 64; sqr++)
	{
		bishopAttacks[sqr] = maskBishopAttacks(sqr);
		rookAttacks[sqr] = maskRookAttacks(sqr);
	}
}

void initAttackMasks()
{
	initLeaperAttacks();
	initSliderAttacks();
}

void printBoard()
{
	for (int i = 0; i < 64; i++)
	{
		if (i == a1 || i == a2 || i == a3 || i == a4 || i == a5 || i == a6 || i == a7)
			cout << endl;

		if (getBit(board[P], i))
			cout << " P";
		else if (getBit(board[p], i))
			cout << " p";
		else if (getBit(board[N], i))
			cout << " N";
		else if (getBit(board[n], i))
			cout << " n";
		else if (getBit(board[B], i))
			cout << " B";
		else if (getBit(board[b], i))
			cout << " b";
		else if (getBit(board[R], i))
			cout << " R";
		else if (getBit(board[r], i))
			cout << " r";
		else if (getBit(board[Q], i))
			cout << " Q";
		else if (getBit(board[q], i))
			cout << " q";
		else if (getBit(board[K], i))
			cout << " K";
		else if (getBit(board[k], i))
			cout << " k";
		else 
			cout << " -";
	}

	cout << endl << endl << "Side: " << whiteToMove << endl << endl;

	cout << "Bitboard: " << (uint64_t)occupied[both] << "ULL" << endl << endl;
}

uint64_t getRookMoves(Square sqr, uint64_t blockers)
{;
	return rookAttacksOTF(sqr, blockers);
}

uint64_t getBishopMoves(Square sqr, uint64_t blockers)
{
	return bishopAttacksOTF(sqr, blockers);
}

uint64_t getLegalMoves(bool whiteToMove)
{
	return 1;
}

// populates the moveList
void generateMoves()
{
	int source, target;

	if (whiteToMove)
	{
		// generate quiet pawn moves
		uint64_t pawnMoves = board[P] << 8;

		while (pawnMoves)
		{
			target = getFirstSetBit(pawnMoves);
			source = target + 8;

			if (!getBit(occupied[both], target))
			{
				// add move to movelist
				moveList.push_back(Move(source, target, P));
			}

			popBit(pawnMoves, target);
		}

		//pawnMoves = getDoublePawnPushes(true);

		while (pawnMoves)
		{
			target = getFirstSetBit(pawnMoves);
			source = target + 16;

			if (!getBit(occupied[both], target))
			{
				moveList.push_back(Move(source, target, P));
			}

			popBit(pawnMoves, target);
		}
	}
	else
	{
		uint64_t pawnMoves = 0;// = getSinglePawnPushes(false);

		while (pawnMoves)
		{
			target = getFirstSetBit(pawnMoves);
			source = target - 8;

			if (!getBit(occupied[both], target))
			{
				moveList.push_back(Move(source, target, p));
			}

			popBit(pawnMoves, target);
		}

		//pawnMoves = getDoublePawnPushes(false);

		while (pawnMoves)
		{
			target = getFirstSetBit(pawnMoves);
			source = target - 16;

			if (!getBit(occupied[both], target))
			{
				moveList.push_back(Move(source, target, p));
			}

			popBit(pawnMoves, target);
		}
	}

	// generate pawn captures
	uint64_t pawnAtcs;
	uint64_t pawns = whiteToMove ? board[P] : board[p];

	while (pawns)
	{
		source = getFirstSetBit(pawns);
		pawnAtcs = pawnAttacks[whiteToMove ? white : black][source] & occupied[whiteToMove ? white : black];

		while (pawnAtcs)
		{
			target = getFirstSetBit(pawnAtcs);

			moveList.push_back(Move(source, target, whiteToMove ? P : p));

			popBit(pawnAtcs, target);
		}

		popBit(pawns, source);
	}

	// generate knight moves
	uint64_t knightMoves;
	uint64_t knightBb = whiteToMove ? board[N] : board[n];

	while (knightBb)
	{
		source = getFirstSetBit(knightBb);

		knightMoves = knightAttacks[source] & (whiteToMove ? ~occupied[white] : ~occupied[black]);

		while (knightMoves)
		{
			target = getFirstSetBit(knightMoves);

			moveList.push_back(Move(source, target, whiteToMove ? N : n));

			popBit(knightMoves, target);
		}

		popBit(knightBb, source);
	}

	
	uint64_t kingBb = whiteToMove ? board[K] : board[k];
	source = getFirstSetBit(kingBb);
	uint64_t kingMoves = kingAttacks[source];


}

void printMoveList()
{
	for (Move move : moveList)
		cout << squareToCoords[move.from] << squareToCoords[move.to] << endl;

	cout << endl;
}

bool isSquareAttacked(Square square, bool whiteToMove)
{
	if (getBishopMoves(square, occupied[both] & bishopAttacks[square]) & (whiteToMove ? board[B] : board[b])) return true;

	if (getRookMoves(square, occupied[both] & rookAttacks[square]) & (whiteToMove ? board[R] : board[r])) return true;

	return false;
}

void makeMove(Move move)
{
	setBit(board[move.piece], move.to);
	popBit(board[move.piece], move.from);
	popBit(occupied[both], move.from);
	setBit(occupied[both], move.to);
	
	if (whiteToMove)
		popBit(occupied[white], move.from);
	else
		popBit(occupied[black], move.from);

	if (move.piece == K)
		wKingSqr = move.to;
	else if (move.piece == k)
		bKingSqr = move.to;

	whiteToMove ^= 1;

	moveHistory.push_back(move);
}

void undoMove()
{
	Move move = moveHistory.back();

	setBit(board[move.piece], move.from);
	popBit(board[move.piece], move.to);
	setBit(occupied[both], move.to);
	popBit(occupied[both], move.from);

	if (whiteToMove)
		popBit(occupied[white], move.from);
	else
		popBit(occupied[black], move.from);

	if (move.piece == K)
		wKingSqr = move.from;
	else if (move.piece == k)
		bKingSqr = move.from;

	whiteToMove ^= 1;

	moveHistory.pop_back();
}

double evaluate()
{
	int ps = 0, ns = 0, rs = 0, bs = 0, qs = 0, ks = 0, Ps = 0, Ns = 0, Bs = 0, Rs = 0, Qs = 0, Ks = 0;

	ps = countSetBits(board[p]);
	ns = countSetBits(board[n]);
	rs = countSetBits(board[r]);
	bs = countSetBits(board[b]);
	qs = countSetBits(board[q]);
	ks = countSetBits(board[k]);
	Ps = countSetBits(board[P]);
	Ns = countSetBits(board[N]);
	Rs = countSetBits(board[R]);
	Bs = countSetBits(board[B]);
	Qs = countSetBits(board[Q]);
	Ks = countSetBits(board[K]);

	double score = 200 * (Ks - ks) + 9 * (Qs - qs) + 5 * (Rs - rs) + 3 * (Bs - bs + Ns - ns) + 1 * (Ps - ps);

	if (whiteToMove) 
		return score;
	else
		return -score;
}

int quiesce(int alpha, int beta)
{
	int standPat = evaluate();

	if (standPat >= beta)
		return beta;
	if (alpha < standPat)
		alpha = standPat;

	for (Move move : moveList)
	{
		if (!move.isCapture)
			continue;

		makeMove(move);
		int score = -quiesce(-beta, -alpha);
		undoMove();

		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}

	return alpha;
}

bool isGameOver()
{
	if (getLegalMoves(true) == 0)
		return true;
	else if (getLegalMoves(false) == 0)
		return true;

	return false;
}

int negamax(int depth, int alpha, int beta)
{
	if (depth == 0 || isGameOver())
		return quiesce(alpha, beta);

	int score = 0;
	int bestScore = -10000;

	generateMoves();

	for(Move move : moveList)
	{
		if (move.from < 0)
		{
			cout << "Error move!" << endl;
			continue; //invalid move
		}

		positions++;

		makeMove(move);
		score = -negamax(depth - 1, -beta, -alpha);
		undoMove();

		if (score > bestScore)
		{
			bestScore = score;
			bestMove = move;
		}

		alpha = max(alpha, bestScore);

		if (alpha >= beta)
			break;
	}

	return bestScore;
}

int main()
{
	//initBitboards();
	initAttackMasks();

	

	return 0;

	printBoard();

	auto start = chrono::high_resolution_clock::now();

	int score = negamax(4, -10000, 10000);

	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start).count();

	cout << "Position Score: " << score << endl;
	cout << "Positions Evaluated: " << positions << endl;
	cout << "Run time: " << duration << " milliseconds" << endl << endl;

	positions = 0;

	printBoard();

	return 0;
}
