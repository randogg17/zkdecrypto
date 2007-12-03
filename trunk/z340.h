#pragma once
//////////////////////////////////////////////////////////////////////// INCLUDES /////////////////////////////////////////////////////////////////

#include 		<stdio.h>
#include 		<string.h>
#include 		<stdlib.h>
#include 		<time.h>
#include		<math.h>

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(EVT_UpdatePlainText, -1)
DECLARE_EVENT_TYPE(EVT_UpdateBestKey, -1)
DECLARE_EVENT_TYPE(EVT_UpdateScore, -1)
END_DECLARE_EVENT_TYPES()



enum
{
	Cipher_Text = wxID_HIGHEST + 1, // declares an id which will be used to call our button
	Plain_Text,
	Start_Key,
	Best_Key,
	Start_Button,
	MENU_New,
	MENU_Open,
	MENU_Close,
	MENU_Save,
	MENU_SaveAs,
	MENU_Quit,
	lblCipher_Text,
	lblPlain_Text,
	lblStart_Key,
	lblBest_Key,
	lblScore_,
	Score_
};


//////////////////////////////////////////////////////////////////////// DEFINES //////////////////////////////////////////////////////////////////
#define		_DEB 0
#define 		VERSION 					"0.5"
#define 		ASCII_SIZE 					256
#define 		MAX_CIPH_LENGTH 				4096
#define		SCRAMBLESTARTKEY 				0				// 0 or 1 : Determines whether to pre-scramble the starting KEY
#define		SETSOLVED					for(x=0;x<cuniq;x++) { for(y=0;y<clength;y++) if(cipher[y]==uniqstr[x]) solved[y]=key[x]; }

/////////////////////////////////////////////////////////////////// GLOBAL VARIABLES //////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////// FUNCTIONS /////////////////////////////////////////////////////////////////

inline int		calcscore(const int,const char *);
inline int		calclsoc(const int,const char *);
inline void		shufflekey(char *);
int			readcipher(char *);
void			read_ngraphs(void);

void			printferror(char *);
void			printcipher(int,char *,char *);
void			printfrequency(int,int *,char *);
void			printvowels(int,char *);
int hillclimb(char*,char*,int,wxFrame*);