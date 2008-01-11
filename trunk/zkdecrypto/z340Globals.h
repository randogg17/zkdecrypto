/////////////////////////////////////////////////////////////////// GLOBAL VARIABLES //////////////////////////////////////////////////////////////
#ifndef Z340_GLOBALS_H
#define Z340_GLOBALS_H

int		freqs[26]			=		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int		E_freqs[26]			=		{8167,1492,2782,4253,12702,2228,2015,6094,6966,153,772,4025,2406,\
									6749,7507,1929,95,5987,6327,9056,2758,978,2360,150,1974,74};		//ENGLISH
int		S_freqs[26]			=		{12529,1420,4679,5856,13676,694,1006,704,6249,443,4,4971,3150,\
									6712,8684,2505,875,6873,7980,4629,3934,895,23,221,895,523};			//SPANISH
int		G_freqs[26]			=		{6506,2566,2837,5414,16693,2044,3647,4064,7812,191,1879,2825,3005,\
									9905,2285,944,55,6539,6765,6742,3703,1069,1396,22,32,1002};			//GERMAN

int		lsocdata[26]		=		{0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1};				//LONGEST STRING OF CONSONANTS DATA

float unigraphs[UNI_SIZE];
/*{
8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153, 0.772, 4.025, 2.406,
6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056, 2.758, 0.978, 2.360, 0.150, 1.974, 0.0749
};*/

int bigraphs[BI_SIZE];
int	trigraphs[TRI_SIZE];
int	tetragraphs[TETRA_SIZE];
int	pentagraphs[PENTA_SIZE];

char	cipher[MAX_CIPH_LENGTH];
int		keylength;

#endif