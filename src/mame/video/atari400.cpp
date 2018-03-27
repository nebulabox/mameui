// license:GPL-2.0+
// copyright-holders:Juergen Buchmueller
/******************************************************************************
    Atari 400/800

    Video handler

    Juergen Buchmueller, June 1998
******************************************************************************/

#include "emu.h"
#include "includes/atari400.h"
#include "screen.h"

#define VERBOSE 0

#define LOG(x)  do { if (VERBOSE) logerror x; } while (0)


/************************************************************************
 * video_start
 * Initialize the ATARI800 video emulation
 ************************************************************************/

void atari_common_state::video_start()
{
	device_palette_interface &palette = m_screen->palette();

	for (int i = 0; i < 256; i++)
		m_gtia->set_color_lookup(i, (palette.pen(0) << 8) + palette.pen(0));
}


/**************************************************************
 *
 * Palette
 *
 **************************************************************/

static const uint8_t atari_palette[256*3] =
{
	/* Grey */
	0x00,0x00,0x00, 0x25,0x25,0x25, 0x34,0x34,0x34, 0x4e,0x4e,0x4e,
	0x68,0x68,0x68, 0x75,0x75,0x75, 0x8e,0x8e,0x8e, 0xa4,0xa4,0xa4,
	0xb8,0xb8,0xb8, 0xc5,0xc5,0xc5, 0xd0,0xd0,0xd0, 0xd7,0xd7,0xd7,
	0xe1,0xe1,0xe1, 0xea,0xea,0xea, 0xf4,0xf4,0xf4, 0xff,0xff,0xff,
	/* Gold */
	0x41,0x20,0x00, 0x54,0x28,0x00, 0x76,0x37,0x00, 0x9a,0x50,0x00,
	0xc3,0x68,0x06, 0xe4,0x7b,0x07, 0xff,0x91,0x1a, 0xff,0xab,0x1d,
	0xff,0xc5,0x1f, 0xff,0xd0,0x3b, 0xff,0xd8,0x4c, 0xff,0xe6,0x51,
	0xff,0xf4,0x56, 0xff,0xf9,0x70, 0xff,0xff,0x90, 0xff,0xff,0xaa,
	/* Orange */
	0x45,0x19,0x04, 0x72,0x1e,0x11, 0x9f,0x24,0x1e, 0xb3,0x3a,0x20,
	0xc8,0x51,0x20, 0xe3,0x69,0x20, 0xfc,0x81,0x20, 0xfd,0x8c,0x25,
	0xfe,0x98,0x2c, 0xff,0xae,0x38, 0xff,0xb9,0x46, 0xff,0xbf,0x51,
	0xff,0xc6,0x6d, 0xff,0xd5,0x87, 0xff,0xe4,0x98, 0xff,0xe6,0xab,
	/* Red-Orange */
	0x5d,0x1f,0x0c, 0x7a,0x24,0x0d, 0x98,0x2c,0x0e, 0xb0,0x2f,0x0f,
	0xbf,0x36,0x24, 0xd3,0x4e,0x2a, 0xe7,0x62,0x3e, 0xf3,0x6e,0x4a,
	0xfd,0x78,0x54, 0xff,0x8a,0x6a, 0xff,0x98,0x7c, 0xff,0xa4,0x8b,
	0xff,0xb3,0x9e, 0xff,0xc2,0xb2, 0xff,0xd0,0xc3, 0xff,0xda,0xd0,
	/* Pink */
	0x4a,0x17,0x00, 0x72,0x1f,0x00, 0xa8,0x13,0x00, 0xc8,0x21,0x0a,
	0xdf,0x25,0x12, 0xec,0x3b,0x24, 0xfa,0x52,0x36, 0xfc,0x61,0x48,
	0xff,0x70,0x5f, 0xff,0x7e,0x7e, 0xff,0x8f,0x8f, 0xff,0x9d,0x9e,
	0xff,0xab,0xad, 0xff,0xb9,0xbd, 0xff,0xc7,0xce, 0xff,0xca,0xde,
	/* Purple */
	0x49,0x00,0x36, 0x66,0x00,0x4b, 0x80,0x03,0x5f, 0x95,0x0f,0x74,
	0xaa,0x22,0x88, 0xba,0x3d,0x99, 0xca,0x4d,0xa9, 0xd7,0x5a,0xb6,
	0xe4,0x67,0xc3, 0xef,0x72,0xce, 0xfb,0x7e,0xda, 0xff,0x8d,0xe1,
	0xff,0x9d,0xe5, 0xff,0xa5,0xe7, 0xff,0xaf,0xea, 0xff,0xb8,0xec,
	/* Purple-Blue */
	0x48,0x03,0x6c, 0x5c,0x04,0x88, 0x65,0x0d,0x90, 0x7b,0x23,0xa7,
	0x93,0x3b,0xbf, 0x9d,0x45,0xc9, 0xa7,0x4f,0xd3, 0xb2,0x5a,0xde,
	0xbd,0x65,0xe9, 0xc5,0x6d,0xf1, 0xce,0x76,0xfa, 0xd5,0x83,0xff,
	0xda,0x90,0xff, 0xde,0x9c,0xff, 0xe2,0xa9,0xff, 0xe6,0xb6,0xff,
	/* Blue 1 */
	0x05,0x1e,0x81, 0x06,0x26,0xa5, 0x08,0x2f,0xca, 0x26,0x3d,0xd4,
	0x44,0x4c,0xde, 0x4f,0x5a,0xec, 0x5a,0x68,0xff, 0x65,0x75,0xff,
	0x71,0x83,0xff, 0x80,0x91,0xff, 0x90,0xa0,0xff, 0x97,0xa9,0xff,
	0x9f,0xb2,0xff, 0xaf,0xbe,0xff, 0xc0,0xcb,0xff, 0xcd,0xd3,0xff,
	/* Blue 2 */
	0x0b,0x07,0x79, 0x20,0x1c,0x8e, 0x35,0x31,0xa3, 0x46,0x42,0xb4,
	0x57,0x53,0xc5, 0x61,0x5d,0xcf, 0x6d,0x69,0xdb, 0x7b,0x77,0xe9,
	0x89,0x85,0xf7, 0x91,0x8d,0xff, 0x9c,0x98,0xff, 0xa7,0xa4,0xff,
	0xb2,0xaf,0xff, 0xbb,0xb8,0xff, 0xc3,0xc1,0xff, 0xd3,0xd1,0xff,
	/* Light-Blue */
	0x1d,0x29,0x5a, 0x1d,0x38,0x76, 0x1d,0x48,0x92, 0x1d,0x5c,0xac,
	0x1d,0x71,0xc6, 0x32,0x86,0xcf, 0x48,0x9b,0xd9, 0x4e,0xa8,0xec,
	0x55,0xb6,0xff, 0x69,0xca,0xff, 0x74,0xcb,0xff, 0x82,0xd3,0xff,
	0x8d,0xda,0xff, 0x9f,0xd4,0xff, 0xb4,0xe2,0xff, 0xc0,0xeb,0xff,
	/* Turquoise */
	0x00,0x4b,0x59, 0x00,0x5d,0x6e, 0x00,0x6f,0x84, 0x00,0x84,0x9c,
	0x00,0x99,0xbf, 0x00,0xab,0xca, 0x00,0xbc,0xde, 0x00,0xd0,0xf5,
	0x10,0xdc,0xff, 0x3e,0xe1,0xff, 0x64,0xe7,0xff, 0x76,0xea,0xff,
	0x8b,0xed,0xff, 0x9a,0xef,0xff, 0xb1,0xf3,0xff, 0xc7,0xf6,0xff,
	/* Green-Blue */
	0x00,0x48,0x00, 0x00,0x54,0x00, 0x03,0x6b,0x03, 0x0e,0x76,0x0e,
	0x18,0x80,0x18, 0x27,0x92,0x27, 0x36,0xa4,0x36, 0x4e,0xb9,0x4e,
	0x51,0xcd,0x51, 0x72,0xda,0x72, 0x7c,0xe4,0x7c, 0x85,0xed,0x85,
	0x99,0xf2,0x99, 0xb3,0xf7,0xb3, 0xc3,0xf9,0xc3, 0xcd,0xfc,0xcd,
	/* Green */
	0x16,0x40,0x00, 0x1c,0x53,0x00, 0x23,0x66,0x00, 0x28,0x78,0x00,
	0x2e,0x8c,0x00, 0x3a,0x98,0x0c, 0x47,0xa5,0x19, 0x51,0xaf,0x23,
	0x5c,0xba,0x2e, 0x71,0xcf,0x43, 0x85,0xe3,0x57, 0x8d,0xeb,0x5f,
	0x97,0xf5,0x69, 0xa0,0xfe,0x72, 0xb1,0xff,0x8a, 0xbc,0xff,0x9a,
	/* Yellow-Green */
	0x2c,0x35,0x00, 0x38,0x44,0x00, 0x44,0x52,0x00, 0x49,0x56,0x00,
	0x60,0x71,0x00, 0x6c,0x7f,0x00, 0x79,0x8d,0x0a, 0x8b,0x9f,0x1c,
	0x9e,0xb2,0x2f, 0xab,0xbf,0x3c, 0xb8,0xcc,0x49, 0xc2,0xd6,0x53,
	0xcd,0xe1,0x53, 0xdb,0xef,0x6c, 0xe8,0xfc,0x79, 0xf2,0xff,0xab,
	/* Orange-Green */
	0x46,0x3a,0x09, 0x4d,0x3f,0x09, 0x54,0x45,0x09, 0x6c,0x58,0x09,
	0x90,0x76,0x09, 0xab,0x8b,0x0a, 0xc1,0xa1,0x20, 0xd0,0xb0,0x2f,
	0xde,0xbe,0x3d, 0xe6,0xc6,0x45, 0xed,0xcd,0x4c, 0xf5,0xd8,0x62,
	0xfb,0xe2,0x76, 0xfc,0xee,0x98, 0xfd,0xf3,0xa9, 0xfd,0xf3,0xbe,
	/* Light-Orange */
	0x40,0x1a,0x02, 0x58,0x1f,0x05, 0x70,0x24,0x08, 0x8d,0x3a,0x13,
	0xab,0x51,0x1f, 0xb5,0x64,0x27, 0xbf,0x77,0x30, 0xd0,0x85,0x3a,
	0xe1,0x93,0x44, 0xed,0xa0,0x4e, 0xf9,0xad,0x58, 0xfc,0xb7,0x5c,
	0xff,0xc1,0x60, 0xff,0xca,0x69, 0xff,0xcf,0x7e, 0xff,0xda,0x96
};


/* Initialise the palette */
PALETTE_INIT_MEMBER(atari_common_state, atari)
{
	int i;

	for ( i = 0; i < sizeof(atari_palette) / 3; i++ )
	{
		palette.set_pen_color(i, atari_palette[i*3], atari_palette[i*3+1], atari_palette[i*3+2]);
	}
}
