#pragma once

#ifndef AES_H
#define AES_H

#include <stddef.h>

#define AES_BLOCK_SIZE 16               // AES operates on 16 bytes at a time

//typedef unsigned char BYTE;            // 8-bit byte
//typedef unsigned int DWORD;             // 32-bit word, change to "long" for 16-bit machines
									   // Key setup must be done before any AES en/de-cryption functions can be used.
void aes_key_setup(const BYTE key[],          // The key, must be 128, 192, or 256 bits
	DWORD w[],                  // Output key schedule to be used later
	int keysize);              // Bit length of the key, 128, 192, or 256

void aes_encrypt(const BYTE in[],             // 16 bytes of plaintext
	BYTE out[],                  // 16 bytes of ciphertext
	const DWORD key[],            // From the key setup
	int keysize);                // Bit length of the key, 128, 192, or 256

void aes_decrypt(const BYTE in[],             // 16 bytes of ciphertext
	BYTE out[],                  // 16 bytes of plaintext
	const DWORD key[],            // From the key setup
	int keysize);                // Bit length of the key, 128, 192, or 256


								 // AES - CBC
			
int aes_encrypt_cbc(const BYTE in[],          // Plaintext
	size_t in_len,            // Must be a multiple of AES_BLOCK_SIZE
	BYTE out[],               // Ciphertext, same length as plaintext
	const DWORD key[],         // From the key setup
	int keysize,              // Bit length of the key, 128, 192, or 256
	const BYTE iv[]);         // IV, must be AES_BLOCK_SIZE bytes long

							  // Only output the CBC-MAC of the input.
int aes_encrypt_cbc_mac(const BYTE in[],      // plaintext
	size_t in_len,        // Must be a multiple of AES_BLOCK_SIZE
	BYTE out[],           // Output MAC
	const DWORD key[],     // From the key setup
	int keysize,          // Bit length of the key, 128, 192, or 256
	const BYTE iv[]);     // IV, must be AES_BLOCK_SIZE bytes long


int aes_decrypt_cbc(const BYTE in[],      // plaintext
	size_t in_len,        // Must be a multiple of AES_BLOCK_SIZE
	BYTE out[],           // Output MAC
	const DWORD key[],     // From the key setup
	int keysize,          // Bit length of the key, 128, 192, or 256
	const BYTE iv[]);     // IV, must be AES_BLOCK_SIZE bytes long


#endif   // AES_H
