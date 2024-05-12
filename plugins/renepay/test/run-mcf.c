#include "config.h"

#define RENEPAY_UNITTEST // logs are written in /tmp/debug.txt
#include "../payment.c"
#include "../flow.c"
#include "../route.c"
#include "../uncertainty.c"
#include "../mcf.c"

#include <ccan/array_size/array_size.h>
#include <ccan/read_write_all/read_write_all.h>
#include <ccan/str/hex/hex.h>
#include <common/bigsize.h>
#include <common/channel_id.h>
#include <common/node_id.h>
#include <common/setup.h>
#include <common/wireaddr.h>
#include <inttypes.h>
#include <stdio.h>
#include <wire/onion_wiregen.h>

/* AUTOGENERATED MOCKS START */
/* Generated stub for disabledmap_add_channel */
void disabledmap_add_channel(struct disabledmap *p UNNEEDED,
			     struct short_channel_id scid UNNEEDED)
{ fprintf(stderr, "disabledmap_add_channel called!\n"); abort(); }
/* Generated stub for disabledmap_add_node */
void disabledmap_add_node(struct disabledmap *p UNNEEDED, struct node_id node UNNEEDED)
{ fprintf(stderr, "disabledmap_add_node called!\n"); abort(); }
/* Generated stub for disabledmap_channel_is_warned */
bool disabledmap_channel_is_warned(struct disabledmap *p UNNEEDED,
				   struct short_channel_id scid UNNEEDED)
{ fprintf(stderr, "disabledmap_channel_is_warned called!\n"); abort(); }
/* Generated stub for disabledmap_new */
struct disabledmap *disabledmap_new(const tal_t *ctx UNNEEDED)
{ fprintf(stderr, "disabledmap_new called!\n"); abort(); }
/* Generated stub for disabledmap_reset */
void disabledmap_reset(struct disabledmap *p UNNEEDED)
{ fprintf(stderr, "disabledmap_reset called!\n"); abort(); }
/* Generated stub for disabledmap_warn_channel */
void disabledmap_warn_channel(struct disabledmap *p UNNEEDED,
			      struct short_channel_id scid UNNEEDED)
{ fprintf(stderr, "disabledmap_warn_channel called!\n"); abort(); }
/* Generated stub for json_add_payment */
void json_add_payment(struct json_stream *s UNNEEDED, const struct payment *payment UNNEEDED)
{ fprintf(stderr, "json_add_payment called!\n"); abort(); }
/* Generated stub for new_routetracker */
struct routetracker *new_routetracker(const tal_t *ctx UNNEEDED, struct payment *payment UNNEEDED)
{ fprintf(stderr, "new_routetracker called!\n"); abort(); }
/* Generated stub for pay_plugin */
struct pay_plugin *pay_plugin;
/* Generated stub for routetracker_cleanup */
void routetracker_cleanup(struct routetracker *routetracker UNNEEDED)
{ fprintf(stderr, "routetracker_cleanup called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

static void swap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

/* Canned gossmap, taken from tests/test_gossip.py, with channels mined.
 * $> od -tx1 -Anone -v < /tmp/ltests-rtchpzh1/test_gossip_store_compact_noappend_1/lightning-2/regtest/gossip_store  | sed 's/ / 0x/g'| cut -c2- | sed -e 's/ /,/g' -e 's/$/,/'
 */
static u8 canned_map[] = {
	0x0c,0x80,0x00,0x01,0xbc,0x09,0x8b,0x67,0xe6,0x00,0x00,0x00,0x00,0x10,0x08,0x00,
	0x00,0x00,0x00,0x00,0x0f,0x42,0x40,0x01,0xb0,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x22,0x6e,
	0x46,0x11,0x1a,0x0b,0x59,0xca,0xaf,0x12,0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,
	0x3a,0x5e,0x33,0x2a,0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x67,
	0x00,0x00,0x01,0x00,0x00,0x02,0x2d,0x22,0x36,0x20,0xa3,0x59,0xa4,0x7f,0xf7,0xf7,
	0xac,0x44,0x7c,0x85,0xc4,0x6c,0x92,0x3d,0xa5,0x33,0x89,0x22,0x1a,0x00,0x54,0xc1,
	0x1c,0x1e,0x3c,0xa3,0x1d,0x59,0x03,0x5d,0x2b,0x11,0x92,0xdf,0xba,0x13,0x4e,0x10,
	0xe5,0x40,0x87,0x5d,0x36,0x6e,0xbc,0x8b,0xc3,0x53,0xd5,0xaa,0x76,0x6b,0x80,0xc0,
	0x90,0xb3,0x9c,0x3a,0x5d,0x88,0x5d,0x03,0x1b,0x84,0xc5,0x56,0x7b,0x12,0x64,0x40,
	0x99,0x5d,0x3e,0xd5,0xaa,0xba,0x05,0x65,0xd7,0x1e,0x18,0x34,0x60,0x48,0x19,0xff,
	0x9c,0x17,0xf5,0xe9,0xd5,0xdd,0x07,0x8f,0x03,0x1b,0x84,0xc5,0x56,0x7b,0x12,0x64,
	0x40,0x99,0x5d,0x3e,0xd5,0xaa,0xba,0x05,0x65,0xd7,0x1e,0x18,0x34,0x60,0x48,0x19,
	0xff,0x9c,0x17,0xf5,0xe9,0xd5,0xdd,0x07,0x8f,0x00,0x00,0x01,0xb0,0x6e,0x30,0x94,
	0x60,0x65,0x55,0xb3,0x60,0x01,0x00,0x00,0x33,0x57,0x33,0xf5,0x94,0x2d,0xf5,0xd9,
	0x50,0xeb,0x87,0x66,0xde,0xe3,0xa9,0xd6,0x62,0x69,0x22,0x84,0x4e,0xd6,0xae,0x7a,
	0x11,0x0d,0xd7,0xe7,0xed,0xc3,0x2e,0x3f,0x6f,0x3d,0x9a,0xc5,0xcd,0xea,0x23,0xce,
	0x25,0xbb,0x8d,0xbf,0x76,0x1f,0xd3,0xd5,0xfc,0x56,0xc0,0x5b,0x68,0x56,0x31,0x6d,
	0x12,0xe9,0xd3,0x2c,0xa0,0xf0,0x8c,0x69,0xca,0x03,0x06,0xfe,0x71,0x6e,0x7b,0x51,
	0x51,0x31,0x7d,0x64,0x40,0xb7,0x37,0x3d,0x9f,0xbc,0x64,0x6e,0xad,0x48,0xf2,0x16,
	0x3f,0x2b,0x6d,0x51,0x1a,0xfe,0x6a,0x79,0xc7,0x55,0x51,0xc2,0x62,0x0f,0xc8,0x09,
	0x74,0xf2,0xf8,0x64,0x32,0x9d,0x97,0x78,0xa0,0x8c,0xdf,0xbc,0x9f,0x2c,0x9c,0x13,
	0x44,0xc4,0x32,0x70,0x2c,0x66,0x80,0x7c,0xfb,0x4d,0xb6,0x9b,0x80,0xfa,0xe8,0xc3,
	0x3c,0x70,0x14,0x3d,0x94,0x8b,0x36,0x61,0x4d,0x62,0x08,0x91,0xbe,0xe2,0xdf,0x99,
	0xc8,0x6b,0xc6,0x22,0x07,0xc1,0x7e,0x3b,0x91,0x86,0x21,0x4c,0x0c,0xcf,0xf2,0xde,
	0xd5,0x59,0x8a,0xcc,0xc9,0x0e,0xb1,0xd5,0xb2,0xf7,0xa8,0x3c,0xd7,0xf6,0x8d,0x71,
	0x2e,0xa0,0x47,0xd8,0x01,0x9f,0x34,0x30,0x63,0xb0,0xa2,0x36,0x35,0x6a,0x38,0x71,
	0x46,0xf5,0x8f,0xa8,0x32,0xdd,0xc1,0x3c,0x47,0x14,0x52,0x2c,0xbb,0x50,0x3f,0x5f,
	0x3c,0xa8,0xfc,0xec,0x66,0x02,0xbe,0x24,0x38,0xad,0x3f,0x98,0xfa,0x0c,0xee,0xd5,
	0x8f,0xe3,0xa0,0x66,0xd3,0x85,0xfc,0xac,0xd9,0x8c,0x70,0x4b,0x2a,0x8e,0x98,0xa3,
	0xe2,0x0b,0xf7,0x6b,0x35,0xb7,0x36,0x00,0x00,0x06,0x22,0x6e,0x46,0x11,0x1a,0x0b,
	0x59,0xca,0xaf,0x12,0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,0x33,0x2a,
	0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x67,0x00,0x00,0x01,0x00,
	0x00,0x02,0x2d,0x22,0x36,0x20,0xa3,0x59,0xa4,0x7f,0xf7,0xf7,0xac,0x44,0x7c,0x85,
	0xc4,0x6c,0x92,0x3d,0xa5,0x33,0x89,0x22,0x1a,0x00,0x54,0xc1,0x1c,0x1e,0x3c,0xa3,
	0x1d,0x59,0x03,0x5d,0x2b,0x11,0x92,0xdf,0xba,0x13,0x4e,0x10,0xe5,0x40,0x87,0x5d,
	0x36,0x6e,0xbc,0x8b,0xc3,0x53,0xd5,0xaa,0x76,0x6b,0x80,0xc0,0x90,0xb3,0x9c,0x3a,
	0x5d,0x88,0x5d,0x02,0x90,0x53,0x52,0x1d,0x6e,0xa7,0xa5,0x2c,0xdd,0x55,0xf7,0x33,
	0xd0,0xfb,0x2d,0x07,0x7c,0x03,0x73,0xb0,0x05,0x3b,0x5b,0x81,0x0d,0x92,0x72,0x44,
	0x06,0x1b,0x75,0x73,0x02,0xd6,0x06,0x3d,0x02,0x26,0x91,0xb2,0x49,0x0a,0xb4,0x54,
	0xde,0xe7,0x3a,0x57,0xc6,0xff,0x5d,0x30,0x83,0x52,0xb4,0x61,0xec,0xe6,0x9f,0x3c,
	0x28,0x4f,0x2c,0x24,0x12,0x00,0x00,0x00,0x0a,0x91,0x11,0x83,0xf6,0x00,0x00,0x00,
	0x00,0x10,0x05,0x00,0x00,0x00,0x00,0x00,0x0f,0x42,0x40,0x80,0x00,0x00,0x8a,0xc5,
	0x33,0xff,0x38,0x65,0x55,0xb3,0x60,0x01,0x02,0x2f,0xd9,0x23,0x60,0x1e,0x1c,0xa0,
	0xac,0xe5,0x06,0x8c,0xe4,0x8e,0x14,0xf3,0xcd,0x31,0x44,0x16,0xc4,0x0d,0x2e,0x14,
	0x8c,0xa1,0xc8,0x4f,0xa6,0xa8,0xe4,0x64,0x9b,0x45,0x79,0xd1,0xb5,0x2f,0x04,0x19,
	0x86,0xe5,0x5c,0x99,0x43,0xf1,0xd0,0xf3,0x6f,0x52,0xd6,0x88,0xf0,0x9b,0x9c,0x58,
	0x98,0x69,0x0d,0x4e,0x76,0x3e,0xbd,0x6e,0x95,0x06,0x22,0x6e,0x46,0x11,0x1a,0x0b,
	0x59,0xca,0xaf,0x12,0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,0x33,0x2a,
	0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x67,0x00,0x00,0x01,0x00,
	0x00,0x65,0x55,0xb3,0x60,0x01,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x3b,0x02,0x33,
	0x80,0x00,0x00,0x00,0x95,0x97,0xd7,0xa4,0x39,0x65,0x55,0xb3,0x65,0x01,0x01,0x36,
	0x59,0x20,0x77,0x0e,0xf4,0x73,0x10,0xd7,0xb6,0x59,0x5a,0x7c,0xbe,0xd0,0x56,0x51,
	0x3d,0x97,0xbe,0x84,0xb7,0x02,0xb5,0x89,0x72,0xbd,0xb3,0x19,0x2a,0x54,0x5f,0x57,
	0x52,0x09,0x1d,0xff,0x0f,0xe6,0x70,0x1e,0x71,0x23,0xaa,0x3f,0x98,0x89,0x86,0x35,
	0x87,0xc2,0x66,0xd9,0x99,0xbd,0xa2,0x7e,0x16,0x12,0xd9,0x6b,0xc4,0xd3,0x0c,0x00,
	0x07,0x88,0xa0,0x00,0x0a,0x0a,0x69,0xa2,0x65,0x55,0xb3,0x65,0x02,0x2d,0x22,0x36,
	0x20,0xa3,0x59,0xa4,0x7f,0xf7,0xf7,0xac,0x44,0x7c,0x85,0xc4,0x6c,0x92,0x3d,0xa5,
	0x33,0x89,0x22,0x1a,0x00,0x54,0xc1,0x1c,0x1e,0x3c,0xa3,0x1d,0x59,0x02,0x2d,0x22,
	0x53,0x49,0x4c,0x45,0x4e,0x54,0x41,0x52,0x54,0x49,0x53,0x54,0x2d,0x31,0x2d,0x31,
	0x2d,0x67,0x63,0x63,0x65,0x34,0x66,0x36,0x38,0x2d,0x6d,0x6f,0x64,0x64,0x65,0x64,
	0x00,0x00,0x80,0x00,0x00,0x8a,0xd0,0xde,0x20,0xc6,0x65,0x55,0xb3,0x60,0x01,0x02,
	0x11,0xd6,0x4b,0xc5,0x88,0x33,0x4b,0x6c,0x1e,0x0c,0x4c,0x5c,0x65,0x69,0x35,0x66,
	0x45,0x80,0xbc,0x4b,0x56,0x3d,0x98,0x2c,0xb3,0x45,0xef,0x22,0x44,0x6e,0xca,0x6c,
	0x1f,0xe9,0x2a,0xf2,0x4a,0xe1,0x26,0xdb,0x15,0x6f,0x7e,0x3b,0xdd,0xd3,0x40,0x54,
	0xb2,0xc7,0x07,0xfe,0x67,0xb0,0xf3,0x35,0x56,0x25,0xc6,0x53,0xfd,0x54,0x56,0x5e,
	0x06,0x22,0x6e,0x46,0x11,0x1a,0x0b,0x59,0xca,0xaf,0x12,0x60,0x43,0xeb,0x5b,0xbf,
	0x28,0xc3,0x4f,0x3a,0x5e,0x33,0x2a,0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,0x88,0x91,0x0f,
	0x00,0x00,0x67,0x00,0x00,0x01,0x00,0x00,0x65,0x55,0xb3,0x60,0x01,0x01,0x00,0x06,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x0a,
	0x00,0x00,0x00,0x00,0x3b,0x02,0x33,0x80,0x00,0x00,0x00,0x95,0x87,0xf8,0x92,0xf3,
	0x65,0x55,0xb3,0x65,0x01,0x01,0x32,0x6c,0xde,0x4a,0xc5,0xe8,0xd4,0xa0,0xed,0xbc,
	0x2c,0x13,0x5e,0xb9,0x1e,0xc3,0xc3,0x86,0xcb,0x75,0xeb,0x6f,0xce,0xc5,0xc5,0x57,
	0x01,0x1c,0x9c,0xb7,0x32,0x17,0x01,0x10,0x8c,0xdd,0x04,0x31,0x78,0xae,0xb4,0x88,
	0x8d,0xf8,0xe8,0x35,0x90,0x69,0x91,0x84,0xd0,0x16,0xd8,0x44,0xbc,0xde,0x37,0xe8,
	0x4e,0x1a,0x95,0xb6,0xa6,0x73,0x00,0x07,0x88,0xa0,0x00,0x0a,0x0a,0x69,0xa2,0x65,
	0x55,0xb3,0x65,0x03,0x5d,0x2b,0x11,0x92,0xdf,0xba,0x13,0x4e,0x10,0xe5,0x40,0x87,
	0x5d,0x36,0x6e,0xbc,0x8b,0xc3,0x53,0xd5,0xaa,0x76,0x6b,0x80,0xc0,0x90,0xb3,0x9c,
	0x3a,0x5d,0x88,0x5d,0x03,0x5d,0x2b,0x48,0x4f,0x50,0x50,0x49,0x4e,0x47,0x46,0x49,
	0x52,0x45,0x2d,0x63,0x31,0x2d,0x31,0x2d,0x67,0x63,0x63,0x65,0x34,0x66,0x36,0x38,
	0x2d,0x6d,0x6f,0x64,0x64,0x65,0x64,0x00,0x00,0x00,0x00,0x00,0x8a,0xcb,0xd4,0xc7,
	0xa1,0x65,0x55,0xb3,0x65,0x01,0x02,0x1d,0x3f,0x42,0x7f,0x3d,0xdb,0x58,0x2b,0xcb,
	0x78,0x5d,0x24,0xf1,0x67,0xc6,0xc7,0xd8,0x6e,0x6c,0x5b,0xf8,0xfb,0x27,0x17,0x58,
	0xaa,0x7e,0x46,0x86,0x49,0x66,0x21,0x47,0x76,0xbd,0xf2,0x2d,0xae,0x29,0xf0,0x6f,
	0x17,0x6e,0xf2,0x7f,0x01,0xda,0x16,0xa9,0x1d,0x6a,0x61,0x4c,0x41,0x71,0x19,0x4d,
	0x37,0xac,0x8a,0x28,0xd8,0x62,0xfb,0x06,0x22,0x6e,0x46,0x11,0x1a,0x0b,0x59,0xca,
	0xaf,0x12,0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,0x33,0x2a,0x1f,0xc7,
	0xb2,0xb7,0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x67,0x00,0x00,0x01,0x00,0x00,0x65,
	0x55,0xb3,0x65,0x01,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x14,0x00,0x00,0x03,0xe8,0x00,0x00,0x00,0x00,0x3b,0x02,0x33,0x80,0x80,
	0x00,0x01,0xbc,0x39,0xdb,0xc7,0xdf,0x00,0x00,0x00,0x00,0x10,0x08,0x00,0x00,0x00,
	0x00,0x00,0x0f,0x42,0x40,0x01,0xb0,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x22,0x6e,0x46,0x11,
	0x1a,0x0b,0x59,0xca,0xaf,0x12,0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,
	0x33,0x2a,0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x6e,0x00,0x00,
	0x01,0x00,0x00,0x02,0x2d,0x22,0x36,0x20,0xa3,0x59,0xa4,0x7f,0xf7,0xf7,0xac,0x44,
	0x7c,0x85,0xc4,0x6c,0x92,0x3d,0xa5,0x33,0x89,0x22,0x1a,0x00,0x54,0xc1,0x1c,0x1e,
	0x3c,0xa3,0x1d,0x59,0x02,0x66,0xe4,0x59,0x8d,0x1d,0x3c,0x41,0x5f,0x57,0x2a,0x84,
	0x88,0x83,0x0b,0x60,0xf7,0xe7,0x44,0xed,0x92,0x35,0xeb,0x0b,0x1b,0xa9,0x32,0x83,
	0xb3,0x15,0xc0,0x35,0x18,0x03,0x1b,0x84,0xc5,0x56,0x7b,0x12,0x64,0x40,0x99,0x5d,
	0x3e,0xd5,0xaa,0xba,0x05,0x65,0xd7,0x1e,0x18,0x34,0x60,0x48,0x19,0xff,0x9c,0x17,
	0xf5,0xe9,0xd5,0xdd,0x07,0x8f,0x03,0x1b,0x84,0xc5,0x56,0x7b,0x12,0x64,0x40,0x99,
	0x5d,0x3e,0xd5,0xaa,0xba,0x05,0x65,0xd7,0x1e,0x18,0x34,0x60,0x48,0x19,0xff,0x9c,
	0x17,0xf5,0xe9,0xd5,0xdd,0x07,0x8f,0x80,0x00,0x00,0x8e,0x85,0x67,0x9b,0xcf,0x00,
	0x00,0x00,0x00,0x10,0x06,0x00,0x8a,0x01,0x02,0x27,0x9e,0x4e,0x43,0x39,0xa6,0x92,
	0x19,0x35,0x50,0x19,0xbb,0x51,0x5b,0xf9,0xac,0xdb,0xda,0x9c,0xde,0x81,0x8b,0x56,
	0x2e,0x0a,0x3d,0xd5,0xb0,0x5f,0x10,0x2e,0x6c,0x22,0x55,0x7e,0x07,0xc2,0x5f,0x4b,
	0x9c,0xc1,0x21,0x6e,0x07,0x66,0x41,0x60,0xde,0x3e,0xe2,0x24,0xa5,0x9e,0xec,0xaf,
	0xd7,0xcc,0x3f,0x87,0x7c,0x32,0x29,0xca,0xe7,0x06,0x22,0x6e,0x46,0x11,0x1a,0x0b,
	0x59,0xca,0xaf,0x12,0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,0x33,0x2a,
	0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x6e,0x00,0x00,0x01,0x00,
	0x00,0x65,0x55,0xb3,0x6c,0x01,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x3b,0x02,0x33,
	0x80,0x80,0x00,0x00,0x8e,0xca,0xa4,0x8e,0x70,0x00,0x00,0x00,0x00,0x10,0x06,0x00,
	0x8a,0x01,0x02,0x4e,0x3f,0x3b,0x6d,0xda,0xcc,0xd3,0xef,0x5f,0xaf,0x26,0x76,0x16,
	0x64,0xa2,0x82,0x97,0xe8,0xb4,0xe4,0xb1,0x2d,0xec,0xa1,0x9e,0x91,0x69,0xd3,0xde,
	0xe9,0x58,0xc7,0x19,0x06,0x90,0x42,0x86,0x97,0xf3,0x88,0xca,0x35,0xd5,0xec,0x79,
	0x5e,0x59,0x33,0x31,0xf4,0x0c,0xdb,0x55,0x5d,0x78,0xd7,0x22,0x59,0xa2,0xe5,0x8d,
	0xeb,0x65,0x41,0x06,0x22,0x6e,0x46,0x11,0x1a,0x0b,0x59,0xca,0xaf,0x12,0x60,0x43,
	0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,0x33,0x2a,0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,
	0x88,0x91,0x0f,0x00,0x00,0x6e,0x00,0x00,0x01,0x00,0x00,0x65,0x55,0xb3,0x6c,0x01,
	0x01,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,
	0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x3b,0x02,0x33,0x80,0x00,0x00,0x00,0x8a,0xbf,
	0x0d,0x7a,0xbc,0x65,0x55,0xb3,0x6f,0x01,0x02,0x59,0xc9,0x36,0x84,0xaa,0x13,0x70,
	0x73,0x7d,0xa7,0xe6,0x8a,0x72,0x05,0x15,0xa9,0x06,0x69,0xe0,0xc0,0x0d,0x15,0x6d,
	0x22,0x5c,0xd6,0x1a,0x3e,0x56,0xaa,0x9d,0x98,0x68,0x5a,0x3c,0xdc,0x38,0x71,0xee,
	0x0d,0x02,0x3e,0x2f,0xd0,0x97,0xd7,0xab,0xe2,0x0d,0xdf,0xf5,0xa6,0xd1,0x9c,0xb6,
	0xb9,0x30,0xd6,0xdd,0x10,0xa7,0xa3,0xce,0x5e,0x06,0x22,0x6e,0x46,0x11,0x1a,0x0b,
	0x59,0xca,0xaf,0x12,0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,0x33,0x2a,
	0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x67,0x00,0x00,0x01,0x00,
	0x00,0x65,0x55,0xb3,0x6f,0x01,0x01,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x14,0x00,0x00,0x03,0xe8,0x00,0x00,0x00,0x00,0x3b,0x02,0x33,
	0x80,0x80,0x00,0x00,0x8e,0x80,0x00,0x75,0xd3,0x00,0x00,0x00,0x00,0x10,0x06,0x00,
	0x8a,0x01,0x02,0x26,0x62,0x25,0x68,0x2d,0x27,0x06,0x9a,0x3b,0xaa,0x1a,0x43,0x93,
	0xac,0xb5,0x05,0x14,0x1c,0x09,0x22,0x8d,0xfb,0x54,0x43,0x73,0x3a,0x88,0xaa,0xa3,
	0x18,0xc8,0xd8,0x0d,0x36,0xd4,0x26,0x10,0xe9,0x82,0xa6,0x3c,0xb4,0x38,0x24,0xaf,
	0xdc,0x06,0xd5,0x3d,0xe3,0x7a,0xe8,0x39,0xdd,0xd4,0x5f,0xd8,0x92,0xf5,0x4b,0xe7,
	0x8d,0xf7,0xab,0x06,0x22,0x6e,0x46,0x11,0x1a,0x0b,0x59,0xca,0xaf,0x12,0x60,0x43,
	0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,0x33,0x2a,0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,
	0x88,0x91,0x0f,0x00,0x00,0x6e,0x00,0x00,0x01,0x00,0x00,0x65,0x55,0xb3,0x71,0x01,
	0x01,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x00,
	0x00,0x03,0xe8,0x00,0x00,0x00,0x00,0x3b,0x02,0x33,0x80,0x80,0x00,0x00,0x8e,0xdc,
	0x5e,0x38,0x7a,0x00,0x00,0x00,0x00,0x10,0x06,0x00,0x8a,0x01,0x02,0x5d,0xf9,0x14,
	0xa0,0xd1,0x46,0x69,0x9a,0x2a,0x97,0xf7,0xb3,0x83,0xf0,0x30,0x73,0x48,0x5e,0x7b,
	0x3e,0x6c,0x45,0xd3,0xf1,0x51,0xf8,0xbc,0x4b,0xd7,0xae,0x02,0xd9,0x57,0x51,0x73,
	0x8f,0x0f,0xd0,0xdd,0x38,0x36,0x48,0xcd,0xda,0xea,0x01,0x24,0x16,0x3d,0x14,0x7e,
	0x5a,0x9e,0xac,0xcd,0x81,0xd1,0x96,0x30,0xd9,0xbc,0xa6,0xe4,0xe8,0x06,0x22,0x6e,
	0x46,0x11,0x1a,0x0b,0x59,0xca,0xaf,0x12,0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,
	0x3a,0x5e,0x33,0x2a,0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x6e,
	0x00,0x00,0x01,0x00,0x00,0x65,0x55,0xb3,0x71,0x01,0x00,0x00,0x06,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x00,0x03,0xe8,0x00,0x00,0x00,
	0x00,0x3b,0x02,0x33,0x80,0x00,0x00,0x01,0xb0,0xb8,0x17,0x71,0xe9,0x65,0x55,0xb3,
	0x71,0x01,0x00,0x0d,0xef,0xc0,0x12,0x4f,0xd4,0xe4,0x1c,0xce,0xc3,0xf4,0x83,0x17,
	0x21,0x7e,0x33,0x11,0x5a,0x8f,0x8c,0x39,0xae,0xbb,0x06,0x2b,0x4e,0x73,0xa6,0x52,
	0xee,0x0e,0x61,0x03,0xf9,0xf1,0x16,0x7b,0x22,0xbd,0xcb,0x92,0xcf,0x82,0x2d,0x0f,
	0xcc,0x26,0xcb,0x11,0x9f,0x0a,0xf5,0x23,0xf4,0x26,0x97,0x86,0x7e,0x21,0x51,0x85,
	0x16,0x3a,0x0e,0x4b,0x1f,0x64,0xf3,0x62,0xf4,0x2f,0x13,0x54,0x5c,0x9a,0xed,0xd6,
	0x9d,0x5d,0xf5,0x17,0x85,0xcc,0x5d,0x53,0xa4,0x84,0x36,0x9b,0xe9,0xa7,0x4a,0x04,
	0x1f,0x8c,0x1c,0x28,0xc7,0x1e,0xab,0xb3,0xf2,0x53,0x3d,0xf8,0xb6,0xcd,0xd8,0x45,
	0x6e,0xc3,0x77,0xdb,0xb1,0x2b,0xa5,0xdd,0xc9,0xbf,0x69,0xa7,0xdf,0x98,0x2c,0x54,
	0xa7,0xa8,0xf5,0x4f,0xc3,0xd1,0x57,0x81,0x98,0x60,0xa0,0xb6,0x32,0x83,0x08,0x65,
	0x1e,0x79,0x24,0xf4,0xbe,0xfe,0x14,0xa6,0xfd,0x31,0x9b,0xfa,0x15,0x6f,0x1b,0xab,
	0x64,0x8b,0x69,0x48,0xb4,0x2e,0x99,0xca,0xd7,0x46,0x97,0x57,0x17,0x1d,0x5d,0x53,
	0xe5,0xde,0xae,0xbb,0x2d,0x74,0x86,0xa5,0xe5,0x8e,0x02,0x5c,0x2c,0x8d,0x0d,0x78,
	0xdc,0xf1,0xa0,0x05,0xb8,0xbd,0x64,0x7d,0x51,0x47,0x5e,0x39,0xaa,0x08,0x2d,0xf2,
	0xd0,0x1a,0x96,0x59,0xab,0x88,0x94,0xa2,0xf7,0x2e,0x09,0x3b,0xd0,0xed,0xfc,0xb4,
	0x1b,0xaa,0xba,0x34,0xf8,0x7b,0x11,0x51,0x36,0x7b,0x13,0x84,0x6f,0xc9,0x4a,0x58,
	0x92,0x3c,0x0c,0xa2,0x4c,0x8f,0xc5,0x78,0xb4,0x4a,0xad,0x98,0x08,0x4e,0x4a,0xf3,
	0x7a,0x28,0x80,0x00,0x00,0x06,0x22,0x6e,0x46,0x11,0x1a,0x0b,0x59,0xca,0xaf,0x12,
	0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,0x33,0x2a,0x1f,0xc7,0xb2,0xb7,
	0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x6e,0x00,0x00,0x01,0x00,0x00,0x02,0x2d,0x22,
	0x36,0x20,0xa3,0x59,0xa4,0x7f,0xf7,0xf7,0xac,0x44,0x7c,0x85,0xc4,0x6c,0x92,0x3d,
	0xa5,0x33,0x89,0x22,0x1a,0x00,0x54,0xc1,0x1c,0x1e,0x3c,0xa3,0x1d,0x59,0x02,0x66,
	0xe4,0x59,0x8d,0x1d,0x3c,0x41,0x5f,0x57,0x2a,0x84,0x88,0x83,0x0b,0x60,0xf7,0xe7,
	0x44,0xed,0x92,0x35,0xeb,0x0b,0x1b,0xa9,0x32,0x83,0xb3,0x15,0xc0,0x35,0x18,0x03,
	0xbb,0xee,0x60,0xc3,0x95,0x05,0x6b,0x8a,0x12,0x01,0xe0,0x6e,0xd7,0x9e,0x29,0x14,
	0xc1,0x1a,0x61,0xd7,0xb1,0xaa,0x78,0x18,0x46,0x46,0x8d,0x02,0x48,0x9d,0xba,0x69,
	0x02,0x32,0x42,0x66,0xde,0x84,0x03,0xb3,0xab,0x15,0x7a,0x09,0xf1,0xf7,0x84,0xd5,
	0x87,0xaf,0x61,0x83,0x1c,0x99,0x8c,0x15,0x1b,0xcc,0x21,0xbb,0x74,0xc2,0xb2,0x31,
	0x4b,0x00,0x00,0x00,0x0a,0x91,0x11,0x83,0xf6,0x00,0x00,0x00,0x00,0x10,0x05,0x00,
	0x00,0x00,0x00,0x00,0x0f,0x42,0x40,0x00,0x00,0x00,0x8a,0xe1,0x31,0x6b,0x76,0x65,
	0x55,0xb3,0x71,0x01,0x02,0x5d,0xf9,0x14,0xa0,0xd1,0x46,0x69,0x9a,0x2a,0x97,0xf7,
	0xb3,0x83,0xf0,0x30,0x73,0x48,0x5e,0x7b,0x3e,0x6c,0x45,0xd3,0xf1,0x51,0xf8,0xbc,
	0x4b,0xd7,0xae,0x02,0xd9,0x57,0x51,0x73,0x8f,0x0f,0xd0,0xdd,0x38,0x36,0x48,0xcd,
	0xda,0xea,0x01,0x24,0x16,0x3d,0x14,0x7e,0x5a,0x9e,0xac,0xcd,0x81,0xd1,0x96,0x30,
	0xd9,0xbc,0xa6,0xe4,0xe8,0x06,0x22,0x6e,0x46,0x11,0x1a,0x0b,0x59,0xca,0xaf,0x12,
	0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,0x33,0x2a,0x1f,0xc7,0xb2,0xb7,
	0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x6e,0x00,0x00,0x01,0x00,0x00,0x65,0x55,0xb3,
	0x71,0x01,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x14,0x00,0x00,0x03,0xe8,0x00,0x00,0x00,0x00,0x3b,0x02,0x33,0x80,0x00,0x00,0x00,
	0x8a,0xbd,0x6f,0x26,0xdf,0x65,0x55,0xb3,0x71,0x01,0x02,0x26,0x62,0x25,0x68,0x2d,
	0x27,0x06,0x9a,0x3b,0xaa,0x1a,0x43,0x93,0xac,0xb5,0x05,0x14,0x1c,0x09,0x22,0x8d,
	0xfb,0x54,0x43,0x73,0x3a,0x88,0xaa,0xa3,0x18,0xc8,0xd8,0x0d,0x36,0xd4,0x26,0x10,
	0xe9,0x82,0xa6,0x3c,0xb4,0x38,0x24,0xaf,0xdc,0x06,0xd5,0x3d,0xe3,0x7a,0xe8,0x39,
	0xdd,0xd4,0x5f,0xd8,0x92,0xf5,0x4b,0xe7,0x8d,0xf7,0xab,0x06,0x22,0x6e,0x46,0x11,
	0x1a,0x0b,0x59,0xca,0xaf,0x12,0x60,0x43,0xeb,0x5b,0xbf,0x28,0xc3,0x4f,0x3a,0x5e,
	0x33,0x2a,0x1f,0xc7,0xb2,0xb7,0x3c,0xf1,0x88,0x91,0x0f,0x00,0x00,0x6e,0x00,0x00,
	0x01,0x00,0x00,0x65,0x55,0xb3,0x71,0x01,0x01,0x00,0x06,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x00,0x03,0xe8,0x00,0x00,0x00,0x00,0x3b,
	0x02,0x33,0x80,0x00,0x00,0x00,0x95,0xf6,0xd7,0x24,0x69,0x65,0x55,0xb3,0x79,0x01,
	0x01,0x42,0x62,0x2b,0xf0,0x0a,0x44,0xc3,0xe0,0x78,0x3c,0xa7,0x08,0x5a,0xa0,0xaa,
	0x9d,0xb2,0xdc,0x2e,0xa7,0x29,0x0a,0x39,0x3d,0x01,0x57,0xbf,0x34,0x23,0x0f,0xb9,
	0x19,0x33,0x6f,0xc2,0xfb,0x9f,0x25,0xac,0xc4,0xc9,0x47,0xb7,0x41,0x92,0xdb,0x2f,
	0xd5,0xf0,0x1f,0x24,0x32,0xdf,0x38,0x30,0x89,0x93,0xeb,0xc9,0xec,0xf1,0x09,0x51,
	0xbb,0x00,0x07,0x88,0xa0,0x00,0x0a,0x0a,0x69,0xa2,0x65,0x55,0xb3,0x79,0x02,0x66,
	0xe4,0x59,0x8d,0x1d,0x3c,0x41,0x5f,0x57,0x2a,0x84,0x88,0x83,0x0b,0x60,0xf7,0xe7,
	0x44,0xed,0x92,0x35,0xeb,0x0b,0x1b,0xa9,0x32,0x83,0xb3,0x15,0xc0,0x35,0x18,0x02,
	0x66,0xe4,0x4a,0x55,0x4e,0x49,0x4f,0x52,0x42,0x45,0x41,0x4d,0x2d,0x72,0x63,0x31,
	0x2d,0x31,0x2d,0x67,0x63,0x63,0x65,0x34,0x66,0x36,0x38,0x2d,0x6d,0x6f,0x64,0x64,
	0x65,0x64,0x00,0x00,
};

/* not_mcf sets NDEBUG, so assert() is useless */
#define ASSERT(x) do { if (!(x)) abort(); } while(0)

static const char *print_flows(
		const tal_t *ctx,
		const char *desc,
		const struct gossmap *gossmap,
		struct flow **flows)
{
	tal_t *this_ctx = tal(ctx,tal_t);
	char *buff = tal_fmt(ctx,"%s: %zu subflows\n", desc, tal_count(flows));
	for (size_t i = 0; i < tal_count(flows); i++) {
		struct amount_msat fee, delivered;
		tal_append_fmt(&buff,"   ");
		for (size_t j = 0; j < tal_count(flows[i]->path); j++) {
			struct short_channel_id scid
				= gossmap_chan_scid(gossmap,
						    flows[i]->path[j]);
			tal_append_fmt(&buff,"%s%s", j ? "->" : "",
			       fmt_short_channel_id(this_ctx, scid));
		}
		delivered = flows[i]->amount;
		if (!flow_fee(&fee, flows[i]))
			abort();
		tal_append_fmt(&buff," prob %.2f, %s delivered with fee %s\n",
		       flows[i]->success_prob,
		       fmt_amount_msat(this_ctx, delivered),
		       fmt_amount_msat(this_ctx, fee));
	}
	tal_free(this_ctx);
	return buff;
}

int main(int argc, char *argv[])
{
	int fd;
	char *gossfile;
	struct gossmap *gossmap;
	struct node_id l1, l2, l3;
	struct flow **flows;
	struct route **routes;
	struct short_channel_id scid12, scid23;
	struct sha256 payment_hash;
	struct amount_msat *amounts;
	char *errmsg;

	if (!hex_decode("0001020304050607080900010203040506070809000102030405060708090102",
			strlen("0001020304050607080900010203040506070809000102030405060708090102"),
			&payment_hash, sizeof(payment_hash)))
		abort();

	common_setup(argv[0]);

	fd = tmpdir_mkstemp(tmpctx, "run-not_mcf.XXXXXX", &gossfile);
	assert(write_all(fd, canned_map, sizeof(canned_map)));

	gossmap = gossmap_load(tmpctx, gossfile, NULL);
	assert(gossmap);
	remove(gossfile);

	/* There is a public channel 2<->3 (103x1x0), and 1<->2 (110x1x1). */
	assert(node_id_from_hexstr("0266e4598d1d3c415f572a8488830b60f7e744ed9235eb0b1ba93283b315c03518", 66, &l1));
	assert(node_id_from_hexstr("022d223620a359a47ff7f7ac447c85c46c923da53389221a0054c11c1e3ca31d59", 66, &l2));
	assert(node_id_from_hexstr("035d2b1192dfba134e10e540875d366ebc8bc353d5aa766b80c090b39c3a5d885d", 66, &l3));
	assert(short_channel_id_from_str("110x1x0", 7, &scid12));
	assert(short_channel_id_from_str("103x1x0", 7, &scid23));

	struct uncertainty *uncertainty = uncertainty_new(tmpctx);
	int skipped_count = uncertainty_update(uncertainty, gossmap);
	assert(skipped_count == 0);

	bitmap *disabled =
	    tal_arrz(tmpctx, bitmap, BITMAP_NWORDS(gossmap_max_chan_idx(gossmap)));

	printf("All set, now let's call minflow ...\n");

	flows = minflow(tmpctx, gossmap,
			gossmap_find_node(gossmap, &l1),
			gossmap_find_node(gossmap, &l3),
			uncertainty_get_chan_extra_map(uncertainty),
			disabled,
			/* Half the capacity */
			AMOUNT_MSAT(500000000),
			 /* max_fee = */ AMOUNT_MSAT(1000000), // 1k sats
			 /* min probability = */ 0.1,
			 /* delay fee factor = */ 1,
			 /* base fee penalty */ 1,
			 /* prob cost factor = */ 10,
			 &errmsg);
	printf("minflow completed.\n");

	if (!flows) {
		printf("Minflow has failed with: %s", errmsg);
		assert(0 && "minflow failed");
	}

	routes = flows_to_routes(tmpctx, 1, 1, payment_hash, 10, gossmap, flows);
	assert(routes);
	for (size_t i = 0; i < tal_count(routes); i++) {
		uncertainty_commit_htlcs(uncertainty, routes[i]);
	}

	printf("%s\n",
	       print_flows(tmpctx,"Flow via single path l1->l2->l3", gossmap, flows));


	printf("Checking results.\n");
	/* Should go 1->2->3 */
	amounts = tal_flow_amounts(tmpctx, flows[0]);
	assert(amounts);
	assert(tal_count(flows) == 1);
	assert(tal_count(flows[0]->path) == 2);
	assert(tal_count(flows[0]->dirs) == 2);
	assert(tal_count(amounts) == 2);

	assert(flows[0]->path[0] == gossmap_find_chan(gossmap, &scid12));
	assert(flows[0]->path[1] == gossmap_find_chan(gossmap, &scid23));
	assert(flows[0]->dirs[0] == 1);
	assert(flows[0]->dirs[1] == 0);
	assert(amount_msat_eq(amounts[1], AMOUNT_MSAT(500000000)));
	/* fee_base_msat == 20, fee_proportional_millionths == 1000 */
	assert(amount_msat_eq(amounts[0], AMOUNT_MSAT(500000000 + 500000 + 20)));

	/* Each one has probability ~ 0.5 */
	assert(flows[0]->success_prob > 0.249);
	assert(flows[0]->success_prob <= 0.250);


	/* Should have filled in some extra data! */
	struct chan_extra *ce = uncertainty_find_channel(uncertainty, scid12);
	assert(short_channel_id_eq(ce->scid, scid12));
	/* l1->l2 dir is 1 */
	assert(ce->half[1].num_htlcs == 1);
	assert(amount_msat_eq(ce->half[1].htlc_total, AMOUNT_MSAT(500000000 + 500000 + 20)));
	assert(amount_msat_eq(ce->half[1].known_min, AMOUNT_MSAT(0)));
	assert(amount_msat_eq(ce->half[1].known_max, AMOUNT_MSAT(1000000000)));
	assert(ce->half[0].num_htlcs == 0);
	assert(amount_msat_eq(ce->half[0].htlc_total, AMOUNT_MSAT(0)));
	assert(amount_msat_eq(ce->half[0].known_min, AMOUNT_MSAT(0)));
	assert(amount_msat_eq(ce->half[0].known_max, AMOUNT_MSAT(1000000000)));

	ce = uncertainty_find_channel(uncertainty, scid23);
	assert(short_channel_id_eq(ce->scid, scid23));
	/* l2->l3 dir is 0 */
	assert(ce->half[0].num_htlcs == 1);
	assert(amount_msat_eq(ce->half[0].htlc_total, AMOUNT_MSAT(500000000)));
	assert(amount_msat_eq(ce->half[0].known_min, AMOUNT_MSAT(0)));
	assert(amount_msat_eq(ce->half[0].known_max, AMOUNT_MSAT(1000000000)));
	assert(ce->half[1].num_htlcs == 0);
	assert(amount_msat_eq(ce->half[1].htlc_total, AMOUNT_MSAT(0)));
	assert(amount_msat_eq(ce->half[1].known_min, AMOUNT_MSAT(0)));
	assert(amount_msat_eq(ce->half[1].known_max, AMOUNT_MSAT(1000000000)));

	/* Clear that */
	for (size_t i = 0; i < tal_count(routes); i++) {
		uncertainty_remove_htlcs(uncertainty, routes[i]);
	}

	// /* Now try adding a local channel scid */

	struct short_channel_id scid13;
	struct gossmap_localmods *mods = gossmap_localmods_new(tmpctx);
	assert(short_channel_id_from_str("111x1x1", 7, &scid13));

	/* 400,000sat channel from 1->3, basefee 0, ppm 1000, delay 5 */
	assert(gossmap_local_addchan(mods, &l1, &l3, scid13, NULL));
	assert(gossmap_local_updatechan(mods, scid13,
					AMOUNT_MSAT(0),
					AMOUNT_MSAT(400000000),
					0, 1000, 5,
					true,
					0));

	/* Apply changes, check they work. */
	gossmap_apply_localmods(gossmap, mods);
	struct gossmap_chan *local_chan = gossmap_find_chan(gossmap, &scid13);
	assert(local_chan);

	/* The local chans have no "capacity", so set it manually. */
	uncertainty_add_channel(uncertainty, scid13, AMOUNT_MSAT(400000000));

	// flows = minflow(tmpctx, gossmap,
	// 		gossmap_find_node(gossmap, &l1),
	// 		gossmap_find_node(gossmap, &l3),
	// 		chan_extra_map, NULL,
	// 		/* This will go first via 1-2-3, then 1->3. */
	// 		AMOUNT_MSAT(500000000),
	// 		 /* max_fee = */ AMOUNT_MSAT(1000000), // 1k sats
	// 		 /* min probability = */ 0.4,
	// 		 /* delay fee factor = */ 1,
	// 		 /* base fee penalty */ 1,
	// 		 /* prob cost factor = */ 10);

	// print_flows("Flow via two paths, low mu", gossmap, flows);

	// assert(tal_count(flows) == 2);
	//
	// if(tal_count(flows[0]->path)<tal_count(flows[1]->path))
	// {
	// 	struct flow* tmp = flows[0];
	// 	flows[0] = flows[1];
	// 	flows[1]=tmp;
	// }
	//
	// assert(tal_count(flows[0]->path) == 2);
	// assert(tal_count(flows[0]->dirs) == 2);
	// assert(tal_count(flows[0]->amounts) == 2);

	// assert(flows[0]->path[0] == gossmap_find_chan(gossmap, &scid12));
	// assert(flows[0]->path[1] == gossmap_find_chan(gossmap, &scid23));
	// assert(flows[0]->dirs[0] == 1);
	// assert(flows[0]->dirs[1] == 0);

	// /* First one has probability ~ 50% */
	// assert(flows[0]->success_prob < 0.55);
	// assert(flows[0]->success_prob > 0.45);

	// assert(tal_count(flows[1]->path) == 1);
	// assert(tal_count(flows[1]->dirs) == 1);
	// assert(tal_count(flows[1]->amounts) == 1);

	// /* We will try cheaper path first, but not to fill it! */
	// assert(flows[1]->path[0] == gossmap_find_chan(gossmap, &scid13));
	// assert(flows[1]->dirs[0] == 0);
	// assert(amount_msat_less(flows[1]->amounts[0], AMOUNT_MSAT(400000000)));

	// /* Second one has probability ~ 50% */
	// assert(flows[1]->success_prob < 0.55);
	// assert(flows[1]->success_prob > 0.45);

	// /* Delivered amount must be the total! */
	// assert(flows[0]->amounts[1].millisatoshis
	//        + flows[1]->amounts[0].millisatoshis == 500000000);

	// /* Clear them. */
	// remove_completed_flow(gossmap, chan_extra_map, flows[0]);
	// remove_completed_flow(gossmap, chan_extra_map, flows[1]);

	printf("All set, let's call minflow ... \n");

	/* Higher mu values mean we pay more for certainty! */
	struct flow **flows2 = minflow(tmpctx, gossmap,
			 gossmap_find_node(gossmap, &l1),
			 gossmap_find_node(gossmap, &l3),
			 uncertainty_get_chan_extra_map(uncertainty),
			 disabled,
			 /* This will go 400000000 via 1->3, rest via 1-2-3. */
			 /* amount = */ AMOUNT_MSAT(500000000), //500k sats
			 /* max_fee = */ AMOUNT_MSAT(1000000), // 1k sats
			 /* min probability = */ 0.1, // 10%
			 /* delay fee factor = */ 1,
			 /* base fee penalty */ 1,
			 /* prob cost factor = */ 10,
			 &errmsg);

	printf("minflow completed execution.\n");

	if (!flows) {
		printf("Minflow has failed with: %s", errmsg);
		assert(0 && "minflow failed");
	}
	printf("%s\n",
		print_flows(tmpctx,"Flow via two paths, high mu", gossmap, flows2));

	printf("Verifying results ... \n");

	assert(tal_count(flows2) == 2);

	/* The solution is composed by two paths, one with lenght 1 and the
	 * other with lenght 2. There is no guaranteed order of the solutions
	 * returning from minflow, hence we need to test them. */
	int ID1 = 0, ID2 = 1;
	if(tal_count(flows2[ID1]->path)==2)
	{
		swap(&ID1,&ID2);
	}
	assert(tal_count(flows2[ID1]->path) == 1);
	assert(tal_count(flows2[ID2]->path) == 2);

	// /* Sends more via 1->3, since it's more expensive (but lower prob) */
	assert(amount_msat_greater(flows2[ID1]->amount, flows2[ID2]->amount));
	assert(flows2[ID1]->success_prob < flows2[ID2]->success_prob);

	/* Delivered amount must be the total! */
	assert(flows2[ID1]->amount.millisatoshis
	       + flows2[ID2]->amount.millisatoshis == 500000000);

	common_shutdown();
}
