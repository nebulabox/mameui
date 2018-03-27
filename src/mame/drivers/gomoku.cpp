// license:BSD-3-Clause
// copyright-holders:Takahiro Nogi, David Haywood
/******************************************************************************

    Gomoku Narabe Renju
    (c)1981 Nihon Bussan Co.,Ltd.

    Driver by Takahiro Nogi <nogi@kt.rim.or.jp> 1999/11/06 -
    Updated to compile again by David Haywood 19th Oct 2002

******************************************************************************/
/******************************************************************************

todo:

- Refactor sound emulation.

- BG(Go table) is generated by board circuitry, so not fully emulated.

- Couldn't figure out the method to specify palette, so I modified palette number manually.

- Couldn't figure out oneshot sound playback parameter. so I adjusted it manually.

******************************************************************************/

#include "emu.h"
#include "includes/gomoku.h"
#include "audio/gomoku.h"

#include "cpu/z80/z80.h"
#include "machine/74259.h"
#include "speaker.h"


/* input ports are rotated 90 degrees */
READ8_MEMBER(gomoku_state::input_port_r)
{
	int i, res;

	res = 0;
	for (i = 0; i < 8; i++)
		res |= ((m_inputs[i].read_safe(0xff) >> offset) & 1) << i;

	return res;
}


void gomoku_state::gomoku_map(address_map &map)
{
	map(0x0000, 0x47ff).rom();
	map(0x4800, 0x4fff).ram();
	map(0x5000, 0x53ff).ram().w(this, FUNC(gomoku_state::gomoku_videoram_w)).share("videoram");
	map(0x5400, 0x57ff).ram().w(this, FUNC(gomoku_state::gomoku_colorram_w)).share("colorram");
	map(0x5800, 0x58ff).ram().w(this, FUNC(gomoku_state::gomoku_bgram_w)).share("bgram");
	map(0x6000, 0x601f).w("gomoku", FUNC(gomoku_sound_device::sound1_w));
	map(0x6800, 0x681f).w("gomoku", FUNC(gomoku_sound_device::sound2_w));
	map(0x7000, 0x7007).w("latch", FUNC(ls259_device::write_d1));
	map(0x7800, 0x7807).r(this, FUNC(gomoku_state::input_port_r));
	map(0x7800, 0x7800).nopw();
}


static INPUT_PORTS_START( gomoku )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_COCKTAIL

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_DIPNAME (0x10, 0x10, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN3 ) /* service coin */
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN2 )

	PORT_START("DSW")
	PORT_SERVICE( 0x01, IP_ACTIVE_HIGH )  PORT_DIPLOCATION("SW1:1")
	PORT_DIPNAME( 0x06, 0x00, DEF_STR( Lives ))  PORT_DIPLOCATION("SW1:2,3")
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x06, "5" )
	PORT_DIPNAME( 0x08, 0x00, "Time" )  PORT_DIPLOCATION("SW1:4")
	PORT_DIPSETTING(    0x00, "60" )
	PORT_DIPSETTING(    0x08, "80" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) )  PORT_DIPLOCATION("SW1:5,6")
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) )  PORT_DIPLOCATION("SW1:7,8")
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
INPUT_PORTS_END


static const gfx_layout charlayout =
{
	8, 8,       /* 8*8 characters */
	256,        /* 256 characters */
	2,          /* 2 bits per pixel */
	{ 0, 4 },   /* the two bitplanes are packed in one byte */
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8        /* every char takes 16 consecutive bytes */
};

static GFXDECODE_START( gomoku )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout, 0, 32 )
GFXDECODE_END


MACHINE_CONFIG_START(gomoku_state::gomoku)
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80, XTAL(18'432'000)/12)      /* 1.536 MHz ? */
	MCFG_CPU_PROGRAM_MAP(gomoku_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", gomoku_state,  irq0_line_hold)

	MCFG_DEVICE_ADD("latch", LS259, 0) // 7J
	MCFG_ADDRESSABLE_LATCH_Q1_OUT_CB(WRITELINE(gomoku_state, flipscreen_w))
	MCFG_ADDRESSABLE_LATCH_Q2_OUT_CB(WRITELINE(gomoku_state, bg_dispsw_w))
	MCFG_ADDRESSABLE_LATCH_Q7_OUT_CB(NOOP) // start LED?

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(256, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 256-1, 16, 256-16-1)
	MCFG_SCREEN_UPDATE_DRIVER(gomoku_state, screen_update_gomoku)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_GFXDECODE_ADD("gfxdecode", "palette", gomoku)
	MCFG_PALETTE_ADD("palette", 64)
	MCFG_PALETTE_INIT_OWNER(gomoku_state, gomoku)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_SOUND_ADD("gomoku", GOMOKU, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_CONFIG_END


ROM_START( gomoku )
	ROM_REGION( 0x10000, "maincpu", 0 ) // program
	ROM_LOAD( "rj_1.7a",    0x0000, 0x1000, CRC(ed20d539) SHA1(7cbbc678cbe5c85b914ca44f82bdbd452cf694a0) )
	ROM_LOAD( "rj_2.7c",    0x1000, 0x1000, CRC(26a28516) SHA1(53d5d134cd91020fa06e380d355deb1df6b9cb6e) )
	ROM_LOAD( "rj_3.7d",    0x2000, 0x1000, CRC(d05db072) SHA1(9697c932c6dcee6f8536c9f0b3c84a719a7d3dee) )
	ROM_LOAD( "rj_4.7f",    0x3000, 0x1000, CRC(6e3d1c18) SHA1(e2f7e4c0de3c78d1b8e686152458972f996b023a) )
	ROM_LOAD( "rj_5.4e",    0x4000, 0x0800, CRC(eaf541b4) SHA1(bc7e7ec1ba68f71ab9ac86f9ae77971ddb9ce3a4) )

	ROM_REGION( 0x1000, "gfx1", 0 ) // text char
	ROM_LOAD( "rj_6.4r",    0x0000, 0x1000, CRC(ed26ae36) SHA1(61cb73d7f2568e88e1c2981e7af3e9a3b26797d3) )

	ROM_REGION( 0x1000, "gomoku", 0 )   // sound
	ROM_LOAD( "rj_7.3c",    0x0000, 0x1000, CRC(d1ed1365) SHA1(4ef08f26fe7df4c400f72e09e56d8825d584f55f) )

	ROM_REGION( 0x0040, "proms", 0 )
	ROM_LOAD( "rj_prom.1m", 0x0000, 0x0020, CRC(5da2f2bd) SHA1(4355ccf06cb09ec3240dc92bda19b1f707a010ef) )  // TEXT color
	ROM_LOAD( "rj_prom.1l", 0x0020, 0x0020, CRC(fe4ef393) SHA1(d4c63f8645afeadd13ff82087bcc497d8936d90b) )  // BG color

	ROM_REGION( 0x0100, "user1", 0 )    // BG draw data X
	ROM_LOAD( "rj_prom.8n", 0x0000, 0x0100, CRC(9ba43222) SHA1(a443df49d7ee9dbfd258b09731d392bf1249cbfa) )

	ROM_REGION( 0x0100, "user2", 0 )    // BG draw data Y
	ROM_LOAD( "rj_prom.7p", 0x0000, 0x0100, CRC(5b5464f8) SHA1(b945efb8a7233f501d67f6b1be4e9d4967dc6719) )

	ROM_REGION( 0x0100, "user3", 0 )    // BG character data
	ROM_LOAD( "rj_prom.7r", 0x0000, 0x0100, CRC(3004585a) SHA1(711b68140827f0f3dc71f2576fcf9b905c999e8d) )

	ROM_REGION( 0x0020, "user4", 0 )    // unknown
	ROM_LOAD( "rj_prom.9k", 0x0000, 0x0020, CRC(cff72923) SHA1(4f61375028ab62da46ed119bc81052f5f98c28d4) )
ROM_END


//    YEAR,   NAME,   PARENT,  MACHINE,  INPUT,  STATE         INIT,   MONITOR, COMPANY,      FULLNAME,              FLAGS
GAME( 1981,   gomoku, 0,       gomoku,   gomoku, gomoku_state, 0,      ROT90,   "Nichibutsu", "Gomoku Narabe Renju", 0 )
