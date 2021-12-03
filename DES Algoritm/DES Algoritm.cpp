#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

typedef unsigned long long u64;

u64 DES(u64 plaintext, u64 key, bool encrypt);
u64 string_to_long(string s);
u64 intial_permutation(u64 plain);
u64 permutation_choice1(u64 key);
u64 shift_left(u64 text, int round);
u64 permutation_choice2(u64 left, u64 right);
u64 expansion_permutation(u64 r_plain);
u64 substitution_choice(u64 s);
u64 permutation(u64 r_plain);
u64 inverse_intial_permutation(u64 plain);

enum { DECRYPT, ENCRYPT };

int main() {
	bool encrypt;
	string plaintext_s, key_s;
	//Fitching the data
	cout << "Enter the massage: ";
	cin >> plaintext_s;
	cout << "Enter the key: ";
	cin >> key_s;
	cout << "Enter 1 to encrypt and 0 to decrypt: ";
	cin >> encrypt;

	//Converting string to unsigned long long
	u64 plaintext = string_to_long(plaintext_s), key = string_to_long(key_s);

	// Deciding to encrypt or decrypt and calling the DES fun
	if (encrypt) printf("cipher: %016llX\n", DES(plaintext, key, ENCRYPT));
	else printf("plain: %016llX\n", DES(plaintext, key, DECRYPT));

	system("Pause");
	return 0;
}


u64 DES(u64 plaintext, u64 key, bool encrypt) {
	//Intial Permutation for the Plain text
	plaintext = intial_permutation(plaintext);

	//split the plaintext into left and right
	u64 plain_left = plaintext >> 32;
	u64 plain_right = plaintext << 32;
	plain_right = plain_right >> 32;

	//Choose 58 bits of the key
	key = permutation_choice1(key);

	//split the plaintext into left and right
	u64 key_left = key >> 28;
	u64 key_right = key << 36;
	key_right = key_right >> 36;

	vector<u64> key_round;
	for (int i = 0; i < 16; ++i) {
		//Lift shift the key
		key_left = shift_left(key_left, i);
		key_right = shift_left(key_right, i);
		//choose 48 bits of the key after the shifting
		u64 key_temp = permutation_choice2(key_left, key_right);
		key_round.push_back(key_temp);
	}
	if (!encrypt) {
		vector<u64> temp = key_round;
		for (int i = 15, j = 0; i >= 0; i--, j++) {
			key_round[j] = temp[i];
		}
	}
	//DES 16 round process
	for (int i = 0; i < 16; ++i) {


		//Putting the right plain in the left without changing it
		u64 temp_left = plain_left;
		plain_left = plain_right;

		//Expansion and Permutation for R plain
		plain_right = expansion_permutation(plain_right);

		//Function (R plain xor with key round)
		plain_right = plain_right ^ key_round[i];

		//S-Box
		plain_right = substitution_choice(plain_right);

		//Permutation
		plain_right = permutation(plain_right);

		//R plain XOR L plain
		plain_right = plain_right ^ temp_left;
	}
	//Swapping and combine
	plaintext = plain_right << 32;
	plaintext += plain_left;

	//Inverse Intial Permutation
	plaintext = inverse_intial_permutation(plaintext);

	return plaintext;
}

u64 string_to_long(string s) {
	u64 value;
	istringstream iss(s);
	iss >> hex >> value;
	return value;
}

u64 intial_permutation(u64 plain) {
	u64 result = 0;
	int map[64] = { 58, 50, 42, 34, 26, 18, 10, 2,
		60, 52, 44, 36, 28, 20, 12, 4,
		62, 54, 46, 38, 30, 22, 14, 6,
		64, 56, 48, 40, 32, 24, 16, 8,
		57, 49, 41, 33, 25, 17, 9, 1,
		59, 51, 43, 35, 27, 19, 11, 3,
		61, 53, 45, 37, 29, 21, 13, 5,
		63, 55, 47, 39, 31, 23, 15, 7 };
	for (int i = 0; i < 64; ++i) {
		result |= (plain >> (64 - map[64 - 1 - i]) & 1) << i;
	}
	return result;
}

u64 permutation_choice1(u64 key) {
	u64 result = 0;
	int map[56] = { 57, 49, 41, 33, 25, 17, 9,
		1, 58, 50, 42, 34, 26, 18,
		10, 2, 59, 51, 43, 35, 27,
		19, 11, 3, 60, 52, 44, 36,
		63, 55, 47, 39, 31, 23, 15,
		7, 62, 54, 46, 38, 30, 22,
		14, 6, 61, 53, 45, 37, 29,
		21, 13, 5, 28, 20, 12, 4 };
	for (int i = 0; i < 56; ++i) {
		result |= (key >> (64 - map[56 - 1 - i]) & 1) << i;
	}
	return result;
}

u64 shift_left(u64 text, int round) {
	int shift_round[16] = { 1, 1, 2, 2,
		2, 2, 2, 2,
		1, 2, 2, 2,
		2, 2, 2, 1 };
	for (int i = 0; i < shift_round[round]; ++i) {
		u64 temp = text >> 27;
		text = text << 37;
		text = text >> 36;
		text += temp;
	}
	return text;
}

u64 permutation_choice2(u64 left, u64 right) {
	u64 combine = left << 28;
	combine += right;
	u64 result = 0;
	int map[48] = { 14, 17, 11, 24, 1, 5,
		3, 28, 15, 6, 21, 10,
		23, 19, 12, 4, 26, 8,
		16, 7, 27, 20, 13, 2,
		41, 52, 31, 37, 47, 55,
		30, 40, 51, 45, 33, 48,
		44, 49, 39, 56, 34, 53,
		46, 42, 50, 36, 29, 32 };
	for (int i = 0; i < 48; ++i) {
		result |= (combine >> (56 - map[48 - 1 - i]) & 1) << i;
	}
	return result;
}

u64 expansion_permutation(u64 r_plain) {
	u64 result = 0;
	int map[48] = { 32, 1, 2, 3, 4, 5,
		4, 5, 6, 7, 8, 9,
		8, 9, 10, 11, 12, 13,
		12, 13, 14, 15, 16, 17,
		16, 17, 18, 19, 20, 21,
		20, 21, 22, 23, 24, 25,
		24, 25, 26, 27, 28, 29,
		28, 29, 30, 31, 32, 1 };

	for (int i = 0; i < 48; ++i) {
		result |= (r_plain >> (32 - map[48 - 1 - i]) & 1) << i;
	}
	return result;
}

u64 substitution_choice(u64 s) {
	u64 result = 0;
	int s_box[8][64] = {
		{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
		0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
		4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
		15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 },

	{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
	3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
	0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
	13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 },

	{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
	13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
	13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
	1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 },

	{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
	13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
	10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
	3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 },

	{ 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
	14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
	4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
	11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 },

	{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
	10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
	9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
	4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 },

	{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
	13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
	1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
	6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 },

	{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
	1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
	7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
	2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }
	};

	for (int i = 0; i < 8; i++) {
		int idx = s >> (7 - i) * 6 & 0x3F;
		idx = idx >> 1 & 15 | (idx & 1) << 4 | idx & 0x20;
		result |= s_box[i][idx] << (7 - i) * 4;
	}
	return result;
}

u64 permutation(u64 r_plain) {
	u64 result = 0;
	int map[32] = { 16, 7, 20, 21,
		29, 12, 28, 17,
		1, 15, 23, 26,
		5, 18, 31, 10,
		2, 8, 24, 14,
		32, 27, 3, 9,
		19, 13, 30, 6,
		22, 11, 4, 25 };
	for (int i = 0; i < 32; ++i) {
		result |= (r_plain >> (32 - map[32 - 1 - i]) & 1) << i;
	}
	return result;
}

u64 inverse_intial_permutation(u64 plain) {
	u64 result = 0;
	int map[64] = { 40, 8, 48, 16, 56, 24, 64, 32,
		39, 7, 47, 15, 55, 23, 63, 31,
		38, 6, 46, 14, 54, 22, 62, 30,
		37, 5, 45, 13, 53, 21, 61, 29,
		36, 4, 44, 12, 52, 20, 60, 28,
		35, 3, 43, 11, 51, 19, 59, 27,
		34, 2, 42, 10, 50, 18, 58, 26,
		33, 1, 41, 9, 49, 17, 57, 25
	};

	for (int i = 0; i < 64; ++i) {
		result |= (plain >> (64 - map[64 - 1 - i]) & 1) << i;
	}
	return result;
}