#include <iostream>
#include <chrono>
#include <Bits.h>

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

uint64_t board[12];
uint64_t occupied[3];
uint64_t emptyBb[3];

class Move {
public:
	Square from;
	Square to;
	Piece piece;

	bool isCapture;

	Move()
	{
	}

	Move(Square from, Square to, Piece piece, bool isCapture = false)
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

Square wKingSqr, bKingSqr;

bool whiteToMove = true;

Move moveList[200];
int moveListIndex = 0;

Move bestMove;
int positions = 0;

uint64_t pawnAttacks[2][64];
uint64_t knightAttacks[64];
uint64_t kingAttacks[64];
uint64_t bishopAttacks[64];
uint64_t rookAttacks[64];
uint64_t queenAttacks[64];

uint64_t soutOne(uint64_t b) { return  b >> 8; }
uint64_t nortOne(uint64_t b) { return  b << 8; }
uint64_t eastOne(uint64_t b) { return (b & notHFile) << 1; }
uint64_t noEaOne(uint64_t b) { return (b & notHFile) << 9; }
uint64_t soEaOne(uint64_t b) { return (b & notHFile) >> 7; }
uint64_t westOne(uint64_t b) { return (b & notAFile) >> 1; }
uint64_t soWeOne(uint64_t b) { return (b & notAFile) >> 9; }
uint64_t noWeOne(uint64_t b) { return (b & notAFile) << 7; }

/*uint64_t soutFill(uint64_t gen) {
	gen |= (gen >> 8);
	gen |= (gen >> 16);
	gen |= (gen >> 32);
	return gen;
}

uint64_t nortFill(uint64_t gen) {
	gen |= (gen << 8);
	gen |= (gen << 16);
	gen |= (gen << 32);
	return gen;
}

uint64_t eastFill(uint64_t gen) {
	const uint64_t pr0 = notAFile;
	const uint64_t pr1 = pr0 & (pr0 << 1);
	const uint64_t pr2 = pr1 & (pr1 << 2);
	gen |= pr0 & (gen << 1);
	gen |= pr1 & (gen << 2);
	gen |= pr2 & (gen << 4);
	return gen;
}

uint64_t noEaFill(uint64_t gen) {
	const uint64_t pr0 = notAFile;
	const uint64_t pr1 = pr0 & (pr0 << 9);
	const uint64_t pr2 = pr1 & (pr1 << 18);
	gen |= pr0 & (gen << 9);
	gen |= pr1 & (gen << 18);
	gen |= pr2 & (gen << 36);
	return gen;
}

uint64_t soEaFill(uint64_t gen) {
	const uint64_t pr0 = notAFile;
	const uint64_t pr1 = pr0 & (pr0 >> 7);
	const uint64_t pr2 = pr1 & (pr1 >> 14);
	gen |= pr0 & (gen >> 7);
	gen |= pr1 & (gen >> 14);
	gen |= pr2 & (gen >> 28);
	return gen;
}

uint64_t westFill(uint64_t gen) {
	const uint64_t pr0 = notHFile;
	const uint64_t pr1 = pr0 & (pr0 >> 1);
	const uint64_t pr2 = pr1 & (pr1 >> 2);
	gen |= pr0 & (gen >> 1);
	gen |= pr1 & (gen >> 2);
	gen |= pr2 & (gen >> 4);
	return gen;
}

uint64_t soWeFill(uint64_t gen) {
	const uint64_t pr0 = notHFile;
	const uint64_t pr1 = pr0 & (pr0 >> 9);
	const uint64_t pr2 = pr1 & (pr1 >> 18);
	gen |= pr0 & (gen >> 9);
	gen |= pr1 & (gen >> 18);
	gen |= pr2 & (gen >> 36);
	return gen;
}

uint64_t noWeFill(uint64_t gen) {
	const uint64_t pr0 = notHFile;
	const uint64_t pr1 = pr0 & (pr0 << 7);
	const uint64_t pr2 = pr1 & (pr1 << 14);
	gen |= pr0 & (gen << 7);
	gen |= pr1 & (gen << 14);
	gen |= pr2 & (gen << 28);
	return gen;
}*/

uint64_t soutOccl(uint64_t gen, uint64_t pro) {
	gen |= pro & (gen >> 8);
	pro &= (pro >> 8);
	gen |= pro & (gen >> 16);
	pro &= (pro >> 16);
	gen |= pro & (gen >> 32);
	return gen;
}

uint64_t nortOccl(uint64_t gen, uint64_t pro) {
	gen |= pro & (gen << 8);
	pro &= (pro << 8);
	gen |= pro & (gen << 16);
	pro &= (pro << 16);
	gen |= pro & (gen << 32);
	return gen;
}

uint64_t eastOccl(uint64_t gen, uint64_t pro) {
	pro &= notAFile;
	gen |= pro & (gen << 1);
	pro &= (pro << 1);
	gen |= pro & (gen << 2);
	pro &= (pro << 2);
	gen |= pro & (gen << 4);
	return gen;
}

uint64_t noEaOccl(uint64_t gen, uint64_t pro) {
	pro &= notAFile;
	gen |= pro & (gen << 9);
	pro &= (pro << 9);
	gen |= pro & (gen << 18);
	pro &= (pro << 18);
	gen |= pro & (gen << 36);
	return gen;
}

uint64_t soEaOccl(uint64_t gen, uint64_t pro) {
	pro &= notAFile;
	gen |= pro & (gen >> 7);
	pro &= (pro >> 7);
	gen |= pro & (gen >> 14);
	pro &= (pro >> 14);
	gen |= pro & (gen >> 28);
	return gen;
}

uint64_t westOccl(uint64_t gen, uint64_t pro) {
	pro &= notHFile;
	gen |= pro & (gen >> 1);
	pro &= (pro >> 1);
	gen |= pro & (gen >> 2);
	pro &= (pro >> 2);
	gen |= pro & (gen >> 4);
	return gen;
}

uint64_t soWeOccl(uint64_t gen, uint64_t pro) {
	pro &= notHFile;
	gen |= pro & (gen >> 9);
	pro &= (pro >> 9);
	gen |= pro & (gen >> 18);
	pro &= (pro >> 18);
	gen |= pro & (gen >> 36);
	return gen;
}

uint64_t noWeOccl(uint64_t gen, uint64_t pro) {
	pro &= notHFile;
	gen |= pro & (gen << 7);
	pro &= (pro << 7);
	gen |= pro & (gen << 14);
	pro &= (pro << 14);
	gen |= pro & (gen << 28);
	return gen;
}

uint64_t soutAttacks(uint64_t rooks, uint64_t empty) { return soutOne(soutOccl(rooks, empty)); }
uint64_t nortAttacks(uint64_t rooks, uint64_t empty) { return nortOne(nortOccl(rooks, empty)); }
uint64_t eastAttacks(uint64_t rooks, uint64_t empty) { return eastOne(eastOccl(rooks, empty)); }
uint64_t noEaAttacks(uint64_t bishops, uint64_t empty) { return noEaOne(noEaOccl(bishops, empty)); }
uint64_t soEaAttacks(uint64_t bishops, uint64_t empty) { return soEaOne(soEaOccl(bishops, empty)); }
uint64_t westAttacks(uint64_t rooks, uint64_t empty) { return westOne(westOccl(rooks, empty)); }
uint64_t soWeAttacks(uint64_t bishops, uint64_t empty) { return soWeOne(soWeOccl(bishops, empty)); }
uint64_t noWeAttacks(uint64_t bishops, uint64_t empty) { return noWeOne(noWeOccl(bishops, empty)); }

BYTE getBit(uint64_t bb, int index)
{
	return (bb >> index) & 1ULL;
}

void setBit(uint64_t *bb, int index)
{
	*bb |= 1ULL << index;
}

void popBit(uint64_t *bb, int index)
{
	getBit(*bb, index) ? *bb ^= (1ULL << index) : 0;
}

void toggleBit(uint64_t* bb, int index)
{
	*bb ^= 1ULL << index;
}

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
	emptyBb[both] = ~(board[P] | board[p] | board[R] | board[r] | board[B] | board[b] | board[N] | board[n] | board[Q] | board[q] | board[K] | board[k]);
	occupied[both] = board[P] | board[p] | board[R] | board[r] | board[B] | board[b] | board[N] | board[n] | board[Q] | board[q] | board[K] | board[k];
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

uint64_t maskPawnAttacks(int sqr, int turn)
{
	uint64_t bitboard = 0ULL;
	uint64_t attacks = 0ULL;

	setBit(&bitboard, sqr);

	if (turn == white)
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

	setBit(&bitboard, sqr);

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

	setBit(&bitboard, sqr);

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

	for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}
	for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}
	for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}
	for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}

	return attacks;
}

uint64_t rookAttacksOTF(int square, uint64_t blockers)
{
	// result attacks bitboard
	uint64_t attacks = 0ULL;

	// init ranks & files
	int r, f;

	// init target rank & files
	int tr = square / 8;
	int tf = square % 8;

	// generate rook attacks
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

	// return attack map
	return attacks;
}

void initLeaperAttacks()
{
	for (int sqr = a1; sqr <= h8; sqr++)
	{
		pawnAttacks[white][sqr] = maskPawnAttacks(sqr, white);
		pawnAttacks[black][sqr] = maskPawnAttacks(sqr, black);

		knightAttacks[sqr] = maskKnightAttacks(sqr);

		kingAttacks[sqr] = maskKingAttacks(sqr);

		bishopAttacks[sqr] = maskBishopAttacks(sqr);

		rookAttacks[sqr] = maskRookAttacks(sqr);
	}
}

void initAttackMasks()
{
	initLeaperAttacks();
}

void printBoard()
{
	for (int i = 0; i < 64; i++)
	{
		if (i == a1 || i == a2 || i == a3 || i == a4 || i == a5 || i == a6 || i == a7)
			cout << endl;

		if (getBit(board[P], i) == 1)
			cout << " wP";
		else if (getBit(board[p], i))
			cout << " bP";
		else if (getBit(board[N], i))
			cout << " wN";
		else if (getBit(board[n], i))
			cout << " bN";
		else if (getBit(board[B], i))
			cout << " wB";
		else if (getBit(board[b], i))
			cout << " bB";
		else if (getBit(board[R], i))
			cout << " wR";
		else if (getBit(board[r], i))
			cout << " bR";
		else if (getBit(board[Q], i))
			cout << " wQ";
		else if (getBit(board[q], i))
			cout << " bQ";
		else if (getBit(board[K], i))
			cout << " wK";
		else if (getBit(board[k], i))
			cout << " bK";
		else 
			cout << " --";
	}

	cout << endl << endl;
}

uint64_t getSinglePawnPushes(bool whiteToMove)
{
	if (whiteToMove)
		return (board[P] >> 8) & emptyBb[both];
	else
		return (board[p] << 8) & emptyBb[both];
}

uint64_t getDoublePawnPushes(bool whiteToMove)
{
	uint64_t rank5 = 0x0000000FF00000000;
	uint64_t rank4 = 0x00000000FF000000;
	
	if (whiteToMove)
		return soutOne(getSinglePawnPushes(whiteToMove)) & emptyBb[both] & rank5;
	else
		return nortOne(getSinglePawnPushes(whiteToMove)) & emptyBb[both] & rank4;
}

uint64_t getPawnCaptures(bool whiteToMove)
{
	uint64_t eastCapture, westCapture;

	if (whiteToMove)
	{
		eastCapture = soEaOne(board[P]);
		westCapture = soWeOne(board[P]);

		return eastCapture | westCapture;
	}
	else
	{
		eastCapture = noEaOne(board[p]);
		westCapture = noWeOne(board[p]);

		return eastCapture | westCapture;
	}
}

uint64_t getPawnMoves(bool whiteToMove)
{
	uint64_t singlePushs, doublePushs, eastCapture, westCapture;

	if (whiteToMove)
	{
		uint64_t rank5 = 0x0000000FF00000000;

		singlePushs = soutOne(board[P]) & board[P];
		doublePushs = soutOne(singlePushs) & emptyBb[both] & rank5;
		eastCapture = soEaOne(board[P]);
		westCapture = soWeOne(board[P]);

		return singlePushs | doublePushs | eastCapture | westCapture;
	}
	else
	{
		uint64_t rank4 = 0x00000000FF000000;

		singlePushs = nortOne(board[p]) & board[p];
		doublePushs = nortOne(singlePushs) & emptyBb[both] & rank4;
		eastCapture = noEaOne(board[p]);
		westCapture = noWeOne(board[p]);

		return singlePushs | doublePushs | eastCapture | westCapture;
	}
}

uint64_t getKnightMoves(uint64_t knightBb, uint64_t emptyBb)
{
	uint64_t west, east, moves;

	east = eastOne(knightBb);
	west = westOne(knightBb);
	moves = (east | west) << 16;
	moves |= (east | west) >> 16;
	east = eastOne(east);
	west = westOne(west);
	moves |= (east | west) << 8;
	moves |= (east | west) >> 8;

	return moves & emptyBb;
}

uint64_t getKingMoves(uint64_t kingBb)
{
	uint64_t moves = eastOne(kingBb) | westOne(kingBb);
	kingBb |= moves;
	moves |= nortOne(kingBb) | soutOne(kingBb);
	return moves; // & bbs.empty
}

uint64_t getRookMoves()
{
	uint64_t blockers = 0ULL;
	uint64_t rookAtcs = rookAttacksOTF(d4, blockers);

	return rookAtcs;
}

uint64_t getBishopMoves(uint64_t bishopBb, uint64_t occupied)
{
	return 0;
}

uint64_t getLegalMoves(bool whiteToMove)
{
	uint64_t pawnMoves, knightMoves, bishopMoves, rookMoves, queenMoves, kingMoves;

	if (whiteToMove)
	{
		pawnMoves = getPawnMoves(true);
		knightMoves = getKnightMoves(board[N], emptyBb[both]);
		kingMoves = getKingMoves(board[K]);
	}
	else
	{
		pawnMoves = getPawnMoves(false);
		knightMoves = getKnightMoves(board[n], emptyBb[both]);
		kingMoves = getKingMoves(board[k]);
	}

	return pawnMoves | knightMoves | kingMoves;
}

// populates the moveList
void generateMoves()
{
	moveListIndex = 0;

	if (whiteToMove)
	{
		uint64_t pawnMoves = getSinglePawnPushes(true);

		while (pawnMoves)
		{
			Square target = (Square)getFirstSetBit(pawnMoves);
			Square source = (Square)(target + 8);

			moveList[moveListIndex] = Move(source, target, P);
			moveListIndex++;

			popBit(&pawnMoves, target);
		}
	}
	else
	{
		uint64_t pawnMoves = getSinglePawnPushes(false);

		while (pawnMoves)
		{
			Square target = (Square)getFirstSetBit(pawnMoves);
			Square source = (Square)(target - 8);

			moveList[moveListIndex] = Move(source, target, p);
			moveListIndex++;

			popBit(&pawnMoves, target);
		}
	}
}

void printMoveList()
{
	for (int i = 0; i < moveListIndex; i++)
	{
		cout << squareToCoords[moveList[i].from] << squareToCoords[moveList[i].to] << endl;
	}
	cout << endl;
}

bool isSquareAttacked(int square, bool whiteToMove)
{
	if (whiteToMove && (getPawnCaptures(false) & board[P])) return 1;
	if (!whiteToMove && (getPawnCaptures(true) & board[P])) return 1;

	if ((whiteToMove ? getKnightMoves(board[N], emptyBb[both]) : getKnightMoves(board[n], emptyBb[both])) & (whiteToMove ? board[N] : board[b])) return 1;

	if ((whiteToMove ? getKingMoves(board[K]) : getKingMoves(board[k])) & (whiteToMove ? board[K] : board[k])) return 1;

	return false;
}

void makeMove(Move move)
{
	setBit(&board[move.piece], move.to);
	popBit(&board[move.piece], move.from);
	setBit(&emptyBb[both], move.from);
	popBit(&occupied[both], move.from);

	if (board[move.piece] == board[K])
		wKingSqr = move.to;
	else if (board[move.piece] == board[k])
		bKingSqr = move.to;

	whiteToMove = !whiteToMove;
}

void undoMove(Move move)
{
	setBit(&board[move.piece], move.from);
	popBit(&board[move.piece], move.to);
	popBit(&emptyBb[both], move.to);
	setBit(&occupied[both], move.to);

	if (board[move.piece] == board[K])
		wKingSqr = move.from;
	else if (board[move.piece] == board[k])
		bKingSqr = move.from;

	whiteToMove = !whiteToMove;
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

	//for (every capture)
	{
		//makeMove(capture);
		int score = -quiesce(-beta, -alpha);
		//undoMove();

		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}

	return alpha;
}

bool isGameOver()
{
	if (getSinglePawnPushes(true) == 0)
		return true;
	else if (getSinglePawnPushes(false) == 0)
		return true;

	return false;
}

int negamax(int depth, int alpha, int beta, bool whiteToMove)
{
	if (depth == 0 || isGameOver())
		return evaluate();

	int bestScore = -10000;

	generateMoves();

	for(int i = 0; i < moveListIndex; i++)
	{
		positions++;

		generateMoves();

		makeMove(moveList[i]);
		int score = -negamax(depth - 1, -beta, -alpha, !whiteToMove);
		undoMove(moveList[i]);

		if (score > bestScore)
		{
			bestScore = score;
			bestMove = moveList[i];
		}

		alpha = max(alpha, bestScore);

		if (alpha >= beta)
			break;
	}

	return bestScore;
}

int main()
{
	auto start = chrono::high_resolution_clock::now();

	initBitboards();
	//initAttackMasks();

	printBoard();

	generateMoves();
	
	int score = negamax(4, -10000, 10000, true);

	cout << "Position Score: " << score << endl;
	cout << "Positions Evaluated: " << positions << endl;

	positions = 0;

	printBoard();
	
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start).count();

	cout << "Run time: " << duration << " milliseconds" << endl;
	
	return 0;
}