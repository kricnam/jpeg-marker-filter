/*
 jfif_filter.c

 simple jfif format filter utility for filter out no standard marks

 based on jpegdump.c by
 Copyright (C) 2006 Ralph Giles. All rights reserved.

 Distributed under the terms of the GNU GPL.
 See http://www.gnu.org/ for details.
*/

#include <stdint.h>
#include <string.h>
#include <string>
#include "jfif_filter.h"


class JFIF_Marks {
protected:
	/* JPEG marker codes
	 - these are the second bytes; a marker consistes of 0xff
	 followed by the type codes given below. */
	typedef enum {

		/* 0xFF00, 0xFF01, 0xFFFE, 0xFFC0-0xFFDF(?) ITU T.81/IEC 10918-1 */
		/* 0xFFF0 - 0xFFF6 ITU T.84/IEC 10918-3 JPEG extensions */
		/* 0xFFF7 - 0xFFF8 ITU T.87/IEC 14495-1 JPEG LS */
		/* 0xFF4F - 0xFF6f, 0xFF90 - 0xFF93 JPEG 2000 IEC 15444-1 */
		/* 0xFF30 - 0xFF3F reserved JP2 (markers only--no marker segments */

		/* JPEG 1994 - defined in ITU T.81 | ISO IEC 10918-1 */
		SOF0 = 0xc0, /* start of frame - baseline jpeg */
		SOF1 = 0xc1, /* extended sequential, huffman */
		SOF2 = 0xc2, /* progressive, huffman */
		SOF3 = 0xc3, /* lossless, huffman */

		SOF5 = 0xc5, /* differential sequential, huffman */
		SOF6 = 0xc6, /* differential progressive, huffman */
		SOF7 = 0xc7, /* differential lossless, huffman */
		JPG = 0xc8, /* reserved for JPEG extension */
		SOF9 = 0xc9, /* extended sequential, arithmetic */
		SOF10 = 0xca, /* progressive, arithmetic */
		SOF11 = 0xcb, /* lossless, arithmetic */

		SOF13 = 0xcd, /* differential sequential, arithmetic */
		SOF14 = 0xce, /* differential progressive, arithmetic */
		SOF15 = 0xcf, /* differential lossless, arithmetic */

		DHT = 0xc4, /* define huffman tables */

		DAC = 0xcc, /* define arithmetic-coding conditioning */

		/* restart markers */
		RST0 = 0xd0,
		RST1 = 0xd1,
		RST2 = 0xd2,
		RST3 = 0xd3,
		RST4 = 0xd4,
		RST5 = 0xd5,
		RST6 = 0xd6,
		RST7 = 0xd7,
		/* delimeters */
		SOI = 0xd8, /* start of image */
		EOI = 0xd9, /* end of image */
		SOS = 0xda, /* start of scan */
		DQT = 0xdb, /* define quantization tables */
		DNL = 0xdc, /* define number of lines */
		DRI = 0xdd, /* define restart interval */
		DHP = 0xde, /* define hierarchical progression */
		EXP = 0xdf, /* expand reference components */

		/* JPEG 1997 extensions ITU T.84 | ISO IEC 10918-3 */
		/* application data sections */
		APP0 = 0xe0,
		APP1 = 0xe1,
		APP2 = 0xe2,
		APP3 = 0xe3,
		APP4 = 0xe4,
		APP5 = 0xe5,
		APP6 = 0xe6,
		APP7 = 0xe7,
		APP8 = 0xe8,
		APP9 = 0xe9,
		APP10 = 0xea,
		APP11 = 0xeb,
		APP12 = 0xec,
		APP13 = 0xed,
		APP14 = 0xee,
		APP15 = 0xef,
		/* extention data sections */
		JPG0 = 0xf0,
		JPG1 = 0xf1,
		JPG2 = 0xf2,
		JPG3 = 0xf3,
		JPG4 = 0xf4,
		JPG5 = 0xf5,
		JPG6 = 0xf6,
		SOF48 = 0xf7, /* JPEG-LS */
		LSE = 0xf8, /* JPEG-LS extension parameters */
		JPG9 = 0xf9,
		JPG10 = 0xfa,
		JPG11 = 0xfb,
		JPG12 = 0xfc,
		JPG13 = 0xfd,
		JCOM = 0xfe, /* comment */

		TEM = 0x01, /* temporary private use for arithmetic coding */

		/* 0x02 -> 0xbf reserved in JPEG 94/97*/

		/* JPEG 2000 - defined in IEC 15444-1 "JPEG 2000 Core (part 1)" */
		/* delimiters */
		SOC = 0x4f, /* start of codestream */
		SOT = 0x90, /* start of tile */
		SOD = 0x93, /* start of data */
		EOC = 0xd9, /* end of codestream */
		/* fixed information segment */
		SIZ = 0x51, /* image and tile size */
		/* functional segments */
		COD = 0x52, /* coding style default */
		COC = 0x53, /* coding style component */
		RGN = 0x5e, /* region of interest */
		QCD = 0x5c, /* quantization default */
		QCC = 0x5d, /* quantization component */
		POC = 0x5f, /* progression order change */
		/* pointer segments */
		TLM = 0x55, /* tile-part lengths */
		PLM = 0x57, /* packet length (main header) */
		PLT = 0x58, /* packet length (tile-part header) */
		PPM = 0x60, /* packed packet headers (main header) */
		PPT = 0x61, /* packet packet headers (tile-part header) */
		/* bitstream internal markers and segments */
		SOP = 0x91, /* start of packet */
		EPH = 0x92, /* end of packet header */
		/* informational segments */
		CRG = 0x63, /* component registration */
		COM = 0x64, /* comment */

	} marker;

	typedef struct {
		const char *mark; /* marker mnemonic string */
		const char *name; /* longer name */
		const char *spec; /* defining specification */
	} marker_info;

	marker_info markers[256];

	/* helper for structure initialization */
	marker_info init_marker(const char *mark, const char *name,
			const char *spec) {
		marker_info marker;

		marker.mark = mark;
		marker.name = name;
		marker.spec = spec;

		return marker;
	}
	;

	void init_markers(void) {
		memset(markers, 0, sizeof(markers));
		/* set defined marks */
		markers[0x00] = init_marker("nul", "reserved 00", "JPEG");
		markers[0x01] = init_marker("TEM", "reserved 01", "JPEG");

		/* JPEG 1994 - defined in ITU T.81 | ISO IEC 10918-1 */
		/* frame types */
		markers[0xc0] = init_marker("SOF0", "start of frame (baseline jpeg)",
				"JPEG 1994");
		markers[0xc1] = init_marker("SOF1",
				"start of frame (extended sequential, huffman)", "JPEG 1994");
		markers[0xc2] = init_marker("SOF2",
				"start of frame (progressive, huffman)", "JPEG 1994");
		markers[0xc3] = init_marker("SOF3",
				"start of frame (lossless, huffman)", "JPEG 1994");

		markers[0xc5] = init_marker("SOF5",
				"start of frame (differential sequential, huffman)",
				"JPEG 1994");
		markers[0xc6] = init_marker("SOF6",
				"start of frame (differential progressive, huffman)",
				"JPEG 1994");
		markers[0xc7] = init_marker("SOF7",
				"start of frame (differential lossless, huffman)", "JPEG 1994");
		markers[0xc8] = init_marker("JPG", "reserved for JPEG extension",
				"JPEG 1994");
		markers[0xc9] = init_marker("SOF9",
				"start of frame (extended sequential, arithmetic)",
				"JPEG 1994");
		markers[0xca] = init_marker("SOF10",
				"start of frame (progressive, arithmetic)", "JPEG 1994");
		markers[0xcb] = init_marker("SOF11",
				"start of frame (lossless, arithmetic)", "JPEG 1994");

		markers[0xcd] = init_marker("SOF13",
				"start of frame (differential sequential, arithmetic)",
				"JPEG 1994");
		markers[0xce] = init_marker("SOF14",
				"start of frame (differential progressive, arithmetic)",
				"JPEG 1994");
		markers[0xcf] = init_marker("SOF15",
				"start of frame (differential lossless, arithmetic)",
				"JPEG 1994");

		markers[0xc4] = init_marker("DHT", "define huffman tables",
				"JPEG 1994");
		markers[0xcc] = init_marker("DAC",
				"define arithmetic coding conditioning", "JPEG 1994");

		/* restart markers */
		markers[0xd0] = init_marker("RST0", "restart marker 0", "JPEG 1994");
		markers[0xd1] = init_marker("RST1", "restart marker 1", "JPEG 1994");
		markers[0xd2] = init_marker("RST2", "restart marker 2", "JPEG 1994");
		markers[0xd3] = init_marker("RST3", "restart marker 3", "JPEG 1994");
		markers[0xd4] = init_marker("RST4", "restart marker 4", "JPEG 1994");
		markers[0xd5] = init_marker("RST5", "restart marker 5", "JPEG 1994");
		markers[0xd6] = init_marker("RST6", "restart marker 6", "JPEG 1994");
		markers[0xd7] = init_marker("RST7", "restart marker 7", "JPEG 1994");
		/* delimeters */
		markers[0xd8] = init_marker("SOI", "start of image", "JPEG 1994");
		markers[0xd9] = init_marker("EOI", "end of image", "JPEG 1994");
		markers[0xda] = init_marker("SOS", "start of scan", "JPEG 1994");
		markers[0xdb] = init_marker("DQT", "define quantization tables",
				"JPEG 1994");
		markers[0xdc] = init_marker("DNL", "define number of lines",
				"JPEG 1994");
		markers[0xdd] = init_marker("DRI", "define restart interval",
				"JPEG 1994");
		markers[0xde] = init_marker("DHP", "define hierarchical progression",
				"JPEG 1994");
		markers[0xdf] = init_marker("EXP", "expand reference components",
				"JPEG 1994");

		/* JPEG 1997 extensions ITU T.84 | ISO IEC 10918-3 */
		/* application data sections */
		markers[0xe0] = init_marker("APP0", "application data section  0",
				"JPEG 1997");
		markers[0xe1] = init_marker("APP1", "application data section  1",
				"JPEG 1997");
		markers[0xe2] = init_marker("APP2", "application data section  2",
				"JPEG 1997");
		markers[0xe3] = init_marker("APP3", "application data section  3",
				"JPEG 1997");
		markers[0xe4] = init_marker("APP4", "application data section  4",
				"JPEG 1997");
		markers[0xe5] = init_marker("APP5", "application data section  5",
				"JPEG 1997");
		markers[0xe6] = init_marker("APP6", "application data section  6",
				"JPEG 1997");
		markers[0xe7] = init_marker("APP7", "application data section  7",
				"JPEG 1997");
		markers[0xe8] = init_marker("APP8", "application data section  8",
				"JPEG 1997");
		markers[0xe9] = init_marker("APP9", "application data section  9",
				"JPEG 1997");
		markers[0xea] = init_marker("APP10", "application data section 10",
				"JPEG 1997");
		markers[0xeb] = init_marker("APP11", "application data section 11",
				"JPEG 1997");
		markers[0xec] = init_marker("APP12", "application data section 12",
				"JPEG 1997");
		markers[0xed] = init_marker("APP13", "application data section 13",
				"JPEG 1997");
		markers[0xee] = init_marker("APP14", "application data section 14",
				"JPEG 1997");
		markers[0xef] = init_marker("APP15", "application data section 15",
				"JPEG 1997");
		/* extention data sections */
		markers[0xf0] = init_marker("JPG0", "extension data 00", "JPEG 1997");
		markers[0xf1] = init_marker("JPG1", "extension data 01", "JPEG 1997");
		markers[0xf2] = init_marker("JPG2", "extension data 02", "JPEG 1997");
		markers[0xf3] = init_marker("JPG3", "extension data 03", "JPEG 1997");
		markers[0xf4] = init_marker("JPG4", "extension data 04", "JPEG 1997");
		markers[0xf5] = init_marker("JPG5", "extension data 05", "JPEG 1997");
		markers[0xf6] = init_marker("JPG6", "extension data 06", "JPEG 1997");
		markers[0xf7] = init_marker("SOF48", "start of frame (JPEG-LS)",
				"JPEG-LS");
		markers[0xf8] = init_marker("LSE", "extension parameters (JPEG-LS)",
				"JPEG-LS");
		markers[0xf9] = init_marker("JPG9", "extension data 09", "JPEG 1997");
		markers[0xfa] = init_marker("JPG10", "extension data 10", "JPEG 1997");
		markers[0xfb] = init_marker("JPG11", "extension data 11", "JPEG 1997");
		markers[0xfc] = init_marker("JPG12", "extension data 12", "JPEG 1997");
		markers[0xfd] = init_marker("JPG13", "extension data 13", "JPEG 1997");
		markers[0xfe] = init_marker("JCOM", "extension data (comment)",
				"JPEG 1997");

		/* JPEG 2000 - defined in IEC 15444-1 "JPEG 2000 Core (part 1)" */
		/* delimiters */
		markers[0x4f] = init_marker("SOC", "start of codestream", "JPEG 2000");
		markers[0x90] = init_marker("SOT", "start of tile", "JPEG 2000");
		markers[0xd9] = init_marker("EOC", "end of codestream", "JPEG 2000");
		/* fixed information segment */
		markers[0x51] = init_marker("SIZ", "image and tile size", "JPEG 2000");
		/* functional segments */
		markers[0x52] = init_marker("COD", "coding style default", "JPEG 2000");
		markers[0x53] = init_marker("COC", "coding style component",
				"JPEG 2000");
		markers[0x5e] = init_marker("RGN", "region of interest", "JPEG 2000");
		markers[0x5c] = init_marker("QCD", "quantization default", "JPEG 2000");
		markers[0x5d] = init_marker("QCC", "quantization component",
				"JPEG 2000");
		markers[0x5f] = init_marker("POC", "progression order change",
				"JPEG 2000");
		/* pointer segments */
		markers[0x55] = init_marker("TLM", "tile-part lengths", "JPEG 2000");
		markers[0x57] = init_marker("PLM", "packet length (main header)",
				"JPEG 2000");
		markers[0x58] = init_marker("PLT", "packet length (tile-part header)",
				"JPEG 2000");
		markers[0x60] = init_marker("PPM",
				"packed packet headers (main header)", "JPEG 2000");
		markers[0x61] = init_marker("PPT",
				"packed packet headers (tile-part header)", "JPEG 2000");
		/* bitstream internal markers and segments */
		markers[0x91] = init_marker("SOP", "start of packet", "JPEG 2000");
		markers[0x92] = init_marker("EPH", "end of packet header", "JPEG 2000");
		/* informational segments */
		markers[0x63] = init_marker("CRG", "component registration",
				"JPEG 2000");
		markers[0x64] = init_marker("COM", "comment", "JPEG 2000");

	}
	;
public:
	JFIF_Marks(void) {
		init_markers();
	}
	;
	bool IsKnownMark(unsigned char index) {
		return (markers[index].mark != NULL);
	}
	;
};

static JFIF_Marks jfifMarks;

/* filter no standard marks from  a jpeg stream */
std::string filter_stream(const std::string &strIn) {

	std::string strOut;
	strOut.reserve(strIn.size());
	std::string::const_iterator it;
	bool fSkipMark = false;
	for (it = strIn.begin(); it != strIn.end(); ++it) {
		char c = *it;
		if (c == '\xFF') {
			++it;
			if (it != strIn.end()) {
				char code = *it;
				if (jfifMarks.IsKnownMark(code)) {
					fSkipMark = false;
					strOut.append(1, c);
					strOut.append(1, code);
				} else
					fSkipMark = true;
				continue;
			} else
				break;
		}
		if (!fSkipMark)
			strOut.append(1, c);
	}
	return strOut;
}

