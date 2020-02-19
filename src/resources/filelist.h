/****************************************************************************
 * Loadiine resource files.
 * This file is generated automatically.
 * Includes 2 files.
 *
 * NOTE:
 * Any manual modification of this file will be overwriten by the generation.
 ****************************************************************************/
#ifndef _FILELIST_H_
#define _FILELIST_H_

typedef struct _RecourceFile
{
	const char          *filename;
	const unsigned char *DefaultFile;
	const unsigned int  &DefaultFileSize;
	unsigned char	    *CustomFile;
	unsigned int        CustomFileSize;
} RecourceFile;

extern const unsigned char bgMusic_ogg[];
extern const unsigned int bgMusic_ogg_size;

extern const unsigned char font_ttf[];
extern const unsigned int font_ttf_size;

static RecourceFile RecourceList[] =
{
	{"bgMusic.ogg", bgMusic_ogg, bgMusic_ogg_size, NULL, 0},
	{"font.ttf", font_ttf, font_ttf_size, NULL, 0},
	{NULL, NULL, 0, NULL, 0}
};

#endif
