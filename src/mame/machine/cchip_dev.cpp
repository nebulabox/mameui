// license:BSD-3-Clause
// copyright-holders:Jonathan Gevaryahu, David Haywood

/*

TC0030CMD "C-chip" pinout: (from operation wolf schematics)
http://www.arcade-museum.com/manuals-videogames/O/Operation%20Wolf%20Schematics.pdf
http://www.jammarcade.net/partial-rainbow-islands-schematic/

Chip contains 4 dies:
(starting from pin 1 edge going toward pin 32 edge)
1 - uPD78C11 MCU w/ 4kx8 mask rom (assumed to be the same between games)
2 - uPD27C64 8kx8 EPROM (assumed to be different between games)
3 - uPD4464 8kx8 SRAM
4 - ASIC of some sort


                         +---------\_/---------+
                VCC -- - |  1               64 | - -- VCC
                PA0 <> m |  2               63 | m <- AN7 (tied to GND externally)
                PA1 <> m |  3               62 | m <- AN6
                PA2 <> m |  4               61 | m <- AN5
                PA3 <> m |  5               60 | m <- AN4
                PA4 <> m |  6               59 | m <- AN3
                PA5 <> m |  7               58 | m <- AN2 (tied to VCC externally)
                PA6 <> m |  8     T         57 | m <- AN1
                PA7 <> m |  9             | 56 | m <- AN0
                PB0 <> m | 10     C    ---| 55 | m <- MODE1 (MODE0 and AVSS are internally tied to GND) **
                PB1 <> m | 11             | 54 | m <- /INT1
                PB2 <> m | 12     0    --\  53 | m <- /NMI, used on rainbow islands
                PB3 <> m | 13           | > 52 | m <> PC7
                PB4 <> m | 14     0    --/  51 | m <> PC6
                PB5 <> m | 15               50 | m <> PC5
                PB6 <> m | 16     3    ---- 49 | m <> PC4
                PB7 <> m | 17               48 | m <> PC3
                PC0 <> m | 18     0       | 47 | m <> PC2
  (tied to VCC) VPP -> e | 19          ---| 46 | m <> PC1
                CLK -> a | 20     C       | 45 | a <- A10
      *ASIC_MODESEL ?> a | 21          /--\ 44 | a <- A9
             /RESET -> a | 22     M    |  | 43 | a <- A8
                 D0 <> a | 23          \--/ 42 | a <- A7
                 D1 <> a | 24     D         41 | a <- A6
                 D2 <> a | 25               40 | a <- A5
                 D3 <> a | 26               39 | a <- A4
                 D4 <> a | 27               38 | a <- A3
                 D5 <> a | 28               37 | a <- A2
                 D6 <> a | 29               36 | a <- A1
                 D7 <> a | 30               35 | a <- A0
                /CS -> a | 31               34 | a -> DTACK out to 68k (/CDTA)
                GND -- - | 32               33 | a <- R/W
                         +---------------------+
a = to asic
e = to eprom
m = to mcu
- = power pins, common

CLK is supplied with a 12MHZ oscillator on operation wolf

*Pin 21 goes to the ASIC and may be a mode pin. (try holding it to gnd?) Normally tied to 5v thru a 1k resistor.

** MODE1 is normally tied high externally via a 330ohm resistor, but can be in either state.
   MODE0 on the c-chip is internally tied low.

(move below MODE notes to upd7811.cpp?)

The four Mode0/Mode1 combinations are:
	LOW/LOW		78c11 mem map is 4k external rom (i.e. the eprom inside the c-chip) from 0x0000-0x0FFF;
				the low 4 bits of port F are used, to provide the high 4 address bits.
				speculation: likely the eprom can be banked so the low or high half is visible here, 
				or possibly one fixed window and 3 variable windows, managed by the asic?
	LOW/HIGH	78c11 mem map boots to internal rom (mask rom inside the 78c11 inside the c-chip) from
				0x0000-0x0fff but the memory map is under full mcu control and can select any of the
				four modes (internal only, 4k external, 16k external, 64k external)
The following two modes are unusable on the c-chip:
	HIGH/LOW	78c11 mem map is 16k external rom from 0x0000-0x3FFF;
				the low 6 bits of port F are used, to provide the high 6 address bits.
	HIGH/HIGH	78c11 mem map is 64k external rom from 0x0000-0xFFFF;
				all 8 bits of port F are used to provide the high 8 address bits.
VPP is only used for programming the 27c64, do not tie it to 18v or you will probably overwrite the 27c64 with garbage.

(see http://www.cpcwiki.eu/index.php/UPD7810/uPD7811 )


C-chip EXTERNAL memory map (it acts as a device mapped to ram; dtack is asserted on /cs for a 68k):
0x000-0x3FF = RW ram window, a 1k window into the 8k byte sram inside the c-chip; the 'bank' visible is selected by 0x600 below
0x400 = unknown, no idea if /DTACK is asserted for R or W here
0x401 = RW 'test command/status register', writing a 0x02 here starts test mode, will return 0x01 set if ok/ready for command and 0x04 set if error; this register is very likely handled by the internal rom in the upd78c11 itself rather than the eprom, and probably tests the sram and the 78c11 internal ram, among other things.
Current guess: 0x401 is actually attached to the high 2 bits of the PF register; bit 0 is pf6 out, bit 1 is pf6 in (attached to pf6 thru a resistor?), bit 2 is pf7 out, bit 3 is pf7 in (attached to pf7 through a resistor). The 78c11 (I'm guessing) reads pf6 and pf7 once per int; if pf6-in is set it reruns the startup selftest, clears pf6-out, then re-sets it. if there is an error, it also sets pf7.
Alternate guess: pf4 selects between rom and ram but pf5,6,7 are all mapped to 0x401. a memory mapped register in upd78c11 space selects low vs high half of rom/ram access
0x402-0x5ff = unknown (may be mirror of 0x400 and 0x401?) no idea if /DTACK is asserted for R or W here
0x600 = ?W ram window bank select, selects one of 8 1k banks to be accessible at 0x000-0x3ff , only low 3 bits are valid on this register. not sure if readable.
0x601-0x7ff = unknown, no idea if /DTACK is asserted for R or W here

This chip *ALWAYS* has a bypass capacitor (ceramic, 104, 0.10 uF) soldered on top of the chip between pins 1 and 32 OR between 64 and 32.

*/

#include "emu.h"
#include "machine/cchip_dev.h"

// uncomment to run code from cchip mask rom
//#define CCHIP_TEST

DEFINE_DEVICE_TYPE(TAITO_CCHIP_DEV, taito_cchip_device, "cchip", "Taito TC0030CMD (C-Chip)")

taito_cchip_device::taito_cchip_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, TAITO_CCHIP_DEV, tag, owner, clock),
	m_upd7811(*this, "upd7811"),
	m_upd4464_bank(*this, "upd4464_bank"),
	m_upd4464(*this, "upd4464")
{
}

ROM_START( taito_cchip )
	ROM_REGION( 0x1000, "upd7811", 0 )
#ifdef CCHIP_TEST
	ROM_LOAD( "cchip_upd78c11.rom", 0x0000, 0x1000, CRC(11111111) SHA1(1111111111111111111111111111111111111111) )
#else
	ROM_LOAD( "cchip_upd78c11.rom", 0x0000, 0x1000, NO_DUMP )
#endif
ROM_END


static ADDRESS_MAP_START( cchip_ram_bank, AS_PROGRAM, 8, taito_cchip_device )
	AM_RANGE(0x0000, 0x1fff) AM_RAM AM_SHARE("upd4464") // upd4464
ADDRESS_MAP_END

READ8_MEMBER(taito_cchip_device::asic_r)
{
	logerror("%s: asic_r %04x\n", machine().describe_context(), offset);

	if (offset == 0x001) // megablst
		return 0x01;

	return 0x00;
}

WRITE8_MEMBER(taito_cchip_device::asic_w)
{
	logerror("%s: asic_w %04x %02x\n", machine().describe_context(), offset, data);
	if (offset == 0x200)
	{
		logerror("cchip set bank to %02x\n", data & 0x7);
		m_upd4464_bank->set_bank(data & 0x7);
	}
}

READ8_MEMBER(taito_cchip_device::mem_r)
{
	offset &= 0x3ff;
	return m_upd4464_bank->read8(space,offset);
}

WRITE8_MEMBER(taito_cchip_device::mem_w)
{
	offset &= 0x3ff;
	return m_upd4464_bank->write8(space,offset,data);
}

static ADDRESS_MAP_START( cchip_map, AS_PROGRAM, 8, taito_cchip_device )
	//AM_RANGE(0x0000, 0x0fff) AM_ROM // internal ROM of uPD7811
	AM_RANGE(0x1000, 0x13ff) AM_DEVICE("upd4464_bank", address_map_bank_device, amap8)
	AM_RANGE(0x1400, 0x17ff) AM_READWRITE(asic_r, asic_w)
	AM_RANGE(0x2000, 0x3fff) AM_ROM AM_REGION("cchip_eprom", 0) // might not map here
ADDRESS_MAP_END

MACHINE_CONFIG_MEMBER( taito_cchip_device::device_add_mconfig )
	MCFG_CPU_ADD("upd7811", UPD7811, DERIVED_CLOCK(1,1))
	MCFG_CPU_PROGRAM_MAP(cchip_map)
#ifndef CCHIP_TEST
	MCFG_DEVICE_DISABLE() 
#endif

	MCFG_DEVICE_ADD("upd4464_bank", ADDRESS_MAP_BANK, 0)
	MCFG_DEVICE_PROGRAM_MAP(cchip_ram_bank)
	MCFG_ADDRESS_MAP_BANK_ENDIANNESS(ENDIANNESS_LITTLE)
	MCFG_ADDRESS_MAP_BANK_DATABUS_WIDTH(8)
	MCFG_ADDRESS_MAP_BANK_ADDRBUS_WIDTH(13)
	MCFG_ADDRESS_MAP_BANK_STRIDE(0x400)
MACHINE_CONFIG_END


void taito_cchip_device::device_start()
{
	m_upd4464_bank->set_bank(0);
}

void taito_cchip_device::device_reset()
{
}

const tiny_rom_entry *taito_cchip_device::device_rom_region() const
{
	return ROM_NAME(taito_cchip);
}
