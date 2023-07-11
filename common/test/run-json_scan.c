#include "config.h"
#include "../json_parse.c"
#include "../json_parse_simple.c"
#include <common/amount.h>
#include <common/setup.h>
#include <wire/wire.h>

/* AUTOGENERATED MOCKS START */
/* Generated stub for amount_asset_is_main */
bool amount_asset_is_main(struct amount_asset *asset UNNEEDED)
{ fprintf(stderr, "amount_asset_is_main called!\n"); abort(); }
/* Generated stub for amount_asset_to_sat */
struct amount_sat amount_asset_to_sat(struct amount_asset *asset UNNEEDED)
{ fprintf(stderr, "amount_asset_to_sat called!\n"); abort(); }
/* Generated stub for amount_sat */
struct amount_sat amount_sat(u64 satoshis UNNEEDED)
{ fprintf(stderr, "amount_sat called!\n"); abort(); }
/* Generated stub for amount_sat_add */
 bool amount_sat_add(struct amount_sat *val UNNEEDED,
				       struct amount_sat a UNNEEDED,
				       struct amount_sat b UNNEEDED)
{ fprintf(stderr, "amount_sat_add called!\n"); abort(); }
/* Generated stub for amount_sat_div */
struct amount_sat amount_sat_div(struct amount_sat sat UNNEEDED, u64 div UNNEEDED)
{ fprintf(stderr, "amount_sat_div called!\n"); abort(); }
/* Generated stub for amount_sat_eq */
bool amount_sat_eq(struct amount_sat a UNNEEDED, struct amount_sat b UNNEEDED)
{ fprintf(stderr, "amount_sat_eq called!\n"); abort(); }
/* Generated stub for amount_sat_greater_eq */
bool amount_sat_greater_eq(struct amount_sat a UNNEEDED, struct amount_sat b UNNEEDED)
{ fprintf(stderr, "amount_sat_greater_eq called!\n"); abort(); }
/* Generated stub for amount_sat_mul */
bool amount_sat_mul(struct amount_sat *res UNNEEDED, struct amount_sat sat UNNEEDED, u64 mul UNNEEDED)
{ fprintf(stderr, "amount_sat_mul called!\n"); abort(); }
/* Generated stub for amount_sat_sub */
 bool amount_sat_sub(struct amount_sat *val UNNEEDED,
				       struct amount_sat a UNNEEDED,
				       struct amount_sat b UNNEEDED)
{ fprintf(stderr, "amount_sat_sub called!\n"); abort(); }
/* Generated stub for amount_sat_to_asset */
struct amount_asset amount_sat_to_asset(struct amount_sat *sat UNNEEDED, const u8 *asset UNNEEDED)
{ fprintf(stderr, "amount_sat_to_asset called!\n"); abort(); }
/* Generated stub for amount_tx_fee */
struct amount_sat amount_tx_fee(u32 fee_per_kw UNNEEDED, size_t weight UNNEEDED)
{ fprintf(stderr, "amount_tx_fee called!\n"); abort(); }
/* Generated stub for fromwire */
const u8 *fromwire(const u8 **cursor UNNEEDED, size_t *max UNNEEDED, void *copy UNNEEDED, size_t n UNNEEDED)
{ fprintf(stderr, "fromwire called!\n"); abort(); }
/* Generated stub for fromwire_bool */
bool fromwire_bool(const u8 **cursor UNNEEDED, size_t *max UNNEEDED)
{ fprintf(stderr, "fromwire_bool called!\n"); abort(); }
/* Generated stub for fromwire_fail */
void *fromwire_fail(const u8 **cursor UNNEEDED, size_t *max UNNEEDED)
{ fprintf(stderr, "fromwire_fail called!\n"); abort(); }
/* Generated stub for fromwire_secp256k1_ecdsa_signature */
void fromwire_secp256k1_ecdsa_signature(const u8 **cursor UNNEEDED, size_t *max UNNEEDED,
					secp256k1_ecdsa_signature *signature UNNEEDED)
{ fprintf(stderr, "fromwire_secp256k1_ecdsa_signature called!\n"); abort(); }
/* Generated stub for fromwire_sha256 */
void fromwire_sha256(const u8 **cursor UNNEEDED, size_t *max UNNEEDED, struct sha256 *sha256 UNNEEDED)
{ fprintf(stderr, "fromwire_sha256 called!\n"); abort(); }
/* Generated stub for fromwire_tal_arrn */
u8 *fromwire_tal_arrn(const tal_t *ctx UNNEEDED,
		       const u8 **cursor UNNEEDED, size_t *max UNNEEDED, size_t num UNNEEDED)
{ fprintf(stderr, "fromwire_tal_arrn called!\n"); abort(); }
/* Generated stub for fromwire_u32 */
u32 fromwire_u32(const u8 **cursor UNNEEDED, size_t *max UNNEEDED)
{ fprintf(stderr, "fromwire_u32 called!\n"); abort(); }
/* Generated stub for fromwire_u64 */
u64 fromwire_u64(const u8 **cursor UNNEEDED, size_t *max UNNEEDED)
{ fprintf(stderr, "fromwire_u64 called!\n"); abort(); }
/* Generated stub for fromwire_u8 */
u8 fromwire_u8(const u8 **cursor UNNEEDED, size_t *max UNNEEDED)
{ fprintf(stderr, "fromwire_u8 called!\n"); abort(); }
/* Generated stub for fromwire_u8_array */
void fromwire_u8_array(const u8 **cursor UNNEEDED, size_t *max UNNEEDED, u8 *arr UNNEEDED, size_t num UNNEEDED)
{ fprintf(stderr, "fromwire_u8_array called!\n"); abort(); }
/* Generated stub for mvt_tag_str */
const char *mvt_tag_str(enum mvt_tag tag UNNEEDED)
{ fprintf(stderr, "mvt_tag_str called!\n"); abort(); }
/* Generated stub for node_id_from_hexstr */
bool node_id_from_hexstr(const char *str UNNEEDED, size_t slen UNNEEDED, struct node_id *id UNNEEDED)
{ fprintf(stderr, "node_id_from_hexstr called!\n"); abort(); }
/* Generated stub for parse_amount_msat */
bool parse_amount_msat(struct amount_msat *msat UNNEEDED, const char *s UNNEEDED, size_t slen UNNEEDED)
{ fprintf(stderr, "parse_amount_msat called!\n"); abort(); }
/* Generated stub for parse_amount_sat */
bool parse_amount_sat(struct amount_sat *sat UNNEEDED, const char *s UNNEEDED, size_t slen UNNEEDED)
{ fprintf(stderr, "parse_amount_sat called!\n"); abort(); }
/* Generated stub for towire */
void towire(u8 **pptr UNNEEDED, const void *data UNNEEDED, size_t len UNNEEDED)
{ fprintf(stderr, "towire called!\n"); abort(); }
/* Generated stub for towire_bool */
void towire_bool(u8 **pptr UNNEEDED, bool v UNNEEDED)
{ fprintf(stderr, "towire_bool called!\n"); abort(); }
/* Generated stub for towire_secp256k1_ecdsa_signature */
void towire_secp256k1_ecdsa_signature(u8 **pptr UNNEEDED,
			      const secp256k1_ecdsa_signature *signature UNNEEDED)
{ fprintf(stderr, "towire_secp256k1_ecdsa_signature called!\n"); abort(); }
/* Generated stub for towire_sha256 */
void towire_sha256(u8 **pptr UNNEEDED, const struct sha256 *sha256 UNNEEDED)
{ fprintf(stderr, "towire_sha256 called!\n"); abort(); }
/* Generated stub for towire_u32 */
void towire_u32(u8 **pptr UNNEEDED, u32 v UNNEEDED)
{ fprintf(stderr, "towire_u32 called!\n"); abort(); }
/* Generated stub for towire_u64 */
void towire_u64(u8 **pptr UNNEEDED, u64 v UNNEEDED)
{ fprintf(stderr, "towire_u64 called!\n"); abort(); }
/* Generated stub for towire_u8 */
void towire_u8(u8 **pptr UNNEEDED, u8 v UNNEEDED)
{ fprintf(stderr, "towire_u8 called!\n"); abort(); }
/* Generated stub for towire_u8_array */
void towire_u8_array(u8 **pptr UNNEEDED, const u8 *arr UNNEEDED, size_t num UNNEEDED)
{ fprintf(stderr, "towire_u8_array called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

static bool json_to_tok(const char *buffer, const jsmntok_t *tok,
			const jsmntok_t **t)
{
	*t = tok;
	return true;
}

int main(int argc, char *argv[])
{
	const jsmntok_t *toks, *t;
	char *buf;
	char *s;
	u32 u32val;
	u8 *hex;
	const char *err;

	common_setup(argv[0]);

	buf = tal_strdup(tmpctx, "{\"1\":\"one\", \"2\":\"two\", \"3\":{\"three\": {\"deeper\": 17}}, \"arr\": [{\"1\": \"arrone\"}, 2, [3, 4]]}");
	toks = json_parse_simple(tmpctx, buf, strlen(buf));
	assert(toks);
	assert(toks->size == 4);

	/* These are direct matches, and they work. */
	assert(!json_scan(tmpctx, buf, toks, "{1:one}"));
	assert(!json_scan(tmpctx, buf, toks, "{1:one,2:two}"));
	assert(!json_scan(tmpctx, buf, toks, "{2:two,1:one}"));
	assert(!json_scan(tmpctx, buf, toks, "{2:two,1:one,3:{three:{deeper:17}}}"));
	assert(!json_scan(tmpctx, buf, toks, "{2:two,1:one,3:{three:{deeper:17}},arr:[0:{1:arrone}]}"));
	assert(!json_scan(tmpctx, buf, toks, "{2:two,1:one,3:{three:{deeper:17}},arr:[1:2]}"));
	assert(!json_scan(tmpctx, buf, toks, "{2:two,1:one,3:{three:{deeper:17}},arr:[1:2,2:[0:3,1:4]]}"));

	/* Optional fields which are present are fine. */
	assert(!json_scan(tmpctx, buf, toks, "{1?:one}"));
	assert(!json_scan(tmpctx, buf, toks, "{1?:one,2?:two}"));
	assert(!json_scan(tmpctx, buf, toks, "{2?:two,1?:one}"));
	assert(!json_scan(tmpctx, buf, toks, "{2?:two,1?:one,3?:{three?:{deeper?:17}}}"));
	assert(!json_scan(tmpctx, buf, toks, "{2?:two,1?:one,3?:{three?:{deeper?:17}},arr?:[0:{1?:arrone}]}"));
	assert(!json_scan(tmpctx, buf, toks, "{2?:two,1?:one,3?:{three?:{deeper?:17}},arr?:[1:2]}"));
	assert(!json_scan(tmpctx, buf, toks, "{2?:two,1?:one,3?:{three?:{deeper?:17}},arr?:[1:2,2:[0:3,1:4]]}"));

	/* Optional field which are missing are fine too */
	assert(!json_scan(tmpctx, buf, toks, "{5?:one}"));
	assert(!json_scan(tmpctx, buf, toks, "{1:one,5?:two}"));
	assert(!json_scan(tmpctx, buf, toks, "{2:two,5?:one}"));
	assert(!json_scan(tmpctx, buf, toks, "{2:two,1:one,5?:{three:{deeper:17}}}"));
	assert(!json_scan(tmpctx, buf, toks, "{2:two,1:one,3:{five?:{deeper:17}},arr:[0:{1:arrone}]}"));
	assert(!json_scan(tmpctx, buf, toks, "{2:two,1:one,3:{three:{notdeeper?:17}},arr:[1:2]}"));
	assert(!json_scan(tmpctx, buf, toks, "{2:two,1:one,3:{three:{deeper:17}},notarr?:[1:2,2:[0:3,1:4]]}"));

	/* These do not match */
	err = json_scan(tmpctx, buf, toks, "{2:one}");
	assert(streq(err, "Parsing '{2:one': \"two\" does not match expected one"));
	err = json_scan(tmpctx, buf, toks, "{1:one,2:tw}");
	assert(streq(err, "Parsing '{1:one,2:tw': \"two\" does not match expected tw"));
	err = json_scan(tmpctx, buf, toks, "{1:one,2:twoo}");
	assert(streq(err, "Parsing '{1:one,2:twoo': \"two\" does not match expected twoo"));
	err = json_scan(tmpctx, buf, toks, "{4:one}");
	assert(streq(err, "Parsing '{4:': object does not have member 4"));
	err = json_scan(tmpctx, buf, toks, "{2:two,1:one,3:three}");
	assert(streq(err, "Parsing '{2:two,1:one,3:three': {\"three\": {\"deeper\": 17}} does not match expected three"));
	err = json_scan(tmpctx, buf, toks, "{3:{three:deeper}}");
	assert(streq(err, "Parsing '{3:{three:deeper': {\"deeper\": 17} does not match expected deeper"));
	err = json_scan(tmpctx, buf, toks, "{3:{three:{deeper:{}}}}");
	assert(streq(err, "Parsing '{3:{three:{deeper:{': token is not an object: 17"));
	err = json_scan(tmpctx, buf, toks, "{arr:{}}");
	assert(streq(err, "Parsing '{arr:{': token is not an object: [{\"1\": \"arrone\"}, 2, [3, 4]]"));
	err = json_scan(tmpctx, buf, toks, "{arr:[0:1]}");
	assert(streq(err, "Parsing '{arr:[0:1': {\"1\": \"arrone\"} does not match expected 1"));
	err = json_scan(tmpctx, buf, toks, "{arr:[4:0]}");
	assert(streq(err, "Parsing '{arr:[4:': token has no index 4: [{\"1\": \"arrone\"}, 2, [3, 4]]"));
	err = json_scan(tmpctx, buf, toks, "{arr:[0:{1:arrtwo}]}");
	assert(streq(err, "Parsing '{arr:[0:{1:arrtwo': \"arrone\" does not match expected arrtwo"));
	err = json_scan(tmpctx, buf, toks, "{arr:[1:3]}");
	assert(streq(err, "Parsing '{arr:[1:3': 2 does not match expected 3"));
	err = json_scan(tmpctx, buf, toks, "{arr:[2:[0:1]]}");
	assert(streq(err, "Parsing '{arr:[2:[0:1': 3 does not match expected 1"));

	/* These capture simple values. */
	assert(!json_scan(tmpctx, buf, toks, "{3:{three:{deeper:%}}}",
			  JSON_SCAN(json_to_number, &u32val)));
	assert(u32val == 17);
	err = json_scan(tmpctx, buf, toks, "{1:%}",
			JSON_SCAN(json_to_number, &u32val));
	assert(streq(err, "Parsing '{1:%': json_to_number could not parse \"one\""));
	assert(!json_scan(tmpctx, buf, toks, "{1:%}",
			  JSON_SCAN_TAL(tmpctx, json_strdup, &s)));
	assert(tal_parent(s) == tmpctx);
	assert(streq(s, "one"));

	err = json_scan(tmpctx, buf, toks, "{1:%}",
			JSON_SCAN_TAL(tmpctx, json_tok_bin_from_hex, &hex));
	assert(streq(err, "Parsing '{1:%': json_tok_bin_from_hex could not parse \"one\""));

	assert(!json_scan(tmpctx, buf, toks, "{3:%}", JSON_SCAN(json_to_tok, &t)));
	assert(t == &toks[6]);

	assert(!json_scan(tmpctx, buf, toks, "{arr:%}", JSON_SCAN(json_to_tok, &t)));
	assert(t == &toks[12]);

	assert(!json_scan(tmpctx, buf, toks, "{arr:[1:%]}",
			  JSON_SCAN(json_to_number, &u32val)));
	assert(u32val == 2);

	assert(!json_scan(tmpctx, buf, toks, "{arr:[2:[1:%]]}",
			  JSON_SCAN(json_to_number, &u32val)));
	assert(u32val == 4);

	common_shutdown();
}
