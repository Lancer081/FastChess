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
	a1, b1, c1, d1, e1, f1, g1, h1, noSqr
};

enum Side { white, black, both };

enum Dir {
	dN, dE, dS, dW, dNE, dNW, dSE, dSW
};

enum Piece {
	P, N, B, R, Q, K, p, n, b, r, q, k
};

enum CastlingRights { wk = 1, wq = 2, bk = 4, bq = 8 };

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

// rook magic numbers
uint64_t rookMagicNumbers[64] = {
	0x8a80104000800020ULL,
	0x140002000100040ULL,
	0x2801880a0017001ULL,
	0x100081001000420ULL,
	0x200020010080420ULL,
	0x3001c0002010008ULL,
	0x8480008002000100ULL,
	0x2080088004402900ULL,
	0x800098204000ULL,
	0x2024401000200040ULL,
	0x100802000801000ULL,
	0x120800800801000ULL,
	0x208808088000400ULL,
	0x2802200800400ULL,
	0x2200800100020080ULL,
	0x801000060821100ULL,
	0x80044006422000ULL,
	0x100808020004000ULL,
	0x12108a0010204200ULL,
	0x140848010000802ULL,
	0x481828014002800ULL,
	0x8094004002004100ULL,
	0x4010040010010802ULL,
	0x20008806104ULL,
	0x100400080208000ULL,
	0x2040002120081000ULL,
	0x21200680100081ULL,
	0x20100080080080ULL,
	0x2000a00200410ULL,
	0x20080800400ULL,
	0x80088400100102ULL,
	0x80004600042881ULL,
	0x4040008040800020ULL,
	0x440003000200801ULL,
	0x4200011004500ULL,
	0x188020010100100ULL,
	0x14800401802800ULL,
	0x2080040080800200ULL,
	0x124080204001001ULL,
	0x200046502000484ULL,
	0x480400080088020ULL,
	0x1000422010034000ULL,
	0x30200100110040ULL,
	0x100021010009ULL,
	0x2002080100110004ULL,
	0x202008004008002ULL,
	0x20020004010100ULL,
	0x2048440040820001ULL,
	0x101002200408200ULL,
	0x40802000401080ULL,
	0x4008142004410100ULL,
	0x2060820c0120200ULL,
	0x1001004080100ULL,
	0x20c020080040080ULL,
	0x2935610830022400ULL,
	0x44440041009200ULL,
	0x280001040802101ULL,
	0x2100190040002085ULL,
	0x80c0084100102001ULL,
	0x4024081001000421ULL,
	0x20030a0244872ULL,
	0x12001008414402ULL,
	0x2006104900a0804ULL,
	0x1004081002402ULL
};

// bishop magic numbers
uint64_t bishopMagicNumbers[64] = {
	0x40040844404084ULL,
	0x2004208a004208ULL,
	0x10190041080202ULL,
	0x108060845042010ULL,
	0x581104180800210ULL,
	0x2112080446200010ULL,
	0x1080820820060210ULL,
	0x3c0808410220200ULL,
	0x4050404440404ULL,
	0x21001420088ULL,
	0x24d0080801082102ULL,
	0x1020a0a020400ULL,
	0x40308200402ULL,
	0x4011002100800ULL,
	0x401484104104005ULL,
	0x801010402020200ULL,
	0x400210c3880100ULL,
	0x404022024108200ULL,
	0x810018200204102ULL,
	0x4002801a02003ULL,
	0x85040820080400ULL,
	0x810102c808880400ULL,
	0xe900410884800ULL,
	0x8002020480840102ULL,
	0x220200865090201ULL,
	0x2010100a02021202ULL,
	0x152048408022401ULL,
	0x20080002081110ULL,
	0x4001001021004000ULL,
	0x800040400a011002ULL,
	0xe4004081011002ULL,
	0x1c004001012080ULL,
	0x8004200962a00220ULL,
	0x8422100208500202ULL,
	0x2000402200300c08ULL,
	0x8646020080080080ULL,
	0x80020a0200100808ULL,
	0x2010004880111000ULL,
	0x623000a080011400ULL,
	0x42008c0340209202ULL,
	0x209188240001000ULL,
	0x400408a884001800ULL,
	0x110400a6080400ULL,
	0x1840060a44020800ULL,
	0x90080104000041ULL,
	0x201011000808101ULL,
	0x1a2208080504f080ULL,
	0x8012020600211212ULL,
	0x500861011240000ULL,
	0x180806108200800ULL,
	0x4000020e01040044ULL,
	0x300000261044000aULL,
	0x802241102020002ULL,
	0x20906061210001ULL,
	0x5a84841004010310ULL,
	0x4010801011c04ULL,
	0xa010109502200ULL,
	0x4a02012000ULL,
	0x500201010098b028ULL,
	0x8040002811040900ULL,
	0x28000010020204ULL,
	0x6000020202d0240ULL,
	0x8918844842082200ULL,
	0x4010011029020020ULL
};

const uint64_t notAFile = 0xfefefefefefefefe;
const uint64_t notHFile = 0x7f7f7f7f7f7f7f7f;
const uint64_t notGHFile = 0x3f3f3f3f3f3f3f3f;
const uint64_t notABFile = 0xfcfcfcfcfcfcfcfc;

class Move {
public:
	int from;
	int to;
	int piece;
	int promotedPiece;
	int enpassant;
	int castling;

	bool isCapture;

	Move()
	{
		from = 0;
		to = 0;
		piece = 0;
		isCapture = false;
	}

	Move(int from, int to, int piece, int promotedPiece = 0, bool enpassant = false,
		 bool isCapture = false, bool isDoublePawnPush = false, bool isCastle = false)
	{
		this->from = from;
		this->to = to;
		this->piece = piece;
		this->isCapture = isCapture;
	}
};

uint64_t board[12];
uint64_t occupied[3];

bool whiteToMove = true;

int castling = 0;
int enpassant = noSqr;

vector<Move> moveList;
vector<Move> moveHistory;

Move bestMove;
int positions = 0;

uint64_t pawnAttacks[2][64];
uint64_t knightAttacks[64];
uint64_t kingAttacks[64];
uint64_t bishopMasks[64];
uint64_t rookMasks[64];
uint64_t queenAttacks[64];
uint64_t rookAttacks[64][4096];
uint64_t bishopAttacks[64][512];

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
	}
}

void initSliderAttacks(bool isBishop)
{
	for (int sqr = 0; sqr < 64; sqr++)
	{
		bishopMasks[sqr] = maskBishopAttacks(sqr);
		rookMasks[sqr] = maskRookAttacks(sqr);

		uint64_t attackMask = isBishop ? bishopMasks[sqr] : rookMasks[sqr];

		int relevant_bits_count = countSetBits(attackMask);

		int occupancy_indicies = (1 << relevant_bits_count);

		for (int index = 0; index < occupancy_indicies; index++)
		{
			if (isBishop)
			{
				uint64_t occupancy = setOccupancy(index, relevant_bits_count, attackMask);

				int magic_index = (occupancy * bishopMagicNumbers[sqr]) >> (64 - bishopRelevantBits[sqr]);

				bishopAttacks[sqr][magic_index] = bishopAttacksOTF(sqr, occupancy);
			}
			else
			{
				uint64_t occupancy = setOccupancy(index, relevant_bits_count, attackMask);

				int magic_index = (occupancy * rookMagicNumbers[sqr]) >> (64 - rookRelevantBits[sqr]);

				rookAttacks[sqr][magic_index] = rookAttacksOTF(sqr, occupancy);
			}
		}
	}
}

void initAttackMasks()
{
	initLeaperAttacks();
	initSliderAttacks(true);
	initSliderAttacks(false);
}

void printBoard()
{
	for (int i = 0; i < 64; i++)
	{
		if (i % 8 == 0)
		{
			cout << endl;
			cout << 8 - (i / 8);
		}

		if (getBit(board[P], i)) cout << " P";
		else if (getBit(board[p], i)) cout << " p";
		else if (getBit(board[N], i)) cout << " N";
		else if (getBit(board[n], i)) cout << " n";
		else if (getBit(board[B], i)) cout << " B";
		else if (getBit(board[b], i)) cout << " b";
		else if (getBit(board[R], i)) cout << " R";
		else if (getBit(board[r], i)) cout << " r";
		else if (getBit(board[Q], i)) cout << " Q";
		else if (getBit(board[q], i)) cout << " q";
		else if (getBit(board[K], i)) cout << " K";
		else if (getBit(board[k], i)) cout << " k";
		else cout << " -";
	}

	cout << endl << "  a b c d e f g h" << endl;

	cout << endl << "Side: " << (whiteToMove ? "white" : "black") << endl;

	cout << "Enpassant: " << ((enpassant != noSqr) ? squareToCoords[enpassant] : "no") << endl;

	cout << "Castling: " << ((castling & wk) ? 'K' : '-') << ((castling & wq) ? 'Q' : '-')
		 << ((castling & bk) ? 'k' : '-') << ((castling & bq) ? 'q' : '-') << endl;

	cout << endl << "Bitboard: " << (uint64_t)occupied[both] << "ULL" << endl << endl;
}

static inline uint64_t getRookMoves(Square sqr, uint64_t occupancy)
{
	occupancy &= rookMasks[sqr];
	occupancy *= rookMagicNumbers[sqr];
	occupancy >>= 64 - rookRelevantBits[sqr];

	return rookAttacks[sqr][occupancy];
}

static inline uint64_t getBishopMoves(Square sqr, uint64_t occupancy)
{
	occupancy &= bishopMasks[sqr];
	occupancy *= bishopMagicNumbers[sqr];
	occupancy >>= 64 - bishopRelevantBits[sqr];

	return bishopAttacks[sqr][occupancy];
}

static inline uint64_t getQueenMoves(Square sqr, uint64_t occupancy)
{
	return getBishopMoves(sqr, occupancy) | getRookMoves(sqr, occupancy);
}

bool isSquareAttacked(Square square, bool whiteToMove)
{
	if (whiteToMove && (pawnAttacks[black][square] & board[P])) return true;
	if (!whiteToMove && (pawnAttacks[white][square] & board[p])) return true;
	if (knightAttacks[square] & (whiteToMove ? board[N] : board[n])) return true;
	if (getBishopMoves(square, occupied[both]) & (whiteToMove ? board[B] : board[b])) return true;
	if (getRookMoves(square, occupied[both]) & (whiteToMove ? board[R] : board[r])) return true;
	if (getQueenMoves(square, occupied[both]) & ((whiteToMove ? board[Q] : board[q]))) return true;
	if (kingAttacks[square] & (whiteToMove ? board[K] : board[k])) return true;

	return false;
}

// populates the moveList
void generateMoves()
{
	int source, target;

	moveList.clear();

	uint64_t bitboard, attacks;

	for (int piece = P; piece <= k; piece++)
	{
		bitboard = board[piece];

		if (whiteToMove)
		{
			if (piece == P)
			{
				while (bitboard)
				{
					source = getFirstSetBit(bitboard);
					target = source - 8;

					// generate quiet pawn moves
					if (!(target < a8) && !getBit(occupied[both], target))
					{
						// pawn promotion
						if (source >= a7 && source <= h7)
						{
							moveList.push_back(Move(source, target, piece, Q));
							moveList.push_back(Move(source, target, piece, R));
							moveList.push_back(Move(source, target, piece, B));
							moveList.push_back(Move(source, target, piece, N));
						}

						else
						{
							// one square ahead pawn move
							moveList.push_back(Move(source, target, piece));

							// two squares ahead pawn move
							if ((source >= a2 && source <= h2) && !getBit(occupied[both], target - 8))
								moveList.push_back(Move(source, target - 8, piece, 0, 0, false, true));
						}
					}

					attacks = pawnAttacks[white][source] & occupied[black];

					// generate pawn captures
					while (attacks)
					{
						target = getFirstSetBit(attacks);

						// pawn promotion captures
						if (source >= a7 && source <= h7)
						{
							moveList.push_back(Move(source, target, piece, Q, 0, true));
							moveList.push_back(Move(source, target, piece, R, 0, true));
							moveList.push_back(Move(source, target, piece, B, 0, true));
							moveList.push_back(Move(source, target, piece, N, 0, true));
						}

						else
							// pawn captures
							moveList.push_back(Move(source, target, piece, 0, 0, true));

						// pop ls1b of the pawn attacks
						popBit(attacks, target);
					}

					// generate enpassant captures
					if (enpassant != noSqr)
					{
						uint64_t enpassant_attacks = pawnAttacks[(whiteToMove ? white : black)][source] & (1ULL << enpassant);

						// make sure enpassant capture available
						if (enpassant_attacks)
						{
							// init enpassant capture target square
							int targetEnpassant = getFirstSetBit(enpassant_attacks);
							moveList.push_back(Move(source, target, piece, 0, targetEnpassant, true));
						}
					}

					popBit(bitboard, source);
				}
			}
			else if (piece == K)
			{
				if (castling & wk)
				{
					if (!getBit(occupied[both], f1) && !getBit(occupied[both], g1))
					{
						if (!isSquareAttacked(e1, black) && !isSquareAttacked(f1, black))
							moveList.push_back(Move(e1, g1, piece, 0, 0, false, false, true));
					}
				}

				if (castling & wq)
				{
					if (!getBit(occupied[both], d1) && !getBit(occupied[both], c1) && !getBit(occupied[both], b1))
					{
						if (!isSquareAttacked(e1, black) && !isSquareAttacked(d1, black))
							moveList.push_back(Move(e1, c1, piece, 0, 0, false, false, true));
					}
				}
			}
		}
		else
		{
			if (piece == p)
			{
				while (bitboard)
				{
					source = getFirstSetBit(bitboard);
					target = source + 8;

					// generate quiet pawn moves
					if (!(target > h1) && !getBit(occupied[both], target))
					{
						// pawn promotion
						if (source >= a2 && source <= h2)
						{
							moveList.push_back(Move(source, target, piece, q));
							moveList.push_back(Move(source, target, piece, r));
							moveList.push_back(Move(source, target, piece, b));
							moveList.push_back(Move(source, target, piece, n));
						}

						else
						{
							// one square ahead pawn move
							moveList.push_back(Move(source, target, piece));

							// two squares ahead pawn move
							if ((source >= a7 && source <= h7) && !getBit(occupied[both], target + 8))
								moveList.push_back(Move(source, target + 8, piece, 0, 0, false, true));
						}
					}

					attacks = pawnAttacks[(whiteToMove ? white : black)][source] & occupied[white];

					// generate pawn captures
					while (attacks)
					{
						target = getFirstSetBit(attacks);

						// pawn promotion captures
						if (source >= a2 && source <= h2)
						{
							moveList.push_back(Move(source, target, piece, q, 0, true));
							moveList.push_back(Move(source, target, piece, r, 0, true));
							moveList.push_back(Move(source, target, piece, b, 0, true));
							moveList.push_back(Move(source, target, piece, n, 0, true));
						}
						else
							// pawn captures
							moveList.push_back(Move(source, target, piece, 0, 0, true));

						popBit(attacks, target);
					}

					// generate enpassant captures
					if (enpassant != noSqr)
					{
						uint64_t enpassant_attacks = pawnAttacks[(whiteToMove ? white : black)][source] & (1ULL << enpassant);

						// make sure enpassant capture available
						if (enpassant_attacks)
						{
							// init enpassant capture target square
							int targetEnpassant = getFirstSetBit(enpassant_attacks);
							moveList.push_back(Move(source, target, piece, 0, true, true));
						}
					}

					popBit(bitboard, source);
				}
			}
			else if (piece == k)
			{
				if (castling & bk)
				{
					if (!getBit(occupied[both], f8) && !getBit(occupied[both], g8))
					{
						if (!isSquareAttacked(e8, white) && !isSquareAttacked(f8, white))
							moveList.push_back(Move(e8, g8, piece, 0, 0, false, false, true));
					}
				}

				if (castling & bq)
				{
					if (!getBit(occupied[both], d8) && !getBit(occupied[both], c8) && !getBit(occupied[both], b8))
					{
						if (!isSquareAttacked(e8, white) && !isSquareAttacked(d8, white))
							moveList.push_back(Move(e8, c8, piece, 0, 0, false, false, true));
					}
				}
			}
		}

		// generate knight moves
		while (bitboard)
		{
			source = getFirstSetBit(bitboard);

			attacks = knightAttacks[source] & (whiteToMove ? ~occupied[white] : ~occupied[black]);

			while (attacks)
			{
				target = getFirstSetBit(attacks);

				moveList.push_back(Move(source, target, whiteToMove ? N : n));

				popBit(attacks, target);
			}

			popBit(bitboard, source);
		}


	}
}

void printMoveList()
{
	for (Move move : moveList)
		cout << squareToCoords[move.from] << squareToCoords[move.to] << endl;

	cout << endl;
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

	whiteToMove ^= 1;

	moveHistory.pop_back();
}

int evaluate()
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

	int score = 200 * (Ks - ks) + 9 * (Qs - qs) + 5 * (Rs - rs) + 3 * (Bs - bs + Ns - ns) + 1 * (Ps - ps);

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

int negamax(int depth, int alpha, int beta)
{
	if (depth == 0)
		return quiesce(alpha, beta);

	int score = 0;
	int bestScore = -10000;

	generateMoves();

	for (Move move : moveList)
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
	initBitboards();
	initAttackMasks();

	printBoard();

	generateMoves();
	printMoveList();

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
